#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "ad.h"
#include "dio.h"
#include "uart.h"
#include "flash.h"
#include "pro212.h"
#include "global.h"
#include "modbus.h"
#include "prodata.h"
#include "at24c64.h"

//=====================================
//���������Ҫ������
//=====================================
void CalcControlData(void)
{
	static int SaveCnt = 0;

	if (++SaveCnt >= 300) {
		SaveCnt = 0;
		//Save_FlowCount(&Flow_Count);
	}
}

//=====================================
//����ͳ������
//=====================================
void CalcStatisData(void)
{
	int i;

	//�������ֵ
	for (i = 0; i < 8; ++i) {
		//��������
		if (0 == Ad_MaxMin[i].mnt_cnt) {
			Ad_MaxMin[i].mnt_max = Ad_RealData[i];
			Ad_MaxMin[i].mnt_min = Ad_RealData[i];
		} else {
			if (Ad_RealData[i] > Ad_MaxMin[i].mnt_max)
				Ad_MaxMin[i].mnt_max = Ad_RealData[i];
			if (Ad_RealData[i] < Ad_MaxMin[i].mnt_min)
				Ad_MaxMin[i].mnt_min = Ad_RealData[i];
		}
		Ad_MaxMin[i].mnt_sum += Ad_RealData[i];
		Ad_MaxMin[i].mnt_cnt++;
		Ad_MaxMin[i].mnt_avg = Ad_MaxMin[i].mnt_sum / Ad_MaxMin[i].mnt_cnt;
		Ad_MaxMin[i].mnt_cou += Ad_RealData[i];

		//Сʱ����
		if (0 == Ad_MaxMin[i].hour_cnt) {
			Ad_MaxMin[i].hour_max = Ad_RealData[i];
			Ad_MaxMin[i].hour_min = Ad_RealData[i];
		} else {
			if (Ad_RealData[i] > Ad_MaxMin[i].hour_max)
				Ad_MaxMin[i].hour_max = Ad_RealData[i];
			if (Ad_RealData[i] < Ad_MaxMin[i].hour_min)
				Ad_MaxMin[i].hour_min = Ad_RealData[i];
		}

		Ad_MaxMin[i].hour_sum += Ad_RealData[i];
		Ad_MaxMin[i].hour_cnt++;
		Ad_MaxMin[i].hour_avg = Ad_MaxMin[i].hour_sum / Ad_MaxMin[i].hour_cnt;
		Ad_MaxMin[i].hour_cou += Ad_RealData[i];

		//������
		if (0 == Ad_MaxMin[i].day_cnt) {
			Ad_MaxMin[i].day_max = Ad_RealData[i];
			Ad_MaxMin[i].day_min = Ad_RealData[i];
		} else {
			if (Ad_RealData[i] > Ad_MaxMin[i].day_max)
				Ad_MaxMin[i].day_max = Ad_RealData[i];
			if (Ad_RealData[i] < Ad_MaxMin[i].day_min)
				Ad_MaxMin[i].day_min = Ad_RealData[i];
		}
		Ad_MaxMin[i].day_sum += Ad_RealData[i];
		Ad_MaxMin[i].day_cnt++;
		Ad_MaxMin[i].day_avg = Ad_MaxMin[i].day_sum / Ad_MaxMin[i].day_cnt;
		Ad_MaxMin[i].day_cou += Ad_RealData[i];
	}

	for (i = 0; i < MODBUS_VARNUM; ++i) {
		//��������
		if (0 == Com_MaxMin[i].mnt_cnt) {
			Com_MaxMin[i].mnt_max = Com_Val[i];
			Com_MaxMin[i].mnt_min = Com_Val[i];
		} else {
			if (Com_Val[i] > Com_MaxMin[i].mnt_max)
				Com_MaxMin[i].mnt_max = Com_Val[i];
			if (Com_Val[i] < Com_MaxMin[i].mnt_min)
				Com_MaxMin[i].mnt_min = Com_Val[i];
		}
		Com_MaxMin[i].mnt_sum += Com_Val[i];
		Com_MaxMin[i].mnt_cnt++;
		Com_MaxMin[i].mnt_avg = Com_MaxMin[i].mnt_sum / Com_MaxMin[i].mnt_cnt;
		Com_MaxMin[i].mnt_cou += Com_Val[i];

		//Сʱ����
		if (0 == Com_MaxMin[i].hour_cnt) {
			Com_MaxMin[i].hour_max = Com_Val[i];
			Com_MaxMin[i].hour_min = Com_Val[i];
		} else {
			if (Com_Val[i] > Com_MaxMin[i].hour_max)
				Com_MaxMin[i].hour_max = Com_Val[i];
			if (Com_Val[i] < Com_MaxMin[i].hour_min)
				Com_MaxMin[i].hour_min = Com_Val[i];
		}
		Com_MaxMin[i].hour_sum += Com_Val[i];
		Com_MaxMin[i].hour_cnt++;
		Com_MaxMin[i].hour_avg = Com_MaxMin[i].hour_sum
				/ Com_MaxMin[i].hour_cnt;
		Com_MaxMin[i].hour_cou += Com_Val[i];

		//������
		if (0 == Com_MaxMin[i].day_cnt) {
			Com_MaxMin[i].day_max = Com_Val[i];
			Com_MaxMin[i].day_min = Com_Val[i];
		} else {
			if (Com_Val[i] > Com_MaxMin[i].day_max)
				Com_MaxMin[i].day_max = Com_Val[i];
			if (Com_Val[i] < Com_MaxMin[i].day_min)
				Com_MaxMin[i].day_min = Com_Val[i];
		}
		Com_MaxMin[i].day_sum += Com_Val[i];
		Com_MaxMin[i].day_cnt++;
		Com_MaxMin[i].day_avg = Com_MaxMin[i].day_sum / Com_MaxMin[i].day_cnt;
		Com_MaxMin[i].day_cou += Com_Val[i];
	}
}

//=====================================
//���ͳ������, 
//CmdΪ212Э��������
//=====================================
void ClearStatisData(int Cmd)
{
	char i;

	if (2051 == Cmd) {
		for (i = 0; i < 8; ++i) {
			Ad_MaxMin[i].mnt_min = 0;
			Ad_MaxMin[i].mnt_max = 0;
			Ad_MaxMin[i].mnt_sum = 0;
			Ad_MaxMin[i].mnt_avg = 0;
			Ad_MaxMin[i].mnt_cou = 0;
			Ad_MaxMin[i].mnt_cnt = 0;
		}
		for (i = 0; i < MODBUS_VARNUM; ++i) {
			Com_MaxMin[i].mnt_min = 0;
			Com_MaxMin[i].mnt_max = 0;
			Com_MaxMin[i].mnt_sum = 0;
			Com_MaxMin[i].mnt_avg = 0;
			Com_MaxMin[i].mnt_cou = 0;
			Com_MaxMin[i].mnt_cnt = 0;
		}
	} else if (2061 == Cmd) {
		for (i = 0; i < 8; ++i) {
			Ad_MaxMin[i].hour_min = 0;
			Ad_MaxMin[i].hour_max = 0;
			Ad_MaxMin[i].hour_sum = 0;
			Ad_MaxMin[i].hour_avg = 0;
			Ad_MaxMin[i].hour_cou = 0;
			Ad_MaxMin[i].hour_cnt = 0;
		}
		for (i = 0; i < MODBUS_VARNUM; ++i) {
			Com_MaxMin[i].hour_min = 0;
			Com_MaxMin[i].hour_max = 0;
			Com_MaxMin[i].hour_sum = 0;
			Com_MaxMin[i].hour_avg = 0;
			Com_MaxMin[i].hour_cou = 0;
			Com_MaxMin[i].hour_cnt = 0;
		}
	} else if (2031 == Cmd) {
		for (i = 0; i < 8; ++i) {
			Ad_MaxMin[i].day_min = 0;
			Ad_MaxMin[i].day_max = 0;
			Ad_MaxMin[i].day_sum = 0;
			Ad_MaxMin[i].day_avg = 0;
			Ad_MaxMin[i].day_cou = 0;
			Ad_MaxMin[i].day_cnt = 0;
		}
		for (i = 0; i < MODBUS_VARNUM; ++i) {
			Com_MaxMin[i].day_min = 0;
			Com_MaxMin[i].day_max = 0;
			Com_MaxMin[i].day_sum = 0;
			Com_MaxMin[i].day_avg = 0;
			Com_MaxMin[i].day_cou = 0;
			Com_MaxMin[i].day_cnt = 0;
		}
	}
}

//=====================================
//��Ⱦ�����ݴ�����
//=====================================
void ProcData(void)
{
	static int LastSecond = 0;
	int secnow = Current_Tm.second;

	if (LastSecond == secnow)
		return;

	LastSecond = secnow;

	//ÿ�����һ������
	CalcControlData();
	CalcStatisData();
}
