#include <stdio.h>
#include "arm_math.h"

int IIR_C(float* InputArray, float* OutputArray, float* coeff, int Length, int Order);
	
int main()
{
float inputArray[10] = {1,2,3,4,5,6,7,8,9,10};

float coeffArray[5] = {0.1,0.15,0.5,0.15,0.1};
float outputArray[5];
IIR_C(inputArray, outputArray, coeffArray, 5, 2);

	return 0;
}


int IIR_C(float* InputArray, float* OutputArray, float* coeff, int Length, int Order) {
	int n, k;
	for (n = 0; n < Length ; n++) {
		OutputArray[n] = coeff[0] * InputArray[n];
		if (n >= 1) {
			OutputArray[n] += coeff[1] * InputArray[n - 1] + coeff[3] * OutputArray[n - 1];
		}
		
		if (n >=2) {
			OutputArray[n] += coeff[2] * InputArray[n - 2] + coeff[4] * OutputArray[n - 2] ;
		}
	}
	return 0;
}