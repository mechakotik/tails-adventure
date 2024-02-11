<h1 align="center">
  <img width="50%" src="https://github.com/mechakotik/tails-adventure/assets/43793572/cb300eb2-4f5f-42f3-97e8-e54e48810aa9">
</h1>

<div align="center">

![GitHub release (with filter)](https://img.shields.io/github/v/release/TA-Remake/TailsAdventure?style=flat&label=latest&color=&logo=Git&logoColor=white)
![GitHub all releases](https://img.shields.io/github/downloads/mechakotik/TailsAdventure/total?color=red)
[![Telegram](https://img.shields.io/badge/Telegram-blue.svg?style=flat&logo=Telegram&logoColor=white)](https://t.me/tailsadventure)

<img width="48%" alt="pf" src="https://github.com/mechakotik/tails-adventure/assets/43793572/7a9a6597-2c18-4e7a-9637-2c043673bab3"><img width="48%" alt="pm2" src="https://github.com/mechakotik/tails-adventure/assets/43793572/144954c7-a9fb-425e-9f4d-795b7ca591f0">

</div>

Tails Adventure is a Sonic the Hedgehog spin-off adventure game, released for Sega Game Gear in 1995. Unlike classic Sonic gameplay, Tails Adventure is a slow-paced platformer with focus on solving puzzles, backtracking and collecting items, much like metroidvania games.

This project tries to completely rewrite Tails Adventure to provide a better way to play it on modern platforms, as well as implement some improvements to original gameplay.

## Improvements
- Higher difficulty that original
- 16:9 widescreen
- Displays pixels on "fractional" positions for smoother gameplay
- Gamepad controls with analog stick support
- Supports high refresh rate displays
- Save system without password
- Pause menu and options menu

## Get it

| Platform | Description |
| ------------- | ------------- |
| Windows | Download latest binary release from [Releases](https://github.com/mechakotik/TailsAdventure/releases) |
| Arch Linux | Install [tails-adventure](https://aur.archlinux.org/packages/tails-adventure) package from AUR |
| Windows/Linux | Build from source (see tutorial below) |

## Building from source

To build this project, you need to install C++ compiler, Meson build system and following libraries:

- [SDL2](https://github.com/libsdl-org/SDL/tree/SDL2)
- [SDL2_image](https://github.com/libsdl-org/SDL_image/tree/SDL2)
- [SDL2_mixer](https://github.com/libsdl-org/SDL_mixer/tree/SDL2)
- [tinyxml2](https://github.com/leethomason/tinyxml2)

The installation process is specific for your OS and package manager, here are a few examples:

```bash
# MSYS2 MINGW64 (Windows)
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-meson mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-tinyxml2

# Arch Linux
sudo pacman -S base-devel meson sdl2 sdl2_image sdl2_mixer tinyxml2

# Debian / Ubuntu
sudo apt install build-essential meson libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libtinyxml2-dev
```

Then clone this repository and run following commands in repository root directory:

```bash
# To build "portable" version in build/output directory
meson setup build && cd build
meson compile
meson install

# Or to install game on Linux system
meson setup build -Dlinux_install=true && cd build
meson compile
sudo meson install
```

## Disclaimer

Tails Adventure Remake is a non-profit fan game project. It is not affiliated with SEGA, Sonic Team or Aspect in any way. The developers of this project have no intent to infringe said copyrights or registered trademarks. No financial gain is made from this project.

Sonic the Hedgehog is a trademark of SEGA. All copyrights and registered trademarks regarding Sonic the Hedgehog belong to SEGA. Original Tails Adventure assets used in this project are property of SEGA, Sonic Team, Aspect and their respective owners.

The source code is published under GNU GPL-3.0 license.
