#include <nds.h>
#include <fat.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/dir.h>

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
#include "BlackTiger.h"

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

void loadState(void) {
	u32 *statePtr;
	FILE *file;
	char stateName[32];

	if (findFolder(folderName)) {
		return;
	}
	strlcpy(stateName, games[selectedGame].gameName, sizeof(stateName));
	strlcat(stateName, ".sta", sizeof(stateName));
	int stateSize = getStateSize();
	if ( (file = fopen(stateName, "r")) ) {
		if ( (statePtr = malloc(stateSize)) ) {
			fread(statePtr, 1, stateSize, file);
			unpackState(statePtr);
			free(statePtr);
			infoOutput("Loaded state.");
		}
		else {
			infoOutput("Couldn't alloc mem for state.");
		}
		fclose(file);
	}
}
void saveState(void) {
	u32 *statePtr;
	FILE *file;
	char stateName[32];

	if (findFolder(folderName)) {
		return;
	}
	strlcpy(stateName, games[selectedGame].gameName, sizeof(stateName));
	strlcat(stateName, ".sta", sizeof(stateName));
	int stateSize = getStateSize();
	if ( (file = fopen(stateName, "w")) ) {
		if ( (statePtr = malloc(stateSize)) ) {
			packState(statePtr);
			fwrite(statePtr, 1, stateSize, file);
			free(statePtr);
			infoOutput("Saved state.");
		}
		else {
			infoOutput("Couldn't alloc mem for state.");
		}
		fclose(file);
	}
}

//---------------------------------------------------------------------------------
bool loadGame(int gameNr) {
	cls(0);
	drawText(" Checking roms", 10, 0);
	if ( loadRoms(gameNr, false) ) {
		return true;
	}
	drawText(" Loading roms", 10, 0);
	loadRoms(gameNr, true);
	selectedGame = gameNr;
	setEmuSpeed(0);
	loadCart(gameNr,0);
	if ( emuSettings & 4 ) {
		loadState();
	}
	else if ( (emuSettings>>10) & 1 ) {
		loadNVRAM();
	}
	return false;
}

bool loadRoms(int gameNr, bool doLoad) {
	int i, j;
	bool found;
	const ArcadeGame *game = &games[gameNr];
	char zipName[32];
	char zipSubName[32];
	u8 *romArea = ROM_Space;
	FILE *file;

	const int romCount = game->romCount;
	strlMerge(zipName, game->gameName, ".zip", sizeof(zipName));

	chdir("/");			// Stupid workaround.
	if ( chdir(currentDir) == -1 ) {
		return true;
	}

	for (i=0; i<romCount; i++) {
		found = false;
		drawSpinner();
		const char *romName = game->roms[i].romName;
		const int romSize = game->roms[i].romSize;
		const u32 romCRC = game->roms[i].romCRC;
		if (strcmp(romName, FILL0XFF) == 0) {
			memset(romArea, 0xFF, romSize);
			romArea += romSize;
			continue;
		}
		if (strcmp(romName, FILL0X00) == 0) {
			memset(romArea, 0x00, romSize);
			romArea += romSize;
			continue;
		}
		if ( (file = fopen(romName, "r")) ) {
			if ( doLoad ) {
				fread(romArea, 1, romSize, file);
				romArea += romSize;
			}
			fclose(file);
			found = true;
		}
		else if ( !(findFileWithCRC32InZip(zipName, romCRC)) ) {
			if ( doLoad ) {
				loadFileWithCRC32InZip(romArea, zipName, romCRC, romSize);
				romArea += romSize;
			}
			found = true;
		}
		else {
			for (j=0; j<GAME_COUNT; j++) {
				strlMerge(zipSubName, games[j].gameName, ".zip", sizeof(zipName));
				if ( !(findFileWithCRC32InZip(zipSubName, romCRC)) ) {
					if ( doLoad ) {
						loadFileWithCRC32InZip(romArea, zipSubName, romCRC, romSize);
						romArea += romSize;
					}
					found = true;
					break;
				}
			}
		}
		if (!found) {
			infoOutput("Couldn't open file:");
			infoOutput(romName);
			return true;
		}
	}
	return false;
}
