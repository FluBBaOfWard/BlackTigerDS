#include <nds.h>

#include "Gui.h"
#include "Shared/EmuMenu.h"
#include "Shared/EmuSettings.h"
#include "Shared/FileHelper.h"
#include "Shared/AsmExtra.h"
#include "Main.h"
#include "FileHandling.h"
#include "Cart.h"
#include "Gfx.h"
#include "io.h"
#include "ARMZ80/Version.h"
#include "BlackTigerVideo/Version.h"
#include "../../arm7/source/YM2203/Version.h"

#define EMUVERSION "V0.2.1 2022-09-28"

const fptr fnMain[] = {nullUI, subUI, subUI, subUI, subUI, subUI, subUI, subUI, subUI, subUI};

const fptr fnList0[] = {uiDummy};
const fptr fnList1[] = {ui8, loadState, saveState, saveSettings, resetGame};
const fptr fnList2[] = {ui4, ui5, ui6, ui7};
const fptr fnList3[] = {uiDummy};
const fptr fnList4[] = {autoBSet, autoASet, controllerSet, swapABSet};
const fptr fnList5[] = {scalingSet, flickSet, gammaSet, fgrLayerSet, bgrLayerSet, sprLayerSet};
const fptr fnList6[] = {speedSet, autoStateSet, autoSettingsSet, autoPauseGameSet, powerSaveSet, screenSwapSet, debugTextSet, sleepSet};
const fptr fnList7[] = {coinASet, coinBSet, difficultSet, continueSet, cabinetSet, livesSet, demoSet, flipSet, serviceSet};
const fptr fnList8[] = {quickSelectGame, quickSelectGame, quickSelectGame, quickSelectGame, quickSelectGame, quickSelectGame, quickSelectGame};
const fptr fnList9[] = {uiDummy};
const fptr *const fnListX[] = {fnList0, fnList1, fnList2, fnList3, fnList4, fnList5, fnList6, fnList7, fnList8, fnList9};
const u8 menuXItems[] = {ARRSIZE(fnList0), ARRSIZE(fnList1), ARRSIZE(fnList2), ARRSIZE(fnList3), ARRSIZE(fnList4), ARRSIZE(fnList5), ARRSIZE(fnList6), ARRSIZE(fnList7), ARRSIZE(fnList8), ARRSIZE(fnList9)};
const fptr drawUIX[] = {uiNullNormal, uiFile, uiOptions, uiAbout, uiController, uiDisplay, uiSettings, uiDipswitches, uiLoadGame, uiDummy};
const u8 menuXBack[] = {0,0,0,0,2,2,2,2,1,8};

u8 gGammaValue = 0;

char *const autoTxt[]	= {"Off", "On", "With R"};
char *const speedTxt[]	= {"Normal", "200%", "Max", "50%"};
char *const brighTxt[]	= {"I", "II", "III", "IIII", "IIIII"};
char *const sleepTxt[]	= {"5min", "10min", "30min", "Off"};
char *const ctrlTxt[]	= {"1P", "2P"};
char *const dispTxt[]	= {"Unscaled", "Scaled"};
char *const flickTxt[]	= {"No Flicker", "Flicker"};

char *const coinTxt[]	= { "1 Coin 1 Credit", "1 Coin 2 Credits","1 Coin 3 Credits","1 Coin 4 Credits",\
							"1 Coin 5 Credits","2 Coins 1 Credit","3 Coins 1 Credit","4 Coins 1 Credit"};
char *const diffTxt[]	= {"Easiest", "Very Easy", "Easy", "Quite Easy", "Normal", "Hard", "Very Hard", "Hardest"};
char *const livesTxt[]	= {"3", "2", "5", "7"};
char *const bonusTxt[]	= {"30K 70K 70K+", "40K 80K 80K+", "50K 100K 100K+", "50K 200K 200K+"};
char *const cabTxt[]	= {"Cocktail", "Upright"};
char *const singleTxt[]	= {"Single", "Dual"};


/// This is called at the start of the emulator
void setupGUI() {
	emuSettings = AUTOPAUSE_EMULATION;
	keysSetRepeat(25, 4);	// Delay, repeat.
	openMenu();
}

/// This is called when going from emu to ui.
void enterGUI() {
}

/// This is called going from ui to emu.
void exitGUI() {
}

void quickSelectGame(void) {
	while (loadGame(selected)) {
		setSelectedMenu(9);
		if (!browseForFileType(FILEEXTENSIONS)) {
			backOutOfMenu();
			return;
		}
	}
	closeMenu();
}

void uiNullNormal() {
	uiNullDefault();
}

void uiFile() {
	setupMenu();
	drawMenuItem("Load Game");
	drawMenuItem("Load State");
	drawMenuItem("Save State");
	drawMenuItem("Save Settings");
	drawMenuItem("Reset Game");
	if (enableExit) {
		drawMenuItem("Quit Emulator");
	}
}

void uiOptions() {
	setupMenu();
	drawMenuItem("Controller");
	drawMenuItem("Display");
	drawMenuItem("Settings");
	drawMenuItem("DipSwitches");
}

void uiAbout() {
	cls(1);
	drawTabs();
	drawText(" Select: Insert coin",4,0);
	drawText(" Start:  Start button",5,0);
	drawText(" DPad:   Move character",6,0);
	drawText(" Up:     Climb up",7,0);
	drawText(" Down:   Crouch/climb down",8,0);
	drawText(" B:      Attack",9,0);
	drawText(" A:      Jump",10,0);

	drawText(" BlackTigerDS " EMUVERSION, 20, 0);
	drawText(" ARMZ80       " ARMZ80VERSION, 21, 0);
	drawText(" BT Video     " BTVIDEOVERSION, 22, 0);
	drawText(" ARMYM2203    " ARMYM2203VERSION, 23, 0);
}

void uiController() {
	setupSubMenu("Controller Settings");
	drawSubItem("B Autofire: ", autoTxt[autoB]);
	drawSubItem("A Autofire: ", autoTxt[autoA]);
	drawSubItem("Controller: ", ctrlTxt[(joyCfg>>29)&1]);
	drawSubItem("Swap A-B:   ", autoTxt[(joyCfg>>10)&1]);
}

void uiDisplay() {
	setupSubMenu("Display Settings");
	drawSubItem("Display: ", dispTxt[gScaling]);
	drawSubItem("Scaling: ", flickTxt[gFlicker]);
	drawSubItem("Gamma: ", brighTxt[gGammaValue]);
	drawSubItem("Disable Foreground: ", autoTxt[gGfxMask&1]);
	drawSubItem("Disable Background: ", autoTxt[(gGfxMask>>1)&1]);
	drawSubItem("Disable Sprites: ", autoTxt[(gGfxMask>>4)&1]);
}

void uiSettings() {
	setupSubMenu("Settings");
	drawSubItem("Speed: ", speedTxt[(emuSettings>>6)&3]);
	drawSubItem("Autoload State: ", autoTxt[(emuSettings>>2)&1]);
	drawSubItem("Autosave Settings: ", autoTxt[(emuSettings>>9)&1]);
	drawSubItem("Autopause Game: ", autoTxt[emuSettings&1]);
	drawSubItem("Powersave 2nd Screen: ",autoTxt[(emuSettings>>1)&1]);
	drawSubItem("Emulator on Bottom: ", autoTxt[(emuSettings>>8)&1]);
	drawSubItem("Debug Output: ", autoTxt[gDebugSet&1]);
	drawSubItem("Autosleep: ", sleepTxt[(emuSettings>>4)&3]);
}

void uiDipswitches() {
//	char s[10];
	setupSubMenu("Dipswitch Settings");
	drawSubItem("Coin A: ", coinTxt[gDipSwitch0 & 0x7]);
	drawSubItem("Coin B: ", coinTxt[(gDipSwitch0>>3) & 0x7]);
	drawSubItem("Difficulty: ", diffTxt[(gDipSwitch1>>2)&7]);
	drawSubItem("Allow Continue: ", autoTxt[(~gDipSwitch1>>6)&1]);
	drawSubItem("Cabinet: ", cabTxt[(gDipSwitch1>>7)&1]);
	drawSubItem("Lives: ", livesTxt[gDipSwitch1 & 3]);
	drawSubItem("Demo Sound: ", autoTxt[(~gDipSwitch1>>5)&1]);
	drawSubItem("Flip Screen: ", autoTxt[(gDipSwitch0>>6)&1]);
	drawSubItem("Service Mode: ", autoTxt[(gDipSwitch0>>7)&1]);

//	int2str(g_coin0, s);
//	drawSubItem("CoinCounter1:       ", s);
//	int2str(g_coin1, s);
//	drawSubItem("CoinCounter2:       ", s);
}

void uiLoadGame() {
	setupSubMenu("Load game");
	drawMenuItem(" Black Tiger");
	drawMenuItem(" Black Tiger (older)");
	drawMenuItem(" Black Tiger (bootleg set 1)");
	drawMenuItem(" Black Tiger (bootleg set 2)");
	drawMenuItem(" Black Dragon (Japan)");
	drawMenuItem(" Black Dragon (bootleg)");
	drawMenuItem(" Black Tiger / Black Dragon (mixed bootleg?)");
}

void nullUINormal(int key) {
	if (key & KEY_TOUCH) {
		openMenu();
	}
}

void nullUIDebug(int key) {
	if (key & KEY_TOUCH) {
		openMenu();
	}
}

void resetGame() {
	loadCart(0,0);
}


//---------------------------------------------------------------------------------
/// Switch between Player 1 & Player 2 controls
void controllerSet() {					// See io.s: refreshEMUjoypads
	joyCfg ^= 0x20000000;
}

/// Swap A & B buttons
void swapABSet() {
	joyCfg ^= 0x400;
}

/// Turn on/off scaling
void scalingSet(){
	gScaling ^= 0x01;
	refreshGfx();
}

/// Change gamma (brightness)
void gammaSet() {
	gGammaValue++;
	if (gGammaValue > 4) gGammaValue=0;
	paletteInit(gGammaValue);
	paletteTxAll();					// Make new palette visible
	setupMenuPalette();
}

/// Turn on/off rendering of foreground
void fgrLayerSet(){
	gGfxMask ^= 0x01;
}
/// Turn on/off rendering of background
void bgrLayerSet(){
	gGfxMask ^= 0x02;
}
/// Turn on/off rendering of sprites
void sprLayerSet(){
	gGfxMask ^= 0x10;
}


/// Number of coins for credits
void coinASet() {
	int i = (gDipSwitch0+1) & 0x7;
	gDipSwitch0 = (gDipSwitch0 & ~0x7) | i;
}
/// Number of coins for credits
void coinBSet() {
	int i = (gDipSwitch0+0x08) & 0x38;
	gDipSwitch0 = (gDipSwitch0 & ~0x38) | i;
}
/// Game difficulty
void difficultSet() {
	int i = (gDipSwitch1+0x04) & 0x1C;
	gDipSwitch1 = (gDipSwitch1 & ~0x1C) | i;
}
/// Allow continue
void continueSet() {
	gDipSwitch1 ^= 0x40;
}
/// Cocktail/upright
void cabinetSet() {
	gDipSwitch1 ^= 0x80;
}
/// Number of lifes to start with
void livesSet() {
	int i = (gDipSwitch1+1) & 3;
	gDipSwitch1 = (gDipSwitch1 & ~3) | i;
}
/// Demo sound on/off
void demoSet() {
	gDipSwitch1 ^= 0x20;
}
/// Flip screen
void flipSet() {
	gDipSwitch0 ^= 0x40;
}
/// Test/Service mode
void serviceSet() {
	gDipSwitch0 ^= 0x80;
}
