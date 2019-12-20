# Lynn's Randomizer
A customizable randomizer for Diablo II  

## Environment
Install [MSYS2](https://www.msys2.org/)  
Install Necessary build tools.  
`pacman -S git gcc make`  
Install the 32 bit toolchain  
`pacman -S mingw-w64-i686-toolchain`  
or the 64 bit toolchain  
`pacman -S mingw-w64-x86_64-toolchain`  

Clone and build [StormLib.dll](https://github.com/ladislav-zezula/StormLib)  
No need to install and or build the tests.
Copy `StormLib.dll` into either `lib/bin/x86_64` or `lib/bin/i686` respectively  

## Building
`make` or `make release`
