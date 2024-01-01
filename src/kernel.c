typedef unsigned int size_t;

char *videoMemPtr = (char*)0xb8000;
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

void printChar(char data) {
    if (data == '\n') {
        ln++;
        cp = 0;
    }

    videoMemPtr[160 * ln + cp * 2] = data;
    videoMemPtr[160 * ln + cp * 2 + 1] = 0x02;
    cp += 1;

    return;
}

void print(char* data) {
    unsigned int j = 0;

    // Loop to write the string to the video memory - each character with 0x02 attribute (green)
    while (data[j] != '\0') {
        if (data[j] == '\n') {
            ln++;
            cp = -1;
        }

        else {
            videoMemPtr[160 * ln + cp * 2] = data[j];
            videoMemPtr[160 * ln + cp * 2 + 1] = 0x02;
        }

        ++j;
        cp += 1;
    }

    return;
}

void printInt(int data) {
    char buffer[20];  // Assuming the maximum number of digits in the number won't exceed 20

    // Convert the number to a string
    itoa(data, buffer, 10);

    print(buffer);
}

void clearScreen() {
    unsigned int i = 0;

    // Clear screen
    for (i = 0; i < 80 * 25 * 2; i += 2) {
        videoMemPtr[i] = ' ';
        videoMemPtr[i + 1] = 0x02;
    }

    return;
}

char readKey() {
    char keyBuffer;

    asm volatile (
        "waitForKey:\n"
        "inb $0x64, %0\n"   // Read status register from the keyboard controller
        "testb $0x01, %0\n" // Check if the output buffer is full
        "jz waitForKey\n" // If not, keep waiting
        "inb $0x60, %0\n"   // Read the key code from the data buffer
        : "=a" (keyBuffer)
        : // no input operands
        : // no clobbered registers
    );

    // Check if the key is a valid ASCII character
    if (keyBuffer >= 2 && keyBuffer <= 57) {
        char keyMap[] = {
            '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
            '\0', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
            '\n', '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
            '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '\0', '\0', ' '
        };

        return keyMap[keyBuffer];
    }

    return '\0';
}

char* cstrncpy(char* dest, const char* src, size_t n) {
    char* original_dest = dest;

    // Copy up to n characters from src to dest
    while (n > 0 && *src != '\0') {
        *dest = *src;
        dest++;
        src++;
        n--;
    }

    // Pad with null bytes if necessary
    while (n > 0) {
        *dest = '\0';
        dest++;
        n--;
    }

    return original_dest;
}

void kernelMain() {
    clearScreen();
    print("Welcome to MegaDumbOS: the Operating System of the Future");
    ln += 3;
    cp = 0;

    char currentToken[20 + 1] = {'\0'};
    char command[10][20 + 1] = {};
    int token = -1;

    // cmd prompt
    print("=> ");

    while (1) {
        char key = readKey();

        if (key == '\n') {
            token++;
            
            cstrncpy(command[token], currentToken, 20);
            command[token][20] = '\0';
            currentToken[0] = '\0';

            print("\n ");
            print(command[0]);
            print("\n ");
            print(command[1]);
            print("\n ");
            print(command[2]);
            print("\n ");
            print(command[3]);

            // reset command
            for (int i = 0; i <= token; i++) {
                command[i][0] = '\0';
            }

            token = -1;
            print("\n=> ");
        }

        else if (key != '\0') {
            int i = 0;
            while (currentToken[i] != '\0') {
                i++;
            }

            // Add the new character to the end
            currentToken[i] = key;

            // Add the null terminator
            currentToken[i + 1] = '\0';

            printChar(key);
        }

        if (key == ' ') {
            token++;
            
            cstrncpy(command[token], currentToken, 20);
            command[token][20] = '\0';

            currentToken[0] = '\0';  // Reset the currentToken
        }
    }
}
