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

extern uint32_t credits;

// #define tone_duration1 420//miliseconds
// #define tone_duration2 320//miliseconds
// #define tone_duration3 220//miliseconds
// #define pause_duration 50//miliseconds
// #define pressing_tone_duration 100//miliseconds
// #define time_out 3000//miliseconds
// #define lossing_tone_duration 1500//miliseconds
// #define intersequence_pause_duration 800//miliseconds
//////////////////////////////////////////////////////////////////
#define tone_duration1 42//miliseconds
#define tone_duration2 32//miliseconds
#define tone_duration3 22//miliseconds
#define pause_duration 5//miliseconds
#define pressing_tone_duration 10//miliseconds
#define time_out 3000//miliseconds
#define lossing_tone_duration 1500//miliseconds
#define intersequence_pause_duration 700//miliseconds
//////////////////////////////////////////////////////////////////

uint16_t tones[6];
uint8_t sequence[31];

void configure_simon(void)
{
	configure_beeper();
	tones[0]=42;	//losing frequency in Hertzs
	tones[1]=252;		//yellow frequency in Hertzs
	tones[2]=209;	//blue frequency in Hertzs
	tones[3]=415;		//green frequency in Hertzs
	tones[4]=310;		//red frequency in Hertzs
	tones[5]=600;		//victory frequency in Hertzs
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

void play_button_five_times(uint8_t button_number)
{
	// for(uint8_t i=0;i<5;i++)//repeat last tone five times
	// 	{
	// 			wait_for_miliseconds(1);
	// 			play_button_for(button_number,0);
	// 	}
		for(uint8_t i=0;i<6;i++)
		{
			toggle_color_button_light(button_number);
			play_tone(button_number);
		}
		nobeep;
}
void play_sequence(uint8_t sequence[],uint8_t length)
{
		uint16_t beep_duration= length<=5? tone_duration1 : length<=13? tone_duration2 : tone_duration1;
		for(uint8_t i=0;i<length;i++)
		{
			play_button_for(sequence[i],beep_duration);
			wait_for_miliseconds(pause_duration);
		}
}

uint8_t rigtht_sequence(uint8_t sequence[],uint8_t length)
{
		for(uint8_t i=0;i<length;i++)
			if(wrong_button(sequence[i]))
				return 0;
		return 1;
}

uint8_t wrong_button(uint8_t button)
{
	count_miliseconds(time_out);
	//wait_till_depressed_all_color_buttons();
	while(!end_of_count)
		if(pressed_color_buttons)
		{
			if(pressed_only_button(button))
			{
				play_button_for(button,pressing_tone_duration);
				return 0;
			}
			else
				break;
		}
	LIGHTS_PORT|=(light_buttons_values<<1) & ~1;//right button not pressed
	play_tone(0);
	wait_for_miliseconds(lossing_tone_duration);
	play_button_five_times(button);
	return 1;
}

uint8_t select_skill_level(void)
{
	turn_on_all_color_button_lights;
	for(uint8_t i=0;1;i=(i+1)&3)
		if(pressed_light_button(i))
			return i+1;
}

uint8_t simon_game(void)
{
	uint8_t previous_lights_value=LIGHTS_PORT;
	uint8_t skill_level=select_skill_level();
	uint8_t current_length;
	uint8_t victory=1;																					//skill_level 0- sequence length=0 (inmediate victory)
	uint8_t sequence_length=0;																	//skill_level 1- sequence length=8
	if(skill_level)																							//skill_level 2- sequence length=14
		sequence_length= skill_level>=4? 31 : 2+6*skill_level;		//skill_level 3- sequence length=20
																															//skill_level 4 and above- sequence length=31
	for(current_length=0;victory && current_length<sequence_length;)
	{
		sequence[current_length++]=random_value;
		play_sequence(sequence,current_length);
		if(!rigtht_sequence(sequence,current_length))
			victory=0;
		wait_for_miliseconds(intersequence_pause_duration);
	}
	if(victory)
	{
			for(uint8_t i=0;i<6;i++)
			{
				toggle_central_button_light;
				play_tone(5);
			}
			nobeep;
	}
		//play_button_five_times(sequence[current_length-1]);
	LIGHTS_PORT=previous_lights_value;
	return (!victory || free_play)? 0 : skill_level>3? ~credits : 1<<((skill_level-1)<<1);
}
