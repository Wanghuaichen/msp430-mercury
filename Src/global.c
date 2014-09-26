#include "type.h"
#include "record.h"

/***************************************
 ϵͳ����
 **************************************/
const char VersionCode[6] = "VT05\0";	//����汾��
INT16U SoftWDT_Cnt = 0;				//��������
INT8U SoftWDT_Flag = 0;				//����ʶ
INT8U LEDTimer;						//����LED����ָʾ�Ƽ�����
INT8U RTCTimer;						//ʵʱʱ�Ӽ�����,���ڶ��ڶ�ȡʵʱʱ��
INT16U RTCErrTimer;					//ʵʱʱ�Ӵ��������,���ڼ��ʵʱʱ������״̬
INT8U KeyTimer;						//���̼�����
INT8U DisplayTimer; 		 		//LCD��ʱˢ�¼�����
INT16U BackLightTimer;				//LCD����ʱ�������
INT8U System_TimeStr[16];			//ϵͳʱ���ַ���
INT8U System_StartTimeStr[16];		//ϵͳ����ʱ���ַ���
T_Times Current_Tm;					//��ǰʱ��ṹ
SYS_PARAM Sys_Param;

INT32U DI_Val;		//����������ֵ�����������Դ���16·DI��������16·
INT16U DO_Val;		//���������ֵ�����������Դ���8·DO��������8·

/***************************************
 ģ��������
 **************************************/
INT8U Ad_ConvertEnd = 0;
INT16U Ad_Value[8];
INT16U Ad_Result[8][10];

AD_CALIBPARAM Ad_CalibParam;		//ģ����У׼����
AD_PARAM Ad_Param[8];				//ģ����ͨ�����ò���
float Ad_RealData[8];				//AD�ɼ�ֵ
MAX_MIN_DATA Ad_MaxMin[8];			//ADͳ������

/**************************************
 ����������
 **************************************/
const char ComBaud_Str[4][6] = { "9600", "19200", "2400", "4800" };
COM_PARAM Com_Param;				//���������ò���
float Com_Val[MODBUS_VARNUM];		//������ʵʱֵ
char Com_Err[MODBUS_VARNUM];		//������ͨ�������ʶ
MAX_MIN_DATA Com_MaxMin[MODBUS_VARNUM];		//������ͨ������ͳ������
INT8U MbOtCnt;			        //Modbus��ʱ������
INT16U MbScanCnt;	            //Modbusɨ�����ڼ�����

MBIO_PARAM MbIO_Param;
char MbIO_Err[MODBUSIO_VARNUM];
INT8U MbIOOtCnt;			//modbus��ʱ������
INT8U MbIODelayCnt;

/**************************************
 212����
 **************************************/
unsigned int Hjt212RtdTimeCnt;	    //ʵʱ���ݷ��ͼ��������
unsigned char Hjt212TimeOffset;		//�������ݵķ���ʱ��ƫ��
HJT212CMD Hjt212Cmd[HJT_STACK_NUM];	//212���յ���������Ϣ
HJT212SIGNAL Hjt212Signal[HJT_STACK_NUM] = { { 0, 1, 1, 1, 1, 1 } };//212�ϴ����ݱ�־
const int HJTUartChannel = GPRS_COM;					//dtu���ں�

/**************************************
 ���ڲ���
 **************************************/
COM_BUF Com_Buf[5];

/**************************************
 ��Ŀ��ز���
 **************************************/
ALARM_PARAM Alarm_Param[MAX_ALARM_NUM];
INT8U BeepEnable = 1;
INT8U LineAlarm = 0;
INT8U OverAlarm = 0;

/**************************************
 ������������
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

