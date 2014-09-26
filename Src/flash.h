#ifndef _FLASH_H
#define _FLASH_H

#include "type.h"

//flash地址生成，pa页地址，ba页内地址
#define FLASHADDR(pa,ba)  	((((u32)(pa))<<11)|((u32)ba))
#define FLASHBUFFER1       	0X00
#define FLASHBUFFER2     	0X01
#define PAGEADDR(x)			((x)<<11 & 0x7FF)

void Flash_Init();
void FlashReadBuffer(INT16U offset, void *array, INT16U len, INT8U buff);
void FlashWriteBuffer(INT16U offset, void *array, INT16U len, INT8U buff);
void FlashReadMemoryContinus(INT16U pageaddr, INT16U pageoffset, void *array, INT16U len);
void FlashReadMemory(INT32U addr, void *array, INT16U len);
void FlashWriteMemory(INT16U pageaddr, void *array, INT16U len);
void FlashWriteMemoryThroughBuffer(INT16U pageaddr, INT16U bufoffset, void *array, INT16U len, INT8U buff);
void FlashMemoryToBufferTransfer(INT16U pageaddr, INT8U buff);
void FlashBufferToMemoryWithErase(INT16U pageaddr, INT8U buff);
void FlashClearMemory(INT16U pageaddr, INT16U pageoffset, INT16U len);
void FlashMemoryErasePage(INT16U pageaddr, INT16U pagecnt);
void FlashMemoryEraseBlock(INT16U blockaddr, INT16U blockcnt);
INT8U FlashIsBusy();
INT8U FlashReadStatusReg();
INT8U FlashCmpMemPageWithBuf(INT16U pageaddr, INT8U bufid);

#endif   //end of _FLASH_H
