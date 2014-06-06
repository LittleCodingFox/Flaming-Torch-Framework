@echo off

set DESTINATION=..\..\Content

rem PDB used for DbgHelp
copy /Y %3 %DESTINATION%

copy %1 %2
