<h1 align="center">
  Tails Adventure Remake
</h1>

<div align="center">

[![GitHub release (with filter)](https://img.shields.io/github/v/release/mechakotik/tails-adventure?style=flat&label=latest&color=&logo=Git&logoColor=white)](https://github.com/mechakotik/tails-adventure/releases)
[![Endpoint Badge](https://img.shields.io/endpoint?url=https%3A%2F%2Fplay.cuzi.workers.dev%2Fplay%3Fi%3Dcom.mechakotik.tailsadventure%26gl%3DUS%26hl%3Den%26l%3Dinstalls%26m%3D%24totalinstalls&logo=google-play&color=)](https://play.google.com/store/apps/details?id=com.mechakotik.tailsadventure)
[![GitHub all releases](https://img.shields.io/github/downloads/mechakotik/tails-adventure/total?color=&logo=github)](https://github.com/mechakotik/tails-adventure/releases)
[![Telegram](https://img.shields.io/badge/Telegram-blue.svg?style=flat&logo=Telegram&logoColor=white)](https://t.me/tailsadventure)

</div>

Tails Adventure is a Sonic the Hedgehog spin-off game, released for Game Gear and featuring Tails in a solo role. Opposed to other Sonic titles with fast-paced gameplay, Tails Adventure is a metroidvania game with emphasis on exploration, collecting items and backtracking. It's a unique "hidden gem" of the series that couldn't get attention it deserves due to low popularity of Game Gear.

This project aims to bring Tails Adventure experience to modern platforms, pushing away Game Gear limitations and implementing some improvements:
- **Balance changes:** rebalanced gameplay, enemies and bosses, to make the game more challenging
- **QoL improvements:** responsive controls, pause menu, item hotswap, tired flying
- **Display support:** runs in any resolution and refresh rate, displays subpixels, allows adjusting internal resolution to "zoom out the camera"
- **Input support:** keyboard, gamepad and touchscreen input, rumble and analog stick support, customizable mappings
- **Save system:** automatic saving, convenient data select menu in Sonic 3 style
- **Mod support:** simple mod system to embed custom assets, easily modifiable formats like TMX and TOML

Also, it was made from the ground up in C++ without using any game engines to be less bloated, maximize performance and portability. Effort is put into making the framework as lightweight as possible, able to run on weak hardware.

## Implemented

**Levels (fully):** Poloy Forest, Polly Mt. 1, Cavern Island, Caron Forest, Green Island, Lake Crystal, Polly Mt. 2.

**Levels (partially):** Volcanic Tunnel, Lake Rocky.

**Items:** Regular Bomb, Remote Bomb, Napalm Bomb, Triple Bomb, Remote Robot, Hammer, Teleport Device, Speed Boots, Radio, Fang.

**Sea Fox items:** Vulcan Gun, Anti-air Missile, Extra Speed.

## Get it

Binary releases for Windows, Android and Linux (amd64) are available at [Releases](https://github.com/mechakotik/tails-adventure/releases). If your platform is not supported or binary release is not working properly, you may build Tails Adventure Remake from source (see [BUILDING](docs/BUILDING.md)).

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

## Modding

Tails Adventure Remake has mod support, which allows you to replace any file in `assets` directory with a custom one. See [MODDING](docs/MODDING.md) for modding guide and info on how game's asset formats work.

## Contributing

Contributions are welcome! Just be sure to follow project's code style (use clang-format to format the code automatically) and write clear descriptions of changes that you are making. To get started, you may search for TODO in source code.

