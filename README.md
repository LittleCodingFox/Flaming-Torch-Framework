# Flaming-Torch-Framework

Open Source 2D Game Framework under zlib/libpng license (currently pre-alpha)

[License](LICENSE.md)

# Dependencies

- [conan](https://conan.io)
- [premake](https://premake.github.io/)

# How to build

There are several folders you need run two commands to prepare the C++ projects:

First, run either `generate.bat GENERATOR_NAME` or `generate.sh GENERATOR_NAME` to automatically setup the projects. `GENERATOR_NAME` is one of [the premake generators](https://premake.github.io/docs/Using-Premake).

Then, you need to build, in order, `Dependencies`, `Development/Tools`, and optionally the project you want to use, like the one located at `Development/DefaultProject`.
