#ifdef __arm__

#include "ARMZ80/ARMZ80.i"
#include "Shared/EmuMenu.i"

	.global ioReset
	.global convertInput
	.global refreshEMUjoypads
	.global Z80In

	.global joyCfg
	.global EMUinput
	.global gDipSwitch0
	.global gDipSwitch1
	.global gDipSwitch2
	.global gDipSwitch3
	.global coinCounter0
	.global coinCounter1

	.syntax unified
	.arm

	.section .text
	.align 2
;@----------------------------------------------------------------------------
ioReset:
;@----------------------------------------------------------------------------
	bx lr
;@----------------------------------------------------------------------------
convertInput:			;@ Convert from device keys to target r0=input/output
	.type convertInput STT_FUNC
;@----------------------------------------------------------------------------
	mvn r1,r0
	tst r1,#KEY_L|KEY_R				;@ Keys to open menu
	orreq r0,r0,#KEY_OPEN_MENU
	bx lr
;@----------------------------------------------------------------------------
refreshEMUjoypads:			;@ Call every frame
;@----------------------------------------------------------------------------
		ldr r4,=frameTotal
		ldr r4,[r4]
		movs r0,r4,lsr#2		;@ C=frame&2 (autofire alternates every other frame)
	ldr r4,EMUinput
	and r0,r4,#0xf0
		ldr r2,joyCfg
		andcs r4,r4,r2
		tstcs r4,r4,lsr#10		;@ L?
		andcs r4,r4,r2,lsr#16
	adr r1,rlud2rldu
	ldrb r0,[r1,r0,lsr#4]

	ands r1,r4,#3				;@ A/B buttons to Knife/Special
	cmpne r1,#3
	eorne r1,r1,#3
	tst r2,#0x400				;@ Swap A/B?
	andne r1,r4,#3
	orr r0,r0,r1,lsl#4

	mov r1,#0
	mov r3,#0
	tst r4,#0x4					;@ Select
	orrne r3,r3,#0x40			;@ Coin
	tst r4,#0x8					;@ Start
	orrne r3,r3,#0x01			;@ Start
	tst r2,#0x20000000			;@ Player2?
	movne r1,r0
	movne r0,#0
	movne r3,r3,lsl#1

	strb r0,joy0state
	strb r1,joy1state
	strb r3,joy2state
	bx lr

joyCfg: .long 0x00ff01ff	;@ byte0=auto mask, byte1=(saves R), byte2=R auto mask
							;@ bit 31=single/multi, 30,29=1P/2P, 27=(multi) link active, 24=reset signal received
nrplayers:	.long 0			;@ Number of players in multilink.
joySerial:	.byte 0
joy0state:	.byte 0
joy1state:	.byte 0
joy2state:	.byte 0
rlud2rldu:		.byte 0x00,0x01,0x02,0x03, 0x08,0x09,0x0a,0x0b, 0x04,0x05,0x06,0x07, 0x0c,0x0d,0x0e,0x0f
rlud2rldu180:	.byte 0x00,0x01,0x02,0x03, 0x08,0x09,0x0a,0x0b, 0x04,0x05,0x06,0x07, 0x0c,0x0d,0x0e,0x0f
rlud2rldu90:	.byte 0x00,0x08,0x04,0x0c, 0x02,0x0a,0x06,0x0e, 0x01,0x09,0x05,0x0d, 0x03,0x0b,0x07,0x0f
rlud2rldu270:	.byte 0x00,0x04,0x08,0x0c, 0x01,0x05,0x09,0x0d, 0x02,0x06,0x0a,0x0e, 0x03,0x07,0x0b,0x0f
gDipSwitch0:	.byte 0
gDipSwitch1:	.byte 0x80		;@ Lives, cabinet & demo sound.
gDipSwitch2:	.byte 0
gDipSwitch3:	.byte 0
coinCounter0:	.long 0
coinCounter1:	.long 0

EMUinput:			;@ This label here for main.c to use
	.long 0			;@ EMUjoypad (this is what Emu sees)

;@----------------------------------------------------------------------------
input0R:		;@ Player 1
;@----------------------------------------------------------------------------
;@	mov r11,r11					;@ No$GBA breakpoint
	ldrb r0,joy0state
	eor r0,r0,#0xFF
	bx lr
;@----------------------------------------------------------------------------
input1R:		;@ Player 2
;@----------------------------------------------------------------------------
;@	mov r11,r11					;@ No$GBA breakpoint
	ldrb r0,joy1state
	eor r0,r0,#0xFF
	bx lr
;@----------------------------------------------------------------------------
input2R:		;@ Coins, Start & Service
;@----------------------------------------------------------------------------
;@	mov r11,r11					;@ No$GBA breakpoint
	ldrb r0,joy2state
	eor r0,r0,#0xFF
	bx lr
;@----------------------------------------------------------------------------
input3R:
;@----------------------------------------------------------------------------
	ldrb r0,gDipSwitch0
	eor r0,r0,#0xFF
	bx lr
;@----------------------------------------------------------------------------
input4R:
;@----------------------------------------------------------------------------
	ldrb r0,gDipSwitch1
	eor r0,r0,#0xFF
	bx lr
;@----------------------------------------------------------------------------
input5R:
;@----------------------------------------------------------------------------
	ldrb r0,gDipSwitch2
	eor r0,r0,#0xFF
	bx lr

;@----------------------------------------------------------------------------
Z80In:
;@----------------------------------------------------------------------------
	and r1,addy,#0xFF
	cmp r1,#8
	ldrmi pc,[pc,r1,lsl#2]
;@---------------------------
	b empty_IO_R
;@io_read_tbl
	.long input2R		;@ 0
	.long input0R		;@ 1
	.long input1R		;@ 2
	.long input3R		;@ 3
	.long input4R		;@ 4
	.long input5R		;@ 5
	.long empty_IO_R	;@ 6
	.long protectionR	;@ 7
;@----------------------------------------------------------------------------
protectionR:
;@----------------------------------------------------------------------------
	mov r11,r11					;@ No$GBA breakpoint
	mov r0,z80de,lsr#24
	bx lr

;@----------------------------------------------------------------------------
	.end
#endif // #ifdef __arm__
