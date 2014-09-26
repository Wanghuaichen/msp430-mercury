#include "type.h"
#include "record.h"

/***************************************
 系统部分
 **************************************/
const char VersionCode[6] = "VT05\0";	//程序版本号
INT16U SoftWDT_Cnt = 0;				//软狗计数器
INT8U SoftWDT_Flag = 0;				//软狗标识
INT8U LEDTimer;						//主板LED运行指示灯计数器
INT8U RTCTimer;						//实时时钟计数器,用于定期读取实时时钟
INT16U RTCErrTimer;					//实时时钟错误计数器,用于检测实时时钟运行状态
INT8U KeyTimer;						//键盘计数器
INT8U DisplayTimer; 		 		//LCD定时刷新计数器
INT16U BackLightTimer;				//LCD背光时间计数器
INT8U System_TimeStr[16];			//系统时间字符串
INT8U System_StartTimeStr[16];		//系统启动时间字符串
T_Times Current_Tm;					//当前时间结构
SYS_PARAM Sys_Param;

INT32U DI_Val;		//数字量输入值，除了主板自带的16路DI，再外扩16路
INT16U DO_Val;		//数字量输出值，除了主板自带的8路DO，再外扩8路

/***************************************
 模拟量部分
 **************************************/
INT8U Ad_ConvertEnd = 0;
INT16U Ad_Value[8];
INT16U Ad_Result[8][10];

AD_CALIBPARAM Ad_CalibParam;		//模拟量校准参数
AD_PARAM Ad_Param[8];				//模拟量通道配置参数
float Ad_RealData[8];				//AD采集值
MAX_MIN_DATA Ad_MaxMin[8];			//AD统计数据

/**************************************
 串口量部分
 **************************************/
const char ComBaud_Str[4][6] = { "9600", "19200", "2400", "4800" };
COM_PARAM Com_Param;				//串口量配置参数
float Com_Val[MODBUS_VARNUM];		//串口量实时值
char Com_Err[MODBUS_VARNUM];		//串口量通道错误标识
MAX_MIN_DATA Com_MaxMin[MODBUS_VARNUM];		//串口量通道参数统计数据
INT8U MbOtCnt;			        //Modbus超时计数器
INT16U MbScanCnt;	            //Modbus扫描周期计数器

MBIO_PARAM MbIO_Param;
char MbIO_Err[MODBUSIO_VARNUM];
INT8U MbIOOtCnt;			//modbus超时计数器
INT8U MbIODelayCnt;

/**************************************
 212部分
 **************************************/
unsigned int Hjt212RtdTimeCnt;	    //实时数据发送间隔计数器
unsigned char Hjt212TimeOffset;		//分钟数据的发送时间偏移
HJT212CMD Hjt212Cmd[HJT_STACK_NUM];	//212接收到的命令信息
HJT212SIGNAL Hjt212Signal[HJT_STACK_NUM] = { { 0, 1, 1, 1, 1, 1 } };//212上传数据标志
const int HJTUartChannel = GPRS_COM;					//dtu串口号

/**************************************
 串口部分
 **************************************/
COM_BUF Com_Buf[5];

/**************************************
 项目相关部分
 **************************************/
ALARM_PARAM Alarm_Param[MAX_ALARM_NUM];
INT8U BeepEnable = 1;
INT8U LineAlarm = 0;
INT8U OverAlarm = 0;

/**************************************
 公共函数部分
 **************************************/
void Delay_N_mS(unsigned int time_ms) /* n mS delay */
{
	unsigned char i;
	while (time_ms--) {
		i = 17;
		while (i--);
	}
}

void Delay_10_uS(void)
{
	char i = 20;
	while (i--);
}

