#ifndef IO_HEADER
#define IO_HEADER

#ifdef __cplusplus
extern "C" {
#endif

extern u32 joyCfg;
extern u32 EMUinput;
extern u8 gDipSwitch0;
extern u8 gDipSwitch1;
extern u8 gDipSwitch2;
extern u8 gDipSwitch3;
extern int coinCounter0;
extern int coinCounter1;

/**
 * Convert device input keys to target keys.
 * @param input NDS/GBA keys
 * @return The converted input.
 */
int convertInput(int input);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IO_HEADER
