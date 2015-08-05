#ifndef SOUND
#include "sound.h"
#endif

uint16_t tones[5];
void configure_simon(void);
#define play_tone(x) beep(tones[x])
