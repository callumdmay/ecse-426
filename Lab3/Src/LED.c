#include "stm32f4xx.h"
#include "tim.h"
#include "stm32f4xx_hal_tim.h"




void LEDSet(int diff[2])
{
int a = 180;
int x = diff[0]*100/a;
int y =	diff[1]*100/a;
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, x);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, x);
	
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, y);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, y);
}
