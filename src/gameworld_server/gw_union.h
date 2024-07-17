#ifndef _GW_UNION_H
#define _GW_UNION_H

#include <string>
#include "union_base.h"
#include <list>
#include <set>
#include <map>

class CUnionMgr;
class CVoteMgr;
class CVote;

// ����
class CUnion
{
public:
	friend class CUnionMgr;
	enum UnionLogType{eCreate, eJobin, eLeave, eBanish, eJobUp, eJobDown, eDoContribution, eGiveCaption, eChangeBulletin, eGetMoney, UnionLogTypeNum};
	CUnion(CUnionMgr *pUnionMgr, const SUnionData &unionData);
	~CUnion();
//	bool Serialize(CG_CmdPacket *cmd);

	DWORD GetID() { return m_core.m_nID; }
	string GetName() { return m_core.m_strName; }
	DWORD GetCaption() { return m_core.m_nCaptainID; }
	SUnionData* GetData() { return &m_core; }

	SUnionCitizen* Find(DWORD playerID);					// ���ҹ���
	bool AddCitizen(DWORD playerID, short level, short metier, BYTE bOnline = true, EUnionOfficial official = UO_Masses);		// ���ӹ���
	bool RemoveCitizen(DWORD playerID);						// ɾ������
	bool SetCitizenNickName(DWORD playerID, char* NickName);// ���óƺ�
	bool Appoint(DWORD playerID, EUnionOfficial official);	// ������ְ
	UINT CitizenCount() { return (UINT)m_core.vecUnionCitizen.size(); }

	bool UpdateLevel();										// ��������
	bool AddScore(int count);								// ����/���ٻ���
	bool SetCaptain(DWORD playerID);						// �������
	bool SetName(string name);								// ��������
	bool SetIcon(char* data, int dataLen, DWORD &outIconID);// ���ñ�־
	unsigned long GetIcon();
	bool SetBulletin(string bulletin);						// ���ù���
	bool InviteWar(DWORD unionInvited);						// ����һ��������ս

	void OnCitizenComming(DWORD playerID);					// �������ʱ�Ļص�
	void OnCitizenGoing(DWORD playerID);					// �������ʱ�Ļص�

	void OnNewVote(CVote* pVote);					// ĳ�˷�����һ������
	int  OnNewLog(UnionLogType type, const char* pData);
	int  GetNum(EUnionOfficial enO);
	void QueryLog(unsigned long dwPlayerID);
	int  AddLog(UnionLogType type, const char* pData);
	int GiveUnion(unsigned long dwPlayerID, unsigned long dwTo);
	SUnionCitizen& operator[](int index)             { return m_core.vecUnionCitizen[index]; }

	int AddWarScores(long nAdd);
	unsigned long GetWarScores();
	void SetWarScores(unsigned long dwWS);
	int UpdateContribution(unsigned long dwPlayerID, long dwC);
	int DoContribution(long playerID, long nMoney, long nItem1, long nItem2, long nItem3, long nItem4);
	int SetIcon(unsigned long dwPlayerID, long dwIconID);
	int OnDelKind();
private:
	SUnionData	m_core;
	CUnionMgr	*m_pUnionMgr;
};

// ��������
class CUnionMgr
{
public:
	CUnionMgr();
	~CUnionMgr();
	bool Init();
	void Tick();
	bool IsInited() { return m_bInited; }

	// �������������ɹ�ʱ����������ID��ʧ��ʱ����ֵΪ0
	DWORD CreateUnion(DWORD playerID, short playerLevel, std::string name, string enounce);		
	// ɾ������
	bool DeleteUnion(DWORD unionID);
	CUnion* FindUnion(DWORD unionID);
//	DWORD GetUnionID(DWORD userID);

	bool RemoveInvalidUnion();

	void OnCitizenComming(DWORD playerID, int& unionID, DWORD regionID);	// �������ʱ�Ļص�
	void OnCitizenGoing(DWORD playerID, DWORD unionID);						// �������ʱ�Ļص�
	void OnDeleteUser(DWORD userDBID);										// ɾ����ɫʱ�Ļص�
	void OnCreateUser(DWORD userDBID);										// ������ɫʱ�Ļص�

	void OnAddNewRegion(int regionID);
	void QueryLog(unsigned long dwUnion, unsigned long dwPlayerID);
	void GiveUnion(unsigned long dwUnion, unsigned long dwPlayerID, unsigned long dwTo);
	int UpdateContribution(GWUser* pUser, long dwDate);
	int DoContribution(DWORD dwUnion, long playerID, long nMoney, long nItem1, long nItem2, long nItem3, long nItem4);
	int AddWarScores(unsigned long dwUnion, long nAdd);
	int SetIcon(GWUser* pUser, long dwIconID);
public:
	vector<CUnion*> m_arrUnions;
//	hash_map<DWORD, DWORD> m_mapUserToUnion;

private:
	bool m_bInited;
};

class CVote
{
public:
	friend class CVoteMgr;
	CVote(unsigned long ID, unsigned long dwCreater, unsigned long dwRecall, unsigned long StopTime);
	unsigned long GetCreater();
	unsigned long GetRecall();
	unsigned long GetUnionID();
	bool OnRun(unsigned long dwTime);
	int GetAgreeNum();
	int GetOpposeNum();
	int GetRecallNum();
	int GetDoneNum();
	int GetAllNum();

	int Agree(unsigned long dwPlayerID);
	int Oppose(unsigned long dwPlayerID);
	int Forfeit(unsigned long dwPlayerID);
	int DoVote(unsigned long dwPlayerID, int nState);
	int GetPlayerVoteState(unsigned long dwPlayerID);
	unsigned long GetVoteID();
	void Load();
	void Save();
	void Delete();
protected:
	int ProcessNum();
	void End();
private:
	CVote(){}
	typedef std::set<unsigned long> VoteSet;
	VoteSet& GetAgreeSet();
	VoteSet& GetOpposeSet();
	VoteSet& GetForfeitSet();
private:
	unsigned long m_dwStopTime;
	unsigned long m_dwCreater;
	unsigned long m_dwRecall;
	unsigned long m_dwVoteID;
	unsigned long m_dwUnionID;

	VoteSet m_setAgree;
	VoteSet m_setOppose;
	VoteSet m_setForfeit;
};

class IDKey
{
public:
	bool operator () (CVote *p1, CVote *p2) const;
};

class RecallKey
{
public:
	bool operator () (CVote *p1, CVote *p2) const;
};

class UnionIDKey
{
public:
	bool operator () (CVote *p1, CVote *p2) const;
};

class CVoteMgr
{
public:
	int CretaeVote(unsigned long dwCreater, unsigned long dwRecall);
	int DoVote(unsigned long dwPlayer, unsigned long dwVoteID, int nState);
	bool OnRun(unsigned long dwTime);
	int Load();
	void Save();
	int GetUnionAllVote(unsigned long dwUnionID, CVote** pBuf, int nBuffNum);
	static CVoteMgr& GetMgr();
protected:
	typedef std::set<CVote*, IDKey> IDKeySet;
	IDKeySet& GetIDKeySet();
	typedef std::set<CVote*, RecallKey> RecallKeySet;
	RecallKeySet& GetRecallKeySet();
	typedef std::multiset<CVote*, UnionIDKey> UnionIDKeySet;
	UnionIDKeySet& GetUnionIDKeySet();
private:
	int DelVote(CVote* pVote);
	int AddVote(CVote* pVote);
private:
	IDKeySet m_IDKeySet;
	RecallKeySet m_RecallKeySet;
	UnionIDKeySet m_UnionIDKeySet;
};

extern "C" CUnionMgr g_UnionMgr;

CUnionMgr* GetUnionMgr();


#endif//_GW_UNION_H

