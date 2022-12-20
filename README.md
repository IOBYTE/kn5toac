Overview
--------

I'm just playing around with the idea of using Assetto Corsa cars in Speed Dreams.

I'm trying to convert Assetto Corsa kn5 files to AC3D files just to learn how things work.  It's partially working  but has issues.  Once I get it fully working I will decide if the AC3D file should be modified to what Speed Dreams expects or just write the kn5 importers and use the kn5 files.

It should also be possible to generate a Speed Dreams configuration file from the ini and lut files.

Status
------

I'm able to extract all the materials, textures and geometry without problems.  The cars render fine in AC3D and Speed Dreams when forcing use of just the diffuse textures. The cars require multiple layers of textures to render properly and AC3D only supports a single texture. Assetto Corsa uses shaders to apply the textures and there is no way to do that in AC3D.

Asseto Corsa car models have wheels and the steering wheels included in the model.  Speed Dreams expects them to be in seperate files.  The wheels and steering wheels are now extracted into seperate files.

The converted Speed Dreams .acc files do not have multiple textures and triangle strips yet.

I think it's time to try generating a car xml config file from the kn5, ini and lut files.

Build with CMake on Windows
---------------------------

Create a build directory inside the example project directory and configure the
build using [CMake](https://cmake.org) to generate the Visual Studio project:
```
mkdir build
cd build
cmake CMAKE_BUILD_TYPE=Release ..
```
Build with CMake and GNU Make
-----------------------------

Create a build directory inside the example project directory and configure the
build using [CMake](https://cmake.org) to generate the Makefile for
[GNU Make](https://www.gnu.org/software/make/):
```
mkdir build
cd build
cmake CMAKE_BUILD_TYPE=Release ..
```

Then build the executable named ```kn5toac``` using GNU Make:
```
make
```

The resulting binary is ```kn5toac``` inside the build directory.

You can install ```kn5toac``` using GNU Make:
```
sudo make install
```
