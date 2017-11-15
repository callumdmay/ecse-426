#include "lis3dsh.h"
#include "stm32f4xx_hal.h"
#include "keypad.h"
#include <math.h>
#include <stdlib.h>

LIS3DSH_InitTypeDef 		Acc_instance;
LIS3DSH_DRYInterruptConfigTypeDef AccIT;
GPIO_InitTypeDef GPIO_InitStruct;
SPI_HandleTypeDef hspi;

double ACC_CALIBRATION_MATRIX[4][3] = {
														{0.000975570176,-0.0000179726958, 0.0000118904418},
														{-0.00000826743781, 0.00100064254, -0.00000108576944},
														{0.00000101638125, 0.00000478541733, 0.000963788305},
														{-0.00438112020, -0.000916585326, 0.000590100884}};

//initialize accelerometer with desired parameters
void initializeACC(void){
  Acc_instance.Axes_Enable				= LIS3DSH_XYZ_ENABLE;
  Acc_instance.AA_Filter_BW				= LIS3DSH_AA_BW_50;
  Acc_instance.Full_Scale					= LIS3DSH_FULLSCALE_2;
  Acc_instance.Power_Mode_Output_DataRate		= LIS3DSH_DATARATE_50;
  Acc_instance.Self_Test					= LIS3DSH_SELFTEST_NORMAL;
  Acc_instance.Continous_Update   = LIS3DSH_ContinousUpdate_Enabled;
  LIS3DSH_Init(&Acc_instance);
  /* Enabling interrupt conflicts with push button. Be careful when you plan to
  use the interrupt of the accelerometer sensor connceted to PIN A.0
  */
}

//initialize interrupt for accelerometer
void ITInit(void){

  AccIT.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
  AccIT.Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;
  AccIT.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;

  LIS3DSH_DataReadyInterruptConfig(&AccIT);

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);


}

//get accelerometer raw data
float* getACC(float *arr) {
  uint8_t status;
  float buffer[3];
  LIS3DSH_Read (&status, LIS3DSH_STATUS, 1);
  //The first four bits denote if we have new data on all XYZ axes,
  //Z axis only, Y axis only or Z axis only. If any or all changed, proceed
  if ((status & 0x0F) != 0x00) {
    LIS3DSH_ReadACC(&buffer[0]);

		arr[0] = (float)buffer[0]; //X
    arr[1] = (float)buffer[1]; //Y
    arr[2] = (float)buffer[2]; //Z


  }
  return arr;
}

//acc contains [x, y, z], conv is output
void conversion(float acc[3], float conv[2]) {
	//Pitch
  conv[0] = atan(acc[0]/(sqrtf(powf(acc[1], 2) + powf(acc[2], 2))));
	//Roll
  conv[1] = atan(acc[1]/(sqrtf(powf(acc[0], 2) + powf(acc[2], 2))));

	float PI = 3.141592654;
	conv[0] = (conv[0]*180/PI)+90;
	conv[1] = (conv[1]*180/PI)+90;

}

void comparison (float actual[2], int pitch, int roll, int diff[2]) {
  //pitch
	diff[0] = abs(pitch - (int)actual[0]);
	//roll
  diff[1] = abs(roll - (int)actual[1]);
}

//matrix multiplication to obtain normalized accelerometer values
void getNormalizedAcc(float acc[3], float output[3]) {
		output[0] = output[1] = output[2] = 0;
		int i, j;
		for (j=0; j < 3; j++)
			for(i=0; i < 4; i++) {
				if (i < 3) {
					output[j] += acc[i] * (float)ACC_CALIBRATION_MATRIX[i][j];
				} else {
					output[j] += (float)ACC_CALIBRATION_MATRIX[i][j];
				}
			}
}
