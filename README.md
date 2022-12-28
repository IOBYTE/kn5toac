Overview
--------

I'm just playing around with the idea of using Assetto Corsa cars in Speed Dreams.

I'm trying to convert an Assetto Corsa car directory to a Speed Dreams car directory.  It's partially working  but has issues. 

Status
------

I'm able to extract all the materials, textures and geometry without problems.  The cars render fine in AC3D and Speed Dreams when forcing use of just the diffuse textures. The cars require multiple layers of textures to render properly and AC3D only supports a single texture. Assetto Corsa uses shaders to apply the textures and there is no way to do that in AC3D.

Asseto Corsa car models have wheels and the steering wheels included in the model.  Speed Dreams expects them to be in seperate files.  The wheels and steering wheels are now extracted into seperate files.

The converted Speed Dreams .acc files do not have multiple textures and triangle strips yet.

The car xml config file is generated from the kn5, ini and lut files.  The ini files in the data.acd file are used when available. The skins and previews are converted.

The wheels can't be used without modifications.  The brake disks can't be used because they are created by Speed Dreams.  Reusing these unmodified from the original model will require changing the Speed Dreams loaders or writing a kn5 loader for osg and ssg.

It's possible to load and drive the assetto corsa sdk formuls_k car.  The frame rate is low because only the highest quality LOD is loaded.  Not every car parameter is avilable so weird defaults are picked.  The default wheels look funny.  It's not just convert and play yet.

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
Examples
--------
```
kn5toac -c 92MP1 -i "C:/Program Files (x86)/Steam/steamapps/common/assettocorsa/sdk/dev/content/cars/formula_k" -o "C:/Users/Bob/speed-dreams-code/data/cars/models"
```
A directory called formula_k will be created in the output directory specified by -o and all the generated files will be placed there.
