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
#include "BlackTiger.h"
#include "ARMZ80/Version.h"
#include "BlackTigerVideo/Version.h"
#include "../../arm7/source/YM2203/Version.h"

#define EMUVERSION "V0.2.1 2026-01-04"

static void scalingSet(void);
static const char *getScalingText(void);
static void controllerSet(void);
static const char *getControllerText(void);
static void swapABSet(void);
static const char *getSwapABText(void);
static void fgrLayerSet(void);
static const char *getFgrLayerText(void);
static void bgrLayerSet(void);
static const char *getBgrLayerText(void);
static void sprLayerSet(void);
static const char *getSprLayerText(void);
static void coinASet(void);
static const char *getCoinAText(void);
static void coinBSet(void);
static const char *getCoinBText(void);
static void difficultSet(void);
static const char *getDifficultText(void);
static void continueSet(void);
static const char *getContinueText(void);
static void cabinetSet(void);
static const char *getCabinetText(void);
static void livesSet(void);
static const char *getLivesText(void);
static void demoSet(void);
static const char *getDemoText(void);
static void flipSet(void);
static const char *getFlipText(void);
static void serviceSet(void);
static const char *getServiceText(void);
static void gammaChange(void);

static void ui11(void);

const MItem dummyItems[] = {
	{"", uiDummy}
};
const MItem fileItems[] = {
	{"Load Game", ui9},
	{"Load State", loadState},
	{"Save State", saveState},
	{"Save Settings", saveSettings},
	{"Reset Game", resetGame},
	{"Quit Emulator", ui11},
};
const MItem optionItems[] = {
	{"Controller", ui4},
	{"Display", ui5},
	{"Settings", ui6},
	{"Debug", ui7},
	{"DipSwitches", ui8},
};
const MItem ctrlItems[] = {
	{"B Autofire:", autoBSet, getAutoBText},
	{"A Autofire:", autoASet, getAutoAText},
	{"Controller:", controllerSet, getControllerText},
	{"Swap A-B:  ", swapABSet, getSwapABText},
};
const MItem displayItems[] = {
	{"Display:", scalingSet, getScalingText},
	{"Scaling:", flickSet, getFlickText},
	{"Gamma:", gammaChange, getGammaText},
};
const MItem setItems[] = {
	{"Speed:", speedSet, getSpeedText},
	{"Autoload State:", autoStateSet, getAutoStateText},
	{"Autosave Settings:", autoSettingsSet, getAutoSettingsText},
	{"Autopause Game:", autoPauseGameSet, getAutoPauseGameText},
	{"Powersave 2nd Screen:", powerSaveSet, getPowerSaveText},
	{"Emulator on Bottom:", screenSwapSet, getScreenSwapText},
	{"Autosleep:", sleepSet, getSleepText},
};
const MItem debugItems[] = {
	{"Debug Output:", debugTextSet, getDebugText},
	{"Disable Foreground:", fgrLayerSet, getFgrLayerText},
	{"Disable Background:", bgrLayerSet, getBgrLayerText},
	{"Disable Sprites:", sprLayerSet, getSprLayerText},
	{"Step Frame", stepFrame},
};
const MItem dipItems[] = {
	{"Coin A:", coinASet, getCoinAText},
	{"Coin B:", coinBSet, getCoinBText},
	{"Difficulty:", difficultSet, getDifficultText},
	{"Allow Continue:", continueSet, getContinueText},
	{"Cabinet:", cabinetSet, getCabinetText},
	{"Lives:", livesSet, getLivesText},
	{"Demo Sound:", demoSet, getDemoText},
	{"Flip Screen:", flipSet, getFlipText},
	{"Service Mode:", serviceSet, getServiceText},
};

const MItem fnList9[GAME_COUNT] = {
	{"",quickSelectGame}, {"",quickSelectGame}, {"",quickSelectGame}, {"",quickSelectGame}, {"",quickSelectGame}, {"",quickSelectGame}, {"",quickSelectGame},
};
const MItem quitItems[] = {
	{"Yes ", exitEmulator},
	{"No ", backOutOfMenu},
};

const Menu menu0 = MENU_M("", uiNullNormal, dummyItems);
Menu menu1 = MENU_M("", uiAuto, fileItems);
const Menu menu2 = MENU_M("", uiAuto, optionItems);
const Menu menu3 = MENU_M("", uiAbout, dummyItems);
const Menu menu4 = MENU_M("Controller Settings", uiAuto, ctrlItems);
const Menu menu5 = MENU_M("Display Settings", uiAuto, displayItems);
const Menu menu6 = MENU_M("Settings", uiAuto, setItems);
const Menu menu7 = MENU_M("Debug", uiAuto, debugItems);
const Menu menu8 = MENU_M("Dipswitch Settings", uiAuto, dipItems);
const Menu menu9 = MENU_M("Load Game", uiLoadGame, fnList9);
const Menu menu10 = MENU_M("", uiDummy, dummyItems);
const Menu menu11 = MENU_M("Quit Emulator?", uiAuto, quitItems);

const Menu *const menus[] = {&menu0, &menu1, &menu2, &menu3, &menu4, &menu5, &menu6, &menu7, &menu8, &menu9, &menu10, &menu11 };

const char *const speedTxt[] = {"Normal", "200%", "Max", "50%"};
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
	menu1.itemCount = ARRSIZE(fileItems) - (enableExit?0:1);
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

void uiLoadGame() {
	setupSubMenuText();
	int i;
	for (i=0; i<ARRSIZE(blktigerGames); i++) {
		drawSubItem(blktigerGames[i].fullName, NULL);
	}
}

void ui11() {
	enterMenu(11);
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
const char *getControllerText() {
	return ctrlTxt[(joyCfg>>29)&1];
}

/// Swap A & B buttons
void swapABSet() {
	joyCfg ^= 0x400;
}
const char *getSwapABText() {
	return autoTxt[(joyCfg>>10)&1];
}

/// Turn on/off scaling
void scalingSet(){
	gScaling ^= 0x01;
	refreshGfx();
}
const char *getScalingText() {
	return dispTxt[gScaling];
}

/// Change gamma (brightness)
void gammaChange() {
	gammaSet();
	paletteInit(gGammaValue);
	paletteTxAll();					// Make new palette visible
	setupMenuPalette();
}

/// Turn on/off rendering of foreground
void fgrLayerSet(){
	gGfxMask ^= 0x01;
}
const char *getFgrLayerText() {
	return autoTxt[gGfxMask&1];
}
/// Turn on/off rendering of background
void bgrLayerSet(){
	gGfxMask ^= 0x02;
}
const char *getBgrLayerText() {
	return autoTxt[(gGfxMask>>1)&1];
}
/// Turn on/off rendering of sprites
void sprLayerSet(){
	gGfxMask ^= 0x10;
}
const char *getSprLayerText() {
	return autoTxt[(gGfxMask>>4)&1];
}


/// Number of coins for credits
void coinASet() {
	int i = (gDipSwitch0+1) & 0x7;
	gDipSwitch0 = (gDipSwitch0 & ~0x7) | i;
}
const char *getCoinAText() {
	return coinTxt[gDipSwitch0 & 0x7];
}
/// Number of coins for credits
void coinBSet() {
	int i = (gDipSwitch0+0x08) & 0x38;
	gDipSwitch0 = (gDipSwitch0 & ~0x38) | i;
}
const char *getCoinBText() {
	return coinTxt[(gDipSwitch0>>3) & 0x7];
}
/// Game difficulty
void difficultSet() {
	int i = (gDipSwitch1+0x04) & 0x1C;
	gDipSwitch1 = (gDipSwitch1 & ~0x1C) | i;
}
const char *getDifficultText() {
	return diffTxt[(gDipSwitch1>>2)&7];
}
/// Allow continue
void continueSet() {
	gDipSwitch1 ^= 0x40;
}
const char *getContinueText() {
	return autoTxt[(~gDipSwitch1>>6)&1];
}
/// Cocktail/upright
void cabinetSet() {
	gDipSwitch1 ^= 0x80;
}
const char *getCabinetText() {
	return cabTxt[(gDipSwitch1>>7)&1];
}
/// Number of lifes to start with
void livesSet() {
	int i = (gDipSwitch1+1) & 3;
	gDipSwitch1 = (gDipSwitch1 & ~3) | i;
}
const char *getLivesText() {
	return livesTxt[gDipSwitch1 & 3];
}
/// Demo sound on/off
void demoSet() {
	gDipSwitch1 ^= 0x20;
}
const char *getDemoText() {
	return autoTxt[(~gDipSwitch1>>5)&1];
}
/// Flip screen
void flipSet() {
	gDipSwitch0 ^= 0x40;
}
const char *getFlipText() {
	return autoTxt[(gDipSwitch0>>6)&1];
}
/// Test/Service mode
void serviceSet() {
	gDipSwitch0 ^= 0x80;
}
const char *getServiceText() {
	return autoTxt[(gDipSwitch0>>7)&1];
}
