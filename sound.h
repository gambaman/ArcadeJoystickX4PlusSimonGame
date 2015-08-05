#define SOUND

#include <stdint.h>

void configure_beeper(void);
void beep(uint16_t frequency);
#define nobeep 	TCCR1B=0
