#ifndef KALMANCMSIS_H
#define KALMANCMSIS_H

#include <stdint.h>
#include "kalman.h"

void kalman_cmsis(kalman_state *kstate, float measurement);
int32_t substract_cmsis(float* InputArray, float* OutputArray, float* DifferenceArray, int Length);
int32_t compute_stddev_and_avg_cmsis(float* DifferenceArray, float* average, float* standard_deviation, int Length);
int32_t correlate_cmsis(float* InputArray, float* OutputArray, float* CorrResult, int Length);
int32_t convolve_cmsis(float* InputArray, float* OutputArray, float* ConvResult, int Length);

#endif
