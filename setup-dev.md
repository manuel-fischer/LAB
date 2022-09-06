# Overview
1. Clone repo recursively `git clone --recursive https://github.com/manuel-fischer/LAB`
2. [Setup an IDE](#Setup-an-IDE)
3. [Install libraries](#Install-libraries)
4. [Build and Test LAB](#Build-and-Test-LAB)

# Setup an IDE
Select your IDE
1. [VSCode](#VSCode) (Recommended)
2. [Code::Blocks](#CodeBlocks)
3. Skip to [Build and Test LAB](#Build-and-Test-LAB)
4. If your IDE is not on this list, please add instructions here for beginners.

## VSCode
0. Install [VSCode](https://code.visualstudio.com/)
1. Install the C/C++ extension
2. Install Compiler

   You must ensure that a compiler like GCC, MSVC or Clang has been installed.
   A compiler with C17 support is recommended.

## Code::Blocks

<u>Note:</u> Currently building with Code::Blocks is not actively supported. Instead run `python ./cbake.py` in the terminal manually.

If you use Windows you might find it useful to install
[CodeBlocks](http://www.codeblocks.org/downloads/binaries) with MinGW. This
automatically installs GCC if you select a link with `mingw` included.
This means that you pay attention to select links for MinGW in further steps.
It can be used to easily build the binaries for LAB.

# Clone LAB
The next step is to clone the repository before continuing with installing the libraries.

Clone [manuel-fischer/LAB](https://github.com/manuel-fischer/LAB)

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

**From your Package Manager**

Install the following packages:`

1. `libsdl2-dev`
2. `libsdl2-ttf-dev`
3. `libsdl2-image-dev`

*Using apt:*

1. `sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev`

---

**From the Website**

Select the **development library** for your specific target for all of the
following libraries (eg. using GCC on Windows: `SDL2-*-devel-2.*.*-mingw.zip`):

1. [SDL2](https://github.com/libsdl-org/SDL/releases)
2. [SDL2-ttf](https://github.com/libsdl-org/SDL_ttf/releases)
3. [SDL2-image](https://github.com/libsdl-org/SDL_image/releases)

For each of these archive files, do the following.

You might open the file with a program like [7zip](https://www.7-zip.org/) on
Windows or usually your default package manager on Linux.

Navigate the archive file structure, until you reach a directory
that contains an `include/` and `lib/` subdirectory. You might have to select
a subdirectory corresponding to your target machine (like `x86_64-w64-mingw32`).

Copy the `include/` and `lib/` directories into the `libs/` directory.

When using CodeBlocks, global variables like `$(sdl2)` can be set under
`Settings>Global variables`. To add the entry click on `new` and enter `sdl2`
into the prompt. Then enter the path(s) on the right, usually only the
`base`-field is needed.

On Windows you might need to copy `libfreetype-6`, `zlib.dll` or `libpng16-16.dll` from the bin directory into the root directory of this repository.

## 2. Install glew (Only windows)
(Independent of section 1.)

Select **binary** (grey box)
1. [GLEW](http://glew.sourceforge.net)

Navigate the archive file structure, until you reach a directory
that contains an `include/` and `lib/` subdirectory.

Copy the `include/` and `lib/` directories into the `libs/` directory.

You might need to copy `glew32.dll` from `bin/Release/x64` or `bin/Release/Win32`
to either a system directory or the execution directory. You might have to
select the subdirectory corresponding to your machine.


# Build and Test LAB
0. Ensure that a recent version of Python 3 is installed.
1. Run `python cbake.py` to build, or `python cbake.py test` to run
   - This corresponds to the VSCode Launch Option *"CBake Test"*.
2. For debugging run `python cbake.py debug && gdb dbg-lab.exe`
   - This corresponds to the VSCode Launch Option *"CBake Test Debug"*.

