/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_ll_adc.h"
#include "stm32l0xx_ll_lpuart.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_crs.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_dma.h"
#include "stm32l0xx_ll_spi.h"
#include "stm32l0xx_ll_usart.h"
#include "stm32l0xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IN_BUTTON_Pin LL_GPIO_PIN_13
#define IN_BUTTON_GPIO_Port GPIOC
#define OUT_LED_Pin LL_GPIO_PIN_14
#define OUT_LED_GPIO_Port GPIOC
#define ADC_PAYLOAD_Pin LL_GPIO_PIN_0
#define ADC_PAYLOAD_GPIO_Port GPIOC
#define IN_IR_Pin LL_GPIO_PIN_1
#define IN_IR_GPIO_Port GPIOA
#define OUT_LPS_CS_Pin LL_GPIO_PIN_4
#define OUT_LPS_CS_GPIO_Port GPIOA
#define OUT_LPS_SCK_Pin LL_GPIO_PIN_5
#define OUT_LPS_SCK_GPIO_Port GPIOA
#define IN_LPS_MISO_Pin LL_GPIO_PIN_6
#define IN_LPS_MISO_GPIO_Port GPIOA
#define OUT_LPS_MOSI_Pin LL_GPIO_PIN_7
#define OUT_LPS_MOSI_GPIO_Port GPIOA
#define ADC_NTC_Pin LL_GPIO_PIN_4
#define ADC_NTC_GPIO_Port GPIOC
#define ADC_BAT_Pin LL_GPIO_PIN_0
#define ADC_BAT_GPIO_Port GPIOB
#define OUT_RF_Boost_Pin LL_GPIO_PIN_12
#define OUT_RF_Boost_GPIO_Port GPIOB
#define OUT_ADF_TX_TIM__Pin LL_GPIO_PIN_13
#define OUT_ADF_TX_TIM__GPIO_Port GPIOB
#define OUT_GPS_ON_Pin LL_GPIO_PIN_14
#define OUT_GPS_ON_GPIO_Port GPIOB
#define OUT_RADIO_EN_Pin LL_GPIO_PIN_15
#define OUT_RADIO_EN_GPIO_Port GPIOB
#define OUT_ADF_CLK_Pin LL_GPIO_PIN_7
#define OUT_ADF_CLK_GPIO_Port GPIOC
#define OUT_ADF_Data_Pin LL_GPIO_PIN_8
#define OUT_ADF_Data_GPIO_Port GPIOC
#define OUT_ADF_LE_Pin LL_GPIO_PIN_9
#define OUT_ADF_LE_GPIO_Port GPIOC
#define OUT_POWER_ON_Pin LL_GPIO_PIN_12
#define OUT_POWER_ON_GPIO_Port GPIOA
#define UART_GPS_TX_Pin LL_GPIO_PIN_10
#define UART_GPS_TX_GPIO_Port GPIOC
#define UART_GPS_RX_Pin LL_GPIO_PIN_11
#define UART_GPS_RX_GPIO_Port GPIOC
#define OUT_ADF_CE_Pin LL_GPIO_PIN_3
#define OUT_ADF_CE_GPIO_Port GPIOB
#define OUT_NTC_475k_Pin LL_GPIO_PIN_4
#define OUT_NTC_475k_GPIO_Port GPIOB
#define OUT_NTC_36k_Pin LL_GPIO_PIN_5
#define OUT_NTC_36k_GPIO_Port GPIOB
#define OUT_NTC_12k_Pin LL_GPIO_PIN_6
#define OUT_NTC_12k_GPIO_Port GPIOB
#define OUT_NTC_2M_Pin LL_GPIO_PIN_8
#define OUT_NTC_2M_GPIO_Port GPIOB
#define OUT_NTC_330k_Pin LL_GPIO_PIN_9
#define OUT_NTC_330k_GPIO_Port GPIOB
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
