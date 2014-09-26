#ifndef _AT24C64_H
#define _AT24C64_H

#include "ad.h"
#include "control.h"
#include "modbus.h"

/* AT24C64�����������£�
 * �洢�����ͣ�EEPROM
 * ������8K * 8Bit(8192 * 8)
 * �ӿڣ�Serial��2Wire
 * ��ţ�SCL(Serial Clock)��SDA(Serial Data)��A0��A1��A2(Device/Page Address)��WP��Write Protect)
 * �ڴ���֯���ڲ���256��ҳ��ÿ��ҳ��32���ֽ�
 * �豸������
 * 		ʱ�Ӻ����ݷ��ͣ�SDA����ͨ�����������ߣ�����ֻ����SCL�͵�ƽʱ���ı䣬�����SCL�ߵ�ƽʱ���ı����
 * 		����Ϊ��һ����ʼ/ֹͣ����
 * ��ʼ������
 * 		��SCL�ߵ�ƽ״̬�£�SDA�ɸߵ��׵�ת������Ϊ��һ����ʼ��������ʼ�����������κ�����Ŀ�ʼ��
 * ֹͣ������
 * 		��SCL�ߵ�ƽ״̬�£�SDA�е͵��ߵı任����Ϊ��һ��ֹͣ������ֹͣ����������EEPROM���뱸��ģʽ��
 * Ӧ��
 * 		EEPROM���з���/���յĵ�ַ/���ݶ���8Bit���֣�EEPROM�ڵھŸ�ʱ�����ڷ���0֪ͨ�յ�һ���֡�
 */

//���ֲ����Ĵ洢��ַ
#define VERSIONCODE_ADDR    0X00
//#define FLOWCOUNT_ADDR      0X10
//#define PROCSTAT_ADDR       0X20
#define SYSPARAM_ADDR       0X100
#define ADPARAM_ADDR        0X200
#define COMPARAM_ADDR       0X300
#define ADCALIBPARAM_ADDR   0X400
#define ALARMPARAM_ADDR		0x500

void AT24C64_W(void *src,INT16U des,INT16U len);
void AT24C64_RS(void *des,INT16U src,INT16U len);

void Read_VersionCode(char *Code);
void Save_VersionCode(char *Code);

void Default_SysParam(SYS_PARAM *Param);
void Read_SysParam(SYS_PARAM *Param);
void Save_SysParam(SYS_PARAM *Param);

void Default_ADParam(AD_PARAM *Param);
void Read_ADParam(AD_PARAM *Param);
void Save_ADParam(AD_PARAM *Param);

void Default_ComParam(COM_PARAM *Param);
void Read_ComParam(COM_PARAM *Param);
void Save_ComParam(COM_PARAM *Param);

void Read_AlarmParam(ALARM_PARAM *Param);
void Save_AlarmParam(ALARM_PARAM *Param);
void Default_AlarmParam(ALARM_PARAM *Param);

void Fix_ADCalibParam(AD_CALIBPARAM *Param);
void Read_ADCalibParam(AD_CALIBPARAM *Param);
void Save_ADCalibParam(AD_CALIBPARAM *Param);

#endif

