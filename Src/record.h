/*
 * record.h
 *
 *  Created on: 2014-8-12
 *      Author: Administrator
 */

#ifndef RECORD_H_
#define RECORD_H_

#include "flash.h"

/*
 * 每个通道的历史上下线记录结构
 * mn:通道对应的mn号
 * cnt:该通道中的历史记录数量
 * head:指向最新记录结构的指针,为-1表示记录为空。
 * datetime：上下线时间戳
 * flag:上下线标记，为'D'表示下线，为'O'表示上线
 * prev:距本记录最近的上一条记录指针,-1表示为空。
 * next:距本记录最近的下一条记录指针,-1表示为空。
 */

#define MAX_RECORD_ITEM_NUM 30
#define OFFSET_OF(TYPE, MEMBER) ((int)&(((TYPE *)0)->MEMBER))

typedef struct {
	char datetime[14];
	unsigned char inuse;
	unsigned char drop_status;	//掉线状态
	unsigned char over_status;	//超标状态
	unsigned int index;
	unsigned int prev;
	unsigned int next;
} RECORD_ITEM_T;

typedef struct {
	char mn[14];
	unsigned char itemcnt;
	unsigned char headpos;
	unsigned char tailpos;
	unsigned char cursor;
} RECORD_HEAD_T;

typedef struct {
	RECORD_HEAD_T info;
	RECORD_ITEM_T item[MAX_RECORD_ITEM_NUM];
} RECORD_T;

void Format_HistoryRecord(INT8U chl, RECORD_HEAD_T *info);
void Format_AllHistoryRecord();
void Fetch_HisRecord(INT8U chl);
INT8U Get_HisRecordItemCnt();
INT8U Get_HisRecordItemsForward(RECORD_ITEM_T *item, unsigned char cnt);
INT8U Get_HisRecordItemsBackward(RECORD_ITEM_T *item, unsigned char cnt);
void Add_HisRecordItem(RECORD_ITEM_T *item, INT8U chl);
void Erase_AllHistoryRecords();

#endif /* RECORD_H_ */
