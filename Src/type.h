#ifndef _TYPE_H_
#define _TYPE_H_

#include <msp430xG46x.h>

//数据类型定义
typedef unsigned char INT8U;
typedef unsigned int INT16U;
typedef unsigned long INT32U;

/*******************************************
 系统部分
 *******************************************/
typedef struct _SYS_PARAM {
	char pw[8];
	char st[4];
	char mn[16];
	char sim[12];
	char rtd[4];            //实时数据上报间隔
} SYS_PARAM;

typedef struct _tm {
	int year;
	int month;
	int date;
	int hour;
	int minute;
	int second;
	int weekday;
} T_Times, *T_pTimes;

#define ENABLEINT()   _BIS_SR(GIE)
#define DISABLEINT()  _BIC_SR(GIE)

#define MS2TENMS(x) ((x)/10)      //将毫秒值转换成10ms值
#define S2TENMS(x)  ((x)*100)     //将秒值转换成10ms值
#define MIN2S(x)    ((x)*60)      //将分钟值转换成秒值
/*******************************************
 模拟量部分
 *******************************************/
typedef struct _AD_PARAM {
	INT8U id[4];    //污染物编号
	INT8U type;     //AI转换种类4－20mA,0－5V,0－10V，
	float highval;  //上位值
	float lowval;   //下位值
	float rate;     //斜率
	float offset;   //偏置
} AD_PARAM;

typedef struct _AD_CALIBPARAM {
	INT16U k20[8];  //20ma的校准值
	INT16U k4[8];   //4ma的校准值
} AD_CALIBPARAM;

typedef struct _MAX_MIN_DATA {
	float mnt_min;
	float mnt_max;
	double mnt_sum;
	float mnt_avg;
	double mnt_cou;
	long mnt_cnt;

	float hour_min;
	float hour_max;
	double hour_sum;
	float hour_avg;
	double hour_cou;
	long hour_cnt;

	float day_min;
	float day_max;
	double day_sum;
	float day_avg;
	double day_cou;
	long day_cnt;
} MAX_MIN_DATA;

/*******************************************
 串口量部分
 *******************************************/
#define COM1      0
#define GPRS      0
#define COM2      1
#define COM3      2
#define COM4      3
#define COM5      4
#define GPRS_COM    COM1
#define MODBUS_COM  COM5
#define MODBUS_SLAVE_COM	COM5
#define MODBUS_SLAVE_ADDR	0x01
#define MODBUS_VARNUM 		7
#define MODBUSIO_COM  		COM4
#define MODBUSIO_VARNUM 	2

typedef struct _MBIO_PARAM {
	INT8U baud_index;
	INT8U devid[MODBUSIO_VARNUM];
	INT8U funccode[MODBUSIO_VARNUM];
	INT16U regaddr[MODBUSIO_VARNUM];
	INT16U reglen[MODBUSIO_VARNUM];
} MBIO_PARAM;

typedef struct _COM_PARAM {
	INT8U baud_index;
	INT8U devid[MODBUS_VARNUM];
	INT16U regaddr[MODBUS_VARNUM];
	char regid[MODBUS_VARNUM][4];
} COM_PARAM;

/*******************************************
 212部分
 *******************************************/
#define HJT_STACK_NUM 1     //212协议栈的个数
#define HJT_UART 0
//#define HJT_ETH	 1
typedef struct _HJT212CMD {
	char qn[18];
	char flag[2];
	char pw[7];
	char mn[15];
	char cn[5];
	char* cp;
} HJT212CMD;

typedef struct _HJT212SIGNAL {
	char upflag_reqstatus;
	char upflag_rt;
	char upflag_rs;
	char upflag_m;
	char upflag_h;
	char upflag_d;
} HJT212SIGNAL;

/*******************************************
 212命令索引结构
 *******************************************/
typedef struct _HjtCNTable {
	int (*func)(void*, int);
	char cn[6];
	int index;
} HjtCNTable;

/*******************************************
 串口环Buffer结构
 *******************************************/
#define RINGBUF_SIZE 200

typedef struct _COM_BUF {
	unsigned char buf[RINGBUF_SIZE];
	int head;
	int tail;
} COM_BUF;

/*******************************************
 LCD部分
 *******************************************/

/*******************************************
 键盘部分
 *******************************************/

/*******************************************
 项目相关部分
 *******************************************/
#define MAX_ALARM_NUM		8
typedef struct _ALARM_PARAM {
	//MN号
	char mn[16];
	//对应的DO通道号
	char inuse;
	//当前掉线状态,0为在线，1为掉线
	char drop_status;
	//当前超标状态,0为正常，1为超标
	char over_status;
} ALARM_PARAM;

#endif /* End Of _TYPE_H_ */
