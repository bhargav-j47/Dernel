# Filter out anything ending in '/' to get files
obj-y-files := $(filter-out %/, $(obj-y))

# Filter ONLY things ending in '/', and remove the slash to get dir names
subdir-y    := $(patsubst %/, %, $(filter %/, $(obj-y)))

# expected sub-objects
subdir-objs := $(patsubst %, %/built-in.o, $(subdir-y))

all: built-in.o

# step into subdirectories first
.PHONY: $(subdir-y)
$(subdir-y):
	@$(MAKE) -C $@

# Merge files & subdirectory built-in.o files together
built-in.o: $(obj-y-files) $(subdir-y)
	@echo "  LD      $(CURDIR)/$@"
	@$(LD) -r -o $@ $(obj-y-files) $(subdir-objs)

# Compile C files
%.o: %.c
	@echo "  CC      $(CURDIR)/$@"
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile Assembly files
%.o: %.asm
	@echo "  AS      $(CURDIR)/$@"
	@$(AS) $(ASFLAGS) -g $< -o $@

clean:
	@rm -f *.o built-in.o
	@for dir in $(subdir-y); do \
		$(MAKE) -C $$dir clean; \
	done
