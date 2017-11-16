#include "stm32f4xx_hal.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "segment_display.h"
#include "keypad.h"

void Thread_segment_display (void const *argument);
void updateSegmentDisplay(char *num_buffer);
void updateDigit(int digit, int value);
void updateSegments(int value);

osThreadId tid_Thread_segment_display;                              // thread id
osThreadDef(Thread_segment_display, osPriorityNormal, 1, 0);

//Start thread for ACC
void start_thread_segment_display (void) {
	tid_Thread_segment_display = osThreadCreate(osThread(Thread_segment_display), NULL);
}

//accelerometer thread entry point function
void Thread_segment_display (void const *argument) {
	while(1) {
    osDelay(5);
    osMutexWait(keypad_mutex, osWaitForever);
    if (kpState.operation_mode == true) {
      char angle[3]= {'\0', '\0', '\0'};
      if (kpState.disp_state == ROLL) {
        if (kpState.disp_type == ENTERED) {
          sprintf(angle, "%d", kpState.roll_angle);
        } else {
         // int roll = (int)axis_angles[0];
          //sprintf(angle, "%d", roll);
        }
        osMutexRelease(keypad_mutex);
        updateSegmentDisplay(angle);
      } else if (kpState.disp_state == PITCH) {
        if (kpState.disp_type == ENTERED) {
          sprintf(angle, "%d", kpState.pitch_angle);
        } else {
          //int pitch = (int)axis_angles[1];
          //sprintf(angle, "%d", pitch);
        }
        osMutexRelease(keypad_mutex);
        updateSegmentDisplay(angle);
      }
    } else {
      osMutexRelease(keypad_mutex);
      updateSegmentDisplay(kpState.num_buffer);
    }
	}
}

void initSegmentDisplay(void) {

  //Display segments
  GPIO_InitTypeDef GPIO_InitDef1;
  GPIO_InitDef1.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitDef1.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitDef1.Pull = GPIO_NOPULL;
  GPIO_InitDef1.Speed = GPIO_SPEED_HIGH;

  GPIO_InitTypeDef GPIO_InitDef2;
  GPIO_InitDef2.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitDef2.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitDef2.Pull = GPIO_NOPULL;
  GPIO_InitDef2.Speed = GPIO_SPEED_HIGH;

  HAL_GPIO_Init(GPIOD, &GPIO_InitDef1);
  HAL_GPIO_Init(GPIOB, &GPIO_InitDef2);

  //Digit selectors
  GPIO_InitTypeDef GPIO_InitDef3;
  GPIO_InitDef3.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitDef3.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitDef3.Pull = GPIO_NOPULL;
  GPIO_InitDef3.Speed = GPIO_SPEED_HIGH;

  HAL_GPIO_Init(GPIOD, &GPIO_InitDef3);
}

//Management function for 7 segment, provide a char array of 4 values to write to the 7 segment
void updateSegmentDisplay(char *num_buffer) {
  static int i = 0;
  if (num_buffer[i] != '\0') {
    updateDigit(i, num_buffer[i] - '0');
  } else {
    updateDigit(i, -1);
  }
   i = i == 3 ? 0 : ++i;
}

//Given a digit index and value, update that value on the 7 segment
void updateDigit(int digit, int value) {
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET); //All digits off
  updateSegments(value);

  switch (digit) {
    case 0:
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET); //digit 1 on
      break;
    case 1:
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET); //digit 2 on
      break;
    case 2:
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET); //digit 3 on
      break;
    case 3:
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET); //digit 4 on
      break;
  }
}

//Update segments for a specific digit
void updateSegments(int value) {
    if(value == -1) {
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_RESET);
      return;
    }
    if ((value != 1) && (value != 4)) {
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET);
    } else {
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET); //reset A
    }

    if ((value != 5) && (value != 6)) {
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
    } else {
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET); //reset B
    }

    if ((value != 2)) {
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
    } else {
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET); //reset C
    }

    if ((value != 1) && (value != 4) && (value != 7) && (value != 9)) {
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
    } else {
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET); //reset D
    }

    if ((value == 2) || (value == 6) || (value == 8) || (value == 0)) {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
    } else {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); //reset E
    }

    if ((value != 1) && (value != 2) && (value != 3) && (value != 7)) {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    } else {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //reset F
    }

    if ((value != 1) && (value != 7) && (value != 0)) {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
    } else {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); //reset G
    }
}
