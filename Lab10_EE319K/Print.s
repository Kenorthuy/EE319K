; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  
n	EQU		0
;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
;push all the registers
	;push each number as it comes up
	;the n value will be frame pointed to for both computation and to point where the stack began
	PUSH	{R0, LR}				;n is initialized to the input variable. size is just allocated
	MOV		R12, SP					;R12 is now a frame pointer
	MOV		R2, #10					;this is for decimal modulus math
	
	CMP		R0, #0					;handle the zero edgecase
	BNE		decbegin
	ADD		R0, R0, #0x30			;this will just print zero and end the printing
	BL	  	ST7735_OutChar
	POP		{R0, LR}				;deallocates everything
	BX		LR
decbegin
	LDR		R0, [R12, #n]			;load from the memory location
	CMP		R0, #0
	BEQ		decprinter				;if its zero, we are done
	
	UDIV	R1, R0, R2				;divide first, and integer division will roun down
	MUL		R1, R1, R2				;multiply. this number will be less than or equal to R0
	SUB		R1, R0, R1				;subtract R1 from R0 for the remainder. that mod is stored to R1
	ADD		R1, R1, #0x30			;add the mod to get an ascii character
	PUSH 	{R1, R2}				;push onto the stack for easy navigation. R2 is pushed for alignment
	
	UDIV	R0, R0, R2				;divide n by 10 and store it into its memory spot. this readies up the next digit
	STR		R0, [R12, #n]
	B		decbegin

decprinter
	CMP		R12, SP					;if the frame pointer is equal to the stack pointer, all the digits were pushed and printed
	BEQ		decexit
									;the top of the stack has the most significant digit
	POP		{R0, R2}				;pushing to E2 also for alignment
	BL	  	ST7735_OutChar
	B		decprinter

decexit
	POP		{R0, LR}				;pop the variable and the LR, which was changed when going to outchar
      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.01, range 0.00 to 9.99
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.00 "
;       R0=3,    then output "0.03 "
;       R0=89,   then output "0.89 "
;       R0=123,  then output "1.23 "
;       R0=999,  then output "9.99 "
;       R0>999,  then output "*.** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
    ;push all the registers
	;push each number as it comes up
	;the n value will be frame pointed to for both computation and to point where the stack began
	PUSH	{R0, LR}				;n is initialized to the input variable
	MOV		R12, SP					;R12 is now a frame pointer
	MOV		R2, #1000				;this will be to make sure the numbers not out of bounds
	MOV		R3, #3					;a counter for checking where the decimal should go
	
	CMP		R0, #0					;handle the zero edgecase
	BNE		checkbig
	ADD		R0, R0, #0x30			;this will just print 0.00 and end the printing
	BL	  	ST7735_OutChar
	MOV		R0, #0x2E
	BL	  	ST7735_OutChar
	MOV		R0, #0x30
	BL	  	ST7735_OutChar
	MOV		R0, #0x30
	BL	  	ST7735_OutChar
	POP		{R0, LR}				;deallocates everything
	BX		LR

checkbig
	CMP		R0, R2					;handle the max value edgecase
	BLO		fixbegin
	MOV		R0, #0x2A				;this will just print *.** and end the printing
	BL	  	ST7735_OutChar
	MOV		R0, #0x2E
	BL	  	ST7735_OutChar
	MOV		R0, #0x2A
	BL	  	ST7735_OutChar
	MOV		R0, #0x2A
	BL	  	ST7735_OutChar
	POP		{R0, LR}				;deallocates everything
	BX		LR
	
	
fixbegin
	MOV		R2, #10					;for decimal modulus
	LDR		R0, [R12, #n]
	CMP		R0, #0
	BEQ		fixfiller
	
	UDIV	R1, R0, R2				;divide first, and integer division will roun down
	MUL		R1, R1, R2				;multiply. this number will be less than or equal to R0
	SUB		R1, R0, R1				;subtract R1 from R0 for the remainder. that mod is stored to R1
	ADD		R1, R1, #0x30			;add the mod to get an ascii character
	PUSH 	{R1, R2}				;push onto the stack for easy navigation. R2 is pushed for alignment
	
	UDIV	R0, R0, R2				;divide n by 10 and store it into its memory spot
	STR		R0, [R12, #n]
	SUB		R3, R3, #1
	B		fixbegin
	
fixfiller							;pads the rest of the number with zeroes, since its of a fixed length
	MOV		R1, #0x30
	CMP		R3, #0					;use the counter to see how many more zeroes need to be added
	BEQ		fixprinter
	PUSH	{R1, R3}
	SUB		R3, R3, #1
	B		fixfiller

fixprinter
	CMP		R12, SP
	BEQ		fixexit
									;the top of the stack has the most significant digit
	POP		{R0, R2}
	BL	  	ST7735_OutChar
	SUB		R2, R12, #16
	CMP		R2, SP					;if the top character was just printed, be sure to add a decimal point
	BNE		dotskip
	
	MOV		R0, #0x2E
	BL	  	ST7735_OutChar
	
dotskip
	B		fixprinter

fixexit
	POP		{R0, LR}
     BX   LR
 
     
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
