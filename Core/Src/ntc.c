#include "ntc.h"

#include <math.h>

#define A ((double)0.0010730351f)
#define B ((double)0.0002412940f)
#define C ((double)0.0000022675f)
#define D ((double)0.0000000653f)

float ntc_rindex(int index) {
    if (index > 2) return 0;

    LL_GPIO_SetPinOutputType(ress[index].port_s, ress[index].pin_s, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetOutputPin(ress[index].port_s, ress[index].pin_s);
    if (ress[index].port_p) {
        LL_GPIO_SetPinOutputType(ress[index].port_p, ress[index].pin_p, LL_GPIO_OUTPUT_PUSHPULL);
        LL_GPIO_ResetOutputPin(ress[index].port_p, ress[index].pin_p);
    }
    LL_mDelay(10);

    float adc = read_adc(LL_ADC_CHANNEL_14);

    LL_GPIO_ResetOutputPin(ress[index].port_s, ress[index].pin_s);
    LL_GPIO_SetPinOutputType(ress[index].port_s, ress[index].pin_s, LL_GPIO_OUTPUT_OPENDRAIN);
    if (ress[index].port_p) {
        LL_GPIO_SetOutputPin(ress[index].port_p, ress[index].pin_p);
        LL_GPIO_SetPinOutputType(ress[index].port_p, ress[index].pin_p, LL_GPIO_OUTPUT_OPENDRAIN);
    }

    double x = (4095.0 - adc) / adc;
    double R = ress[index].r_s / (x - ress[index].r_s / ress[index].r_p);
    double l_R = log(R);
    double T = 1.f / (A + B*l_R + C*l_R*l_R + D*l_R*l_R*l_R);

    return T - 273.15f;
}