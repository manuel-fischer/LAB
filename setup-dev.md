# 1. Tools
You must install a compiler like GCC or Clang. A compiler with C17 support is
recommended.

For IXO you might need to install python3.

If you use Windows you might find it useful to install
[CodeBlocks](http://www.codeblocks.org/downloads/binaries) with mingw.
This means that you pay attention to select links for mingw in further steps.
Because this IDE is used in both of my projects IXO and LAB, it can be used to
easily build the binaries.

Your compiler/IDE installation usually include the following

1. C-Standard library
2. OpenGL library

If any of these libraries above is not distributed with your compiler/IDE you
might need to install them manually.

The next two steps are mandatory, if your compiler/IDE does not provide the
following libraries. If your IDE provides its own installation process of
those libraries, you might prefer that instead.

# 2. Install SDL2, SDL2-ttf, SDL2-image
Select the **development library** for your specific target for all of the
following libraries:

1. [SDL2](https://www.libsdl.org/download-2.0.php)
2. [SDL2-ttf](https://www.libsdl.org/projects/SDL_ttf/)
3. [SDL2-image](https://www.libsdl.org/tmp/SDL_image/)

For each of these archive files, do the following.

You might open the file with a program like 7zip.

Navigate the archive file structure, until you reach a directory
that contains an `include/` and `lib/` subdirectory. You might have to select
a subdirectory for your target machine.

Copy the files in the `include/` and `lib/` directory in your preferred
`include/` and `lib/` directories that are used by your compiler.

When using CodeBlocks, global variables like `$(sdl2)` can be set under
`Settings>Global variables`. To add the entry click on `new` and enter `sdl2`
into the prompt. Then enter the path(s) on the right, usually only the
`base`-field is needed.

# 3. Install glew (Only windows)
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

# 4. Install and build IXO
(Independent of sections 2. and 3.)

1. Clone [manuel-fischer/IXO](https://github.com/manuel-fischer/IXO) into
   a temporary directory
2. Run `python3 install.py` in that directory\
   &#8594; This script determines the directory for c-header files and
   library-files accessed by your compiler and moves all nessesary files to
   those directories, these paths can be used to copy the other libraries by
   hand.\
   &#8594; on Windows admin rights might be required (when auto detecting path)
3. The temporary directory containing the cloned repository can be deleted by
   now
   
# 5. Install and build LAB
1. Clone [manuel-fischer/LAB](https://github.com/manuel-fischer/LAB)
2. Use CodeBlocks to build and run LAB: open `Laboratory.cbp` with 
   CodeBlocks
3. In CodeBlocks press [F9] to build and run
