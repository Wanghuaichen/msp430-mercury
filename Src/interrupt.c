#include "uart.h"
#include "global.h"
#include "cpu_init.h"
#include "sc16is752.h"

/*******************************************
		TimerA�жϴ�����,10MSһ���ж�
*******************************************/
#pragma vector = TIMERA0_VECTOR
__interrupt void TimerA0_ISR(void)
{
	LEDTimer++;
	KeyTimer++;
	DisplayTimer++;
	RTCTimer++;
	BackLightTimer++;
	MbOtCnt++;
	MbScanCnt++;
	MbIOOtCnt++;
	MbIODelayCnt++;
}

/*******************************************
		TimerB�жϴ�����
*******************************************/
#pragma vector = TIMERB0_VECTOR
__interrupt void TimerB0_ISR(void)
{
	//ÿ5s���һ������־�������������Ӧ������ѭ���б���0��
	//���û����0��˵�������ܷɣ���ʱ�������Ź�
	RTCErrTimer++;
	SoftWDT_Cnt++;
	if(SoftWDT_Cnt > 5000)
	{
		if(SoftWDT_Flag == 0)
		{
			SoftWDT_Cnt = 0;
			SoftWDT_Flag = 1;
		}
		else
		{
			Start_WDT();
		}
	}
}

/*******************************************
          AD Interrupt Function
*******************************************/
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
	static INT8U Ad_Times = 0;

	Ad_Result[0][2 * Ad_Times] = ADC12MEM0;
	Ad_Result[0][2 * Ad_Times + 1] = ADC12MEM1;
	Ad_Result[1][2 * Ad_Times] = ADC12MEM2;
	Ad_Result[1][2 * Ad_Times + 1] = ADC12MEM3;
	Ad_Result[2][2 * Ad_Times] = ADC12MEM4;
	Ad_Result[2][2 * Ad_Times + 1] = ADC12MEM5;
	Ad_Result[3][2 * Ad_Times] = ADC12MEM6;
	Ad_Result[3][2 * Ad_Times + 1] = ADC12MEM7;
	Ad_Result[4][2 * Ad_Times] = ADC12MEM8;
	Ad_Result[4][2 * Ad_Times + 1] = ADC12MEM9;
	Ad_Result[5][2 * Ad_Times] = ADC12MEM10;
	Ad_Result[5][2 * Ad_Times + 1] = ADC12MEM11;
	Ad_Result[6][2 * Ad_Times] = ADC12MEM12;
	Ad_Result[6][2 * Ad_Times + 1] = ADC12MEM13;
	Ad_Result[7][2 * Ad_Times] = ADC12MEM14;
	Ad_Result[7][2 * Ad_Times + 1] = ADC12MEM15;

	//ÿ�����ݲ���ʮ��
	Ad_Times++;
	if(Ad_Times >= 5)
	{
		ADC12CTL0 &= ~(ADC12SC + ENC);  //�ر�ADת��
		Ad_Times = 0;
		Ad_ConvertEnd = 1;
	}
}

/*******************************************
          UART0 Receive Interrupt Function
*******************************************/
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIA0RX_ISR(void)
{
	INT8U temp = UCA0RXBUF;
	PutComBuf(&Com_Buf[COM1], &temp, 1);
}

/*******************************************
        External Interrupt Function
        Used For IS752(Extended Uart)
*******************************************/
#pragma vector = PORT2_VECTOR
__interrupt void EXTERNALISR(void)
{
	unsigned char data;
	int com,num;

	if(P2IFG & BIT1) {
		P2IFG &= ~BIT1;
		for (com = COM2; com <= COM3; ++com) {
			num = 0;
			while ((ReadRegister_IS752(com, RXLVL752) & 0x7f) != 0) {
				data = ReadRegister_IS752(com, RHR752);
				PutComBuf(&Com_Buf[com], &data, 1);
				//�����������ַ�����ֹ��ʱ��ռ���ж�
				if (++num > 7)
					break;
			}
		}
	}

	if(P2IFG&BIT2) {
		P2IFG &= ~BIT2;
		for (com = COM4; com <= COM5; ++com) {
			num = 0;
			while ((ReadRegister_IS752(com, RXLVL752) & 0x7f) != 0) {
				data = ReadRegister_IS752(com, RHR752);
				PutComBuf(&Com_Buf[com],&data,1);
				//��ֹ��ʱ��ռ���ж�
				if (++num > 7)
					break;
			}
		}
	}
}
