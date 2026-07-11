#include "gps.h"

#include <stdint.h>

#include "main.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_lpuart.h"
#include "printf.h"
// Protocol version 32.01

void lpuart_send(uint8_t *buf, int len) {
    while (len--) {
        while (!LL_LPUART_IsActiveFlag_TXE(LPUART1)) {}
        LL_LPUART_TransmitData8(LPUART1, *buf);
        buf++;
    }
    while (!LL_LPUART_IsActiveFlag_TC(LPUART1)) {}
}

void usart_send(uint8_t *buf, int len) {
    while (len--) {
        while (!LL_LPUART_IsActiveFlag_TXE(USART1)) {}
        LL_LPUART_TransmitData8(USART1, *buf);
        buf++;
    }
    while (!LL_LPUART_IsActiveFlag_TC(USART1)) {}
}

void switch_ubx() {
    LL_GPIO_SetOutputPin(OUT_LED_GPIO_Port, OUT_LED_Pin);
    LL_LPUART_DisableIT_RXNE(LPUART1);
    // 3.10.17.2  Port configuration for UART ports
    uint8_t prt[] = {
        0xb5, 0x62,
        0x06, 0x00,
        0x14, 0x00,

        0x01, /* port ID */
        0x00, /* reserved */
        0x00, 0x00, /* tx Ready */
        0xd0, 0x08, 0x00, 0x00, /* mode */
        0x80, 0x25, 0x00, 0x00, /* baudRate*/
        0x01, 0x00, /* inProtoMax */
        0x01, 0x00, /* outProtoMax */
        0x00, 0x00, /* flags */
        0x00, 0x00,/* reserved */
        0x99, 0x73 /* crc */
    };
    lpuart_send(prt, 28);

    uint8_t gnss[] = {
        0xb5, 0x62,
        0x06, 0x3e,
        0x3c, 0x00,

        0x00, /* message version */
        0x00, 0x20,
        0x07, /* nb block */
        0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01,
        0x01, 0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01,
        0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01,
        0x03, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01,
        0x04, 0x00, 0x08, 0x00, 0x01, 0x00, 0x01, 0x01,
        0x05, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01,
        0x06, 0x08, 0x0e, 0x00, 0x00, 0x00, 0x01, 0x01,
        0x2f, 0x99
    };
    lpuart_send(gnss,68);

    uint8_t msg1[] = {
        0xb5, 0x62,
        0x06, 0x01,
        0x08, 0x00,

        0x01, /* message class */
        0x07, /* message id */
        0x00, 0x01, 0x00, 0x00, 0x00, 0x00, /* rate */
        0x18, 0xe1 /* crc */
    };
    lpuart_send(msg1, 16);

    uint8_t msg2[] = {
        0xb5, 0x62,
        0x06, 0x01,
        0x08, 0x00,

        0x01,
        0x20,
        0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x31, 0x90
    };
    lpuart_send(msg2, 16);

    /*
    uint8_t msg3[] = {
        0xb5, 0x62,
        0x06, 0x01,
        0x08, 0x00,

        0x01,
        0x35,
        0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x46, 0x23
    };
    lpuart_send(msg3, 16);
    */

    LL_LPUART_ClearFlag_ORE(LPUART1);
    LL_LPUART_ClearFlag_FE(LPUART1);
    LL_LPUART_EnableIT_RXNE(LPUART1);
    LL_GPIO_ResetOutputPin(OUT_LED_GPIO_Port, OUT_LED_Pin);
}

uint16_t u16(uint8_t *b) {
    return b[0] | (b[1] << 8);
}

int16_t i16(uint8_t *b) {
    return b[0] | (b[1] << 8);
}

uint32_t u32(uint8_t *b) {
    return b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
}

int32_t i32(uint8_t *b) {
    return b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
}

struct {
    uint32_t time; // Unix timestamp

    uint32_t lat;
    uint32_t lon;
    uint16_t alt;

    uint16_t vel_h;
} frame_part;

typedef enum {NAV_PVT = 7, NAV_TIMEGPS = 0x20} frame_t;
void parse_ubx(uint8_t* buf) {
    LL_LPUART_DisableIT_RXNE(LPUART1);
    n_printf("⌛ Parsing UBX packet ...\n");
    uint8_t class = buf[0];
    uint8_t id = buf[1];
    uint16_t len = (uint16_t)(buf[2] | (buf[3] << 8));
    uint8_t* payload = &buf[4];
    uint16_t crc = *(uint16_t*)&buf[4 + len];

    n_printf("⚙️ Class : 0x%02X\n", class);
    n_printf("⚙️ Id : 0x%02X\n", id);
    n_printf("⚙️ Len : 0x%02X\n", len);
    if (class == 1) {
        switch (id) {
        case NAV_PVT:
            uint16_t flags3 = u16(&payload[78]);
            if (flags3 & 0x0001) {
                n_printf("⚠️ No valid lat/lon/height/hMSL\n");
                break;
            }
            uint32_t lon = i32(&payload[24]);
            uint32_t lat = i32(&payload[28]);
            uint32_t alt = i32(&payload[32]);
            uint32_t vel_h = i32(&payload[60]);
            frame_part.lon = lon;
            frame_part.lat = lat;
            frame_part.alt = alt;
            frame_part.vel_h = vel_h;

            n_printf("🌍 Lon: %.5f\n", lon / 1e7f);
            n_printf("🌍 Lat: %.5f\n", lat / 1e7f);
            n_printf("🌍 Height: %.1f\n", alt / 1e3f);

            n_printf("⚡ Ground speed: %.1fm/s\n", vel_h / 1e3f);
            break;
        case NAV_TIMEGPS:
            uint32_t iTOW = u32(&payload[0]);
            int32_t fTOW  = i32(&payload[4]);
            int16_t week = i16(&payload[8]);
            int8_t leaps = payload[10];

            uint32_t sow = (iTOW * 1e-3) + ((uint32_t) fTOW * 1e-9); // second of
            uint32_t s = (uint32_t)week * 604800 + sow;

            uint32_t timestamp = 315964800 + s - (uint32_t)leaps;
            frame_part.time = timestamp;
            n_printf("🕰️ Timestamp : %u\n", timestamp);
        }
    }
    n_printf("\n");

    LL_LPUART_ClearFlag_ORE(LPUART1);
    LL_LPUART_ClearFlag_FE(LPUART1);
    LL_LPUART_EnableIT_RXNE(LPUART1);
}
