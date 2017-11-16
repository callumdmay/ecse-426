#include "stm32f4xx.h"
#include "tim.h"
#include "stm32f4xx_hal_tim.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX

void Thread_LED (void const *argument);
osThreadId tid_Thread_LED;                              // thread id
osThreadDef(Thread_LED, osPriorityNormal, 1, 0);

void start_thread_LED (void) {
	tid_Thread_LED = osThreadCreate(osThread(Thread_LED), NULL);
}

void Thread_LED (void const *argument) {
  while(1) {
  }
}

void LEDSet(int diff[2]) {
  int MAX_DIFFERENCE = 180;
  int roll;
  int pitch;
	int MAX_VALUE = 100;

  if (diff[0] > 5) {
    roll = diff[0] * MAX_VALUE / MAX_DIFFERENCE;
  } else {
    roll = 0;
  }
  if (diff[1] > 5) {
    pitch = diff[1] * MAX_VALUE / MAX_DIFFERENCE;
  } else {
    pitch = 0;
  }

  __HAL_TIM_SET_COMPARE( & htim4, TIM_CHANNEL_1, roll);
  __HAL_TIM_SET_COMPARE( & htim4, TIM_CHANNEL_3, roll);

  __HAL_TIM_SET_COMPARE( & htim4, TIM_CHANNEL_2, pitch);
  __HAL_TIM_SET_COMPARE( & htim4, TIM_CHANNEL_4, pitch);
}
