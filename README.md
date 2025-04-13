![Image](https://github.com/user-attachments/assets/41256855-7d63-486b-b965-2ceb2c4796fb)

Bunix all rights reserved.
--------------------------

Bunix is a Unix-Like Operating System Developed entirely from scratch by one person.
It focuses on performance stability and security.
Updates may slow down sometimes due to school and my personal life.

Processor supported currently is only i386 by now but x86_64 will be planned on to be added as well.
Only supports the BIOS currently UEFI will be planned on to be added as well.
Also works using UEFI CSM!

# Interested in Contributing?
In order to contribute to this Project you must:
Write Code (of course).
Test it and provide a screenshot that it's working.
(We will not accept contributions that haven't been tested).
Tell us specifically what that contribution is for.
Example for syscalls, keyboard drivers, etc.

# How to boot the system
To start the system you can compile the necessary dependencies using make, then make run to run it in qemu.

## Building from scratch
1. Install the required dependencies: `sudo apt-get update && sudo apt-get install qemu-system nasm mtool gcc-multilib`
2. Build: `make`
3. After building a `bunix.iso` file will be provided in the project's root directory

## Build and Run
1. Install the required dependencies: `sudo apt-get update && sudo apt-get install qemu-system nasm mtool gcc-multilib`
2. Execute: `make run`

# Future of Bunix
This is definitely Fun to Work on and Will improve over time!
We will have to look and see.

