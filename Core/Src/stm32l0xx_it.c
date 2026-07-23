/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l0xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gps.h"
#include "printf.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t gps_buf[128];
int gps_idx = 0;
int to_read = 0;
int reading = 0;

int tim_tick = 0;
uint32_t tim1, tim2;
uint32_t tim_diff;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32L0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM22 global interrupt.
  */
void TIM22_IRQHandler(void)
{
  /* USER CODE BEGIN TIM22_IRQn 0 */
  if (LL_TIM_IsActiveFlag_CC1(TIM22)) {
    LL_TIM_ClearFlag_CC1(TIM22);

    if (tim_tick == 0) {
      tim1 = LL_TIM_IC_GetCaptureCH1(TIM22);
    }
    tim_tick++;

    if (tim_tick > 51) {
      tim2 = LL_TIM_IC_GetCaptureCH1(TIM22);

      if (tim2 >= tim1)
        tim_diff = tim2 - tim1;
      else
        tim_diff = (0xFFFF - tim1) + tim2 + 1;

      float freq = (2000000.0f * 51) / tim_diff;
      float hum = -0.1548f * freq + 1037;
      n_printf("💧 Hum %.1f%%\n", hum);

      tim_tick = 0;
    }
  }

  /* USER CODE END TIM22_IRQn 0 */
  /* USER CODE BEGIN TIM22_IRQn 1 */

  /* USER CODE END TIM22_IRQn 1 */
}

/**
  * @brief This function handles LPUART1 global interrupt / LPUART1 wake-up interrupt through EXTI line 28.
  */
void LPUART1_IRQHandler(void)
{
  /* USER CODE BEGIN LPUART1_IRQn 0 */
  if (LL_LPUART_IsActiveFlag_RXNE(LPUART1) && LL_LPUART_IsEnabledIT_RXNE(LPUART1)) {
    uint8_t c = LL_LPUART_ReceiveData8(LPUART1);

    if (reading == 0 && c == 0xb5) {
      reading = 1;
      gps_idx = 0;
      gps_buf[gps_idx++] = c;
    } else if (reading == 1) {
      if (gps_idx == 1 && c != 0x62) {
        reading = 0;
      } else {
        gps_buf[gps_idx++] = c;
        if (gps_idx <= 6) {
          if (gps_idx == 6) {
            to_read = (gps_buf[5] << 8) | gps_buf[4];
            to_read += 2;
            if (to_read > 127) reading = 0;
          }
        } else {
          to_read--;
          if (to_read == 0) {
            parse_ubx(gps_buf+2);
            gps_idx = 0;
            reading = 0;
          }
        }
      }
    }
  }
  if (LL_LPUART_IsActiveFlag_ORE(LPUART1)) {
    LL_LPUART_ClearFlag_ORE(LPUART1);
  }
  /* USER CODE END LPUART1_IRQn 0 */
  /* USER CODE BEGIN LPUART1_IRQn 1 */

  /* USER CODE END LPUART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
