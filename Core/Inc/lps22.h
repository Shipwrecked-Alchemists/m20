#ifndef LPS22_H
#define LPS22_H

#define LPS_WHO_AM_I 0x0F

#define LPS_PRESS_OUT_XL 0x28
#define LPS_PRESS_OUT_L 0x29
#define LPS_PRESS_OUT_H 0x2A

#define LPS_TEMP_OUT_L 0x2B
#define LPS_TEMP_OUT_H 0x2C

#define LPS_CTRL_REG1 0x10
#define LPS_CTRL_REG2 0x11
#define LPS_RES_CONF 0x1A

#if ENABLE_LPS
int lps22_init();
float lps_temp();
float lps_press();
#endif

#endif
