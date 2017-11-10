#ifndef FILTER
#define FILTER

float IIR(float* InputArray, float* coeff, int Length, int Order);
void filterValues(float rawValues[3], float filteredValues[3] );


#endif
