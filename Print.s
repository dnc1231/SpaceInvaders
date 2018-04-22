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
	PRESERVE8

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
COUNT EQU 0 ; BINDING
 CMP R0, #0 ; FIRST CHECK AND SEE IF NUMBER IS 0
 BNE MORE ;IF NOT CONTINUE WITH REST OF CODE
 PUSH {R4, LR} 	;PUSH REGISTERS FOR AAPCS
 ADD R0, R0, #0X30 ;IF IS ITS 0 CREATE ASCII CHARACTER
 BL ST7735_OutChar ;CALL C SUBROUTING
 POP {R4,LR} ;POP BACK VALUES
 BX LR	;RETURN FROM OUTDEC SUBROUTINE
MORE
 PUSH {R11, LR}
 SUB SP, #4	;ALLOCATING SPACE ON STACK FOR LOCAL VARIABLE COUNT
 MOV R11, SP	;CREATING FRAME POINTER
 MOV R1, #10	;MOVING 10 INTO R1
 MOV R3, #0
 STR R3, [R11, #COUNT] ;STORE COUNTER AS LOCAL VARIABLE
OutDecLoop
 CMP R0, #0	;CHECK IF QUOTIENT IS 0
 BEQ PopNow	;IF SO GO TO POP
 UDIV R2,R0,R1 ; c = a/b
 MUL R3,R2,R1 ; c x b
 SUB R3,R0,R3 ; remainder = a - (c x b)
 PUSH {R3}
 MOV R0,R2	;R2 NOW HOLDS QUOTIENT
 LDR R3, [R11, #COUNT]
 ADD R3, #1     ;INCREMENT COUNTER AND STORE BACK ONTO STACK
 STR R3, [R11, #COUNT]
 B OutDecLoop

PopNow
 LDR R3, [R11, #COUNT] ;LOAD COUNTER INTO R3
 CMP R3, #0 ;CHECK IF COUNTER IS 0
 BEQ DONE ; IF SO DONE
 POP {R0} ; POP FIRST NUMBER OFF STACK AND PUT INTO R0 - AAPCS
 ADD R0, R0, #0X30 ;ADD OFFSET TO FIRST NUMBER TO CREATE ASCII
 BL ST7735_OutChar ;CALL OUTCHAR TO OUTPUT CHARACTER
 LDR R3, [R11, #COUNT] ; RELOAD COUNTER BECAUSE R3 DESTROYED (AAPCS) 
 SUB R3, #1 	;SUBTRACT 1 FROM COUNTER 
 STR R3, [R11, #COUNT] 	;RELOAD COUNTER ONTO STACK USING FRAME POINTER
 B PopNow
DONE 
 MOV SP, R11
 ADD SP, #4 ; DEALLOCATING
 POP {R11, LR}
 


      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
max EQU 9999
star EQU 0x2A
period EQU 0x2E
turnToASCII EQU 0x30
NULL EQU 7 ;BINDING OF THE VARIABLES
THOUS EQU 4
HUND EQU 3
TEN EQU 2
PERIOD EQU 1
FIRST EQU 0
c EQU 0x63
m EQU 0x6D	
	
LCD_OutFix
	PUSH {R4,R11,LR}
	LDR R1, =max 
	CMP R0, R1 ;CHECK IF NUMBER IS IN RANGE
	BHI OverFlow_OutFix
Regular_OutFix	
	MOV R1, R0 
	SUB SP, #8 ;ALLOCTION OF STACK
	MOV R2,#0
	STRB R2,[SP,#NULL] ;Creating a null value at the end of the string
;Set-Up
	MOV R4,#10 	    ;The divisor
	LDR R2,=turnToASCII
;0.00X
	UDIV R3,R1,R4 ; c = a/b
	MUL R3,R3,R4 ; c x b
	SUB R3,R1,R3 ; remainder = a - (c x b)
	ADD R3,R3,R2 ;ADDING OFFSET TO TURN INTO CHARACTER
	STRB R3,[SP,#THOUS] 
;0.0X0	
	UDIV R1,R1,R4
	UDIV R3,R1,R4 ; c = a/b
	MUL R3,R3,R4 ; c x b
	SUB R3,R1,R3 ; remainder = a - (c x b)
	ADD R3,R3,R2
	STRB R3,[SP,#HUND]
;0.X00	
	UDIV R1,R1,R4
	UDIV R3,R1,R4 ; c = a/b
	MUL R3,R3,R4 ; c x b
	SUB R3,R1,R3 ; remainder = a - (c x b)
	ADD R3,R3,R2
	STRB R3,[SP,#TEN]
;period
	LDR R3,=period
	STRB R3,[SP,#PERIOD]
;X.000	
	UDIV R1,R1,R4
	UDIV R3,R1,R4 ; c = a/b
	MUL R3,R3,R4 ; c x b
	SUB R3,R1,R3 ; remainder = a - (c x b)
	ADD R3,R3,R2
	STRB R3,[SP,#FIRST]
;"cm"
	LDR R0, =c
	STRB R0,[SP,#5]
	LDR R0, =m
	STRB R0,[SP,#6]
;print and return
	MOV R0, SP
	BL ST7735_OutString
	ADD SP,#8
	POP {R4,R11,LR}
	BX LR
OverFlow_OutFix     	;if R0 > 9999
	SUB SP, #8
	LDR R0, =star
	LDR R1, =period
	MOV R2, #0
	STRB R0,[SP,#0]
	STRB R1,[SP,#1]
	STRB R0,[SP,#2]
	STRB R0,[SP,#3]
	STRB R0,[SP,#4]
	STRB R2,[SP,#5]	
	MOV R0, SP
	BL ST7735_OutString
	ADD SP,#8
	POP {R4,R11,LR}

     BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
