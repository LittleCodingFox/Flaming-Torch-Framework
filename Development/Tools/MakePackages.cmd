@echo off

set PACKER_EXE=%~1
set BASE_DIR=%~2
set OUT_DIR=%~3

for /f "usebackq tokens=*" %%a in (`dir /b/a:d "%BASE_DIR%"`) do (
	echo Packing %%~nxa to %OUT_DIR%/%%~nxa.package
	"%PACKER_EXE%" -dir "%BASE_DIR%/%%~nxa" "" -out "%OUT_DIR%/%%~nxa.package"
)
