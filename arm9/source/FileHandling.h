#ifndef FILEHANDLING_HEADER
#define FILEHANDLING_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#include "Shared/FileHelper.h"

#define FILEEXTENSIONS ".zip"

int loadSettings(void);
void saveSettings(void);
int loadNVRAM(void);
void saveNVRAM(void);
void loadState(void);
void saveState(void);
bool loadGame(int gameNr);
bool loadRoms(int gameNr, bool doLoad);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // FILEHANDLING_HEADER
