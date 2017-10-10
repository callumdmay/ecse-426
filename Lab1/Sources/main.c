#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"

int IIR_C(float* InputArray, float* OutputArray, float* coeff, int Length, int Order);
extern int IIR_asm(float* inputArray, float* OutputArray, int Lenfth, float_ coeff)


int main() {
    float inputArray[5] = {1,2,3,4,5};
		int length = 5;
		int order = 2;
		float outputArray[5];

    float coeffArray[5] = {0.1,0.15,0.5,0.15,0.1};

    IIR_C(inputArray, outputArray, coeffArray, length, order);
		int i;
		for (i = 0; i < length; i++) {
			printf("%f ", outputArray[i]);
		}
		printf("\n Arm Output:\n");
    IIR_asm(inputArray, outputArray, coeffArray, length, order);
		for (i = 0; i < length; i++) {
			printf("%f ", outputArray[i]);
		}

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
