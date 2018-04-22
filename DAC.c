// DAC.c


#include <stdint.h>
#include "tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	unsigned volatile long delay;
	SYSCTL_RCGC2_R |= 0x02;
	delay = SYSCTL_RCGC2_R; //  Wait for clock to initialize
	GPIO_PORTB_DIR_R |= 0x0F; // PB0-PB3 are all outputs
	GPIO_PORTB_AFSEL_R &= 0x0F; // Turn off alternate functions
	GPIO_PORTB_DEN_R |= 0x0F; // Turn on digital enable
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R = data;
}
