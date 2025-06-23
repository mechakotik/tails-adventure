## Building for Linux

To build Tails Adventure Remake, you need to install C++ compiler, Make, CMake and git. The installation process varies through different distros and package managers, here is a few examples:

```sh
# Debian and its derives
sudo apt install build-essential cmake git

# Arch and its derives
sudo pacman -S gcc make cmake git
```

After installing required libraries, run following commands to clone repository, build and install Tails Adventure Remake:

```sh
git clone --recurse-submodules https://github.com/mechakotik/tails-adventure
cd tails-adventure
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DTA_UNIX_INSTALL=true
cmake --build . -j$(nproc)
sudo cmake --install .
```

This will install Tails Adventure Remake system-wide and you'll be able to see it in your applications menu. To uninstall, run this command in `tails-adventure/build` directory:

```sh
sudo make uninstall
```

## Building for Windows

To build for Windows, you need to install [MSYS2](https://www.msys2.org). After installing, run this command in MSYS2 MINGW64 shell to update packages:

```sh
pacman -Syu
```

If it asks to restart shell, restart it and run this command again. After MSYS2 is fully updated, run following commands in MSYS2 MINGW64 shell to install dependencies, clone repository and build Tails Adventure Remake:

```sh
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-ninja mingw-w64-x86_64-cmake git
git clone --recurse-submodules https://github.com/mechakotik/tails-adventure
cd tails-adventure
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DTA_STATIC=true
cmake --build . -j$(nproc)
cmake --install .
```

Build result will be located in `tails-adventure/build/output` directory.

## Building for Android

The Android project is located in `android` directory. However, some of the necessary files that are not part of Tails Adventure Remake are missing and need to be copied from SDL sources.

If you have a Unix-like environment, you may use `setup.sh` shell script in `android` directory, it will automatically copy all the needed files. If you are not able to run shell scripts, you'll have to copy all the files mentioned in the script manually.

After copying the necessary files, you may just open `android` directory as a project in Android Studio and build using it.

