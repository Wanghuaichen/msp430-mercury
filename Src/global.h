#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "type.h"
#include "record.h"

extern const char VersionCode[6];	//程序版本号
extern INT16U SoftWDT_Cnt;			//软狗计数器
extern INT8U SoftWDT_Flag;
extern INT8U LEDTimer;
extern INT8U RTCTimer;
extern INT16U RTCErrTimer;
extern INT8U KeyTimer;						//键盘计数器
extern INT8U System_TimeStr[16];
extern INT8U System_StartTimeStr[16];
extern T_Times Current_Tm;
extern INT8U DisplayTimer;         //LCD定时刷新计数器
extern INT16U BackLightTimer;      //LCD背光时间计数器

extern INT32U DI_Val;		//除了主板自带的16路DI，再外扩16路
extern INT16U DO_Val;		//除了主板自带的8路DO，再外扩8路

/***************************************
		212参数
 **************************************/
extern unsigned int Hjt212RtdTimeCnt;	    //实时数据发送间隔计数器
extern unsigned char Hjt212TimeOffset;		//分钟数据的发送时间偏移
extern HJT212CMD Hjt212Cmd[HJT_STACK_NUM];	//212接收到的命令信息
extern HJT212SIGNAL Hjt212Signal[HJT_STACK_NUM];	//212上传数据标志
extern const int HJTUartChannel;					//dtu串口号

/***************************************
		系统参数
**************************************/
extern SYS_PARAM Sys_Param;

/***************************************
		模拟量部分
**************************************/
extern INT8U Ad_ConvertEnd;
extern INT16U Ad_Value[8];
extern INT16U Ad_Result[8][10];
extern AD_CALIBPARAM Ad_CalibParam;
extern AD_PARAM Ad_Param[8];
extern float Ad_RealData[8];
extern MAX_MIN_DATA Ad_MaxMin[8];

/**************************************
		串口量部分
**************************************/
extern MBIO_PARAM MbIO_Param;
extern char MbIO_Err[MODBUSIO_VARNUM];
extern INT8U MbIOOtCnt;			//modbus超时计数器
extern INT8U MbIODelayCnt;

extern COM_PARAM Com_Param;
extern const char ComBaud_Str[4][6];
extern float Com_Val[MODBUS_VARNUM];
extern char Com_Err[MODBUS_VARNUM];		//串口量通道错误标识
extern MAX_MIN_DATA Com_MaxMin[MODBUS_VARNUM];		//串口量通道参数统计数据

extern INT8U MbOtCnt;		//Modbus超时计数器
extern INT16U MbScanCnt;	//Modbus扫描周期计数器

/**************************************
		串口部分
**************************************/
extern COM_BUF Com_Buf[5];

/**************************************
		项目相关部分
 **************************************/
extern ALARM_PARAM Alarm_Param[MAX_ALARM_NUM];
extern INT8U BeepEnable;

/**************************************
		公共函数部分
**************************************/
void Delay_N_mS(unsigned int n_milisecond);
void Delay_10_uS(void);

#endif /* End Of _GLOBAL_H_ */
