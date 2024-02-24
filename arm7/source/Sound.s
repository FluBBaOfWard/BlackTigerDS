#ifdef __arm__

#include "YM2203/YM2203.i"

	.global soundInit
	.global soundReset
	.global soundMixer
	.global setMuteSoundGUI
	.global setMuteSoundGame
	.global ym1StatusR
	.global ym1DataR
	.global ym1IndexW
	.global ym1DataW
	.global ym2StatusR
	.global ym2DataR
	.global ym2IndexW
	.global ym2DataW
	.global YM2203_1
	.global YM2203_2
	.global memclr_
	.global memset_

//	.extern pauseEmulation


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

//	ldr r0,=YM2203_1
//	mov r1,#1
//	bl YM2203_init				;@ Sound

	ldmfd sp!,{lr}
//	bx lr

;@----------------------------------------------------------------------------
soundReset:
;@----------------------------------------------------------------------------
	stmfd sp!,{lr}

	ldr r0,=YM2203_1
//	ldr r1,=cpu1SetIRQ
	mov r1,#0
	bl ym2203Reset				;@ Sound

	ldr r0,=YM2203_2
	mov r1,#0
	bl ym2203Reset				;@ Sound

	ldmfd sp!,{lr}
	bx lr

;@----------------------------------------------------------------------------
setMuteSoundGUI:
	.type   setMuteSoundGUI STT_FUNC
;@----------------------------------------------------------------------------
//	ldr r1,=pauseEmulation		;@ Output silence when emulation paused.
//	ldr r0,[r1]
	strb r0,muteSoundGUI
	bx lr
;@----------------------------------------------------------------------------
setMuteSoundGame:			;@ For System E ?
;@----------------------------------------------------------------------------
	strb r0,muteSoundGame
	bx lr
;@----------------------------------------------------------------------------
soundMixer:					;@ r0=length, r1=pointer
;@----------------------------------------------------------------------------
;@	mov r11,r11
	stmfd sp!,{r0,r1,lr}

	ldr r2,muteSound
	cmp r2,#0
	b silenceMix			// Change !!!

//	ldr r1,pcmPtr0
	ldr r2,=YM2203_1
	bl ym2203Mixer
/*
	ldmfd sp,{r0,r1}
	ldr r3,pcmPtr0
wavLoop:
	ldrh r2,[r3],#2
	strh r2,[r1],#2
	subs r0,r0,#1
	bhi wavLoop
*/
	ldmfd sp!,{r0,r1,lr}
	bx lr

silenceMix:
	ldmfd sp!,{r0,r1}
	mov r12,r0
	mov r2,#0
silenceLoop:
	subs r12,r12,#1
	strhpl r2,[r1],#2
	bhi silenceLoop

	ldmfd sp!,{lr}
	bx lr

;@----------------------------------------------------------------------------
ym1StatusR:
;@----------------------------------------------------------------------------
	ldr r0,=YM2203_1
	b ym2203StatusR
;@----------------------------------------------------------------------------
ym1DataR:
;@----------------------------------------------------------------------------
	ldr r0,=YM2203_1
	b ym2203DataR
;@----------------------------------------------------------------------------
ym1IndexW:
;@----------------------------------------------------------------------------
	ldr r1,=YM2203_1
	b ym2203IndexW
;@----------------------------------------------------------------------------
ym1DataW:
;@----------------------------------------------------------------------------
	ldr r1,=YM2203_1
	b ym2203DataW
;@----------------------------------------------------------------------------
ym2StatusR:
;@----------------------------------------------------------------------------
	ldr r0,=YM2203_2
	b ym2203StatusR
;@----------------------------------------------------------------------------
ym2DataR:
;@----------------------------------------------------------------------------
	ldr r0,=YM2203_2
	b ym2203DataR
;@----------------------------------------------------------------------------
ym2IndexW:
;@----------------------------------------------------------------------------
	ldr r1,=YM2203_2
	b ym2203IndexW
;@----------------------------------------------------------------------------
ym2DataW:
;@----------------------------------------------------------------------------
	ldr r1,=YM2203_2
	b ym2203DataW

;@----------------------------------------------------------------------------
memclr_:					;@ r0=Dest r1=word count
;@	Exit with r0 unchanged, r2=r1, r1=0
;@----------------------------------------------------------------------------
	mov r2,r1
	mov r1,#0
;@----------------------------------------------------------------------------
memset_:					;@ r0=Dest r1=data r2=word count
;@	Exit with r0 & r1 unchanged, r2=0
;@----------------------------------------------------------------------------
	subs r2,r2,#1
	strpl r1,[r0,r2,lsl#2]
	bhi memset_
	bx lr
;@----------------------------------------------------------------------------
pcmPtr0:	.long WAVBUFFER
pcmPtr1:	.long WAVBUFFER+528

muteSound:
muteSoundGUI:
	.byte 0
muteSoundGame:
	.byte 0
soundLatch:
	.byte 0
	.space 1

	.section .bss
	.align 2
YM2203_1:
	.space ymSize
YM2203_2:
	.space ymSize
FREQTBL:
	.space 1024*2
WAVBUFFER:
	.space 0x1000
;@----------------------------------------------------------------------------
	.end
#endif // #ifdef __arm__
