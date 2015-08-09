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

#ifndef SOUND
#include "sound.h"
#endif

#include <avr/io.h>

void configure_beeper(void)
{
	TCCR1A=_BV(COM1C0);//toggle oc1c on compare match
}

#define beeper_base_fequency 7812 //Hertz

void beep(uint16_t frequency)
{
	uint16_t ICR1_value=beeper_base_fequency/frequency;
	TCCR1B=0; //disable timer
	ICR1H=ICR1_value>>8;
	ICR1L=ICR1_value&255;
	TCNT1=0;     // set timer0 counter initial value to 0
	DDRB|=1<<7;
	TCCR1B=_BV(WGM13)|_BV(WGM12)|5;//clear timer on compare match with ICR1|divide frequency by 64 (15625 Khz=>waveform of 7812 Khz)
}
