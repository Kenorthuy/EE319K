// StepperMotorController.c starter file EE319K Lab 5
// Runs on TM4C123
// Finite state machine to operate a stepper motor.  
// Jonathan Valvano
// 1/17/2020

// Hardware connections (External: two input buttons, five outputs to stepper motor, and one LED)
// PA5 is Wash input (1 means pressed, 0 means not pressed)
// PA4 is Wiper input (1 means pressed, 0 means not pressed)
// PE5 is Water pump output LED (toggle means washing)
// PE4-0 are stepper motor outputs
// PF1 PF2 or PF3 control the LED on Launchpad used as a heartbeat

#include "SysTick.h"
#include "TExaS.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#define HOME 0
#define WIPE1 1
#define WIPE2 2
#define WIPE3 3
#define WIPE4 4
#define WIPE5 5
#define WIPE6 6
#define WIPE7 7
#define WIPE8 8
#define WIPE9 9
#define WIPE10 10
#define WIPE11 11
#define WIPE12 12
#define WIPE13 13
#define WIPE14 14
#define WIPE15 15
#define WIPE16 16
#define WIPE17 17
#define WIPE18 18
#define WIPE19 19
#define WASH1 20
#define WASH2 21
#define WASH3 22
#define WASH4 23
#define WASH5 24
#define WASH6 25
#define WASH7 26
#define WASH8 27
#define WASH9 28
#define WASH10 29


typedef struct State {
		uint8_t Next[4];
		uint8_t output;
		uint32_t wait;
} State_t;

State_t FSM[30] = {
		{{HOME,WIPE1,WASH1,WASH1}, 			0x01, 4000000},		//HOME
		
		{{WIPE2,WIPE2,WIPE2,WIPE2}, 		0x02, 4000000},		//WIPE1
		{{WIPE3,WIPE3,WASH2,WASH2}, 		0x04, 4000000},		//WIPE2
		
		{{WIPE4,WIPE4,WIPE4,WIPE4}, 		0x08, 4000000},		//WIPE3
		{{WIPE5,WIPE5,WASH3,WASH3}, 		0x10, 4000000},		//WIPE4
		
		{{WIPE6,WIPE6,WIPE6,WIPE6}, 		0x01, 4000000},		//WIPE5
		{{WIPE7,WIPE7,WASH4,WASH4}, 		0x02, 4000000},		//WIPE6
		
		{{WIPE8,WIPE8,WIPE8,WIPE8}, 		0x04, 4000000},		//WIPE7
		{{WIPE9,WIPE9,WASH5,WASH5}, 		0x08, 4000000},		//WIPE8
		
		{{WIPE10,WIPE10,WIPE10,WIPE10}, 0x10, 4000000},		//WIPE9
		{{WIPE11,WIPE11,WASH6,WASH6}, 	0x01, 4000000},		//WIPE10
		
		{{WIPE12,WIPE12,WIPE12,WIPE12}, 0x10, 4000000},		//WIPE11
		{{WIPE13,WIPE13,WASH7,WASH7}, 	0x08, 4000000},		//WIPE12
		
		{{WIPE14,WIPE14,WIPE14,WIPE14}, 0x04, 4000000},		//WIPE13
		{{WIPE15,WIPE15,WASH8,WASH8}, 	0x02, 4000000},		//WIPE14
		
		{{WIPE16,WIPE16,WIPE16,WIPE16}, 0x01, 4000000},		//WIPE15
		{{WIPE17,WIPE17,WASH9,WASH9}, 	0x10, 4000000},		//WIPE16
		
		{{WIPE18,WIPE18,WIPE18,WIPE18}, 0x08, 4000000},		//WIPE17
		{{WIPE19,WIPE19,WASH10,WASH10}, 0x04, 4000000},		//WIPE18
		
		{{HOME,HOME,HOME,HOME},					0x02, 4000000},		//WIPE19
		
		{{WIPE2,WIPE2,WIPE2,WIPE2}, 		0x22, 4000000},		//WASH1
		{{WIPE4,WIPE4,WIPE4,WIPE4}, 		0x28, 4000000},		//WASH2
		
		{{WIPE6,WIPE6,WIPE6,WIPE6}, 		0x21, 4000000},		//WASH3
		{{WIPE8,WIPE8,WIPE8,WIPE8}, 		0x24, 4000000},		//WASH4
		
		{{WIPE10,WIPE10,WIPE10,WIPE10}, 0x30, 4000000},		//WASH5
		{{WIPE12,WIPE12,WIPE12,WIPE12}, 0x30, 4000000},		//WASH6
		
		{{WIPE14,WIPE14,WIPE14,WIPE14}, 0x24, 4000000},		//WASH7
		{{WIPE16,WIPE16,WIPE16,WIPE16}, 0x21, 4000000},		//WASH8
		
		{{WIPE18,WIPE18,WIPE18,WIPE18}, 0x28, 4000000},		//WASH9
		{{HOME,HOME,HOME,HOME}, 				0x22, 4000000}			//WASH10
	};

void EnableInterrupts(void);
// edit the following only if you need to move pins from PA4, PE3-0      
// logic analyzer on the real board
#define PA4       (*((volatile unsigned long *)0x40004040))
#define PE50      (*((volatile unsigned long *)0x400240FC))
void SendDataToLogicAnalyzer(void){
  UART0_DR_R = 0x80|(PA4<<2)|PE50;
}

int main(void){
	volatile uint8_t currst;
	uint8_t inputs = 0;
	
	SYSCTL_RCGCGPIO_R |= 0x31;
	
	currst = 0;
	
	GPIO_PORTA_DIR_R &= ~(0x30);
	GPIO_PORTA_DEN_R |= (0x30);
	
	GPIO_PORTE_DIR_R |= (0x3F);
	GPIO_PORTE_DEN_R |= (0x3F);
	
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R		= 0x1F;
	
	GPIO_PORTF_DIR_R |= (0x02);
	GPIO_PORTF_DEN_R |= (0x02);
	
  TExaS_Init(&SendDataToLogicAnalyzer);    // activate logic analyzer and set system clock to 80 MHz
  SysTick_Init();   
	
  EnableInterrupts();  
  while(1){
		uint32_t temp = GPIO_PORTE_DATA_R & 0xc0;
		GPIO_PORTF_DATA_R	^= 0x02;
		temp |= FSM[currst].output;
		//output
		GPIO_PORTE_DATA_R = temp;
		//delay
		SysTick_Wait((FSM[currst].wait));
		//input
		inputs = (GPIO_PORTA_DATA_R & 0x30) >> 4;
		//next		
		currst = FSM[currst].Next[inputs];
  }
}


