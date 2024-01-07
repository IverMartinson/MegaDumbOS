# MegaDumbOS: the Operating System of the Future

## OS  

### Language  

&emsp;The MegaDumbOS kernel is written in C, with a kernel entry in x86 ASM.  

### Bootloader
&emsp;The OS's bootloader is GRUB.

### CLI Documentation

#### Help Menu
&emsp;The help menu shows the list of commands that are available, as well as their functions and inputs. It is formatted like this:  
&emsp; * `command` (`input type of command`) [`function of command`]

#### Input Types
&emsp;The CLI currently supports English letters, numbers, and the symbols `` ` ``, `-`, `=`, `[`, `]`, `\`, `;`, `'`, `,`, `.` and `/`.

#### How to Format String Inputs
&emsp;If a command requires a string as an input and  
&emsp; * Has no spaces, then no formatting is required.  
&emsp; * Has spaces, then wrap the input in `` ` ` ``.
  
&emsp;E.g., ``echo `hello, world` `` would output `hello, world`, but `echo hello, world` would output `hello,`.  
&emsp;Currently, string inputs cannot have the `` ` `` character in them.

## Makefile

### Function
&emsp;The makefile compiles all of the code for the OS into an ISO, outputting into the build folder for ease of access.

### Requirements
&emsp;To successfully run the makefile, ensure you have the following dependencies installed: nasm, gcc, ld, and grub-mkrescue. If not installed, run `requirements.sh` to install them.

## Credits

### Code
&emsp;Most of the code is written by me. ChatGPT made around 3/4 of the conversion functions, and inb and outb functions, as well as the kernel entry, which I refactored to fit in the code.

### Comments
&emsp;To enhance code understanding, almost all comments have been written or rewritten by ChatGPT for clarity.

### Resources
&emsp; * [oswiki](https://wiki.osdev.org)  
&emsp; * [wikipedia](https://www.wikipedia.org/)  
&emsp; * [reddit](https://www.reddit.com/)  
&emsp; * Other various forums
