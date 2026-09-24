#include "kalman.h"
#include "kalmanMath.h"
#include <math.h>

void kalman_plain(kalman_state *kstate, float measurement) {
    kstate->p = kstate->p + kstate->q;
    kstate->k = kstate->p / (kstate->p + kstate->r);
    kstate->x = kstate->x + kstate->k * (measurement - kstate->x);
    kstate->p = (1 - kstate->k) * kstate->p;
}

int32_t substract_plain(float* InputArray, float* OutputArray, float* DifferenceArray, int Length) {
    for (int i = 0; i < Length; i++) {
        DifferenceArray[i] = InputArray[i] - OutputArray[i];
    }
    return 0;
}

int32_t compute_stddev_and_avg_plain(float* DifferenceArray, float* average, float* standard_deviation, int Length) {
    float sum = 0.0f;
    for (int i = 0; i < Length; i++) {
        sum += DifferenceArray[i];
    }
    *average = sum / Length;

    float variance_sum = 0.0f;
    for (int i = 0; i < Length; i++) {
        float delta = DifferenceArray[i] - *average;
        variance_sum += delta * delta;
    }
    *standard_deviation = sqrtf(variance_sum / Length);
    return 0;
}

int32_t correlate_plain(float* InputArray, float* OutputArray, float* CorrResult, int Length) {
    int result_length = 2 * Length - 1;
    for (int n = 0; n < result_length; n++) {
        float sum = 0.0f;
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

int32_t convolve_plain(float* InputArray, float* OutputArray, float* ConvResult, int Length) {
    int result_length = 2 * Length - 1;
    for (int n = 0; n < result_length; n++) {
        float sum = 0.0f;
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
