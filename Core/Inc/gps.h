#ifndef GPS_H
#define GPS_H
#include <stdint.h>

#if ENABLE_GPS
void switch_ubx();

void parse_ubx(uint8_t *buf);
#endif

#endif
