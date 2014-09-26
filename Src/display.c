#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "lcd.h"
#include "dio.h"
#include "ad.h"
#include "at24c64.h"
#include "control.h"
#include "ds1302.h"
#include "modbus.h"
#include "record.h"

//各个显示画面的索引编号
#define WELCOME_INDEX       0
#define MAINSEL_INDEX       1
#define PWINPUT_INDEX       2
#define STATWATCH_INDEX     3
#define LOGWATCH_INDEX      4
#define	ALARMSETUP_INDEX	5
#define SYSSETUP_INDEX      6
#define LOGLIST_INDEX		7

static char Dis_AllRefresh = 0;     //整屏刷新标志
static char Dis_PartRefresh = 0;    //部分屏幕刷新标志
static char MainDis_Index = 0;      //画面显示索引
static char SndDis_Index = 0;       //子画面显示索引
static char PWLast_Index = 0;       //密码输入之前的窗口索引
//static char ThdDis_Index = 0;       //子子画面索引

static char Input_Temp[24];         //临时输入数据
static char Input_Pos;              //光标位置
static char Input_Type;             //显示状态    

static RECORD_ITEM_T His_Record_Item[5];
static INT8U His_Record_Cnt;

//显示部分
//显示欢迎画面
void Dis_Welcome(INT8U AllRefresh)
{
	INT8U indexp[12];
	INT8U k;

	static INT8U index_wel[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0 };
	static INT8U index_date[] = { 11, 12, 0 };
	static INT8U index_time[] = { 13, 14, 0 };

	if (AllRefresh) {
		//欢迎字符串
		Clr_LCDRam();
		Disp_Chn1616_Str(4, 35, index_wel, 1);
		Disp_Xline(52);
		Disp_Xline(54);

		//版本号
		if ((VersionCode[0] == 'V') && (VersionCode[1] == 'A')) {
			indexp[0] = 16;
			indexp[1] = 22;
			indexp[2] = (VersionCode[2] & 0x0f) + 1;
			indexp[3] = (VersionCode[3] & 0x0f) + 1;
			indexp[4] = 0;
		} else if ((VersionCode[0] == 'V') && (VersionCode[1] == 'D')) {
			indexp[0] = 16;
			indexp[1] = 21;
			indexp[2] = (VersionCode[2] & 0x0f) + 1;
			indexp[3] = (VersionCode[3] & 0x0f) + 1;
			indexp[4] = 0;
		} else if ((VersionCode[0] == 'V') && (VersionCode[1] == 'T')) {
			indexp[0] = 16;
			indexp[1] = 32;
			indexp[2] = (VersionCode[2] & 0x0f) + 1;
			indexp[3] = (VersionCode[3] & 0x0f) + 1;
			indexp[4] = 0;
		} else {
			indexp[0] = 26;
			indexp[1] = 26;
			indexp[2] = 26;
			indexp[3] = 26;
			indexp[4] = 0;
		}
		Disp_Chn816_Str(26, 55, indexp, 1);

		Disp_Chn1616_Str(8, 80, index_date, 1);
		Disp_Chn1616_Str(8, 98, index_time, 1);

		//：号和-号
		Disp_Chn816_Char(18, 80, 13, 1);
		Disp_Chn816_Char(21, 80, 13, 1);
		Disp_Chn816_Char(16, 98, 14, 1);
		Disp_Chn816_Char(19, 98, 14, 1);
	}

	if (!AllRefresh) {
		Clr_RamBlock(14, 80, 4);
		Clr_RamBlock(19, 80, 2);
		Clr_RamBlock(22, 80, 2);
		Clr_RamBlock(14, 98, 2);
		Clr_RamBlock(17, 98, 2);
		Clr_RamBlock(20, 98, 2);
	}

	//日期和时间的数值
	for (k = 0; k < 4; k++)
		indexp[k] = (System_TimeStr[k] & 0x0f) + 1;
	indexp[4] = 0;
	Disp_Chn816_Str(14, 80, indexp, 1);
	for (k = 0; k < 2; k++)
		indexp[k] = (System_TimeStr[k + 4] & 0x0f) + 1;
	indexp[2] = 0;
	Disp_Chn816_Str(19, 80, indexp, 1);
	for (k = 0; k < 2; k++)
		indexp[k] = (System_TimeStr[k + 6] & 0x0f) + 1;
	indexp[2] = 0;
	Disp_Chn816_Str(22, 80, indexp, 1);

	for (k = 0; k < 2; k++)
		indexp[k] = (System_TimeStr[k + 8] & 0x0f) + 1;
	indexp[2] = 0;
	Disp_Chn816_Str(14, 98, indexp, 1);
	for (k = 0; k < 2; k++)
		indexp[k] = (System_TimeStr[k + 10] & 0x0f) + 1;
	indexp[2] = 0;
	Disp_Chn816_Str(17, 98, indexp, 1);
	for (k = 0; k < 2; k++)
		indexp[k] = (System_TimeStr[k + 12] & 0x0f) + 1;
	indexp[2] = 0;
	Disp_Chn816_Str(20, 98, indexp, 1);
}

//显示主菜单
void Dis_MainSel(INT8U AllRefresh)
{
	static INT8U index_menu[] = { 15, 16, 17, 18, 0 };
	static INT8U index_watch[] = { 19, 20, 21, 22, 0 };
	static INT8U index_alarm_setup[] = { 86, 124, 29, 30, 0 };
	static INT8U index_sys_setup[] = { 27, 28, 29, 30, 0 };

	if (AllRefresh) {
		SndDis_Index = 0;
		Clr_LCDRam();
		Disp_Chn1616_Str(11, 26, index_menu, 1);
		Disp_Chn1616_Str(11, 46, index_watch, 0);
		Disp_Chn1616_Str(11, 66, index_alarm_setup, 1);
		Disp_Chn1616_Str(11, 86, index_sys_setup, 1);
	} else {
		unsigned char pon[3] = { 1, 1, 1 };
		pon[SndDis_Index % sizeof(pon)] = 0;
		Disp_Chn1616_Str(11, 46, index_watch, pon[0]);
		Disp_Chn1616_Str(11, 66, index_alarm_setup, pon[1]);
		Disp_Chn1616_Str(11, 86, index_sys_setup, pon[2]);
	}
}

//设备状态
void Dis_StatWatch(INT8U AllRefresh)
{
	static INT8U index_stat[] = { 19, 20, 0 };
	static INT8U index_his[] = { 36, 37, 0 };
	static INT8U index_chl[] = { 65, 125, 0 };
	static INT8U index_normal[] = { 127, 128, 0 };
	static INT8U index_drop[] = { 110, 126, 0};
	static INT8U index_over[] = {114, 84, 0};
	static INT8U index_close[] = { 120, 121, 0 };

	int row, col, pos_x, pos_y, cnt;
	if (AllRefresh) {
		//显示固定不变的内容
		Clr_LCDRam();
		Disp_Yline(4, 0);
		Disp_Chn1616_Str(0, 15, index_stat, 0);
		Disp_Chn1616_Str(0, 35, index_his, 1);

		for (col = 0; col < 2; col++) {
			for (row = 0; row < 4; row++) {
				pos_x = col * 14 + 5;
				pos_y = row * 26 + 17;
				cnt = row + col * 4 + 1;
				Disp_Chn1616_Str(pos_x, pos_y, index_chl, 1);
				Disp_Chn816_Char(pos_x + 4, pos_y, cnt, 1);
				Disp_Chn816_Char(pos_x + 5, pos_y, 14, 1);
			}
		}
	}

	for (col = 0; col < 2; col++) {
		for (row = 0; row < 4; row++) {
			pos_x = col * 14 + 11;
			pos_y = row * 26 + 17;
			cnt = row + col * 4;
			if (Alarm_Param[cnt].inuse == 0) {
				Disp_Chn1616_Str(pos_x, pos_y, index_close, 1);
			} else {
				if (Alarm_Param[cnt].drop_status != 0) {
					Disp_Chn1616_Str(pos_x, pos_y, index_drop, 1);
				} else if (Alarm_Param[cnt].over_status != 0) {
					Disp_Chn1616_Str(pos_x, pos_y, index_over, 1);
				} else {
					Disp_Chn1616_Str(pos_x, pos_y, index_normal, 1);
				}
			}
		}

	}
}

//LOG查看
void Dis_LogWatch(INT8U AllRefresh)
{
	static INT8U index_stat[] = { 19, 20, 0 };
	static INT8U index_his[] = { 36, 37, 0 };
	static INT8U index_chl[] = { 65, 125, 0 };

	int row, col, pos_x, pos_y, cnt;
	INT8U pon;

	if (AllRefresh) {
		Clr_LCDRam();
		Disp_Yline(4, 0);
		Disp_Chn1616_Str(0, 15, index_stat, 1);
		Disp_Chn1616_Str(0, 35, index_his, 0);
	}

	for (col = 0; col < 2; col++) {
		for (row = 0; row < 4; row++) {
			pos_x = col * 9 + 10;
			pos_y = row * 26 + 17;
			cnt = row + col * 4;

			pon = 1;
			if (Input_Type != 0 && cnt == SndDis_Index) {
				pon = 0;
			}
			Disp_Chn1616_Str(pos_x, pos_y, index_chl, pon);
			Disp_Chn816_Char(pos_x + 4, pos_y, cnt + 1, pon);
		}
	}
}

void Dis_LogList(INT8U AllRefresh)
{
	static INT8U index_chl[] = { 65, 125, 0 };
	static INT8U index_his_record[] = { 36, 37, 28, 31, 0 };
	static INT8U index_line_time[] = { 126, 13, 14, 0 };
	INT8U	pos_y, row, cnt;

	if (AllRefresh) {
		Clr_LCDRam();
		Disp_Chn1616_Str(8, 4, index_chl, 1);
		Disp_Chn816_Char(12, 4, SndDis_Index + 1, 1);
		Disp_Chn1616_Str(14, 4, index_his_record, 1);
		Disp_Xline(24);
		Disp_Xline(46);
		Disp_Xline(66);
		Disp_Xline(86);
		Disp_Xline(106);
		Disp_Xline(126);
	}

	for (row = 0; row < 5; row++) {
		pos_y = 28 + 20 * row;
		if (row < His_Record_Cnt) {
			if (His_Record_Item[row].drop_status == 'O') {
				Disp_Chn1616_Char(1, pos_y, 72, 1);
			} else {
				Disp_Chn1616_Char(1, pos_y, 73, 1);
			}
			Disp_Chn1616_Str(3, pos_y, index_line_time, 1);
			Disp_Chn816_Char(9, pos_y, 14, 1);

			/* 年 */
			for (cnt = 0; cnt < 4; cnt++) {
				Disp_Chn816_Char(10 + cnt, pos_y, His_Record_Item[row].datetime[cnt] - 48, 1);
			}
			Disp_Chn816_Char(14, pos_y, 13, 1);

			/* 月 */
			for (cnt = 4; cnt < 6; cnt++) {
				Disp_Chn816_Char(11 + cnt, pos_y, His_Record_Item[row].datetime[cnt] - 48, 1);
			}
			Disp_Chn816_Char(17, pos_y, 13, 1);

			/* 日 */
			for (cnt = 6; cnt < 8; cnt++) {
				Disp_Chn816_Char(12 + cnt, pos_y, His_Record_Item[row].datetime[cnt] - 48, 1);
			}
			Disp_Chn816_Char(20, pos_y, 11, 1);

			/* 时 */
			for (cnt = 8; cnt < 10; cnt++) {
				Disp_Chn816_Char(13 + cnt, pos_y, His_Record_Item[row].datetime[cnt] - 48, 1);
			}
			Disp_Chn816_Char(23, pos_y, 14, 1);

			/* 分 */
			for (cnt = 10; cnt < 12; cnt++) {
				Disp_Chn816_Char(14 + cnt, pos_y, His_Record_Item[row].datetime[cnt] - 48, 1);
			}
			Disp_Chn816_Char(26, pos_y, 14, 1);

			/* 秒 */
			for (cnt = 12; cnt < 14; cnt++) {
				Disp_Chn816_Char(15 + cnt, pos_y, His_Record_Item[row].datetime[cnt] - 48, 1);
			}
		} else {
			Clr_RamBlock(0, pos_y, LCD_WIDTH);
		}
	}
}

//用户密码
void Dis_PWInput(INT8U AllRefresh)
{
	static INT8U index_pw[] = { 57, 58, 59, 60, 61, 0 };

	char i;

	if (AllRefresh) {
		Clr_LCDRam();
		Disp_Chn1616_Str(10, 40, index_pw, 1);
		Disp_Chn816_Char(10, 64, 12, 0);
		memset(Input_Temp, '0', sizeof(Input_Temp));
		Input_Pos = 0;
	} else {
		//当前输入位置显示实际字符，其它位置显示星号
		Clr_RamBlock(10, 64, 6);
		for (i = 0; i < Input_Pos; ++i)
			Disp_Chn816_Char(10 + i, 64, 12, 1);
		Disp_Chn816_Char(10 + Input_Pos, 64, Input_Temp[Input_Pos] & 0x0f, 0);
	}
}

//系统参数
void Dis_SysSetup(INT8U AllRefresh)
{
	static INT8U index_pw[] = { 63, 64, 60, 61, 0 };
	static INT8U index_time[] = { 63, 64, 13, 14, 0 };
	static INT8U index_sim[] = { 17, 20, 38, 0 };
	static INT8U index_num[] = { 99, 68, 0 };
	static INT8U index_mn[] = { 63, 64, 84, 85, 0 };
	static INT8U index_st[] = { 63, 64, 71, 68, 0 };
	static INT8U index_interval[] = { 65, 66, 14, 87, 0 };

	char i;
	char pon[8];

	memset(pon, 1, sizeof(pon));
	pon[SndDis_Index % sizeof(pon)] = 0;

	if (AllRefresh) {
		Clr_LCDRam();
		Disp_Yline(10, 0);
		Input_Type = 0;
		SndDis_Index = 0;
		Input_Pos = 0;
	}

	Disp_Chn1616_Str(1, 5, index_pw, pon[0]);
	Disp_Chn1616_Str(1, 25, index_time, pon[1]);
	Disp_Chn816_Str(1, 45, index_sim, pon[2]);
	Disp_Chn1616_Str(4, 45, index_num, pon[2]);
	Disp_Chn1616_Str(1, 65, index_mn, pon[3]);
	Disp_Chn1616_Str(1, 85, index_st, pon[4]);
	Disp_Chn1616_Str(1, 105, index_interval, pon[5]);

	//密码
	if ((Input_Type == 1) && (SndDis_Index == 0)) {
		Clr_RamBlock(12, 5, 6);
		for (i = 0; i <= Input_Pos; ++i) {
			if (i == Input_Pos)
				Disp_Chn816_Char(12 + i, 5, Input_Temp[i] & 0x0f, 0);
			else
				Disp_Chn816_Char(12 + i, 5, 12, 1);
		}
	} else {
		for (i = 0; i < 6; ++i)
			Disp_Chn816_Char(12 + i, 5, 12, 1);
	}

	//系统时间
	if ((Input_Type == 1) && (SndDis_Index == 1)) {
		//Clr_RamBlock(12,25,14);
		for (i = 0; i < 14; ++i) {
			if (i == Input_Pos)
				Disp_Chn816_Char(12 + i, 25, Input_Temp[i] & 0x0f, 0);
			else
				Disp_Chn816_Char(12 + i, 25, Input_Temp[i] & 0x0f, 1);
		}
	} else {
		for (i = 0; i < 14; ++i)
			Disp_Chn816_Char(12 + i, 25, System_TimeStr[i] & 0x0f, 1);
	}

	//SIM卡号
	if ((Input_Type == 1) && (SndDis_Index == 2)) {
		//Clr_RamBlock(12,45,14);
		for (i = 0; i < 11; ++i) {
			if (i == Input_Pos)
				Disp_Chn816_Char(12 + i, 45, Input_Temp[i] & 0x0f, 0);
			else
				Disp_Chn816_Char(12 + i, 45, Input_Temp[i] & 0x0f, 1);
		}
	} else {
		for (i = 0; i < 11; ++i)
			Disp_Chn816_Char(12 + i, 45, Sys_Param.sim[i] & 0x0f, 1);
	}

	//MN号
	if ((Input_Type == 1) && (SndDis_Index == 3)) {
		//Clr_RamBlock(12,65,2);
		for (i = 0; i < 14; ++i) {
			if (i == Input_Pos)
				Disp_Chn816_Char(12 + i, 65, Input_Temp[i] & 0x0f, 0);
			else
				Disp_Chn816_Char(12 + i, 65, Input_Temp[i] & 0x0f, 1);
		}
	} else {
		for (i = 0; i < 14; ++i)
			Disp_Chn816_Char(12 + i, 65, Sys_Param.mn[i] & 0x0f, 1);
	}

	//ST编码
	if ((Input_Type == 1) && (SndDis_Index == 4)) {
		//Clr_RamBlock(12,65,3);
		for (i = 0; i < 2; ++i) {
			if (i == Input_Pos)
				Disp_Chn816_Char(12 + i, 85, Input_Temp[i] & 0x0f, 0);
			else
				Disp_Chn816_Char(12 + i, 85, Input_Temp[i] & 0x0f, 1);
		}
	} else {
		for (i = 0; i < 2; ++i)
			Disp_Chn816_Char(12 + i, 85, Sys_Param.st[i] & 0x0f, 1);
	}

	//通讯间隔
	if ((Input_Type == 1) && (SndDis_Index == 5)) {
		for (i = 0; i < 3; ++i) {
			if (i == Input_Pos)
				Disp_Chn816_Char(12 + i, 105, Input_Temp[i] & 0x0f, 0);
			else
				Disp_Chn816_Char(12 + i, 105, Input_Temp[i] & 0x0f, 1);
		}
	} else {
		for (i = 0; i < 3; ++i)
			Disp_Chn816_Char(12 + i, 105, Sys_Param.rtd[i] & 0x0f, 1);
	}
}

//系统参数
void Dis_AlarmSetup(INT8U AllRefresh)
{
	static INT8U index_title[] = { 85, 123, 26, 27, 28, 29, 0 };
	static INT8U index_chl[] = { 65, 125, 0 };
	static INT8U index_dev_num[] = { 29, 95, 68, 0 };

	char pos, cnt;
	char pon[4];

	memset(pon, 1, sizeof(pon));
	pon[SndDis_Index % sizeof(pon)] = 0;

	if (AllRefresh) {
		Clr_LCDRam();
		Disp_Yline(3, 0);
		Disp_Xline_Seg(4, 34, 26);
		Input_Type = 0;
		SndDis_Index = 0;
		Input_Pos = 0;

		/* 报警参数设置 */
		for (pos = 0; pos < sizeof(index_title) - 1; pos++) {
			if (pos == 0) {
				Disp_Chn1616_Char(1, 11, index_title[pos], 1);
			} else {
				Disp_Chn1616_Char(1, pos * 18 + 11, index_title[pos], 1);
			}
		}

		/* 通道ID */
		Disp_Chn1616_Str(6, 10, index_chl, 1);
		Disp_Chn816_Char(10, 10, 19, 1);
		Disp_Chn816_Char(11, 10, 20, 1);

		/* 设备号 */
		Disp_Chn1616_Str(17, 10, index_dev_num, 1);
	}

	/* 通道N */
	for (pos = 0; pos < 4; pos++) {
		Disp_Chn1616_Str(6, 42 + pos * 20, index_chl, pon[pos]);
		if (SndDis_Index < 4) {
			Disp_Chn816_Char(10, 42 + pos * 20, pos + 1, pon[pos]);
		} else {
			Disp_Chn816_Char(10, 42 + pos * 20, pos + 5, pon[pos]);
		}
	}

	/* 通道N设备号 */
	for (pos = 0; pos < 4; pos++) {
		if ((Input_Type == 1) && (pos == SndDis_Index % 4)) {
			for (cnt = 0; cnt < 14; cnt++) {
				if (cnt == Input_Pos)
					Disp_Chn816_Char(13 + cnt, 42 + pos * 20, Input_Temp[cnt] & 0x0F, 0);
				else
					Disp_Chn816_Char(13 + cnt, 42 + pos * 20, Input_Temp[cnt] & 0x0F, 1);
			}
		} else {
			for (cnt = 0; cnt < 14; cnt++) {
				Disp_Chn816_Char(13 + cnt, 42 + pos * 20, Alarm_Param[pos + SndDis_Index / 4 * 4].mn[cnt] & 0x0F, 1);
			}
		}
	}

	/* 显示箭头 */
	if (SndDis_Index < 4) {
		//Down Arrow
		Disp_Chn1616_Char(27, 10, 39, 1);
	} else {
		//Up Arrow
		Disp_Chn1616_Char(27, 10, 38, 1);
	}
}

//通信参数
/*void Dis_ComSetup(INT8U AllRefresh)    
  {
  static INT8U index_rate[] = {
  92,93,94,0
  };
  static INT8U index_code[] = {
  29,95,71,68,0
  };
  static INT8U index_var[] = {
  96,56,0
  };
  static INT8U index_addr[] = {
  97,98,0
  };


  char i,regindex;
  char pon[10],str[6];

  if(AllRefresh)
  {
  Clr_LCDRam();
  Input_Type = 0;
  SndDis_Index = 0;
  Input_Pos = 0;

  Disp_Xline(25);
  }

  memset(pon,1,sizeof(pon));
  pon[SndDis_Index] = 0;

  Disp_Chn1616_Str(1,5,index_rate,pon[0]);
  Disp_Chn1616_Str(16,5,index_code,pon[1]);

  for(i=0;i<MODBUS_VARNUM;++i)
  {
  Disp_Chn1616_Str(1,30+20*i,index_var,pon[2+i]);
  Disp_Chn816_Char(5,30+20*i,i+1,pon[2+i]);
  Disp_Chn1616_Str(6,30+20*i,index_addr,pon[2+i]);
  }

  Clr_RamBlock(8,5,5);
  Clr_RamBlock(25,5,3);
  for(i=0;i<MODBUS_VARNUM;++i)
  Clr_RamBlock(12,30+20*i,6);

//波特率
if((Input_Type == 1) && (SndDis_Index == 0))
{
for(i=0;i<strlen(ComBaud_Str[Input_Temp[0]]);++i)
{
Disp_Chn816_Char(8+i,5,ComBaud_Str[Input_Temp[0]][i]&0x0f,0);
}
}
else
{
for(i=0;i<strlen(ComBaud_Str[Com_Param.baud_index]);++i)
{
Disp_Chn816_Char(8+i,5,ComBaud_Str[Com_Param.baud_index][i]&0x0f,1);
}
}

//设备编号
if((Input_Type == 1) && (SndDis_Index == 1))
{
for(i=0;i<=Input_Pos;++i)
{
if(i == Input_Pos)
Disp_Chn816_Char(25+i,5,Input_Temp[i]&0x0f,0);
else
Disp_Chn816_Char(25+i,5,Input_Temp[i]&0x0f,1);
}
}
else
{
	sprintf(str,"%d",Com_Param.devid);
	for(i=0;i<strlen(str);++i)
		Disp_Chn816_Char(25+i,5,str[i]&0x0f,1);
}

//变量地址
for(regindex=0;regindex<MODBUS_VARNUM;++regindex)
{
	if((Input_Type == 1) && (SndDis_Index == 2+regindex))
	{
		for(i=0;i<=Input_Pos;++i)
		{
			if(i == Input_Pos)
				Disp_Chn816_Char(12+i,30+20*regindex,Input_Temp[i]&0x0f,0);
			else
				Disp_Chn816_Char(12+i,30+20*regindex,Input_Temp[i]&0x0f,1);
		}
	}
	else
	{
		INT32U temp = Com_Param.regaddr[regindex];
		sprintf(str,"%ld",temp);
		for(i=0;i<strlen(str);++i)
			Disp_Chn816_Char(12+i,30+20*regindex,str[i]&0x0f,1);
	}
}
}*/

//Logo画面
void Dis_Logo(void)
{
	Disp_Img(LCD_IMG_ADDR + 960, 30, 64, zimo240128);
	Delay_N_mS(60000);
}

//总的显示处理程序
void LCD_Display(INT8U AllRefresh)
{
	switch (MainDis_Index) {
		case WELCOME_INDEX:
			Dis_Welcome(AllRefresh);
			break;
		case MAINSEL_INDEX:
			Dis_MainSel(AllRefresh);
			break;
		case STATWATCH_INDEX:
			Dis_StatWatch(AllRefresh);
			break;
		case LOGWATCH_INDEX:
			Dis_LogWatch(AllRefresh);
			break;
		case LOGLIST_INDEX:
			Dis_LogList(AllRefresh);
			break;
		case PWINPUT_INDEX:
			Dis_PWInput(AllRefresh);
			break;
		case ALARMSETUP_INDEX:
			Dis_AlarmSetup(AllRefresh);
			break;
		case SYSSETUP_INDEX:
			Dis_SysSetup(AllRefresh);
			break;
		default:
			break;
	}
}

//按键处理部分
void Key_Welcome(INT8U KeyVal)
{
	if ((KeyVal == KEY_ENTER) || (KeyVal == KEY_CANCEL)) {
		MainDis_Index = MAINSEL_INDEX;
		Dis_AllRefresh = 1;
		DisplayTimer = 0;
	}

	//后门程序，同时按住左、右、关闭蜂鸣器
	if ((KeyVal == BACK_DOOR0) && (DisplayTimer > 50)) {
		BeepEnable = 0;
	}
}

//菜单选择
void Key_MainSel(INT8U KeyVal)
{
	if (KeyVal == KEY_ENTER) {
		//进入相应页面
		if (SndDis_Index == 0) {
			MainDis_Index = STATWATCH_INDEX;
		} else if (SndDis_Index == 1) {
			MainDis_Index = PWINPUT_INDEX;
			PWLast_Index = ALARMSETUP_INDEX;
		} else if (SndDis_Index == 2) {
			MainDis_Index = PWINPUT_INDEX;
			PWLast_Index = SYSSETUP_INDEX;
		}
		Dis_AllRefresh = 1;
		DisplayTimer = 0;
	} else if (KeyVal == KEY_CANCEL) {
		//返回主页面
		MainDis_Index = WELCOME_INDEX;
		Dis_AllRefresh = 1;
		DisplayTimer = 0;
	} else if (KeyVal == KEY_UP) {
		//切换选择项
		if (SndDis_Index == 0)
			SndDis_Index = 2;
		else
			SndDis_Index--;
		Dis_PartRefresh = 1;
	} else if (KeyVal == KEY_DOWN) {
		if (SndDis_Index == 2)
			SndDis_Index = 0;
		else
			SndDis_Index++;
		Dis_PartRefresh = 1;
	}

	//后门程序，同时按住左、右、ESC可不用输入密码
	if ((KeyVal != BACK_DOOR0) && (KeyVal != BACK_DOOR1))
		DisplayTimer = 0;

	if ((KeyVal == BACK_DOOR1) && (DisplayTimer > 200)) {
		if (SndDis_Index == 1) {
			MainDis_Index = ALARMSETUP_INDEX;
		} else if  (SndDis_Index == 2) {
			MainDis_Index = SYSSETUP_INDEX;
		}

		SndDis_Index = 0;
		Dis_AllRefresh = 1;
		DisplayTimer = 0;
	}
}

//状态显示
void Key_StatWatch(INT8U KeyVal)
{
	if (KeyVal == KEY_CANCEL) {
		MainDis_Index = MAINSEL_INDEX;
		Dis_AllRefresh = 1;
		DisplayTimer = 0;
	} else if (KeyVal == KEY_UP || KeyVal == KEY_DOWN) {
		MainDis_Index = LOGWATCH_INDEX;
		Input_Type = 0;
		SndDis_Index = 0;
		Dis_AllRefresh = 1;
		DisplayTimer = 0;
	}
}

//LOG查看
void Key_LogWatch(INT8U KeyVal)
{
	if (KeyVal == KEY_CANCEL) {
		if (Input_Type == 0) {
			MainDis_Index = MAINSEL_INDEX;
			Dis_AllRefresh = 1;
		} else {
			Dis_PartRefresh = 1;
			Input_Type = 0;
		}
		DisplayTimer = 0;
	} else if (KeyVal == KEY_ENTER) {
		if (Input_Type == 0) {
			Input_Type = 1;
			SndDis_Index = 0;
			Dis_PartRefresh = 1;
		} else {
			MainDis_Index = LOGLIST_INDEX;
			Fetch_HisRecord(SndDis_Index % 8);
			His_Record_Cnt = Get_HisRecordItemsBackward(His_Record_Item, 5);
			Dis_AllRefresh = 1;
		}
		DisplayTimer = 0;
	} else if (KeyVal == KEY_UP) {
		if (Input_Type == 0) {
			MainDis_Index = STATWATCH_INDEX;
			Dis_AllRefresh = 1;
		} else {
			if (SndDis_Index == 0) {
				SndDis_Index = 7;
			} else {
				SndDis_Index--;
			}
			Dis_PartRefresh = 1;
		}
		DisplayTimer = 0;
	} else if (KeyVal == KEY_DOWN) {
		if (Input_Type == 0) {
			MainDis_Index = STATWATCH_INDEX;
			Dis_AllRefresh = 1;
		} else {
			SndDis_Index++;
			if (SndDis_Index > 7) {
				SndDis_Index = 0;
			}
			Dis_PartRefresh = 1;
		}
		DisplayTimer = 0;
	}
}

void Key_LogList(INT8U KeyVal)
{
	if (KeyVal == KEY_CANCEL) {
		MainDis_Index = LOGWATCH_INDEX;
		Dis_AllRefresh = 1;
		DisplayTimer = 0;
	} else if (KeyVal == KEY_UP) {
		His_Record_Cnt = Get_HisRecordItemsForward(His_Record_Item, 5);
		Dis_AllRefresh = 1;
		DisplayTimer = 0;
	} else if (KeyVal == KEY_DOWN) {
		His_Record_Cnt = Get_HisRecordItemsBackward(His_Record_Item, 5);
		Dis_AllRefresh = 1;
		DisplayTimer = 0;
	}
}

//密码输入
void Key_PWInput(INT8U KeyVal)
{
	if (KeyVal == KEY_CANCEL) {
		MainDis_Index = MAINSEL_INDEX;
		Dis_AllRefresh = 1;
		DisplayTimer = 0;
	} else if (KeyVal == KEY_ENTER) {
		//判断密码是否正确
		if ((Input_Pos == 5) && (memcmp(Input_Temp, Sys_Param.pw, 6) == 0)) {
			MainDis_Index = PWLast_Index;
		}
		Dis_AllRefresh = 1;
		DisplayTimer = 0;
	} else if (KeyVal == KEY_RIGHT) {
		//切换输入位置
		if (Input_Pos < 5) {
			Input_Pos++;
			Dis_PartRefresh = 1;
		}
	} else if (KeyVal == KEY_LEFT) {
		if (Input_Pos > 0) {
			Input_Pos--;
			Dis_PartRefresh = 1;
		}
	} else if (KeyVal == KEY_UP) {
		//切换输入字符
		if (Input_Temp[Input_Pos] > '0')
			Input_Temp[Input_Pos]--;
		else
			Input_Temp[Input_Pos] = '9';
		Dis_PartRefresh = 1;
	} else if (KeyVal == KEY_DOWN) {
		if (Input_Temp[Input_Pos] < '9')
			Input_Temp[Input_Pos]++;
		else
			Input_Temp[Input_Pos] = '0';
		Dis_PartRefresh = 1;
	}
}

//报警参数设置
void Key_AlarmSetup(INT8U KeyVal)
{
	if (KeyVal == KEY_ENTER) {
		if (Input_Type == 0) {
			//将当前设置拷贝至临时输入变量
			Input_Type = 1;
			Input_Pos = 0;
			Dis_PartRefresh = 1;
			memset(Input_Temp, '0', sizeof(Input_Temp));
			memcpy(Input_Temp, Alarm_Param[SndDis_Index].mn, 14);
		} else if (Input_Type == 1) {
			//保存设置参数
			Input_Temp[14] = 0;
			strcpy(Alarm_Param[SndDis_Index].mn, Input_Temp);
			if (strncmp(Alarm_Param[SndDis_Index].mn, "00000000000000", 14)) {
				Alarm_Param[SndDis_Index].inuse = 1;
				Alarm_Param[SndDis_Index].drop_status = 1;
				Alarm_Param[SndDis_Index].over_status = 0;
				Hjt212Signal[HJT_UART].upflag_reqstatus = 1;

				RECORD_HEAD_T head;
				memcpy(head.mn, Alarm_Param[SndDis_Index].mn, 14);
				head.headpos = 0;
				head.tailpos = 0;
				Format_HistoryRecord(SndDis_Index, &head);
			} else {
				Alarm_Param[SndDis_Index].inuse = 0;
				Alarm_Param[SndDis_Index].drop_status = 0;
				Alarm_Param[SndDis_Index].over_status = 0;
			}
			Input_Type = 0;
			Dis_PartRefresh = 1;
			Save_AlarmParam(Alarm_Param);
		}
	} else if (KeyVal == KEY_CANCEL) {
		if (Input_Type == 0) {
			MainDis_Index = MAINSEL_INDEX;
			Dis_AllRefresh = 1;
			DisplayTimer = 0;
		} else if (Input_Type == 1) {
			Input_Type = 0;
			Dis_PartRefresh = 1;
		}
	} else if (KeyVal == KEY_RIGHT) {
		//切换输入位置
		if (Input_Type == 1) {
			Dis_PartRefresh = 1;
			if (Input_Pos < 13)
				Input_Pos++;
		}
	} else if (KeyVal == KEY_LEFT) {
		if (Input_Type == 1) {
			Dis_PartRefresh = 1;
			if (Input_Pos > 0)
				Input_Pos--;
		}
	} else if (KeyVal == KEY_UP) {
		if (Input_Type == 0) {
			if (SndDis_Index > 0) {
				SndDis_Index--;
				Dis_PartRefresh = 1;
			}
		} else if (Input_Type == 1) {
			//切换输入数据
			Dis_PartRefresh = 1;
			if (Input_Temp[Input_Pos] < '0' || Input_Temp[Input_Pos] > '9')
				Input_Temp[Input_Pos] = '0';
			else if (Input_Temp[Input_Pos] == '0')
				Input_Temp[Input_Pos] = '9';
			else
				Input_Temp[Input_Pos]--;
		}
	} else if (KeyVal == KEY_DOWN) {
		if (Input_Type == 0) {
			if (SndDis_Index < MAX_ALARM_NUM - 1) {
				SndDis_Index++;
				Dis_PartRefresh = 1;
			}
		} else if (Input_Type == 1) {
			Dis_PartRefresh = 1;
			if (Input_Temp[Input_Pos] < '0' || Input_Temp[Input_Pos] > '9')
				Input_Temp[Input_Pos] = '0';
			else if (Input_Temp[Input_Pos] == '9')
				Input_Temp[Input_Pos] = '0';
			else
				Input_Temp[Input_Pos]++;
		}
	}
}

//系统设置
void Key_SysSetup(INT8U KeyVal)
{
	if (KeyVal == KEY_ENTER) {
		if (Input_Type == 0) {
			//将当前设置拷贝至临时输入变量
			Input_Type = 1;
			Input_Pos = 0;
			Dis_PartRefresh = 1;
			if (SndDis_Index == 0) {
				memset(Input_Temp, '0', sizeof(Input_Temp));
			} else if (SndDis_Index == 1) {
				memcpy(Input_Temp, System_TimeStr, 14);
			} else if (SndDis_Index == 2) {
				memcpy(Input_Temp, Sys_Param.sim, 11);
			} else if (SndDis_Index == 3) {
				memcpy(Input_Temp, Sys_Param.mn, 14);
			} else if (SndDis_Index == 4) {
				memcpy(Input_Temp, Sys_Param.st, 2);
			} else if (SndDis_Index == 5) {
				memcpy(Input_Temp, Sys_Param.rtd, 3);
			}
		} else if (Input_Type == 1) {
			//保存设置参数
			if (SndDis_Index == 0) {
				if (Input_Pos == 5) {
					Input_Temp[6] = 0;
					strcpy(Sys_Param.pw, Input_Temp);
					Input_Type = 0;
					Dis_PartRefresh = 1;
					Save_SysParam(&Sys_Param);
				}
			} else if (SndDis_Index == 1) {
				Input_Temp[14] = 0;
				v_Set1302((INT8U *) Input_Temp);
				v_Get1302(System_TimeStr, &Current_Tm);
				Input_Type = 0;
				Dis_PartRefresh = 1;
			} else if (SndDis_Index == 2) {
				Input_Temp[11] = 0;
				strcpy(Sys_Param.sim, Input_Temp);
				Input_Type = 0;
				Dis_PartRefresh = 1;
				Save_SysParam(&Sys_Param);
			} else if (SndDis_Index == 3) {
				Input_Temp[14] = 0;
				strcpy(Sys_Param.mn, Input_Temp);
				Input_Type = 0;
				Dis_PartRefresh = 1;
				Save_SysParam(&Sys_Param);
			} else if (SndDis_Index == 4) {
				Input_Temp[2] = 0;
				strcpy(Sys_Param.st, Input_Temp);
				Input_Type = 0;
				Dis_PartRefresh = 1;
				Save_SysParam(&Sys_Param);
			} else if (SndDis_Index == 5) {
				Input_Temp[3] = 0;
				strcpy(Sys_Param.rtd, Input_Temp);
				Input_Type = 0;
				Dis_PartRefresh = 1;
				Save_SysParam(&Sys_Param);
			}
		}
	} else if (KeyVal == KEY_CANCEL) {
		if (Input_Type == 0) {
			MainDis_Index = MAINSEL_INDEX;
			Dis_AllRefresh = 1;
			DisplayTimer = 0;
		} else if (Input_Type == 1) {
			Input_Type = 0;
			Dis_PartRefresh = 1;
		}
	} else if (KeyVal == KEY_RIGHT) {
		//切换输入位置
		if (Input_Type == 1) {
			Dis_PartRefresh = 1;

			if (SndDis_Index == 0) {
				if (Input_Pos < 5)
					Input_Pos++;
			} else if ((SndDis_Index == 1) || (SndDis_Index == 3)) {
				if (Input_Pos < 13)
					Input_Pos++;
			} else if (SndDis_Index == 2) {
				if (Input_Pos < 10)
					Input_Pos++;
			} else if (SndDis_Index == 4) {
				if (Input_Pos < 1)
					Input_Pos++;
			} else if (SndDis_Index == 5) {
				if (Input_Pos < 2)
					Input_Pos++;
			}
		}
	} else if (KeyVal == KEY_LEFT) {
		if (Input_Type == 1) {
			Dis_PartRefresh = 1;

			if (SndDis_Index <= 5) {
				if (Input_Pos > 0)
					Input_Pos--;
			}
		}
	} else if (KeyVal == KEY_UP) {
		if (Input_Type == 0) {
			if (SndDis_Index > 0) {
				SndDis_Index--;
				Dis_PartRefresh = 1;
			}
		} else if (Input_Type == 1) {
			//切换输入数据
			Dis_PartRefresh = 1;

			if (SndDis_Index <= 5) {
				if ((Input_Temp[Input_Pos] < '0')
						|| (Input_Temp[Input_Pos] > '9'))
					Input_Temp[Input_Pos] = '0';
				else if (Input_Temp[Input_Pos] == '0')
					Input_Temp[Input_Pos] = '9';
				else
					Input_Temp[Input_Pos]--;
			}
		}
	} else if (KeyVal == KEY_DOWN) {
		if (Input_Type == 0) {
			if (SndDis_Index < 5) {
				SndDis_Index++;
				Dis_PartRefresh = 1;
			}
		} else if (Input_Type == 1) {
			Dis_PartRefresh = 1;

			if (SndDis_Index <= 5) {
				if ((Input_Temp[Input_Pos] < '0')
						|| (Input_Temp[Input_Pos] > '9'))
					Input_Temp[Input_Pos] = '0';
				else if (Input_Temp[Input_Pos] == '9')
					Input_Temp[Input_Pos] = '0';
				else
					Input_Temp[Input_Pos]++;
			}
		}
	}
}

//总的键盘处理程序
void Key_Proc(INT8U KeyVal)
{
	switch (MainDis_Index) {
	case WELCOME_INDEX:
		Key_Welcome(KeyVal);
		break;
	case MAINSEL_INDEX:
		Key_MainSel(KeyVal);
		break;
	case STATWATCH_INDEX:
		Key_StatWatch(KeyVal);
		break;
	case LOGWATCH_INDEX:
		Key_LogWatch(KeyVal);
		break;
	case LOGLIST_INDEX:
		Key_LogList(KeyVal);
		break;
	case PWINPUT_INDEX:
		Key_PWInput(KeyVal);
		break;
	case ALARMSETUP_INDEX:
		Key_AlarmSetup(KeyVal);
		break;
	case SYSSETUP_INDEX:
		Key_SysSetup(KeyVal);
		break;
	default:
		break;
	}
}

//总处理函数
void ProcDisplay(void)
{
	INT8U keyval = Get_KeyVal();

	if (keyval == KEY_NONE) {
		//120s无按键操作则关闭背光返回欢迎界面
		if (BackLightTimer >= S2TENMS(120)) {
			BackLightTimer = 0;
			LCD_LIGHT_OFF;
			if ((MainDis_Index == ALARMSETUP_INDEX)
					|| (MainDis_Index == SYSSETUP_INDEX)) {
				MainDis_Index = WELCOME_INDEX;
				Dis_AllRefresh = 1;
				DisplayTimer = 0;
			}
		}

		//950ms定时刷新界面
		if (DisplayTimer >= MS2TENMS(950)) {
			if ((MainDis_Index == WELCOME_INDEX)
					|| (MainDis_Index == STATWATCH_INDEX)
					|| (MainDis_Index == LOGWATCH_INDEX)) {
				DisplayTimer = 0;
				Dis_PartRefresh = 1;
			}
		}
	} else {
		//有按键按下
		BackLightTimer = 0;
		LCD_LIGHT_ON;
		Key_Proc(keyval);       //处理按键事件
	}

	if ((Dis_AllRefresh == 1) || (Dis_PartRefresh == 1)) {
		LCD_Display(Dis_AllRefresh);
		Dis_AllRefresh = 0;
		Dis_PartRefresh = 0;
	}
}
