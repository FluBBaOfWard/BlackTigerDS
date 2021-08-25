#ifdef __arm__

#include "c_defs.h"

	.global soundInit
	.global soundReset
	.global setMuteSoundGUI
	.global setMuteSoundGame
	.global soundRamR
	.global soundRamW
	.global soundLatchW
	.global ymRead
	.global ymWrite

	.extern pauseEmulation


;@----------------------------------------------------------------------------

	.syntax unified
	.arm

	.section .text
	.align 2
;@----------------------------------------------------------------------------
soundInit:
	.type soundInit STT_FUNC
;@----------------------------------------------------------------------------
	stmfd sp!,{lr}

	mov r1,#1
//	bl YM2203_init				;@ sound

	ldmfd sp!,{lr}
//	bx lr

;@----------------------------------------------------------------------------
soundReset:
;@----------------------------------------------------------------------------
	stmfd sp!,{lr}

	ldmfd sp!,{lr}
	bx lr

;@----------------------------------------------------------------------------
setMuteSoundGUI:
	.type   setMuteSoundGUI STT_FUNC
;@----------------------------------------------------------------------------
	ldr r1,=pauseEmulation		;@ Output silence when emulation paused.
	ldrb r0,[r1]
	strb r0,muteSoundGUI
	bx lr
;@----------------------------------------------------------------------------
setMuteSoundGame:			;@ For System E ?
;@----------------------------------------------------------------------------
	strb r0,muteSoundGame
	bx lr
;@----------------------------------------------------------------------------
soundRamR:				;@ Ram read 0xC000-0xC7FF
;@----------------------------------------------------------------------------
	tst r12,#0x1800
	bne soundLatchR
	ldr r1,=SOUND_RAM-0xC000
	ldrb r0,[r1,r12]
	bx lr
;@----------------------------------------------------------------------------
soundRamW:				;@ Ram write 0xC000-0xC7FF
;@----------------------------------------------------------------------------
	tst r12,#0x1800
	bne soundLatchW
	ldr r1,=SOUND_RAM-0xC000
	strb r0,[r1,r12]
	bx lr
;@----------------------------------------------------------------------------
soundLatchR:
;@----------------------------------------------------------------------------
	ldrb r0,soundLatch
	bx lr
;@----------------------------------------------------------------------------
soundLatchW:
;@----------------------------------------------------------------------------
	strb r0,soundLatch
	bx lr

;@----------------------------------------------------------------------------
ymRead:
;@----------------------------------------------------------------------------
	bic r1,r12,#0xFF000
	cmp r12,#4
	bx lr
;@----------------------------------------------------------------------------
ymWrite:
;@----------------------------------------------------------------------------
	bic r1,r12,#0xFF000
	cmp r12,#4
	bxmi lr
	adr r1,ymWriteTbl
	ldr r1,[r1,r12,lsl#2]
	orr r1,r0,r1
	mov r0,#15
	b fifoSendValue32
ymWriteTbl:
	.long (FIFO_CHIP0<<20)+(0<<16) // index
	.long (FIFO_CHIP0<<20)+(1<<16) // data
	.long (FIFO_CHIP1<<20)+(0<<16) // index
	.long (FIFO_CHIP1<<20)+(1<<16) // data
;@----------------------------------------------------------------------------
muteSound:
muteSoundGUI:
	.byte 0
muteSoundGame:
	.byte 0
soundLatch:
	.byte 0
	.space 1

FREQTBL:
	.space 1024*2
;@----------------------------------------------------------------------------
	.end
#endif // #ifdef __arm__
