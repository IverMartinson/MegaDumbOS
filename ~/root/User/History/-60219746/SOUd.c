// Definitions   

    #define NUMBER_OF_COMMANDS 5

    // Timezone
    int Timezone = -10;

    // List of available commands
    char* listOfCommands[NUMBER_OF_COMMANDS] = {"help [displays commands]", "shutdown [shuts down computer]", "echo (string) [echos input]", "clear [clears screen]", "scroll (int) [scrolls screen]"};

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

    // RTC registers
    #define RTC_PORT 0x70
    #define CMOS_PORT 0x71

    // Datetime struct
    struct datetime {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
    };
    
    struct datetime currentTime;

    // Early definitions for printing variables
    void printInt(int);
    void print(char*);
    void printChar(char);
    void dPrintInt(int);
    void dPrint(char*);
    void dPrintChar(char);
    void setCursorPosition(uint16_t);



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

    // Function to convert a string to an integer
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

    // Define the input/output functions for x86 architecture
    static inline void outb(unsigned short port, unsigned char value) {
        asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
    }

    static inline unsigned char inb(unsigned short port) {
        unsigned char value;
        asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
        return value;
    }

    // BCD to Decimal conversion
    int bcd_to_decimal(int bcd) {
        return (bcd & 0xF) + ((bcd >> 4) * 10);
    }

    unsigned char get_RTC_register(int reg) {
        outb(RTC_PORT, reg);
        return inb(CMOS_PORT);
    }

    void getCurrentTime(struct datetime *dt) {
        dt->second = bcd_to_decimal(get_RTC_register(0x00));
        dt->minute = bcd_to_decimal(get_RTC_register(0x02));

        // Read the hour register and adjust for time zone
        int hour = bcd_to_decimal(get_RTC_register(0x04)) + Timezone;

        int pm = 1;
        if (hour >= 12) pm++;

        dt->hour = hour / pm; // Adjust for UTC+2

        dt->day = bcd_to_decimal(get_RTC_register(0x07));
        dt->month = bcd_to_decimal(get_RTC_register(0x08));
        dt->year = bcd_to_decimal(get_RTC_register(0x09));
    }


    // Function to read a key from the keyboard
    char readKey() {
        unsigned char keyBuffer;

        // Characters
        asm volatile (
            "waitForKey:\n"
            "call updateDebug\n"
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

        // Arrow keys
        if (keyBuffer == 0xE0) {
            // Escape code for extended keys, read the next byte
            asm volatile (
                "waitForArrowKey:\n"
                "call updateDebug\n"
                "inb $0x64, %0\n"
                "testb $0x01, %0\n"
                "jz waitForArrowKey\n"
                "inb $0x60, %0\n"
                : "=a" (keyBuffer)
                : 
                : 
            );

            switch (keyBuffer) {
                case 72: return -1; // Up arrow
                case 80: return -2; // Down arrow
                case 75: return -3; // Left arrow
                case 77: return -4; // Right arrow
            }
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

    // Print debug stuff at the bottom of the screen
    void updateDebug(){
        int i = cp;
        int j = ln;

        cp = 0;
        ln = 24;

        dPrint("cp: ");
        dPrintInt(i);
        dPrint("  ln: ");
        dPrintInt(j);

        getCurrentTime(&currentTime);

        int timeOffset = 0;

        if (currentTime.hour >= 10) timeOffset++;
        if (currentTime.minute >= 10) timeOffset++;
        if (currentTime.second >= 10) timeOffset++;

        int dateOffset = 0;

        if (currentTime.month >= 10) dateOffset++;
        if (currentTime.day >= 10) dateOffset++;

        cp = 57 - timeOffset - dateOffset;
            
        dPrint("   ");
        dPrintInt(currentTime.hour);
        dPrintChar(':');
        dPrintInt(currentTime.minute);
        dPrintChar(':');
        dPrintInt(currentTime.second);

        cp = 71 - dateOffset;
            
        dPrint("   ");            
        dPrintInt(currentTime.month);
        dPrintChar('/');
        dPrintInt(currentTime.day);
        dPrintChar('/');
        dPrintInt(currentTime.year);

        cp = i;
        ln = j;

        setCursorPosition(ln*80 + cp);
    }

    // Scroll function
    void scrollScreen(int lines){
        for (int pass = 0; pass < lines; pass++)
            for (int line=0; line < 80; line++)
                for (int cp = 0; cp < 80; cp++){
                    videoMemPtr[160 * (line - 1) + cp * 2] = videoMemPtr[160 * line + cp * 2];
                    videoMemPtr[160 * (line - 1) + cp * 2 + 1] = videoMemPtr[160 * line + cp * 2 + 1];
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
        if (cp >= 80){
            cp = 0;
            ln++;
        }
        
        if (ln >= 24) {
            scrollScreen(1);
            ln = 23;
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

    // Function to print a single character to the bottom of the screen
    void dPrintChar(char data) {
        videoMemPtr[160 * ln + cp * 2] = data;
        videoMemPtr[160 * ln + cp * 2 + 1] = 0x02;
        cp += 1;

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

    // Function to print a string to the bottom of the screen
    void dPrint(char* data) {
        unsigned int j = 0;

        while (data[j] != '\0') {
            dPrintChar(data[j]);

            ++j;
        }

        return;
    }

    // Function to print an integer to the screen
    void printInt(int data) {
        char buffer[20];
        intToString(data, buffer, 10);
        print(buffer);
    }

    // Function to print an integer to the bottom of the screen
    void dPrintInt(int data) {
        char buffer[20];
        intToString(data, buffer, 10);
        dPrint(buffer);
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
                if (j < NUMBER_OF_COMMANDS - 1) printChar('\n');
            }
        }

        // shutdown command
        else if (cstrcmp(command[0], "shutdown") == 0){
            print("[INFO] Shutting down...");

            shutdown();
        } 

        // Echo command
        else if (cstrcmp(command[0], "echo") == 0){
            print(command[1]);
            printChar('\n');
        }

        // clear screen command
        else if (cstrcmp(command[0], "clear") == 0){
            clearScreen();
        
            ln = -1;
        } 

        // scroll command
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

            else if (key < 0){
                switch (key){
                    case -1: print("up"); break;
                    case -2: print("down"); break;
                    case -3: print("left"); break;
                    case -4: print("right"); break;
                }
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