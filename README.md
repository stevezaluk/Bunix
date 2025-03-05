# Start
In order to compile we will have to use the 32-bit format,
Commands:

# Compile the source files in 32-bit mode
gcc -m32 -ffreestanding -c drivers/video/vga.c -o drivers/video/vga.o
gcc -m32 -ffreestanding -c drivers/keyboard/kb.c -o drivers/keyboard/kb.o
gcc -m32 -ffreestanding -c drivers/shell/shell.c -o drivers/shell/shell.o
gcc -m32 -ffreestanding -c init/main.c -o init/main.o
gcc -m32 -ffreestanding -c lib/string.c -o lib/string.o

# Link the object files
ld -m elf_i386 -T linker.ld -o kernel.elf \
    drivers/video/vga.o \
    drivers/keyboard/kb.o \
    drivers/shell/shell.o \
    init/main.o \
    lib/string.o

# Optionally, create a raw binary
objcopy -O binary kernel.elf kernel.bin

# Finally run the kernel 
qemu-system-x86_64 -kernel kernel.elf
