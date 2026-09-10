#ifndef KALMAN_H
#define KALMAN_H

typedef struct {
    float q; //process noise covariance
    float r; //measurement noise covariance
    float x; //estimated value
    float p; //estimation error covariance
    float k; // adaptive Kalman filter gain
} kalman_state;

void kalman(kalman_state *state, float measurement);

#endif

int kalman_c(kalman_state *state, float measurement) {
    
}
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

//Subtraction of original and data obtained by Kalman filter tracking.
int substract() {

}

//Calculation of the standard deviation and the average of the difference obtained in a).
int calculate_standard_deviation_and_average() {

}

//Calculation of the correlation between the original and tracked vectors. 
int calculate_correlation() {

}

//Calculation of the convolution between the two vectors.
int calculate_convolution() {

}
