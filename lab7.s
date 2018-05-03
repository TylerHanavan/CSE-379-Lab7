	PRESERVE8
	AREA interrupts, CODE, READWRITE

	EXPORT lab7

	EXPORT FIQ_Handler

	EXTERN uart_init
	EXTERN pin_connect_block_setup_for_uart0
	EXTERN setup_pins
	EXTERN validate_input
	EXTERN change_display
	EXTERN clear_display
	EXTERN toggle_seven_seg
	EXTERN read_character
	EXTERN output_character
	EXTERN output_string
	EXTERN new_line

	EXTERN change_display_digit
	EXTERN get_digit

	EXTERN from_ascii
		
	EXTERN store_input
	EXTERN get_input
	EXTERN clear_input
		
	EXTERN is_enemy_alive
	EXTERN do_tick
	EXTERN handle_input
		
	EXTERN get_score_level
	EXTERN score_level_

	EXTERN number_to_memory
	
	EXTERN clear_input
	EXTERN get_input
		
enemies = "////",0
shields_alive = "////",0    
shields_type = "////",0   
instructions = "Welcome to Space Invaders! The purpose of the game is to rack up as many points as possble within 2 minutes. Each level will contain 35 enemies and 3 random motherships. The first two rows of enemies ('W') will count as 10 points. The next two rows of enemies ('M') will count as 20 points. The following row ('O') will count as 40 points. The last row will contain a mothership moving at a different speed in random directions unlike the space invaders. Each mothership will vary in points within the range 100 to 300. It only takes one shot to kill an enemy, but they can shoot back and kill you in one shot. Use your shields to protect you. Press 'a' to move your spaceship left and 'd' to move your spaceship right. Press 'w' or the space button to fire at your enemies. Be careful not to shoot at your shields or else they will deplete. Once all 35 space invaders are destroyed, the level progresses as well as the speed of the space invaders. You have 4 lives, every life you lose you will also lose 100 points. Once all 4 lives are used up, your total score will be shown. When you are ready to play hit the letter 'p'. Press 'm' to quit the game.",0
	ALIGN

lab7 	

	STMFD sp!, {lr}

	BL uart_init					;setup the uart with its init subroutine
	BL pin_connect_block_setup_for_uart0		;setup the pin connect block
	BL setup_pins					;setup pins required for momentary push button and seven segment display	

	LDR r4, =instructions
	BL output_string
	
check_game_start
	BL read_character
	
	CMP r0, #0x70     		;character 'p'
	BNE check_game_start
	
	BL interrupt_init
	BL clear_display
	
	MOV r9, #0
	
	MOV r8, #2

lab7_loop

	B lab7_loop

lab7_end

	BL clear_display
	
	LDMFD sp!,{lr}

	BX lr



interrupt_init       

		STMFD SP!, {r0-r2, lr}   ; Save registers 

		; Push button setup		 

		LDR r0, =0xE002C000

		LDR r1, [r0]

		ORR r1, r1, #0x20000000

		BIC r1, r1, #0x10000000

		STR r1, [r0]  ; PINSEL0 bits 29:28 = 10


		;key board setup
		LDR r0, =0xE000C004
		
		LDR r1, [r0]
		
		ORR r1, r1, #0x1
		
		STR r1, [r0]
		

		; Classify sources as IRQ or FIQ

		LDR r0, =0xFFFFF000

		LDR r1, [r0, #0xC]
		
		LDR r2, =0x8050

		ORR r1, r1, r2 ; External Interrupt 1

		STR r1, [r0, #0xC]



		; Enable Interrupts

		LDR r0, =0xFFFFF000

		LDR r1, [r0, #0x10]

		LDR r2, =0x8050

		ORR r1, r1, r2 ; External Interrupt 1

		STR r1, [r0, #0x10]



		; External Interrupt 1 setup for edge sensitive

		LDR r0, =0xE01FC148

		LDR r1, [r0]

		ORR r1, r1, #2  ; EINT1 = Edge Sensitive

		STR r1, [r0]		

		; External Timer 0 modify MR0

		LDR r0, =0xE0004018

		LDR r1, =110000

		STR r1, [r0]

		
		; External Timer 0 MCR

		LDR r0, =0xE0004014

		LDR r1, [r0]
		
		;ORR r1, r1, #3
		;ORR r1, r1, #8
		ORR r1, r1, #0x3
		BIC r1, r1, #0x20

		STR r1, [r0]
		
		; External Timer 0 timer control register

		LDR r0, =0xE0004004

		LDR r1, [r0]

		ORR r1, r1, #1  

		STR r1, [r0]

		; Enable FIQ's, Disable IRQ's

		MRS r0, CPSR

		BIC r0, r0, #0x40

		ORR r0, r0, #0x80

		MSR CPSR_c, r0



		LDMFD SP!, {r0-r2, lr} ; Restore registers

		BX lr             	   ; Return

FIQ_Handler

	STMFD SP!, {r0, r1, r2, r3, r4, lr}   ; Save registers
		
	; Disable Interrupts

	LDR r0, =0xFFFFF000

	LDR r1, [r0, #0x10]
	
	MOV r2, #0x10

	BIC r1, r1, r2 ; External Interrupt 1

	STR r1, [r0, #0x10]
		
	LDR r0, =0xE0004000
	LDR r1, [r0]
	CMP r1, #0
	BGT TIMER0
	B EINT1
		
TIMER0

	ORR r1, r1, #1
	STR r1, [r0]
	
TIMER0LOOP
	
	LDR r2, =do_tick
	MOV lr, pc
	BX r2
	
	LDR r6, =get_score_level
	MOV lr, pc
	BX r6
	
	BL number_to_memory
	
	CMP r7, #0
	BEQ cycle_1

	CMP r7, #1
	BEQ cycle_2

	CMP r7, #2
	BEQ cycle_3

	CMP r7, #3
	BEQ cycle_4
		
	B FIQ_Exit
	
cycle_1
	MOV r0, #0
	BL get_input
	
	BL from_ascii
	
	MOV r0, r4
	
	MOV r4, #0
	
	BL clear_display
	BL change_display_digit

	ADD r7, r7, #1

	B FIQ_Exit

cycle_2
	MOV r0, #1
	BL get_input
	
	BL from_ascii

	MOV r0, r4
	
	MOV r4, #1
	
	BL clear_display
	BL change_display_digit

	ADD r7, r7, #1

	B FIQ_Exit

cycle_3
	MOV r0, #2
	BL get_input
	
	BL from_ascii
	
	MOV r0, r4
	
	MOV r4, #2

	BL clear_display
	BL change_display_digit

	ADD r7, r7, #1

	B FIQ_Exit

cycle_4
	MOV r0, #3
	BL get_input
	
	BL from_ascii
	

	MOV r0, r4
	
	MOV r4, #3

	BL clear_display
	BL change_display_digit

	MOV r7, #0
	BL clear_input
	B FIQ_Exit
	
R0_RESET
	STMFD SP!, {lr}
	MOV r0, #0x0
	ADD r1, #0x1
	BL new_line
	LDMFD SP!, {lr}
	BX lr

	B FIQ_Exit

EINT1			; Check for EINT1 interrupt
	

	LDR r0, =0xE01FC140

	LDR r1, [r0]

	TST r1, #2

	BEQ FIQ_Keys

	BL toggle_seven_seg

	ORR r1, r1, #2		; Clear Interrupt

	STR r1, [r0]
	
	B FIQ_Exit

FIQ_Keys

	LDR r0, =0xE000C008

	LDR r1, [r0]
	
	AND r2, r1, #1

	CMP r2, #0

	BNE FIQ_Exit

	BL read_character
	
	CMP r0, #0x6D
	BEQ lab7_end
	
	LDR r2, =handle_input
	MOV lr, pc
	BX r2



FIQ_Exit
		
		; Enable Interrupts

		LDR r0, =0xFFFFF000

		LDR r1, [r0, #0x10]

		LDR r2, =0x8050

		ORR r1, r1, r2 ; External Interrupt 1

		STR r1, [r0, #0x10]
		
		LDR r0, =0xE0004000
		LDR r1, [r0]

		BIC r1, r1, #1
		STR r1, [r0]

		LDMFD SP!, {r0, r1, r2, r3, r4, lr}

		SUBS pc, lr, #4



	END
