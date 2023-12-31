char *videomemptr = (char*)0xb8000;

int ln = 0;

void print(char* string){
    unsigned int i = 0;
    unsigned int j = 0;

    //loop to write the string to the video memory - each character with 0x02 attribute(green)
    while(string[j] != '\0')
    {
        videomemptr[i+160*ln] = string[j];
        videomemptr[i+160*ln+1] = 0x02;
        ++j;
        i = i+2;
    }

    ln++;

    return;
}  

void clear(){
    unsigned int i = 0;
    unsigned int j = 0;
            
    // clear screen
    while(j < 80 * 25 * 2) 
    {
        videomemptr[j] = ' ';
        videomemptr[j+1] = 0x02; 
        j = j+2;
    }
    j = 0;

    return;
}

char readChar() {
    char input;
    asm volatile(
        "sti\n"              // Disable interrupts
        "int $0x16\n"        // Invoke interrupt 0x16 (BIOS keyboard input)
        "cli\n"              // Enable interrupts
        : "=a" (input)
        : "a" (0x00)
    );
    return input;
}

void kernelMain() {
    clear();
    print("Welcome to MegaDumbOS: the operating system of the future");
    print("print test");
    print("Hello, World!");
}