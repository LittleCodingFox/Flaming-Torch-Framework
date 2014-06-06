@echo off
cls
cls
set /P PROJECTNAME=Please type the project name: 
set /P EXENAME=Please type the project exe name (without the .exe)

echo "..\%PROJECTNAME%"

cd "..\%PROJECTNAME%"

if exist "..\%PROJECTNAME%\finishbuild.bat" (
	"..\%PROJECTNAME%\finishbuild.bat" "..\..\Binaries\%EXENAME%\Release\%EXENAME%.exe" "..\..\Content\%EXENAME%.exe" "..\..\Binaries\%EXENAME%\%EXENAME%.pdb" "..\Tools\MakePackages.cmd" "..\Tools\Packer\PackerBin\Packer.exe" "Artwork\PackageData" "..\..\Content\"
	cd ..\Tools\
	echo PackProject is done!
) else (
	echo Missing finishbuild.bat!
)

pause
