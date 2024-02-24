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
#include "cpu.h"
#include "ARMZ80/Version.h"
#include "BlackTigerVideo/Version.h"
#include "../../arm7/source/YM2203/Version.h"

#define EMUVERSION "V0.2.1 2024-02-24"

static void uiDebug(void);

const fptr fnMain[] = {nullUI, subUI, subUI, subUI, subUI, subUI, subUI, subUI, subUI, subUI, subUI};

const fptr fnList0[] = {uiDummy};
const fptr fnList1[] = {ui9, loadState, saveState, saveSettings, resetGame};
const fptr fnList2[] = {ui4, ui5, ui6, ui7, ui8};
const fptr fnList3[] = {uiDummy};
const fptr fnList4[] = {autoBSet, autoASet, controllerSet, swapABSet};
const fptr fnList5[] = {scalingSet, flickSet, gammaSet};
const fptr fnList6[] = {speedSet, autoStateSet, autoSettingsSet, autoPauseGameSet, powerSaveSet, screenSwapSet, sleepSet};
const fptr fnList7[] = {debugTextSet, fgrLayerSet, bgrLayerSet, sprLayerSet, stepFrame};
const fptr fnList8[] = {coinASet, coinBSet, difficultSet, continueSet, cabinetSet, livesSet, demoSet, flipSet, serviceSet};
const fptr fnList9[] = {quickSelectGame, quickSelectGame, quickSelectGame, quickSelectGame, quickSelectGame, quickSelectGame, quickSelectGame};
const fptr fnList10[] = {uiDummy};
const fptr *const fnListX[] = {fnList0, fnList1, fnList2, fnList3, fnList4, fnList5, fnList6, fnList7, fnList8, fnList9, fnList10};
const u8 menuXItems[] = {ARRSIZE(fnList0), ARRSIZE(fnList1), ARRSIZE(fnList2), ARRSIZE(fnList3), ARRSIZE(fnList4), ARRSIZE(fnList5), ARRSIZE(fnList6), ARRSIZE(fnList7), ARRSIZE(fnList8), ARRSIZE(fnList9), ARRSIZE(fnList10)};
const fptr drawUIX[] = {uiNullNormal, uiFile, uiOptions, uiAbout, uiController, uiDisplay, uiSettings, uiDebug, uiDipswitches, uiLoadGame, uiDummy};

u8 gGammaValue = 0;

const char *const autoTxt[] = {"Off", "On", "With R"};
const char *const speedTxt[] = {"Normal", "200%", "Max", "50%"};
const char *const brighTxt[] = {"I", "II", "III", "IIII", "IIIII"};
const char *const sleepTxt[] = {"5min", "10min", "30min", "Off"};
const char *const ctrlTxt[] = {"1P", "2P"};
const char *const dispTxt[] = {"Unscaled", "Scaled"};
const char *const flickTxt[] = {"No Flicker", "Flicker"};

const char *const coinTxt[]	= {
	"1 Coin 1 Credit",  "1 Coin 2 Credits", "1 Coin 3 Credits", "1 Coin 4 Credits",
	"1 Coin 5 Credits", "2 Coins 1 Credit", "3 Coins 1 Credit", "4 Coins 1 Credit"};
const char *const diffTxt[] = {"Easiest", "Very Easy", "Easy", "Quite Easy", "Normal", "Hard", "Very Hard", "Hardest"};
const char *const livesTxt[] = {"3", "2", "5", "7"};
const char *const bonusTxt[] = {"30K 70K 70K+", "40K 80K 80K+", "50K 100K 100K+", "50K 200K 200K+"};
const char *const cabTxt[] = {"Cocktail", "Upright"};
const char *const singleTxt[] = {"Single", "Dual"};


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

void autoLoadGame(void) {
	ui9();
	quickSelectGame();
}

void quickSelectGame(void) {
	while (loadGame(selected)) {
		ui10();
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
	drawMenuItem("Debug");
	drawMenuItem("DipSwitches");
}

void uiAbout() {
	cls(1);
	drawTabs();
	drawMenuText("Select: Insert coin", 4, 0);
	drawMenuText("Start:  Start button", 5, 0);
	drawMenuText("DPad:   Move character", 6, 0);
	drawMenuText("Up:     Climb up", 7, 0);
	drawMenuText("Down:   Crouch/climb down", 8, 0);
	drawMenuText("B:      Attack", 9, 0);
	drawMenuText("A:      Jump", 10, 0);

	char s[10];
	int2Str(coinCounter0, s);
	drawStrings("CoinCounter1:", s, 1, 16, 0);
	int2Str(coinCounter1, s);
	drawStrings("CoinCounter2:", s, 1, 17, 0);

	drawMenuText("BlackTigerDS " EMUVERSION, 20, 0);
	drawMenuText("ARMZ80       " ARMZ80VERSION, 21, 0);
	drawMenuText("BT Video     " BTVIDEOVERSION, 22, 0);
	drawMenuText("ARMYM2203    " ARMYM2203VERSION, 23, 0);
}

void uiController() {
	setupSubMenu("Controller Settings");
	drawSubItem("B Autofire:", autoTxt[autoB]);
	drawSubItem("A Autofire:", autoTxt[autoA]);
	drawSubItem("Controller:", ctrlTxt[(joyCfg>>29)&1]);
	drawSubItem("Swap A-B:  ", autoTxt[(joyCfg>>10)&1]);
}

void uiDisplay() {
	setupSubMenu("Display Settings");
	drawSubItem("Display:", dispTxt[gScaling]);
	drawSubItem("Scaling:", flickTxt[gFlicker]);
	drawSubItem("Gamma:", brighTxt[gGammaValue]);
}

void uiSettings() {
	setupSubMenu("Settings");
	drawSubItem("Speed:", speedTxt[(emuSettings>>6)&3]);
	drawSubItem("Autoload State:", autoTxt[(emuSettings>>2)&1]);
	drawSubItem("Autosave Settings:", autoTxt[(emuSettings>>9)&1]);
	drawSubItem("Autopause Game:", autoTxt[emuSettings&1]);
	drawSubItem("Powersave 2nd Screen:",autoTxt[(emuSettings>>1)&1]);
	drawSubItem("Emulator on Bottom:", autoTxt[(emuSettings>>8)&1]);
	drawSubItem("Autosleep:", sleepTxt[(emuSettings>>4)&3]);
}

void uiDebug() {
	setupSubMenu("Debug");
	drawSubItem("Debug Output:", autoTxt[gDebugSet&1]);
	drawSubItem("Disable Foreground:", autoTxt[gGfxMask&1]);
	drawSubItem("Disable Background:", autoTxt[(gGfxMask>>1)&1]);
	drawSubItem("Disable Sprites:", autoTxt[(gGfxMask>>4)&1]);
	drawSubItem("Step Frame", NULL);
}

void uiDipswitches() {
	setupSubMenu("Dipswitch Settings");
	drawSubItem("Coin A:", coinTxt[gDipSwitch0 & 0x7]);
	drawSubItem("Coin B:", coinTxt[(gDipSwitch0>>3) & 0x7]);
	drawSubItem("Difficulty:", diffTxt[(gDipSwitch1>>2)&7]);
	drawSubItem("Allow Continue:", autoTxt[(~gDipSwitch1>>6)&1]);
	drawSubItem("Cabinet:", cabTxt[(gDipSwitch1>>7)&1]);
	drawSubItem("Lives:", livesTxt[gDipSwitch1 & 3]);
	drawSubItem("Demo Sound:", autoTxt[(~gDipSwitch1>>5)&1]);
	drawSubItem("Flip Screen:", autoTxt[(gDipSwitch0>>6)&1]);
	drawSubItem("Service Mode:", autoTxt[(gDipSwitch0>>7)&1]);
}

void uiLoadGame() {
	setupSubMenu("Load Game");
	int i;
	for (i=0; i<ARRSIZE(blktigerGames); i++) {
		drawSubItem(blktigerGames[i].fullName, NULL);
	}
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
