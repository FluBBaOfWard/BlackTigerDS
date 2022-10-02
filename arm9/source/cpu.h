#ifndef CPU_HEADER
#define CPU_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#include "ARMZ80/ARMZ80.h"

extern u8 waitMaskIn;
extern u8 waitMaskOut;
extern ARMZ80Core z80CPU1;

void run(void);
void stepFrame(void);
void cpuReset(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CPU_HEADER
