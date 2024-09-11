#ifdef __arm__

#include "Shared/nds_asm.h"
#include "Shared/EmuSettings.h"
#include "ARMZ80/ARMZ80.i"
#include "BlackTigerVideo/BlackTigerVideo.i"

	.global gfxInit
	.global gfxReset
	.global paletteInit
	.global paletteTxAll
	.global refreshGfx
	.global setPaletteCount
	.global convertGfx
	.global gfxState
//	.global oamBufferReady
	.global gFlicker
	.global gTwitch
	.global gScaling
	.global gGfxMask
	.global vblIrqHandler
	.global yStart

	.global Z80Out
	.global blkTgrVideo_0
	.global blkTgrRAM_0
	.global blkTgrVideoCD_0R
	.global blkTgrVideoCD_0W


	.syntax unified
	.arm

	.section .text
	.align 2
;@----------------------------------------------------------------------------
gfxInit:					;@ Called from machineInit
;@----------------------------------------------------------------------------
	stmfd sp!,{lr}

	ldr r0,=OAM_BUFFER1			;@ No stray sprites please
	mov r1,#0x200+SCREEN_HEIGHT
	mov r2,#0x100
	bl memset_
	adr r0,scaleParms
	bl setupSpriteScaling

	ldr r0,=gGammaValue
	ldrb r0,[r0]
	bl paletteInit				;@ Do palette mapping

	bl blkTgrInit

	ldmfd sp!,{pc}

;@----------------------------------------------------------------------------
scaleParms:					;@  NH     FH     NV     FV
	.long OAM_BUFFER1,0x0000,0x0100,0xff01,0x0120,0xfee1
;@----------------------------------------------------------------------------
gfxReset:					;@ Called with CPU reset
;@----------------------------------------------------------------------------
	stmfd sp!,{lr}

	ldr r0,=gfxState
	mov r1,#5					;@ 5*4
	bl memclr_					;@ Clear GFX regs

	mov r1,#REG_BASE
	ldr r0,=0x00FF				;@ Start-end
	strh r0,[r1,#REG_WIN0H]
	mov r0,#0x0000+SCREEN_HEIGHT	;@ Start-end
	strh r0,[r1,#REG_WIN0V]
	mov r0,#0x0000
	strh r0,[r1,#REG_WINOUT]

	ldr r0,=Z80SetIRQPinCurrentCpu
	ldr r1,=blkTgrRAM_0
	bl blkTgrReset0

	ldr r0,=BG_GFX+0x4000		;@ r0 = GBA/NDS BG tileset
	str r0,[btptr,#chrGfxDest]
	add r0,#0x4000				;@ r0 = GBA/NDS BG tileset
	str r0,[btptr,#bgrGfxDest]
	ldr r1,=vromBase0			;@ r1 = source
	ldr r0,[r1],#4
	str r0,[btptr,#chrRomBase]
	ldr r0,[r1],#4
	str r0,[btptr,#bgrRomBase]
	ldr r0,[r1],#4
	str r0,[btptr,#sprRomBase]

	ldmfd sp!,{pc}
;@----------------------------------------------------------------------------
paletteInit:		;@ r0-r3 modified.
	.type paletteInit STT_FUNC
;@ Called by ui.c:  void paletteInit(u8 gammaVal);
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r9,lr}
	mov r1,r0					;@ Gamma value = 0 -> 4
	mov r7,#0xF
	ldr r6,=MAPPED_RGB
	mov r4,#4096				;@ Black Tiger BRG
noMap:							;@ Map 0000bbbbrrrrgggg  ->  0bbbbbgggggrrrrr
	rsb r8,r4,#4096
	and r0,r7,r8,lsr#8			;@ Blue ready
	bl gPrefix
	mov r5,r0

	and r0,r7,r8				;@ Green ready
	bl gPrefix
	orr r5,r0,r5,lsl#5

	and r0,r7,r8,lsr#4			;@ Red ready
	bl gPrefix
	orr r5,r0,r5,lsl#5

	strh r5,[r6],#2
	subs r4,r4,#1
	bne noMap

	ldmfd sp!,{r4-r9,lr}
	bx lr

;@----------------------------------------------------------------------------
gPrefix:
	orr r0,r0,r0,lsl#4
;@----------------------------------------------------------------------------
gammaConvert:	;@ Takes value in r0(0-0xFF), gamma in r1(0-4),returns new value in r0=0x1F
;@----------------------------------------------------------------------------
	rsb r2,r0,#0x100
	mul r3,r2,r2
	rsbs r2,r3,#0x10000
	rsb r3,r1,#4
	orr r0,r0,r0,lsl#8
	mul r2,r1,r2
	mla r0,r3,r0,r2
	mov r0,r0,lsr#13

	bx lr

;@----------------------------------------------------------------------------
setPaletteCount:
	.type setPaletteCount STT_FUNC
;@ Called from C.  void setPaletteCount(int count);
;@----------------------------------------------------------------------------
	ldr btptr,=blkTgrVideo_0
	strb r0,[btptr,#paletteSlots]
	mov r0,#-1
	strb r0,[btptr,#palMemReload]
	b refreshGfx

;@----------------------------------------------------------------------------
vblIrqHandler:
	.type vblIrqHandler STT_FUNC
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r8,lr}
	bl calculateFPS

	ldrb r0,gScaling
	cmp r0,#UNSCALED
	ldrbeq r4,yStart
	movne r4,#0
	add r4,r4,#0x10
	moveq r6,#0
	ldrne r6,=0x80000000 + ((GAME_HEIGHT-SCREEN_HEIGHT)*0x10000) / (SCREEN_HEIGHT-1)		;@ NDS 0x2B10 (was 0x2AAB)
	mov r4,r4,lsl#16
	orr r4,r4,#(GAME_WIDTH-SCREEN_WIDTH)/2
	ldr r7,scrollTemp

	ldr r0,gFlicker
	eors r0,r0,r0,lsl#31
	str r0,gFlicker
	addpl r6,r6,r6,lsl#16

	ldr r2,=SCROLLBUFF
	mov r3,r2

	mov r1,#SCREEN_HEIGHT
scrolLoop2:
	add r5,r4,r7
	stmia r3!,{r4,r5}
	adds r6,r6,r6,lsl#16
	addcs r4,r4,#0x10000
	subs r1,r1,#1
	bne scrolLoop2


	mov r6,#REG_BASE
	strh r6,[r6,#REG_DMA0CNT_H]	;@ DMA0 stop

	add r1,r6,#REG_DMA0SAD		;@ Setup DMA buffer for scrolling:
	ldmia r2!,{r4-r5}			;@ Read
	add r3,r6,#REG_BG0HOFS		;@ DMA0 always goes here
	stmia r3,{r4-r5}			;@ Set 1st value manually, HBL is AFTER 1st line
	ldr r4,=0x96600002			;@ noIRQ hblank 32bit repeat incsrc inc_reloaddst, 2 word
	stmia r1,{r2-r4}			;@ DMA0 go

	add r1,r6,#REG_DMA3SAD

	ldr r2,dmaOamBuffer			;@ DMA3 src, OAM transfer:
	mov r3,#OAM					;@ DMA3 dst
	mov r4,#0x84000000			;@ noIRQ 32bit incsrc incdst
	orr r4,r4,#128*2			;@ 128 sprites * 2 longwords
	stmia r1,{r2-r4}			;@ DMA3 go

	ldr r2,=EMUPALBUFF			;@ DMA3 src, Palette transfer:
	mov r3,#BG_PALETTE			;@ DMA3 dst
	mov r4,#0x84000000			;@ noIRQ 32bit incsrc incdst
	orr r4,r4,#0x100			;@ 256 words (1024 bytes)
	stmia r1,{r2-r4}			;@ DMA3 go

	ldr btptr,=blkTgrVideo_0
	ldrb r2,[btptr,#btVideoEnable]
	ldrb r3,[btptr,#btIrqControl]

	mov r0,#0x0013
	ldrb r1,gGfxMask
	bic r0,r0,r1
	tst r3,#0x80				;@ Chr on?
	bicne r0,r0,#0x01
	tst r2,#0x02				;@ Bgr on?
	bicne r0,r0,#0x02
	tst r2,#0x04				;@ Sprites on?
	bicne r0,r0,#0x10
	strh r0,[r6,#REG_WININ]

	blx scanKeys
	ldmfd sp!,{r4-r8,pc}


;@----------------------------------------------------------------------------
gFlicker:		.byte 1
				.space 2
gTwitch:		.byte 0

gScaling:		.byte SCALED
gGfxMask:		.byte 0
yStart:			.byte 0
				.byte 0
;@----------------------------------------------------------------------------
refreshGfx:					;@ Called from C.
	.type refreshGfx STT_FUNC
;@----------------------------------------------------------------------------
	adr btptr,blkTgrVideo_0
;@----------------------------------------------------------------------------
convertGfx:		;@ Called just before screen start (~line 262)	(r0-r2 safe to use)
;@----------------------------------------------------------------------------
	stmfd sp!,{r3,lr}

	bl preparePalette
	ldr r0,=BG_GFX+0x1000
	bl convertChrMapBlkTgr
	ldr r0,=BG_GFX
	bl convertBgrMapBlkTgr
	ldr r0,tmpOamBuffer
	bl convertSpritesBlkTgr
;@--------------------------
	bl paletteTxAll

	ldr r0,[btptr,#btScrlXReg]
	bic r0,r0,#0xF000
	str r0,scrollTemp

	ldr r0,dmaOamBuffer
	ldr r1,tmpOamBuffer
	str r0,tmpOamBuffer
	str r1,dmaOamBuffer

	mov r0,#1
	str r0,oamBufferReady

	ldr r0,=windowTop			;@ Load wTop, store in wTop+4.......load wTop+8, store in wTop+12
	ldmia r0,{r1-r3}			;@ Load with increment after
	stmib r0,{r1-r3}			;@ Store with increment before

	ldmfd sp!,{r3,lr}
	bx lr

;@----------------------------------------------------------------------------

tmpOamBuffer:		.long OAM_BUFFER1
dmaOamBuffer:		.long OAM_BUFFER2

oamBufferReady:		.long 0
emuPaletteReady:	.long 0

;@----------------------------------------------------------------------------
paletteTxAll:				;@ Called from Gui.c
	.type paletteTxAll STT_FUNC
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r10,lr}

	ldr r10,=MAPPED_RGB
	ldr r9,=0x1FFE				;@ Mask
	ldr r3,=blkTgrRAM_0+0x4800
	ldr r4,=EMUPALBUFF

	add r6,r3,#0x280			;@ Start with sprites
	add r7,r4,#0x200
	mov r5,#128
	bl palMapRow

	ldr r8,=palMapping			;@ r0=destination
	add r3,r3,#0x100			;@ Background

	mov r2,#15
palMapBgr:
	ldrb r0,[r8],#1
	tst r0,#0xF0
	bleq remapPal16
	subs r2,r2,#1
	bpl palMapBgr

	add r3,r3,#0x280			;@ Foreground

	mov r2,#31
palMapChr:
	ldrb r0,[r8],#1
	tst r0,#0xF0
	bleq remapPal32
	subs r2,r2,#1
	bpl palMapChr

	ldmfd sp!,{r4-r10,lr}
	bx lr

;@----------------------------------------------------------------------------
remapPal32:
;@----------------------------------------------------------------------------
	sub r6,r3,r2,lsl#2
	mov r5,#4
	b palMapStart
;@----------------------------------------------------------------------------
remapPal16:
;@----------------------------------------------------------------------------
	sub r6,r3,r2,lsl#4
	mov r5,#16
palMapStart:
	add r7,r4,r0,lsl#5
palMapRow:
	ldrb r0,[r6,#-1]!
	ldrb r1,[r6,#0x400]
	orr r0,r0,r1,lsl#8
	and r0,r9,r0,lsl#1
	ldrh r0,[r10,r0]
	strh r0,[r7],#2
	subs r5,r5,#1
	bne palMapRow

	bx lr
;@----------------------------------------------------------------------------
blkTgrReset0:			;@ r0=IRQ(frameIrqFunc), r1= RAM
;@----------------------------------------------------------------------------
	adr btptr,blkTgrVideo_0
	b blkTgrReset
;@----------------------------------------------------------------------------
blkTgrVideoCD_0R:			;@ RAM read, (0xC000-0xDFFF)
;@----------------------------------------------------------------------------
	movs r1,addy,lsl#19
	ldrpl r2,blkTgrVideo_0+ramPage0
	ldrmi r2,blkTgrVideo_0+ramPage1
	ldrb r0,[r2,r1,lsr#19]
	bx lr
;@----------------------------------------------------------------------------
blkTgrVideoCD_0W:			;@ RAM write  (0xC000-0xDFFF)
;@----------------------------------------------------------------------------
	movs r1,addy,lsl#19
	ldrpl r2,blkTgrVideo_0+ramPage0
	ldrmi r2,blkTgrVideo_0+ramPage1
	strb r0,[r2,r1,lsr#19]
	adr r2,blkTgrVideo_0+dirtyMem
	mov r0,#-1
	strb r0,[r2,r1,lsr#30]
	bx lr
;@----------------------------------------------------------------------------
Z80Out:
;@----------------------------------------------------------------------------
	and r1,addy,#0xFF
	adr btptr,blkTgrVideo_0
	b blkTgrIOWrite

blkTgrVideo_0:
	.space blkTgrSize
;@----------------------------------------------------------------------------

gfxState:
adjustBlend:
	.long 0
windowTop:
	.long 0,0,0,0				;@ L/R scrolling in unscaled mode
scrollTemp:
	.long 0
	.byte 0
	.byte 0
	.byte 0,0

	.section .bss
OAM_BUFFER1:
	.space 0x400
OAM_BUFFER2:
	.space 0x400
DMA0BUFF:
	.space 0x200
SCROLLBUFF:
	.space 0x400*2				;@ Scrollbuffer.
MAPPED_RGB:
	.space 0x2000
EMUPALBUFF:
	.space 0x400
blkTgrRAM_0:
	.space 0x7000
	.space CHRBLOCKCOUNT*4
	.space BGRBLOCKCOUNT*4
	.space SPRBLOCKCOUNT*4
palMapping:
	.space 16+32

;@----------------------------------------------------------------------------
	.end
#endif // #ifdef __arm__
