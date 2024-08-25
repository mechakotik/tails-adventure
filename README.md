<h1 align="center">
  Tails Adventure Remake
</h1>

<div align="center">

![GitHub release (with filter)](https://img.shields.io/github/v/release/mechakotik/tails-adventure?style=flat&label=latest&color=&logo=Git&logoColor=white)
![GitHub all releases](https://img.shields.io/github/downloads/mechakotik/tails-adventure/total?color=red)
[![Telegram](https://img.shields.io/badge/Telegram-blue.svg?style=flat&logo=Telegram&logoColor=white)](https://t.me/tailsadventure)

</div>

Tails Adventure is Sonic the Hedgehog spin-off game, released for Game Gear and featuring Tails in a solo role. Opposed to other SMS and GG Sonic titles, which try to bring famous fast-paced Sonic gameplay in 8-bit world, Tails Adventure is metroidvania-like game focusing on exploration, collecting items and backtracking. It's a really unique "hidden gem" of the series, which couldn't get attention it deserves due to low popularity of Game Gear.

This project tries to bring Tails Adventure experience to modern platforms, pushing away Game Gear limitations and implementing some quality of life improvements:
- More difficult but more fair gameplay, improved enemies and bosses
- Smooth and responsive character physics
- Support for modern displays with varying aspect ratios and refresh rates
- Game runs in your native resolution and displays sprites on "fractional" positions for smoother gameplay
- Keyboard and gamepad controls with customizable keybinds and analog stick support
- Save system without password
- Pause menu and options menu

Also, it was made from the ground up in C++ without using any game engines to be less bloated, have maximum performance and portability. Latest demo includes playable Poloy Forest, Volcanic Tunnel, Polly Mt. 1, Lake Rocky, Cavern Island and Caron Forest.

## Get it

Binary releases for Windows and Linux (amd64) are available at [Releases](https://github.com/mechakotik/tails-adventure/releases). If your platform is not supported or binary release is not working properly, you may build Tails Adventure Remake from source (don't worry, it's not as hard as you might think).

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
cmake .. -DCMAKE_BUILD_TYPE=Release -DTA_LINUX_INSTALL=true
cmake --build .
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
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-cmake mingw-w64-x86_64-git
git clone --recurse-submodules https://github.com/mechakotik/tails-adventure
cd tails-adventure
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DTA_STATIC=true
cmake --build .
cmake --install .
```

Build result will be located in `tails-adventure/build/output` directory.
