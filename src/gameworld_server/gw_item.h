#ifndef _GW_ITEM_H
#define _GW_ITEM_H

#include <string>

const int GMSever_RegionID				= 255;			// ��Ϊһ��region
const int GWServer_RegionID				= 254;			// ��Ϊһ��region

class CGWItemIDMgr
{
public:
    bool GetIDData(int idx, DWORD &beginID, DWORD &currentID, DWORD &maxID);
    bool SetIDData(int idx, DWORD beginID, DWORD currentID, DWORD maxID);

	void SendIDData(int regionID);
    bool ChangeNewIDIndex(int regionID);        // ���һ��region��id��Դ�ù��ˣ���Ҫ������һ��δʹ�õ�id��Դ

	void OnRegionRegister(int regionID);
};
extern CGWItemIDMgr g_GWItemIDMgr;




const int ItemID_UpdateDB_Circle		= 1000;			// IDÿ�����ĵ����پ͸������ݿ�
class CGWItemIDFactory
{
public:
	CGWItemIDFactory();
	~CGWItemIDFactory();
	bool Init();
	void CheckItemID();
	DWORD GetNextItemID();
private:
	DWORD m_dwNextID;			// ��һ��ID
	DWORD m_dwIDInDB;			// ���ݿ��е�ID
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

