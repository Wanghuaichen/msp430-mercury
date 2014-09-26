#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "type.h"
#include "record.h"

extern const char VersionCode[6];	//����汾��
extern INT16U SoftWDT_Cnt;			//��������
extern INT8U SoftWDT_Flag;
extern INT8U LEDTimer;
extern INT8U RTCTimer;
extern INT16U RTCErrTimer;
extern INT8U KeyTimer;						//���̼�����
extern INT8U System_TimeStr[16];
extern INT8U System_StartTimeStr[16];
extern T_Times Current_Tm;
extern INT8U DisplayTimer;         //LCD��ʱˢ�¼�����
extern INT16U BackLightTimer;      //LCD����ʱ�������

extern INT32U DI_Val;		//���������Դ���16·DI��������16·
extern INT16U DO_Val;		//���������Դ���8·DO��������8·

/***************************************
		212����
 **************************************/
extern unsigned int Hjt212RtdTimeCnt;	    //ʵʱ���ݷ��ͼ��������
extern unsigned char Hjt212TimeOffset;		//�������ݵķ���ʱ��ƫ��
extern HJT212CMD Hjt212Cmd[HJT_STACK_NUM];	//212���յ���������Ϣ
extern HJT212SIGNAL Hjt212Signal[HJT_STACK_NUM];	//212�ϴ����ݱ�־
extern const int HJTUartChannel;					//dtu���ں�

/***************************************
		ϵͳ����
**************************************/
extern SYS_PARAM Sys_Param;

/***************************************
		ģ��������
**************************************/
extern INT8U Ad_ConvertEnd;
extern INT16U Ad_Value[8];
extern INT16U Ad_Result[8][10];
extern AD_CALIBPARAM Ad_CalibParam;
extern AD_PARAM Ad_Param[8];
extern float Ad_RealData[8];
extern MAX_MIN_DATA Ad_MaxMin[8];

/**************************************
		����������
**************************************/
extern MBIO_PARAM MbIO_Param;
extern char MbIO_Err[MODBUSIO_VARNUM];
extern INT8U MbIOOtCnt;			//modbus��ʱ������
extern INT8U MbIODelayCnt;

extern COM_PARAM Com_Param;
extern const char ComBaud_Str[4][6];
extern float Com_Val[MODBUS_VARNUM];
extern char Com_Err[MODBUS_VARNUM];		//������ͨ�������ʶ
extern MAX_MIN_DATA Com_MaxMin[MODBUS_VARNUM];		//������ͨ������ͳ������

extern INT8U MbOtCnt;		//Modbus��ʱ������
extern INT16U MbScanCnt;	//Modbusɨ�����ڼ�����

/**************************************
		���ڲ���
**************************************/
extern COM_BUF Com_Buf[5];

/**************************************
		��Ŀ��ز���
 **************************************/
extern ALARM_PARAM Alarm_Param[MAX_ALARM_NUM];
extern INT8U BeepEnable;

/**************************************
		������������
**************************************/
void Delay_N_mS(unsigned int n_milisecond);
void Delay_10_uS(void);

#endif /* End Of _GLOBAL_H_ */
