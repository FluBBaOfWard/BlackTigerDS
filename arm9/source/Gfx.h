#ifndef GFX_HEADER
#define GFX_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#include "BlackTigerVideo/BlackTigerVideo.h"

extern u8 g_flicker;
extern u8 g_twitch;
extern u8 g_scaling;
extern u8 g_gfxMask;

extern BlkTgrVideo blkTgrVideo_0;
extern u16 EMUPALBUFF[0x200];

void gfxInit(void);
void vblIrqHandler(void);
void paletteInit(u8 gammaVal);
void paletteTxAll(void);
void setPaletteCount(int count);
void refreshGfx(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // GFX_HEADER
