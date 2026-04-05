# TTArchExt 编译指南

## 编译器配置

### LLVM MinGW64 编译器路径

```
D:\llvm_mingw64\llvm-mingw-20250812-ucrt-x86_64\bin
```

### 编译器可执行文件

| 架构 | 编译器路径 |
|------|-----------|
| x64 | `D:\llvm_mingw64\llvm-mingw-20250812-ucrt-x86_64\bin\x86_64-w64-mingw32-gcc.exe` |
| x86 | `D:\llvm_mingw64\llvm-mingw-20250812-ucrt-x86_64\bin\i686-w64-mingw32-gcc.exe` |

### Make 工具

```
D:\llvm_mingw64\llvm-mingw-20250812-ucrt-x86_64\bin\mingw32-make.exe
```

## 依赖项

### Zlib 库

```
ZLIB_DIR = d:/SwitchGames/SAK_64bit/output/TelltaleToolLib/ToolLibrary/Zlib
```

### Zlib 源文件

- adler32.c, compress.c, crc32.c, deflate.c
- infback.c, inffast.c, inflate.c, inftrees.c
- trees.c, uncompr.c, zutil.c

### 运行时依赖

- `oo2core_5_win64.dll` (Oodle 压缩库，需与 ttarchext_x64.exe 同目录)

## 编译命令

### 基本编译

```bash
# 编译 64 位版本（默认）
"D:\llvm_mingw64\llvm-mingw-20250812-ucrt-x86_64\bin\mingw32-make.exe"

# 编译 32 位版本
"D:\llvm_mingw64\llvm-mingw-20250812-ucrt-x86_64\bin\mingw32-make.exe" ARCH=x86

# 同时编译两个版本
"D:\llvm_mingw64\llvm-mingw-20250812-ucrt-x86_64\bin\mingw32-make.exe" all-x64
"D:\llvm_mingw64\llvm-mingw-20250812-ucrt-x86_64\bin\mingw32-make.exe" all-x86
```

### 清理构建产物

```bash
"D:\llvm_mingw64\llvm-mingw-20250812-ucrt-x86_64\bin\mingw32-make.exe" clean
```

### 检查构建配置

```bash
"D:\llvm_mingw64\llvm-mingw-20250812-ucrt-x86_64\bin\mingw32-make.exe" check
```

## 构建输出

### 目录结构

```
Ttarch_ext/
├── build/              # 中间文件（.o 对象文件）
├── ttarchext_x64.exe   # 64 位可执行文件
└── ttarchext_x86.exe   # 32 位可执行文件
```

### 源文件

- `ttarchext.c` - 主程序
- `blowfish_ttarch.c` - Blowfish 加密
- `oodle.c` - Oodle 压缩接口

## 编译选项

| 选项 | 说明 |
|------|------|
| `-O2` | 优化级别 2 |
| `-Wall` | 启用所有警告 |
| `-I"$(ZLIB_INC)"` | Zlib 头文件路径 |

## 常见问题

### 编译器未找到

确保 LLVM MinGW64 路径正确：
```
D:\llvm_mingw64\llvm-mingw-20250812-ucrt-x86_64\bin
```

### Zlib 依赖未找到

确保 TelltaleToolLib 项目存在：
```
d:/SwitchGames/SAK_64bit/output/TelltaleToolLib/ToolLibrary/Zlib
```

### 运行时 Oodle 错误

确保 `oo2core_5_win64.dll` 与编译后的 exe 文件在同一目录。
