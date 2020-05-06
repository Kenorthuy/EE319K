// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 1/17/2020 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "dac.h"
#include "../inc/tm4c123gh6pm.h"



// 6-bit 64-element sine wave				
const unsigned short sine[64] = {				
  32,35,38,41,44,47,49,52,54,56,58,				
  59,61,62,62,63,63,63,62,62,61,59,				
  58,56,54,52,49,47,44,41,38,35,				
  32,29,26,23,20,17,15,12,10,8,				
  6,5,3,2,2,1,1,1,2,2,3,				
  5,6,8,10,12,15,17,20,23,26,29};	

	const unsigned char Trumpet[64] = {  
  10,10,11,11,11,12,12,11,10,9,8,
  5,3,2,1,3,8,11,14,15,15,13,
  12,11,10,10,10,10,11,11,10,10,
  10,10,10,10,10,10,10,10,10,10,
  10,10,10,11,11,11,11,11,11,11,
  11,11,11,11,11,10,10,10,10,10,10,10
};  





uint16_t indeg;
uint32_t freqs;


// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void){
	DAC_Init();
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 0x00FFFFFF;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x05;
	
	indeg = 0;
	freqs = 0;
}

void SysTick_Handler(void){
	//NVIC_ST_CTRL_R &= ~(0x10000);
	DAC_Out(sine[indeg]);
	indeg = (indeg+1)%64;
	freqs++;
}

// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Play called again
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be microseconds
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
void Sound_Play(uint32_t period){
	NVIC_ST_RELOAD_R = period;
	NVIC_ST_CTRL_R |= 0x07;
	//DAC_Out(sine[indeg]);
}
