/* Teensy SNES Gamepad
 * Copyright (C) 2011 Josh Kropf <josh@slashdev.ca>
 *
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
#include <util/delay.h>
#include "usb_gamepad.h"

#define LED_CONFIG  (DDRD |= (1<<6))
#define LED_OFF     (PORTD &= ~(1<<6))
#define LED_ON      (PORTD |= (1<<6))

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

#define DIRECTION_DDR DDRC
#define DIRECTION_PORT PORTC
#define DIRECTION_PINS PINC

#define UP_PIN 4
#define DOWN_PIN 5
#define LEFT_PIN 6
#define RIGHT_PIN 7
#define DIRECTION_PINS_MASK ((1<<UP_PIN)|(1<<DOWN_PIN)|(1<<RIGHT_PIN)|(1<<LEFT_PIN))

#define is_active_pin(port,pin) (!(port&(1<<pin)))

#define BUTTONS_DDR	DDRF
#define BUTTONS_PORT	PORTF
#define BUTTONS_PINS	PINF

void read_snes_state(void) {
	gamepad_state.buttons=~BUTTONS_PINS;
	gamepad_state.y_axis =	(is_active_pin(DIRECTION_PINS,UP_PIN)?0:0x80)
												+ (is_active_pin(DIRECTION_PINS,DOWN_PIN)?0x7f:0);
	gamepad_state.x_axis = 	(is_active_pin(DIRECTION_PINS,RIGHT_PIN)?0X7F:0)
												+ (is_active_pin(DIRECTION_PINS,LEFT_PIN)?0:0x80);
	/*
		case 4: if (on(i)) gamepad_state.y_axis = 0x00; break;
		case 5: if (on(i)) gamepad_state.y_axis = 0xff; break;
		case 6: if (on(i)) gamepad_state.x_axis = 0x00; break;
		case 7: if (on(i)) gamepad_state.x_axis = 0xff; break;
*/
		}


int main(void) {
	// set for 16 MHz clock
	CPU_PRESCALE(0);

	// good explenation of how AVR pins work:
	// http://www.pjrc.com/teensy/pins.html

	BUTTONS_DDR=  0;DIRECTION_DDR=  0; //set as input
	BUTTONS_PORT=  0xff; DIRECTION_PORT|=DIRECTION_PINS_MASK; MCUCR&=~(1<<4); //activate pullup

	LED_CONFIG;
	LED_ON; // power up led on startup for 1 sec

	// Initialize the USB, and then wait for the host to set configuration.
	// If the Teensy is powered without a PC connected to the USB port,
	// this will wait forever.
	usb_init();
	while (!usb_configured()) /* wait */ ;

	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	_delay_ms(1000);

	LED_OFF;
/*
while(1)
	if((SNES_PINS&1))
		LED_ON;
	else
		LED_OFF;*/
	while (1) {
		usb_gamepad_reset_state();
		read_snes_state();
		usb_gamepad_send();
	}
}
