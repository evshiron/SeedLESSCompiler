# Seed LESS Compiler

## How To Build

Please use GCC version > 4.9.0 in order to have the `regex` funtioning properly.

This is a CMake project and should be able to compile with the following commands:

```bash

cd $DIR_PROJECT
cmake .
make

```

And it's confirmed that this project won't compile with MSVC. If you are going to build it on Windows, try using MinGW 64 with GCC 4.9.0, later versions like 5.2.0 might fail to build due to header changes.




