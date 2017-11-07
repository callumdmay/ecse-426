#ifndef ACCELEROMETER
#define ACCELEROMETER

void initializeACC(void);
void ITInit(void);
void getACC(float *arr);
void conversion(float acc[3], float out[2]);
void comparison(float actual[2], int input[2], int diff[2]);
#endif
