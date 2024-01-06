// Definitions    

    #define NUMBER_OF_COMMANDS 4

    // List of available commands
    char* listOfCommands[NUMBER_OF_COMMANDS] = {"help [displays commands]\0", "shutdown [shuts down computer]\0", "clear [clears screen]\0", "scroll (int) [scrolls screen]"};

    // Define size_t as an alias for unsigned int
    typedef unsigned int size_t;

    // Pointer to the video memory
    char *videoMemPtr = (char*)0xb8000;

    // Current line and column positions
    int ln = 0;
    int cp = 0;

    // Define uint16_t and uint8_t as aliases for unsigned short and unsigned char
    typedef unsigned short uint16_t;
    typedef unsigned char uint8_t;

    // VGA ports and controller register indices
    #define VGA_PORT_CTRL 0x3D4
    #define VGA_PORT_DATA 0x3D5
    #define VGA_REG_CURSOR_HIGH 0x0E
    #define VGA_REG_CURSOR_LOW  0x0F

    // Define the address for the ACPI shutdown register
    #define ACPI_SHUTDOWN_REGISTER 0x604

    void printInt();



// String Functions

    // Function to reverse a string
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

    // Function to convert an integer to a string
    char* intToString(int num, char* str, int base) {
        int i = 0;
        int isNegative = 0;

        if (num == 0) {
            str[i++] = '0';
            str[i] = '\0';
            return str;
        }

        if (num < 0 && base != 10) {
            isNegative = 1;
            num = -num;
        }

        while (num != 0) {
            int rem = num % base;
            str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
            num = num / base;
        }

        if (isNegative && base == 10)
            str[i++] = '-';

        str[i] = '\0';
        reverse(str, i);

        return str;
    }

    int stringToInt(char* str){
        int final = 0;
     
        int length = 0;
        int power = 1;

        while(str[length] != '\0') {
            length++;
            power *= 10;
        }

        power /= 10;

        for (int i = 0; i < length; i++){
            if (str[i] < 48 || str[i] > 57) {
                print("[ERROR] Number of lines is invalid (must be a positive integer)");
            
                return -1;
            }

            final += (str[i] - 48) * power;
            power /= 10;
        }

        return final;
    }

    // Function to compare two strings
    int cstrcmp(const char* str1, const char* str2) {
        while (*str1 && (*str1 == *str2)) {
            str1++;
            str2++;
        }

        return *(unsigned char*)str1 - *(unsigned char*)str2;
    }

    // Function to copy a string with specified length
    char* cstrncpy(char* dest, const char* src, size_t n) {
        char* original_dest = dest;

        while (n > 0 && *src != '\0') {
            *dest = *src;
            dest++;
            src++;
            n--;
        }

        while (n > 0) {
            *dest = '\0';
            dest++;
            n--;
        }

        return original_dest;
    }



// System Functions

    // Function to write a byte to a port
    void outb(uint16_t port, uint8_t value) {
        asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
    }

    // Function to read a key from the keyboard
    char readKey() {
        char keyBuffer;

        asm volatile (
            "waitForKey:\n"
            "inb $0x64, %0\n"
            "testb $0x01, %0\n"
            "jz waitForKey\n"
            "inb $0x60, %0\n"
            : "=a" (keyBuffer)
            : 
            : 
        );

        if (keyBuffer >= 2 && keyBuffer <= 57) {
            char keyMap[] = {
                '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
                '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
                '\n', '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
                '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '\0', '\0', ' '
            };

            return keyMap[keyBuffer];
        }

        return '\0';
    }

    // Shutdown computer
    void shutdown(){
        outb(ACPI_SHUTDOWN_REGISTER, 0x02);

        asm volatile ("mov $0, %eax\n int $0x16");
        asm volatile ("hlt");
    }



// Kernel Functions

    // Scroll function
    void scrollScreen(int lines){
        for (int pass = 0; pass < lines; pass++)
            for (int line=0; line < 80; line++){
                for (int cp = 0; cp < 80; cp++){
                    videoMemPtr[160 * (line - 1) + cp * 2] = videoMemPtr[160 * line + cp * 2];
                    videoMemPtr[160 * (line - 1) + cp * 2 + 1] = videoMemPtr[160 * line + cp * 2 + 1];
                }
            }

        ln -= lines;
    }

    // Function to set the cursor position on the screen
    void setCursorPosition(uint16_t position) {
        outb(VGA_PORT_CTRL, VGA_REG_CURSOR_HIGH);
        outb(VGA_PORT_DATA, (position >> 8) & 0xFF);
        outb(VGA_PORT_CTRL, VGA_REG_CURSOR_LOW);
        outb(VGA_PORT_DATA, position & 0xFF);
    }

    // Function to print a single character to the screen
    void printChar(char data) {
        if (cp > 80){
            cp = 0;
            ln++;
        }
        
        if (ln >= 25) {
            scrollScreen(1);
            ln = 24;
        }
        
        if (data == '\n') {
            ln++;
            cp = 0;
            return;
        }

        videoMemPtr[160 * ln + cp * 2] = data;
        videoMemPtr[160 * ln + cp * 2 + 1] = 0x02;
        cp += 1;

        setCursorPosition(ln*80 + cp);

        return;
    }

    // Function to print a string to the screen
    void print(char* data) {
        unsigned int j = 0;

        while (data[j] != '\0') {
            printChar(data[j]);

            ++j;
        }

        setCursorPosition(ln*80 + cp);

        return;
    }

    // Function to print an integer to the screen
    void printInt(int data) {
        char buffer[20];
        intToString(data, buffer, 10);
        print(buffer);
    }

    // Function to clear the screen
    void clearScreen() {
        unsigned int i = 0;

        for (i = 0; i < 80 * 25 * 2; i += 2) {
            videoMemPtr[i] = ' ';
            videoMemPtr[i + 1] = 0x02;
        }

        ln = 0;
        cp = 0;

        return;
    }

    // Function to check and execute commands
    void checkCommand(char command[10][21]){        
        // help command
        if (cstrcmp(command[0], "help") == 0){
            for (int j = 0; j < NUMBER_OF_COMMANDS; j++) {
                print(listOfCommands[j]);
                if (j < NUMBER_OF_COMMANDS - 1) print("\n ");
            }
        }

        // shutdown command
        else if (cstrcmp(command[0], "shutdown") == 0){
            print("[INFO] Shutting down...");

            shutdown();
        } 

        // clear screen commnad
        else if (cstrcmp(command[0], "clear") == 0){
            clearScreen();
        
            ln = -1;
    
        } 

        else if (cstrcmp(command[0], "scroll") == 0){
            if (command[1][0] == 0) print("[ERROR] Number of lines not specified");
            if (command[1][0] == 48) print("[ERROR] Number of lines cannot start with or be zero");

            int scrollValue = stringToInt(command[1]);

            if (scrollValue == -1) return;

            scrollScreen(scrollValue);

            ln--;
        }

        else{
            print("[ERROR] Command '");
            print(command[0]);
            print("' not found. Try 'help' for a list of commands");
        }

        return;
    }



    // Main kernel function

    void kernelMain() {
        clearScreen();

        ln = 1;
        cp = 11;

        print("Welcome to MegaDumbOS: the Operating System of the Future!\n");

        ln += 3;

        char currentToken[21] = {'\0'};
        char currentCommand[100] = {'\0'};
        char command[10][21] = {};
        int token = 0;
        int canDelete = 0;
        int ignoreSpace = 0;

        // Command prompt
        print("=> ");

        while (1) {
            char key = readKey();

            if (key == '\n'){
                int i = 0;
                while (currentCommand[i] != '\0') {
                    i++;
                }

                for (int j = 0; j < i; j++) {
                    int k = 0;
                    while (currentToken[k] != '\0') {
                        k++;
                    }

                    if (currentCommand[j] == '`') {
                        if (ignoreSpace == 0){
                            ignoreSpace = 1;
                        }

                        else{
                            ignoreSpace = 0;
                        }
                    }
                
                    else if (ignoreSpace == 0 && currentCommand[j] == ' ') {
                        currentToken[k] = currentCommand[j];
                        currentToken[k] = '\0';
                        cstrncpy(command[token], currentToken, 20);
                        currentToken[0] = '\0';
                        token++;
                        continue;
                    }

                    if (currentCommand[j] != '`'){
                        currentToken[k] = currentCommand[j];
                        currentToken[k + 1] = '\0';
                    }
                }

                cstrncpy(command[token], currentToken, 20);

                print("\n ");

                checkCommand(command);

                token = 0;
                currentToken[0] = '\0';
                currentCommand[0] = '\0';

                for (int i = 0; i <= token; i++) {
                    command[i][0] = '\0';
                }

                canDelete = 0;
                print("\n=> ");
            }

            else if (key == '\b'){
                if (canDelete <= 0) continue;
            
                int i = 0;
                while (currentCommand[i] != '\0') {
                    i++;
                }

                currentCommand[i - 1] = '\0';

                cp--;

                printChar('\0');
        
                cp--;
                setCursorPosition(ln*80 + cp);

                canDelete--;
            }

            else if (key != '\0'){
                int i = 0;
                while (currentCommand[i] != '\0') {
                    i++;
                }

                currentCommand[i] = key;
                currentCommand[i + 1] = '\0';

                canDelete++;

                printChar(key);
            }
        }

        return;
    }
