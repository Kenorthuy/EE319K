;****************** main.s ***************
; Program initially written by: Yerraballi and Valvano
; Author: Place your name here
; Date Created: 1/15/2018 
; Last Modified: 1/17/2020 
; Brief description of the program: Solution to Lab1
; The objective of this system is to implement odd-bit counting system
; Hardware connections: 
;  Output is positive logic, 1 turns on the LED, 0 turns off the LED
;  Inputs are negative logic, meaning switch not pressed is 1, pressed is 0
;    PE0 is an input 
;    PE1 is an input 
;    PE2 is an input 
;    PE3 is the output
; Overall goal: 
;   Make the output 1 if there is an odd number of switches pressed, 
;     otherwise make the output 0

; The specific operation of this system 
;   Initialize Port E to make PE0,PE1,PE2 inputs and PE3 an output
;   Over and over, read the inputs, calculate the result and set the output
; PE2  PE1 PE0  | PE3
; 0    0    0   |  1    odd number of 0’s
; 0    0    1   |  0    even number of 0’s
; 0    1    0   |  0    even number of 0’s
; 0    1    1   |  1    odd number of 0’s
; 1    0    0   |  0    even number of 0’s
; 1    0    1   |  1    odd number of 0’s
; 1    1    0   |  1    odd number of 0’s
; 1    1    1   |  0    even number of 0’s
;There are 8 valid output values for Port E: 0x01,0x02,0x04,0x07,0x08,0x0B,0x0D, and 0x0E. 

; NOTE: Do not use any conditional branches in your solution. 
;       We want you to think of the solution in terms of logical and shift operations

GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_DEN_R   EQU 0x4002451C
SYSCTL_RCGCGPIO_R  EQU 0x400FE608

       THUMB
       AREA    DATA, ALIGN=2
;global variables go here
      ALIGN
      AREA    |.text|, CODE, READONLY, ALIGN=2
      EXPORT  Start
Start
    ;code to run once that initializes PE3,PE2,PE1,PE0
	
	;turn on clock of port E
	LDR	R0,	= SYSCTL_RCGCGPIO_R
	LDRB	R1, [R0]
	ORR	R1, #0x10
	STRB	R1, [R0]
	
	NOP
	NOP
	
	;set direction of the needed ports
	LDR	R0, = GPIO_PORTE_DIR_R
	LDRB R1, [R0]
	AND R1, #0xF8
	ORR	R1, #0x08
	STRB R1, [R0]

	;digital enable all pins
	LDR	R0, = GPIO_PORTE_DEN_R
	LDRB R1, [R0]
	ORR	R1, #0x0F
	STRB R1, [R0]
	
loop
    ;code that runs over and over
	
	;read the data register B [2,0] and put each of its bits into a separate register
	; R2=portE [0]
	; R3=portE [1]
	; R4=port# [2]
	LDR R0, = GPIO_PORTE_DATA_R
	LDR R1, [R0]
	AND R2, R1, #0x01
	AND R3, R1, #0x02
	LSR R3, R3, #1
	AND R4, R1, #0x04
	LSR R4, R4, #2

	;EOR operation of each bit
	EOR R5, R2, R3
	EOR R5, R4, R5
	EOR R5, #1
	LSL R5, #3
	
	;clear PortE data [3] and store modified register
	AND R1, #0xF7
	ORR R1, R1, R5
	STR R1, [R0]
	
    B    loop


    
      ALIGN        ; make sure the end of this section is aligned
      END          ; end of file
          