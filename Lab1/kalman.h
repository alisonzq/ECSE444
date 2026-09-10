#ifndef KALMAN_H
#define KALMAN_H

typedef struct {
    float q; //process noise covariance
    float r; //measurement noise covariance
    float x; //estimated value
    float p; //estimation error covariance
    float k; // adaptive Kalman filter gain
} kalman_state;

void kalman(kalman_state *kstate, float measurement);

int Kalmanfilter(float* InputArray, float* OutputArray, kalman_state* kstate, int Length);

#endif
