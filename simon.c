#ifndef SIMON
#include "simon.h"
#endif
#ifndef SOUND
#include "sound.h"
#endif
#ifndef CENTRAL_BUTTONS
#include "central_buttons.h"
#endif
#ifndef TIMING
#include "timing.h"
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

void play_button(uint8_t button_number)
{
	turn_off_color_button_lights;
	turn_on_color_button_light(button_number);
	play_tone(button_number+1);
}

void play_button_for(uint8_t button_number,uint16_t time)
{
	play_button(button_number);
	wait_for_miliseconds(time);
	turn_off_color_button_lights;
	nobeep;
}

void play_sequence(uint8_t sequence[],uint8_t length)
{
		uint16_t beep_duration= length<=5? 420 : length<=13? 320 : 200;//miliseconds
		for(uint8_t i=0;i<length;i++)
		{
			play_button_for(sequence[i],beep_duration);
			wait_for_miliseconds(50);
		}
}

uint8_t simon_game(uint8_t skill_level)
{
	uint8_t previous_lights_value=LIGHTS_PORT;
	uint8_t sequence[31];
	uint8_t victory=1;																					//skill_level 0- sequence length=0 (inmediate victory)
	uint8_t sequence_length=0;																	//skill_level 1- sequence length=8
	if(skill_level)																							//skill_level 2- sequence length=14
		sequence_length= skill_level>=4? 31 : 2+6*skill_level;		//skill_level 3- sequence length=20
																															//skill_level 4 and above- sequence length=31
	for(uint8_t current_length=0;victory && current_length<sequence_length;)
	{
		sequence[current_length++]=random_value;
		play_sequence(sequence,current_length);
		wait_for_miliseconds(1000);//for debugging
		// for(i=0;victory && i<current_length;i++)
		// 	victory=pressed_correct_button(i);
	}
	LIGHTS_PORT=previous_lights_value;
	return victory;
}
