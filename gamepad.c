/* 4+1 Joystick plus Simon game
 * Copyright (C) 2015 David Guerrero
 * Based on Teensy Gamepad for Josh Kropf <josh@slashdev.ca>
 * Based on keyboard example for Teensy USB Development Board
 * http://www.pjrc.com/teensy/usb_keyboard.html
 * Copyright (c) 2008 PJRC.COM, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#define F_CPU 16000000UL // 1 MHz
#include "usb_gamepad.h"

#ifndef TIMING
#include "timing.h"
#endif

#ifndef SOUND
#include "sound.h"
#endif

#ifndef CENTRAL_BUTTONS
#include "central_buttons.h"
#endif

#ifndef SIMON
#include "simon.h"
#endif

#define VIRTUAL_GAMEPAD_ID (NUMBER_OF_INTERFACES-1)

#define LED_CONFIG  (DDRD |= (1<<6))
#define LED_OFF     (PORTD &= ~(1<<6))
#define LED_ON      (PORTD |= (1<<6))

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

#define DIRECTION_DDR DDRC
#define DIRECTION_PORT PORTC
#define DIRECTION_PINS PINC

#define UP_PIN 1
#define DOWN_PIN 3
#define LEFT_PIN 0
#define RIGHT_PIN 2
#define DIRECTION_PINS_MASK ((1<<UP_PIN)|(1<<DOWN_PIN)|(1<<RIGHT_PIN)|(1<<LEFT_PIN))

#define SELECTOR_DDR DDRB
#define SELECTOR_PORT PORTB
#define SELECTOR_PINS PINB
#define SELECTOR_PINS_MASK (((1<<NUMBER_OF_INTERFACES)-1)<<1)//player selection pins starting at 1

#define is_active_pin(port,pin) (!(port&(1<<pin)))

#define BUTTONS_DDR	DDRF
#define BUTTONS_PORT	PORTF
#define BUTTONS_PINS	PINF

uint8_t axis_value(uint8_t port,uint8_t decrement_pin, uint8_t increment_pin)
{
	if(is_active_pin(port,increment_pin))
		return 0xff;
	if(is_active_pin(port,decrement_pin))
		return 0;
	return 0x80;
}

volatile uint8_t scaned_gamepad;
volatile uint8_t master_gamepad;
volatile uint8_t light_buttons_values;
volatile uint8_t random_value;

void read_gamepad_state(void)
{
	gamepad_state_t tmp;
	tmp.buttons=~BUTTONS_PINS;
	tmp.y_axis=axis_value(DIRECTION_PINS,DOWN_PIN,UP_PIN);
	tmp.x_axis=axis_value(DIRECTION_PINS,RIGHT_PIN,LEFT_PIN);

	if(scaned_gamepad!=VIRTUAL_GAMEPAD_ID)
				if(scaned_gamepad!=master_gamepad)
					memcpy(&gamepad_state[scaned_gamepad], &tmp, sizeof(gamepad_state_t));
				else
					memcpy(&gamepad_state[VIRTUAL_GAMEPAD_ID], &tmp, sizeof(gamepad_state_t));
	else
		memcpy(&gamepad_state[master_gamepad], &tmp, sizeof(gamepad_state_t));

	uint8_t selected_light_button_mask=1<<scaned_gamepad;
	light_buttons_values&=~selected_light_button_mask;

	if(
			 scaned_gamepad!=VIRTUAL_GAMEPAD_ID && (~BUTTONS_PINS & (1<<7))
		|| scaned_gamepad==VIRTUAL_GAMEPAD_ID && (~CENTRAL_BUTTON_PINS & (1<<CENTRAL_BUTTON))
	 )
		light_buttons_values|=selected_light_button_mask;

}

void select_gamepad(void)
{
	SELECTOR_PORT= SELECTOR_PINS_MASK & ~(1<<(scaned_gamepad+1));
}



void usb_gamepad_reset_state(uint8_t gamepad)
{
	gamepad_state[gamepad].buttons=0;
	gamepad_state[gamepad].y_axis =	0x80;
	gamepad_state[gamepad].x_axis = 0x80;
}


void configure_polling_interrupt(void)
{
//green_semaphore=1;//by default the polling counter can be modified
TCCR0A=2;//clear counter on compare match
OCR0A=1; //compare match when counter=1 (1919 times per second aprox.)=>0.5 miliseconds period aprox.
TCNT0=0x00;     // set timer0 counter initial value to 0
TIMSK0=2;// enable timer 0 output compare match 2
TCCR0B=4;//divide frequency by 256
sei(); // enable interrupts
}
int main(void) {
	// set for 16 MHz clock
	CPU_PRESCALE(0);
	configure_clock();
	configure_beeper();
	configure_central_buttons();
	configure_simon();
	LED_CONFIG;

	BUTTONS_DDR=  0;DIRECTION_DDR=  0; //set as input
	BUTTONS_PORT=  0xff; DIRECTION_PORT|=DIRECTION_PINS_MASK; MCUCR&=~(1<<4); //activate pull-up
	SELECTOR_DDR=SELECTOR_PINS_MASK;//set as output
	SELECTOR_PORT=SELECTOR_PINS_MASK;//no gamepad selected, deactivate pullups

	for(scaned_gamepad=0;scaned_gamepad<NUMBER_OF_INTERFACES;scaned_gamepad++)
		usb_gamepad_reset_state(scaned_gamepad); //players will be reported as idle by default
	scaned_gamepad=0;
	select_gamepad();
	master_gamepad=VIRTUAL_GAMEPAD_ID;
	light_buttons_values=0;

	LED_ON; // power up led on startup for 1 sec

	// Initialize the USB, and then wait for the host to set configuration.
	// If the Teensy is powered without a PC connected to the USB port,
	// this will wait forever.
	usb_init();
	while (!usb_configured()) /* wait */ ;
	wait_for_miliseconds(1000);
	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	configure_polling_interrupt();
// {
// 	uint8_t i;
// 	for(i=0;i<5;i++)
// 	{
// 		//play_tone(i);
// 		LIGHTS_PORT=1<<i;
// 		//active_wait(20);
// 		//nobeep;
//
// 		wait_for_miliseconds(1000);
// 		//_delay_ms(1);
// 		//wait_for_miliseconds(500);
// 		//nobeep;
// 		//wait_for(1000);
// 	}
// }
// 	nobeep;
	LED_OFF;
	while (1)
	{
		//
		// if(is_active_pin(CENTRAL_BUTTON_PINS,CENTRAL_BUTTON))
		// 	LIGHTS_PORT|= LIGHTS_PINS_MASK;// turn all of them on
		// else
		// 	LIGHTS_PORT&= ~LIGHTS_PINS_MASK;// turn all of them off
		//LIGHTS_PORT=light_buttons_values<<1;//for debugging
		//while(!light_buttons_values);//wait till a special light button is pressed
		if(pressed_central_button)//master player change or Simon game request
			{
				uint8_t color_button_has_been_pressed=0;
				do{
							for(uint8_t i=0;i<VIRTUAL_GAMEPAD_ID;i++)
							{
									if(pressed_light_button(i))
									{
											color_button_has_been_pressed=1;
											turn_off_color_button_lights;
											if(i!=master_gamepad)//select this button as master button
											{
														master_gamepad=i;
														turn_on_color_button_light(i);
														turn_off_central_button_light;//for debugging
											}
											else//deselect this button as master button
											{
														master_gamepad=VIRTUAL_GAMEPAD_ID;
														turn_on_central_button_light;//for debugging
											}
											wait_till_depressed_button(i);
									}
							}
					}while(pressed_central_button);
					if(!color_button_has_been_pressed)//a Simon game session has been requested
						simon_game(4);
			}
	}/////////////////jarrrrrr
	// else//insert coin request
	// 	;
}

ISR(TIMER0_COMPA_vect)
{
 	 read_gamepad_state();
 	 usb_gamepad_send(GAMEPAD_INTERFACE(scaned_gamepad));
	 scaned_gamepad=(scaned_gamepad+1)%NUMBER_OF_INTERFACES;
	 select_gamepad();
	 random_value++;
	 random_value&=3;
 }
