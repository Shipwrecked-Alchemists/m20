#ifndef NTC_H
#define NTC_H
#include "main.h"
#include "stm32l051xx.h"


static struct {
    GPIO_TypeDef *port;
    uint32_t pin;
    float r;
} ress[] = {
    {OUT_NTC_12k_GPIO_Port, OUT_NTC_12k_Pin, 12.1e3},
    {OUT_NTC_36k_GPIO_Port, OUT_NTC_36k_Pin, 36.5e3},
    {OUT_NTC_330k_GPIO_Port, OUT_NTC_330k_Pin, 330e3},
    {OUT_NTC_475k_GPIO_Port, OUT_NTC_475k_Pin, 475e3},
    {OUT_NTC_2M_GPIO_Port, OUT_NTC_2M_Pin, 2000e3},
};

uint16_t ntc_rindex(int index);

#endif