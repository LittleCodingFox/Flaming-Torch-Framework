#!/bin/sh

if [ $# -eq 0 ] 
    then
    echo "Usage: generate.sh GENERATOR_NAME"

    exit 1
fi

export cwd=$(pwd)

cd Dependencies

conan install .

if [ $? -ne 0 ]; then
    echo Conan failed to run. Please check for any errors and that you have it properly installed and try again.
    exit 1
fi

premake5 $1

if [ $? -ne 0 ]; then
    echo Premake failed to run. Please check for any errors and that you have it properly installed and try again.
    exit 1
fi

cd ../Development/Tools

conan install .

if [ $? -ne 0 ]; then
    echo Conan failed to run. Please check for any errors and that you have it properly installed and try again.
    exit 1
fi

premake5 $1

if [ $? -ne 0 ]; then
    echo Premake failed to run. Please check for any errors and that you have it properly installed and try again.
    exit 1
fi

cd ../DefaultProject

conan install .

if [ $? -ne 0 ]; then
    echo Conan failed to run. Please check for any errors and that you have it properly installed and try again.
    exit 1
fi

premake5 $1

if [ $? -ne 0 ]; then
    echo Premake failed to run. Please check for any errors and that you have it properly installed and try again.
    exit 1
fi

cd ../..

echo Project files for Dependencies and Development Tools have been generated. Please compile them.

cd $cwd
