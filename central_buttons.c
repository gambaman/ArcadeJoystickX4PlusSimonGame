#ifndef CENTRAL_BUTTONS
#include "central_buttons.h"
#endif

void configure_central_buttons(void)
{
    LIGHTS_DDR= LIGHTS_PINS_MASK; //set as output
  	CENTRAL_BUTTON_DDR=0;//set as input
  	CENTRAL_BUTTON_PORT|=(1<<CENTRAL_BUTTON);//activate pull-up
}

void wait_till_depressed_button(uint8_t x)
{
  while(pressed_light_button(x));
  skip_bounces;
}
