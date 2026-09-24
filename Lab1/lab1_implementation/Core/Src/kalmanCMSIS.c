#define ARM_MATH_CM4
#include "arm_math.h"
#include "kalmanCMSIS.h"

void kalman_cmsis(kalman_state *kstate, float measurement) {
    arm_add_f32(&kstate->p, &kstate->q, &kstate->p, 1);

    float p_plus_r;
    arm_add_f32(&kstate->p, &kstate->r, &p_plus_r, 1);

    kstate->k = kstate->p / p_plus_r;

    float diff, correction;
    arm_sub_f32(&measurement, &kstate->x, &diff, 1);
    arm_mult_f32(&kstate->k, &diff, &correction, 1);
    arm_add_f32(&kstate->x, &correction, &kstate->x, 1);

    float one = 1.0f, one_minus_k;
    arm_sub_f32(&one, &kstate->k, &one_minus_k, 1);
    arm_mult_f32(&one_minus_k, &kstate->p, &kstate->p, 1);
}

int32_t substract_cmsis(float* InputArray, float* OutputArray, float* DifferenceArray, int Length) {
    arm_sub_f32(InputArray, OutputArray, DifferenceArray, Length);
    return 0;
}

int32_t compute_stddev_and_avg_cmsis(float* DifferenceArray, float* average, float* standard_deviation, int Length) {
    arm_mean_f32(DifferenceArray, Length, average);
    arm_std_f32(DifferenceArray, Length, standard_deviation);
    return 0;
}

int32_t correlate_cmsis(float* InputArray, float* OutputArray, float* CorrResult, int Length) {
    arm_correlate_f32(InputArray, Length, OutputArray, Length, CorrResult);
    return 0;
}

int32_t convolve_cmsis(float* InputArray, float* OutputArray, float* ConvResult, int Length) {
    arm_conv_f32(InputArray, Length, OutputArray, Length, ConvResult);
    return 0;
}
