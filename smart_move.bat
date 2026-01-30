@echo off
setlocal EnableDelayedExpansion

echo.
set /p FILE_HINT=Enter the approximate name of the file you want to move: 

if "%FILE_HINT%"=="" (
    echo No input given.
    exit /b 1
)

echo.
echo Searching for files...
set COUNT=0

for /r "%~dp0" %%F in (*%FILE_HINT%*) do (
    set /a COUNT+=1
    set FILE_!COUNT!=%%F
    echo [!COUNT!] %%F
)

if %COUNT%==0 (
    echo No matching files found.
    exit /b 1
)

echo.
set /p FILE_CHOICE=Choose file number: 

if not defined FILE_%FILE_CHOICE% (
    echo Invalid selection.
    exit /b 1
)

set SELECTED_FILE=!FILE_%FILE_CHOICE%!

echo.
set /p DIR_HINT=Enter the destination directory name: 

if "%DIR_HINT%"=="" (
    echo No input given.
    exit /b 1
)

echo.
echo Searching for directories...
set DCOUNT=0

for /d /r "%~dp0" %%D in (*%DIR_HINT%*) do (
    set /a DCOUNT+=1
    set DIR_!DCOUNT!=%%D
    echo [!DCOUNT!] %%D
)

if %DCOUNT%==0 (
    echo No matching directories found.
    exit /b 1
)

echo.
set /p DIR_CHOICE=Choose destination number: 

if not defined DIR_%DIR_CHOICE% (
    echo Invalid selection.
    exit /b 1
)

set SELECTED_DIR=!DIR_%DIR_CHOICE%!

echo.
echo Moving:
echo   From: %SELECTED_FILE%
echo   To:   %SELECTED_DIR%
echo.

move "%SELECTED_FILE%" "%SELECTED_DIR%" >nul

if errorlevel 1 (
    echo Move failed.
    exit /b 1
)

echo Move completed successfully.
endlocal
