/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stdbool.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "clock.h"
#include "accelerometer.h"
#include "segment_display.h"
#include "main.h"
#include "filter.h"
#include "LED.h"
#include "tim.h"
#include "string.h"

/* Private variables ---------------------------------------------------------*/
int SysTickCount;
float acc_output_values[3];
int angle_difference[2];
int inputs[2];
float axis_angles[2];
uint8_t updateFlag;

struct keypadState kpState;

int main(void) {
  updateFlag=0;
  initializeACC();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM4_Init();
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
  ITInit();

  initKeypad();

  initKeypadState(&kpState);

  initSegmentDisplay();
  int display_counter = 0;
  float acc_filtered_values[3];
	char angle[3];
	while (1) {
    display_counter++;
		
		//execute after every accelerometer interrupt
		if (updateFlag ==1 && kpState.operation_mode == true)
		{
			//filter data
			filterValues(acc_output_values, acc_filtered_values);
			
			//normalize values
			float normalized_acc_values[3];
			getNormalizedAcc(acc_filtered_values, normalized_acc_values);
			
			//convert values
      conversion(normalized_acc_values, axis_angles);
			
			//compare values to input
			comparison(axis_angles, kpState.pitch_angle, kpState.roll_angle,  angle_difference);
			
			//set LEDs
			LEDSet(angle_difference);
			
			//reset flag
			updateFlag=0;
		}
		
		//Refresh 7-segment display
    if(display_counter % 10000 == 0) {
      if (kpState.operation_mode == true) {
        updateSegmentDisplay(angle);
      } else {
        updateSegmentDisplay(kpState.num_buffer);
      }
    }
		
		//scan keypad for input
    if (SysTickCount % 30 == 0) {
      processKeypadInput(&kpState);
			int zero[2]={0};
			LEDSet(zero);
			printf("Buffer: %c %c %c\n", kpState.num_buffer[0],kpState.num_buffer[1], kpState.num_buffer[2]);
			printf("Roll %d\n", kpState.roll_angle);
			printf("Pitch %d\n", kpState.pitch_angle);
			printf("Operation %d\n", kpState.operation_mode);
			printf("Monitoring %s\n", kpState.disp_state == ROLL ? "ROLL" : "PITCH");
			printf("\n");
    }

		//update value to be displayed by 7-segment
		if (SysTickCount%250==0  && kpState.operation_mode==true)
		{
				angle[0]=angle[1]=angle[2]= '\0';
        if (kpState.disp_state == ROLL) {
          int roll = (int)axis_angles[0];
          sprintf(angle, "%d", roll);
          
        } else if (kpState.disp_state == PITCH) {
          int pitch = (int)axis_angles[1];
          sprintf(angle, "%d", pitch);
         
        }
		}
	//reset counters
    SysTickCount = SysTickCount == 1000 ? 0 : SysTickCount;
    display_counter = display_counter == 200000 ? 0 : display_counter;
  }
}
//get Accelerometer value and set flag to execute accelerometer functions in main code
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  getACC(acc_output_values);
  updateFlag=1;
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
