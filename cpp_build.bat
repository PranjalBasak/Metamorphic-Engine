@echo off
setlocal

:: Define Paths
set SRC_DIR=src
set INC_DIR=include
set LIB_DIR=lib
set BIN_DIR=bin

:: Ensure bin directory exists
if not exist %BIN_DIR% mkdir %BIN_DIR%

echo [1/2] Compiling Metamorphic Engine (C++ Mode)...

:: Compiler Flags:
:: /nologo - Suppresses the startup banner
:: /O2     - Optimizes for maximum speed
:: /EHsc   - Enables standard C++ exception handling
:: /I      - Include paths (Added subfolders for Capstone/Keystone)
:: /W3     - Enable a healthy level of warnings for development
cl.exe /nologo /O2 /W3 /EHsc ^
    /I%INC_DIR% ^
    /I%INC_DIR%\capstone ^
    /I%INC_DIR%\keystone ^
    /I. ^
    -D_CRT_SECURE_NO_WARNINGS ^
    %SRC_DIR%\main.cpp ^
    %SRC_DIR%\engine.cpp ^
    %SRC_DIR%\r2session.cpp ^
    %SRC_DIR%\mutator.cpp ^
    %SRC_DIR%\cJSON.cpp ^
    %SRC_DIR%\getopt.cpp ^
    /Fe:%BIN_DIR%\metamorphic.exe ^
    /link /LIBPATH:%LIB_DIR% capstone.lib keystone.lib Shell32.lib

if %errorlevel% equ 0 (
    echo [2/2] Cleaning up intermediate files...
    :: Cleanup .obj files and linker artifacts
    del *.obj 2>nul
    del %BIN_DIR%\metamorphic.exp 2>nul
    del %BIN_DIR%\metamorphic.lib 2>nul
    echo.
    echo SUCCESS: Binaries are ready in \%BIN_DIR%\metamorphic.exe
) else (
    echo.
    echo ERROR: Build failed. Check the errors above.
    exit /b %errorlevel%
)