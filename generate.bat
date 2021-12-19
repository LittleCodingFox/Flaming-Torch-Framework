@echo off
set CURRENT=%cd%

if "%1"=="" goto usage

cd Dependencies
conan install . || goto conan_failed
premake5 %1 || goto premake_failed

cd ..\Development\Tools
conan install . || goto conan_failed
premake5 %1 || goto premake_failed

cd ..\DefaultProject
conan install . || goto conan_failed
premake5 %1 || goto premake_failed

cd ..\..

@echo Project files for Dependencies and Development Tools have been generated. Please compile them.
cd %CURRENT%

goto :eof

:premake_failed
@echo Premake failed to run. Please check for any errors and that you have it properly installed and try again.
cd %CURRENT%

goto :eof

:conan_failed
@echo Conan failed to run. Please check for any errors and that you have it properly installed and try again.

goto :eof

:usage
@echo "Usage: %0 GENERATOR_NAME"
exit /B 1
