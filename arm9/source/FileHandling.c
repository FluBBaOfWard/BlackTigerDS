#include <nds.h>
#include <stdio.h>
#include <unistd.h>

#include "FileHandling.h"
#include "Shared/EmuMenu.h"
#include "Shared/EmuSettings.h"
#include "Shared/FileHelper.h"
#include "Shared/Unzip/unzipnds.h"
#include "Shared/EmubaseAC.h"
#include "Main.h"
#include "Gui.h"
#include "Cart.h"
#include "Gfx.h"
#include "io.h"

static const char *const folderName = "acds";
static const char *const settingName = "settings.cfg";

static int selectedGame = 0;
ConfigData cfg;

static bool loadRoms(int gameNr, bool doLoad);

//---------------------------------------------------------------------------------
int loadSettings() {
	FILE *file;

	if (findFolder(folderName)) {
		return 1;
	}
	if ( (file = fopen(settingName, "r")) ) {
		fread(&cfg, 1, sizeof(ConfigData), file);
		fclose(file);
		if (!strstr(cfg.magic,"cfg")) {
			infoOutput("Error in settings file.");
			return 1;
		}
	}
	else {
		infoOutput("Couldn't open file:");
		infoOutput(settingName);
		return 1;
	}

	gScaling    = cfg.scaling&1;
	gFlicker    = cfg.flicker&1;
	gGammaValue = cfg.gammaValue;
	emuSettings  = cfg.emuSettings &~ 0xC0;			// Clear speed setting.
	sleepTime    = cfg.sleepTime;
	joyCfg       = (joyCfg&~0x400)|((cfg.controller&1)<<10);
	strlcpy(currentDir, cfg.currentPath, sizeof(currentDir));
	gDipSwitch0 = cfg.dipSwitchBT0;
	gDipSwitch1 = cfg.dipSwitchBT1;
	gDipSwitch2 = cfg.dipSwitchBT2;
	gDipSwitch3 = cfg.dipSwitchBT3;

	infoOutput("Settings loaded.");
	return 0;
}
void saveSettings() {
	FILE *file;

	strcpy(cfg.magic,"cfg");
	cfg.scaling     = gScaling&1;
	cfg.flicker     = gFlicker&1;
	cfg.gammaValue  = gGammaValue;
	cfg.emuSettings = emuSettings &~ 0xC0;			// Clear speed setting.
	cfg.sleepTime   = sleepTime;
	cfg.controller  = (joyCfg>>10)&1;
	strlcpy(cfg.currentPath, currentDir, sizeof(currentDir));
	cfg.dipSwitchBT0 = gDipSwitch0;
	cfg.dipSwitchBT1 = gDipSwitch1;
	cfg.dipSwitchBT2 = gDipSwitch2;
	cfg.dipSwitchBT3 = gDipSwitch3;

	if (findFolder(folderName)) {
		return;
	}
	if ( (file = fopen(settingName, "w")) ) {
		fwrite(&cfg, 1, sizeof(ConfigData), file);
		fclose(file);
		infoOutput("Settings saved.");
	}
	else {
		infoOutput("Couldn't open file:");
		infoOutput(settingName);
	}
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

bool loadRoms(int gameNr, bool doLoad) {
	return loadACRoms(ROM_Space, blktigerGames, gameNr, ARRSIZE(blktigerGames), doLoad);
}
