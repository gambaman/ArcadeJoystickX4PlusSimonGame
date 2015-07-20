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

#define SNES_DDR	DDRF
#define SNES_PORT	PORTF
#define SNES_PINS	PINF

void read_snes_state() {
	uint8_t cached_pins=~SNES_PINS;
	// 12 cycles to read 4 directions and 8 buttons
	for (int i=0; i<12; i++) {
#define on(x) ((cached_pins >> x) & 1)
		switch (i) {
		case 0: gamepad_state.b_btn = 0x1; break;
		case 1: gamepad_state.y_btn = 0x1; break;
		case 2: gamepad_state.select_btn = on(i); break;
		case 3: gamepad_state.start_btn = on(i); break;

		case 4: if (on(i)) gamepad_state.y_axis = 0x00; break;
		case 5: if (on(i)) gamepad_state.y_axis = 0xff; break;
		case 6: if (on(i)) gamepad_state.x_axis = 0x00; break;
		case 7: if (on(i)) gamepad_state.x_axis = 0xff; break;

		case 8: gamepad_state.a_btn = on(i); break;
		case 9: gamepad_state.x_btn = on(i); break;
		case 10: gamepad_state.l_btn = on(i); break;
		case 11: gamepad_state.r_btn = on(i); break;
		}
	}

}

int main(void) {
	// set for 16 MHz clock
	CPU_PRESCALE(0);

	// good explenation of how AVR pins work:
	// http://www.pjrc.com/teensy/pins.html

	SNES_DDR=  0; //set as input
	SNES_PORT=  0xff; MCUCR&=~(1<<4); //activate pullup

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
