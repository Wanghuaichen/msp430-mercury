#ifndef _AT24C64_H
#define _AT24C64_H

#include "ad.h"
#include "control.h"
#include "modbus.h"

/* AT24C64基本参数如下：
 * 存储器类型：EEPROM
 * 容量：8K * 8Bit(8192 * 8)
 * 接口：Serial，2Wire
 * 阵脚：SCL(Serial Clock)、SDA(Serial Data)、A0、A1、A2(Device/Page Address)、WP（Write Protect)
 * 内存组织：内部分256个页，每个页有32个字节
 * 设备操作：
 * 		时钟和数据发送：SDA引脚通常被外设拉高，并且只能在SCL低电平时被改变，如果在SCL高电平时被改变则会
 * 		被认为是一个起始/停止条件
 * 起始条件：
 * 		在SCL高电平状态下，SDA由高到底的转换被认为是一个起始条件。起始条件必须在任何命令的开始。
 * 停止条件：
 * 		在SCL高电平状态下，SDA有低到高的变换被认为是一个停止条件。停止条件讲引起EEPROM进入备用模式。
 * 应答：
 * 		EEPROM所有发送/接收的地址/数据都是8Bit的字，EEPROM在第九个时钟周期发送0通知收到一个字。
 */

//各种参数的存储地址
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

