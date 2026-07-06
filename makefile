# Cross-Compiler Configuration
TARGET = i686-elf
CPATH = /home/bhargav/extra-space/opt/cross/bin
CC = $(CPATH)/$(TARGET)-gcc
AS = nasm

# Project Directories
ARCH_X86_DIR = arch/x86
INC_DIR = include
OBJ_DIR = build

# Output and Flags
KERNEL = kernel.bin

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(INC_DIR) -g
ASFLAGS = -f elf32
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib 

# 1. Automatically find all subdirectories inside arch/x86/
SRC_DIRS := $(shell find $(ARCH_X86_DIR) -type d)

# 2. Tell Make to search ALL found directories for source files
vpath %.c $(SRC_DIRS)
vpath %.asm $(SRC_DIRS)

# 3. Recursively find every .c and .asm file inside arch/x86/
ASM_SRCS := $(shell find $(ARCH_X86_DIR) -name '*.asm')
C_SRCS := $(shell find $(ARCH_X86_DIR) -name '*.c')

# 4. Map them to a flat list of .o files in the build/ directory using 'notdir'
ASM_OBJS := $(patsubst %.asm, $(OBJ_DIR)/%.o, $(notdir $(ASM_SRCS)))
C_OBJS := $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(C_SRCS)))

# Combine all object files
OBJS = $(ASM_OBJS) $(C_OBJS)

all: $(KERNEL)

# We use $(CC) to link rather than $(LD) directly so it can locate libgcc internally
$(KERNEL): $(OBJS)
	@echo "LD	$@"
	@$(CC) $(LDFLAGS) -o $@ $^ -lgcc

# assemble assembly files
$(OBJ_DIR)/%.o: %.asm
	@mkdir -p $(OBJ_DIR)
	@echo "AS	$<"
	@$(AS) $(ASFLAGS) $< -o $@

# compile C files
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	@echo "CC	$<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "CLEAN"
	@rm -rf $(OBJ_DIR) $(KERNEL)

.PHONY: all clean
