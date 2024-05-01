/*
 * ESE350 Lab 4 Pong
 *
 * Created: 03/15/2024 01:49:21 AM
 * Author : Maya Huizar
 */ 

#define F_CPU 16000000UL
#define F_SCL 100000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "ST7735.h"
#include "LCD_GFX.h"
#include <util/delay.h>
#include "uart.h"
#define __PRINT_NEW_LINE__  UART_putstring("\r\n");

uint16_t speed = 80;
uint16_t old_speed = 65535;
uint16_t hour = 15;
uint16_t old_hour = 65535;
CharUpdate* charUp;
uint16_t minute = 30;
uint16_t old_minute = 65535;
volatile int isDay = 1;


uint16_t counter = 0;

char String[25];

volatile uint8_t accValH;
volatile uint8_t accValL;
volatile uint8_t accCount;

void I2C_start() {
	TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1 <<TWEN); //start
	while (!(TWCR0 & (1<<TWINT)));
}

void I2C_stop() {
	TWCR0 = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN); //stop
}

void I2C_write(uint8_t data) {
	TWDR0 = data;
	TWCR0 = (1 << TWINT) | (1 <<TWEN);
	while (!(TWCR0 & (1<<TWINT)));
}

uint8_t I2C_readA() {
	TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while (!(TWCR0 & (1<<TWINT)));
	return TWDR0;
}

uint8_t I2C_readNA() {
	TWCR0 = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR0 & (1<<TWINT)));
	return TWDR0;
}

void I2C_SETUP() {
	TWBR0 |= (1 << TWBR0);
	TWSR0 |= (1 << TWPS1);
	TWBR0 = (uint8_t)(((F_CPU / F_SCL) - 16) / 2);
	TWCR0 = (1<<TWEN);
}


void ACC_SETUP() {
	I2C_start();
	/*
	I2C_write(0b11010000);//address of device
	I2C_write(0b00011100); //reg add
	I2C_write(0b01000000); //data
	I2C_start();
	
	I2C_write(0b11010000);//address of device
	I2C_write(0b00011001); //reg add
	I2C_write(0b00000001); //data
	I2C_start();
	
	I2C_write(0b11010000);//address of device
	I2C_write(0b00100011); //reg add
	I2C_write(0b00001000); //data
	I2C_start();
	*/
	I2C_write(0b11010000);//address of device
	I2C_write(0b01101010); //reg add
	I2C_write(0x00); //data
	I2C_start();
	I2C_write(0b11010000);//address of device
	I2C_write(0b01101011); //reg add
	I2C_write(0x00); //data
	I2C_stop();	
}

void GET_ACC() {
	I2C_start();
	I2C_write(0b11010000); //address of device
	I2C_write(0b00111101); //acc-data reg address
	//I2C_write(0b01000010); //debug using temperature)
	I2C_start();
	I2C_write(0b11010001); //address of device
	
	accValH = I2C_readNA();
	//sprintf(String, "\r%u", accValH);
	//UART_putstring(String);
	I2C_start();
	I2C_write(0b11010000); //address of device
	I2C_write(0b00111110); //acc-data reg address
	I2C_start();
	I2C_write(0b11010001); //address of device
	accValL = I2C_readNA();
	I2C_stop();
}

void setup_ROTC() {
	I2C_start();
	I2C_write(0b10100010);
	I2C_write(0b00000000);
	I2C_write(0b00000000);
	I2C_write(0b00000000);
	I2C_write(0b00000000);
	I2C_write(0b01000101);
	I2C_write(0b00010100);
	I2C_write(0b00010110);
	I2C_write(0b00000101);
	I2C_write(0b10000111);
	I2C_write(0b00001000);
	I2C_write(0b10000000);
	I2C_write(0b10000000);
	I2C_write(0b10000000);
	I2C_write(0b10000000);
	I2C_write(0b10000000);
	I2C_write(0b00000000);
	I2C_stop();
}

void ROTC_SETUP() {
	TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1 <<TWEN); //start
	while (!(TWCR0 & (1<<TWINT)));
}

void led_setup() {
	accCount = 0;
}

void pwm_setup() {

	cli();   // Disable global interrupts
	DDRD |= (1 << DDD5);

	// Timer0, prescale of 1
	TCCR0B |= (1 << CS00);

	// Timer1, Fast PWM mode
	TCCR0A |= (1 << WGM00);
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << WGM02);

	OCR0A = 39;              // Sets frequency, 400kHz
	OCR0B = OCR0A * 1 / 10;   // Sets duty cycle, 75%

	// Non-inverting mode
	// Clear on Compare Match
	TCCR0A |= (1 << COM0B1);

	sei();   // Enable global interrupts
}



void adc_setup() {
	// code from nick's atmega example
	// Setup for ADC (10bit = 0-1023)
	// Clear power reduction bit for ADC
	PRR0 &= ~(1 << PRADC);

	// Select Vref = AVcc
	ADMUX |= (1 << REFS0);
	ADMUX |= (1 << REFS1);

	// Set the ADC clock div by 128
	// 16M/128=125kHz
	ADCSRA |= (1 << ADPS0);
	ADCSRA |= (1 << ADPS1);
	ADCSRA |= (1 << ADPS2);

	// Select Channel ADC0 (pin C0)
	ADMUX &= ~(1 << MUX0);
	ADMUX &= ~(1 << MUX1);
	ADMUX &= ~(1 << MUX2);
	ADMUX &= ~(1 << MUX3);

	ADCSRA |= (1 << ADATE);   // Autotriggering of ADC

	// Free running mode ADTS[2:0] = 000
	ADCSRB &= ~(1 << ADTS0);
	ADCSRB &= ~(1 << ADTS1);
	ADCSRB &= ~(1 << ADTS2);

	// Disable digital input buffer on ADC pin
	DIDR0 |= (1 << ADC0D);

	// Enable ADC
	ADCSRA |= (1 << ADEN);

	// Start conversion
	ADCSRA |= (1 << ADSC);
}


void pinchange_setup() {
	cli();
	DDRB |= (1 << DDRB5);
	DDRC &= ~(1 << DDRC2);

	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT10);
	PORTC |= (1 << PORTC2);
	sei();
}

void Initialize()
{
	_delay_ms(500);
	lcd_init();
	LCD_setScreen(BLACK);
	UART_init(BAUD_PRESCALER);
	_delay_ms(500);
	adc_setup();
	pwm_setup();
	pinchange_setup();
	_delay_ms(500); 
	I2C_SETUP();
	_delay_ms(50);
	//setup_ROTC();
	ACC_SETUP();
	_delay_ms(500);
}


void draw_speed(void) {
	char oldchar1;
	char oldchar2;
	char newchar1;
	char newchar2;
	if (old_speed < 100) {
		oldchar1 = '0' + (old_speed / 10);
		oldchar2 = '0' + (old_speed % 10);
	} else {
		oldchar1 = '0' + (old_speed / 100);
		oldchar2 = '0' + (old_speed  % 100) / 10;
	}
	if (speed < 100) {
		newchar1 = '0' + (speed / 10);
		newchar2 = '0' + (speed % 10);
		} else {
		newchar1 = '0' + (speed / 100);
		newchar2 = '0' + (speed  % 100) / 10;
	}
	if (old_speed == 65535) {
		oldchar1 = ' ';
		oldchar2 = ' ';
	}
	if ((old_speed > 100) && (speed <+ 100)) {
		prepareCharUpdate(charUp, ' ', '.');
		redrawChar(55, 19, charUp, BLACK, WHITE, 2);   
	} else if ((old_speed < 100) && (speed >= 100)) {
		prepareCharUpdate(charUp, '.', ' ');
		redrawChar(55, 19, charUp, BLACK, WHITE, 2);   
	}
	
	
	prepareCharUpdate(charUp, oldchar1, newchar1);
	redrawChar(40, 11, charUp, BLACK, WHITE, 3);   
	prepareCharUpdate(charUp, oldchar2, newchar2);
	redrawChar(64, 11, charUp, BLACK, WHITE, 3);   
	old_speed = speed;
	return;
}

void draw_time(void) {
	if (minute == 60) {
		minute = 0;
		hour++;
	}
	
	hour = hour % 24;
	
	char oldchar1;
	char oldchar2;
	char newchar1;
	char newchar2;

	oldchar1 = '0' + (old_hour / 10);
	oldchar2= '0' + (old_hour  % 10);
	newchar1 = '0' + (hour / 10);
	newchar2 = '0' + (hour  % 10);

	
	if (old_hour == 65535) {
		oldchar1 = ' ';
		oldchar2 = ' ';
	}
	prepareCharUpdate(charUp, oldchar1, newchar1);
	redrawChar(8, 105, charUp, BLACK, WHITE, 2);   
	prepareCharUpdate(charUp, oldchar2, newchar2);
	redrawChar(20, 105, charUp, BLACK, WHITE, 2);
	

	oldchar1 = '0' + (old_minute / 10);
	oldchar2= '0' + (old_minute  % 10);
	newchar1 = '0' + (minute / 10);
	newchar2 = '0' + (minute  % 10);

		
	if (old_minute == 65535) {
		oldchar1 = ' ';
		oldchar2 = ' ';
	}
	prepareCharUpdate(charUp, oldchar1, newchar1);
	redrawChar(42, 105, charUp, BLACK, WHITE, 2);   
	prepareCharUpdate(charUp, oldchar2, newchar2);
	redrawChar(54, 105, charUp, BLACK, WHITE, 2);   
	old_hour = hour;
	old_minute = minute;
	return;
}


ISR(PCINT1_vect) {
	PORTB ^= (1 << PORTB5);
	
	if (PINC & (1 << PINC2)) {
		prepareCharUpdate(charUp, 'O', 'L');
		redrawChar(129, 105, charUp, BLACK, WHITE, 2);
		prepareCharUpdate(charUp, 'K', 'O');
		redrawChar(141, 105, charUp, BLACK, WHITE, 2);
	} else {
		prepareCharUpdate(charUp, 'L', 'O');
		redrawChar(129, 105, charUp, BLACK, WHITE, 2);
		prepareCharUpdate(charUp, 'O', 'K');
		redrawChar(141, 105, charUp, BLACK, WHITE, 2);
	}
}



int main(void)
{
	Initialize();
	
	
	LCD_drawBlock(1, 1, 158, 127, WHITE);
	LCD_drawBlock(11, 6, 148, 36, BLACK);
	LCD_drawBlock(12, 7, 147, 35, WHITE);

	LCD_drawBlock(6, 102, 70, 122, BLACK);
	LCD_drawBlock(7, 103, 69, 121, WHITE);
	
	LCD_drawBlock(89, 102, 153, 122, BLACK);
	LCD_drawBlock(90, 103, 152, 121, WHITE);

	

	char buf[3] = ".";
	LCD_drawString(55, 19, buf, BLACK, WHITE, 2);
	sprintf(buf, "mph/s");
	LCD_drawString(85, 19, buf, BLACK, WHITE, 2);
	LCD_drawBigChar(32, 105, ':', BLACK, WHITE, 2);
		
	charUp = malloc(sizeof(CharUpdate));
	prepareCharUpdate(charUp, ' ', '.');
	redrawChar(55, 19, charUp, BLACK, WHITE, 2);   
	
	prepareCharUpdate(charUp, ' ', 'B');
	redrawChar(91, 105, charUp, BLACK, WHITE, 2);   
	prepareCharUpdate(charUp, ' ', 'A');
	redrawChar(103, 105, charUp, BLACK, WHITE, 2);   
	prepareCharUpdate(charUp, ' ', 'T');
	redrawChar(115, 105, charUp, BLACK, WHITE, 2);   
	
	// set to OK or LO based on value read from pin.
	
	if (PINC & (1 << PINC2)) {
		prepareCharUpdate(charUp, ' ', 'L');
		redrawChar(129, 105, charUp, BLACK, WHITE, 2);
		prepareCharUpdate(charUp, ' ', 'O');
		redrawChar(141, 105, charUp, BLACK, WHITE, 2);
	} else {
		prepareCharUpdate(charUp, ' ', 'O');
		redrawChar(129, 105, charUp, BLACK, WHITE, 2);
		prepareCharUpdate(charUp, ' ', 'K');
		redrawChar(141, 105, charUp, BLACK, WHITE, 2);
	}

	DDRC &= ~(1 << DDRC4);
    while (1) {
		
		GET_ACC();
		_delay_ms(5);
		
		if (accValH > 128 && accValH < 256 - 6){
			if (accCount < 10) {
				accCount += 2;
			}	
		} else {
			if (accCount > 0) {
				accCount--;
			}	
		}
		
		if(isDay == 1) {
			if(accCount > 4){
				TCCR0A |= (1<<COM0B0);
				TCCR0A |= (1<<COM0B1);
				OCR0B = 0;
			} else {
				TCCR0A &= ~(1<<COM0B0);
				TCCR0A &= ~(1<<COM0B1);
				OCR0B = OCR0A;
			}
		} else {
			if(accCount > 6) {
				TCCR0A |= (1<<COM0B0);
				TCCR0A |= (1<<COM0B1);
				OCR0B = 0;
			} else {
				TCCR0A |= (1<<COM0B0);
				TCCR0A |= (1<<COM0B1);
				OCR0B = OCR0A * 9 / 10;
			}
		}
		/*
		if(accCount > 6) {
			TCCR0A |= (1<<COM0B0);
			TCCR0A |= (1<<COM0B1);
			OCR0B = OCR0A;
			
		} else {
			if (isDay == 1) {
				TCCR0A &= ~(1<<COM0B0);
				TCCR0A &= ~(1<<COM0B1);
			} else {
				TCCR0A |= (1<<COM0B0);
				TCCR0A |= (1<<COM0B1);
				OCR0B = 1;
			}
			//PORTB &= ~(1 << PORTB4);
		}
		*/
		
		
	    // Reset or increment speed
		//speed++;
		//minute++;
		// update speed
		//sprintf(String, "ADC: \r%u\n", ADC);
		//UART_putstring(String);
		if (ADC <= 1000) {
			isDay = 0;
		} else {
			isDay = 1;
		}
		sprintf(String, "ACC: %u\n", accValH);
		UART_putstring(String);
		if (accValH > 120) {
			accValH = 256 - accValH;
		}
		float gees = (((float)accValH) / 60.0);
		speed = gees * 22;
		speed = speed % 99;
		counter++;
		if (counter == 500) {
			minute++;
			speed++;
			counter = 0;
		}
		draw_speed();
		draw_time();
		
		
    }
	return 0;
}