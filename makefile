##########################################################
# Settings:

# Executable name:
EXE_NAME = squares

# Source and object files locations:
SRC_DIR = src
OBJ_DIR = obj

##########################################################
# Libraries:

# Remove this line if SDL2 isn't installed or to be used:
GRAPHIC_LIB = SDL2

ifeq ($(GRAPHIC_LIB), SDL2)
	GRAPHIC_FLAGS = `sdl2-config --cflags`
	GRAPHIC_LINKS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
endif

##########################################################
# Compiler options:

# # For better performance:
# PROCESSOR_ARCH = -march=native

# # Multithreading API:
# OPENMP = -fopenmp

# N.B: gcc for C, g++ for C++, alternative: clang.
CC := gcc
# CC := clang
CPPFLAGS :=
CFLAGS := -std=c99 -Wall -O2 $(PROCESSOR_ARCH) $(GRAPHIC_FLAGS) $(OPENMP)
LDFLAGS :=
LDLIBS := $(GRAPHIC_LINKS) $(OPENMP) -lm

##########################################################
# Collecting files:

# Creates the OBJ_DIR directory, if necessary:
$(shell mkdir -p $(OBJ_DIR))

# Executable, sources, objects files and dependencies:
EXE := $(EXE_NAME).exe
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEP := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.d)

##########################################################
# Compilation rules:

# The following names are not associated with files:
.PHONY: all clean

# All executables to be created:
all: $(EXE)

# Linking the program:
$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compiling the source files:
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -MP -MD $(CPPFLAGS) $(CFLAGS) -c $< -o $@

-include $(DEP)

# Cleaning with 'make clean' the object files:
clean:
	rm -fv $(EXE) $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d
