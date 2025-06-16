@echo off
gcc main.c -o main.exe
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)
