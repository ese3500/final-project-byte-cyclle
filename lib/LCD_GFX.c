/*
 * LCD_GFX.c
 *
 * Created: 9/20/2021 6:54:25 PM
 *  Author: You
 */ 

#include "LCD_GFX.h"
#include "ST7735.h"

/******************************************************************************
* Local Functions
******************************************************************************/

CONSTANTS
#define LCD_WIDTH 160
#define LCD_HEIGHT 128


#define FIXED_POINT 6
#define FIXED_MASK ((1 << FIXED_POINT) - 1

int16_t fixed_add(int16_t a, int16_t b)
{
    return a + b;
}

int16_t fixed_sub(int16_t a, int16_t b)
{
    return a - b;
}

int16_t fixed_mul(int16_t a, int16_t b)
{
    return (a * b) >> FIXED_POINT;
}

int16_t fixed_div(int16_t a, int16_t b)
{
    return (a << FIXED_POINT) / b;
}

int16_t fixed_from_int(int16_t a)
{
	return a << FIXED_POINT;
}

int16_T int_from_fixed(int16_t a)
{
	return a >> FIXED_POINT;
}





/******************************************************************************
* Global Functions
******************************************************************************/

/**************************************************************************//**
* @fn			uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
* @brief		Convert RGB888 value to RGB565 16-bit color data
* @note
*****************************************************************************/
uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
	return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
}

/**************************************************************************//**
* @fn			void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color)
* @brief		Draw a single pixel of 16-bit rgb565 color to the x & y coordinate
* @note
*****************************************************************************/
void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
	LCD_setAddr(x,y,x,y);
	SPI_ControllerTx_16bit(color);
}

/**************************************************************************//**
* @fn			void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
* @brief		Draw a character starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor){
	uint16_t row = character - 0x20;		//Determine row of ASCII table starting at space
	int i, j;
	if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
		for(i=0;i<5;i++){
			uint8_t pixels = ASCII[row][i]; //Go through the list of pixels
			for(j=0;j<8;j++){
				if ((pixels>>j)&1==1){
					LCD_drawPixel(x+i,y+j,fColor);
				}
				else {
					LCD_drawPixel(x+i,y+j,bColor);
				}
			}
		}
	}
}


/******************************************************************************
* LAB 4 TO DO. COMPLETE THE FUNCTIONS BELOW.
* You are free to create and add any additional files, libraries, and/or
*  helper function. All code must be authentically yours.
******************************************************************************/

/**************************************************************************//**
* @fn			void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
* @brief		Draw a colored circle of set radius at coordinates
* @note
*****************************************************************************/
void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint16_t color)
{
	// trick to 1) only calculate 1/4 of the circle and 2) don't use setPixel, use more efficient drawLine
	for (int x = 0; x <= radius; x++) {
		for (int y = 0; y <= radius; y++) {
			if (x*x + y*y <= radius*radius) {
				LCD_drawLine(x0 + x, y0 + y, x0 - x, y0 + y, color);
				LCD_drawLine(x0 + x, y0 + y, x0 + x, y0 - y, color);
			}
		}
	}
}


/**************************************************************************//**
* @fn			void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
* @brief		Draw a line from and to a point with a color
* @note
*****************************************************************************/
void LCD_drawLine(short x0, short y0,short x1,short y1,uint16_t c)
{
	// check if perfectly vertical or horizontal
	if (x0 == x1 || y0 == y1) {
		LCD_setAddr(x0, y0, x1, y1);
		SPI_ControllerTx_16bit(c);
	} else { // this code is from wikipedia article linked in lab 4
		short dx = abs(x1 - x0);
		short sx = x0 < x1 ? 1 : -1;
		short dy = -abs(y1 - y0);
		short sy = y0 < y1 ? 1 : -1;
		short error = dx + dy;
		while (1) {
			LCD_drawPixel(x0, y0, c);
			if (x0 == x1 && y0 == y1) {
				break;
			}
			short e2 = 2 * error;
			if (e2 >= dy) {
				error += dy;
				x0 += sx;
			}
			if (e2 <= dx) {
				error += dx;
				y0 += sy;
			}
		}
	}
}

/**************************************************************************//**
* @fn			short abs(short x)
* @brief		Returns the absolute value of a short
* @note
*****************************************************************************/
short abs(short x) {
	return x < 0 ? -x : x;
}




/**************************************************************************//**
* @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
* @brief		Draw a colored block at coordinates
* @note
*****************************************************************************/
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
{
	LCD_setAddr(x0,y0,x1,y1);
	SPI_ControllerTx_16bit(color);
}

/**************************************************************************//**
* @fn			void LCD_setScreen(uint16_t color)
* @brief		Draw the entire screen to a color
* @note
*****************************************************************************/
void LCD_setScreen(uint16_t color) 
{
	LCD_setAddr((uint8_t)(0),(uint8_t)(0),(uint8_t)(159),(uint8_t)(127));
	SPI_ControllerTx_16bit(color);
}

/**************************************************************************//**
* @fn			void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
* @brief		Draw a string starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
{
	while(*str){
		LCD_drawChar(x,y,*str++,fg,bg);
		x+=7;
	}
}