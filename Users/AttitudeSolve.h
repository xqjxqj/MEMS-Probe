/**
 * @file Untitled-1
 * @brief
 * @author weikanggui 
 * @version 1.0
 * @date 2024-06-18
 * @copyright Copyright (c) 2024  CAS
 */
 
#ifndef __AttitudeSolve_H_
#define __AttitudeSolve_H_

void splitFloat(float value, volatile short *intPart, volatile short *decimalPart);
void gyroRotation(float *gyrov,float temp);
void AccCalibrate(float *Ax,float *Ay,float *Az,float temp,short group);
void MagCalibrate(float *Mx,float *My,float *Mz,float temp);

float GravityToolface(float gx,float gy);
float calculateInclination(float gx, float gy, float gz);
float calculateCurrentAngle(float startAngle, float changeAngle);
float calculateAzi(float I,float Be,float Bn,float Bu,float Mz,float DEC,float Azibefore);
//float slidingAverageFilter(float newMeasurement, float *HistorySum, int *index, float oldMeasurement,float result,int flag);
#endif // __AttitudeSolve_H_