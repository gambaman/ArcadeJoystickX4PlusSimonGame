#define SIMON

#ifndef SOUND
#include "sound.h"
#endif

uint16_t tones[6];
void configure_simon(void);
#define play_tone(x) beep(tones[x])
uint8_t simon_game(void);
extern volatile uint8_t random_value;
uint8_t wrong_button(uint8_t button);
