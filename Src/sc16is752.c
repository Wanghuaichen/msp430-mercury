#include "uart.h"
#include "global.h"
#include "sc16is752.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************
752的片选, 1为使能,0为禁止
*******************************************************************/
void IS752En(unsigned char chip, unsigned char value)
{
	if(chip == 0)
	{
		if (value == 1)
			P7OUT &= ~BIT1;
		else if (value == 0)
			P7OUT |= BIT1;
	}
	else if(chip == 1)
	{
		if (value == 1)
			P4OUT &= ~BIT2;
		else if (value == 0)
			P4OUT |= BIT2;
	}
}

/*******************************************************************
复位752
*******************************************************************/
void IS752Reset(unsigned char chip)
{
	if(chip == 0)
	{
		P8OUT &= ~BIT3; 
		Delay_10_uS(); 
		P8OUT |= BIT3; 
		Delay_10_uS();
	}
	else if(chip == 1)
	{
		P7OUT &= ~BIT6; 
		Delay_10_uS(); 
		P7OUT |= BIT6; 
		Delay_10_uS();
	}
}

/*******************************************************************
写752的寄存器
*******************************************************************/
void WriteRegister_IS752(unsigned char com, unsigned char address,
	unsigned char data)
{
	unsigned char chip;
	unsigned char channel;
		
	switch(com)
    {
    case COM2:
		chip = 0;
		channel = 0;
        break;
    case COM3:
        chip = 0;
		channel = 1;
        break;
    case COM4:
        chip = 1;
		channel = 0;
        break;
    case COM5:
        chip = 1;
		channel = 1;
        break;
    default:
        return;
    }
	
	IS752En(chip, 1);
	
	address <<= 3;
	channel <<= 1;
	address |= channel;
	
	while((U1TCTL&TXEPT)!=0X01);	//发地址
	IFG2 &= ~(UTXIFG1+URXIFG1);
    U1TXBUF = address;
    while(!(IFG2&UTXIFG1));			//发数据
	//while((U1TCTL&TXEPT)!=0X01);
    U1TXBUF = data;
	while((U1TCTL&TXEPT)!=0X01);
	
	IS752En(chip, 0);
}

/*******************************************************************
读752的寄存器
*******************************************************************/
unsigned char ReadRegister_IS752(unsigned char com, unsigned char address)
{
	unsigned char chip;
	unsigned char channel;
	unsigned char data = 0;
	
	switch(com)  {
    case COM2:
		chip = 0;
		channel = 0;
        break;
    case COM3:
        chip = 0;
		channel = 1;
        break;
    case COM4:
        chip = 1;
		channel = 0;
        break;
    case COM5:
        chip = 1;
		channel = 1;
        break;
    default:
        return 0;
    }
	
	IS752En(chip,1);
	
	address <<= 3;
	channel <<= 1;
	address |= channel;
	address |= 0x80;
	
	//send cmd
    while((U1TCTL&TXEPT)!=0X01);
	IFG2  &=~(UTXIFG1+URXIFG1);
	U1TXBUF = address;
	    
    //read
    while(!(IFG2&UTXIFG1));
	//while((U1TCTL&TXEPT)!=0X01);
	U1TXBUF = 0;
	while((U1TCTL&TXEPT)!=0X01);
    data = U1RXBUF;
		
	IS752En(chip,0);

	return data;
}

void IS752_SendData(unsigned char *data,int len,int com)
{
    int num, cnt;
	
	for (num = 0; num < len; ++num)
	{
		for (cnt = 0; cnt < 10000; ++cnt) //防止占用太长时间
		{
			if ((ReadRegister_IS752(com, TXLVL752) & 0x7f) != 0)
			{
				WriteRegister_IS752(com, THR752, data[num]);
				break;
			}
		}
		if (cnt >= 10000)	//发送失败
			break;
	}
}

void IS752_Init(unsigned long baud,int com)
{
    unsigned long int divisor;
	
	divisor = CRYSTALFREQUENCY / 16L / baud;

	//IS752Reset();
	WriteRegister_IS752(com, LCR752, 0xBF);
	WriteRegister_IS752(com, EFR752, 0x10);
	WriteRegister_IS752(com, LCR752, 0x80);
	WriteRegister_IS752(com, DLL752, (unsigned char) (divisor & 0xff));
	WriteRegister_IS752(com, DLH752,
						(unsigned char) ((divisor >> 8) & 0xff));
	WriteRegister_IS752(com, LCR752, 0x03);

	WriteRegister_IS752(com, IER752, 0x01);
	WriteRegister_IS752(com, FCR752, 0x07);
	Delay_N_mS(200); 	//wait for the end of fifo reset
	WriteRegister_IS752(com, MCR752, 0x04);
	//WriteRegister_IS752(com, TCR752, 0x00);
	//WriteRegister_IS752(com, TLR752, 0x00);
	WriteRegister_IS752(com, IODIR752, 0xFF);
	WriteRegister_IS752(com, IOSTATE752, 0xFF);
	WriteRegister_IS752(com, IOCONTROL752, 0x00);
	WriteRegister_IS752(com, IOINTENA752, 0x00);
	WriteRegister_IS752(com, EFCR752, 0x10);
}

void IS752_ChangeBaudRate(unsigned long baud,int com)
{
    unsigned long int divisor;

	divisor = CRYSTALFREQUENCY / 16L / baud;

	WriteRegister_IS752(com, LCR752, 0x80);
	WriteRegister_IS752(com, DLL752, (unsigned char) (divisor & 0xff));
	WriteRegister_IS752(com, DLH752,
		(unsigned char) ((divisor >> 8) & 0xff));
	WriteRegister_IS752(com, LCR752, 0x03);
}
