#include "kalman.h"
#include <math.h>

int Kalmanfilter(float* InputArray, float* OutputArray, kalman_state* kstate, int Length) {
    for (int i = 0; i < Length; i++) {
        kalman(kstate, InputArray[i]);
        OutputArray[i] = kstate->x;

        if(isnan(kstate->x)) {
            return 1;
        }
    }

    return 0;
}

int kalman_c(kalman_state *kstate, float measurement) {
    kstate->p = kstate->p + kstate->q;
    kstate->k = kstate->p / (kstate->p + kstate->r);
    kstate->x = kstate->x + kstate->k * (measurement - kstate->x);
    kstate->p = (1 - kstate->k) * kstate->p;
    return 0;
}

//Subtraction of original and data obtained by Kalman filter tracking.
int substract(float* InputArray, float* OutputArray, float* DifferenceArray, int Length) {
    for (int i = 0; i < Length; i++) {
        DifferenceArray[i] = InputArray[i] - OutputArray[i];
    }
    return 0;
}

//Calculation of the standard deviation and the average of the difference obtained in a).
int compute_stddev_and_avg(float* DifferenceArray, float* average, float* standard_deviation, int Length) {
    float sum = 0;
    for (int i = 0; i < Length; i++) {
        sum += DifferenceArray[i];
    }
    *average = sum / Length;

    float variance_sum = 0;
    for (int i = 0; i < Length; i++) {
        variance_sum += (DifferenceArray[i] - *average) * (DifferenceArray[i] - *average);
    }
    *standard_deviation = sqrt(variance_sum / Length);
    return 0;
}

//Calculation of the correlation between the original and tracked vectors. 
int correlate(float* InputArray, float* OutputArray, float* CorrResult, int Length) {
    int result_length = 2 * Length - 1;
    for (int n = 0; n < result_length; n++) {
        float sum = 0;
        int shift = n - (Length - 1);
        for (int i = 0; i < Length; i++) {
            int j = i + shift;
            if (j >= 0 && j < Length) {
                sum += InputArray[i] * OutputArray[j];
            }
        }
        CorrResult[n] = sum;
    }
    return 0;
}

//Calculation of the convolution between the two vectors.
int convolve(float* InputArray, float* OutputArray, float* ConvResult, int Length) {
    int result_length = 2 * Length - 1;
    for (int n = 0; n < result_length; n++) {
        float sum = 0;
        for (int i = 0; i < Length; i++) {
            int j = n - i;
            if (j >= 0 && j < Length) {
                sum += InputArray[i] * OutputArray[j];
            }
        }
        ConvResult[n] = sum;
    }
    return 0;
}
