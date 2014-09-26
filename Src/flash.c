#include "flash.h"
#include "global.h"
#include "cpu_init.h"

#define OPCODE_READ_MEM_CONTINUOUS      0X03
#define OPCODE_READ_BUF1         		0XD1
#define OPCODE_READ_BUF2          		0XD3
#define OPCODE_WRITE_BUF1         		0X84
#define OPCODE_WRITE_BUF2         		0X87
#define OPCODE_MEM_WRITE_THBUF1  		0X82
#define OPCODE_MEM_WRITE_THBUF2  		0X85
#define OPCODE_BUF1_TO_MEM_WITH_ERASE   0X83
#define OPCODE_BUF2_TO_MEM_WITH_ERASE   0X86
#define OPCODE_MEM_TO_BUF1_TRANSFER     0X53
#define OPCODE_MEM_TO_BUF2_TRANSFER     0X55
#define OPCODE_READ_STATUS_REG			0X57
#define OPCODE_ERASE_MEM_PAGE			0x81
#define OPCODE_ERASE_MEM_BLOCK			0x50
#define OPCODE_ERASE_MEM_SECTOR			0x7C
#define OPCODE_CMP_MEM_WITH_BUF1		0x60
#define OPCODE_CMP_MEM_WITH_BUF2		0x61

#define FLASH_IDLE_BIT_MASK			0x80
#define FLASH_CMP_BIT_MASK			0x40

#define FLASH_WRITE		1
#define FLASH_READ		0

static inline void FlashReset()
{
	P4OUT &= ~BIT0;
	P4OUT |= BIT0;
}

/* 使能flash之前，需要先关闭中断，否则读取出来的数据可能存在问题 */
static inline void FlashEnable()
{
	P4OUT &= ~BIT1;
	DISABLEINT();
	Stop_WDT();
}

/* 退出flash之后，要开中断  */
static inline void FlashDisable()
{
	ENABLEINT();
	P4OUT |= BIT1;
	//Start_WDT();
}

static inline void FlashWriteProtect()
{
	P5OUT &= ~BIT7;
}

static inline void FlashWriteUnprotect()
{
	P5OUT |= BIT7;
}

static inline void FlashSend(char *ptr, unsigned char cnt)
{
	while (cnt--) {
		while (!(IFG2 & UTXIFG1));
		U1TXBUF = *ptr++;
	}
}

static inline void FlashRecv(char *ptr, unsigned char cnt)
{
	while (cnt--) {
		while (!(IFG2 & UTXIFG1));
		U1TXBUF = 0;
		while (!(IFG2 & URXIFG1));
		*ptr++ = U1RXBUF;
	}
}

static inline void FlashReadyToPerform(unsigned char opcode, INT32U addr,
		unsigned char isWrite, unsigned int delay_ms)
{
	char bytes[3], cnt;

	bytes[0] = addr >> 16 & 0xFF;
	bytes[1] = addr >> 8 & 0xFF;
	bytes[2] = addr & 0xFF;

	U1TXBUF = 0;
	while(!(TXEPT & U1TCTL));

	if (isWrite) {
		FlashWriteUnprotect();
	}

	FlashEnable();

	while (!(IFG2 & UTXIFG1));
	U1TXBUF = opcode;

	for (cnt = 0; cnt < 3; cnt++) {
		while (!(IFG2 & UTXIFG1));
		U1TXBUF = bytes[cnt];
	}

	if (!isWrite) {
		/* Receive A Dummy Byte */
		Delay_N_mS(20);
		FlashRecv(bytes, 1);
	}

	Delay_N_mS(delay_ms);
}

static inline void FlashFinishPerform(unsigned char isWrite,
		unsigned int delay_ms)
{
	Delay_N_mS(delay_ms);
	FlashDisable();
	if (isWrite) {
		while (!(TXEPT & U1TCTL));
		FlashWriteProtect();
	}
}


void Flash_Init()
{
	FlashReset();
	FlashEnable();
	FlashDisable();
}

INT8U FlashIsBusy()
{
	INT8U ret;

	U1TXBUF = 0;
	while(!(TXEPT & U1TCTL));
	FlashEnable();
	while (!(IFG2 & UTXIFG1));
	U1TXBUF = OPCODE_READ_STATUS_REG;

	/* Dummy Byte */
	while (!(IFG2 & UTXIFG1));
	U1TXBUF = 0;
	while (!(IFG2 & URXIFG1));
	ret = U1RXBUF;

	while (!(IFG2 & UTXIFG1));
	U1TXBUF = 0;
	while (!(IFG2 & URXIFG1));
	ret = U1RXBUF;

	FlashDisable();

	return !(ret & 0x80);
}

INT8U FlashReadStatusReg()
{
	INT8U ret;

	FlashEnable();

	U1TXBUF = OPCODE_READ_STATUS_REG;

	while (!(IFG2 & UTXIFG1));
	U1TXBUF = 0;
	while (!(IFG2 & URXIFG1));
	ret = U1RXBUF;

	FlashDisable();

	return ret;
}

/* 相同返回1，不同返回0 */
INT8U FlashCmpMemPageWithBuf(INT16U pageaddr, INT8U bufid)
{
	char ret;
	INT32U addr = pageaddr << 11;

	FlashEnable();

	if (bufid == FLASHBUFFER1) {
		FlashReadyToPerform(OPCODE_CMP_MEM_WITH_BUF1, addr, FLASH_READ, 0);
	} else {
		FlashReadyToPerform(OPCODE_CMP_MEM_WITH_BUF2, addr, FLASH_READ, 0);
	}

	FlashRecv(&ret, 1);

	FlashFinishPerform(FLASH_READ, 0);

	return !(ret & FLASH_CMP_BIT_MASK);
}

/*
 *********************************************************************************************************
 *                               FLASH MEMORY READ
 *
 * Description: This function is used to read the main memory of the flash
 *
 * Arguments  : addr:  memory address where to store the data
 *              array: pointer to the stored data
 *              len:   the length of the data to be stored
 *
 * Returns    : none
 *
 * write by   : Wen Jianjun
 *
 * data       : 0722 2008
 *
 *********************************************************************************************************
 */
void FlashReadMemoryContinus(INT16U pageaddr, INT16U pageoffset, void *array,
		INT16U len)
{
	INT32U address;

	address =  pageaddr << 11 | pageoffset & 0x7FF;

	FlashReadyToPerform(OPCODE_READ_MEM_CONTINUOUS, address, FLASH_READ, 20);

	FlashRecv((char *) array, len);

	FlashFinishPerform(FLASH_READ, 20);
}

#if 0
void FlashReadMemory(INT16U pageaddr, INT16U pageoffset, void *array, INT16U len)
{
	char bytes[3], cnt;
	INT32U addr = pageaddr << 11 | pageoffset & 0x7FF;

	bytes[0] = addr >> 16 & 0xFF;
	bytes[0] = addr >> 8 & 0xFF;
	bytes[0] = addr & 0xFF;

	Dest  =(char *)array;

	U1TXBUF = 0;
	while(!(TXEPT&U1TCTL));

	FlashEnable();

	while (!(IFG2 & UTXIFG1));
	U1TXBUF = OPCODE_READ_MEM_CONTINUOUS;

	/*send address 3 bytes*/
	for(i=0;i<3;i++) {
		while (!(IFG2 & UTXIFG1));
		U1TXBUF = x;
	}

	/*read a dummy byte*/
	Delay_N_mS(20);
	FlashReadByte(0);

	/*read data from flash*/
	while(len--)
	{
		*Dest++ = FlashReadByte(0);
	}
	//_BIS_SR(GIE);
	Delay_N_mS(20);
	FLASH_DISABLE;
}
#endif

/*
 *********************************************************************************************************
 *                               FLASH BUFFER READ
 *
 * Description: This function is used to read the data from the buffers of the flash
 *
 * Arguments  : offset:  offset address
 *              array: pointer to the stored data
 *              len:   the length of the data to read
 *              buff:  indicate that which buffer to read from(the value should be one
 *                     of these:FLASHBUFFER1,FLASHBUFFER2)
 *
 * Returns    : none
 *
 * write by   : Wen Jianjun
 *
 * data       : 0722 2008
 *
 *********************************************************************************************************
 */
void FlashReadBuffer(INT16U offset, void *array, INT16U len, INT8U buff)
{
	INT32U address = offset & 0x7FF;

	if (buff == FLASHBUFFER1) {
		FlashReadyToPerform(OPCODE_READ_BUF1, address, FLASH_READ, 0);
	} else {
		FlashReadyToPerform(OPCODE_READ_BUF2, address, FLASH_READ, 0);
	}

	FlashRecv((char *) array, len);

	FlashFinishPerform(FLASH_READ, 0);
}

/*
 *********************************************************************************************************
 *                               FLASH BUFFER WRITE
 *
 * Description: This function is used to write the buffers of the flash
 *
 * Arguments  : addr:  memory address where to store the data
 *              array: pointer to the stored data
 *              len:   the length of the data to be stored
 *              buff:  indicate that which buffer to write to (the value should be one
 *                     of these:FLASHBUFFER1,FLASHBUFFER2)
 *
 * Returns    : none
 *
 * write by   : Wen Jianjun
 *
 * data       : 0722 2008
 *
 *********************************************************************************************************
 */
void FlashWriteBuffer(INT16U offset, void *array, INT16U len, INT8U buff)
{
	INT32U address = offset & 0x7FF;

	/*send command*/
	if (buff == FLASHBUFFER1) {
		FlashReadyToPerform(OPCODE_WRITE_BUF1, address, FLASH_WRITE, 0);
	} else {
		FlashReadyToPerform(OPCODE_WRITE_BUF2, address, FLASH_WRITE, 0);
	}

	/*write data to flash buffer*/
	FlashSend(array, len);

	FlashFinishPerform(FLASH_WRITE, 0);
}

/*
 *********************************************************************************************************
 *                               FLASH MEMORY WRITE
 *
 * Description: This function is used to program the memory of the flash through the buffers
 *
 * Arguments  : addr:  memory address where to store the data
 *              array: pointer to the stored data
 *              len:   the length of the data to be stored
 *              buff:  indicate that which buffer to be used (the value should be one
 *                     of these:FLASHBUFFER1,FLASHBUFFER2)
 *
 * Returns    : none
 *
 * write by   : Wen Jianjun
 *
 * data       : 0722 2008
 *
 *********************************************************************************************************
 */
void FlashWriteMemoryThroughBuffer(INT16U pageaddr, INT16U bufoffset,
		void *array, INT16U len, INT8U buff)
{
	INT32U address;

	address = pageaddr << 11 | bufoffset & 0x7FF;

	/*send command*/
	if (buff == FLASHBUFFER1) {
		FlashReadyToPerform(OPCODE_MEM_WRITE_THBUF1, address, FLASH_WRITE, 20);
	} else {
		FlashReadyToPerform(OPCODE_MEM_WRITE_THBUF2, address, FLASH_WRITE, 20);
	}

	/*write data to flash memory*/
	FlashSend(array, len);

	FlashFinishPerform(FLASH_WRITE, 2500);
}

void FlashWriteMemory(INT16U pageaddr, void *array, INT16U len)
{
	FlashWriteBuffer(0, array, len, FLASHBUFFER2);
	FlashBufferToMemoryWithErase(pageaddr, FLASHBUFFER2);
}

void FlashMemoryToBufferTransfer(INT16U pageaddr, INT8U buff)
{
	INT32U address = pageaddr << 11;

	if (buff == FLASHBUFFER1) {
		FlashReadyToPerform(OPCODE_MEM_TO_BUF1_TRANSFER, address, FLASH_READ, 0);
	} else {
		FlashReadyToPerform(OPCODE_MEM_TO_BUF2_TRANSFER, address, FLASH_READ, 0);
	}

	while (!(TXEPT & U1TCTL));
	FlashFinishPerform(FLASH_READ, 0);
	//while(FlashIsBusy());
	Delay_N_mS(2500);
}

void FlashBufferToMemoryWithErase(INT16U pageaddr, INT8U buff)
{
	INT32U address = pageaddr << 11;

	if (buff == FLASHBUFFER1) {
		FlashReadyToPerform(OPCODE_BUF1_TO_MEM_WITH_ERASE, address, FLASH_WRITE, 0);
	} else {
		FlashReadyToPerform(OPCODE_BUF2_TO_MEM_WITH_ERASE, address, FLASH_WRITE, 0);
	}

	FlashFinishPerform(FLASH_WRITE, 2500);
}

void FlashClearMemory(INT16U pageaddr, INT16U pageoffset, INT16U len)
{
	INT32U address = pageaddr << 11 | pageoffset & 0x7FF;

	FlashReadyToPerform(OPCODE_MEM_WRITE_THBUF1, address, FLASH_WRITE, 20);

	while (len--) {
		while (!(IFG2 & UTXIFG1));
		U1TXBUF = 0;
	}

	FlashFinishPerform(FLASH_WRITE, 2500);
}

void FlashMemoryErasePage(INT16U pageaddr, INT16U pagecnt)
{
	INT32U addr, cnt;

	for (cnt = 0; cnt < pagecnt; cnt++) {
		addr = (INT32U) ((pageaddr + cnt) << 11);
		FlashReadyToPerform(OPCODE_ERASE_MEM_PAGE, addr, FLASH_WRITE, 0);
		FlashFinishPerform(FLASH_READ, 8);
	}
}

void FlashMemoryEraseBlock(INT16U blockaddr, INT16U blockcnt)
{
	INT32U addr, cnt;

	for (cnt = 0; cnt < blockcnt; cnt++) {
		addr = (INT32U) ((blockaddr + cnt) << 14);
		FlashReadyToPerform(OPCODE_ERASE_MEM_BLOCK, addr, FLASH_WRITE, 0);
		FlashFinishPerform(FLASH_READ, 12);
	}
}
