#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"

int IIR_C(float* InputArray, float* OutputArray, float* coeff, int Length, int Order);
extern int IIR_asm(float* inputArray, float* OutputArray, int Length, float* coeff);


int main() {
    float inputArray[5] = {1,2,3,4,5};
		int length = 5;
		int order = 2;
		float outputArray[5];
    float coeffArray[5] = {0.1,0.15,0.5,0.15,0.1};

    //Call the C filter
    IIR_C(inputArray, outputArray, coeffArray, length, order);
		printf("C Output:\n");
		int i;
		for (i = 0; i < length; i++) {
			printf("%f ", outputArray[i]);
		}

    //Call the assembly ARM filter
		printf("\nArm Output:\n");
    IIR_asm(inputArray, outputArray,length, coeffArray);
		for (i = 0; i < length; i++) {
			printf("%f ", outputArray[i]);
		}

		//Call the built in filter
		printf("\nCMSIS Output:\n");
		float initial_state[4] = {0};
		arm_biquad_casd_df1_inst_f32 S1 = {1, initial_state, coeffArray};
		arm_biquad_cascade_df1_f32(&S1, inputArray, outputArray, 5);
		for (i = 0; i < length; i++) {
			printf("%f ", outputArray[i]);
		}

		return 0;
}


int IIR_C(float* InputArray, float* OutputArray, float* coeff, int Length, int Order) {
	int n, k;
	for (n = 0; n < Length; n++) {
        //Find min to calculate values of the output when the previous inputs/outputs aren't defined
        int min = Order > n ? n: Order;
        //Initialize output array
        OutputArray[n] = 0;
        for (k = 0 ; k < min + 1; k++) {
            //Add previous input value to current output
            OutputArray[n] += coeff[k] * InputArray[n - k];
            //This check ensures we only add the previos output values if they are available
            if (k > 0 && min > 0) {
              //Add previous output value to current output
              OutputArray[n] += coeff[k + Order] * OutputArray[n - k];
            }
        }
    }
	return 0;
}
