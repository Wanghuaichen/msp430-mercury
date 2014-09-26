#ifndef _UART_H
#define _UART_H

#include "type.h"

int PutComBuf(COM_BUF *pbuf, unsigned char *pdata, int datasize);
int GetComBuf(COM_BUF *pbuf, unsigned char *pdata, int datasize);

void Uart_SendData(unsigned char *src,int len,int com);
int Uart_RcvData(unsigned char *dst,int len,int com);
void Uart_ClearRcvBuf(int com);

#endif  /*End Of _UART_H*/

