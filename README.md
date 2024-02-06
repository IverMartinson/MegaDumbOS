# MegaDumbOS: the Operating System of the Future

## OS  

### Language  

&emsp;The MegaDumbOS kernel is written in C, with a kernel entry in x86 ASM.  

### Bootloader
&emsp;The OS's bootloader is GRUB.

### CLI Documentation

#### Help Menu
&emsp;The help menu shows the list of commands that are available, as well as their functions and inputs. It is formatted like this: `command` (`input type of command`) [`function of command`] (if no input type is shown, no input is required)

#### Input Types
&emsp;The CLI currently supports English letters, numbers, and the symbols `` ` ``, `-`, `=`, `[`, `]`, `\`, `;`, `'`, `,`, `.`, and `/`.

#### How to Format String Inputs
&emsp;If a command requires a string as an input and  
&emsp; * Has no spaces, then no formatting is required.  
&emsp; * Has spaces, then wrap the input in `` ` ` ``.
  
&emsp;E.g, ``echo `hello world` `` would output `hello world`, but `echo hello world` would output `hello`.  
&emsp;Currently, string inputs cannot have the `` ` `` character in them.

## Credits

### Comments
&emsp;Almost all comments have been written or rewritten by ChatGPT for clarity.

### Resources
&emsp; * [oswiki](https://wiki.osdev.org)  
&emsp; * [wikipedia](https://www.wikipedia.org/)  
&emsp; * [reddit](https://www.reddit.com/)  
&emsp; * Other various forums

# How To Run Or Install MegaDumbOS On Linux And Windows

## Running With Emulator (QEMU) On Linux

### 1: Makefile

#### Run The Makefile
&emsp;The first step is to run the makefile. Run it with `sudo make all`. Be sure to be in the same folder.

#### Function
&emsp;The makefile compiles all of the code for the OS into an ISO, outputting into the build folder for ease of access.

#### Requirements
&emsp;To successfully run the makefile, ensure you have the following dependencies installed: nasm, gcc, ld, and grub-mkrescue. If not installed, run `requirements.sh` to install them.

### 2: Run.sh

#### Execute Run.sh
&emsp;To execute run.sh, make sure you have QEMU installed and the latest build of MegaDumbOS. Run it with `./run.sh`. If you get any errors about permissions, run `chmod 777 run.sh`.

## Installing MegaDumbOS On Linux

### 1: Makefile
&emsp;The first step is to run the makefile. Run it with `make linux`. Be sure to be in the same folder.

### 2: Lsblk Command
&emsp;Use lsblk to find out which media you would like to install to. Be ABSOLUTLY SURE you choose the correct one. Choosing the wrong disk may result in data loss.

### 3: DD Command
&emsp;Finally, use the dd command to install the os. Run `sudo dd bs=4M if=build/output.iso of=/dev/YOURDEVICEHERE status=progress`. Replace YOURDEVICEHERE with the correct device. I.E, sda, sdb, nvme0n1p1, etc.

### 4: Rename The Drive (optional)
&emsp;Run `sudo umount /dev/YOURDEVICEHERE` to unmount the drive. Then run `sudo dosfslabel /dev/YOURDEVICEHERE MegaDumbOS`. Replace YOURDEVICEHERE with the correct device.

### 5: Eject The Drive
&emsp;Use `eject /dev/YOURDEVICEHERE` to eject the drive. Replace YOURDEVICEHERE with the correct device. 

### 6: Boot Into MegaDumbOS
&emsp;Finally, restart the computer. Then, boot into the BOIS or boot menu. It may vary from manufacturer, but usually when you see the manufacturer logo, you should press F2, F12 or DEL. Then go to the boot section. You should see your drive. Then select it and boot it.

# How To Run Or Install MegaDumbOS On Windows

## Running With Emulator (VirtualBox) On Windows

### Preface
&emsp;Please note that some features of MegaDumbOS may not function properly on VirtualBox, as it was designed for Linux on QEMU.

### 1: Make.bat
&emsp;The first step is to run make.bat and build MegaDumbOS. Run it with `make.bat`. Be sure to be in the same folder.

### 2: Install VirtualBox
&emsp;Go to [the VirtualBox downloads page](https://www.virtualbox.org/wiki/Downloads) and click `Windows Hosts`. Then run the exe.

### 3: Use VirtualBox
&emsp;Open VirtualBox. Click the new button. For ISO select the newley built `output.iso` in the build folder. Follow the rest of the steps.

### 4: Run MegaDumbOS
&emsp;After creating the virtual machine, double-click on the VM's entry in the machine list in VirtualBox Manager. Select the VM's entry in the machine list in VirtualBox Manager, and click Start in the toolbar the top of the window. 

## Installing MegaDumbOS On Windows

### 1: Make.bat
&emsp;The first step is to run make.bat and build MegaDumbOS. Run it with `make.bat`. Be sure to be in the same folder.

### 2: Download Rufus
&emsp;Go to [the Rufus download page](https://rufus.ie/downloads/) and download the latest version.

### 3: Run Rufus
&emsp;Run the exe that you downloaded. Click the `SELECT` button and select the newly built `output.iso`. Then, if it isn't allready selected, chose the correct device that you want MegaDumbOS on. Finally, click the start button.

### 4: Rename The Drive (optional)
&emsp;In file explorer, go to `This PC`. Right-click on the device MegaDumbOS is installed on. Click rename. Type in MegaDumbOS.

### 5: Eject The Drive
&emsp;In file explorer, go to `This PC`. Right-click on the device MegaDumbOS is installed on. Click eject.

### 4: Boot Into MegaDumbOS
&emsp;Finally, restart the computer. Then, boot into the BOIS or boot menu. It may vary from manufacturer, but usually when you see the manufacturer logo, you should press F2, F12 or DEL. Then go to the boot section. You should see your drive. Then select it and boot it.
