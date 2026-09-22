#define ARM_MATH_CM4
#include "arm_math.h"
#include "lab1math.h"

//library overhead isn't free when you don't actually have data to vectorize
//Here using a vector-processing function to do a single scalar addition

//kalman filter implementation 
void kalman_cmsis(kalman_state *kstate, float measurement) {
    arm_add_f32(&kstate->p, &kstate->q, &kstate->p, 1); // p = p + q (block_size of 1, one element to process)

    float p_plus_r;
    arm_add_f32(&kstate->p, &kstate->r, &p_plus_r, 1); // p + r

    //CMSIS has no arm_div_f32 for f32 scalars
    kstate->k = kstate->p / p_plus_r; // k = p / (p+r)

    float diff, correction;
    arm_sub_f32(&measurement, &kstate->x, &diff, 1); // measurement - x
    arm_mult_f32(&kstate->k, &diff, &correction, 1); // k * (measurement - x)
    arm_add_f32(&kstate->x, &correction, &kstate->x, 1); // x = x + correction

    float one = 1.0f, one_minus_k;
    arm_sub_f32(&one, &kstate->k, &one_minus_k, 1); // 1 - k
    arm_mult_f32(&one_minus_k, &kstate->p, &kstate->p, 1); // p = (1-k)*p
}

//Subtraction of original and data obtained by Kalman filter tracking.
int32_t substract(float* InputArray, float* OutputArray, float* DifferenceArray, int Length) {
    arm_sub_f32(InputArray, OutputArray, DifferenceArray, Length);
    return 0;
}

//Calculation of the standard deviation and the average of the difference obtained in a).
int32_t compute_stddev_and_avg(float* DifferenceArray, float* average, float* standard_deviation, int Length) {
    arm_mean_f32(DifferenceArray, Length, average);
    arm_std_f32(DifferenceArray, Length, standard_deviation);
    return 0;
}

//Calculation of the correlation between the original and tracked vectors. 
int32_t correlate(float* InputArray, float* OutputArray, float* CorrResult, int Length) {
    arm_correlate_f32(InputArray, Length, OutputArray, Length, CorrResult);
    return 0;
}

//Calculation of the convolution between the two vectors.
int32_t convolve(float* InputArray, float* OutputArray, float* ConvResult, int Length) {
    arm_conv_f32(InputArray, Length, OutputArray, Length, ConvResult);
    return 0;
}
