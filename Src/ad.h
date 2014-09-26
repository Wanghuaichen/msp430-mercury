#ifndef _AD_H
#define _AD_H

#include "global.h"
#include "prodata.h"

//根据校准数据计算相关的校准量
void ADParam_Calculate(void);
//AD Converter
void AD_ReadResult(void);
//根据校准参数计算出实际的采样值
void AD_Calculation(void);
//周期性处理ADC采样数据
void ProcADC(void);
//
float GetO2Val(char Index);

#endif
