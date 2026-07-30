#ifndef NTC_H
#define NTC_H
#include "main.h"
#include "stm32l051xx.h"


static struct {
    GPIO_TypeDef *port_s; // series
    uint32_t pin_s;
    GPIO_TypeDef *port_p; // parallel
    uint32_t pin_p;
    float r_s;
    float r_p;
} ress[] = {
    {
        OUT_NTC_12k_GPIO_Port, OUT_NTC_12k_Pin,
        0, 0,
        12.1e3, 2e20
    },
    {
        OUT_NTC_36k_GPIO_Port, OUT_NTC_36k_Pin,
        OUT_NTC_330k_GPIO_Port, OUT_NTC_330k_Pin,
        36.5e3, 330e3
    },
    {
        OUT_NTC_475k_GPIO_Port, OUT_NTC_475k_Pin,
        OUT_NTC_2M_GPIO_Port, OUT_NTC_2M_Pin,
        475e3, 2000e3
    },
};

float ntc_rindex(int index);

#endif