#ifndef _AD_H
#define _AD_H

#include "global.h"
#include "prodata.h"

//����У׼���ݼ�����ص�У׼��
void ADParam_Calculate(void);
//AD Converter
void AD_ReadResult(void);
//����У׼���������ʵ�ʵĲ���ֵ
void AD_Calculation(void);
//�����Դ���ADC��������
void ProcADC(void);
//
float GetO2Val(char Index);

#endif
