# TTArchExt

A command-line tool for extracting and rebuilding Telltale Games archive files (.ttarch, .ttarch2), with added Oodle compression support for TTArch2 (zCTT) format.

Based on [ttarchext 0.3.2](https://aluigi.altervista.org/papers.htm#ttarchive) by Luigi Auriemma.

## Features

- Extract and rebuild TTArch (v1-v6) and TTArch2 (v7+) archives
- Oodle LZHLW compression/decompression for TTArch2 zCTT archives (`-z` flag)
- Blowfish encryption/decryption (per-game keys)
- Built-in meta extraction (DDS from D3DTX, OGG from AUD, etc.)
- Supports 68 Telltale Games with built-in keys
- Custom Blowfish key support (`-k` flag)

## Prerequisites

- **Compiler**: [LLVM MinGW64](https://github.com/mstorsjo/llvm-mingw) (UCRT)
- **Runtime**: `oo2core_5_win64.dll` (place in the same directory as the executable)

> `oo2core_5_win64.dll` is not included in this repository. Obtain it from your game installation.

## Building

```bash
# 64-bit (default)
make

# 32-bit
make ARCH=x86

# Clean
make clean
```

Requires LLVM MinGW64 at `D:/llvm_mingw64/llvm-mingw-20250812-ucrt-x86_64`. Edit `Makefile` to change the compiler path.

## Usage

### Extract files

```bash
ttarchext <gamenum> <archive.ttarch2> <output_folder>
```

### Rebuild archive (uncompressed, recommended)

```bash
ttarchext -b -V 7 <gamenum> <output.ttarch2> <input_folder>
```

### Rebuild archive (Oodle compressed)

```bash
ttarchext -b -z -V 7 <gamenum> <output.ttarch2> <input_folder>
```

### List files

```bash
ttarchext -l <gamenum> <archive.ttarch2>
```

> **Note**: Output file path comes **before** the input folder.

## Options

| Flag | Description |
|------|-------------|
| `-b` | Rebuild mode |
| `-z` | Use Oodle LZHLW compression (rebuild only) |
| `-V VER` | Force archive version (e.g., `-V 7`) |
| `-l` | List files without extracting |
| `-f W` | Filter files by wildcard (e.g., `-f "*.lua"`) |
| `-o` | Overwrite existing files |
| `-m` | Extract embedded meta (DDS, OGG, etc.) |
| `-k K` | Custom Blowfish key in hex |
| `-x` | Force old mode format |
| `-T F` | Dump decrypted name table |
| `-v` | Verbose output |

## Oodle Compression

Oodle compression is optional. For maximum compatibility, use uncompressed mode. The `-z` flag enables Oodle LZHLW compression (algorithm `0x00`, level 7) using `oo2core_5_win64.dll`.

| Mode | Compatibility | Archive Size |
|------|--------------|--------------|
| Uncompressed (no `-z`) | 100% | Larger |
| Oodle compressed (`-z`) | Game-dependent | ~15-20% of original |

## Supported Games

| ID | Game |
|----|------|
| 24-31 | Tales of Monkey Island |
| 52 | The Walking Dead: A New Day |
| 54 | The Wolf Among Us |
| 55 | The Walking Dead: Season 2 |
| 56 | Tales from the Borderlands |
| 57 | Game of Thrones |
| 58 | Minecraft: Story Mode |
| 60 | Batman: The Telltale Series |
| 61 | The Walking Dead: A New Frontier |
| 62 | Marvel's Guardians of the Galaxy |
| 63 | Minecraft: Story Mode - Season Two |
| 64 | Batman: The Enemy Within |
| 67 | The Walking Dead: The Telltale Definitive Series |

Run `ttarchext` without arguments for the full list of 68 supported games.

## Project Structure

```
ttarchext.c          - Main program (archive parsing, extraction, rebuilding)
blowfish_ttarch.c    - Blowfish encryption/decryption
oodle.c              - Oodle LZHLW compression interface
zlib/                - Bundled zlib library
Makefile             - LLVM MinGW64 build
```

## License

Original tool by Luigi Auriemma. Modifications for Oodle compression
