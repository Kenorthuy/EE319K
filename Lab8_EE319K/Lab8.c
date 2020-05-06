// Lab8.c
// Runs on LM4F120 or TM4C123
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 1/17/2020 

// Specifications:
// Measure distance using slide pot, sample at 10 Hz
// maximum distance can be any value from 1.5 to 2cm
// minimum distance is 0 cm
// Calculate distance in fixed point, 0.01cm
// Analog Input connected to PD2=ADC5
// displays distance on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats (use them in creative ways)
// 

#include <stdint.h>

#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "../inc/tm4c123gh6pm.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on ST7735
// main3 adds your convert function, position data is no ST7735

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
	volatile int delay;
	SYSCTL_RCGCGPIO_R |= 0x28;
	delay = 0; 
	
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
	GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
	GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;
	GPIO_PORTF_DIR_R |= 0x0E;
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
	GPIO_PORTF_PUR_R |= 0x0E;
	GPIO_PORTF_DEN_R |= 0x0E;

}
uint32_t Data;        // 12-bit ADC
uint32_t Position;    // 32-bit fixed-point 0.001 cm
int main1(void){       // single step this program and look at Data
  TExaS_Init();       // Bus clock is 80 MHz 
  ADC_Init();         // turn on ADC, set channel to 5
  while(1){                
    Data = ADC_In();  // sample 12-bit channel 5
  }
}

uint32_t time0,time1,time2,time3;
uint32_t ADCtime,OutDectime; // in usec
//uint32_t Data;    	// 12-bit ADC
int main2(void){
  TExaS_Init();   	// Bus clock is 80 MHz
  NVIC_ST_RELOAD_R = 0x00FFFFFF; // maximum reload value
  NVIC_ST_CURRENT_R = 0;      	// any write to current clears it
  NVIC_ST_CTRL_R = 5;
  ADC_Init();     	// turn on ADC, set channel to 5
  
  ST7735_InitR(INITR_REDTAB);
  while(1){       	// use SysTick 
    time0 = NVIC_ST_CURRENT_R;
    Data = ADC_In();  // sample 12-bit channel 5
    time1 = NVIC_ST_CURRENT_R;
    ST7735_SetCursor(0,0);
    time2 = NVIC_ST_CURRENT_R;
    LCD_OutDec(Data);
    ST7735_OutString("	");  // spaces cover up characters from last output
    time3 = NVIC_ST_CURRENT_R;
    ADCtime = ((time0-time1)&0x0FFFFFF)/80;	// usec
    OutDectime = ((time2-time3)&0x0FFFFFF)/80; // usec
  }
}


// your function to convert ADC sample to distance (0.01cm)
uint32_t Convert(uint32_t input){
  return (((155*input)/4096) + 16); // replace this line with your Lab 8 solution
}
int main3(void){
  TExaS_Init();     	// Bus clock is 80 MHz
  ST7735_InitR(INITR_REDTAB);
  ADC_Init();     	// turn on ADC, set channel to 5
  ADC0_SAC_R = 4;   // 16-point averaging, move this line into your ADC_Init()
  while(1){  
    Data = ADC_In();  // sample 12-bit channel 5
    Position = Convert(Data);
    ST7735_SetCursor(0,0);
    LCD_OutDec(Data); ST7735_OutString("	");
    ST7735_SetCursor(6,0);
    LCD_OutFix(Position); // your Lab 7 solution
  }
}



uint32_t ADCMail;
uint8_t ADCStatus;

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 7999999;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_ST_CTRL_R = 0x07; 			// enable SysTick with core clock and interrupts
}

void SysTick_Handler(void){
	GPIO_PORTF_DATA_R ^= 0x2;
	ADCMail = ADC_In();
	ADCStatus |= 0x1;
}


int main(void){
	uint32_t pos;
  TExaS_Init();
  // your Lab 8
	PortF_Init();
	ADC_Init();
	SysTick_Init();
  ST7735_InitR(INITR_REDTAB);
  EnableInterrupts();
  while(1){
		if(ADCStatus ==1){
			ADCStatus &= ~0x1;
			pos= Convert(ADCMail);
		}
		ST7735_SetCursor(1,1);
		LCD_OutFix(pos);

  }
}
