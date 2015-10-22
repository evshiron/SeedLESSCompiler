# Seed LESS Compiler

## How To Build

Please use GCC version > 4.9.0 in order to have the `regex` funtioning properly.

This is a CMake project and should be able to compile with the following commands:

```bash

cd $DIR_PROJECT
cmake .
make

```

And it's confirmed that this project won't compile with MSVC. If you are going to build it on Windows, try using MinGW 64 with GCC version > 4.9.0. It's confirmed to build with GCC 5.2.0.

## How It Works

The LESS Compiler is designed to have several stages before the final CSS is generated.

  * PreParse
    * Format newline characters
    * Remove single line comments
    * Expand short colors
    * TODO: rgb(r, g, b) to #RRGGBB
  * Parse
    * Generate AST
      * Block
      * Comment
      * Variable
      * Mixin
      * Literal
    * Parse variables, with MIXINs and outer scopes
      * `@[a-zA-Z0-9-_]`
      * `"@\{[a-zA-Z0-9-_]\}"`
    * Calculate in LITERALs
      * `+ - * /`
      * `#[a-fA-F0-9]{6}`
      * `\d+px`
