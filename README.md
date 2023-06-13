# Tails Adventure Remake

## Building

### Linux
Install GCC, G++ and CMake (installation process is specific for your distro and package manager). Then create a build directory and run following commands to build:

```console
cd [PATH TO BUILD DIRECTORY]
cmake [PATH TO SOURCES]
cmake --build . -j[NUMBER OF THREADS]
```

Built executable will be located in `output` directory.

### Windows
To build for Windows, you can use MSYS2 and mingw32-make. Download and install MSYS2, then run following command in its terminal to install required packages:

```console
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-cmake
```

Then open MSYS2 MINGW64 terminal, create a build directory and run CMake build with same commands as on Linux. If you don't have MSYS2's `bin` folder in your `PATH`, you will need to manually copy runtime DLLs (`libgcc_s_seh-1.dll`, `libstdc++-6.dll` and `libwinpthread-1.dll`) to `output` directory.
