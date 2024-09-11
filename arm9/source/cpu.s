#ifdef __arm__

#include "Shared/nds_asm.h"
#include "ARMZ80/ARMZ80.i"
#include "BlackTigerVideo/BlackTigerVideo.i"

#define CYCLE_PSL (254)
#define CYCLE_PSL1 (226)

	.global run
	.global stepFrame
	.global cpuReset
	.global frameTotal
	.global waitMaskIn
	.global waitMaskOut
	.global cpu1SetIRQ
	.global z80CPU1


	.syntax unified
	.arm

#if GBA
	.section .ewram, "ax", %progbits	;@ For the GBA
#else
	.section .text						;@ For anything else
#endif
	.align 2
;@----------------------------------------------------------------------------
run:						;@ Return after X frame(s)
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
	ldr z80ptr,=z80CPU1
	mov r0,#CYCLE_PSL1
//	bl Z80RestoreAndRunXCycles
	add r0,z80ptr,#z80Regs
//	stmia r0,{z80f-z80pc,z80sp}	;@ Save Z80 state
;@--------------------------------------
	ldr z80ptr,=Z80OpTable
	mov r0,#CYCLE_PSL
	bl Z80RestoreAndRunXCycles
	add r0,z80ptr,#z80Regs
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
frameTotal:			.long 0		;@ Let Gui.c see frame count for savestates
waitCountIn:		.byte 0
waitMaskIn:			.byte 0
waitCountOut:		.byte 0
waitMaskOut:		.byte 0

;@----------------------------------------------------------------------------
stepFrame:					;@ Return after 1 frame
	.type stepFrame STT_FUNC
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r11,lr}
;@----------------------------------------------------------------------------
ccStepLoop:
;@----------------------------------------------------------------------------
	ldr z80ptr,=z80CPU1
	mov r0,#CYCLE_PSL1
//	bl Z80RestoreAndRunXCycles
	add r0,z80ptr,#z80Regs
//	stmia r0,{z80f-z80pc,z80sp}	;@ Save Z80 state
;@--------------------------------------
	ldr z80ptr,=Z80OpTable
	mov r0,#CYCLE_PSL
	bl Z80RestoreAndRunXCycles
	add r0,z80ptr,#z80Regs
	stmia r0,{z80f-z80pc,z80sp}	;@ Save Z80 state
;@--------------------------------------
	ldr btptr,=blkTgrVideo_0
	bl doScanline
	cmp r0,#0
	bne ccStepLoop
;@----------------------------------------------------------------------------

	ldr r1,frameTotal
	add r1,r1,#1
	str r1,frameTotal

	ldmfd sp!,{r4-r11,lr}
	bx lr
;@----------------------------------------------------------------------------
cpu1SetIRQ:
;@----------------------------------------------------------------------------
	ldr r1,=z80CPU1
	b Z80SetIRQPin
;@----------------------------------------------------------------------------
cpuReset:		;@ Called by loadCart/resetGame
;@----------------------------------------------------------------------------
	stmfd sp!,{lr}

;@---Speed - 4.0MHz / 60Hz		;Black Tiger / Black Dragon.
	ldr r0,=CYCLE_PSL			;@ 254
	str r0,cyclesPerScanline

;@--------------------------------------
	ldr z80ptr,=Z80OpTable

	ldr r0,=z80CPU1
	mov r1,z80ptr
	ldr r2,=z80Size
	bl memcpy


	adr r4,cpuMapData
	bl mapZ80Memory

	mov r0,z80ptr
	mov r1,#0
	bl Z80Reset

;@---Speed - 3.579MHz / 60Hz	;Black Tiger / Black Dragon, audio
//	ldr r0,=CYCLE_PSL1			;@ 226
//	str r0,cyclesPerScanline
;@--------------------------------------
	ldr z80ptr,=z80CPU1

	adr r4,cpuMapData+8
	bl mapZ80Memory

	mov r0,z80ptr
	mov r1,#0
	bl Z80Reset

	ldmfd sp!,{lr}
	bx lr
;@----------------------------------------------------------------------------
cpuMapData:
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
	.space 102*4
z80CPU1:
	.space z80Size
;@----------------------------------------------------------------------------
	.end
#endif // #ifdef __arm__
