#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"

int IIR_C(float* InputArray, float* OutputArray, float* coeff, int Length, int Order);
	
int main() {
    float inputArray[5] = {1,2,3,4,5};
    float outputArray[5];
    int length = 5;

    int order = 2;
    float coeffArray[5] = {0.1,0.15,0.5,0.15,0.1};

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
				printf("%f\n", OutputArray[n]);
    }
	return 0;
}
