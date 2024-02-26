#include "libs/standard_vartype.h"
#include "libs/standard_io.h"
#include "libs/standard_math.h"
#include "libs/standard_string.h"

// definitions ---------------------------------------------------------------------------------------------------------------------
            
    #define NUMBER_OF_COMMANDS 9

    // timezone
    int TIMEZONE = 0;

    // list of available commands
    char* listOfCommands[NUMBER_OF_COMMANDS] = {"help [displays commands]", "shutdown [shuts down computer]", "echo (string(message)) [echos input]", "clear [clears screen]", "scroll (int(lines)) [scrolls screen]", "xtra (string(xtra command)) [runs xtra commands]", "timezone (int(UTC offset) [changes the timezone])", "read (int(sector)) (int(# of sectors)) [reads from the disk]", "write (int(sector)) (int(# of sectors)) (int(value)) [writes to the disk]"};

    // define the address for the ACPI shutdown register
    #define ACPI_SHUTDOWN_REGISTER 0x604

    // RTC registers
    #define RTC_PORT 0x70
    #define CMOS_PORT 0x71

    // datetime struct
    struct datetime {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
    };
    
    struct datetime currentTime;

    // XTRA
    extern void xtraASM();
    extern void enableInterrupts();
    extern void readFromDisk(unsigned short *dataBuffer);

// Definitions ---------------------------------------------------------------------------------------------------------------------



// System Functions ----------------------------------------------------------------------------------------------------------------

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

        // read the hour register and adjust for time zone
        int hour = bcd_to_decimal(get_RTC_register(0x04));

        int dayOffset = 0;

        hour += TIMEZONE;

        if (hour >= 24) {
            hour -= 24;
            dayOffset++;
        }

        if (hour < 0) {
            hour += 24;
            dayOffset--;
        }

        dt->hour = hour;
        dt->day = bcd_to_decimal(get_RTC_register(0x07)) + dayOffset;
        dt->month = bcd_to_decimal(get_RTC_register(0x08));
        dt->year = bcd_to_decimal(get_RTC_register(0x09)); 
    }


    // Function to read a key from the keyboard
    char readKey() {
        unsigned char keyBuffer;

        // characters
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

        // special keys
        if (keyBuffer == 0xE0) {
            // escape code for extended keys, read the next byte
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

            // adjust index based on the escape code and handle out-of-range values
            int index = keyBuffer - 72;
            if (index >= 0 && index < sizeof(keyMap) / sizeof(keyMap[0])) {
                return keyMap[index];
            } else {
                // handle out-of-range values, return a default value or an error code
                return '\0';
            }
        }

        return '\0';
    }

    // Shutdown computer
    void shutdown() {
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

        int timeOffset = intLength(currentTime.hour) + intLength(currentTime.minute) + intLength(currentTime.second);

        if (currentTime.hour < 0) timeOffset++;

        int dateOffset = intLength(currentTime.month) + intLength(currentTime.day);

        cp = 40 - timeOffset - dateOffset;

        dPrint("                   ");
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

        else if (cstrcmp(command[0], "timezone") == 0){
            if (cstrcmp(command[1], "") == 0){
                print("[ERROR] Timezone not specified");
                
                return;
            }

            int offset = stringToInt(command[1]);
            
            if (offset < -11 || offset > 12) {
                print("[ERROR] UTC offset can only be between -11 and 12"); 

                return;
            }
            
            TIMEZONE = offset;

            return;
        }

        // XTRA
        // else if (cstrcmp(command[0], "xtra") == 0){
        //     if (cstrcmp(command[1], "shutdown") == 0) {
        //         print("[INFO] Shutting down...");

        //         return;
        //     }
        //     else if (cstrcmp(command[1], "write") == 0) {
        //         print("[INFO] Writing to disk...");
                
        //         return;
        //     }
        //     else if (cstrcmp(command[1], "read") == 0) { 
        //         unsigned short dataBuffer[256];

        //         print("[INFO] Reading from disk...");

        //         readFromDisk(dataBuffer);

        //         clearScreen();

        //         for (int i=0; i < 256; i++){
        //             printInt(dataBuffer[i]);
        //             printChar(' ');
        //         }

        //         print("\n[INFO] Done");

        //         return;
        //     }
        //     else if (cstrcmp(command[1], "enableint") == 0) {
        //         print("[INFO] Enabling interrupts...");
                
        //         enableInterrupts();
                
        //         return;
        //     }
        //     else if (cstrcmp(command[1], "") == 0){
        //         print("[ERROR] No XTRA command specified\n");
                
        //         return;
        //     }
            
        //     print("[ERROR] Unknown XTRA command\n");
        // }

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
