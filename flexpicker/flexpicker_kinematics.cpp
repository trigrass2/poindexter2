#include "flexpicker_kinematics.h"

#include <cmath>
#include <iostream>

void forwardkin(double* theta, double* xyz)
{
	/* Computes unique workspace feasible solution to the direct kinematics for the Delta-manipulator
       Input: array theta of three elements, joint angles in radians
       Output: array of coordinates for the end-effector relative to the base */

    int i,j;
    double tmp;

    double b_B[3];
    double t_C[3];

    double p[3][3];
    double n[3];

    for (i=0;i<3;i++) {

        b_B[0]=KIN_R1+KIN_L*cos(theta[i]);
        b_B[1]=0.0;
        b_B[2]=-KIN_L*sin(theta[i]);
        tmp=b_B[0];
        b_B[0]=cos(phi[i])*b_B[0]+sin(phi[i])*b_B[1];
        b_B[1]=-sin(phi[i])*tmp+cos(phi[i])*b_B[1];

        t_C[0]=cos(phi[i])*KIN_R2;
        t_C[1]=-sin(phi[i])*KIN_R2;
        t_C[2]=0.0;

        for (j=0;j<3;j++) 
            p[i][j]=b_B[j]-t_C[j];

        n[i]=p[i][0]*p[i][0]+p[i][1]*p[i][1]+p[i][2]*p[i][2]-KIN_M*KIN_M;
    }

    double n1=n[0]-n[1];
    double n2=n[1]-n[2];
    double p11=2.0*(p[0][0]-p[1][0]);
    double p12=2.0*(p[0][1]-p[1][1]);
    double p13=2.0*(p[0][2]-p[1][2]);
    double p21=2.0*(p[1][0]-p[2][0]);
    double p22=2.0*(p[1][1]-p[2][1]);
    double p23=2.0*(p[1][2]-p[2][2]);

    double l[3];
    double m[3];

    l[0]=-(p12*n2-n1*p22)/(p11*p22-p21*p12);
    l[1]=(p11*n2-p21*n1)/(p11*p22-p21*p12);
    l[2]=0.0;
    m[0]=(p12*p23-p13*p22)/(p11*p22-p21*p12);
    m[1]=-(-p21*p13+p11*p23)/(p11*p22-p21*p12);
    m[2]=1.0;

    double beta=n[0]-2.0*(p[0][0]*l[0]+p[0][1]*l[1]+p[0][2]*l[2]);
    double a0=m[0]*m[0]+m[1]*m[1]+m[2]*m[2];
    double a1=2.0*(l[0]*m[0]+l[1]*m[1]+l[2]*m[2])-2.0*(p[0][0]*m[0]+p[0][1]*m[1]+p[0][2]*m[2]);   
    double a2=beta+l[0]*l[0]+l[1]*l[1]+l[2]*l[2];

    double alfa1=1.0/2.0/a0*(-a1+sqrt(a1*a1-4.0*a0*a2));
    double alfa2=1.0/2.0/a0*(-a1-sqrt(a1*a1-4.0*a0*a2));
    double alfa=0.0;

    if ((l[2]+alfa1*m[2]<0) & (l[2]+alfa2*m[2]>=0)) 
        alfa=alfa1;
    else if ((l[2]+alfa2*m[2]<0) & (l[2]+alfa1*m[2]>=0)) 
        alfa=alfa2;
    else if ((l[2]+alfa2*m[2]<0) & (l[2]+alfa1*m[2]<0)) {
        alfa=alfa1;
        std::cerr << "Warning: two workspace feasible direct kinematic solutions found." << std::endl;
    } else {
        std::cerr << "Warning: no workspace feasible direct kinematics solution found." << std::endl;
        alfa=0.0;
        l[0]=0.0; l[1]=0.0; l[2]=0.0;
    }

    xyz[0]=l[0]+alfa*m[0];
    xyz[1]=l[1]+alfa*m[1];
    xyz[2]=l[2]+alfa*m[2];
}

void inversekin(double* xyz, double* theta)
{
	/* Computes the unique workspace feasible solution to the inverse kinematics for the Delta-manipulator
       Input: array xyz of three elements, coordinates for the end-effector relative to the base
       Output: array of joint angles in radians */

    int i;

    double a_Ot[3];
    double theta1[3];
    double theta2[3];
    double Xstar=0.0;
    double Q=0.0;

    //double[] theta = new double[3];

    for (i=0;i<3;i++) {
        a_Ot[0]=cos(phi[i])*xyz[0]-sin(phi[i])*xyz[1];
        a_Ot[1]=sin(phi[i])*xyz[0]+cos(phi[i])*xyz[1];
        a_Ot[2]=xyz[2];

        Xstar=a_Ot[0]+KIN_R2-KIN_R1;
        Q=(Xstar*Xstar+KIN_L*KIN_L+a_Ot[1]*a_Ot[1]+a_Ot[2]*a_Ot[2]-KIN_M*KIN_M)/(2.0*KIN_L);

        theta1[i]=atan2(-(-0.5*Xstar/(Xstar*Xstar+a_Ot[2]*a_Ot[2])*(2.0*Q*Xstar+2.0*sqrt(Xstar*Xstar*a_Ot[2]*a_Ot[2]+a_Ot[2]*a_Ot[2]*a_Ot[2]*a_Ot[2]-a_Ot[2]*a_Ot[2]*Q*Q))+Q)/a_Ot[2],0.5/(Xstar*Xstar+a_Ot[2]*a_Ot[2])*(2.0*Q*Xstar+2.0*sqrt(Xstar*Xstar*a_Ot[2]*a_Ot[2]+a_Ot[2]*a_Ot[2]*a_Ot[2]*a_Ot[2]-a_Ot[2]*a_Ot[2]*Q*Q)));
        theta2[i]=atan2(-(-0.5*Xstar/(Xstar*Xstar+a_Ot[2]*a_Ot[2])*(2.0*Q*Xstar-2.0*sqrt(Xstar*Xstar*a_Ot[2]*a_Ot[2]+a_Ot[2]*a_Ot[2]*a_Ot[2]*a_Ot[2]-a_Ot[2]*a_Ot[2]*Q*Q))+Q)/a_Ot[2],0.5/(Xstar*Xstar+a_Ot[2]*a_Ot[2])*(2.0*Q*Xstar-2.0*sqrt(Xstar*Xstar*a_Ot[2]*a_Ot[2]+a_Ot[2]*a_Ot[2]*a_Ot[2]*a_Ot[2]-a_Ot[2]*a_Ot[2]*Q*Q)));

        if ((theta1[i]>=KIN_LOWER) & (theta1[i]<=KIN_UPPER) & !((theta2[i]<KIN_LOWER) & (theta2[i]>KIN_UPPER))) 
            theta[i]=theta1[i];
        else if ((theta2[i]>=KIN_LOWER) & (theta2[i]<=KIN_UPPER) & !((theta1[i]<KIN_LOWER) & (theta1[i]>KIN_UPPER))) 
            theta[i]=theta2[i];
        else if (((theta1[i]>=KIN_LOWER) & (theta1[i]<=KIN_UPPER)) & ((theta2[i]<KIN_LOWER) & (theta2[i]>KIN_UPPER))) {
            theta[i]=theta1[i];
            std::cerr << "Warning: two workspace feasible inverse kinematic solutions found." << std::endl;
        } else {
            theta[i]=0.0f;
            std::cerr << "Warning: no workspace feasible inverse kinematic solution found." << std::endl;
        }
    }
}