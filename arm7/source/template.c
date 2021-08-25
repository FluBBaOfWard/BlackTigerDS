/*---------------------------------------------------------------------------------
---------------------------------------------------------------------------------*/
#include <nds.h>
#include <dswifi7.h>
#include "SoundHandler.h"

//---------------------------------------------------------------------------------
void VBlankHandler(void) {
//---------------------------------------------------------------------------------
	Wifi_Update();
}


//---------------------------------------------------------------------------------
void VCountHandler() {
//---------------------------------------------------------------------------------
	inputGetAndSend();
}

volatile bool exitflag = false;

//---------------------------------------------------------------------------------
void powerButtonCB() {
//---------------------------------------------------------------------------------
	exitflag = true;
}

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	readUserSettings();

	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();
	fifoInit();

	SetYtrigger(80);

	installWifiFIFO();

	installSystemFIFO();

	irqSet(IRQ_VCOUNT, VCountHandler);
	irqSet(IRQ_VBLANK, VBlankHandler);

	irqEnable( IRQ_TIMER1 | IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

	setPowerButtonCB(powerButtonCB);   

	soundStartup();
	// Keep the ARM7 mostly idle
	while (!exitflag) {
		if ( 0 == (REG_KEYINPUT & (KEY_SELECT | KEY_START | KEY_L | KEY_R))) {
			exitflag = true;
		}
		swiWaitForVBlank();
	}
	return 0;
}
