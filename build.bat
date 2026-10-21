@echo off
setlocal

REM Configure PATH to include MSYS2 UCRT64 toolchain
set "PATH=C:\msys64\ucrt64\bin;C:\msys64\usr\bin;%PATH%"

if "%1"=="clean" (
    echo Cleaning build artifacts...
    if exist cinematch.exe del /f /q cinematch.exe
    if exist test_runner.exe del /f /q test_runner.exe
    if exist *.o del /f /q *.o
    echo Clean complete.
    goto :eof
)

if "%1"=="test" (
    echo Building CineMatch Automated Test Suite...
    gcc -std=c11 -Wall -Wextra -pedantic -Iinclude tests/test_runner.c tests/test_ds.c tests/test_json.c tests/test_tmdb.c src/movie.c src/ds.c src/json_parser.c src/tmdb.c src/ui.c -lcurl -o test_runner.exe
    if errorlevel 1 (
        echo [ERROR] Test compilation failed!
        exit /b 1
    )
    echo Running tests...
    test_runner.exe
    goto :eof
)

if "%1"=="run" (
    if not exist cinematch.exe (
        call :build_app
    )
    cinematch.exe
    goto :eof
)

:build_app
echo Building CineMatch application (C11, MSYS2 UCRT64, libcurl)...
gcc -std=c11 -Wall -Wextra -pedantic -Iinclude src/main.c src/movie.c src/ds.c src/json_parser.c src/tmdb.c src/ui.c -lcurl -o cinematch.exe
if errorlevel 1 (
    echo [ERROR] Compilation failed!
    exit /b 1
)
echo [SUCCESS] cinematch.exe built successfully.
goto :eof
