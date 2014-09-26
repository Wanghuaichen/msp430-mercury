#include <string.h>
#include "global.h"
#include "at24c64.h"

#define AT24WR_PROTECT    P3OUT|=BIT0
#define AT24WR_UNPROTECT  P3OUT&=~BIT0

#define I2C_START     UCB0CTL1|=UCTXSTT
#define I2C_TRANSMIT  UCB0CTL1|=UCTR
#define I2C_RECEIVE   UCB0CTL1&=~UCTR
#define I2C_STOP      UCB0CTL1|=UCTXSTP

void AT24C64_W(void *src,INT16U des,INT16U len)
{
	volatile INT16U i;
	INT8U *Source;

	Source  =(INT8U *)src;
	I2C_TRANSMIT;   //transmit mode
	UCB0I2CSA =0x50;
	AT24WR_UNPROTECT;
	for(i=0;i<len;i++)
	{
		I2C_TRANSMIT;
		I2C_START;
		while((IFG2&UCB0TXIFG)==0);
		UCB0TXBUF =(INT8U)(des>>8);
		while((IFG2&UCB0TXIFG)==0);
		UCB0TXBUF =(INT8U)des;
		while((IFG2&UCB0TXIFG)==0);
		UCB0TXBUF =*Source++;
		while((IFG2&UCB0TXIFG)==0);
		I2C_STOP;
		while(UCB0CTL1&UCTXSTP);
		des++;
		Delay_N_mS(350);
	}
	AT24WR_PROTECT;
}

void AT24C64_RS(void *des,INT16U src,INT16U len)
{
	INT16U i;
	INT8U *Dest;
	Dest  =(INT8U *)des;

	I2C_TRANSMIT;   //transmit mode
	I2C_START;
	while((IFG2&UCB0TXIFG)==0);
	UCB0TXBUF =(INT8U)(src>>8);
	while((IFG2&UCB0TXIFG)==0);
	UCB0TXBUF =(INT8U)src;
	while((IFG2&UCB0TXIFG)==0);

	I2C_RECEIVE;
	I2C_START;
	while((IFG2&UCB0RXIFG)==0);
	*Dest =UCB0RXBUF;

	for(i=0;i<len;i++)
	{
		while((IFG2&UCB0RXIFG)==0);
		*Dest++ =UCB0RXBUF;
	}
	I2C_STOP;
}

////////////////////////////////

//程序版本号
void Read_VersionCode(char *Code)
{
	AT24C64_RS(Code,VERSIONCODE_ADDR,6);
	Delay_N_mS(500);
	AT24C64_RS(Code,VERSIONCODE_ADDR,6);
	Delay_N_mS(500);
}

void Save_VersionCode(char *Code)
{
	char temp[6];
	while(1) {   //写了再读，保证数据正确写入
		AT24C64_W(Code, VERSIONCODE_ADDR, 6);

		Read_VersionCode(temp);
		if(memcmp(temp, Code, 6) == 0)
			break;
	}
}

//系统参数
void Default_SysParam(SYS_PARAM *Param)
{
	strcpy(Param->pw,"123456");
	strcpy(Param->st,"32");
	strcpy(Param->mn,"12345678901234");
	strcpy(Param->sim,"00000000000");
	strcpy(Param->rtd,"030");
	strcpy(Param->pw,"123456");
}


void Read_SysParam(SYS_PARAM *Param)
{
	AT24C64_RS(Param, SYSPARAM_ADDR, sizeof(SYS_PARAM));
	Delay_N_mS(500);
	AT24C64_RS(Param, SYSPARAM_ADDR, sizeof(SYS_PARAM));
	Delay_N_mS(500);
}

void Save_SysParam(SYS_PARAM *Param)
{
	SYS_PARAM temp;

	while(1) {
		AT24C64_W(Param, SYSPARAM_ADDR, sizeof(SYS_PARAM));
		Read_SysParam(&temp);
		if(memcmp(&temp, Param, sizeof(SYS_PARAM)) == 0)
			break;
	}
}

//AD参数
void Default_ADParam(AD_PARAM *Param)
{
	char i;

	for(i=0; i<8; ++i) {
		strcpy((char *)Param[i].id,"000");
		Param[i].type = 0;
		Param[i].highval = 60;
		Param[i].lowval = 0;
		Param[i].rate = 0;
		Param[i].offset = 0;
	}
}

void Read_ADParam(AD_PARAM *Param)
{
	AT24C64_RS(Param, ADPARAM_ADDR, sizeof(AD_PARAM)*8);
	Delay_N_mS(500);
	AT24C64_RS(Param, ADPARAM_ADDR, sizeof(AD_PARAM)*8);
	Delay_N_mS(500);
}

void Save_ADParam(AD_PARAM *Param)
{
	AD_PARAM temp[8];
	while(1) {
		AT24C64_W(Param, ADPARAM_ADDR, sizeof(AD_PARAM)*8);

		Read_ADParam(temp);
		if(memcmp(temp, Param, sizeof(AD_PARAM) * 8) == 0)
			break;
	}
}

//通信参数
void Default_ComParam(COM_PARAM *Param)
{
	char i;

	Param->baud_index = 0;
	for(i = 0; i < MODBUS_VARNUM; ++i) {
		strcpy((char *)Param->regid[i],"000");
		Param->regaddr[i] = 1;
		Param->devid[i] = 1;
	}
}

void Read_ComParam(COM_PARAM *Param)
{
	AT24C64_RS(Param, COMPARAM_ADDR, sizeof(COM_PARAM));
	Delay_N_mS(500);
	AT24C64_RS(Param, COMPARAM_ADDR, sizeof(COM_PARAM));
	Delay_N_mS(500);
}

void Save_ComParam(COM_PARAM *Param)
{
	COM_PARAM temp;
	while(1) {
		AT24C64_W(Param, COMPARAM_ADDR, sizeof(COM_PARAM));

		Read_ComParam(&temp);
		if(memcmp(&temp, Param, sizeof(COM_PARAM)) == 0)
			break;
	}
}

void Default_AlarmParam(ALARM_PARAM *Param)
{
	char i;

	for(i = 0; i < MAX_ALARM_NUM; ++i) {
		strcpy(Param[i].mn, "00000000000000");
		Param[i].inuse = 0;
		Param[i].drop_status = 0;
		Param[i].over_status = 0;
	}
}

void Read_AlarmParam(ALARM_PARAM *Param)
{
	AT24C64_RS(Param, ALARMPARAM_ADDR, sizeof(ALARM_PARAM) * MAX_ALARM_NUM);
	Delay_N_mS(500);
	AT24C64_RS(Param, ALARMPARAM_ADDR, sizeof(ALARM_PARAM) * MAX_ALARM_NUM);
	Delay_N_mS(500);
}

void Save_AlarmParam(ALARM_PARAM *Param)
{
	ALARM_PARAM temp[MAX_ALARM_NUM];

	while(1) {
		AT24C64_W(Param, ALARMPARAM_ADDR, sizeof(ALARM_PARAM) * MAX_ALARM_NUM);
		Read_AlarmParam(temp);
		if(memcmp(temp, Param, sizeof(ALARM_PARAM) * MAX_ALARM_NUM) == 0)
			break;
	}
}

//对AD校准参数进行修正
void Fix_ADCalibParam(AD_CALIBPARAM *Param)
{
	char i;

	for(i=0;i<8;++i) {
		if((Param->k4[i] < 0x100) || (Param->k20[i] > 0x850)) {
			Param->k4[i] = 0x19b;
			Param->k20[i] = 0x7f9;
		}
	}
}

void Read_ADCalibParam(AD_CALIBPARAM *Param)
{
	AT24C64_RS(Param, ADCALIBPARAM_ADDR, sizeof(AD_CALIBPARAM));
	Delay_N_mS(500);
	AT24C64_RS(Param, ADCALIBPARAM_ADDR, sizeof(AD_CALIBPARAM));
	Delay_N_mS(500);
}

void Save_ADCalibParam(AD_CALIBPARAM *Param)
{
	AD_CALIBPARAM temp;
	while(1)
	{
		AT24C64_W(Param,ADCALIBPARAM_ADDR,sizeof(AD_CALIBPARAM));

		Read_ADCalibParam(&temp);
		if(memcmp(&temp,Param,sizeof(AD_CALIBPARAM)) == 0)
			break;
	}
} 

