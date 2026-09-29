@echo off
rem OmniWorld Engine - Windows build helper.
rem Enters the MSVC environment, configures (Ninja), builds, and runs tests.
rem Usage: build.bat [--test]
setlocal
call "D:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
cd /d "%~dp0"
cmake -B build -G Ninja || exit /b 1
cmake --build build || exit /b 1
if "%1"=="--test" (
  ctest --test-dir build --output-on-failure || exit /b 1
)
endlocal
