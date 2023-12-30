# Makefile for building kernel image

# Compiler and linker
NASM = nasm
GCC = gcc
LD = ld

# Flags for compilation and linking
NASM_FLAGS = -f elf32
GCC_FLAGS = -m32 -c
LD_FLAGS = -m elf_i386 -T src/linker.ld

# Source and build directories
SRC_DIR = src
BUILD_DIR = build

# Source files
NASM_SOURCE = $(SRC_DIR)/kernel.asm
GCC_SOURCE = $(SRC_DIR)/kernel.c

# Object files
NASM_OBJ = $(BUILD_DIR)/kernasm.o
GCC_OBJ = $(BUILD_DIR)/kernc.o

# Target binary
TARGET = $(BUILD_DIR)/kernel

all: $(TARGET)

$(NASM_OBJ): $(NASM_SOURCE)
	$(NASM) $(NASM_FLAGS) $(NASM_SOURCE) -o $(NASM_OBJ)

$(GCC_OBJ): $(GCC_SOURCE)
	$(GCC) $(GCC_FLAGS) $(GCC_SOURCE) -o $(GCC_OBJ)

$(TARGET): $(NASM_OBJ) $(GCC_OBJ)
	$(LD) $(LD_FLAGS) -o $(TARGET) $(NASM_OBJ) $(GCC_OBJ)

clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)
