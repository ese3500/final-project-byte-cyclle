/*
 * SPI.c
 *
 * Created: 4/19/2024 11:42:29 AM
 * Author : chris
 */ 

#define F_CPU 16000000UL
#define F_SCL 100000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include "uart.h"

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
	sprintf(String, "\r%u", accValH);
	UART_putstring(String);
	I2C_start();
	I2C_write(0b11010000); //address of device
	I2C_write(0b00111110); //acc-data reg address
	I2C_start();
	I2C_write(0b11010001); //address of device
	accValL = I2C_readNA();
	I2C_stop();
}

void ROTC_SETUP() {
	TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1 <<TWEN); //start
	while (!(TWCR0 & (1<<TWINT)));
}

void led_setup() {
	DDRB |= (1 << DDRB0);
	accCount = 0;
}


int main(void)
{
	UART_init(BAUD_PRESCALER); 
    /* Replace with your application code */
	_delay_ms(500);
	I2C_SETUP();
	ACC_SETUP();
	_delay_ms(2000);
	
    while (1) {
		
		GET_ACC();
		_delay_ms(5);
		
		if (accValH > 25 && accValH < 180) {
			if (accCount < 15) {
				accCount+=2;
			}
		} else {
			if (accCount > 0) {
				accCount--;
			}
		}
		if (accCount > 6) {
			PORTB |= (1 << PORTB0);
		} else {
			PORTB &= ~(1 << PORTB0);
		}
		
		/*
		if (accValH > 40 && accValH < 200) {
			PORTB |= (1 << PORTB0);
		} else {
			PORTB &= ~(1 << PORTB0);
		}
		*/
		
    }
}

