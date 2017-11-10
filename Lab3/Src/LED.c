#include "stm32f4xx.h"
#include "tim.h"
#include "stm32f4xx_hal_tim.h"




void LEDSet(int diff[2])
{
int a = 180;
	int x;
	int y;
	if (diff[0] > 5)
	{
x = diff[0]*840/a;
	}
	else 
	{
		x=0;
	}
	if (diff[1] > 5)
	{
		y =	diff[1]*840/a;
	}
	else
	{
		y=0;
	}
		
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, x);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, x);
	
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, y);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, y);
}
