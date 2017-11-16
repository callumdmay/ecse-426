#ifndef ACCELEROMETER
#define ACCELEROMETER
#include "cmsis_os.h"

void start_thread_acc (void);
void initAccInterrupt(void);
void initializeACC(void);
void ACC_PowerUp (void);
void ACC_PowerDown (void);

extern float axis_angles[2];
extern osMutexId acc_mutex;
#endif
