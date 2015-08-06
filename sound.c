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
	TCCR1B=0; //disable timer
	ICR1=beeper_base_fequency/frequency; // toggles when reach ICR1
	TCNT1=0;     // set timer0 counter initial value to 0
	DDRB|=1<<7;
	TCCR1B=_BV(WGM13)|_BV(WGM12)|5;//clear timer on compare match with ICR1|divide frequency by 64 (15625 Khz=>waveform of 7812 Khz)
}
