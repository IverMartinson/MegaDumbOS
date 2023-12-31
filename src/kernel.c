char *videomemptr = (char*)0xb8000;
int ln = 0;
int cp = 10;

void reverse(char* str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        ++start;
        --end;
    }
}

char* itoa(int num, char* str, int base) {
    int i = 0;
    int isNegative = 0;

    // Handle 0 explicitly, otherwise empty string is printed
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // Handle negative numbers for bases other than 10
    if (num < 0 && base != 10) {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // Append negative sign for base 10
    if (isNegative && base == 10)
        str[i++] = '-';

    str[i] = '\0'; // Null-terminate string

    // Reverse the string
    reverse(str, i);

    return str;
}

void print(char* data) {
    unsigned int i = 0;
    unsigned int j = 0;

    // Loop to write the string to the video memory - each character with 0x02 attribute (green)
    while (data[j] != '\0' && i < 80 * 2) {
        videomemptr[i + 160 * ln + cp * 2] = data[j];
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
        "movb %%al, %0;"      // Store the key code in keybuffer
        : "=r" (keybuffer)
    );
    
    if(keybuffer==2)return '1';
    if(keybuffer==3)return '2';
    if(keybuffer==4)return '3';
    if(keybuffer==5)return '4';
    if(keybuffer==6)return '5';
    if(keybuffer==7)return '6';
    if(keybuffer==8)return '7';
    if(keybuffer==9)return '8';
    if(keybuffer==10)return '9';
    if(keybuffer==11)return '0';
    if(keybuffer==16)return 'q';
    if(keybuffer==17)return 'w';
    if(keybuffer==18)return 'e';
    if(keybuffer==19)return 'r';
    if(keybuffer==20)return 't';
    if(keybuffer==21)return 'y';
    if(keybuffer==22)return 'u';
    if(keybuffer==23)return 'i';
    if(keybuffer==24)return 'o';
    if(keybuffer==25)return 'p';
    if(keybuffer==26)return '[';
    if(keybuffer==27)return ']';
    if(keybuffer==43)return '\\';
    if(keybuffer==30)return 'a';
    if(keybuffer==31)return 's';
    if(keybuffer==32)return 'd';
    if(keybuffer==33)return 'f';
    if(keybuffer==34)return 'g';
    if(keybuffer==35)return 'h';
    if(keybuffer==36)return 'j';
    if(keybuffer==37)return 'k';
    if(keybuffer==38)return 'l';
    if(keybuffer==39)return ';';
    if(keybuffer==40)return '\'';
    if(keybuffer==44)return 'z';
    if(keybuffer==45)return 'x';
    if(keybuffer==46)return 'c';
    if(keybuffer==47)return 'v';
    if(keybuffer==48)return 'b';
    if(keybuffer==49)return 'n';
    if(keybuffer==50)return 'm';
    if(keybuffer==51)return ',';
    if(keybuffer==52)return '.';
    if(keybuffer==53)return '/';
    else return '\0';
}

void kernelMain() {
    clear();
    print("Welcome to MegaDumbOS: the Operating System of the Future");
    ln += 3;
    cp = 0;

    while (1) {
        char key = readKey();
        if (key != '\0'){
            print(&key);  // Print the pressed key
            ln -= 1;
            cp++;
        }
    }
}
