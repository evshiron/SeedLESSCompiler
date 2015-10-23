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
  * Handle
    * WIP: Parse variables, with MIXINs and outer scopes
      * `@[a-zA-Z0-9-_]`
      * `"@\{[a-zA-Z0-9-_]\}"`
    * WIP: Calculate in LITERALs
      * `+ - * /`
      * `#[a-fA-F0-9]{6}`
      * `\d+px`

## How To Mixin

<del>Replace MIXIN with corresponding BLOCK and strip out inner MIXINs (or throw errors).</del>

In `handleMixin`, help every MIXIN find their matching BLOCK. Currently no further operation.

## How To Parse Variables

Get variable values from current BLOCK first, then MIXINs in current BLOCK, finally parent/grandparent BLOCKs.

## How To Generate CSS

Recursively generate from top `BlockNode` to bottom `BlockNode`.

## Seed Test Results

  * `test1.less`: PASS
  * `test2.less`: PASS
  * `test3.less`: FAIL
  * `test4.less`: PASS
  * `test5.less`: PASS
  * `test6.less`: PASS
  * `test7.less`: FAIL
  * `test8.less`: PASS

