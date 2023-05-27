# Tails Adventure Remake

## Building
### Windows
To build for Windows, you can use MSYS2 and mingw32-make. Download and install MSYS2, then run following command in its terminal to install required packages:

```console
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-cmake
```

Then create a build directory and run following commands in MSYS2 MINGW64 terminal to build:

```console
cd [PATH TO BUILD DIRECTORY]
cmake [PATH TO SOURCES] -G "MinGW Makefiles"
cmake --build .
```

Built executable will be located in output folder.

### Linux
Install GCC, G++ and CMake (installation process is specific for your distro and package manager). Then run following commands to build:

```console
cd [PATH TO BUILD DIRECTORY]
cmake [PATH TO SOURCES]
cmake --build .
```
