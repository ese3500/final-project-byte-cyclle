/*
 * LCD_GFX.h
 *
 * Created: 9/20/2021 6:54:37 PM
 *  Author: You
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include "ASCII_LUT.h"
#include <string.h>

#ifndef LCD_GFX_H_
#define LCD_GFX_H_

// colors
#define	BLACK     0x0000
#define WHITE     0xFFFF
#define	BLUE      0x001F
#define	RED       0xF800
#define	GREEN     0x07E0
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0


#define LCD_WIDTH 160
#define LCD_HEIGHT 128

#define CHAR_WIDTH 5   // Number of columns in the character bitmap
#define CHAR_HEIGHT 8  // Number of rows in the character bitmap

typedef struct {
	uint8_t foreground[CHAR_HEIGHT][CHAR_WIDTH];
	uint8_t background[CHAR_HEIGHT][CHAR_WIDTH];
} CharUpdate;


uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue);
void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color);
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor, uint16_t multiplier);
void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color);
void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c);
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color);
void LCD_setScreen(uint16_t color);
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg, uint16_t multiplier);

void redrawChar(uint8_t x, uint8_t y, CharUpdate *update, uint16_t fColor, uint16_t bColor, uint8_t multiplier);
void prepareCharUpdate(CharUpdate *update, char oldChar, char newChar);

#endif /* LCD_GFX_H_ */