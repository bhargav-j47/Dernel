# Cross-Compiler Configuration
TARGET = i686-elf
CPATH = /home/bhargav/extra-space/opt/cross/bin
CC = $(CPATH)/$(TARGET)-gcc
AS = nasm

# Project Directories
BOOT_DIR = boot
SRC_DIR = src
INC_DIR = include
OBJ_DIR = build

# Output and Flags
KERNEL = kernel.bin

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(INC_DIR)
ASFLAGS = -f elf32
# -T linker.ld: Uses your specific linker script
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib

# Source and Object Files
BOOT_SRCS = $(wildcard $(BOOT_DIR)/*.asm)
C_SRCS = $(wildcard $(BOOT_DIR)/*.c)

# Map source files to object files in the build directory
BOOT_OBJS = $(patsubst $(BOOT_DIR)/%.asm, $(OBJ_DIR)/%.o, $(BOOT_SRCS))
C_OBJS = $(patsubst $(BOOT_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SRCS))

# Combine all object files
OBJS = $(BOOT_OBJS) $(C_OBJS)

all: $(KERNEL)

# We use $(CC) to link rather than $(LD) directly so it can locate libgcc internally
$(KERNEL): $(OBJS)
	@echo "LD	$@"
	$(CC) $(LDFLAGS) -o $@ $^ -lgcc

# assemble assembly files
$(OBJ_DIR)/%.o: $(BOOT_DIR)/%.asm
	@mkdir -p $(OBJ_DIR)
	@echo "AS	$<"
	$(AS) $(ASFLAGS) $< -o $@

# compile C files
$(OBJ_DIR)/%.o: $(BOOT_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "  CC      $<"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(KERNEL)

.PHONY: all clean
