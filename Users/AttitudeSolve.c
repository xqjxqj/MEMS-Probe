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



//Split a float into its integer and fractional parts represented as two short integers
void splitFloat(float value, volatile short *intPart, volatile short *decimalPart) {  
    // integer part
    *intPart = (short)value;      
    // round to 4 decimal places(DECIMAL_FACTOR)  
    *decimalPart = (short)(roundf((value - *intPart) * DECIMAL_FACTOR));  
} 

//Calculating the change of toolface angle based on gyro measurements
float gyroRotation(float gyrov,float Static,float GyroCal1,float GyroCal2){
	//return (gyrov/(1+GyroCal1)-GyroCal2)/SampleRate;
	return (gyrov-Static)/(GyroCal1)-GyroCal2;
}

//Determining if the drilling tool is rotating. 0=static 1=dynamic
char isDynamic(float gyrov){
	char flag;
	if(gyrov>20||gyrov<-20){
		flag='1';}else{
			flag='0';
		}
		return flag;
}

//toolface
float GravityToolface(float gx,float gy){
	float angle;  
    if (gx == 0.0f && gy == 0.0f) {  
        angle = NAN; 
    } else if (gy == 0.0f) {  
        
        angle = (gx > 0.0f) ? 90.0f : -90.0f;  
    } else {  
        
        angle = atan2f(gx, gy) * 180.0f / PI; 
        if (angle < 0.0f) {  
            angle += 360.0f;  
        }  
    }  
    return angle; }

//inc
float calculateInclination(float gx, float gy, float gz) {  
   
    if (gz == 0) gz = 1e-6;  
    float angleRadians = atan2(sqrt(gx*gx + gy*gy), gz);   
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
