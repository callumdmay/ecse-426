/*******************************************************************************
  * @file    main.c
  * @author  Amirhossein Shahshahani
	* @version V1.2.0
  * @date    10-Nov-2017
  * @brief   This file demonstrates flasing one LED at an interval of one second
	*          RTX based using CMSIS-RTOS
  ******************************************************************************
  */

#include "stdbool.h"
#include "string.h"
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "RTE_Components.h"             // Component selection
#include "accelerometer.h"
#include "segment_display.h"
#include "filter.h"
#include "LED.h"
#include "tim.h"
#include "clock.h"
#include "keypad.h"
#include "gpio.h"

extern osThreadId tid_Thread_keypad, tid_Thread_acc, tid_Thread_segment_display, tid_Thread_LED;
void EXTI0_IRQHandler(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

/**
	These lines are mandatory to make CMSIS-RTOS RTX work with te new Cube HAL
*/
#ifdef RTE_CMSIS_RTOS_RTX
extern uint32_t os_time;

uint32_t HAL_GetTick(void) {
  return os_time;
}
#endif

/**
  * Main function
  */
int main (void) {
  osKernelInitialize();                     /* initialize CMSIS-RTOS          */

  HAL_Init();                               /* Initialize the HAL Library     */
  SystemClock_Config();                     /* Configure the System Clock     */
	MX_GPIO_Init();

  //Init 4 LEDs with PWM
  MX_TIM4_Init();
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

  //Init functions
  initKeypad();
  initSegmentDisplay();
  initializeACC();
	initAccInterrupt();

  //Start threads
  start_thread_keypad();
  start_thread_segment_display();

	osKernelStart();                          /* start thread execution         */
}
