// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: March 30, 2018
// Last Modified:  change this or look silly
// Lab number: 7


#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include <stdint.h>

//------------IO_Init------------
// Initialize GPIO Port for a switch and an LED
// Input: none
// Output: none
void IO_Init(void) {
	volatile int delay;
	SYSCTL_RCGCGPIO_R |= 0x20;
	delay = 0; 
	GPIO_PORTF_DIR_R &= ~(0x10);
	GPIO_PORTF_DIR_R |= 0x4;
	GPIO_PORTF_PUR_R |= 0x10;
	GPIO_PORTF_DEN_R |= 0x14;
	
 // --UUU-- Code to initialize PF4 and PF2
}

//------------IO_HeartBeat------------
// Toggle the output state of the  LED.
// Input: none
// Output: none
void IO_HeartBeat(void) {
	GPIO_PORTF_DATA_R ^= 0x4;
}


//------------IO_Touch------------
// wait for release and press of the switch
// Delay to debounce the switch
// Input: none
// Output: none
void IO_Touch(void) {
	while((GPIO_PORTF_DATA_R & 0x10) == 0x10) {				//wait for release
	}
	Delay1ms(20);																			//Delay1ms 20ms
	while((GPIO_PORTF_DATA_R & 0x10) == 0) {					//wait for press
	}
	
 // --UUU-- wait for release; delay for 20ms; and then wait for press
}  

