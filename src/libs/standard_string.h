#ifndef STANDARD_STRING_H
#define STANDARD_STRING_H

#include "standard_math.h"
#include "standard_vartype.h"

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
    char* intToString(int _num, char* str) {
        int i = 0;
        int isNegative = 0;
        int num = _num;

        // Handle 0 case
        if (num == 0) {
            str[i++] = '0';
            str[i] = '\0';
            return str;
        }

        // Handle negative numbers for non-decimal bases
        if (num < 0) {
            isNegative = 1;
            num = abs(num);
        }

        // Convert integer to string
        while (num != 0) {
            str[i++] = num % 10 + 48;
            num /= 10;
        }

        // Add negative sign for decimal base
        if (isNegative)
            str[i++] = '-';

        // Add null terminator and reverse the string
        str[i] = '\0';
        reverse(str, i);

        return str;
    }

    // Function to convert a string to an integer
    int stringToInt(char* str){
        int final = 0;
    
        int length = 0;
        int power = 1;

        int negative = 1;

        while(str[length] != '\0') {
            length++;
            power *= 10;
        }

        power /= 10;

        int j = 0;

        if (str[0] == 45){
            negative = -1;
            j = 1;
            power /= 10;
        }

        for (int i=j; i < length; i++){
            if (str[i] < 48 || str[i] > 57) {
                return 0;
            }

            final += (str[i] - 48) * power;
            power /= 10;
        }

        return final * negative;
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

// String Functions ----------------------------------------------------------------------------------------------------------------

#endif