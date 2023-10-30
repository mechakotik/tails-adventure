# Tails Adventure Remake

An unofficial remake of Tails Adventure for modern platforms, featuring 16:9 widescreen, high refresh rate support, improved physics, controls and enemies' behaviour. It is written in C++ and SDL2 without using any game engines / frameworks. The current demo includes first 3 areas - Poloy Forest, Volcanic Tunnel and Polly Mt. 1.

## Getting dependencies

To build this project, you need to install C++ compiler, Meson build system and following libraries:

- [SDL2](https://github.com/libsdl-org/SDL/tree/SDL2)
- [SDL2_image](https://github.com/libsdl-org/SDL_image/tree/SDL2)
- [SDL2_mixer](https://github.com/libsdl-org/SDL_mixer/tree/SDL2)
- [tinyxml2](https://github.com/leethomason/tinyxml2)

The installation process is specific for your OS and package manager, here is a few examples:

### MSYS2 MinGW (Windows)

IMPORTANT: run install and build commands in MSYS2 MINGW64 terminal, not in the default MSYS terminal.

```console
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-meson mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-tinyxml2 mingw-w64-x86_64-curl mingw-w64-x86_64-
```

### Arch Linux

```console
sudo pacman -Syu
sudo pacman -S base-devel meson sdl2 sdl2_image sdl2_mixer tinyxml2
```

## Building

Clone this repository and run following command in repository root directory:

```console
meson setup build
```

This will create `build` subdirectory in repository root and configure Meson to build in it. Then run these commands to build project:

```console
cd build
meson compile && meson install
```

Built game will be located in `build/output` directory.

### Using static linking

It is recommended to link all the dependencies statically for better portability. Howewer, most of distros don't package static libraries, so static linking is impossible without manually building all the dependent libraries. To make build system try using static linking, pass the `prefer_static` option when creating build directory:

```console
meson setup build -Dprefer_static=true
```
