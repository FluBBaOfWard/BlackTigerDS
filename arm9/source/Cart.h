#ifndef CART_HEADER
#define CART_HEADER

#ifdef __cplusplus
extern "C" {
#endif

extern u32 g_ROM_Size;
extern u32 g_emuFlags;
extern u8 g_cartFlags;
extern u8 gArcadeGameSet;

extern u8 ROM_Space[0x24220];

void machineInit(void);
void loadCart(int, int);
void ejectCart(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CART_HEADER
