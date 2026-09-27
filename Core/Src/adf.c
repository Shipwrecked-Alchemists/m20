#include "adf.h"

#include "main.h"
#include "stm32l0xx_ll_gpio.h"

struct {
    uint16_t error_correction;
    uint8_t r_div;
    uint8_t crystal_doubler;
    uint8_t xoeb;
    uint8_t clock_out_div;
    uint8_t vco_adjust;
    uint8_t output_div;
} adf_r0;

struct {
    uint16_t frac_n;
    uint8_t int_n;
    uint8_t prescaler;
} adf_r1;

struct {
    uint8_t mod_ctrl;
    uint8_t gook;
    uint8_t pa;
    uint16_t mod_deviation;
    uint8_t gfsk_mod_ctrl;
    uint8_t index_counter;
} adf_r2;

struct {
    uint8_t pll_enable;
    uint8_t pa_enable;
    uint8_t clkout_enable;
    uint8_t data_invert;
    uint8_t charge_pump;
    uint8_t bleed_current;
    uint8_t vco_disable;
    uint8_t muxout;
    uint8_t ld_precision;
    uint8_t vco_bias;
    uint8_t pa_bias;
} adf_r3;

void adf_write_register(uint32_t data) {
    uint16_t wl = 5;
    LL_GPIO_SetOutputPin(OUT_ADF_LE_GPIO_Port, OUT_ADF_LE_Pin);

    for (uint16_t n = 0; n < (wl * 2); n++)
        asm("NOP");

    LL_GPIO_ResetOutputPin(OUT_ADF_LE_GPIO_Port, OUT_ADF_LE_Pin);
    LL_GPIO_ResetOutputPin(OUT_ADF_Data_GPIO_Port, OUT_ADF_Data_Pin);
    LL_GPIO_ResetOutputPin(OUT_ADF_CLK_GPIO_Port, OUT_ADF_CLK_Pin);

    for (uint16_t n = 0; n < wl; n++)
        asm("NOP");

    for (int i = 0; i < 32; i++) {
        LL_GPIO_ResetOutputPin(OUT_ADF_CLK_GPIO_Port, OUT_ADF_CLK_Pin);
        if (data & 0b10000000000000000000000000000000) {
            LL_GPIO_SetOutputPin(OUT_ADF_Data_GPIO_Port, OUT_ADF_Data_Pin);
        } else {
            LL_GPIO_ResetOutputPin(OUT_ADF_Data_GPIO_Port, OUT_ADF_Data_Pin);
        }

        for (uint16_t n = 0; n < (wl * 2); n++)
            asm("NOP");

        LL_GPIO_SetOutputPin(OUT_ADF_CLK_GPIO_Port, OUT_ADF_CLK_Pin);
        data = data << 1;
    }

    LL_GPIO_SetOutputPin(OUT_ADF_LE_GPIO_Port, OUT_ADF_LE_Pin);
    for (uint16_t n = 0; n < (wl * 2); n++)
        asm("NOP");

    LL_GPIO_ResetOutputPin(OUT_ADF_CLK_GPIO_Port, OUT_ADF_CLK_Pin);
    LL_GPIO_ResetOutputPin(OUT_ADF_LE_GPIO_Port, OUT_ADF_LE_Pin);
}


void adf_write_r0() {
    uint32_t data = 0x0 |
        (adf_r0.error_correction & 0x7FF) << 2 | (adf_r0.r_div & 0xF) << 13 |
        (adf_r0.crystal_doubler & 0x1) << 17 | (adf_r0.xoeb & 0x1) << 18 |
        (adf_r0.clock_out_div & 0xF) << 19 | (adf_r0.vco_adjust & 0x3) << 23 |
        (adf_r0.output_div & 0x3) << 25;
    adf_write_register(data);
}

void adf_write_r1() {
    uint32_t data = 0x1 |
        (adf_r1.frac_n & 0xFFF) << 2 | adf_r1.int_n << 14 | (adf_r1.prescaler & 1) << 22;
    adf_write_register(data);
}

void adf_write_r2() {
    uint32_t data = 0x2 |
        (adf_r2.mod_ctrl & 3) << 2 | (adf_r2.gook & 1) << 4 |
        (adf_r2.pa & 0x3F) << 5 | (adf_r2.mod_deviation & 0x1FF) << 11 |
        (adf_r2.gfsk_mod_ctrl & 7) << 20 | (adf_r2.index_counter & 3) << 23;
    adf_write_register(data);
}

void adf_write_r3() {
    uint32_t data = 0x3 |
        (adf_r3.pll_enable & 1) << 2 | (adf_r3.pa_enable & 1) << 3 |
        (adf_r3.clkout_enable & 1) << 4 | (adf_r3.data_invert & 1) << 5 |
        (adf_r3.charge_pump & 3) << 6 | (adf_r3.bleed_current & 3) << 8 |
        (adf_r3.vco_disable & 1) << 10 | (adf_r3.muxout & 0xF) << 11 |
        (adf_r3.ld_precision & 1) << 15 | (adf_r3.vco_bias & 0xF) << 16 |
        (adf_r3.pa_bias & 7) << 20;
    adf_write_register(data);
}

void adf_init() {
    LL_GPIO_SetOutputPin(OUT_ADF_CE_GPIO_Port, OUT_ADF_CE_Pin);
    adf_r0.error_correction = 20;
    adf_r0.r_div = 1;
    adf_r0.crystal_doubler = 0;
    adf_r0.xoeb = 1;
    adf_r0.clock_out_div = 8;
    adf_r0.vco_adjust = 3;
    adf_r0.output_div = 1;
    adf_write_r0();

    adf_r1.frac_n = 1782;
    adf_r1.int_n = 50;
    adf_r1.prescaler = 0;
    adf_write_r1();

    adf_r2.mod_ctrl = 0; // FSK
    adf_r2.gook = 0;
    adf_r2.pa = 0x3F;
    adf_r2.mod_deviation = 5;
    adf_r2.gfsk_mod_ctrl = 0;
    adf_r2.index_counter = 0;
    adf_write_r2();

    adf_r3.pll_enable = 1;
    adf_r3.pa_enable = 1;
    adf_r3.clkout_enable = 0;
    adf_r3.data_invert = 0;
    adf_r3.charge_pump = 2;
    adf_r3.bleed_current = 0;
    adf_r3.vco_disable = 0;
    adf_r3.muxout = 3;
    adf_r3.ld_precision = 0;
    adf_r3.vco_bias = 0;
    adf_r3.pa_bias = 7;
    adf_write_r3();

    adf_set_freq(433500000.f);
}

void adf_set_freq(float f) {
    float latch = f / 8000000;
    uint32_t n_int = latch;
    latch = latch - n_int;
    uint32_t n_frac = latch * 4096;

    adf_r1.int_n = n_int;
    adf_r1.frac_n = n_frac;
    adf_write_r1();
}

void adf_on(float f, uint8_t pa) {
    LL_GPIO_SetOutputPin(OUT_ADF_CE_GPIO_Port, OUT_ADF_CE_Pin);
    adf_set_freq(f);

    adf_r2.pa = pa;

    adf_r3.pa_enable = 1;

    adf_write_r0();
    adf_write_r1();
    adf_write_r2();
    adf_write_r3();

    LL_GPIO_ResetOutputPin(OUT_RF_Boost_GPIO_Port, OUT_RF_Boost_Pin);
}

void adf_off() {
    adf_r2.pa = 0;
    adf_write_r2();

    adf_r3.pa_enable = 0;
    adf_write_r3();

    LL_GPIO_SetOutputPin(OUT_RF_Boost_GPIO_Port, OUT_RF_Boost_Pin);
}