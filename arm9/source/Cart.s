#ifdef __arm__

#include "Shared/EmuSettings.h"
#include "ARMZ80/ARMZ80mac.h"
#include "BlackTigerVideo/BlackTigerVideo.i"

	.global machineInit
	.global loadCart
	.global z80Mapper
	.global blkTgrMapper
	.global emuFlags
	.global romNum
	.global cartFlags
	.global romStart
	.global vromBase0
	.global vromBase1
	.global SOUND_RAM

	.global ROM_Space


	.syntax unified
	.arm

	.section .rodata
	.align 2

rawRom:
/*
// Code
	.incbin "blktiger/bdu-01a.5e"
	.incbin "blktiger/bdu-02a.6e"
	.incbin "blktiger/bdu-03a.8e"
	.incbin "blktiger/bd-04.9e"
	.incbin "blktiger/bd-05.10e"
// Audio Code
	.incbin "blktiger/bd-06.1l"
// MCU
//	.incbin "blktiger/bd.6k"
// Characters tiles
	.incbin "blktiger/bd-15.2n"
// Background tiles
	.incbin "blktiger/bd-12.5b"
	.incbin "blktiger/bd-11.4b"
	.incbin "blktiger/bd-14.9b"
	.incbin "blktiger/bd-13.8b"
// Sprite tiles
	.incbin "blktiger/bd-08.5a"
	.incbin "blktiger/bd-07.4a"
	.incbin "blktiger/bd-10.9a"
	.incbin "blktiger/bd-09.8a"
*/
/*
// Code
	.incbin "blktiger/blkdrgon.5e"
	.incbin "blktiger/blkdrgon.6e"
	.incbin "blktiger/blkdrgon.8e"
	.incbin "blktiger/blkdrgon.9e"
	.incbin "blktiger/blkdrgon.10e"
// Audio Code
	.incbin "blktiger/bd-06.1l"
// Characters tiles
	.incbin "blktiger/blkdrgon.2n"
// Background tiles
	.incbin "blktiger/blkdrgon.5b"
	.incbin "blktiger/blkdrgon.4b"
	.incbin "blktiger/blkdrgon.9b"
	.incbin "blktiger/blkdrgon.8b"
// Sprite tiles
	.incbin "blktiger/bd-08.5a"
	.incbin "blktiger/bd-07.4a"
	.incbin "blktiger/bd-10.9a"
	.incbin "blktiger/bd-09.8a"
*/
	.align 2
;@----------------------------------------------------------------------------
machineInit: 	;@ Called from C
	.type   machineInit STT_FUNC
;@----------------------------------------------------------------------------
	stmfd sp!,{lr}
	bl gfxInit
//	bl ioInit
	bl soundInit
//	bl cpuInit

	ldmfd sp!,{lr}
	bx lr

	.section .ewram,"ax"
	.align 2
;@----------------------------------------------------------------------------
loadCart: 		;@ Called from C:  r0=rom number, r1=emuflags
	.type   loadCart STT_FUNC
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r11,lr}
	str r0,romNum
	str r1,emuFlags

//	ldr r7,=rawRom
	ldr r7,=ROM_Space
								;@ r7=rombase til end of loadcart so DON'T FUCK IT UP
	str r7,romStart				;@ Set rom base
	add r0,r7,#0x50000			;@ 0x48000+0x8000
	str r0,vromBase0			;@ Chr
	add r0,r0,#0x08000
	str r0,vromBase1			;@ Tiles
	add r0,r0,#0x40000
	str r0,vromBase2			;@ Sprites

	ldr r4,=MEMMAPTBL_
	ldr r5,=RDMEMTBL_
	ldr r6,=WRMEMTBL_

	mov r0,#0
	ldr r2,=memZ80R0
	ldr r3,=rom_W
tbLoop1:
	add r1,r7,r0,lsl#13
	bl initMappingPage
	add r0,r0,#1
	cmp r0,#0x88
	bne tbLoop1

	ldr r2,=empty_R
	ldr r3,=empty_W
tbLoop2:
	bl initMappingPage
	add r0,r0,#1
	cmp r0,#0x100
	bne tbLoop2

	mov r0,#0xF8				;@ RAM6
	ldr r1,=blkTgrRAM_0+0x3000
	ldr r2,=blkTgrVideoCD_0R
	ldr r3,=blkTgrVideoCD_0W
	bl initMappingPage
	add r1,r1,#0x2000

	mov r0,#0xF9				;@ RAM7
	ldr r2,=memZ80R7
	ldr r3,=ramZ80W7
	bl initMappingPage

	mov r0,#0xFA				;@ RAM6
	ldr r1,=SOUND_RAM
	ldr r2,=soundRamR
	ldr r3,=soundRamW
	bl initMappingPage

	mov r0,#0xFB				;@ RAM7
	ldr r2,=ymRead
	ldr r3,=ymWrite
	bl initMappingPage


	bl gfxReset
	bl ioReset
	bl soundReset
	bl cpuReset

	ldmfd sp!,{r4-r11,lr}
	bx lr

;@----------------------------------------------------------------------------
initMappingPage:	;@ r0=page, r1=mem, r2=rdMem, r3=wrMem
;@----------------------------------------------------------------------------
	str r1,[r4,r0,lsl#2]
	str r2,[r5,r0,lsl#2]
	str r3,[r6,r0,lsl#2]
	bx lr

;@----------------------------------------------------------------------------
//	.section itcm
;@----------------------------------------------------------------------------

;@----------------------------------------------------------------------------
blkTgrMapper:				;@ Switch bank for 0x8000-0xBFFF, 16 banks.
;@----------------------------------------------------------------------------
	stmfd sp!,{r3,z80optbl,lr}
	ldr z80optbl,=Z80OpTable

	and r3,r0,#0xF
	mov r3,r3,lsl#1
	add r1,r3,#4
	mov r0,#0x10
	bl z80Mapper
	add r1,r3,#5
	mov r0,#0x20
	bl z80Mapper

	ldmfd sp!,{r3,z80optbl,pc}
;@----------------------------------------------------------------------------
z80Mapper:		;@ Rom paging..
;@----------------------------------------------------------------------------
	ands r0,r0,#0xFF			;@ Safety
	bxeq lr
	stmfd sp!,{r3-r8,lr}
	ldr r5,=MEMMAPTBL_
	ldr r2,[r5,r1,lsl#2]!
	ldr r3,[r5,#-1024]			;@ RDMEMTBL_
	ldr r4,[r5,#-2048]			;@ WRMEMTBL_

	mov r5,#0
	cmp r1,#0xF8
	movmi r5,#12

	add r6,z80optbl,#z80ReadTbl
	add r7,z80optbl,#z80WriteTbl
	add r8,z80optbl,#z80MemTbl
	b z80MemAps
z80MemApl:
	add r6,r6,#4
	add r7,r7,#4
	add r8,r8,#4
z80MemAp2:
	add r3,r3,r5
	sub r2,r2,#0x2000
z80MemAps:
	movs r0,r0,lsr#1
	bcc z80MemApl				;@ C=0
	strcs r3,[r6],#4			;@ readmem_tbl
	strcs r4,[r7],#4			;@ writemem_tb
	strcs r2,[r8],#4			;@ memmap_tbl
	bne z80MemAp2

;@------------------------------------------
z80Flush:		;@ Update cpu_pc & lastbank
;@------------------------------------------
	reEncodePC

	ldmfd sp!,{r3-r8,lr}
	bx lr


;@----------------------------------------------------------------------------

romNum:
	.long 0						;@ romnumber
romInfo:						;@ Keep emuflags/BGmirror together for savestate/loadstate
emuFlags:
	.byte 0						;@ emuflags      (label this so GUI.c can take a peek) see EmuSettings.h for bitfields
//scaling:
	.byte SCALED				;@ (display type)
	.byte 0,0					;@ (sprite follow val)
cartFlags:
	.byte 0 					;@ cartflags
	.space 3

romStart:
	.long 0
vromBase0:
	.long 0
vromBase1:
	.long 0
vromBase2:
	.long 0
	.pool

	.section .bss
WRMEMTBL_:
	.space 256*4
RDMEMTBL_:
	.space 256*4
MEMMAPTBL_:
	.space 256*4
SOUND_RAM:
	.space 0x800
ROM_Space:
	.space 0xD8400
;@----------------------------------------------------------------------------
	.end
#endif // #ifdef __arm__
