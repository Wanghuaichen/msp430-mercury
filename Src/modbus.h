#ifndef _MODBUS_H
#define _MODBUS_H

//=====================================
//TimerA的定时器10ms产生一次中断
//在每次中断中加入一个计数器，
//每10ms计数器加1，用于modbus的超时判断
//=====================================
#define  MBOTCNTNUM	    (MS2TENMS(1000)) 	//modbus超时值的计数器
#define  MBSCANCNTNUM	(S2TENMS(2))	    //modbus扫描周期的计数器
#define  MBMAXERRCNT 	3			    	//modbus最大错误计数，超过此值后会将数据清零

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
