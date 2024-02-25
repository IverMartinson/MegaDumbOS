// Definitions ---------------------------------------------------------------------------------------------------------------------
            
    #define NUMBER_OF_COMMANDS 8

    // Timezone
    int TIMEZONE = -5;

    // List of available commands
    char* listOfCommands[NUMBER_OF_COMMANDS] = {"help [displays commands]", "shutdown [shuts down computer]", "echo (string(message)) [echos input]", "clear [clears screen]", "scroll (int(lines)) [scrolls screen]", "xtra (string(xtra command)) [runs xtra commands]", "read (int(sector)) (int(# of sectors)) [reads from the disk]", "write (int(sector)) (int(# of sectors)) (int(value)) [writes to the disk]"};

    // Define size_t as an alias for unsigned int
    typedef unsigned int size_t;

    // Pointer to the video memory
    char *videoMemPtr = (char*)0xb8000;

    // Current line and column positions
    int ln = 0;
    int cp = 0;

    // Define uint32_t, uint16_t, and uint8_t
    typedef unsigned int uint32_t;
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
    void clearScreen();

    // XTRA
    extern void xtraASM();
    extern void enableInterrupts();
    extern void readFromDisk(unsigned short *dataBuffer);

// Definitions ---------------------------------------------------------------------------------------------------------------------



// String Functions ----------------------------------------------------------------------------------------------------------------

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

// String Functions ----------------------------------------------------------------------------------------------------------------



// System Functions ----------------------------------------------------------------------------------------------------------------

    // Define the input/output functions for x86 architecture
    static inline void outb(unsigned short port, unsigned char value) {
        asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
    }

    static inline unsigned char inb(unsigned short port) {
        unsigned char value;
        asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
        return value;
    }

    static inline void outw(unsigned short port, unsigned short value) {
        asm volatile("outw %0, %1" : : "a"(value), "Nd"(port));
    }

    static inline unsigned short inw(unsigned short port) {
        unsigned short value;
        asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port));
        return value;
    }


    void diskDriver(char command, uint16_t buffer[], int startingSector, int numberOfSectors){
        if (command == 'r') {
            print("[INFO] Reading ");
            printInt(numberOfSectors);
            print(" sectors, starting at sector ");
            printInt(startingSector);
            printChar('\n');

            // make sure buffer is clear
            for (uint32_t j = 0; j < 256 * numberOfSectors; j++){
                buffer[j] = 0;
            }
                
            for (uint8_t i=0; i < numberOfSectors; i++) {
                outb(0x1F6, (0xE0 | ((startingSector >> 24) & 0x0F))); // Head/drive # port - send head/drive #
                outb(0x1F2, 1); // Sector count port - # of sectors to read/write
                outb(0x1F3, startingSector + i & 0xFF); // Sector number port / LBA low bits 0-7
                outb(0x1F4, ((startingSector >> 8) & 0xFF)); // Cylinder low port / LBA mid bits 8-15
                outb(0x1F5, ((startingSector >> 16) & 0xFF)); // Cylinder high port / LBA high bits 16-23           
                outb(0x1F7, 0x20); // Command port - send read/write command        
                
                if (i != numberOfSectors) printChar('\n');
                
                // Poll status port after reading 1 sector
                while (inb(0x1F7) & (1 << 7)) // Wait until BSY bit is clear
                    ;

                // Read 256 words from data port into memory
                for (uint32_t j = 0; j < 256; j++){
                    buffer[j + 256 * i] = inw(0x1F0);
                    //printInt(inw(0x1F0));
                    //printChar(' ');
                }

                //print("\n[INFO] Sector ");
                //printInt(i);
                //print(", error status ");
                //printInt(inb(0x1F1));

                // 400ns delay - Read alternate status register
                for (uint8_t k = 0; k < 4; k++)
                    inb(0x3F6);
            }

        } else if (command == 'w') {
            print("[INFO] Writing ");
            printInt(numberOfSectors);
            print(" sectors, starting at sector ");
            printInt(startingSector);

            for (uint8_t i=0; i < numberOfSectors; i++) {
                outb(0x1F6, (0xE0 | ((startingSector >> 24) & 0x0F))); // Head/drive # port - send head/drive #
                outb(0x1F2, 1); // Sector count port - # of sectors to read/write
                outb(0x1F3, startingSector + i & 0xFF); // Sector number port / LBA low bits 0-7
                outb(0x1F4, ((startingSector >> 8) & 0xFF)); // Cylinder low port / LBA mid bits 8-15
                outb(0x1F5, ((startingSector >> 16) & 0xFF)); // Cylinder high port / LBA high bits 16-23           
                outb(0x1F7, 0x30); // Command port - send read/write command    

                // Poll status port after reading 1 sector
                while (inb(0x1F7) & (1 << 7)) // Wait until BSY bit is clear
                    ;

                // Write 256 words from memory to data port
                for (uint32_t j = 0; j < 256; j++)
                    outw(0x1F0, buffer[j + 256 * i]);

                print("\n[INFO] Sector ");
                printInt(i);
                print(", error status ");
                printInt(inb(0x1F1));

                // 400ns delay - Read alternate status register
                for (uint8_t k = 0; k < 4; k++)
                    inb(0x3F6);
            }

            // Send cache flush command after write command is finished
            outb(0x1F7, 0xE7);

            // Wait until BSY bit is clear after cache flush
            while (inb(0x1F7) & (1 << 7))
                ;
        }
    }

    // Read from disk
    extern void readFromDisk();


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
        int hour = bcd_to_decimal(get_RTC_register(0x04)) + TIMEZONE;

        if (hour >= 12) hour -= 12;
        if (hour <= 0) hour = 12 + hour;

        dt->hour = hour;

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

        // Special keys
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

            char keyMap[] = {
                -1, '\0', '\0', -3, '\0', -4, '\0', '\0', -2, '\0', '\0', -5
            };

            // Adjust index based on the escape code and handle out-of-range values
            int index = keyBuffer - 72;
            if (index >= 0 && index < sizeof(keyMap) / sizeof(keyMap[0])) {
                return keyMap[index];
            } else {
                // Handle out-of-range values, return a default value or an error code
                return '\0';
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

// System Functions ----------------------------------------------------------------------------------------------------------------



// Kernel Functions ----------------------------------------------------------------------------------------------------------------

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
        dPrint("   ");

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
        int i = ln;

        if (i - lines <= 0) {
            clearScreen();
            ln = 0;
            cp = 0;
            return;
        }

        for (int pass=0; pass < lines; pass++){
            for (int line=0; line < 24; line++){
                for (int cp = 0; cp < 80; cp++){
                    videoMemPtr[160 * (line - 1) + cp * 2] = videoMemPtr[160 * line + cp * 2];
                    videoMemPtr[160 * (line - 1) + cp * 2 + 1] = videoMemPtr[160 * line + cp * 2 + 1];
                }
            }
        }

        cp = 0;
        ln = 23;
        print("                                                                                ");

        ln = i - lines;

        if (ln < 0) ln = 0;
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
        
        if (data == '\n') {
            ln++;
            cp = 0;
            return;
        }

        if (ln >= 24) {
            scrollScreen(ln - 23);
            cp = 0;
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
        char buffer[10];
        intToString(data, buffer, 10);
        print(buffer);
    }

    // Function to print an integer to the bottom of the screen
    void dPrintInt(int data) {
        char buffer[10];
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
            if (command[1][0] == 0) print("[ERROR] Number of lines not specified\n");
            if (command[1][0] == 48) print("[ERROR] Number of lines cannot start with or be zero\n");

            int scrollValue = stringToInt(command[1]);

            if (scrollValue == -1) return;

            scrollScreen(scrollValue);

            ln--;
        }

        // write to disk command
        else if (cstrcmp(command[0], "write") == 0){
            int sectorSize = 256 * stringToInt(command[2]);

            uint16_t dataBuffer[sectorSize];

            if (cstrcmp(command[3], "") == 0){
                for (uint32_t j = 0; j < sectorSize; j++){
                    dataBuffer[j] = j;
                }
            } else {
                for (uint32_t j = 0; j < sectorSize; j++){
                    dataBuffer[j] = stringToInt(command[3]);
                }
            }

            for (int i=0; i < sectorSize; i++){
                //printInt(dataBuffer[i]);
                //printChar(' ');
            }

            diskDriver('w', &dataBuffer[0], stringToInt(command[1]), stringToInt(command[2]));
        
            print("\n[INFO] Done");
        }

        // read from disk command
        else if (cstrcmp(command[0], "read") == 0){
            int sectorSize = 256 * stringToInt(command[2]);

            uint16_t dataBuffer[sectorSize];

            diskDriver('r', &dataBuffer[0], stringToInt(command[1]), stringToInt(command[2]));

            printChar('\n');

            for (int i=0; i < sectorSize; i++){
                printInt(dataBuffer[i]);
                printChar(' ');
            }
        
            print("\n[INFO] Done");
        }

        // XTRA
        else if (cstrcmp(command[0], "xtra") == 0){
            if (cstrcmp(command[1], "shutdown") == 0) {
                print("[INFO] Shutting down...");

                return;
            }
            else if (cstrcmp(command[1], "write") == 0) {
                print("[INFO] Writing to disk...");
                
                return;
            }
            else if (cstrcmp(command[1], "read") == 0) { 
                unsigned short dataBuffer[256];

                print("[INFO] Reading from disk...");

                readFromDisk(dataBuffer);

                clearScreen();

                for (int i=0; i < 256; i++){
                    printInt(dataBuffer[i]);
                    printChar(' ');
                }

                print("\n[INFO] Done");

                return;
            }
            else if (cstrcmp(command[1], "enableint") == 0) {
                print("[INFO] Enabling interrupts...");
                
                enableInterrupts();
                
                return;
            }
            else if (cstrcmp(command[1], "") == 0){
                print("[ERROR] No XTRA command specified\n");
                
                return;
            }
            
            print("[ERROR] Unknown XTRA command\n");
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
        char currentCommand[21] = {'\0'};
        char command[10][21] = {};
        int token = 0;
        int canDelete = 0;
        int ignoreSpace = 0;
        int arrowKeyOffset = 0;

        // Command prompt
        print("=> ");

        while (1) {
            print("     ");

            cp -= 5;

            char key = readKey();

            int i = 0;
            while (currentCommand[i] != '\0') {
                i++;
            }

            if (key == '\n'){
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

                printChar('\n');

                checkCommand(command);

                token = 0;
                currentToken[0] = '\0';
                currentCommand[0] = '\0';
                arrowKeyOffset = 0;

                for (int i = 0; i < 10; i++)
                    for (int j = 0; j < 21; j++)
                        command[i][j] = '\0';

                canDelete = 0;
                print("\n=> ");
            }

            else if (key == '\b'){
                if (canDelete <= 0) continue;

                cp--;

                int offset = i - arrowKeyOffset;
                for (int j = offset - 1; j <= i; j++){
                    currentCommand[j] = currentCommand[j + 1];
                    printChar(currentCommand[j]);
                }

                print("   ");

                cp -= arrowKeyOffset + 5;

                canDelete--;
            }

            else if (key == -5) {
                if (arrowKeyOffset > i || arrowKeyOffset <= 0) {
                    continue;
                }

                int offset = i - arrowKeyOffset;
                for (int j = offset; j <= i; j++) {
                    currentCommand[j] = currentCommand[j + 1];
                    printChar(currentCommand[j]);
                }

                print("   ");

                cp -= arrowKeyOffset + 4;
                arrowKeyOffset--;
            }

            else if (key < 0){
                switch (key){
                    case -1: print("up"); break;
                    case -2: print("down"); break;
                    case -3: if (arrowKeyOffset >= i) {break;} arrowKeyOffset++; cp--; canDelete--; setCursorPosition(ln*80 + cp); break; // Left
                    case -4: if (arrowKeyOffset <= 0) {break;} arrowKeyOffset--; cp++; canDelete++; setCursorPosition(ln*80 + cp); break; // Right
                }
            }

            else if (key != '\0'){
                cp++;

                int offset = i - arrowKeyOffset;
                for (int j = i; j >= offset; j--){
                    if (currentCommand[j] != '\0') printChar(currentCommand[j]);
                    currentCommand[j + 1] = currentCommand[j];
                }

                cp -= arrowKeyOffset + 1;

                currentCommand[offset] = key;

                canDelete++;

                printChar(key);
            }
        }

        return;
    }

// Kernel Functions ----------------------------------------------------------------------------------------------------------------
