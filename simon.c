#ifndef SIMON
#include "simon.h"
#endif

#ifndef SOUND
#include "sound.h"
#endif

uint16_t tones[5];

void configure_simon(void)
{
	configure_beeper();
	tones[0]=42;	//losing frequency in Hertzs
	tones[1]=209;	//blue frequency in Hertzs
	tones[2]=252;		//yellow frequency in Hertzs
	tones[3]=415;		//green frequency in Hertzs
	tones[4]=310;		//red frequency in Hertzs
}
