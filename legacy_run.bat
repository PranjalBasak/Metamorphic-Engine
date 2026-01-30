@echo off
setlocal EnableDelayedExpansion

REM ===============================
REM Validate input argument
REM ===============================
if "%~1"=="" (
    echo Usage: run.bat ^<chunk_number^>
    exit /b 1
)

set CHUNK_NUM=%~1
set BIN_DIR=%~dp0bin
set CHUNK_DIR=%BIN_DIR%\chunk_%CHUNK_NUM%
set OUTPUT_ROOT=%~dp0output
set OUTPUT_DIR=%OUTPUT_ROOT%\chunk_%CHUNK_NUM%
set METAMORPHIC_EXE=%BIN_DIR%\metamorphic.exe

REM ===============================
REM Validate metamorphic.exe
REM ===============================
if not exist "%METAMORPHIC_EXE%" (
    echo [ERROR] metamorphic.exe not found in /bin
    exit /b 1
)

REM ===============================
REM Validate chunk directory
REM ===============================
if not exist "%CHUNK_DIR%" (
    echo Chunk Folder Not Available.
    echo Please create a chunk folder chunk_%CHUNK_NUM% in directory /bin
    exit /b 1
)

REM ===============================
REM Prepare output directory
REM ===============================
if exist "%OUTPUT_DIR%" (
    echo [INFO] Cleaning existing output directory: %OUTPUT_DIR%
    rmdir /s /q "%OUTPUT_DIR%"
)

mkdir "%OUTPUT_DIR%"

REM ===============================
REM Process executables
REM ===============================
echo [INFO] Processing executables in %CHUNK_DIR%

set FOUND_EXE=0

for %%F in ("%CHUNK_DIR%\*.exe") do (
    set FOUND_EXE=1
    echo -----------------------------------------
    echo [INFO] Mutating %%~nxF
    "%METAMORPHIC_EXE%" ^
        --input "%%F" ^
        --output "%OUTPUT_DIR%\o_%%~nxF"
)

if "%FOUND_EXE%"=="0" (
    echo [WARN] No .exe files found in %CHUNK_DIR%
)

echo -----------------------------------------
echo [INFO] Done processing chunk_%CHUNK_NUM%
exit /b 0
