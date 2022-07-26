# fmmidi2
fmmidi2 is Yamaha YM2608 FM synthesizer emulator. It is based on fmmidi by yuno and supercatexpert1. (https://github.com/supercatexpert/fmmidi)

# Changes compared to fmmidi
- General code cleanup
- Some options are missing for the sake of simplicity (ex. only libao supported)
- Custom WAV file writer (Due to a bug on Windows)

# Compiling
This app is tested on Windows and Linux.

## Windows
- Install MSYS2 (https://www.msys2.org/)
- Install `ming-w64-gcc`
- Install `make` and `mingw-w64-make`
- Install `mingw-w64-libao`
- Run `make build`

## Linux
- Install libao package from your distribution's package manager (libao on Arch, libao-dev on Ubuntu)
- Install g++ and make 
- Run `make build`

# License 
See license.txt.