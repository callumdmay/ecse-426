#include "lis3dsh.h"
#include "stm32f4xx_hal.h"
#include "keypad.h"
#include "filter.h"
#include "accelerometer.h"
#include <math.h>

//Prototypes
void Thread_acc (void const *argument);
void initAccInterrupt(void);
void getACC(float *arr);
void conversion(float acc[3], float out[2]);
void getNormalizedAcc(float acc[3], float output[3]);

int comparedValues[2];
float axis_angles[2];
float raw_acc_data_buffer[3];
osMutexId acc_mutex;
osMutexDef(acc_mutex);

LIS3DSH_InitTypeDef Acc_instance;
LIS3DSH_DRYInterruptConfigTypeDef AccIT;
GPIO_InitTypeDef GPIO_InitStruct;
SPI_HandleTypeDef hspi;

//Matrix of accelerometer calibration values
double ACC_CALIBRATION_MATRIX[4][3] = {
														{0.000975570176,-0.0000179726958, 0.0000118904418},
														{-0.00000826743781, 0.00100064254, -0.00000108576944},
														{0.00000101638125, 0.00000478541733, 0.000963788305},
														{-0.00438112020, -0.000916585326, 0.000590100884}};

osThreadId tid_Thread_acc;                              // thread id
osThreadDef(Thread_acc, osPriorityNormal, 1, 0);

//Start thread for ACC
void start_thread_acc (void) {
  if (tid_Thread_acc == NULL) {
    tid_Thread_acc = osThreadCreate(osThread(Thread_acc), NULL);
  }
}

//accelerometer thread entry point function
void Thread_acc (void const *argument) {
	float filteredValues[3];
	float normalizedValues[3];

  while(1) {
		osSignalWait(0x01, osWaitForever);
    getACC(raw_acc_data_buffer);
    //filter values using biquad filter
		filterValues(raw_acc_data_buffer, filteredValues);
    //Normalize the values
    getNormalizedAcc(filteredValues, normalizedValues);
    //Perform conversion between normalized cartesian coordinates and axis angles pitch and roll
    osMutexWait(acc_mutex, osWaitForever);
		conversion(normalizedValues, axis_angles);
    osMutexRelease(acc_mutex);
  }
}

void ACC_PowerDown(void)
{
	 uint8_t regbits;

  //read current register
  LIS3DSH_Read(&regbits, LIS3DSH_CTRL_REG4, 1);

  //set desired register values (ODR bits to 0000)
  regbits &= (uint8_t)0x0F;

  //write to register
  LIS3DSH_Write(&regbits, LIS3DSH_CTRL_REG4, 1);
}

void ACC_PowerUp(void)
{
	 uint8_t regbits;

  //read current register
  LIS3DSH_Read(&regbits, LIS3DSH_CTRL_REG4, 1);

  //set desired register values (ODR bits to 0101)
  regbits |= (uint8_t)0x50;

  //write to register
  LIS3DSH_Write(&regbits, LIS3DSH_CTRL_REG4, 1);
}

void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  osSignalSet(tid_Thread_acc, 0x01);
}

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
void initAccInterrupt(void){
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
void getACC(float *arr) {
  uint8_t status;
  LIS3DSH_Read (&status, LIS3DSH_STATUS, 1);
  //The first four bits denote if we have new data on all XYZ axes,
  //Z axis only, Y axis only or Z axis only. If any or all changed, proceed
  if ((status & 0x0F) != 0x00) {
    float buffer[3];
    LIS3DSH_ReadACC(&buffer[0]);

		arr[0] = (float)buffer[0]; //X
    arr[1] = (float)buffer[1]; //Y
    arr[2] = (float)buffer[2]; //Z
  }
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
