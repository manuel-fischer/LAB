# Overview
1. [Setup an IDE](#Setup-an-IDE)
2. [Install libraries](#Install-libraries)

# Setup an IDE
Select your IDE
1. [VSCode](#VSCode)
2. [Code::Blocks](#CodeBlocks)
3. If your IDE is not on this list, please add instructions here for beginners.

## VSCode
0. Install [VSCode](https://code.visualstudio.com/)
1. Install the C/C++ extension
2. Install Compiler

   You must ensure that a compiler like GCC, MSVC or Clang has been installed.
   A compiler with C17 support is recommended.

## Code::Blocks
If you use Windows you might find it useful to install
[CodeBlocks](http://www.codeblocks.org/downloads/binaries) with MinGW. This
automatically installs GCC if you select a link with `mingw` included.
This means that you pay attention to select links for MinGW in further steps.
It can be used to easily build the binaries for LAB.

# Install libraries
Your compiler/IDE installation usually include the following

1. C-Standard library
2. OpenGL library

The following Libraries need to be installed
1. [SDL2, SDL2-ttf, SDL2-image](#SDL2-SDL2-ttf-SDL2-image)
2. [Glew (Only windows)](#glew-Only-windows)

If any of these libraries above is not distributed with your compiler/IDE you
might need to install them manually.

The next two steps are mandatory, if your compiler/IDE does not provide the
following libraries. If your IDE provides its own installation process of
those libraries, you might prefer that instead.

## 1. Install SDL2, SDL2-ttf, SDL2-image
Select the **development library** for your specific target for all of the
following libraries:

1. [SDL2](https://www.libsdl.org/download-2.0.php)
2. [SDL2-ttf](https://www.libsdl.org/projects/SDL_ttf/)
3. [SDL2-image](https://www.libsdl.org/tmp/SDL_image/)

For each of these archive files, do the following.

You might open the file with a program like [7zip](https://www.7-zip.org/) on
Windows or usually your default package manager on Linux.

Navigate the archive file structure, until you reach a directory
that contains an `include/` and `lib/` subdirectory. You might have to select
a subdirectory for your target machine.

Copy the files in the `include/` and `lib/` directory in your preferred
`include/` and `lib/` directories that are used by your compiler.

When using CodeBlocks, global variables like `$(sdl2)` can be set under
`Settings>Global variables`. To add the entry click on `new` and enter `sdl2`
into the prompt. Then enter the path(s) on the right, usually only the
`base`-field is needed.

## 2. Install glew (Only windows)
(Independent of section 2.)

Select **binary** (grey box)
1. [GLEW](http://glew.sourceforge.net)

Navigate the archive file structure, until you reach a directory
that contains an `include/` and `lib/` subdirectory.

Copy the files in the `include/` and `lib/` directory in your preferred
`include/` and `lib/` directories that are used by your compiler.

You might need to copy `glew32.dll` from `bin/` (or a subdirectory of this)
to either a system directory or the execution directory. You might have to
select a subdirectory for your target machine.
   
# 5. Install and build LAB
1. Clone [manuel-fischer/LAB](https://github.com/manuel-fischer/LAB)
2. Use CodeBlocks to build and run LAB: open `Laboratory.cbp` with 
   CodeBlocks
3. In CodeBlocks press [F9] to build and run
