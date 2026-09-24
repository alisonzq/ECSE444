#ifndef KALMANMATH_H
#define KALMANMATH_H

#include <stdint.h>

#define KALMAN_MATH_IMPL_PLAIN 0
#define KALMAN_MATH_IMPL_CMSIS 1

#ifndef KALMAN_MATH_IMPLEMENTATION
#define KALMAN_MATH_IMPLEMENTATION KALMAN_MATH_IMPL_CMSIS
#endif

int32_t substract_plain(float* InputArray, float* OutputArray, float* DifferenceArray, int Length);
int32_t compute_stddev_and_avg_plain(float* DifferenceArray, float* average, float* standard_deviation, int Length);
int32_t correlate_plain(float* InputArray, float* OutputArray, float* CorrResult, int Length);
int32_t convolve_plain(float* InputArray, float* OutputArray, float* ConvResult, int Length);

int32_t substract_cmsis(float* InputArray, float* OutputArray, float* DifferenceArray, int Length);
int32_t compute_stddev_and_avg_cmsis(float* DifferenceArray, float* average, float* standard_deviation, int Length);
int32_t correlate_cmsis(float* InputArray, float* OutputArray, float* CorrResult, int Length);
int32_t convolve_cmsis(float* InputArray, float* OutputArray, float* ConvResult, int Length);

#if KALMAN_MATH_IMPLEMENTATION == KALMAN_MATH_IMPL_PLAIN
#define substract substract_plain
#define compute_stddev_and_avg compute_stddev_and_avg_plain
#define correlate correlate_plain
#define convolve convolve_plain
#else
#define substract substract_cmsis
#define compute_stddev_and_avg compute_stddev_and_avg_cmsis
#define correlate correlate_cmsis
#define convolve convolve_cmsis
#endif

#endif