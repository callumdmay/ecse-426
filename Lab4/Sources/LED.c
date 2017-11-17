#include "stm32f4xx.h"
#include "tim.h"
#include "stm32f4xx_hal_tim.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "accelerometer.h"
#include "keypad.h"
#include <stdlib.h>

//Prototypes
void LEDSet(int diff[2]);
void comparison (float actual[2], int pitch, int roll, int diff[2]);
void Thread_LED (void const *argument);

osThreadId tid_Thread_LED;                              // thread id
osThreadDef(Thread_LED, osPriorityNormal, 1, 0);

void start_thread_LED (void) {
  if (tid_Thread_LED == NULL) {
    tid_Thread_LED = osThreadCreate(osThread(Thread_LED), NULL);
  }
}

//Entry point function for LED thread
void Thread_LED (void const *argument) {
  while(1) {
		osDelay(5);
    //Check for thread sleep mode
    osEvent event = osSignalWait(0x05, 0);
    if (event.status == osEventSignal) {
      osSignalWait(0x04, osWaitForever);
    }

    //Wait for accelerometer and keypad state to be free
    osMutexWait(acc_mutex, osWaitForever);
    osMutexWait(keypad_mutex, osWaitForever);

    //Perform comparison of keypad values and measured accelerometer values
    int compared_values[2];
    comparison(axis_angles, kpState.pitch_angle, kpState.roll_angle, compared_values);

    //Release mutex's before updating the LED's
    osMutexRelease(keypad_mutex);
    osMutexRelease(acc_mutex);
		LEDSet(compared_values);
  }
}

void LED_PowerUp (void)
{	//start PWM LEDs
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
}

void LED_PowerDown (void)
{	//stop PWM LEDs
	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
}

void LEDSet(int diff[2]) {
	
  int MAX_DIFFERENCE = 180;
  int roll;
  int pitch;
	int MAX_VALUE = 100;
	//percentage of the maximum difference (180 deg) of the pitch and roll values
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
	
	//set PWM duty cycle
  __HAL_TIM_SET_COMPARE( & htim4, TIM_CHANNEL_1, roll);
  __HAL_TIM_SET_COMPARE( & htim4, TIM_CHANNEL_3, roll);

  __HAL_TIM_SET_COMPARE( & htim4, TIM_CHANNEL_2, pitch);
  __HAL_TIM_SET_COMPARE( & htim4, TIM_CHANNEL_4, pitch);
}

//compare actual angle to entered value
void comparison (float actual[2], int pitch, int roll, int diff[2]) {
  //pitch
	diff[0] = abs(pitch - (int)actual[0]);
	//roll
  diff[1] = abs(roll - (int)actual[1]);
}
