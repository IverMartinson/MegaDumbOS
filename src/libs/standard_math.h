#ifndef STANDARD_MATH_H
#define STANDARD_MATH_H

// Math Functions

    // Absolute function
    int abs(int num) {
        return (num < 0) ? -num : num;
    }


    // Return length of int
    int intLength(int num) {
        int length = 0;

        num = abs(num);

        if (num >= 10) length++;
        if (num >= 100) length++;
        if (num >= 1000) length++;
        if (num >= 10000) length++;
        if (num >= 100000) length++;
        if (num >= 1000000) length++;
        if (num >= 10000000) length++;
        if (num >= 100000000) length++;
        if (num >= 1000000000) length++;
        if (num >= 10000000000) length++;
        if (num >= 100000000000) length++;
        if (num >= 1000000000000) length++;
        if (num >= 10000000000000) length++;
        if (num >= 100000000000000) length++;
        if (num >= 1000000000000000) length++;

        return length;
    }
    

// Math Functions

#endif