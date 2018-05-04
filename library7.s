	AREA library7, CODE, READWRITE
	EXPORT uart_init
	EXPORT pin_connect_block_setup_for_uart0
	EXPORT setup_pins
	EXPORT validate_input
	EXPORT toggle_seven_seg
	EXPORT read_character
	EXPORT output_character
	EXPORT output_string
	EXPORT new_line
	EXPORT clear_display
	EXPORT change_display

	EXPORT change_display_digit

	EXPORT from_ascii
	EXPORT number_to_memory
		
	EXPORT illuminate_red
	EXPORT illuminate_green
	EXPORT illuminate_blue
	EXPORT illuminate_yellow
	EXPORT illuminate_white
	EXPORT illuminate_purple
	EXPORT illuminate_reset
	
	EXPORT clear_input
	EXPORT get_input
		
input = "    ",0
in_count = "0",0
digits_SET   
	DCD 0x00003780  ; 0
    DCD 0x00003000  ; 1  
	DCD 0x00009580	; 2
	DCD 0x00008780	; 3
	DCD 0x0000A300	; 4
	DCD 0x0000A680 	; 5
	DCD 0x0000B680	; 6
	DCD 0x00000380	; 7
	DCD 0x0000B780	; 8
	DCD 0x0000A380 	; 9
	DCD 0x0000B380	; A
	DCD 0x0000B600	; B
	DCD 0x00003480	; C
	DCD 0x00009700	; D
	DCD 0x0000B480	; E
	DCD 0x0000B080  ; F 
      ALIGN 

uart_init
	STMFD SP!,{lr}			;push link register to stack
	LDR r0, =0xE000C00C		;loads the memory address 0xE000C00C into r0
	MOV r1, #131			;copies decimal 131 into r1
	STR r1, [r0]			;stores r1 into the memory address at r0
	LDR r0, =0xE000C000		;loads the memory address 0xE000C000 into r0
	MOV r1, #1			;copies decimal 120 into r1 / U0DLL
	STR r1, [r0]			;stores r1 into the memory address at r0
	LDR r0, =0xE000C004		;loads the memory address 0xE000C004 into r0
	MOV r1, #0			;copies decimal 0 into r1 / U0DLM
	STR r1, [r0]			;stores r1 into the memory address at r0
	LDR r0, =0xE000C00C		;loads the memory address 0xE000C00C into r0
	MOV r1, #3			;copies decimal 3 into r1
	STR r1, [r0]			;stores r1 into the memory address at r0
	;uart0 baud rate = PCLK / 16 * (16 * UART0DLM + U0DLL)
	;18.432
	LDMFD sp!, {lr}			;pop link register from stack
	BX lr				;move pc,lr


pin_connect_block_setup_for_uart0
	STMFD sp!, {r0, r1, lr}		;Push stack
	LDR r0, =0xE002C000  		; PINSEL0 load pinsel0 r0
	LDR r1, [r0]			;Load pinsel0 contents to r1
	ORR r1, r1, #5			; Or with 5 dec.
	BIC r1, r1, #0xA		; Clear against 0xA
	STR r1, [r0]			; Store results to r0 in memory
	LDMFD sp!, {r0, r1, lr}		;Pop stack
	BX lr				;Branch back

setup_pins
	STMFD SP!,{lr, r1, r2, r3}

	LDR r1, =0xE002C004			;PINSEl1 -> r1
	LDR r2, [r1]				;load contents to r2
	MOV r3, #0x0				;copy #0 to r3
	BIC r2, r2, r3				;bitclear r2 masked r3
	STR r2, [r1]				;store results in r1

	LDR r1, =0xE002C000			;PINSEL0 -> r1
	LDR r2, [r1]				;Load contents to r2
	MOV r3, #0xF00000			;copy 0xF00000 to r3
	BIC r2, r2, r3				;bitclear r2 by mask r3
	STR r2, [r1]				;store results in r1

	LDR r1, =0xE0028008			;IODIR for Seven-Seg
	;LDR r2, [r1]	
	LDR r3, =0x26B784			;Load 0x26B784 (for bit manipulation) to r3
	STR r3, [r1]				;store results to r1
	
	LDR r1, =0xE0028018			
	LDR r3, =0xF0000
	STR r3, [r1] 				;store contents 0xF0000 to memory at 0xE0028018
	
	LDMFD sp!, {lr, r1, r2, r3}
	BX lr 

validate_input					;checks that the inputted value (r0) is either hexadecimal or 'q'
	STMFD SP!, {lr, r1, r2, r3}		;returns output as boolean in (r4)

	CMP r0, #0x71				;'q'
	BEQ quit
	
	CMP r0, #0xD
	BEQ vi_valid_number

;	CMP r9, #0				;not accepting input (bar 'q') when display is off
;	BEQ vi_invalid		

	CMP r0, #0x30				;<0x30 invalid
	BLT vi_invalid

	CMP r0, #0x46				;>0x66 invalid
	BGT vi_invalid

	CMP r0, #0x39				;<=0x39 valid number
	BLE vi_valid_number
	
	CMP r0, #0x41				;>=0x41 valid uppercase letter
	BGE vi_valid_letter_upper
	
vi_valid_number

	MOV r4, #1				;return 1 for r4

	B vi_exit

vi_valid_letter_upper

	MOV r4, #1				;return 1 for r4
	
	B vi_exit

vi_invalid

	MOV r4, #0				;return 0 for r4

vi_exit

	LDMFD SP!, {lr, r1, r2, r3}
	BX lr

from_ascii					; converts (singe-digit) number at r0 from ascii number to normal number, returning at r4
	STMFD SP!, {lr, r1, r2, r3}

	CMP r0, #0x39
	BLE fa_number

	B fa_upper

fa_number

	SUB r4, r0, #0x30
	
	B fa_exit

fa_upper
	
	SUB r4, r0, #0x41
	ADD r4, r4, #10

fa_exit

	LDMFD SP!, {lr, r1, r2, r3}
	BX lr

toggle_seven_seg
	STMFD SP!, {lr, r0}

	CMP r9, #0				;check if seven seg is off
	BNE tss_off

tss_on

	MOV r9, #1				;set the flag to #1 (r9) to say seven seg is on

	MOV r0, r6

;	BL change_display			;change display


	B tss_exit 

tss_off
	
	MOV r9, #0				;set the flag to #0 (r9) to say seven seg is off
	BL clear_display			;clear (turn off) display


tss_exit

	LDMFD SP!, {lr, r0}
	BX lr

change_display				;Displays hex value passed in r0
	STMFD SP!,{lr, r1, r2, r3}
	
	MOV r9, #1
	MOV r6, r0

	LDR r1, =0xE0028000 		; Base address 
	LDR r3, =digits_SET 
	MOV r0, r0, LSL #2 		; Each stored value is 32 bits 
	LDR r2, [r3, r0]   		; Load IOSET pattern for digit in r0 
	STR r2, [r1, #4]   		; Display (0x4 = offset to IOSET) 

	LDMFD sp!, {lr, r1, r2, r3}
	BX lr

change_display_digit			;Displays hex value passed in r0 at digit r4
	STMFD SP!,{lr, r1, r2, r3, r5}
	
	MOV r5, #0xF					;mask
	MOV r5, r5, LSL #2
	
	CMP r4, #0
	BEQ mask_0
	
	CMP r4, #1
	BEQ mask_1
	
	CMP r4, #2
	BEQ mask_2
	
	CMP r4, #3
	BEQ mask_3
	
mask_0

	BIC r5, r5, #4
	LDR r1, =0xE0028008
    LDR r2, [r1]
	ORR r2, r2, r5
	STR r2, [r1]
	
	B mask_done

mask_1

	BIC r5, r5, #8
	LDR r1, =0xE0028008
    LDR r2, [r1]
	ORR r2, r2, r5
	STR r2, [r1]
	
	B mask_done

mask_2

	BIC r5, r5, #0x10
	LDR r1, =0xE0028008
    LDR r2, [r1]
	ORR r2, r2, r5
	STR r2, [r1]
	
	B mask_done

mask_3

	BIC r5, r5, #0x20
	LDR r1, =0xE0028008
    LDR r2, [r1]
	ORR r2, r2, r5	
	STR r2, [r1]
	
	B mask_done

mask_done
	
	MOV r9, #1
	LDR r1, =0xE0028004		; Base address 
	LDR r3, =digits_SET 
	MOV r0, r0, LSL #2 		; Each stored value is 32 bits 
	LDR r2, [r3, r0]   		; Load IOSET pattern for digit in r0 
	;MOV r5, r5, LSL #2
	ORR r2, r2, r5
	STR r2, [r1]   		; Display (0x4 = offset to IOSET) 

	LDMFD sp!, {lr, r1, r2, r3, r5}
	BX lr
	
clear_display
	STMFD SP!,{lr, r1, r2}
	
	LDR r1, =0xE002800C							;Load P0xCLR to r1
	LDR r2, =0xB7BC								;Load number (to r2) for bits of seven-segment display
	STR r2, [r1]								;Store number in P0xClr at r1
	
	LDMFD sp!, {lr, r1, r2}
	BX lr
	
read_character 				;Begin Receive Character block
	STMFD SP!,{lr, r3, r4, r5}
read_character_2
	LDR r3, =0xE000C014		;loads the address of uart0 into register r3 
	
	LDRB r4, [r3]			;loads the bytes at address r3 into r4 (RXFE - RDR)
	
	MOV r5, #1			;immediate value 1 is copied into r5
	AND r5, r4, r5			;logically AND r4 and r5 to compare the LSB(RDR) of r4
	
	;CMP r5, #1			;if the value of r5 is one, we are ready to receive data
	;BNE read_character_2		;else redo the process
	
	; Receiving
	
	LDR r3, =0xE000C000		;loads the address of the receive buffer register into r5
	LDR r0, [r3]			;hex value at r3 is loaded into r0
read_character_break
	LDMFD sp!, {lr, r3, r4, r5}
	BX lr

output_character 				;Begin Transmit Character block
	STMFD SP!,{lr, r3, r6, r5, r7, r8}
	MOV r7, #0					;prevent infinite loops
	LDR r8, =10000
output_character_2
	LDR r3, =0xE000C014			;loads address of uart0 into register r3
	
	
	LDRB r6, [r3]				;loads bytes at address r3 into r6 (RXFE - RDR)
	
	MOV r5, #32					;immediate value 32 (00010000) copied into r5		
	AND r5, r6, r5				;logically AND r6 and r5 to compare the 5th bit(THRE) of r6
	
	ADD r7, r7, #1				;to prevent infinite looping
	
	CMP r7, r8
	BEQ output_character_skip
	
	CMP r5, #32					;if the fifth bit is 1, then we are ready to transmit
	BNE output_character_2		;else we redo the process
	
	CMP r0, #0
	BEQ output_character_skip
	
	; Transmitting
	
	LDR r5, =0xE000C000			;loads the address of the transmit holding register (same as receive buffer)
	
	STR r0, [r5]				;stores the value of r0 into the address at r5
output_character_skip
	LDMFD sp!, {lr, r3, r6, r5, r7, r8}
	BX lr
		
output_string
	STMFD SP!,{lr, r0, r1}
	
output_string_2
	LDRB r0, [r4], #1      		;Load =prompt contents from memory (r4) to r0, one byte at a time. Then increments memory address, r4, by 1.
	BL output_character			;Branch and link to output_character
	
	CMP r0,#0					;compares r0 to null terminator
	BNE output_string_2			;if equal we continue on with program
	
	BL new_line
	
	LDMFD sp!, {lr, r0, r1}
	BX lr
	
new_line
	STMFD SP!,{lr, r0, r10}
	;MOV r10, r0					;saves contents of r0 into r10 before using it
	MOV r0, #0xA				;new line character copied into r0
	BL output_character			;branch and link to output character
	MOV r0, #0xD				;carriage return copied into r0
	BL output_character			;branch and link to output character
	;MOV r0, r10					;takes saved content from r10 and copies into r0
	LDMFD sp!, {lr, r0, r10}
	BX lr
	
clear_input
	STMFD SP!, {lr, r1-r5}
	
	LDR r0, =in_count			; Load in_count address
	MOV r1, #0x30
	BL to_mem
	
	LDMFD SP!, {lr, r1-r5}
	BX lr
	
get_input						;Return char in memory at r0 (0 for 1st; 3 for 4th)
	STMFD SP!, {lr, r1-r5}

	MOV r2, r0					;Which char -> r2

	LDR r0, =input
	ADD r0, r0, r2				;increment address by r2
	BL from_mem
	
	CMP r1, #0x30
	BLT gi_fix
	
	B gi_end
gi_fix
	MOV r1, #0x30	
gi_end
	MOV r0, r1

	LDMFD SP!, {lr, r1-r5}
	BX lr
	
to_mem								;r0 - memory address, r1 - contents
	STMFD SP!, {lr, r2-r5}
	
	STRB r1, [r0]
	
	LDMFD SP!, {lr, r2-r5}
	BX lr
	
from_mem								;r0 - memory address, return contents - r1
	STMFD SP!, {lr, r2-r5}
	
	LDRB r1, [r0]
	
	LDMFD SP!, {lr, r2-r5}
	BX lr
	
store_input
	STMFD SP!, {lr, r1-r5}

	; IN r0 - contents of key pressed

	MOV r3, r0

	LDR r0, =in_count			; Load in_count address
	BL from_mem					; Get value of in_count
	
	MOV r0, r1
	BL from_ascii
	
	MOV r1, r4

	LDR r0, =input				; Load input address
	ADD r0, r0, r1				; Pre-increment r0 (address) by r1 (# of elements). 
	
	MOV r4, r1
	
	MOV r1, r3
	
	BL to_mem
	
	LDR r0, =in_count
	MOV r1, r4
	ADD r1, r1, #0x30
	ADD r1, r1, #1
	
	BL to_mem

	LDMFD SP!, {lr, r1-r5}
	BX lr
	
	
illuminate_red
	STMFD SP!, {lr, r0, r1, r2}

	LDR r0, =0xE002800C							;Load P0xCLR to r0
	LDR r1, [r0]								;Load contents to r1
	MOV r2, #0x2								;Copy 0x3 to r2
	MOV r2, r2, LSL #16 						;Logical shift left r2 by 16
	ORR r1, r1, r2								;Or r1 with r2
	STR r1, [r0]								;Store result in r0

	LDMFD SP!, {lr, r0, r1, r2}
	BX lr


illuminate_blue
	STMFD SP!, {lr, r0, r1, r2}

	LDR r0, =0xE002800C							;Load P0xCLR to r0
	LDR r1, [r0]								;Load contents to r1
	MOV r2, #0x4								;Mov 0x4 to r2
	MOV r2, r2, LSL #16							;logical shift left r2 by 16
	ORR r1, r1, r2								; or r1 with r2
	STR r1, [r0]								; store result to r0

	LDMFD SP!, {lr, r0, r1, r2}
	BX lr


illuminate_green
	STMFD SP!, {lr, r0, r1, r2}

	LDR r0, =0xE002800C							;Load P0xCLR to r0
	LDR r1, [r0]								;Load contents to r1
	MOV r2, #0x20								;Move 0x20 to r2 (to manipulate respective bits)
	MOV r2, r2, LSL #16							;logical shift left r2 by 16
	ORR r1, r1, r2								; or r1 with r2
	STR r1, [r0]								; store its contents to r0

	LDMFD SP!, {lr, r0, r1, r2}
	BX lr

illuminate_white
	STMFD SP!, {lr}

	LDR r0, =0xE002800C							;Load P0xCLR to r0
	LDR r1, [r0]								; load its contents to r1
	MOV r2, #0x26								;Move 0x26 to r2 (to manipulate bits in P0xCLR)
	MOV r2, r2, LSL #16							;logical shift left by 16 on r2
	ORR r1, r1, r2								; or r1 with r2
	STR r1, [r0]								; store contents r0

	LDMFD SP!, {lr}
	BX lr

illuminate_purple
	STMFD SP!, {lr}
	
	BL illuminate_red
	BL illuminate_blue

	LDMFD SP!, {lr}
	BX lr

illuminate_yellow
	STMFD SP!, {lr}
	
	BL illuminate_blue
	BL illuminate_green

	LDMFD SP!, {lr}
	BX lr

illuminate_reset
	STMFD SP!, {lr, r0, r1, r2}

	LDR r0, =0xE0028004							; load P0xSET -> r0
	LDR r1, [r0]								; load its contents
	MOV r2, #0x26								; 0x26 (respective bits to maniupulate in the P0xSET) -> r2
	MOV r2, r2, LSL #16							; shift left 16 places
	ORR r1, r1, r2								; or r1 with r2
	STR r1, [r0]								; store results to r0

	LDMFD SP!, {lr, r0, r1, r2}
	BX lr

number_to_memory
	STMFD SP!, {lr}
	;CMP r9, #0			;Compare r9 to 0
	;BGT neg_skip			;Branch greater than neg_skip
	;BL insert_neg			;Branch/link insert_neg

	;MVN r9, r9			;
	;ADD r9, r9, #1			;Two's compliment r9

;neg_skip
		
	;MOV r0, r9	
	MOV r9, r0
	MOV r12, #1000			;r12 = 1000
	MOV r7, #0			;r7 = 0
	BL aaa				;Branch/link aaa
	MOV r0, #0			;r0 = 0
	ADD r0, r6, #0x30		;r0 = r6 + 0x30
	BL store_input		;Branch/link output_character
	
	MOV r12, #100	 		;r12 = 100
	MOV r7, #1000			;r7 = 1000
	BL aaa			 	;Branch/link aaa
	MOV r0, #0		;r0 = 0
	ADD r0, r6, #0x30		;r0 = r6 + 0x30
	BL store_input		;Branch/link output_character
	
	MOV r12, #10			;r12 = 10	
	MOV r7, #100			;r7 = 100
	BL aaa			  	;Branch/link aaa
	MOV r0, #0		;r0 = 0
	ADD r0, r6, #0x30		;r0 = r6 + 0x30
	BL store_input		;Branch/link output_character

	MOV r12, #1			;r12 = 1		 
	MOV r7, #10			;r7 = 10
	BL aaa				;Branch/link aaa
	MOV r0, #0			;r0 = 0
	ADD r0, r6, #0x30		;r0 = r6 + 0x30
	BL store_input		;Branch/link output_character
	
	LDMFD SP!, {lr}
	BX lr	
	
	
aaa					;Subroutine to get single digit of a number
    STMFD SP!,{lr}			;Preserve stack
    MOV r6, #0	   		;r6 = 0
    MOV r5, r9			;r5 = r9
aab					;Begin aab subroutine
		
	CMP r7, #0			;Compare r7 to 0
	BEQ aab_skip			;branch if equal -> aab_skip

aab_retry				;Check if r5 is too large compared to r7
	CMP r5, r7			;Compare r5 t0 r7
	BLE aab_skip			;branch if less than or equal -> aab_skip
	SUB r5, r5, r7			;r5 = r5 - r7
	B aab_retry			;Branch to aab_retry

aab_skip				;r5 is within bounds, now loop subtracting r12 from it

    CMP r5, r12				;Compare r5 to r12
    BLT aad				;Branch less than aad

    SUB r5, r5, r12			;r5 = r5 - r12
    ADD r6, r6, #1			;decrement r6

    CMP r5, #0				;Compare r5 to 0
    BGT aab				;Branch if greater than -> aab

aac					;Reset r6 if needed

	CMP r6, #10			;Compare r6 to 10
	BLT aad				;Branch if less than -> aad

	MOV r6, #0			;r6 = 0

aad

    LDMFD sp!, {lr}			;Preserve stack
    BX lr				;Move pc,lr
	
last					;Program is done


quit
	MOV r7, #5
	END
