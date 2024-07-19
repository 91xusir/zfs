//-----------------------------------------------------------------------------
//CCastleWarMgr.h
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#ifndef GW_CCASTLEWARMGR_H
#define GW_CCASTLEWARMGR_H

#include "castlewar_base.h"

#define DEFAULT_WEEKDAY			3	//星期
#define DEFAULT_HOUR			20	//小时
#define DEFAULT_MIN				0	//分钟
#define DEFAULT_DURATION		2	//小时

class CUnion;
struct SUnionData;
struct SUnionCitizen;

class CGWCastleWarMgr
{
public:
	CGWCastleWarMgr(){};
	~CGWCastleWarMgr(){};

public:
	bool Init();

	/* 内存存档 */
	void UpdateData(STWarInfo* p_war_info);

	/* 城战开始前/结束时存档 */
	bool SaveToDB(string feod_name);

	bool SetDefWarTime(string feod_name, int def_weekday, int def_hour, int def_minute, int def_duration = 2);

	bool SetLatestWarTime(string feod_name, DWORD latest_time);

	void GetCastleWarDefaultStartTime(DWORD& ret, DWORD latest_time);

	bool IsInSameWeek(DWORD tm1, DWORD tm2);
	
	void OnUserOnline(long region_id, DWORD user_db_id);

	void OnRegionRegister(long region_id);

	bool ReloadTime(std::string feod_name);

private:
	vector<STWarInfo>	m_war_list;

	//-----------------------------
	//默认城战时间
	//-----------------------------
	int m_default_weekday;

	int m_default_hour;

	int m_default_min;

	int m_default_duration;
};

extern "C" CGWCastleWarMgr g_CastlewarMgr;

CGWCastleWarMgr* GetCastleWarMgr();

#endif //GW_CCASTLEWARMGR_H
