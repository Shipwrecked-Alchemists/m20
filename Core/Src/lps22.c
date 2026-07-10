#include "lps22.h"

#include <stdint.h>

#include "main.h"
#include "printf.h"
#include "stm32l0xx.h"
#include "stm32l0xx_ll_gpio.h"

static uint8_t SPI_RW(uint8_t byte) {
    SET_BIT(SPI1->CR1, SPI_CR1_SPE);

    while (!(SPI1->SR & SPI_SR_TXE)) {}

    *(uint8_t*)&SPI1->DR = byte;

    while (!(SPI1->SR & SPI_SR_RXNE)) {}
    while (SPI1->SR & SPI_SR_BSY) {}

    CLEAR_BIT(SPI1->CR1, SPI_CR1_SPE);
    return *(uint8_t*)&SPI1->DR;
}

static void spi_write(uint8_t addr, uint8_t data) {
    LL_GPIO_ResetOutputPin(OUT_LPS_CS_GPIO_Port, OUT_LPS_CS_Pin);
    SPI_RW(addr);
    SPI_RW(data);
    LL_GPIO_SetOutputPin(OUT_LPS_CS_GPIO_Port, OUT_LPS_CS_Pin);
}

static uint8_t spi_read(uint8_t addr) {
    LL_GPIO_ResetOutputPin(OUT_LPS_CS_GPIO_Port, OUT_LPS_CS_Pin);
    SPI_RW(0x80 | addr);
    uint8_t data = SPI_RW(0x00);
    LL_GPIO_SetOutputPin(OUT_LPS_CS_GPIO_Port, OUT_LPS_CS_Pin);
    return data;
}

uint8_t lps22_whoami() {
    return spi_read(LPS_WHO_AM_I);
}

int lps22_init() {
    if (lps22_whoami() != 0xB1) return -1;

    uint8_t reg = spi_read(LPS_RES_CONF);
    reg &= 0x02;
    spi_write(LPS_RES_CONF, reg);
    LL_mDelay(5);

    reg = 0x00;
    reg |= 0x50; // DR 75 Hz
    reg |= 0x02; // BDU (Block Data Update)
    spi_write(LPS_CTRL_REG1, reg);
    LL_mDelay(5);

    reg = spi_read(LPS_CTRL_REG2);
    reg &= 0xED;
    reg |= 0x10;
    spi_write(LPS_CTRL_REG2, reg);
    LL_mDelay(5);

    return 0;
}

float lps_temp() {
    uint8_t l = spi_read(LPS_TEMP_OUT_L);
    uint8_t h = spi_read(LPS_TEMP_OUT_H);
    uint32_t raw_val = (h << 8) | l;
    return (float)raw_val / 100.f;
}

float lps_press() {
    uint8_t xl = spi_read(LPS_PRESS_OUT_XL);
    uint8_t l = spi_read(LPS_PRESS_OUT_L);
    uint8_t h = spi_read(LPS_PRESS_OUT_H);
    uint32_t raw_val = (h << 16) | (l << 8) | xl;
    return (float)raw_val / 4096.f;
}