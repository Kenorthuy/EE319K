//music.c
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
//#include "Timer0A.h"
#include "Sound.h"
#include "music.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

// ***************** Timer0A_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none

note_t *songPt;

uint8_t playing;
uint32_t notes;

void Timer0A_Init(uint32_t period){
	long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = period-1;    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
  EndCritical(sr);
	
	playing = 0;
	notes = 0;
}

void Music_Init(void){

	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
	GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
	GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;
	GPIO_PORTF_DIR_R &= ~(0x11);
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
	GPIO_PORTF_PUR_R |= 0x11;
	GPIO_PORTF_DEN_R |= 0x15;
	Timer0A_Init(80000);	
	TIMER0_IMR_R = 0; //turns off the timer
	songPt = NULL;
}

void Music_Play(note_t song[]){
	songPt=song;
	TIMER0_IMR_R = 0x1; 									//turns on the timer
	//NVIC_EN0_R = 1<<19;
	while(TIMER0_IMR_R == 0x1){
	if((playing ==1) && (GPIO_PORTF_DATA_R & 0x1)== 0x0){
			Music_Stop();
		}
	}
}

void Music_Stop(void){
			playing = 0;
			TIMER0_IMR_R = 0x00000000;
		}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
	if((playing == 1) && songPt != NULL && songPt->length != 0){
		NVIC_ST_CTRL_R = 0;
		Sound_Play(songPt->pitch);
		TIMER0_TAILR_R = songPt->length;    // 4) reload value
		songPt++;
		notes++;
	}else{
		  TIMER0_IMR_R = 0x00000000;    // disarm timer
			playing = 0;
			NVIC_ST_CTRL_R = 0;
			//NVIC_DIS0_R = 1<<19;
	}
	
}

