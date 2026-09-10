#define ARM_MATH_CM4
#include "arm_math.h"
#include "lab1math.h"

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
    arm_convolve_f32(InputArray, Length, OutputArray, Length, ConvResult);
    return 0;
}
