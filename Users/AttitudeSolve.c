 /**
 * @file Untitled-1
 * @brief
 * @author weikanggui
 * @version 1.0
 * @date 2024-06-18
 * @copyright Copyright (c) 2024  CAS
 ****************************************************/
#define DECIMAL_FACTOR 10000   
#define PI 3.14159265358979323846f
#include "AttitudeSolve.h"
#include <stdio.h>  
#include <math.h>
// correction coefficient
// gyro: real gyro = measure gyro/(1+GyroCal1)-GyroCal2;
//volatile float GyroCal1 = 1.0330f;
//volatile float GyroCal2 = 3.094f;
//volatile float Static = 10.1598f;


//Split a float into its integer and fractional parts represented as two short integers
void splitFloat(float value, volatile short *intPart, volatile short *decimalPart) {  
    // integer part
    *intPart = (short)value;      
    // round to 4 decimal places(DECIMAL_FACTOR)  
    *decimalPart = (short)(roundf((value - *intPart) * DECIMAL_FACTOR));  
} 

//Calculating the change of toolface angle based on gyro measurements

void gyroRotation(float *gyrov,float temp){
	
	//gyrov = (gyrov-Static)/(GyroCal1)-GyroCal2;
	
}

void AccCalibrate(float *Ax,float *Ay,float *Az,float temp,short group){
	//Temperature
	float Acc[3]={*Ax,*Ay,*Az};
  float CalAcc[3];
	float Acc_K[3]={1.0,1.0,1.0};
	float Acc_M[3][3] = {{1.0, 2.0, 3.0},{4.0, 5.0, 6.0},{7.0, 8.0, 9.0}};
	if (temp<37.5f){
		float Acc_K[3]={1.0,1.0,1.0};
		float Acc_M[3][3] = {{1.0, 2.0, 3.0},{4.0, 5.0, 6.0},{7.0, 8.0, 9.0}};
	}else if(temp>37.5f&&temp<62.5f){
		float Acc_K[3]={1.0,1.0,1.0};
		float Acc_M[3][3] = {{1.0, 2.0, 3.0},{4.0, 5.0, 6.0},{7.0, 8.0, 9.0}};
	}else if(temp>62.5f&&temp<87.5f){
		float Acc_K[3]={1.0,1.0,1.0};
		float Acc_M[3][3] = {{1.0, 2.0, 3.0},{4.0, 5.0, 6.0},{7.0, 8.0, 9.0}};
	}else if(temp>87.5f&&temp<112.5f){
		float Acc_K[3]={1.0,1.0,1.0};
		float Acc_M[3][3] = {{1.0, 2.0, 3.0},{4.0, 5.0, 6.0},{7.0, 8.0, 9.0}};
	}else if(temp>112.5f){
		float Acc_K[3]={1.0,1.0,1.0};
		float Acc_M[3][3] = {{1.0, 2.0, 3.0},{4.0, 5.0, 6.0},{7.0, 8.0, 9.0}};
	}
	
	for (short i = 0; i < 3; i++) {
        float diff = Acc[i] - Acc_K[i];
        CalAcc[i] = 0.0f;
        for (short j = 0; j < 3; j++) {
            CalAcc[i] += diff * Acc_M[i][j];
        }
    }
	*Ax=CalAcc[0];
	*Ay=CalAcc[1];
  *Az=CalAcc[2];		
	
}

void MagCalibrate(float *Mx,float *My,float *Mz,float temp){
		//Temperature
	float Mag[3]={*Mx,*My,*Mz};
  float CalMag[3];
	float Mag_K[3]={1.0,1.0,1.0};
	float Mag_M[3][3] = {{1.0, 2.0, 3.0},{4.0, 5.0, 6.0},{7.0, 8.0, 9.0}};
	if (temp<37.5f){
	float Mag_K[3]={1.0,1.0,1.0};
	float Mag_M[3][3] = {{1.0, 2.0, 3.0},{4.0, 5.0, 6.0},{7.0, 8.0, 9.0}};
	}else if(temp>37.5f&&temp<62.5f){
	float Mag_K[3]={1.0,1.0,1.0};
	float Mag_M[3][3] = {{1.0, 2.0, 3.0},{4.0, 5.0, 6.0},{7.0, 8.0, 9.0}};
	}else if(temp>62.5f&&temp<87.5f){
	float Mag_K[3]={1.0,1.0,1.0};
	float Mag_M[3][3] = {{1.0, 2.0, 3.0},{4.0, 5.0, 6.0},{7.0, 8.0, 9.0}};
	}else if(temp>87.5f&&temp<112.5f){
	float Mag_K[3]={1.0,1.0,1.0};
	float Mag_M[3][3] = {{1.0, 2.0, 3.0},{4.0, 5.0, 6.0},{7.0, 8.0, 9.0}};
	}else if(temp>112.5f){
	float Mag_K[3]={1.0,1.0,1.0};
	float Mag_M[3][3] = {{1.0, 2.0, 3.0},{4.0, 5.0, 6.0},{7.0, 8.0, 9.0}};
	}
	
	for (short i = 0; i < 3; i++) {
        float diff = Mag[i] - Mag_K[i];
        CalMag[i] = 0.0f;
        for (short j = 0; j < 3; j++) {
            CalMag[i] += diff * Mag_M[i][j];
        }
    }
	*Mx=CalMag[0];
	*My=CalMag[1];
  *Mz=CalMag[2];	
}

//toolface
float GravityToolface(float gx,float gy){
	  float angle; 
	  if(gx>1) gx=1;
	  if(gx<-1) gx=-1;
		if(gy>1) gy=1;
	  if(gy<-1) gy=-1;
    if (gx == 0.0f && gy == 0.0f) {  
        angle = NAN; 
    } else if (gy == 0.0f) {          
        angle = (gx > 0.0f) ? 90.0f : 270.0f;  
    } else {         
        angle = atan2f(gy, gx) * 180.0f / PI; 
        if (angle < 0.0f) {  
            angle += 360.0f;  
        }  
    }  
    return angle; }

//inc
float calculateInclination(float gx, float gy, float gz) {  
   
   // if (gz == 0) gz = 1e-6;  
   // float angleRadians = atan2(sqrt(gx*gx + gy*gy), gz);   
   // float angleDegrees = angleRadians * (180.0 / PI);  
	if(gz>1) gz=1;
	if(gz<-1) gz=-1;
	float angleRadians = acos(gz); 
  float angleDegrees = angleRadians * (180.0 / PI); 
	return angleDegrees;  
} 

// calculate dynamic toolface
float calculateCurrentAngle(float startAngle, float changeAngle) {  
    float currentAngle = fmod(startAngle + changeAngle, 360.0f); 
    if (currentAngle < 0.0f) {
        currentAngle += 360.0f;  
    }  
    return currentAngle;  
} 

float calculateAzi(float I,float Be,float Bn,float Bu,float Mz,float DEC,float Azibefore){
	float MagA;
	if(I==0.0f){
		return Azibefore;
	}else{
	  I=I*PI/180.0f;
		float temp =-(Mz-cos(I)*Bu)/(sin(I)*sqrt(Be*Be+Bn*Bn));
		if(temp>1) temp=1;
		if(temp<-1) temp=-1;
	  MagA=acos(temp)/PI*180.0;
	  return MagA-DEC;
	}
}


