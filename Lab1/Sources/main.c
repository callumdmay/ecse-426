#include <stdio.h>
#include <stdlib.h>
//#include "arm_math.h"

int IIR_C(float* InputArray, float* OutputArray, float* coeff, int Length, int Order);
	
int main() {
    float inputArray[6] = {1,2,3,4,5,6};
    float outputArray[6];
    int length = 6;

    int order = 3;
    float coeffArray[5] = {0.1,0.15,0.5,0.15,0.1, 0.1, 0.15};

    IIR_C(inputArray, outputArray, coeffArray, length, order);

	return 0;
}


int IIR_C(float* InputArray, float* OutputArray, float* coeff, int Length, int Order) {
	int n, k;
	for (n = 0; n < Length; n++) {
        int min = Order > n ? n: Order;
        OutputArray[n] = 0;
        for (k = 0 ; k < min + 1; k++) {
            OutputArray[n] += coeff[k] * InputArray[n - k];

            if (k > 0 && min > 0) {
                OutputArray[n] += coeff[k + Order] * OutputArray[n - k];
            }
        }
    }
	return 0;
}