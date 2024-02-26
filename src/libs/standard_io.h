#ifndef STANDARD_IO_H
#define STANDARD_IO_H

#include "standard_string.h"
#include "standard_vartype.h"

// VGA ports and controller register indices
    #define VGA_PORT_CTRL 0x3D4
    #define VGA_PORT_DATA 0x3D5
    #define VGA_REG_CURSOR_HIGH 0x0E
    #define VGA_REG_CURSOR_LOW  0x0F

// pointer to the video memory
char *videoMemPtr = (char*)0xb8000;

// current line and column positions
int ln = 0;
int cp = 0;

void printInt(int);
void print(char*);
void printChar(char);
void dPrintInt(int);
void dPrint(char*);
void dPrintChar(char);
void setCursorPosition(uint16_t);
void clearScreen();

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


    // ATA PIO 28-bit disk driver
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
                outb(0x1F6, (0xE0 | ((startingSector >> 24) & 0x0F))); // head/drive # port - send head/drive #
                outb(0x1F2, 1); // sector count port - # of sectors to read/write
                outb(0x1F3, startingSector + i & 0xFF); // sector number port / LBA low bits 0-7
                outb(0x1F4, ((startingSector >> 8) & 0xFF)); // cylinder low port / LBA mid bits 8-15
                outb(0x1F5, ((startingSector >> 16) & 0xFF)); // cylinder high port / LBA high bits 16-23           
                outb(0x1F7, 0x20); // command port - send read/write command        
                
                if (i != numberOfSectors) printChar('\n');
                
                // poll status port
                while (inb(0x1F7) & (1 << 7)) // wait until BSY bit is clear
                    ;

                // read 256 words from data port into memory
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
                outb(0x1F6, (0xE0 | ((startingSector >> 24) & 0x0F))); // head/drive # port - send head/drive #
                outb(0x1F2, 1); // sector count port - # of sectors to read/write
                outb(0x1F3, startingSector + i & 0xFF); // sector number port / LBA low bits 0-7
                outb(0x1F4, ((startingSector >> 8) & 0xFF)); // cylinder low port / LBA mid bits 8-15
                outb(0x1F5, ((startingSector >> 16) & 0xFF)); // cylinder high port / LBA high bits 16-23           
                outb(0x1F7, 0x30); // command port - send read/write command    

                // poll status port
                while (inb(0x1F7) & (1 << 7)) // wait until BSY bit is clear
                    ;

                // write 256 words from memory to data port
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

            // send cache flush command after write command is finished
            outb(0x1F7, 0xE7);

            // wait until BSY bit is clear after cache flush
            while (inb(0x1F7) & (1 << 7))
                ;
        }
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
        char buffer[40];
        intToString(data, buffer);
        print(buffer);
    }

    // Function to print an integer to the bottom of the screen
    void dPrintInt(int data) {
        char buffer[40];
        intToString(data, buffer);
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

#endif