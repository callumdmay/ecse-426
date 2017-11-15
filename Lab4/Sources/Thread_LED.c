/*******************************************************************************
  * @file    Thread_LED.c
  * @author  Amirhossein Shahshahani
	* @version V1.2.0
  * @date    10-Nov-2017
  * @brief   This file initializes one LED as an output, implements the LED thread
  *					 which toggles and LED, and function which creates and starts the thread
  ******************************************************************************
  */

#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"

void Thread_LED_1 (void const *argument);                 // thread function
osThreadId tid_Thread_LED_1;                              // thread id

/*		**** depending on the version of the CMSIS library, you may need to make your threads in different way:   *******

	osThreadDef(Thread_1,Thread_LED, osPriorityNormal, osPriorityNormal, 128);
	osThreadDef(Thread_2, Thread_LED_2, osPriorityNormal, osPriorityNormal, 128);
	osThreadDef(Thread_3, Thread_LED_3, osPriorityNormal, osPriorityNormal, 128);

	tid_Thread_LED_1 = osThreadCreate(osThread(Thread_1), NULL);
	tid_Thread_LED_2 = osThreadCreate(osThread(Thread_2), NULL);
	tid_Thread_LED_3 = osThreadCreate(osThread(Thread_3), NULL);

	This is a style when you create a project with CubMx as it uses the newest version

*/

// Following is different format of creating your threads. This project is based on the older CMSIS version.
osThreadDef(Thread_LED_1, osPriorityNormal, 1, 0);
GPIO_InitTypeDef 				LED_configuration;

/*----------------------------------------------------------------------------
 *      Create the thread within RTOS context
 *---------------------------------------------------------------------------*/
int start_Thread_LED (void) {

	tid_Thread_LED_1 = osThreadCreate(osThread(Thread_LED_1), NULL);
  //if (!tid_Thread_LED_1) return(-1);
	//else if (!tid_Thread_LED_2) return(-1);
	//else if (!tid_Thread_LED_3) return(-1);
 return(0);
}

 /*----------------------------------------------------------------------------
*      Thread  'LED_Thread': Toggles LED
 *---------------------------------------------------------------------------*/
	void Thread_LED_1 (void const *argument) {
		while(1){
				osDelay(1000);
				HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
			}
	}

/*----------------------------------------------------------------------------
 *      Initialize the GPIO associated with the LED
 *---------------------------------------------------------------------------*/
	void initializeLED_IO (void){

	__HAL_RCC_GPIOD_CLK_ENABLE();

	LED_configuration.Pin		= GPIO_PIN_12|GPIO_PIN_15;
	LED_configuration.Mode 	= GPIO_MODE_OUTPUT_PP;
	LED_configuration.Speed	= GPIO_SPEED_FREQ_VERY_HIGH;
	LED_configuration.Pull	= GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &LED_configuration);
}
/*----------------------------------------------------------------------------
 *
 *---------------------------------------------------------------------------*/
