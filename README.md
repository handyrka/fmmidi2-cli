# fmmidi2
fmmidi2 is Yamaha YM2608 FM synthesizer emulator. It is based on fmmidi by yuno and supercatexpert1. (https://github.com/supercatexpert/fmmidi)

# Changes compared to fmmidi
- General code cleanup
- More intuitive to use
- Output to files (WAV, Ogg Vorbis)
- Bugfixes

# Dependencies
`libao libvorbis libvorbisenc libogg`

# Compiling
## Windows
- Install MSYS2 (https://www.msys2.org/)
- Run following command:
##### 64 bit
`pacman -S --needed base-devel make mingw-w64-x86_64-toolchain mingw-w64-x86_64-libao mingw-w64-x86_64-libvorbis mingw-w64-x86_64-libogg`
##### 32 bit
`pacman -S --needed base-devel make mingw-w64-i686-toolchain mingw-w64-i686-libao mingw-w64-i686-libvorbis mingw-w64-i686-libogg`
- Run `make build` in project's base folder

## Linux
- Install `libao libvorbis libogg` from your distribution's package manager (ex. libao on Arch, libao-dev on Ubuntu)
- Install g++ and make
- Run `make build`

## Other platforms
While not tested yet, it should run just fine.

# License 
See license.txt.
