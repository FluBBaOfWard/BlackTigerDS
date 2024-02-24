#ifndef CART_HEADER
#define CART_HEADER

#ifdef __cplusplus
extern "C" {
#endif

extern u8 ROM_Space[0x24220];
extern u8 *mainCpu;
extern u8 *soundCpu;
extern u8 *vromBase0;
extern u8 *vromBase1;
extern u8 *vromBase2;
extern u8 *promBase;

void machineInit(void);
void loadCart(int, int);
void ejectCart(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CART_HEADER
