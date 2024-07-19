#ifndef _GW_ITEM_H
#define _GW_ITEM_H

#include <string>

const int GMSever_RegionID				= 255;			// 作为一个region
const int GWServer_RegionID				= 254;			// 作为一个region

class CGWItemIDMgr
{
public:
    bool GetIDData(int idx, DWORD &beginID, DWORD &currentID, DWORD &maxID);
    bool SetIDData(int idx, DWORD beginID, DWORD currentID, DWORD maxID);

	void SendIDData(int regionID);
    bool ChangeNewIDIndex(int regionID);        // 如果一个region的id资源用光了，就要换另外一个未使用的id资源

	void OnRegionRegister(int regionID);
};
extern CGWItemIDMgr g_GWItemIDMgr;




const int ItemID_UpdateDB_Circle		= 1000;			// ID每次消耗掉多少就更新数据库
class CGWItemIDFactory
{
public:
	CGWItemIDFactory();
	~CGWItemIDFactory();
	bool Init();
	void CheckItemID();
	DWORD GetNextItemID();
private:
	DWORD m_dwNextID;			// 下一个ID
	DWORD m_dwIDInDB;			// 数据库中的ID
	DWORD m_dwIDMaxInDB;

	void UpdateDB();
};
extern CGWItemIDFactory g_GWItemIDFactory;
DWORD GetNextItemID();


class CBankPass
{
public:
	CBankPass() { }
	~CBankPass() { }

	static bool UpdatePass(DWORD userID, short operateID, const char* pass);
private:
	static bool Send_g2r_bankpass_result(DWORD userID, short operateID, short errorCode, const char* newPass);
};



#endif//_GW_ITEM_H

