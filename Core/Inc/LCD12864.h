#ifndef _LCD12864_H
#define _LCD12864_H

#include "main.h"
#include "gpio.h"
#include "tim.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

                                  /* SYNC  | RW RS  */
#define SYNCCMD  0xf8             //1111 1   0  0   0
#define SYNCDATA 0xf8 | 0x02      //1111 1   0  1   0

#define CLEAR 0x01

#define ROW1 0x80
#define ROW2 0x90
#define ROW3 0x88
#define ROW4 0x98

enum COLx
{
    COL1 = 1,
    COL2,
    COL3,
    COL4,
    COL5,
    COL6,
    COL7,
    COL8,
    COL9,
    COL10,
    COL11,
    COL12,
    COL13,
    COL14,
    COL15,
    COL16
};

void LCD_Clear(void);
void LCD_Init(void);
void LCD_DispChar(uint8_t row,uint8_t col,char ch);
void LCD_DispString(uint8_t row,uint8_t col,const char *str);
void LCD_DispCN(uint8_t row,uint8_t col,char *str);

#endif