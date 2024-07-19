#ifndef _MASTER_PRENTICE_H
#define _MASTER_PRENTICE_H

const short c2r_prentice_base				= 2000;
const short r2c_prentice_base				= 4000;

const short c2r_prentice_add_apply			= c2r_prentice_base + 1;	// 申请拜师
const short c2r_prentice_add_response		= c2r_prentice_base + 2;	// 被申请拜师的反馈
const short c2r_prentice_remove				= c2r_prentice_base + 3;	// 取消师徒关系
const short c2r_prentice_finish_apply		= c2r_prentice_base + 4;	// 申请出师
const short c2r_prentice_finish_response	= c2r_prentice_base + 5;	// 被申请出师的反馈
const short c2r_prentice_gift				= c2r_prentice_base + 6;	// 徒弟到达一定数量时的礼品

const short r2c_prentice_add_apply			= r2c_prentice_base + 1;
const short r2c_prentice_finish_apply		= r2c_prentice_base + 2;
const short r2c_prentice_add_result			= r2c_prentice_base + 3;
const short r2c_prentice_remove_result		= r2c_prentice_base + 4;
const short r2c_prentice_finish_result		= r2c_prentice_base + 5;
const short r2c_prentice_onlinetime			= r2c_prentice_base + 6;
const short r2c_prentice_gift_result		= r2c_prentice_base + 7;

const short prentice_code_success			= 0;
const short prentice_code_error_unknown		= 1;
const short prentice_code_agree				= 2;
const short prentice_code_disagree			= 3;
const short prentice_code_time_limit		= 4;
const short prentice_code_gift_withdrawed	= 5;


#define PRESYS_MAX_PER_USER				2
#define PRESYS_ADD_START_LEV			8
#define PRESYS_ADD_END_LEV				16
#define PRESYS_MASTER_MIN_LEV			37
#define PRESYS_FINISH_START_LEV			31
#define PRESYS_FINISH_END_LEV			33
#define PRESYS_COLDOWN_TIME				(24*3600)
#define PRESYS_GIFT_LEVEL_COUNT			2	// 师徒系统额外礼品的种类数目

const int PrenSysGiftNeedCount[PRESYS_GIFT_LEVEL_COUNT] = { 5, 15 };	// 取师徒系统额外礼品时需要的角色等级
const int PrenSysGiftNeedGrids[PRESYS_GIFT_LEVEL_COUNT] = { 1, 1 };		// 取师徒系统额外礼品时要求的包裹最少空格


struct SMasterPrentice
{
	long			index;
	DWORD			masterID;
	DWORD			prenticeID;
	std::string		masterName;
	std::string		prenticeName;
	DWORD			finishTime;
	long			finishOnlineTime;
	int				creditToAdd;

	SMasterPrentice() { Clear(); }
	~SMasterPrentice() { }
	void Clear()
	{
		index = 0;
		masterID = 0;
		prenticeID = 0;
		masterName = "";
		prenticeName = "";
		finishTime = 0;
		finishOnlineTime = 0;
		creditToAdd = 0;
	}
};

inline bool PrenSysGiftLevel_IsValid(int lev)
{
	if (lev >= (sizeof(DWORD)*8))
		return false;

	return ((lev>=0) && (lev<PRESYS_GIFT_LEVEL_COUNT));
}


#endif//_MASTER_PRENTICE_H
