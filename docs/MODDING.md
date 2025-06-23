## Mod structure

Mods let you replace any file in `assets` directory with a custom one. Mods directory is located in:

- `[executable directory]/mods` on portable Windows/Linux version
- `Android/data/com.mechakotik.tailsadventure/files/mods` on Android
- `~/.local/share/tails-adventure/mods` on Linux install

A mod is a subdirectory of `mods` directory. Its files will override corresponding files in `assets` directory. For example, if file `mods/example/tails/tails.png` exists, it will be loaded by game instead of `assets/tails/tails.png`.

To make the game load the mod, you also need to enable it: create file `mods/example/enabled` and put 1 in it.

There are also priorities to resolve conflicts between mods. To set priority for the mod, create text file `mods/example/priority` and put integer priority value in it. If multiple mods override the same file, the game will load this file from the mod with highest priority. The default priority value (if `priority` file doesn't exist) is 0.

## Debugging

If you did something wrong, the game will probably crash. To make debugging easier, you may run the game from terminal / command line: if it crashes, the error message telling what exactly went wrong will be printed (unless it's segfault).

## Sprites

A sprite consists of a spritesheet and a TOML file defining sprite and its animations. Basic sprite TOML looks like this:

```toml
[sprite]
image = "sprite.png"
width = 16
height = 16
```

Here it says to use spritesheet in `image.png` (path is relative from sprite TOML location), where each frame is 16 pixels width and 16 pixels height. The spritesheet is divided to frames of needed width and height, and these frames get indices from 0 left to right, up to down, like this:

```
0 1 2 3 4
5 6 7 8 9
10 11 12 13 14
...
```

To define an animation, you should use the following syntax:

```toml
[animations.name]
# frame sequence
frames = [0, 1, 4, 5]
# how many time units should each frame be displayed
delay = 6
# optional: how many times should animation be repeated before stopping
# if unspecified, animation will be played forever until changed
repeat = 1
```

One time unit equals 1/60 of a second. Basically a number of frames if the game is running at 60 FPS, but framerate independent.

There is a simpler syntax for static single-frame animations:

```toml
[animations]
name1 = 2
name2 = 4
name3 = 5
```

## Tilemaps

Tilemaps are using TMX format and can be edited conveniently with [Tiled map editor](https://www.mapeditor.org/). Though the support of TMX format is limited to basic features which are actually used in this game.

To make character detect collisions on a layer, you need to add bool property named `collision` to this layer and set it to `true`. To make layer rendered in front of the character, you need to add bool property named `priority` to this layer and set it to `true`.

To make a tile soild, you need to add a polygon shape to it in Tile Collision Editor. You can also add int property named `type` with int value to polygon and set it to:

- `1` to make tile up solid (like bridges in Poloy Forest)
- `2` to make tile make damage
- `3` to make tile water (like water in Cavern Island)
- `4` to make tile down solid (like down arrows in Coco Island)

To make a tile behave like spikes, you don't need to create any polygons, you just need to add int property named `spikes` to the tile and set it to:

- `0` for spikes directed up
- `1` for spikes directed down
- `2` for spikes directed left
- `3` for spikes directed right

## Levels

A level consists of a tilemap and a TOML file that defines level and objects. Level file header looks like this:

```toml
[level]
# path to music relative to assets directory
music = "sound/lr.vgm"

# leave unset for usual mode, "seafox" for Sea Fox, "seafox_ground" for
# Lake Crystal-like gameplay
mode = "seafox"

# water level Y position, leave unset if not needed
water_level = 96

spawn = [
	{ x=12, y=84 }, # default spawn point
  # spawn point if player went from LR2, flip character
	{ x=987, y=371, previous="maps/lr/lr2", flip=true },
]
```

Object definitions go in `[objects.static]`, `[objects.default]`, `[objects.possible]` blocks. Currently there is no difference between these blocks, they are used for yet unimplemented randomizer feature.

You can see usage examples for each object in level TOML files in assets, they are pretty much self descriptive.

Object parameters `x` and `y` are used often and are a  bit hard to read as bare numbers, so instead of specifying them directly, you may specify `tile_x`, `tile_y`, and optionally `offset_x`, `offset_y` (their default value is `0`). If they are specified, `x = tile_x * 16 + offset_x` and `y = tile_y * 16 + offset_y`. This is more convenient, as you may look for tile position in Tiled to see position of object.

## World map

You can edit world map points by modifying file `worldmap/points.toml`. Each point here have an index, which is its position in the list, starting from `0`. Entering a point with `path="maps/example/example"` will load level with tilemap `maps/example/example.tmx` and level file `maps/example/example.toml`.

Parameter `selection` of `map_transition` object specifies index in the world map which will be selected after player goes through this transition. If level with such index is locked yet, it will be unlocked.

