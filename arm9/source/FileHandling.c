#include <nds.h>
#include <stdio.h>

#include "FileHandling.h"
#include "Shared/EmuMenu.h"
#include "Shared/EmuSettings.h"
#include "Shared/FileHelper.h"
#include "Shared/EmubaseAC.h"
#include "Main.h"
#include "Gui.h"
#include "Cart.h"
#include "Gfx.h"
#include "io.h"
#include "BlackTiger.h"

static const char *const folderName = "acds";
static const char *const settingName = "settings.cfg";

ConfigData cfg;
static int selectedGame = 0;

//---------------------------------------------------------------------------------
void applyConfigData(void) {
	emuSettings  = cfg.emuSettings & ~EMUSPEED_MASK; // Clear speed setting.
	gScaling     = cfg.scaling & SCALED;
	gFlicker     = cfg.flicker & 1;
	gGammaValue  = cfg.gammaValue;
	sleepTime    = cfg.sleepTime;
	joyCfg       = (joyCfg & ~0x400) | ((cfg.controller & 1) << 10);
	strlcpy(currentDir, cfg.currentPath, sizeof(currentDir));
	coinCounter0 = cfg.coinCounter0;
	coinCounter1 = cfg.coinCounter1;

	gDipSwitch0  = cfg.dipSwitchBT0;
	gDipSwitch1  = cfg.dipSwitchBT1;
	gDipSwitch2  = cfg.dipSwitchBT2;
	gDipSwitch3  = cfg.dipSwitchBT3;
}

void updateConfigData(void) {
	strcpy(cfg.magic, "cfg");
	cfg.emuSettings = emuSettings & ~EMUSPEED_MASK; // Clear speed setting.
	cfg.scaling     = gScaling & SCALED;
	cfg.flicker     = gFlicker & 1;
	cfg.gammaValue  = gGammaValue;
	cfg.sleepTime   = sleepTime;
	cfg.controller  = (joyCfg >> 10) & 1;
	strlcpy(cfg.currentPath, currentDir, sizeof(currentDir));
	cfg.coinCounter0 = coinCounter0;
	cfg.coinCounter1 = coinCounter1;

	cfg.dipSwitchBT0 = gDipSwitch0;
	cfg.dipSwitchBT1 = gDipSwitch1;
	cfg.dipSwitchBT2 = gDipSwitch2;
	cfg.dipSwitchBT3 = gDipSwitch3;
}

void initSettings() {
	memset(&cfg, 0, sizeof(ConfigData));
	cfg.emuSettings  = AUTOPAUSE_EMULATION | AUTOLOAD_NVRAM | AUTOSLEEP_OFF;
	cfg.scaling      = SCALED;
	cfg.flicker      = 1;
	cfg.sleepTime    = 60*60*5;
	cfg.dipSwitchBT1 = 0x80;		// Lives, cabinet & demo sound.

	applyConfigData();
}

int loadSettings() {
	FILE *file;
	if (!findFolder(folderName)
		&& (file = fopen(settingName, "r"))) {
		int len = fread(&cfg, 1, sizeof(ConfigData), file);
		fclose(file);
		if (strstr(cfg.magic, "cfg") && len == sizeof(ConfigData)) {
			applyConfigData();
			infoOutput("Settings loaded.");
			return 0;
		}
		else {
			updateConfigData();
			infoOutput("Error in settings file.");
		}
	}
	else {
		infoOutput("Couldn't open file:");
		infoOutput(settingName);
	}
	return 1;
}

int saveSettings() {
	updateConfigData();

	FILE *file;
	if (!findFolder(folderName)
		&& (file = fopen(settingName, "w"))) {
		int len = fwrite(&cfg, 1, sizeof(ConfigData), file);
		fclose(file);
		if (len == sizeof(ConfigData)) {
			infoOutput("Settings saved.");
			return 0;
		}
		infoOutput("Couldn't save settings.");
	}
	else {
		infoOutput("Couldn't open file:");
		infoOutput(settingName);
	}
	return 1;
}

int loadNVRAM() {
	return 0;
}

void saveNVRAM() {
}

void loadState() {
	loadDeviceState(folderName);
}

void saveState() {
	saveDeviceState(folderName);
}

//---------------------------------------------------------------------------------
static bool loadRoms(int gameNr, bool doLoad) {
	return loadACRoms(ROM_Space, blktigerGames, gameNr, ARRSIZE(blktigerGames), doLoad);
}

bool loadGame(int gameNr) {
	cls(0);
	drawText(" Checking roms", 10, 0);
	if (loadRoms(gameNr, false)) {
		return true;
	}
	drawText(" Loading roms", 10, 0);
	loadRoms(gameNr, true);
	selectedGame = gameNr;
	strlcpy(currentFilename, blktigerGames[selectedGame].gameName, sizeof(currentFilename));
	setEmuSpeed(0);
	loadCart(gameNr,0);
	if (emuSettings & AUTOLOAD_STATE) {
		loadState();
	}
	else if (emuSettings & AUTOLOAD_NVRAM) {
		loadNVRAM();
	}
	return false;
}
