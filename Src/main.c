#include <string.h>
#include <stdio.h>

#include "dio.h"
#include "lcd.h"
#include "uart.h"
#include "flash.h"
#include "record.h"
#include "ds1302.h"
#include "global.h"
#include "pro212.h"
#include "at24c64.h"
#include "display.h"
#include "control.h"
#include "modbusio.h"
#include "cpu_init.h"
#include "sc16is752.h"

//硬件操作宏定义
#define RUN_ON        P7OUT&=~BIT4
#define RUN_OFF       P7OUT|=BIT4

void ProcRunLED(void)
{
	static char run = 0;

	if (LEDTimer >= S2TENMS(1)) {
		LEDTimer = 0;
		if (run == 0) {
			run = 1;
			RUN_ON;
		} else {
			run = 0;
			RUN_OFF;
		}
	}
}

#define BEEP_MASK					(1 << 2)
#define RUN_INDICATOR_LIGHT_MASK 	(1 << 5)
#define DROP_INDICATOR_LIGHT_MASK	(1 << 6)
#define OVER_INDICATOR_LIGHT_MASK	(1 << 7)

void ProcAlarm(void)
{
	INT8U cnt;
	INT8U drop_flag = 0, over_flag = 0;

	DO_Val |= RUN_INDICATOR_LIGHT_MASK;

	for (cnt = 0; cnt < MAX_ALARM_NUM; cnt++) {
		if (Alarm_Param[cnt].inuse) {
			if (Alarm_Param[cnt].drop_status) {
				drop_flag = 1;
			}

			if (Alarm_Param[cnt].over_status) {
				over_flag = 1;
			}
		}
	}

	if (drop_flag) {
		DO_Val |= DROP_INDICATOR_LIGHT_MASK;
	} else {
		DO_Val &= ~DROP_INDICATOR_LIGHT_MASK;
	}

	if (over_flag) {
		DO_Val |= OVER_INDICATOR_LIGHT_MASK;
	} else {
		DO_Val &= ~OVER_INDICATOR_LIGHT_MASK;
	}

	if (BeepEnable && (drop_flag || over_flag)) {
		DO_Val |= BEEP_MASK;
	} else {
		DO_Val &= ~BEEP_MASK;
	}

	return;
}

void main(void)
{
	static char code[6];

	CPU_Init();
	Flash_Init();
	GPRS_Init();
	LCD_Init();
	Clr_LCDRam();
	LCD_LIGHT_ON;
	Dis_Logo();

	//#define FLASH_DEBUG
#ifdef FLASH_DEBUG
	while (1) {
		char checknum[32];
		INT16U cnt, offset;
		RECORD_HEAD_T head;
		RECORD_ITEM_T item;

		Format_AllHistoryRecord();

		for (cnt = 0; cnt < MAX_ALARM_NUM; cnt++) {
			FlashMemoryToBufferTransfer(cnt, FLASHBUFFER1);
			memset(&head, 0, sizeof(RECORD_HEAD_T));
			offset = OFFSET_OF(RECORD_T, info);
			FlashReadBuffer(offset, &head, sizeof(RECORD_HEAD_T), FLASHBUFFER1);
		}

		memset(&head, 0, sizeof(RECORD_HEAD_T));
		sprintf(head.mn, "98765432101234");
		Format_HistoryRecord(0, &head);

		FlashMemoryToBufferTransfer(0, FLASHBUFFER1);
		memset(&head, 0, sizeof(RECORD_HEAD_T));
		offset = OFFSET_OF(RECORD_T, info);
		FlashReadBuffer(offset, &head, sizeof(RECORD_HEAD_T), FLASHBUFFER1);

		memset(&item, 0, sizeof(RECORD_ITEM_T));
		sprintf(item.datetime, "12345678901234");
		item.inuse = 1;
		item.status = 'O';
		Add_HisRecordItem(&item, 0);

		FlashMemoryToBufferTransfer(0, FLASHBUFFER1);
		memset(&head, 0, sizeof(RECORD_HEAD_T));
		offset = OFFSET_OF(RECORD_T, info);
		FlashReadBuffer(offset, &head, sizeof(RECORD_HEAD_T), FLASHBUFFER1);

		Fetch_HisRecord(0);
		memset(&item, 0, sizeof(RECORD_ITEM_T));
		Get_HisRecordItemsBackward(&item, 1);

		FlashWriteBuffer(0, "asdfghjklqwertyuiop", 32, FLASHBUFFER1);
		memset(checknum, 0, sizeof(checknum));
		FlashReadBuffer(0, checknum, 32, FLASHBUFFER1);

		FlashBufferToMemoryWithErase(0, FLASHBUFFER1);
		memset(checknum, 0, sizeof(checknum));
		FlashReadBuffer(0, checknum, 32, FLASHBUFFER1);

		FlashMemoryToBufferTransfer(0, FLASHBUFFER1);
		memset(checknum, 0, sizeof(checknum));
		FlashReadBuffer(0, checknum, 32, FLASHBUFFER1);

		FlashReadMemoryContinus(0, 0, checknum, 32);
		memset(checknum, 0, sizeof(checknum));
		FlashReadBuffer(0, checknum, 32, FLASHBUFFER1);

		//FlashMemoryErasePage(0, 1);
		FlashClearMemory(0, 0, 32);
		memset(checknum, 0, sizeof(checknum));
		FlashReadBuffer(0, checknum, 8, FLASHBUFFER1);

		FlashWriteMemoryThroughBuffer(0, 0, "ABCDEFGHIJKLMNOP", 8, FLASHBUFFER1);
		memset(checknum, 0, sizeof(checknum));
		FlashReadBuffer(0, checknum, 8, FLASHBUFFER1);
	}
#endif

	Read_VersionCode(code);
	if (memcmp(code, VersionCode, 6) == 0) {
		Read_SysParam(&Sys_Param);
		Read_ADParam(Ad_Param);
		Read_ComParam(&Com_Param);
		Read_AlarmParam(Alarm_Param);
	} else {
		Default_SysParam(&Sys_Param);
		Default_ADParam(Ad_Param);
		Default_ComParam(&Com_Param);
		Default_AlarmParam(Alarm_Param);
		Save_SysParam(&Sys_Param);
		Save_ADParam(Ad_Param);
		Save_ComParam(&Com_Param);
		Save_AlarmParam(Alarm_Param);
		Save_VersionCode((char *) VersionCode);
		Format_AllHistoryRecord();
	}
	Read_ADCalibParam(&Ad_CalibParam);
	Fix_ADCalibParam(&Ad_CalibParam);
	ADParam_Calculate();

	v_Get1302(System_TimeStr, &Current_Tm);
	strcpy((char *) System_StartTimeStr, (char *) System_TimeStr);
	Dis_Welcome(1);

	IS752_Init(9600, COM2);
	IS752_Init(9600, COM3);
	IS752_Init(9600, COM4);
	IS752_Init(9600, COM5);
	Modbus_Init();
	ModbusIO_Init();
	InitHjt212();
	ENABLEINT();

	while (1) {
		SoftWDT_Flag = 0;
		ProcRunLED();
		ProcAlarm();
		ProcRTC();
		ProcDIO();
		//ProcADC();
		ProcessModbusSlave();
		//ProcessModbus();
		//ProcessModbusIO();
		//ProcData();
		ProcDisplay();
		ProcessHjt212();
	}
}
