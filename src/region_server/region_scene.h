#ifndef __REGION_SCENE_H__
#define __REGION_SCENE_H__

#include "region_spawn.h"
//--------------------add start by tony 05.05.23-----------------------------//
#include "region_guarder.h"
//--------------------add end   by tony 05.05.23-----------------------------//

class RtsSceneDoc;

class CRegionItem : public CRegionObject
{
public:
	CRegionItem();
	virtual ~CRegionItem();

	void WriteSnapshot(CG_CmdPacket *cmd,CRegionCreature *target);
	SItemID m_item;
	long    m_pos[3];
	char    m_dir;
	long    m_masterId;
	long    m_masterKeepTime;
	long    m_sceneLiveTime;
	char	m_dropStyle;
    RtsPathPos *m_pathNode;
	long	m_dropUserID;

	long    m_hideTime;
	static bool GetNextDropPos(long* posCenter, long* pos, int& counter);
};

struct SMonsterLair
{
	DWORD LastSpawnTime;
	DWORD SpawnInterval;
	short NpcId;
	long  Position[3];
	char  Dir;
	CRegionCreature *Monster;
};

struct SSceneNpc
{
	long lID;
	short sSceneID;
	short sNpcId;
	long pos[3];
	char cDir;
	short sDif;
	bool bEnable;
};

typedef std::list<CRegionObject*> TGridObj;

const int SCENE_TIMER_SPAWN_MONSTER = OBJECT_C_TIMER_BASE + 1;
const int SCENE_TIMER_ITEM          = OBJECT_C_TIMER_BASE + 2;
const int SCENE_TIMER_GUARDER		= OBJECT_C_TIMER_BASE + 3;
const int SCENE_TIMER_CASTLEWAR		= OBJECT_C_TIMER_BASE + 4;
const int SCENE_TIMER_WARFIELD		= OBJECT_C_TIMER_BASE + 5;
const int SCENE_TIMER_WARFIELD_SPAWN= OBJECT_C_TIMER_BASE + 6;
const int SCENE_MAX_FIND_OBJECT     = 100;
class CRegionScene : public CRegionObject
{
public:
	CRegionScene();
	virtual ~CRegionScene();

	virtual void CreateAllNpc();

	// Add npc to this scene [8/27/2009 Louis.Huang]
	virtual CRegionCreature* AddNpc(SSceneNpc& npcInfo, long lLevel = -1, float fAtt[] = 0);

	// CRegionObject interface
	virtual void OnTimer(ULONG id, DWORD dwParam[]);

	// scene event begin
	virtual void OnCreate();
	virtual void OnRun();
	virtual void OnDestroy();

	virtual void OnUserEnter(CRegionUser *user);
	virtual void OnUserLeave(CRegionUser *user,int reason);

	virtual bool OnTryAddUser(CRegionUser *user);
	
	virtual void OnCreatureDead(CRegionCreature *cre,CRegionCreature *killer);

	/*
	virtual void OnUserNetDead(CRegionUser *user);
	virtual void OnUserGetItem(CRegionUser *user,SItemID item);
	*/
	// Scene event end

	// Treat scene npc list here [8/27/2009 Louis.Huang]
	static bool LoadNpcList(const char* file);

	void		SetSceneID(short id) {m_nSceneID = id;}
	typedef std::vector<SSceneNpc> NPCLIST;

protected:
	static EXT_SPACE::unordered_map< int, NPCLIST > ms_npcLists;

	short	m_nSceneID;		// 主场景为0，其他场景为fblist中的id列

public:
	bool Init();
	bool Exit();

	// Struct
	bool LoadSceneFile(const char *file);

	// Message
	bool BuildSnapshot(CG_CmdPacket *cmd,CRegionCreature *cre);
	void BroadcastCmd(long *pos,long range,CG_CmdPacket *cmd,CRegionCreature *exclude);
	void BroadcastMsg(long *pos,long range,const char *msg,
		const char *param,CRegionCreature *exclude);

	// Creature and actor
	bool TestAddCreature(long x,long y,long z);
	virtual bool AddCreature(CRegionCreature *cre,long *pos,char dir);
	CRegionCreature *RemoveCreature(ULONG oId);
	CRegionCreature *FindCreature(ULONG oId);
	CRegionCreature *FindUserByName(const char* username);

	void FindNpcByType(ULONG npcType, std::vector<CRegionCreature*>& foundNpcs);
	bool MoveCreature(CRegionCreature *cre,long x,long y,long z);

	void GetAroundCreatureByGrid(long *pos,long range,
		vector<CRegionCreature*>& creList,CRegionCreature *exclude);
	int GetAroundCreatureByTile(CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT], long *pos, long range, CRegionCreature *exclude,
        char cType, bool bBlock, int iMaxFind=30); // 返回找到的个数
	int GetAroundObjectByGrid(CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT], long *pos, long range, int maxFind); // 返回找到的个数

	// Block access
	RtsSceneBlockRtb* FindBlockByFileName(const char* filename);
	RtsSceneBlockRtb* FindBlockByPos(const long* pos);

	// Item
	bool AddItem(CRegionItem *item,long *pos,char dir);
	CRegionItem *RemoveItem(ULONG oId);
	CRegionItem *FindItem(ULONG oId);
	void ItemProcess();		// 地上道具的处理

	int GetPathContaniner(RtSceneBlockTerrain::STileAttr* pAttr,
		CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT],
		RtsPathPos* pPathNode,
		CRegionCreature *exclude, 
		char cType,
		bool bBlock,
		int iCnt,
		int iMaxFind);

	// object index
	TObjectHash			 m_objectHash;
	TObjectHash			 m_userHash;

	RtsSceneDoc*		 m_pTerrain;
	RtsPathFind*		 m_pPathFind;
	RtsSceneArea*		 m_pArea;
	long				 m_defPos[3];

	// grid system
	// CRegionObject      **m_gridObj;
    TGridObj*           m_gridObj;

	void BuildGrid();
	bool AddObject(CRegionObject *obj,long *pos);
	bool RemoveObject(CRegionObject *obj,long *pos);
	bool GetGridByPos(long *pos,int &x,int &y);
	int m_minX,m_maxX,m_minY,m_maxY;
	int m_gridxNum,m_gridyNum;

	string m_sceneName;
    CG_CmdPacket m_cmdTmp;

	//team AI
	virtual void AddTeam(int key, int num);
	virtual void RemoveTeam(int key, int num);
	virtual int GetTeamNum(int key);
};

class CRegionSceneFB : public CRegionScene
{
public:
	CRegionSceneFB();
	virtual ~CRegionSceneFB();

	// Add npc to this scene [8/27/2009 Louis.Huang]
	virtual CRegionCreature* AddNpc(SSceneNpc& npcInfo, long lLevel = -1, float fAtt[] = 0);

	// CRegionObject interface
 	virtual void OnTimer(ULONG id, DWORD dwParam[]);

	// Event
	virtual void OnCreate();
	virtual void OnRun();
	virtual void OnDestroy();

	virtual void OnUserEnter(CRegionUser *user);
	virtual void OnUserLeave(CRegionUser *user,int reason);

	virtual bool OnTryAddUser(CRegionUser *user);
	virtual void OnCreatureDead(CRegionCreature *cre,CRegionCreature *killer);

	virtual bool AddCreature(CRegionCreature *cre,long *pos,char dir);

	virtual void AddTeam(int key, int num);
	virtual void RemoveTeam(int key, int num);
	virtual int GetTeamNum(int key);

	int m_curLev;
	int m_curDif;
	int m_curUserLV;
	long m_lExitPos[3];
	SFuBen *m_info;
	EXT_SPACE::unordered_map<int,int> m_hash_team;
};

class CRsSceneWarfield : public CRegionScene
{
public:
	CRsSceneWarfield();
	virtual ~CRsSceneWarfield();

	// CRegionObject interface
	virtual void OnTimer(ULONG id, DWORD dwParam[]);

	// Event
	virtual void OnCreate();
	virtual void OnRun();
	virtual void OnDestroy();

	virtual void OnUserEnter(CRegionUser *user);
	virtual void OnUserLeave(CRegionUser *user,int reason);

	virtual bool OnTryAddUser(CRegionUser *user);
	virtual void OnCreatureDead(CRegionCreature *cre,CRegionCreature *killer);

	SWarfieldInfo *m_info;
};

#endif
