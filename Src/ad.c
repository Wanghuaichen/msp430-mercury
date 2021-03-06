#include "ad.h"
#include "global.h"
#include "string.h"
#include "stdio.h"

//根据校准数据计算相关的校准量
void ADParam_Calculate(void)
{
	INT16U i;
	float k4tmp;
	float k20_k4,su_sd;

	for(i = 0; i < 8; i++) {
		if(Ad_Param[i].type == 0) {
			//在这个程序中4mA测出的值比校准的值大2左右
			//Ad_Calib_Param.k4[i] += 2;
			k4tmp = (float)Ad_CalibParam.k4[i];
			k20_k4 =(float)(Ad_CalibParam.k20[i] - k4tmp);
			su_sd = Ad_Param[i].highval - Ad_Param[i].lowval;
			Ad_Param[i].rate = su_sd / k20_k4;
			Ad_Param[i].offset =((5 * k4tmp - Ad_CalibParam.k20[i]) * su_sd) /
					(4 * k20_k4) - (5 * Ad_Param[i].lowval - Ad_Param[i].highval) / 4;
		} else {
			Ad_Param[i].rate = (Ad_Param[i].highval - Ad_Param[i].lowval) / 0x1000;
		}
	}
}

/*******************************************
              AD Converter
*******************************************/
void AD_ReadResult(void)
{
    INT8U i,j;
    INT16U *max,*min,*mid;
    
    for(i=0;i<8;i++)
    {
        Ad_Value[i]=0;
        max=&Ad_Result[i][0];
        min=max;
        mid=&Ad_Result[i][1];
        for(j=0;j<9;j++)           //查找最高最低值
        {
            if(*max<*mid)
                max=mid;
            if(*min>*mid)
                min=mid;
            mid=&Ad_Result[i][j+2];
        }
        if(max==min)
            min+=1;
        *max=0;      //去掉最高值
        *min=0;      //去掉最低值
        
        for(j=0;j<10;j++)
            Ad_Value[i]+=Ad_Result[i][j];
        Ad_Value[i]=(Ad_Value[i]>>3);   //取平均值
    }
}

//根据校准参数计算出实际的采样值
void AD_Calculation(void)
{
    INT8U i;
    INT16U AD_f;
    float AD_k;
    
    for(i=0;i<8;i++)
    {
        AD_f=Ad_Value[i];
        switch(Ad_Param[i].type)
        {
        case 0:
            if(AD_f<Ad_CalibParam.k4[i])
              AD_f =Ad_CalibParam.k4[i];
            AD_k =Ad_Param[i].rate*AD_f-Ad_Param[i].offset;
            break;
        case 1:
            AD_k=Ad_Param[i].rate*AD_f+Ad_Param[i].lowval;	
            break;
        case 2:
            AD_k=Ad_Param[i].rate*AD_f*2+Ad_Param[i].lowval;
            break;
        default:
            break;
        }
        
        if(AD_k>Ad_Param[i].highval)
          Ad_RealData[i]=Ad_Param[i].highval;
        else
          Ad_RealData[i]=AD_k;
    }
}

//周期性处理ADC采样数据
void ProcADC(void)
{
	if(Ad_ConvertEnd) {
		Ad_ConvertEnd = 0;
		AD_ReadResult();		    //读AD转换数据
		AD_Calculation();           //计算AD转换值
		ADC12CTL0 |= ADC12SC + ENC;     //重新开启AD转换
    }  
}

///////////////////////////////////////////
float GetO2Val(char Index)
{
	char i;
	char str[8];
	
	sprintf(str, "O2%01d", Index+1);
	for(i=0;i<8;++i)
	{
		if(memcmp(Ad_Param[i].id,str,3) == 0)
		{
			return Ad_RealData[i]; 
		}
	}
	
	return 0;
}
