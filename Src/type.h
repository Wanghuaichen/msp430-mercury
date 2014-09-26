#ifndef _TYPE_H_
#define _TYPE_H_

#include <msp430xG46x.h>

//�������Ͷ���
typedef unsigned char INT8U;
typedef unsigned int INT16U;
typedef unsigned long INT32U;

/*******************************************
 ϵͳ����
 *******************************************/
typedef struct _SYS_PARAM {
	char pw[8];
	char st[4];
	char mn[16];
	char sim[12];
	char rtd[4];            //ʵʱ�����ϱ����
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

#define MS2TENMS(x) ((x)/10)      //������ֵת����10msֵ
#define S2TENMS(x)  ((x)*100)     //����ֵת����10msֵ
#define MIN2S(x)    ((x)*60)      //������ֵת������ֵ
/*******************************************
 ģ��������
 *******************************************/
typedef struct _AD_PARAM {
	INT8U id[4];    //��Ⱦ����
	INT8U type;     //AIת������4��20mA,0��5V,0��10V��
	float highval;  //��λֵ
	float lowval;   //��λֵ
	float rate;     //б��
	float offset;   //ƫ��
} AD_PARAM;

typedef struct _AD_CALIBPARAM {
	INT16U k20[8];  //20ma��У׼ֵ
	INT16U k4[8];   //4ma��У׼ֵ
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
 ����������
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
 212����
 *******************************************/
#define HJT_STACK_NUM 1     //212Э��ջ�ĸ���
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
 212���������ṹ
 *******************************************/
typedef struct _HjtCNTable {
	int (*func)(void*, int);
	char cn[6];
	int index;
} HjtCNTable;

/*******************************************
 ���ڻ�Buffer�ṹ
 *******************************************/
#define RINGBUF_SIZE 200

typedef struct _COM_BUF {
	unsigned char buf[RINGBUF_SIZE];
	int head;
	int tail;
} COM_BUF;

/*******************************************
 LCD����
 *******************************************/

/*******************************************
 ���̲���
 *******************************************/

/*******************************************
 ��Ŀ��ز���
 *******************************************/
#define MAX_ALARM_NUM		8
typedef struct _ALARM_PARAM {
	//MN��
	char mn[16];
	//��Ӧ��DOͨ����
	char inuse;
	//��ǰ����״̬,0Ϊ���ߣ�1Ϊ����
	char drop_status;
	//��ǰ����״̬,0Ϊ������1Ϊ����
	char over_status;
} ALARM_PARAM;

#endif /* End Of _TYPE_H_ */
