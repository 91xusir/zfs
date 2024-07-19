//-----------------------------------------------------------------------------
//CCastleWarMgr.h
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#ifndef GW_CCASTLEWARMGR_H
#define GW_CCASTLEWARMGR_H

#include "castlewar_base.h"

#define DEFAULT_WEEKDAY			3	//����
#define DEFAULT_HOUR			20	//Сʱ
#define DEFAULT_MIN				0	//����
#define DEFAULT_DURATION		2	//Сʱ

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

	/* �ڴ�浵 */
	void UpdateData(STWarInfo* p_war_info);

	/* ��ս��ʼǰ/����ʱ�浵 */
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
	//Ĭ�ϳ�սʱ��
	//-----------------------------
	int m_default_weekday;

	int m_default_hour;

	int m_default_min;

	int m_default_duration;
};

extern "C" CGWCastleWarMgr g_CastlewarMgr;

CGWCastleWarMgr* GetCastleWarMgr();

#endif //GW_CCASTLEWARMGR_H
