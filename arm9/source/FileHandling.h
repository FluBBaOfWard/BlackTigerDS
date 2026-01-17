#ifndef FILEHANDLING_HEADER
#define FILEHANDLING_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define FILEEXTENSIONS ".zip"

void initSettings(void);
int loadSettings(void);
int saveSettings(void);
int loadNVRAM(void);
void saveNVRAM(void);
void loadState(void);
void saveState(void);
bool loadGame(int gameNr);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // FILEHANDLING_HEADER
