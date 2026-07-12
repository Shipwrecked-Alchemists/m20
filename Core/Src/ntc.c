#include "ntc.h"

uint16_t ntc_rindex(int index) {
    LL_GPIO_SetPinOutputType(ress[index].port, ress[index].pin, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetOutputPin(ress[index].port, ress[index].pin);
    LL_mDelay(10);

    LL_ADC_REG_SetSequencerChannels(ADC1, LL_ADC_CHANNEL_14);
    LL_ADC_REG_StartConversion(ADC1);
    while (!LL_ADC_IsActiveFlag_EOC(ADC1)) {}
    uint16_t adc = LL_ADC_REG_ReadConversionData12(ADC1);
    LL_GPIO_ResetOutputPin(ress[index].port, ress[index].pin);
    LL_GPIO_SetPinOutputType(ress[index].port, ress[index].pin, LL_GPIO_OUTPUT_OPENDRAIN);
    return adc;
}