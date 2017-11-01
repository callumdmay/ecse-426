#include "lis3dsh.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"

LIS3DSH_InitTypeDef 		Acc_instance;


void initializeACC(void){
	
	Acc_instance.Axes_Enable				= LIS3DSH_XYZ_ENABLE;
	Acc_instance.AA_Filter_BW				= LIS3DSH_AA_BW_50;
	Acc_instance.Full_Scale					= LIS3DSH_FULLSCALE_2;
	Acc_instance.Power_Mode_Output_DataRate		= LIS3DSH_DATARATE_25;
	Acc_instance.Self_Test					= LIS3DSH_SELFTEST_NORMAL;
	Acc_instance.Continous_Update   = LIS3DSH_ContinousUpdate_Enabled;
	
	LIS3DSH_Init(&Acc_instance);	
	
	/* Enabling interrupt conflicts with push button. Be careful when you plan to 
	use the interrupt of the accelerometer sensor connceted to PIN A.0
	*/
}

 void printACC(void) {
	 uint8_t status;
	 float buffer[3];
	 float accX, accY, accZ;

	 LIS3DSH_Read (&status, LIS3DSH_STATUS, 1);
				//The first four bits denote if we have new data on all XYZ axes, 
		   	//Z axis only, Y axis only or Z axis only. If any or all changed, proceed
				if ((status & 0x0F) != 0x00)
				{
					LIS3DSH_ReadACC(&buffer[0]);
					accX = (float)buffer[0];
					accY = (float)buffer[1];
					accZ = (float)buffer[2];
					printf("X: %3f   Y: %3f   Z: %3f  absX: %d\n", accX, accY, accZ , (int)(buffer[0]));
					HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_13);
				}
 }
