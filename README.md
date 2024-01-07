# MegaDumbOS: the Operating System of the Future

## OS

### Language
The MegaDumbOS kernel is written in C, with a kernel entry in x86 ASM.

### Bootloader
The OS's bootloader is GRUB.

### CLI Documentation

#### Help Menu
The help menu shows the list of commands that are available, as well as their functions and inputs. It is formatted like this:
* `command` (`input type of command`) [`function of command`]

#### Input Types
The CLI currently supports English letters, numbers, and the symbols `` ` ``, `-`, `=`, `[`, `]`, `\`, `;`, `'`, `,`, `.` and `/`.

#### How to Format String Inputs
If a command requires a string as an input and
* Has no spaces, then no formatting is required.
* Has spaces, then wrap the input in `` ` ``. E.g., ``echo `this is a test` `` would output `this is a test`, but `echo this is a test` would output `this`.

## Makefile

### Function
The makefile compiles all of the code for the OS into an ISO, outputting into the build folder for ease of access.

### Requirements
To successfully run the makefile, ensure you have the following dependencies installed: nasm, gcc, ld, and grub-mkrescue. If not installed, run `requirements.sh` to install them.

## Credits

### Code
Most of the code is written by me. ChatGPT made the cstrncpy and cstrcmp functions, as well as the kernel entry, which have been refactored to fit in the code.

### Comments
To enhance code understanding, almost all comments have been written or rewritten by ChatGPT for clarity.

### Resources
* [oswiki](https://wiki.osdev.org)
* [wikipedia](https://www.wikipedia.org/)
* [reddit](https://www.reddit.com/)
* Other various forums
