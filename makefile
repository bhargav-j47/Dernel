MAKEFLAGS += --no-print-directory

export TARGET := i686-elf
export CPATH  := /home/bhargav/extra-space/opt/cross/bin
export CC     := $(CPATH)/$(TARGET)-gcc
export AR     := $(CPATH)/$(TARGET)-ar
export AS     := nasm
export LD 	  := $(CPATH)/$(TARGET)-ld

export TOPDIR := $(CURDIR)
export INC_DIR:= $(TOPDIR)/include

export CFLAGS := -std=gnu99 -masm=intel -ffreestanding -O2 -Wall -Wextra -I$(INC_DIR) -g
export ASFLAGS:= -f elf32
export LDFLAGS:= -T linker.ld -ffreestanding -O2 -nostdlib

KERNEL  := kernel.bin
SUBDIRS := arch32 lib 

# Collect the built-in.a from each subsystem
OBJS    := $(patsubst %, %/built-in.o, $(SUBDIRS))

.PHONY: all clean $(SUBDIRS)

all: $(KERNEL)

# link using CC to automatically locate libgcc
$(KERNEL): $(SUBDIRS)
	@echo "  LD      $@"
	@$(CC) $(LDFLAGS) -o $@ $(OBJS) -lgcc

# step into subdirectories
$(SUBDIRS):
	@$(MAKE) -C $@ TOPDIR=$(TOPDIR)

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
	@rm -f $(KERNEL)
	@echo "  CLEAN   $(KERNEL)"
