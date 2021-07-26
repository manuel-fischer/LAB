
ifeq ($(OS),Windows_NT)
UNAME := Windows
else
UNAME := $(shell uname -s)
endif



ifeq ($(UNAME), Windows)
RM = del
else
RM = rm -f
endif



CC = gcc
# Common compiler settings
CFLAGS  = -std=c17 \
		  -D_POSIX_C_SOURCE=200101L


# short debug output of filenames
CFLAGS += -fmacro-prefix-map=$(CURDIR)=.

# Warnings
CFLAGS += -Wall \
		  -Wextra \
		  -pedantic \
		  -Wfatal-errors \
		  -pedantic-errors \
		  -Winit-self \
		  -Winline \
		  -Wmissing-declarations \
		  -Wmissing-include-dirs \
		  -Wundef \
		  -Wredundant-decls \
		  -Wfloat-equal \
		  -Wmain \
		  -Wunreachable-code \
		  -Wshadow \
		  -Wcast-align \
		  -Wswitch-enum \
		  \
		  -Wno-unused-parameter

# Include paths
CFLAGS += -I$(CURDIR)/include \
		  -I/Libs/include \
		  -L/Libs/lib

# Link Libraries
ifeq ($(UNAME), Windows)
LINK = -lmingw32 \
	   -luser32 \
	   -lgdi32 \
	   -lwinmm \
	   -ldxguid \
	   -lglew32 \
	   -lSDL2main \
	   -lSDL2 \
	   -lSDL2_image \
	   -lSDL2_ttf \
	   -lopengl32 \
	   \
	   -limagehlp
else
#LINK = TODO
endif

# -mwindows



ifeq ($(UNAME), Windows)
LAB_DEBUG_EXEC   = lab-debug.exe
LAB_RELEASE_EXEC = lab.exe
else
LAB_DEBUG_EXEC   = lab-debug.out
LAB_RELEASE_EXEC = lab.out
endif

.PHONY: debug_exec release_exec clean

debug_exec: $(LAB_DEBUG_EXEC)
release_exec: $(LAB_RELEASE_EXEC)

$(LAB_DEBUG_EXEC): $(wildcard src/*.*) $(wildcard include/*.*)
	$(CC) -o $(LAB_DEBUG_EXEC) -O2 -g $(CFLAGS) $(wildcard src/*.c) $(LINK)

$(LAB_RELEASE_EXEC): $(wildcard src/*.*) $(wildcard include/*.*)
	$(CC) -o $(LAB_RELEASE_EXEC) -Ofast $(CFLAGS) $(wildcard src/*.c) $(LINK)


clean:
	$(RM) lab.exe