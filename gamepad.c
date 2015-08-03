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
//#define F_CPU 1000000UL // 1 MHz
#include <util/delay.h>
#include "usb_gamepad.h"

#ifndef TIMING
#include "timing.h"
#endif

#ifndef SOUND
#include "sound.h"
#endif

#ifndef SIMON
#include "simon.h"
#endif

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
#define select_gamepad(x) SELECTOR_PORT= SELECTOR_PINS_MASK & ~(1<<(x+1))

#define is_active_pin(port,pin) (!(port&(1<<pin)))

#define BUTTONS_DDR	DDRF
#define BUTTONS_PORT	PORTF
#define BUTTONS_PINS	PINF

#define CENTRAL_BUTTON_DDR DDRE
#define CENTRAL_BUTTON_PORT PORTE
#define CENTRAL_BUTTON_PINS PINE
#define CENTRAL_BUTTON 6

#define LIGHTS_DDR	DDRD
#define LIGHTS_PORT	PORTD
#define LIGHTS_PINS	PIND
#define LIGHTS_PINS_MASK ((1<<(NUMBER_OF_INTERFACES+1))-1) //There is a LIGHT per player plus the central one

uint8_t axis_value(uint8_t port,uint8_t decrement_pin, uint8_t increment_pin)
{
	if(is_active_pin(port,increment_pin))
		return 0xff;
	if(is_active_pin(port,decrement_pin))
		return 0;
	return 0x80;
}

void read_gamepad_state(uint8_t gamepad)
{
	gamepad_state[gamepad].buttons=~BUTTONS_PINS;

	gamepad_state[gamepad].y_axis =	axis_value(DIRECTION_PINS,DOWN_PIN,UP_PIN);
	gamepad_state[gamepad].x_axis = axis_value(DIRECTION_PINS,RIGHT_PIN,LEFT_PIN);
}
void usb_gamepad_reset_state(uint8_t gamepad)
{
	gamepad_state[gamepad].buttons=0;
	gamepad_state[gamepad].y_axis =	0x80;
	gamepad_state[gamepad].x_axis = 0x80;
}

volatile uint8_t selected_player;

void configure_polling_interrupt(void)
{
green_semaphore=1;//by default the polling counter can be modified
TCCR0A=2;//clear counter on compare match
TCCR0B=4;//divide frequency by 256
OCR0A=1; //compare match when counter=1 (1953 times per second aprox.)=>0.5 miliseconds period aprox.
TCNT0=0x00;     // set timer0 counter initial value to 0
TIMSK0=2;// enable timer 0 output compare match 2
sei(); // enable interrupts
}
int main(void) {
	// set for 1 MHz clock
	CPU_PRESCALE(4);
	//configure_clock();
	configure_beeper();
	configure_simon();
	// good explenation of how AVR pins work:
	// http://www.pjrc.com/teensy/pins.html

	BUTTONS_DDR=  0;DIRECTION_DDR=  0; //set as input
	LIGHTS_DDR= LIGHTS_PINS_MASK; //set as output
	BUTTONS_PORT=  0xff; DIRECTION_PORT|=DIRECTION_PINS_MASK; MCUCR&=~(1<<4); //activate pull-up

	CENTRAL_BUTTON_DDR=0;//set as input
	CENTRAL_BUTTON_PORT|=(1<<CENTRAL_BUTTON);//activate pull-up

	//LIGHTS_PORT= LIGHTS_PINS_MASK;// turn all of them on

	SELECTOR_DDR=SELECTOR_PINS_MASK;//set as output
	SELECTOR_PORT=SELECTOR_PINS_MASK;//no gamepad selected, deactivate pullups

	LED_CONFIG;
	LED_ON; // power up led on startup for 1 sec

	// Initialize the USB, and then wait for the host to set configuration.
	// If the Teensy is powered without a PC connected to the USB port,
	// this will wait forever.
	usb_init();
	while (!usb_configured()) /* wait */ ;
		_delay_ms(1000);
	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	for(selected_player=0;selected_player<NUMBER_OF_INTERFACES;selected_player++)
		usb_gamepad_reset_state(selected_player); //player 0 will be reported as idle by default
	selected_player=0;
	select_gamepad(selected_player);



configure_polling_interrupt();

// {
// 	int i;
// 	for(i=4;i>=0;i--)
// 	{
// 		//play_tone(i);
// 		//wait_for_miliseconds(500);
// 		//nobeep;
// 		//wait_for(1000);
// 	}
// }

	nobeep;
	LED_OFF;
	while (1) {
		// read_gamepad_state(GAMEPAD_INTERFACE(selected_player));
		// usb_gamepad_send(GAMEPAD_INTERFACE(selected_player));
		// selected_player=(selected_player%NUMBER_OF_INTERFACES)+1;
		// select_gamepad(selected_player);
		//  wait_for(1000);
		//  LED_ON;
		//  wait_for(50);
		//  LED_OFF;

		if(is_active_pin(CENTRAL_BUTTON_PINS,CENTRAL_BUTTON))
			LIGHTS_PORT|= LIGHTS_PINS_MASK;// turn all of them on
		else
			LIGHTS_PORT&= ~LIGHTS_PINS_MASK;// turn all of them off

	}
}
// ISR(TIMER0_COMPA_vect){
// PORTD ^= (1<<6);//LED state changes
// }

volatile uint16_t pullings_counter;
volatile uint8_t green_semaphore;

 ISR(TIMER0_COMPA_vect) {
	 if(green_semaphore)
	 	pullings_counter++;
 	 read_gamepad_state(GAMEPAD_INTERFACE(selected_player));
 	 usb_gamepad_send(GAMEPAD_INTERFACE(selected_player));
	 selected_player=(selected_player+1)%(NUMBER_OF_INTERFACES-1);
	 select_gamepad(selected_player);
 }
