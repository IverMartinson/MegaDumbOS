# MegaDumbOS: the Operating System of the Future


## OS

### Language
The MegaDumbOS kernel is writen in C, with a kernel entry in x86 ASM

### Bootloader
The OS's bootloader is GRUB.

## Makefile

### Function
The makefile compiles all of the code for the OS into an iso. It outputs into the build folder.

### Requirements
The makefile requires nasm, gcc, ld, and grub-mkrescue. 
Run run requirements.sh if not installed.

## Credits

### Code
All code is written by me, except for the cstrncpy and cstrcmp functions, and the kernel entry.
They were initially written by ChatGPT and refactored by me.

### Comments
Almost all comments are writen, or rewrote by ChatGPT for clarity.
