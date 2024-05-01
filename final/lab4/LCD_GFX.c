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

// CONSTANTS







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
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor, uint16_t multiplier){
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


/**************************************************************************//**
* @fn            void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor, uint8_t multiplier)
* @brief         Draw a character starting at the point with foreground and background colors, scaled by multiplier
* @note
*****************************************************************************/
void LCD_drawBigChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor, uint8_t multiplier){
uint16_t row = character - 0x20;    // Determine row of ASCII table starting at space
int i, j, k, l;

if (LCD_WIDTH - x > 7 * multiplier && LCD_HEIGHT - y > 7 * multiplier){
for(i = 0; i < 5; i++){
uint8_t pixels = ASCII[row][i]; // Go through the list of pixels
for(j = 0; j < 8; j++){
if ((pixels >> j) & 1){
for(k = 0; k < multiplier; k++){
for(l = 0; l < multiplier; l++){
LCD_drawPixel(x + i * multiplier + k, y + j * multiplier + l, fColor);
}
}
} else {
for(k = 0; k < multiplier; k++){
for(l = 0; l < multiplier; l++){
LCD_drawPixel(x + i * multiplier + k, y + j * multiplier + l, bColor);
}
}
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
	int decisionOver2 = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0

	while (y <= x) {
		LCD_drawPixel(x0 + x, y0 + y, color); // Octant 1
		LCD_drawPixel(x0 + y, y0 + x, color); // Octant 2
		LCD_drawPixel(x0 - y, y0 + x, color); // Octant 3
		LCD_drawPixel(x0 - x, y0 + y, color); // Octant 4
		LCD_drawPixel(x0 - x, y0 - y, color); // Octant 5
		LCD_drawPixel(x0 - y, y0 - x, color); // Octant 6
		LCD_drawPixel(x0 + y, y0 - x, color); // Octant 7
		LCD_drawPixel(x0 + x, y0 - y, color); // Octant 8
		y++;
		if (decisionOver2 <= 0) {
			decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
			} else {
			x--;
			decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
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
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
{
	LCD_setAddr(x0,y0,x1,y1);
	clear(LCD_PORT, LCD_TFT_CS);	//CS pulled low to start communication

	for(int i = 0; i < ((abs(x0 - x1)+1) * (abs(y0 - y1))+1) - 1; i++){
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



void initCharUpdate(CharUpdate *update) {
	// Use memset to set all bytes in the CharUpdate structure to 0.
	memset(update, 0, sizeof(CharUpdate));
}

void prepareCharUpdate(CharUpdate *update, char oldChar, char newChar) {
	uint8_t oldIndex = oldChar - 0x20;
	uint8_t newIndex = newChar - 0x20;

	memset(update, 0, sizeof(CharUpdate));  // Clear the update structure

	for (int col = 0; col < CHAR_WIDTH; col++) {
		uint8_t oldCol = ASCII[oldIndex][col];
		uint8_t newCol = ASCII[newIndex][col];

		// Calculate foreground and background updates
		uint8_t fgMask = newCol & ~oldCol;  // New bits that are 1 where old bits are 0
		uint8_t bgMask = ~newCol & oldCol;  // Old bits that are 1 where new bits are 0

		// Set the corresponding bits in the update structure
		for (int row = 0; row < CHAR_HEIGHT; row++) {
			update->foreground[row][col] = (fgMask >> row) & 1;
			update->background[row][col] = (bgMask >> row) & 1;
		}
	}
}


void redrawChar(uint8_t x, uint8_t y, CharUpdate *update, uint16_t fColor, uint16_t bColor, uint8_t multiplier) {
	// Loop through each pixel in the character matrix (5x8)
	for (int col = 0; col < CHAR_WIDTH; col++) {
		for (int row = 0; row < CHAR_HEIGHT; row++) {
			uint16_t baseX = x + col * multiplier;
			uint16_t baseY = y + row * multiplier;

			// Determine the appropriate color for each block based on the update arrays
			uint16_t color;
			if (update->foreground[row][col]) {
				color = fColor;  // Draw foreground color if foreground is set
				} else if (update->background[row][col]) {
				color = bColor;  // Draw background color if background is set
				} else {
				continue;  // Skip drawing if no updates are needed
			}
            LCD_drawBlock(baseX, baseY, baseX + multiplier - 1, baseY + multiplier, color);			// Draw a block of size multiplier x multiplier for each pixel
			
		}
	}
}




void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg, uint16_t multiplier)
{
	while(*str){
		LCD_drawBigChar(x,y,*str++,fg,bg, multiplier);
		x+=(8 * multiplier)  - (3 - multiplier)*3;
	}
}