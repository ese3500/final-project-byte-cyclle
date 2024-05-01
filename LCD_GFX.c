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



// I decided to use fixed point to store the ball's position and velocity
// and the paddle's as well. 
// floats are slow on microcontrollers, and i don't need the whole range
// of a int16_t, so i can use the lower 8 bits for the fractional part
// this is especially important for the velocities, as they
// are often below 1 and i would have an annoying time and precision loss
// if i were to frequently cast to float
// i like it

int16_t fixed_add(int16_t a, int16_t b) {
	return a + b;
}

int16_t fixed_sub(int16_t a, int16_t b) {
	return a - b;
}

int16_t fixed_mul(int16_t a, int16_t b) {
	int32_t temp = (int32_t)a * (int32_t)b;
	return (int16_t)(temp >> FIXED_POINT);
}

int16_t fixed_div(int16_t a, int16_t b) {
	if (b == 0) {
		// Handle division by zero, perhaps return 0 or some error value
		return 0;
	}
	int32_t temp = ((int32_t)a << FIXED_POINT) + (b / 2); // Add half of b for rounding
	return (int16_t)(temp / b);
}

int16_t fixed_from_int(int16_t a) {
	return a << FIXED_POINT;
}

int16_t int_from_fixed(int16_t a) {
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
void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint16_t color) {
	int x = radius;
	int y = 0;
	int d = 1 - x;   
	// fast circle drawing algorithm.
	// i wrote a separate function for filled circle and
	// default to this one because:
	// 1) i prefer the look of the unfilled circle
	// 2) i learned about this algorithm last semester in
	// CIS 4600 when writing rasterizers
	// and wanted to use it again because it's cool


	while (y <= x) {
		LCD_drawPixel(x0 + x, y0 + y, color); 
		LCD_drawPixel(x0 + y, y0 + x, color); 
		LCD_drawPixel(x0 - y, y0 + x, color); 
		LCD_drawPixel(x0 - x, y0 + y, color); 
		LCD_drawPixel(x0 - x, y0 - y, color); 
		LCD_drawPixel(x0 - y, y0 - x, color); 
		LCD_drawPixel(x0 + y, y0 - x, color); 
		LCD_drawPixel(x0 + x, y0 - y, color); 
		y++;
		if (d <= 0) {
			d += 2 * y + 1;   
			} else {
			x--;
			d += 2 * (y - x) + 1;   
		}
	}
}


void LCD_drawFilledCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint16_t color)
{
	// slow circle drawing algorithm that fills in the circle
	for (int x = -radius; x <= radius; x++) {
		for (int y = -radius; y <= radius; y++) {
			if (x * x + y*y <= radius * radius) {
				LCD_drawPixel(x0 +x, y0 + y, color);
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
		clear(LCD_PORT, LCD_TFT_CS);	//CS pulled low to start communication

		for(int i = 0; i < (abs(x0 - x1) * abs(y0-y1)); i++){
			SPI_ControllerTx_16bit_stream(c);
		}

		set(LCD_PORT, LCD_TFT_CS);	//set CS to high
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
	return(x < 0 ? -x : x);
}




/**************************************************************************//**
* @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
* @brief		Draw a colored block at coordinates
* @note
*****************************************************************************/
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
{
	LCD_setAddr(x0,y0,x1,y1);
	clear(LCD_PORT, LCD_TFT_CS);	//CS pulled low to start communication

	for(int i = 0; i < (abs(x0 - x1) * abs(y0-y1)); i++){
		SPI_ControllerTx_16bit_stream(color);
	}

	set(LCD_PORT, LCD_TFT_CS);	//set CS to high
}

/**************************************************************************//**
* @fn			void LCD_setScreen(uint16_t color)
* @brief		Draw the entire screen to a color
* @note
*****************************************************************************/
void LCD_setScreen(uint16_t color) 
{
	LCD_setAddr((uint8_t)(0),(uint8_t)(0),(uint8_t)(159),(uint8_t)(127));
	clear(LCD_PORT, LCD_TFT_CS);	//CS pulled low to start communication

	for(int i = 0; i < 20480; i++){
		SPI_ControllerTx_16bit_stream(color);
	}

	set(LCD_PORT, LCD_TFT_CS);	//set CS to high
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