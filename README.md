# MegaDumbOS: the Operating System of the Future

## OS

### Language
The MegaDumbOS kernel is written in C, with a kernel entry in x86 ASM.

### Bootloader
The OS's bootloader is GRUB.

## Makefile

### Function
The makefile compiles all of the code for the OS into an ISO, outputting into the build folder for ease of access.

### Requirements
To successfully run the makefile, ensure you have the following dependencies installed: nasm, gcc, ld, and grub-mkrescue. If not installed, run `requirements.sh` to install them.

## Credits

### Code
While the majority of the code is authored by the project creator, credit is given to ChatGPT for the initial implementation of cstrncpy and cstrcmp functions, as well as the kernel entry, which have been refactored for optimization.

### Comments
To enhance code understanding, almost all comments have been written or rewritten by ChatGPT for clarity.

### Resources
* [oswiki](https://wiki.osdev.org) - A valuable resource for OS development.
* [wikipedia](https://www.wikipedia.org/) - General information on various OS-related topics.
* [reddit](https://www.reddit.com/) - Community insights and discussions on OS development.
* Other various forums - Additional sources contributing to the development knowledge base.
