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

#define CENTRAL_BUTTONS
#include <avr/io.h>

#ifndef usb_gamepad_h__
#include "usb_gamepad.h"
#endif

#ifndef TIMING
#include "timing.h"
#endif

#define CENTRAL_BUTTON_DDR DDRE
#define CENTRAL_BUTTON_PORT PORTE
#define CENTRAL_BUTTON_PINS PINE
#define CENTRAL_BUTTON 6

#define LIGHTS_DDR	DDRD
#define LIGHTS_PORT	PORTD
#define LIGHTS_PINS	PIND
#define LIGHTS_PINS_MASK ((1<<(NUMBER_OF_INTERFACES+1))-1) //There is a LIGHT per player plus the central one
#define COLOR_PINS_MASK ((1<<(NUMBER_OF_INTERFACES))-1) //There is a LIGHT per player

extern volatile uint8_t light_buttons_values;

#define pressed_light_button(x) (light_buttons_values&(1<<x))
#define pressed_only_button(x) (light_buttons_values==1<<x)
#define pressed_color_buttons (light_buttons_values & COLOR_PINS_MASK)
//#define pressed_central_button pressed_light_button (VIRTUAL_GAMEPAD_ID)
#define pressed_central_button (~CENTRAL_BUTTON_PINS & (1<<CENTRAL_BUTTON))
#define turn_on_color_button_light(x) LIGHTS_PORT|= 1<<(x+1)
#define toggle_color_button_light(x) LIGHTS_PORT^= 1<<(x+1)
#define turn_on_central_button_light LIGHTS_PORT|= 1
#define toggle_central_button_light LIGHTS_PORT^= 1
#define turn_off_color_button_lights LIGHTS_PORT&= ~(((1<<(NUMBER_OF_INTERFACES))-1)<<1)
#define turn_on_all_color_button_lights LIGHTS_PORT|= (((1<<(NUMBER_OF_INTERFACES))-1)<<1)
#define turn_off_central_button_light LIGHTS_PORT&= ~1
#define skip_bounces wait_for_miliseconds(50)

void configure_central_buttons(void);
void wait_till_depressed_button(uint8_t x);
void wait_till_depressed_all_color_buttons(void);
