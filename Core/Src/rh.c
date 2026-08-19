#include "rh.h"

#include <math.h>
#include <stdint.h>

#include "main.h"

uint16_t rh_ticks = 0;
uint32_t rh_counts = 0;

uint32_t tim_overflow = 0;

void update_rh(void) {
    if (rh_ticks == 0) {
        LL_TIM_SetCounter(TIM22, 0);
        tim_overflow = 0;
        rh_ticks++;
        LL_TIM_EnableCounter(TIM22);
    }
    else if (rh_ticks >= 0xA0) {
        rh_counts = LL_TIM_GetCounter(TIM22) + (tim_overflow << 16);

        LL_TIM_DisableCounter(TIM22);
        rh_ticks = 0;
        LL_TIM_ClearFlag_UPDATE(TIM22);
        LL_TIM_ClearFlag_CC1(TIM22);
    }
    else {
        rh_ticks++;
    }
}

static float rh(float rh_ref, float rh_temp){
    float rh_cal = -1.0f;
    float humval;
    float x;

    humval = 6.4e8f / (rh_ref + 80000.0f);
    x = rh_counts * humval * (1.0f - 5.8e-4f * (rh_temp - 25.0f));
    x = 4.16e9f / x;
    x = 10.087f * x * x * x - 211.62f * x * x + 1388.2f * x - 2797.0f;
    if (rh_counts < 128000.0f) {
        if (x > -20.0f && x < 120.0f) {
            rh_cal = x;
            if (rh_cal < 0.0f)   rh_cal = 0.0f;
            if (rh_cal > 100.0f) rh_cal = 100.0f;
        }
    }
    return rh_cal;
}

static float rh_temp(float adc_rh_temp){
    const float Rs  = 22.1e3f;
    const float R25 = 2.2e3f;
    const float b   = 3650.0f;
    const float T25 = 25.0f + 273.15f;
    float T = 0.0f;
    float x, R;

    x = (4095.0f - adc_rh_temp) / adc_rh_temp;
    R = Rs / x;
    if (R > 0.0f) {
        T = 1.0f / (1.0f / T25 + (1.0f / b) * log(R / R25));
    }

    return T - 273.15f;
}

float get_humidity(void) {
    uint16_t rh_ref = *(uint16_t*)0x0808001C; // RH calibration of original firmware

    LL_GPIO_SetOutputPin(OUT_NTC_RH_GPIO_Port, OUT_NTC_RH_Pin);
    LL_mDelay(10);
    uint16_t adc_rh = read_adc(LL_ADC_CHANNEL_15);
    LL_GPIO_ResetOutputPin(OUT_NTC_RH_GPIO_Port, OUT_NTC_RH_Pin);

    float temp = rh_temp(adc_rh);

    return rh(rh_ref, temp);
}