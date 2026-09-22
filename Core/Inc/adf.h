#ifndef ADF_H
#define ADF_H
#include <stdint.h>

void adf_init();
void adf_set_freq(float f);

void adf_off();
void adf_on(float f, uint8_t pa);

#endif
