#ifndef _SC16IS752_H
#define _SC16IS752_H

#define CRYSTALFREQUENCY	14745600L

/*
 	 For 16is752
 */
#define RHR752 			0x00
#define THR752 			0x00
#define IER752 			0x01
#define FCR752 			0x02
#define IIR752 			0x02
#define LCR752 			0x03
#define MCR752 			0x04
#define LSR752 			0x05
#define MSR752 			0x06
#define SPR752 			0x07
#define TCR752 			0x06
#define TLR752 			0x07
#define TXLVL752 		0x08
#define RXLVL752 		0x09
#define IODIR752 		0x0A
#define IOSTATE752 		0x0B
#define IOINTENA752 	0x0C
#define IOCONTROL752	0x0E
#define EFCR752 		0x0F
#define DLL752 			0x00
#define DLH752 			0x01
#define EFR752 			0x02
#define XON1752 		0x04
#define XON2752 		0x05
#define XOFF1752 		0x06
#define XOFF2752 		0x07

#define NO_PARITY   	0x00
#define EVEN_PARITY 	0x18
#define ODD_PARITY  	0x08

#define RCVRDY  		0x01
#define OVRERR  		0x02
#define PRTYERR 		0x04
#define FRMERR  		0x08
#define BRKERR  		0x10
#define XMTRDY  		0x20
#define XMTRSR  		0x40
#define TIMEOUT			0x80

#define DTR 			0x01
#define RTS 			0x02
#define MC_INT			0x08

#define CTS 			0x10
#define DSR 			0x20

#define RX_INT  		0x01
#define TX_INT			0x02

#define NO_INT			0x01
#define URS_ID			0x06
#define RX_ID   		0x04
#define FDT_ID			0x0c
#define TBRE_ID			0x02
#define HSR_ID			0x00
#define RX_MASK 		0x0f

void IS752_Init(unsigned long baud,int com);
void IS752_ChangeBaudRate(unsigned long baud,int com);
void IS752_SendData(unsigned char *data,int len,int com);
unsigned char ReadRegister_IS752(unsigned char com, unsigned char address);
void WriteRegister_IS752(unsigned char com, unsigned char address, unsigned char data);
void IS752Reset(unsigned char chip);
void IS752En(unsigned char chip, unsigned char value);

#endif      //End Of _SC16IS752_H
