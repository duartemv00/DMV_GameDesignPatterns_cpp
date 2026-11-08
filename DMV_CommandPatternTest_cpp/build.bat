@echo off
rem Builds main.cpp with MSVC. Usage: build.bat   (add "run" to also start the game: build.bat run)
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d "%~dp0"
cl /nologo /EHsc /std:c++20 /W4 main.cpp
if errorlevel 1 exit /b 1
if /i "%~1"=="run" main.exe
