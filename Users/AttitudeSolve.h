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
float gyroRotation(float gyrov,float SampleRate,float GyroCal1,float CyroCal2);
char isDynamic(float gyrov);
float GravityToolface(float gx,float gy);
float calculateInclination(float gx, float gy, float gz);
float calculateCurrentAngle(float startAngle, float changeAngle);
#endif // __AttitudeSolve_H_