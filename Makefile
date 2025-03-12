# Compiler and linker settings
CC = gcc
LD = ld
CFLAGS = -m32 -ffreestanding -fno-stack-protector -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld
ISO_TOOL = grub-mkrescue
QEMU = qemu-system-x86_64

# Directories
SRC_DIR = .
OBJ_DIR = obj
ISO_DIR = isodir
BOOT_DIR = $(ISO_DIR)/boot
BIN_DIR = bin
INCLUDE_DIR = include

# Source files
SRCS = \
    init/main.c \
    drivers/video/vga.c \
    drivers/keyboard/kb.c \
    drivers/shell/shell.c \
    lib/libc/string/string.c \
    sys/syscall/syscall.c \
    sys/rtc/rtc.c  # Added RTC source file

# Bin folder source files
BIN_SRCS = \
    $(BIN_DIR)/clear.c \
    $(BIN_DIR)/echo.c \
    $(BIN_DIR)/cpuinfo.c \
    $(BIN_DIR)/reboot.c \
    $(BIN_DIR)/shutdown.c \
    $(BIN_DIR)/time.c \
    $(BIN_DIR)/date.c \
    $(BIN_DIR)/uptime.c  # Added uptime.c

# Object files
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))
BIN_OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(BIN_SRCS))

# Output files
KERNEL_ELF = kernel.elf
ISO_IMAGE = bunix.iso

# Default target
all: $(ISO_IMAGE)

# Rule to compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Rule to compile bin folder source files into object files
$(OBJ_DIR)/$(BIN_DIR)/%.o: $(BIN_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Rule to link object files into the kernel ELF
$(KERNEL_ELF): $(OBJS) $(BIN_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	@mkdir -p $(BOOT_DIR)
	cp $(KERNEL_ELF) $(BOOT_DIR)/$(KERNEL_ELF)

# Rule to create the ISO image
$(ISO_IMAGE): $(KERNEL_ELF)
	$(ISO_TOOL) -o $@ $(ISO_DIR)

# Rule to run the ISO in QEMU
run: $(ISO_IMAGE)
	$(QEMU) -cdrom $(ISO_IMAGE)

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(KERNEL_ELF) $(ISO_IMAGE) $(BOOT_DIR)/$(KERNEL_ELF)

.PHONY: all clean run
