@echo off
chcp 65001 >nul
REM ============================================================================
REM TTArch2 Patch Rebuild Script
REM ============================================================================
REM Purpose:
REM   1. Extract clean files to a patch directory
REM   2. Apply modified choice.prop
REM   3. Rebuild patch archive with Oodle compression
REM ============================================================================

setlocal

REM ============================================================================
REM Configuration
REM ============================================================================
set "SOURCE_ARCHIVE=D:\SwitchGames\SAK_64bit\output\WD2_nx_Project_data.ttarch2\original-archives\WD2_nx_Project_data.ttarch2"
set "PATCH_DIR=D:\SwitchGames\SAK_64bit\output\WD2_nx_Project_data.ttarch2\patch-temp"
set "FIXED_FILE=D:\SwitchGames\SAK_64bit\output\WD2_nx_Project_data.ttarch2\fixed\choice.prop"
set "OUTPUT_DIR=D:\SwitchGames\SAK_64bit\output\WD2_nx_Project_data.ttarch2\pack-by-ttarchext"
set "OUTPUT_FILE=%OUTPUT_DIR%\WD2_nx_Project_data.ttarch2"
set "GAME_NUM=55"

REM ============================================================================
REM Step 1: Clean and create patch directory
REM ============================================================================
echo.
echo ============================================================================
echo Step 1: Preparing patch directory
echo ============================================================================
echo.
if exist "%PATCH_DIR%" (
    echo Removing existing patch directory: %PATCH_DIR%
    rmdir /s /q "%PATCH_DIR%"
)
echo Creating patch directory: %PATCH_DIR%
mkdir "%PATCH_DIR%"

REM ============================================================================
REM Step 2: Extract clean files to patch directory
REM ============================================================================
echo.
echo ============================================================================
echo Step 2: Extracting clean files
echo ============================================================================
echo.
echo Source: %SOURCE_ARCHIVE%
echo Target: %PATCH_DIR%
echo.

.\ttarchext_x64.exe -o -V 7 %GAME_NUM% "%SOURCE_ARCHIVE%" "%PATCH_DIR%"

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Extraction failed with error code %ERRORLEVEL%
    pause
    exit /b 1
)

echo.
echo Extraction completed successfully!
echo.

REM ============================================================================
REM Step 3: Apply modified file
REM ============================================================================
echo ============================================================================
echo Step 3: Applying modified choice.prop
echo ============================================================================
echo.
echo Copying: %FIXED_FILE%
echo To:      %PATCH_DIR%\choice.prop
echo.

if not exist "%FIXED_FILE%" (
    echo ERROR: Modified file not found: %FIXED_FILE%
    pause
    exit /b 1
)

copy /y "%FIXED_FILE%" "%PATCH_DIR%\choice.prop"

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Failed to copy modified file
    pause
    exit /b 1
)

echo.
echo Modified file applied successfully!
echo.

REM ============================================================================
REM Step 4: Create output directory
REM ============================================================================
echo.
echo ============================================================================
echo Step 4: Preparing output directory
echo ============================================================================
echo.
if not exist "%OUTPUT_DIR%" (
    echo Creating output directory: %OUTPUT_DIR%
    mkdir "%OUTPUT_DIR%"
)

REM ============================================================================
REM Step 5: Rebuild patch archive
REM ============================================================================
echo.
echo ============================================================================
echo Step 5: Rebuilding patch archive with Oodle compression
echo ============================================================================
echo.
echo Source: %PATCH_DIR%
echo Output: %OUTPUT_FILE%
echo Options: -b -z -A -V 7 %GAME_NUM%
echo.
echo - Rebuild mode (-b): Create new archive
echo - Oodle compression (-z): Use LZHLW compression (level 7)
echo - Alphabetical sort (-A): Match original archive format
echo - Version 7 (-V 7): Force TTArch2 version
echo.

.\ttarchext_x64.exe -o -b -z -A -L -4 -V 7 %GAME_NUM% "%OUTPUT_FILE%" "%PATCH_DIR%"

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Rebuild failed with error code %ERRORLEVEL%
    pause
    exit /b 1
)

echo.
echo ============================================================================
echo SUCCESS: Patch archive rebuilt successfully!
echo ============================================================================
echo.
echo Output: %OUTPUT_FILE%
echo.

REM Show file size
for %%F in ("%OUTPUT_FILE%") do (
    echo File size: %%~zF bytes
)
echo.

REM ============================================================================
REM Optional: Clean up patch directory
REM ============================================================================
echo.
echo ============================================================================
echo Cleanup
REM ============================================================================
echo.
echo Patch directory kept at: %PATCH_DIR%
echo You can manually delete it when done.
echo.
