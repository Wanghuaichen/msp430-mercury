#ifndef _MODBUS_H
#define _MODBUS_H

//=====================================
//TimerA�Ķ�ʱ��10ms����һ���ж�
//��ÿ���ж��м���һ����������
//ÿ10ms��������1������modbus�ĳ�ʱ�ж�
//=====================================
#define  MBOTCNTNUM	    (MS2TENMS(1000)) 	//modbus��ʱֵ�ļ�����
#define  MBSCANCNTNUM	(S2TENMS(2))	    //modbusɨ�����ڵļ�����
#define  MBMAXERRCNT 	3			    	//modbus�����������������ֵ��Ὣ��������

void Modbus_Init(void);
void SendModbusCmd(void);
unsigned short CRC16_Modbus(unsigned char *src, int len);
int RcvModbusRsp(void);
int AnalystModbusRsp(void);
void ProcessModbus(void);
void ProcessModbusSlave(void);
int ModbusSlaveRecv(unsigned char *recvBuf);
void ModbusSlaveParse(unsigned char *pkt, int len);
void ModbusSlaveRespReadCoils(unsigned int addr, unsigned int cnt);
void ModbusSlaveRespReadDIs(unsigned int addr, unsigned int cnt);
void ModbusSlaveRespReadHoldRegs(unsigned int addr, unsigned int cnt);
void ModbusSlaveRespReadInputRegs(unsigned int addr, unsigned int cnt);
void ModbusSlaveRespWriteCoil(unsigned int addr, unsigned int value);
void ModbusSlaveRespWriteCoils(unsigned int addr, unsigned int cnt, unsigned char *value);
void ModbusSlaveRespWriteReg(unsigned int addr, unsigned int value);
void ModbusSlaveRespWriteRegs(unsigned int addr, unsigned int cnt, unsigned char *value);
void ModbusSlaveResp(unsigned char *pkt, unsigned char len);
void ModbusSlaveExceptionResp(unsigned char errorCode, unsigned char exceptionCode);

#endif //End Of _MODBUS_H
