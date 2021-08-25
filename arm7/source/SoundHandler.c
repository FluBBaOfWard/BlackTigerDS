#include <nds.h>
#include <string.h>

#include "c_defs.h"
#include "Sound.h"

#define MIXFREQ 0x5e00
#define MIXCLOCK -0x2b9
//#define MIXCLOCK -0x249
#define MIXBUFSIZE 128

s16 buffer[MIXBUFSIZE*2];

s16 currentSample;
s16 sampleDelta = 1500;

static int chan = 0;

void restartSound(int ch) {
	chan = ch;

	TIMER0_CR = TIMER_ENABLE;
	TIMER1_CR = TIMER_CASCADE | TIMER_IRQ_REQ | TIMER_ENABLE;
	SCHANNEL_CR(0) = SCHANNEL_ENABLE | SOUND_REPEAT | SOUND_VOL(0x7F) | SOUND_PAN(0x40) | SOUND_FORMAT_16BIT;
}

void stopSound() {
	SCHANNEL_CR(0)=0;
	TIMER0_CR = 0;
	TIMER1_CR = 0;
}

int APU_paused=0;

void initSound() {
	int i;
	powerOn(POWER_SOUND); 
	REG_SOUNDCNT = SOUND_ENABLE | SOUND_VOL(0x7F);
	for(i = 0; i < 16; i++) {
		SCHANNEL_CR(i) = 0;
	}
	SCHANNEL_SOURCE(0) = (u32)&buffer[0];
	SCHANNEL_TIMER(0) = MIXCLOCK;
	SCHANNEL_LENGTH(0) = MIXBUFSIZE;
	SCHANNEL_REPEAT_POINT(0) = 0;
	TIMER0_DATA = MIXCLOCK*2;
	TIMER1_DATA = 0x10000 - MIXBUFSIZE;
	memset(buffer, 0, sizeof(buffer));
}

void lidInterrupt(void)
{
	stopSound();
	restartSound(1);
}

void soundInterrupt(void)
{
//	int i;
	chan ^= 1;
	s16 *dst = &buffer[chan*MIXBUFSIZE];
	soundMixer(MIXBUFSIZE, dst);
//	for (i = 0; i < MIXBUFSIZE;i++) {
//		currentSample += sampleDelta;
//		dst[i] = currentSample;
//	}
}

void fifoInterrupt(u32 msg, void *none)					//This should be registered to a fifo channel.
{
	int cmd = (msg >> 20) & 0xF;
	int adr = (msg>>16) & 0xF;
	int val = msg & 0xFF;
	switch (cmd) {
		case FIFO_SET_FREQ:
			sampleDelta = msg & 0xFFFF;
			break;
		case FIFO_CHIP0:
			if (adr == 0) {
				ym1IndexW(val);
			}
			else if (adr == 1) {
				ym1DataW(val);
			}
			break;
		case FIFO_CHIP1:
			if (adr == 0) {
				ym2IndexW(val);
			}
			else if (adr == 1) {
				ym2DataW(val);
			}
			break;
		default:
			break;
	}
}

void soundStartup() {
	
	swiWaitForVBlank();
	initSound();
	restartSound(1);

	fifoSetValue32Handler(FIFO_USER_08, fifoInterrupt, 0);		//use the last IPC channel to comm..
	irqSet(IRQ_TIMER1, soundInterrupt);
}
