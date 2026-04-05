# Makefile for ttarchext
# Usage: make -f Makefile [all|clean|x64|x86]

# ============================================
# Configuration
# ============================================

# Compiler and flags
CC_x64   := D:/llvm_mingw64/llvm-mingw-20250812-ucrt-x86_64/bin/x86_64-w64-mingw32-gcc.exe
CC_x86   := D:/llvm_mingw64/llvm-mingw-20250812-ucrt-x86_64/bin/i686-w64-mingw32-gcc.exe

CFLAGS   := -O2 -Wall
LDFLAGS  :=

# Directories
BUILD_DIR  := build
ZLIB_DIR  := zlib
ZLIB_INC  := $(ZLIB_DIR)

# Target
TARGET_X64 := ttarchext_x64.exe
TARGET_X86 := ttarchext_x86.exe
TARGET    := $(TARGET_X64)

# Build architecture (x64 or x86)
ARCH ?= x64

ifeq ($(ARCH),x86)
    CC := $(CC_x86)
    TARGET := $(TARGET_X86)
else
    CC := $(CC_x64)
    TARGET := $(TARGET_X64)
endif

# ============================================
# Source Files
# ============================================

# Main source files
TT_SOURCES := ttarchext.c blowfish_ttarch.c oodle.c

# Zlib source files
ZLIB_SOURCES := \
    $(ZLIB_DIR)/adler32.c \
    $(ZLIB_DIR)/compress.c \
    $(ZLIB_DIR)/crc32.c \
    $(ZLIB_DIR)/deflate.c \
    $(ZLIB_DIR)/infback.c \
    $(ZLIB_DIR)/inffast.c \
    $(ZLIB_DIR)/inflate.c \
    $(ZLIB_DIR)/inftrees.c \
    $(ZLIB_DIR)/trees.c \
    $(ZLIB_DIR)/uncompr.c \
    $(ZLIB_DIR)/zutil.c

# Object files
TT_OBJECTS   := $(TT_SOURCES:%.c=$(BUILD_DIR)/%.o)
ZLIB_OBJECTS := $(notdir $(ZLIB_SOURCES:.c=.o))
ZLIB_OBJECTS := $(ZLIB_OBJECTS:%.o=$(BUILD_DIR)/%.o)
OBJECTS      := $(TT_OBJECTS) $(ZLIB_OBJECTS)

# ============================================
# Build Rules
# ============================================

.PHONY: all all-x64 all-x86 clean check help

# Default target
all: $(TARGET)

# Build 64-bit version
all-x64: ARCH := x64
all-x64: $(TARGET_X64)

# Build 32-bit version
all-x86: ARCH := x86
all-x86: $(TARGET_X86)

# Link
$(TARGET_X64): $(OBJECTS)
	@echo "Linking $@ (64-bit)..."
	@$(CC_x64) -o $@ $^ $(LDFLAGS)
	@echo "Build complete: $@"
	@file $@

$(TARGET_X86): $(OBJECTS)
	@echo "Linking $@ (32-bit)..."
	@$(CC_x86) -o $@ $^ $(LDFLAGS)
	@echo "Build complete: $@"
	@file $@

# Compile main source files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@$(CC) -c $< -o $@ -I"$(ZLIB_INC)" $(CFLAGS)

# Compile zlib files (vpath for source files in different directory)
$(BUILD_DIR)/%.o: $(ZLIB_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@$(CC) -c $< -o $@ $(CFLAGS)

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# ============================================
# Utility Targets
# ============================================

clean:
	@echo "Cleaning build artifacts..."
	@-rm -rf $(BUILD_DIR) $(TARGET_X64) $(TARGET_X86)
	@echo "Clean complete"

check:
	@echo "=== Build Configuration ==="
	@echo "Architecture: $(ARCH)"
	@echo "Compiler:     $(CC)"
	@echo "Target:       $(TARGET)"
	@echo "ZLIB path:    $(ZLIB_DIR)"
	@echo ""
	@echo "=== Source Files ==="
	@echo "Main sources: $(TT_SOURCES)"
	@echo "Zlib sources: $(ZLIB_SOURCES)"

help:
	@echo "ttarchext Makefile"
	@echo ""
	@echo "Usage: make [target] [ARCH=x64|x86]"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build $(TARGET) (default)"
	@echo "  all-x64   - Build 64-bit version"
	@echo "  all-x86   - Build 32-bit version"
	@echo "  clean     - Remove build artifacts"
	@echo "  check     - Show build configuration"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make               - Build 64-bit version"
	@echo "  make ARCH=x86     - Build 32-bit version"
	@echo "  make clean         - Clean build artifacts"
	@echo ""
	@echo "Dependencies:"
	@echo "  - LLVM MinGW64 at D:/llvm_mingw64/llvm-mingw-20250812-ucrt-x86_64"
	@echo "  - oo2core_5_win64.dll (runtime, in project root)"
