# NFE (ArcticFox Configuration)

![](res/nfe_ss.png)
![](res/nfe_dm_ss.png)
![](res/nfe_theme_ss.png) ![](res/nfe_theme_rp_ss.png)

This is an **UNOFFICIAL** configuration utility for devices running the ArtixFox and RedPanda **FW version >= 190602**

Please see the official forum here:
https://nfeteam.org/forum
and the STM32 thread:
https://nfeteam.org/forum/threads/lost-vape-quest-chip-grus-ursa-cyborg-btb-thelema.4905

# Audience
I wrote this for myself and I use Linux exclussively. It only needs Qt5 and hidapi, both available for Windows/Apple too, should be portable.

# Warning
This being Linux (less users who report bugs) _and_ unofficial (I don't have all the insights), will have more bugs than the official release.
I'm using this myself and don't want to brick my devices, so I've taken care... but can't promise you anything.

# Installation
You'll have to build this yourselves.
Besides, you have to give yourselves permission to access the USB device ("hidraw").
Create the file `/etc/udev/rules.d/99-af.rules` and paste this to it:
```
KERNEL=="hidraw*", MODE="0666", TAG+="uucp", TAG+="udev-acl", TAG+="uaccess"
```

## Prerequisites
### Arch and derivates
- base-devel
- qt5-base
- qt5charts
- hidapi

### Ubuntu and derivates (tested on Linux Mint 20.2 "Uma")
- build-essential
- qt5-default
- libqt5charts5-dev
- libhidapi-dev

## Build
Clone this repo and `cd` to it. Then:
```
mkdir build && cd build
cmake ..
make -j
```
That's it. You only need the generated `nfe` executable (will autodetect ArcticFox/RedPanda).

## Copyright
I got the logo picture from here: https://github.com/TBXin/NFirmwareEditor/blob/master/src/NToolbox/Resources/arcticfox_logo.png

And the fontawesome.otf from here: https://fontawesome.com/download (free for desktop)
