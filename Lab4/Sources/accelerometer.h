#ifndef ACCELEROMETER
#define ACCELEROMETER

void start_thread_acc (void);
void initializeACC(void);
void ITInit(void);

extern int comparedValues[2];
extern float convertedValues[2];
extern osMutexId conversion_mutex;
extern osMutexId comparison_mutex;
#endif
