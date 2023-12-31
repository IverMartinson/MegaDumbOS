char *videomemptr = (char*)0xb8000;
int ln = 0;
int cp = 10;

void print(char* string) {
    unsigned int i = 0;
    unsigned int j = 0;

    // loop to write the string to the video memory - each character with 0x02 attribute (green)
    while (string[j] != '\0' && i < 80 * 2) {
        videomemptr[i + 160 * ln + cp * 2] = string[j];
        videomemptr[i + 160 * ln + 1] = 0x02;
        ++j;
        i = i + 2;
    }

    ln++;

    return;
}

void clear() {
    unsigned int i = 0;

    // Clear screen
    for (i = 0; i < 80 * 25 * 2; i += 2) {
        videomemptr[i] = ' ';
        videomemptr[i + 1] = 0x02;
    }

    return;
}

char readKey() {
    char keybuffer;

    asm volatile (
        "wait_for_key:"
        "inb $0x64, %%al;"     // Read status register from keyboard controller
        "testb $0x01, %%al;"   // Check if the output buffer is full
        "jz wait_for_key;"     // If not, keep waiting
        "inb $0x60, %%al;"     // Read key code from data buffer
        "movb %%al, %0;"       // Store the key code in keybuffer
        : "=r" (keybuffer)
    );

    return keybuffer;
}

void kernelMain() {
    clear();
    print("Welcome to MegaDumbOS: the Operating System of the Future");
    ln += 3;
    cp = 0;

    while (1) {
        char key = readKey();
        print(&key);  // Print the pressed key
        ln -= 1;
        cp++;
    }
}
