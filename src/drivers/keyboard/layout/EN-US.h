#ifndef KB_LAYOUT_EN_US_H
#define KB_LAYOUT_EN_US_H
/*
*   This file declares the keyboard map for US keyboards.
*/

const unsigned char keyboard_us_lower[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y',
    'u', 'i', 'o', 'p', '[', ']', 
    '\n' /* Enter */ , 0, /* CTRL */
    'a', 's', 'd', 'f', 'g', 'h','j', 'k', 'l', ';', '\'', '`', 0, /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, /* Right shift */
     '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

//Shift key is pressed
const unsigned char keyboard_us_upper[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(',
    ')', '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y',
    'U', 'I', 'O', 'P', '{', '}', 
    '\n' /* Enter */ , 0, /* CTRL */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, /* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, /* Right shift */
     '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

const unsigned char keyboard_us_upper_caps[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '-', '=', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y',
    'U', 'I', 'O', 'P', '[', ']', 
    '\n' /* Enter */ , 0, /* CTRL */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, /* Left shift */
    '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, /* Right shift */
     '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};


#endif //KB_LAYOUT_EN_US_H