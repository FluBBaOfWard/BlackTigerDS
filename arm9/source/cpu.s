#ifdef __arm__

#include "Shared/nds_asm.h"
#include "ARMZ80/ARMZ80.i"
#include "BlackTigerVideo/BlackTigerVideo.i"

#define CYCLE_PSL (254)
#define CYCLE_PSL1 (226)

	.global cpuReset
	.global run
	.global frameTotal
	.global waitMaskIn
	.global waitMaskOut
	.global cpu1SetIRQ
	.global z80CPU1



	.syntax unified
	.arm

	.section .text
	.align 2
;@----------------------------------------------------------------------------
run:		;@ Return after 1 frame
	.type   run STT_FUNC
;@----------------------------------------------------------------------------
	ldrh r0,waitCountIn
	add r0,r0,#1
	ands r0,r0,r0,lsr#8
	strb r0,waitCountIn
	bxne lr
	stmfd sp!,{r4-r11,lr}

;@----------------------------------------------------------------------------
runStart:
;@----------------------------------------------------------------------------
	ldr r0,=EMUinput
	ldr r0,[r0]

	ldr r2,=yStart
	ldrb r1,[r2]
	tst r0,#0x200				;@ L?
	subsne r1,#1
	movmi r1,#0
	tst r0,#0x100				;@ R?
	addne r1,#1
	cmp r1,#GAME_HEIGHT-SCREEN_HEIGHT
	movpl r1,#GAME_HEIGHT-SCREEN_HEIGHT
	strb r1,[r2]

	bl refreshEMUjoypads		;@ Z=1 if communication ok

;@----------------------------------------------------------------------------
capcomFrameLoop:
;@----------------------------------------------------------------------------
	ldr z80optbl,=z80CPU1
	mov r0,#CYCLE_PSL1
//	b Z80RestoreAndRunXCycles
BTCPU1End:
	add r0,z80optbl,#z80Regs
//	stmia r0,{z80f-z80pc,z80sp}	;@ Save Z80 state
;@--------------------------------------
	ldr z80optbl,=Z80OpTable
	mov r0,#CYCLE_PSL
	b Z80RestoreAndRunXCycles
BTCPU0End:
	add r0,z80optbl,#z80Regs
	stmia r0,{z80f-z80pc,z80sp}	;@ Save Z80 state
;@--------------------------------------
	ldr btptr,=blkTgrVideo_0
	bl doScanline
	cmp r0,#0
	bne capcomFrameLoop
;@----------------------------------------------------------------------------

	ldr r1,=fpsValue
	ldr r0,[r1]
	add r0,r0,#1
	str r0,[r1]

	ldr r1,frameTotal
	add r1,r1,#1
	str r1,frameTotal

	ldrh r0,waitCountOut
	add r0,r0,#1
	ands r0,r0,r0,lsr#8
	strb r0,waitCountOut
	ldmfdeq sp!,{r4-r11,lr}		;@ Exit here if doing single frame:
	bxeq lr						;@ Return to rommenu()
	b runStart

;@----------------------------------------------------------------------------
cyclesPerScanline:	.long 0
frameTotal:			.long 0		;@ Let ui.c see frame count for savestates
waitCountIn:		.byte 0
waitMaskIn:			.byte 0
waitCountOut:		.byte 0
waitMaskOut:		.byte 0

;@----------------------------------------------------------------------------
cpu1SetIRQ:
;@----------------------------------------------------------------------------
	stmfd sp!,{z80optbl,lr}
	ldr z80optbl,=z80CPU1
	bl Z80SetIRQPin
	ldmfd sp!,{z80optbl,pc}
;@----------------------------------------------------------------------------
cpuReset:		;@ Called by loadCart/resetGame
;@----------------------------------------------------------------------------
	stmfd sp!,{lr}

;@---Speed - 4.0MHz / 60Hz		;Black Tiger / Black Dragon.
	ldr r0,=CYCLE_PSL			;@ 254
	str r0,cyclesPerScanline

;@--------------------------------------
	ldr z80optbl,=Z80OpTable

	ldr r0,=z80CPU1
	mov r1,z80optbl
	ldr r2,=z80Size
	bl memcpy


	adr r4,cpuMapData
	bl mapZ80Memory

	adr r0,BTCPU0End
	str r0,[z80optbl,#z80NextTimeout]
	str r0,[z80optbl,#z80NextTimeout_]

	mov r0,#0
	bl Z80Reset

;@---Speed - 3.579MHz / 60Hz	;Black Tiger / Black Dragon, audio
//	ldr r0,=CYCLE_PSL			;@ 226
//	str r0,cyclesPerScanline
;@--------------------------------------
	ldr z80optbl,=z80CPU1

	adr r4,cpuMapData+8
	bl mapZ80Memory

	adr r0,BTCPU1End
	str r0,[z80optbl,#z80NextTimeout]
	str r0,[z80optbl,#z80NextTimeout_]

	mov r0,#0
	bl Z80Reset

	ldmfd sp!,{lr}
	bx lr
;@----------------------------------------------------------------------------
cpuMapData:
;@	.byte 0x07,0x06,0x05,0x04,0xFD,0xF8,0xFE,0xFF			;@ Double Dribble CPU0
;@	.byte 0x0B,0x0A,0x09,0x08,0xFB,0xFB,0xF9,0xF8			;@ Double Dribble CPU1
;@	.byte 0x0F,0x0E,0x0D,0x0C,0xFB,0xFB,0xFB,0xFA			;@ Double Dribble CPU2
;@	.byte 0x09,0x08,0x03,0x02,0x01,0x00,0xFE,0xFF			;@ Jackal CPU0
;@	.byte 0x0D,0x0C,0x0B,0x0A,0xF8,0xFD,0xFA,0xFB			;@ Jackal CPU1
;@	.byte 0x05,0x04,0x03,0x02,0x01,0x00,0xFE,0xFF			;@ Iron Horse
;@	.byte 0x05,0x04,0x03,0x02,0x01,0x00,0xFE,0xFF			;@ Finalizer
;@	.byte 0x03,0x02,0x01,0x00,0xF9,0xF9,0xFF,0xFE			;@ Jail Break
;@	.byte 0xFF,0xFE,0x05,0x04,0x03,0x02,0x01,0x00			;@ Green Beret
	.byte 0xF9,0xF8,0x05,0x04,0x03,0x02,0x01,0x00			;@ Black Tiger
	.byte 0xFB,0xFA,0x88,0x88,0x17,0x16,0x15,0x14			;@ Black Tiger audio
;@----------------------------------------------------------------------------
mapZ80Memory:
	stmfd sp!,{lr}
	mov r5,#0x80
z80DataLoop:
	mov r0,r5
	ldrb r1,[r4],#1
	bl z80Mapper
	movs r5,r5,lsr#1
	bne z80DataLoop
	ldmfd sp!,{pc}
;@----------------------------------------------------------------------------
#ifdef NDS
	.section .dtcm, "ax", %progbits				;@ For the NDS
#endif
;@----------------------------------------------------------------------------
	.space 105*4
z80CPU1:
	.space z80Size
;@----------------------------------------------------------------------------
	.end
#endif // #ifdef __arm__
