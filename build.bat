@echo off
setlocal

:: Define Paths
set SRC_DIR=src
set INC_DIR=include
set LIB_DIR=lib
set BIN_DIR=bin

echo [1/2] Compiling Metamorphic Engine...
cl.exe /I%INC_DIR% /I. -D_CRT_SECURE_NO_WARNINGS ^
    %SRC_DIR%\main.c %SRC_DIR%\engine.c %SRC_DIR%\r2session.c ^
    %SRC_DIR%\mutator.c %SRC_DIR%\cJSON.c %SRC_DIR%\getopt.c ^
    /Fe:%BIN_DIR%\metamorphic.exe ^
    /link /LIBPATH:%LIB_DIR% capstone.lib keystone.lib Shell32.lib

if %errorlevel% equ 0 (
    echo [2/2] Cleaning up intermediate files...
    del *.obj *.exp %BIN_DIR%\metamorphic.lib
    echo.
    echo SUCCESS: Binaries are in the \bin folder.
) else (
    echo.
    echo ERROR: Build failed.
)