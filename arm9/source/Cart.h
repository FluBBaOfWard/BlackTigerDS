#ifndef CART_HEADER
#define CART_HEADER

#ifdef __cplusplus
extern "C" {
#endif

extern u8 ROM_Space[0x24220];

void machineInit(void);
void loadCart(int, int);
void ejectCart(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CART_HEADER
