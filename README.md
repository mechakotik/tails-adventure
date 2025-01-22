<h1 align="center">
  Tails Adventure Remake
</h1>

<div align="center">

[![GitHub release (with filter)](https://img.shields.io/github/v/release/mechakotik/tails-adventure?style=flat&label=latest&color=&logo=Git&logoColor=white)](https://github.com/mechakotik/tails-adventure/releases)
[![Endpoint Badge](https://img.shields.io/endpoint?url=https%3A%2F%2Fplay.cuzi.workers.dev%2Fplay%3Fi%3Dcom.mechakotik.tailsadventure%26gl%3DUS%26hl%3Den%26l%3Dinstalls%26m%3D%24totalinstalls&logo=google-play&color=)](https://play.google.com/store/apps/details?id=com.mechakotik.tailsadventure)
[![GitHub all releases](https://img.shields.io/github/downloads/mechakotik/tails-adventure/total?color=&logo=github)](https://github.com/mechakotik/tails-adventure/releases)
[![Telegram](https://img.shields.io/badge/Telegram-blue.svg?style=flat&logo=Telegram&logoColor=white)](https://t.me/tailsadventure)

</div>

Tails Adventure is Sonic the Hedgehog spin-off game, released for Game Gear and featuring Tails in a solo role. Opposed to other Sonic titles with fast-paced gameplay, Tails Adventure is metroidvania game with emphasis on exploration, collecting items and backtracking. It's a really unique "hidden gem" of the series, which couldn't get attention it deserves due to low popularity of Game Gear.

This project tries to bring Tails Adventure experience to modern platforms, pushing away Game Gear limitations and implementing some improvements:
- **New features:** item hotswap menu, ring drop system
- **Balance improvements:** rebalanced gameplay, enemies and bosses
- **Smoother gameplay:** runs in your native resolution and refresh rate, displays subpixels
- **Better controls:** customizable keyboard and gamepad mappings, analog input support
- **Save system:** convenient data select menu in Sonic 3 style

Also, it was made from the ground up in C++ without using any game engines to be less bloated, maximize performance and portability.

## Default controls

Controls can be remapped with Options -> Controls -> Map keyboard / Map gamepad.

| Action | Keyboard | Gamepad |
| ------ | -------- | ------- |
| Move | Arrow keys | Left stick or D-Pad |
| Jump (A) | Z | A |
| Use item (B) | C | B |
| Previous item | A | LB |
| Next item | D | RB |
| Start | Enter | Start |
| Toggle fullscreen | Right Alt + Enter | - |

## Implemented

**Levels (fully):** Poloy Forest, Polly Mt. 1, Cavern Island, Caron Forest.

**Levels (partially):** Volcanic Tunnel, Lake Rocky.

**Items:** Regular Bomb, Remote Bomb, Napalm Bomb, Triple Bomb, Remote Robot, Hammer, Teleport Device, Speed Boots, Radio.

**Sea Fox items:** Vulcan Gun.

## Get it

Binary releases for Windows, Android and Linux (amd64) are available at [Releases](https://github.com/mechakotik/tails-adventure/releases). If your platform is not supported or binary release is not working properly, you may build Tails Adventure Remake from source (don't worry, it's not as hard as you might think).

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
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-cmake git
git clone --recurse-submodules https://github.com/mechakotik/tails-adventure
cd tails-adventure
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DTA_STATIC=true
cmake --build .
cmake --install .
```

Build result will be located in `tails-adventure/build/output` directory.

## Building for Android

The Android project is located in `android` directory. However, some of the necessary files that are not part of Tails Adventure Remake are missing and need to be copied from SDL sources.

If you have a Unix-like environment, you may use `setup.sh` shell script in `android` directory, it will automatically copy all the needed files. If you are not able to run shell scripts, you'll have to copy all the files mentioned in the script manually.

After copying the necessary files, you may just open `android` directory as a project in Android Studio and build using it.

## Contributing

Contributions are welcome! Just be sure to follow project's code style and write clear descriptions of changes that you are making. To get started, you may search for TODO in source code.
