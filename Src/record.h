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
 * ÿ��ͨ������ʷ�����߼�¼�ṹ
 * mn:ͨ����Ӧ��mn��
 * cnt:��ͨ���е���ʷ��¼����
 * head:ָ�����¼�¼�ṹ��ָ��,Ϊ-1��ʾ��¼Ϊ�ա�
 * datetime��������ʱ���
 * flag:�����߱�ǣ�Ϊ'D'��ʾ���ߣ�Ϊ'O'��ʾ����
 * prev:�౾��¼�������һ����¼ָ��,-1��ʾΪ�ա�
 * next:�౾��¼�������һ����¼ָ��,-1��ʾΪ�ա�
 */

#define MAX_RECORD_ITEM_NUM 30
#define OFFSET_OF(TYPE, MEMBER) ((int)&(((TYPE *)0)->MEMBER))

typedef struct {
	char datetime[14];
	unsigned char inuse;
	unsigned char drop_status;	//����״̬
	unsigned char over_status;	//����״̬
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
