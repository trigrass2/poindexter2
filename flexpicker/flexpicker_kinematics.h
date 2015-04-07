#ifndef __FLEXPICKER_KINEMATICS_H__
#define __FLEXPICKER_KINEMATICS_H__

// Transparency time: I stole this from the original control code.
// Kinematics looks awful, I ain't doing it myself :P

#define KIN_L 354.5
#define KIN_M 800.0
#define KIN_R1 200.0
#define KIN_R2 52.0
#define RADTODEG 57.2957795
#define KIN_LOWER (-40.0 / RADTODEG)
#define KIN_UPPER (100.0 / RADTODEG)

const double phi[3] = { 0, 120.0/RADTODEG, 240.0/RADTODEG };

void forwardkin(double* theta, double* xyz);
void inversekin(double* xyz, double* theta);

#endif
