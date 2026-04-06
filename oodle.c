/*
by Luigi Auriemma
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define FREE(X) if(X) { free(X); X = NULL; }

// Type definition for 64-bit integer
typedef long long i64;

// Compress uses the enum value while Decompress uses the raw value stored in the header of the compressed file
// the algorithm number is NOT the same of the enum, that's why we need this
typedef struct {
    char    *name;
    int     algo_compress;
    int     algo_raw;       // 0x8c followed by this byte
} Oodle_algorithms_raw_t;

Oodle_algorithms_raw_t    Oodle_algorithms_raw[] = {
    { "LZH",            0,  7 },
    { "LZHLW",          1,  0 },
    { "LZNIB",          2,  1 },
    { "None",           3,  7 },    // 0x8c->0xcc
    { "LZB16",          4,  2 },
    { "LZBLW",          5,  3 },
    { "LZA",            6,  4 },
    { "LZNA",           7,  5 },
    { "Kraken",         8,  6 },
    { "Mermaid",        9, 10 },
    { "BitKnit",       10, 11 },
    { "Selkie",        11, 10 },
    { "Akkorokamui",   12,  6 },
    //
    { "LZQ1",           8,  6 },    // old name of Kraken
    { "LZNIB2",         9, 10 },    // old name of Mermaid
    //
    { NULL, -1, -1 }
};



#ifdef WIN32
#include <windows.h>

#ifndef stricmp
#define stricmp     _stricmp
#endif

// Function pointer matching QuickBMS signature (10 parameters for Compress):
// int __stdcall OodleLZ_Compress(int algo, void *in, int insz, void *out, int max, void *a, void *b, void *c, void *d, int e)
typedef int (__stdcall *OodleLZ_Compress_t)(int Format, void* Buffer, long long BufferSize, void* OutputBuffer, int Level, void* a, void* b, void* c, void* d, int e);
OodleLZ_Compress_t OodleLZ_Compress = NULL;
int (__stdcall *OodleLZ_Decompress)(unsigned char *in, int insz, unsigned char *out, int outsz, int a, int b, int c, void *d, void *e, void *f, void *g, void *h, void *i, int j) = NULL;   // Oodle 2.3.0
size_t (__stdcall *OodleLZ_GetDecodeBufferSize)(int format, const void *compressedBuffer, int compressedBufferSize) = NULL;
void* (__stdcall *OodlePlugins_SetAssertion)(void *func) = NULL;
int __stdcall oodle_noassert(const char * a,const int b,const char * c,const char * d) { return 0; }

// Forward declaration from ttarchext.c
extern void std_err(void);
#define myexit(x) std_err()
#define QUICKBMS_ERROR_DLL 0
#endif



int oodle_get_algo(char *name, int raw) {
    int     i;
    if(name) {
        for(i = 0; Oodle_algorithms_raw[i].name; i++) {
            if(!stricmp(name, Oodle_algorithms_raw[i].name)) {
                if(raw) return Oodle_algorithms_raw[i].algo_raw;
                else    return Oodle_algorithms_raw[i].algo_compress;
            }
        }
    }
    return -1;
}



int oodle_init(void) {
#ifdef WIN32
    static HMODULE hlib = NULL;
    static int initialized = 0;
    fprintf(stderr, "- oodle_init() called\n");
    fflush(stderr);
    if(!hlib) {
        // Try to load from known locations
        // Prioritize oo2core_8_win64.dll (newer version, potentially better compatibility)
        static const char *dll_paths[] = {
            "oo2core_8_win64.dll",
            "oo2core_7_win64.dll",
            "oo2core_6_win64.dll",
            "oo2core_5_win64.dll",
            "oo2core_5_win32.dll",
            "oo2core_4_win64.dll",
            "oo2core_4_win32.dll",
            "oodle_dll.dll",
            NULL
        };
        int i;

        for(i = 0; dll_paths[i]; i++) {
            hlib = LoadLibraryA(dll_paths[i]);
            if(hlib) {
                fprintf(stderr, "- loaded Oodle DLL: %s\n", dll_paths[i]);
                break;
            }
        }


        if(hlib) {
            // Try to get 10-parameter version of OodleLZ_Compress (40 bytes on x64: 10 * 4 bytes for stdcall)
            if(!OodleLZ_Compress)   OodleLZ_Compress   = (void *)GetProcAddress(hlib, "OodleLZ_Compress");
            // Note: On x64, stdcall decoration is not used, so @40 won't exist
            if(!OodleLZ_Compress)   OodleLZ_Compress   = (void *)GetProcAddress(hlib, "_OodleLZ_Compress@40");
            if(!OodleLZ_Decompress) OodleLZ_Decompress = (void *)GetProcAddress(hlib, "OodleLZ_Decompress");
            if(!OodleLZ_Decompress) OodleLZ_Decompress = (void *)GetProcAddress(hlib, "_OodleLZ_Decompress@56");
            // Try to load OodleLZ_GetDecodeBufferSize if available (for getting decompressed size)
            if(!OodleLZ_GetDecodeBufferSize) OodleLZ_GetDecodeBufferSize = (void *)GetProcAddress(hlib, "OodleLZ_GetDecodeBufferSize");

            fprintf(stderr, "- OodleLZ_Compress function pointer: %p\n", (void*)OodleLZ_Compress);
            fprintf(stderr, "- OodleLZ_Decompress function pointer: %p\n", (void*)OodleLZ_Decompress);
            fprintf(stderr, "- OodleLZ_GetDecodeBufferSize function pointer: %p\n", (void*)OodleLZ_GetDecodeBufferSize);
        }
        if(!hlib || !OodleLZ_Compress || !OodleLZ_Decompress) {
            fprintf(stderr, "\nError: unable to load the Oodle DLL and functions\n");
            fprintf(stderr, "Please place oo2core_5_win64.dll (or oo2core_5_win32.dll) in the same directory\n");
            myexit(QUICKBMS_ERROR_DLL);
        }

        // better to leave the asserts enabled for debug information
        //if(!OodlePlugins_SetAssertion) OodlePlugins_SetAssertion = (void *)GetProcAddress(hlib, "OodlePlugins_SetAssertion");
        //if(!OodlePlugins_SetAssertion) OodlePlugins_SetAssertion = (void *)GetProcAddress(hlib, "_OodlePlugins_SetAssertion@4");
        //if(OodlePlugins_SetAssertion) OodlePlugins_SetAssertion(oodle_noassert);
    }

    // IMPORTANT: Oodle DLL needs to be "initialized" by making at least one call
    // with stack-allocated memory. Otherwise, calls with malloc-allocated memory
    // will crash. This is a quirk of the Oodle library.
    if(!initialized) {
        unsigned char dummy_in[65536];
        unsigned char dummy_out[131072];
        memset(dummy_in, 'A', sizeof(dummy_in));
        fprintf(stderr, "- initializing Oodle DLL with dummy compression...\n");
        fflush(stderr);
        // Use 10-parameter call style from QuickBMS
        // Use level 3 for consistency with actual compression
        OodleLZ_Compress(1, dummy_in, 65536, dummy_out, 3, NULL, NULL, NULL, NULL, 0);
        fprintf(stderr, "- Oodle DLL initialized\n");
        fflush(stderr);
        initialized = 1;
    }

    return 0;
#else
    return -1;
#endif
}



int oodle_compress(unsigned char *in, int insz, unsigned char *out) {
#ifdef WIN32
    int compressed_size;
    // EXTERNAL: Algorithm can be set via environment variable OODLE_ALGO
    // Default: 1 = LZHLW, try 0 = LZH if LZHLW doesn't work
    static int algo = -1;

    if(algo < 0) {
        char *algo_env = getenv("OODLE_ALGO");
        if(algo_env) {
            algo = atoi(algo_env);
        } else {
            algo = 8;  // Kraken (raw value 0x06)
        }
    }

    // Use global function pointer initialized by oodle_init()
    if(!OodleLZ_Compress) {
        fprintf(stderr, "- ERROR: OodleLZ_Compress is NULL, call oodle_init() first!\n");
        return -1;
    }

    // Use level 7 to match original game compression level
    static int level = 7;
    compressed_size = OodleLZ_Compress(algo, in, (long long)insz, out, level, NULL, NULL, NULL, NULL, 0);

    fprintf(stderr, "- Oodle compress: algo=%d, level=%d, insz=%d -> compressed=%d\n",
            algo, level, insz, compressed_size);

    return compressed_size;
#else
    return -1;
#endif
}



int oodle_decompress(unsigned char *in, int insz, unsigned char *out, int outsz, char *algo_name) {
#ifdef WIN32
    i64 result = 0;

    // Check if input already has 0x8c prefix
    // OodleLZ_Compress returns data WITH 0x8c prefix
    if(insz >= 2 && in[0] == 0x8c) {
        // Method 1: Try using OodleLZ_GetDecodeBufferSize if available
        if(OodleLZ_GetDecodeBufferSize) {
            size_t needed = OodleLZ_GetDecodeBufferSize(1, in, insz);
            if(needed > 0 && needed <= (size_t)outsz) {
                outsz = (int)needed;
            }
        }

        // Method 2: If GetDecodeBufferSize not available or failed, use query trick
        // First call with outsz=0 to get the decompressed size (if supported)
        // or try with a large buffer and check return value
        result = OodleLZ_Decompress(in, insz, out, outsz, 1, 0, 1, NULL, 0, NULL, NULL, NULL, 0, 0);

        // If standard decompression failed with 0 return, the output buffer size might be wrong
        if(result == 0) {
            // Try querying the size first by calling with NULL output buffer
            i64 decoded_size = OodleLZ_Decompress(in, insz, NULL, 0, 1, 0, 1, NULL, 0, NULL, NULL, NULL, 0, 0);

            if(decoded_size > 0 && decoded_size <= outsz) {
                // Got the size, now decompress for real
                result = OodleLZ_Decompress(in, insz, out, (int)decoded_size, 1, 0, 1, NULL, 0, NULL, NULL, NULL, 0, 0);
            } else if(decoded_size > 0) {
                // Decoded size exceeds buffer - error
                fprintf(stderr, "- Oodle ERROR: decoded size %lld exceeds buffer %d\n", decoded_size, outsz);
                return -1;
            }
        }
    } else {
        // No 0x8c prefix - shouldn't happen with our compressed data
        fprintf(stderr, "- WARNING: Input data missing 0x8c prefix, adding...\n");
        unsigned char *p = malloc(2 + insz);
        if(!p) return -1;
        memcpy(p + 2, in, insz);
        p[0] = 0x8c;
        p[1] = 0;  // LZHLW

        i64 decoded_size = OodleLZ_Decompress(p, insz + 2, NULL, 0, 1, 0, 1, NULL, 0, NULL, NULL, NULL, 0, 0);
        if(decoded_size > 0 && decoded_size <= outsz) {
            result = OodleLZ_Decompress(p, insz + 2, out, (int)decoded_size, 1, 0, 1, NULL, 0, NULL, NULL, NULL, 0, 0);
        } else {
            result = OodleLZ_Decompress(p, insz + 2, out, outsz, 1, 0, 1, NULL, 0, NULL, NULL, NULL, 0, 0);
        }
        free(p);
    }
    return (int)result;
#else
    return -1;
#endif
}


