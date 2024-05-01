/*
 * ESE350 Lab 4 Pong
 *
 * Created: 03/15/2024 01:49:21 AM
 * Author : Maya Huizar
 */ 

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include "ST7735.h"
#include "LCD_GFX.h"
#include <util/delay.h>
#include "uart.h"
#define SOUND
#define __PRINT_NEW_LINE__  UART_putstring("\r\n");



char String[25];

void adc_init() {
	// Setup for ADC (10bit = 0-1023)

	// THIS ADC setup code is taken from
	// the ATMEGA328P examples github repo that
	// Prof. McGill-Gardner provided (this code is not mine
	// but I understand it and also use PC0 as the input pin)



	// Clear power reduction bit for ADC
	PRR0 &= ~(1 << PRADC);

	// Select Vref = AVcc
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);

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

void Initialize()
{
	lcd_init();
	DDRD |= (1 << DDD0);	
	DDRD |= (1 << DDD1);
	DDRD |= (1 << DDD2);
	adc_init();
	UART_init(BAUD_PRESCALER); 
}



void flashLEDone() {
	PORTD |= (1 << PORTD0);
	_delay_ms(100);
	PORTD &= ~(1 << PORTD0);	
	_delay_ms(100);
}

void flashLEDtwo() {
	PORTD |= (1 << PORTD1);
	_delay_ms(100);
	PORTD &= ~(1 << PORTD1);
	_delay_ms(100);
}


void buzz_sound() {

	// sorry about this function in particular, 
	// timer1 and timer2 were giving me trouble as the prescaler is shared and can't be changed
	// without messing up the display, and they kept not working, so
	// this was a compromise that is functional but not ideal. 
	#ifdef SOUND
	for (int i = 0; i < 25; i++) {
			// Turn on the LED
			PORTD |= (1 << PORTD2);
			// Wait for 100 milliseconds
			_delay_us(2700);
			// Turn off the LED
			PORTD &= ~(1 << PORTD2);
			// Wait for another 100 milliseconds to make the flash noticeable
			_delay_us(2700);
	}
	#endif
	;
}

// GLOBAL VARIABLES
int16_t ball_x, ball_y; // ball position
int16_t paddle1_x, paddle1_y; // paddle 1 position
int16_t paddle2_x, paddle2_y; // paddle 2 position
int16_t ball_vel_x, ball_vel_y; // ball velocity
int16_t paddle1_vel, paddle2_vel; // paddle velocities
uint8_t score1, score2; // scores

void reset_game()
{
	// reset the score
	score1 = 0;
	score2 = 0;


	// reset the ball
	ball_x = fixed_from_int(80);
	ball_y = fixed_from_int(64);
	ball_vel_x = fixed_from_int(1);
	ball_vel_y = fixed_from_int(1);

	// reset the paddles
	paddle1_x = fixed_from_int(LCD_WIDTH - 10);
	paddle1_y = fixed_from_int(64);
	paddle2_x = fixed_from_int(10);
	paddle2_y = fixed_from_int(120);
	paddle1_vel = fixed_from_int(0);
	paddle2_vel = fixed_from_int(0);

	LCD_setScreen(BLACK);

}

void draw_paddles(uint16_t color)
{
    // draw the paddles
    LCD_drawBlock(int_from_fixed(paddle1_x) - 3, int_from_fixed(paddle1_y) - 10, int_from_fixed(paddle1_x) + 3, int_from_fixed(paddle1_y) + 10, color);
    LCD_drawBlock(int_from_fixed(paddle2_x) - 3, int_from_fixed(paddle2_y) - 10, int_from_fixed(paddle2_x) + 3, int_from_fixed(paddle2_y) + 10, color);
}

void draw_ball(uint16_t color)
{
    // draw the ball
    LCD_drawCircle(int_from_fixed(ball_x), int_from_fixed(ball_y), 3, color);


}

void move_paddles()
{
    // move the paddles
	if (fixed_add(paddle1_y, paddle1_vel) <= fixed_from_int(10)) {
		paddle1_vel = fixed_from_int(0);
	}
	if (fixed_add(paddle1_y, paddle1_vel) >= fixed_from_int(118)) {
		paddle1_vel = fixed_from_int(0);
	}
    paddle1_y = fixed_add(paddle1_y, paddle1_vel);
    paddle2_y = ball_y;


}

void move_ball()
{
    // move the ball
    ball_x = fixed_add(ball_x, ball_vel_x);
    ball_y = fixed_add(ball_y, ball_vel_y);
}

void draw(uint16_t color) {
	draw_ball(color);
	draw_paddles(color);
}

void move()
{
	draw(BLACK);
    move_paddles();
    move_ball();
}

void draw_scores(uint16_t color)
{
    // draw the scores
	char string[20];
	itoa(score1, string, 10);
	

	LCD_drawString(70, 110, string, color, BLACK);
	itoa(score2, string, 10);
	LCD_drawString(90, 110, string, color, BLACK);

}


void check_collisions()
{
    // check for collisions
    // with the top and bottom of the screen
    if (int_from_fixed(ball_y) <= 10 || int_from_fixed(ball_y) >= 118) {
        ball_vel_y = -ball_vel_y;
		buzz_sound();
    }

    // with the paddles (adjust angle based on where the ball hits the paddle)
    if (ball_x <= fixed_add(paddle1_x, fixed_from_int(3)) && ball_x >= fixed_sub(paddle1_x, fixed_from_int(3)) && ball_y >= fixed_sub(paddle1_y, fixed_from_int(10)) && ball_y <= fixed_add(paddle1_y, fixed_from_int(10))) {
        ball_vel_x = -ball_vel_x;
		buzz_sound();

    }
    if (ball_x <= fixed_add(paddle2_x, fixed_from_int(3)) && ball_x >= fixed_sub(paddle2_x, fixed_from_int(3)) && ball_y >= fixed_sub(paddle2_y, fixed_from_int(10)) && ball_y <= fixed_add(paddle2_y, fixed_from_int(10))) {
        ball_vel_x = -ball_vel_x;
		buzz_sound();
    }

    // check for scoring
    if (ball_x <= fixed_from_int(5)) {
        // player 2 scores
        score2++;
        // reset the ball
        ball_x = fixed_from_int(80);
        ball_y = fixed_from_int(64);
        ball_vel_x = fixed_from_int(1);
        ball_vel_y = fixed_from_int(1);
		flashLEDtwo();
		buzz_sound();
		draw_scores(WHITE);


    }
    if (ball_x >= fixed_from_int(160)) {
        // player 1 scores
        score1++;
        // reset the ball
        ball_x = fixed_from_int(80);
        ball_y = fixed_from_int(64);
        ball_vel_x = fixed_from_int(-1);
        ball_vel_y = fixed_from_int(-1);
		flashLEDone();
		buzz_sound();
		draw_scores(WHITE);

    }
}

void check_game_over()
{
    // check for game over
    if (score1 >= 3 || score2 >= 3) {
        // game over
        // display the winner
        // reset the game
        reset_game();
    }
}


int16_t velocity_from_adc(int16_t x) {
	
	// NOTE: input is int16_t (actual) output is int16_t that represents a fixed point number.
    const int32_t scale_factor = 64; // scale by 2^6 b/c fixed point
    const int32_t max_input = 1023;
    const int32_t multiplier = 6 * scale_factor;
    const int32_t offset = 3 * scale_factor; // used int32_t here b/c max value of my fixed point in 512ish
    

    int32_t y = ((int32_t)x * multiplier / max_input) - offset;

    
    return (-(int16_t)y);
}



int main(void)
{
	Initialize();
    reset_game();

		
    while (1) {
		// main game loop

        // move the ball and paddles
		draw(WHITE);
        move();
        // check for collisions
        check_collisions();
        // check for game over
        check_game_over();
		
		// legacy code to print adc input to uart
		//char intStringBuffer[10];
		//itoa(int_from_fixed(paddle1_vel), intStringBuffer, 10);
		//UART_putstring(intStringBuffer);
		//__PRINT_NEW_LINE__
		
		paddle1_vel = velocity_from_adc(ADC);
    }
}