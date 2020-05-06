;****************** main.s ***************
; Program written by: ***Your Names**update this***
; Date Created: 2/4/2017
; Last Modified: 1/17/2020
; Brief description of the program
;   The LED toggles at 2 Hz and a varying duty-cycle
; Hardware connections (External: One button and one LED)
;  PE1 is Button input  (1 means pressed, 0 means not pressed)
;  PE2 is LED output (1 activates external LED on protoboard)
;  PF4 is builtin button SW1 on Launchpad (Internal) 
;        Negative Logic (0 means pressed, 1 means not pressed)
; Overall functionality of this system is to operate like this
;   1) Make PE2 an output and make PE1 and PF4 inputs.
;   2) The system starts with the the LED toggling at 2Hz,
;      which is 2 times per second with a duty-cycle of 30%.
;      Therefore, the LED is ON for 150ms and off for 350 ms.
;   3) When the button (PE1) is pressed-and-released increase
;      the duty cycle by 20% (modulo 100%). Therefore for each
;      press-and-release the duty cycle changes from 30% to 70% to 70%
;      to 90% to 10% to 30% so on
;   4) Implement a "breathing LED" when SW1 (PF4) on the Launchpad is pressed:
;      a) Be creative and play around with what "breathing" means.
;         An example of "breathing" is most computers power LED in sleep mode
;         (e.g., https://www.youtube.com/watch?v=ZT6siXyIjvQ).
;      b) When (PF4) is released while in breathing mode, resume blinking at 2Hz.
;         The duty cycle can either match the most recent duty-
;         cycle or reset to 30%.
;      TIP: debugging the breathing LED algorithm using the real board.
; PortE device registers
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_AFSEL_R EQU 0x40024420
GPIO_PORTE_DEN_R   EQU 0x4002451C
; PortF device registers
GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_AFSEL_R EQU 0x40025420	;
GPIO_PORTF_PUR_R   EQU 0x40025510	;pull up
GPIO_PORTF_DEN_R   EQU 0x4002551C
GPIO_PORTF_LOCK_R  EQU 0x40025520	;
GPIO_PORTF_CR_R    EQU 0x40025524	;
GPIO_LOCK_KEY      EQU 0x4C4F434B  ; Unlocks the GPIO_CR register
SYSCTL_RCGCGPIO_R  EQU 0x400FE608

		IMPORT	TExaS_Init
		THUMB
		AREA	DATA, ALIGN=2
curdel								;this stores the duty cycle for standard delays
		SPACE	2
helldel 							;this stores the period length for breathing. a higher value means the light is dimmer
		SPACE	4
increasing							;this is a boolean value for determining whether the breathing cycle is brightening or dimming. 1 means that helldel increases, 0 means it decreases
		SPACE	1

		AREA    |.text|, CODE, READONLY, ALIGN=2
		THUMB
		EXPORT  Start
Start
 ; TExaS_Init sets bus clock at 80 MHz
		BL  	TExaS_Init ; voltmeter, scope on PD3
		
		LDR		R1,=SYSCTL_RCGCGPIO_R		;enable clock
		LDR		R0, [R1]
		ORR		R0, #0x30
		STR		R0, [R1]
		
		NOP
		NOP
		
		LDR		R1,=GPIO_PORTE_DIR_R
		LDR		R0, [R1]
		ORR		R0, #0x4					;make PE2 output
		BIC		R0, #0x2					;make PE1 input
		STR		R0, [R1]
		
		LDR		R1,=GPIO_PORTE_DEN_R		;enable PE1 and PE2
		LDR		R0, [R1]
		ORR		R0, #0x6
		STR		R0, [R1]
		
		LDR		R1,=GPIO_PORTF_DIR_R
		LDR		R0, [R1]
		BIC		R0, #0x10					;make PF4 input
		STR		R0, [R1]
		
		LDR		R1,=GPIO_PORTF_AFSEL_R		;enable PF4
		LDR		R0, [R1]
		BIC		R0, #0x0
		STR		R0, [R1]
		
		LDR		R1,=GPIO_PORTF_LOCK_R		;enable PF4
		LDR		R0,=GPIO_LOCK_KEY
		STR		R0, [R1]
		
		LDR		R1,=GPIO_PORTF_CR_R			;enable PF4
		LDR		R0, [R1]
		ORR		R0, #0x11
		STR		R0, [R1]
		
		LDR		R1,=GPIO_PORTF_PUR_R		;enable PF4
		LDR		R0, [R1]
		ORR		R0, #0x11
		STR		R0, [R1]
		
		LDR		R1,=GPIO_PORTF_DEN_R		;enable PF4
		LDR		R0, [R1]
		ORR		R0, #0x10
		STR		R0, [R1]
		
		
		
		
		
		LDR		R7, =curdel					;R7 permanently holds the location for curdel
		LDR		R9, =helldel				;R9, helldel
		LDR		R11, =increasing			;R11, increasing

		;enable interrupts

		CPSIE	I    ; TExaS voltmeter, scope runs on interrupts
		
		MOV		R6, #30						;store initial duty cycle of 30% to the variable curdel
		STRH	R6, [R7]
		LDR		R8, =10000					;store initial period of 1000000 to helldel. this makes it start out dim
		STR		R8, [R9]
		MOV		R10, #0						;store 0 to increasing so that the breathing code starts out with an decreasing period, and gets brighter
		STRB	R10, [R11]
		
		
		
		
		
loop  										;here is where the main code begins
		LDR		R1, =GPIO_PORTE_DATA_R		;R1 will always have porte data
		LDR		R4, =GPIO_PORTF_DATA_R		;R4 will always have portf data
		LDR		R0, [R1]
		
		
		CMP		R0, #0x02					;pull port e data, see if the button is *pressed*
		BNE		normloop
		
prepress
		ORR		R0, #0x04					;if pressed, hold light on high
		STR		R0, [R1]
press
		LDR		R0, [R1]
		;CMP		R0, #0x00				;this line is for the simulator due to bugs in the board
		CMP		R0, #0x04					;no longer being pressed?
		BNE		skipp
				
		LDRH	R6, [R7]					;pull curdel value, add 20 to it. modulus by checking value and subtracting 90 if greater than 90
		ADD		R6, #20
		CMP		R6, #90
		BLE		skipmod						;if not greater than 90, do not apply modulus (do not subtract 100)
		SUB		R6, #100
skipmod 
		STRH	R6, [R7]					;now store the changed curdel value and return to loop
		B		loop
skipp
		B		press						;this branch only fires if the button continues to be held
		
		
		
normloop		
		LDR		R0, [R1]					;turn the light on
		ORR		R0, #0x4
		STR		R0, [R1]
		
delay1
		LDRH	R6, [R7]					;pull curdel value, plug into formula to find number of cycles for the high state
		LDR		R5, =120000					;this ones for the board
		;LDR		R5, =100000				;this ones for the simulator
		MUL		R6, R5
dlop1										;waste time until R6 reaches 0
		SUBS	R6, #1
		BNE		dlop1
		
		LDR		R0, [R1]					;now turn the light off
		BIC		R0, #0x4
		STR		R0, [R1]
		
delay2
		LDRH	R6, [R7]					;pull curdel value, but subtract it from 100 to get the "negative" side of the duty cycle
		LDR		R5, =120000					;this ones for the board
		;LDR		R5, =100000				;this ones for the simulator
		EOR		R6, #0xFFFFFFFF
		ADD		R6, #1
		ADD		R6, R6, #100
		MUL		R6, R5
dlop2										;waste time until R6 reaches 0
		SUBS	R6, #1
		BNE		dlop2
		
		
	
		
											;this is where the breathing code starts
		
		
		
		LDR		R8, [R9]					;these lines reset the breathing cycle	
		LDR		R8, =10000
		STR		R8, [R9]
		
		MOV		R10, #0x0					;this determines whether the duty cycle is increasing or decreasing. 
											;initialized to zero to decrease the period and increase the brightness
		
breath
		LDR		R3, [R4]
		CMP		R3, #0x10					;since PF4 is negative logic, see if the flag for it is high. if it is, the button is not being pressed and we return to loop
		BEQ		loop
		
		LDR		R0, [R1]					;turns on led
		ORR		R0, #0x04
		STR		R0, [R1]
		
brdelay1
		LDR		R8, =1000					;the on cycle is fixed
brdlop1
		SUBS	R8, #1
		BNE		brdlop1
		
		LDR		R0, [R1]					;turns off led
		BIC		R0, #0x04
		STR		R0, [R1]
		
brdelay2
		LDR		R8, [R9]					;pull helldel value and waste time for the off cycle. if its 0 it reroutes directly to setting R10
		CMP		R8, #0
		BEQ		review
brdlop2										
		SUBS	R8, #1
		BNE		brdlop2

review
		LDR		R8, [R9]					;check if the state of increasing or decreasing need to be changed
		CMP		R8, #0
		BEQ		increaseI
		LDR		R12, =10000
		CMP		R8, R12
		BEQ		decreaseI
		B		checkI
		
increaseI
		LDRB	R10, [R11]					;if helldel reaches 0, it needs to be increased
		MOV		R10, #0x1
		STRB	R10, [R11]
		B		checkI
		
decreaseI
		LDRB	R10, [R11]					;if helldel reaches 10000, it needs to be decreased
		MOV		R10, #0x0
		STRB	R10, [R11]
		B		checkI
		
checkI
		LDRB	R10, [R11]					;now check whether the period should be decreased or increased. if its 0, go to moveup and decrease helldel. otherwise go to movedown and decrease it
		CMP		R10, #0
		BEQ		moveup
		B		movedown

moveup	
		LDR		R8, [R9]
		SUB		R8, #5
		B		breathstore
		
movedown
		LDR		R8, [R9]
		ADD		R8, #5
		B		breathstore
		
breathstore
		STR		R8, [R9]
		B		breath

		
		ALIGN      ; make sure the end of this section is aligned
		END        ; end of file