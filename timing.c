/* Copyright (C) 2015 David Guerrero
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

#ifndef TIMING
#include "timing.h"
#endif

#include <avr/io.h>

void configure_clock(void)
{
	TCCR3A=_BV(WGM32);
	TCCR3B=_BV(WGM33)|_BV(WGM32); //Compare match with ICR3
}
#define timing_clock_base_frequency 15625UL// 	by executing TCCR3B|=5;	the frequency will be divided by 1024 (15625 Hz)

void count_miliseconds(uint16_t miliseconds)
{
	TCCR3B&= ~7;//stop the counter
	ICR3= (timing_clock_base_frequency*miliseconds)/1000-1;
	TCNT3=0;     // set timer3 counter initial value to 0
	TIFR3|=1<<3;  // clear the compare match flag
	TCCR3B|=5;	//Start counting dividing frequency by 1024 (15625 Hz)
}

void wait_for_miliseconds(uint16_t miliseconds_delay)
{
	count_miliseconds(miliseconds_delay);
	while(!end_of_count);
}
