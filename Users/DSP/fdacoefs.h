/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 * Generated by MATLAB(R) 9.11 and Signal Processing Toolbox 8.7.
 * Generated on: 27-Apr-2022 15:06:04
 */

/*
 * 离散时间 IIR 滤波器(实数)
 * ----------------
 * 滤波器结构  : 直接 II 型，二阶节
 * 节数     : 2
 * 稳定     : 是
 * 线性相位   : 否
 */

/* General type conversion for MATLAB generated C-code  */
#include "tmwtypes.h"
/* 
 * Expected path to tmwtypes.h 
 * D:\MATLAB\R2021b\extern\include\tmwtypes.h 
 */
#define MWSPT_NSEC 5
const int NL[MWSPT_NSEC] = { 1,3,1,3,1 };
const real64_T RawData_IIR_B[MWSPT_NSEC][3] = {
  {
    0.02598145190428,                 0,                 0 
  },
  {
                   1,                 2,                 1 
  },
  {
    0.02236385339992,                 0,                 0 
  },
  {
                   1,                 2,                 1 
  },
  {
                   1,                 0,                 0 
  }
};
const int DL[MWSPT_NSEC] = { 1,3,1,3,1 };
const real64_T RawData_IIR_A[MWSPT_NSEC][3] = {
  {
                   1,                 0,                 0 
  },
  {
                   1,    -1.66730964199,   0.7712354496069 
  },
  {
                   1,                 0,                 0 
  },
  {
                   1,   -1.435157224589,   0.5246126381883 
  },
  {
                   1,                 0,                 0 
  }
};
