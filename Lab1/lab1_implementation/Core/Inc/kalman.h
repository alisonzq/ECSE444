#ifndef KALMAN_H
#define KALMAN_H

#define KALMAN_IMPL_PLAIN 0
#define KALMAN_IMPL_CMSIS 1
#define KALMAN_IMPL_ASM 2

#ifndef KALMAN_IMPLEMENTATION
#define KALMAN_IMPLEMENTATION KALMAN_IMPL_ASM
#endif

typedef struct {
    float q; //process noise covariance
    float r; //measurement noise covariance
    float x; //estimated value
    float p; //estimation error covariance
    float k; // adaptive Kalman filter gain
} kalman_state;

extern void kalman(kalman_state *kstate, float measurement);
extern void kalman_plain(kalman_state *kstate, float measurement);
extern void kalman_cmsis(kalman_state *kstate, float measurement);

#if KALMAN_IMPLEMENTATION == KALMAN_IMPL_PLAIN
#define KALMAN_STEP kalman_plain
#elif KALMAN_IMPLEMENTATION == KALMAN_IMPL_CMSIS
#define KALMAN_STEP kalman_cmsis
#else
#define KALMAN_STEP kalman
#endif

int Kalmanfilter(float* InputArray, float* OutputArray, kalman_state* kstate, int Length);

#endif
