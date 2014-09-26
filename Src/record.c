/*
 * record.c
 *
 *  Created on: 2014-8-12
 *      Author: Administrator
 */

#include "record.h"
#include "global.h"
#include <string.h>
#include <stdio.h>

static RECORD_HEAD_T His_Record_Head;

void Erase_AllRecords()
{
	FlashMemoryErasePage(0, 8);
}

void Format_HistoryRecord(INT8U chl, RECORD_HEAD_T *info)
{
	FlashWriteBuffer(OFFSET_OF(RECORD_T, info), info, sizeof(RECORD_HEAD_T), FLASHBUFFER1);
	FlashBufferToMemoryWithErase(chl, FLASHBUFFER1);
}

void Format_AllHistoryRecord()
{
	INT16U cnt, pos, offset;
	RECORD_HEAD_T record_head;
	RECORD_ITEM_T record_item;

	memset(&record_head, 0, sizeof(RECORD_HEAD_T));
	sprintf(record_head.mn, "00000000000000");
	offset = OFFSET_OF(RECORD_T, info);
	FlashWriteBuffer(offset, &record_head, sizeof(RECORD_HEAD_T), FLASHBUFFER1);

	memset(&record_item, 0, sizeof(RECORD_ITEM_T));
	sprintf(record_item.datetime, "20140101000000");
	record_item.drop_status = 'O';
	record_item.over_status = 'N';

	for (cnt = 0; cnt < MAX_RECORD_ITEM_NUM; cnt++) {
		record_item.index = cnt;
		offset = OFFSET_OF(RECORD_T, item[cnt]);
		FlashWriteBuffer(offset, &record_item, sizeof(RECORD_ITEM_T), FLASHBUFFER1);
	}

	for (cnt = 0; cnt < MAX_ALARM_NUM; cnt++) {
		FlashBufferToMemoryWithErase(cnt, FLASHBUFFER1);
	}

#if 0
	for (cnt = 0; cnt < 8; cnt++) {
		FlashMemoryToBufferTransfer(cnt, FLASHBUFFER1);
		FlashReadBuffer(0, &record_head, sizeof(record_head), FLASHBUFFER1);
		for (pos = 0; pos < MAX_RECORD_ITEM_NUM; pos++) {
			FlashReadBuffer(OFFSET_OF(RECORD_T, item[cnt]), &record_item, sizeof(record_item), FLASHBUFFER1);
		}
		FlashReadMemoryContinus(cnt, 0, &record_head, sizeof(RECORD_HEAD_T));
	}
#endif
}

void Fetch_HisRecord(INT8U chl)
{
	INT32U pageaddr = chl;
	INT32U offset = OFFSET_OF(RECORD_T, info);

	FlashMemoryToBufferTransfer(pageaddr, FLASHBUFFER1);
	FlashReadBuffer(offset, &His_Record_Head, sizeof(RECORD_HEAD_T), FLASHBUFFER1);

	His_Record_Head.cursor = (His_Record_Head.headpos - 1) % MAX_RECORD_ITEM_NUM;
	His_Record_Head.itemcnt = (His_Record_Head.headpos + MAX_RECORD_ITEM_NUM - His_Record_Head.tailpos) % MAX_RECORD_ITEM_NUM;
}

INT8U Get_HisRecordItemCnt()
{
	INT8U ret;

	ret = (His_Record_Head.headpos + MAX_RECORD_ITEM_NUM - His_Record_Head.tailpos) % MAX_RECORD_ITEM_NUM;

	return ret;
}

INT8U Get_HisRecordItemsForward(RECORD_ITEM_T *item, unsigned char cnt)
{
	INT32U offset;
	INT8U pos, i, totalcnt;

	totalcnt = (His_Record_Head.headpos + MAX_RECORD_ITEM_NUM - His_Record_Head.tailpos) % MAX_RECORD_ITEM_NUM;
	if (cnt > totalcnt - His_Record_Head.itemcnt) {
		His_Record_Head.cursor = (His_Record_Head.headpos - 1 + MAX_RECORD_ITEM_NUM) % MAX_RECORD_ITEM_NUM;
		His_Record_Head.itemcnt = totalcnt;
		if (cnt > totalcnt) {
			cnt = totalcnt;
		}
	} else {
		His_Record_Head.cursor = (His_Record_Head.cursor + cnt) % MAX_RECORD_ITEM_NUM;
		His_Record_Head.itemcnt += cnt;
	}

	memset(item, 0, sizeof(RECORD_ITEM_T) * cnt);

	pos = His_Record_Head.cursor;
	for (i = 0; i < cnt; i++) {
		offset = OFFSET_OF(RECORD_T, item[pos]);
		FlashReadBuffer(offset, &item[i], sizeof(RECORD_ITEM_T), FLASHBUFFER1);
		if (pos == 0) {
			pos = MAX_RECORD_ITEM_NUM - 1;
		} else {
			pos--;
		}
	}

	return cnt;
}

INT8U Get_HisRecordItemsBackward(RECORD_ITEM_T *item, unsigned char cnt)
{
	INT32U offset;
	INT8U i, pos;

	pos = His_Record_Head.cursor;

	if (cnt > His_Record_Head.itemcnt) {
		cnt = His_Record_Head.itemcnt;
	} else {
		His_Record_Head.cursor = (His_Record_Head.cursor - cnt + MAX_RECORD_ITEM_NUM) % MAX_RECORD_ITEM_NUM;
		His_Record_Head.itemcnt -= cnt;
	}

	memset(item, 0, sizeof(RECORD_ITEM_T) * cnt);

	for (i = 0; i < cnt; i++) {
		offset = OFFSET_OF(RECORD_T, item[pos]);
		FlashReadBuffer(offset, &item[i], sizeof(RECORD_ITEM_T), FLASHBUFFER1);
		pos = (pos - 1 + MAX_RECORD_ITEM_NUM) % MAX_RECORD_ITEM_NUM;
	}

	return cnt;
}

void Add_HisRecordItem(RECORD_ITEM_T *item, INT8U chl)
{
	INT32U pageaddr = chl;
	INT32U offset = OFFSET_OF(RECORD_T, info);
	RECORD_HEAD_T head;

	FlashMemoryToBufferTransfer(pageaddr, FLASHBUFFER1);
	memset(&head, 0, sizeof(RECORD_HEAD_T));
	FlashReadBuffer(offset, &head, sizeof(RECORD_HEAD_T), FLASHBUFFER1);

	item->index = head.headpos;
	offset = OFFSET_OF(RECORD_T, item[head.headpos]);
	FlashWriteBuffer(offset, item, sizeof(RECORD_ITEM_T), FLASHBUFFER1);

	head.headpos = (head.headpos + 1) % MAX_RECORD_ITEM_NUM;
	head.itemcnt = (head.headpos + MAX_RECORD_ITEM_NUM - head.tailpos) % MAX_RECORD_ITEM_NUM;
	if (head.itemcnt == 0) {
		head.tailpos = (head.tailpos + 1) % MAX_RECORD_ITEM_NUM;
	}
	offset = OFFSET_OF(RECORD_T, info);
	FlashWriteBuffer(offset, &head, sizeof(RECORD_HEAD_T), FLASHBUFFER1);

	FlashBufferToMemoryWithErase(pageaddr, FLASHBUFFER1);
}
