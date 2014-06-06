@echo off

set DESTINATION=Artwork\PackageData\Default

if exist %DESTINATION% (
	rmdir /S /Q %DESTINATION%
)

mkdir %DESTINATION%

set SOURCE=..\Artwork\Logo\

copy /Y %SOURCE%\torch_withtext_v3.png %DESTINATION%\torch_withtext.png
copy /Y %SOURCE%\torch_small_v3.png %DESTINATION%\torch_small.png

set SOURCE=..\Artwork

copy /Y %SOURCE%\*.ttf %DESTINATION%

mkdir %DESTINATION%\UIThemes

xcopy /S /Y %SOURCE%\UIThemes %DESTINATION%\UIThemes

set DESTINATION=..\..\Content

echo %3

rem PDB used for DbgHelp
copy /Y %3 %DESTINATION%

copy %1 %2

call %4 %5 %6 %7

rmdir /S /Q %6
