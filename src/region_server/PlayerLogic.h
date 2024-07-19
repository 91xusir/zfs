#ifndef __PLAYER_LOGIC_H__
#define __PLAYER_LOGIC_H__

#include <map>
#include <set>
#include <list>
#include <fstream>
#include <strstream>
#include <string>
#include "region.h"

#include "region_object.h"
#include "scene/rts_sv_scene.h"
#include "region_creature.h"
#include "item_base.h"
#include "game_mail_shared_const.h"

struct ItemTaskData
{
	STaskDesc *pTask;
	std::string strArea;
	short nTaskID;
	char cIndex;
};
class CMonsterLogicExt;
class CNewPlayerGuide;
class CPlayerLogic:public IOnItemChange, public IOnConnectionChange
{
public:
	enum ForTaskType{eNo, eList, eRand, eCircle};
	enum LiveType{eLive, eHaveItem, eHaveSkin, eDropSkin, eDead,eFlop};
	CPlayerLogic(CRegionCreature& rPlayer);
	~CPlayerLogic();
	CNewPlayerGuide* GetCNewPlayerGuide();
	SAttackParam& GetSAttackParam();
	unsigned long GetFireTime();
	bool IsIntonating();
	int  StartIntonate(const SAttackParam& Param);
	void StopIntonate();
	bool CanStopSomeOnceIntonating(SSkill* pSkill, CRegionCreature& rPlayer);
	void SetCurUseSkill(SSkill* pCurUseSkill);
	SSkill* GetCurUseSkill();
	bool	IsMiss();
	void	SetMiss(bool bMiss);
	bool	IsTeamFree();
	void	SetTeamFree(bool bTeamFree);
	void    AddtoInviteMap(unsigned long dwDBID);
	void    RemoveFromInviteMap(unsigned long dwDBID);
	bool    IsInInviteMap(unsigned long dwDBID);
	void	OnRun(unsigned long dwTime);
	long    CanFinishState(STaskDesc *task, CRegionCreature* npc);
	void    SharedTask(long nTaskID);
	void	AcceptSharedTask(long nTaskID);
	int		AllMyTeamAddTaskItem(CRegionCreature* pDieNPC, long kk);
	ForTaskType GetForTaskType();
	void SetForTaskType(ForTaskType enType);
	int ProcessTaskKey(STaskKey &key, int op, vector<long> *param, STaskDesc *pDesc = NULL);
	bool	IsTaskDone(int nID);
	LiveType GetLiveType();
	void SetLiveType(LiveType type);
	bool IsLive();
	void Die();
	void ReLive();
	long GetMoveSpeed();
	CMonsterLogicExt* GetMonsterLogicExt();
	void OnGetItem(SItemID item, EItemContainer c, int page, int i, int j);
	bool IsGetingSkin();
	bool IsGetingMedicine();
	bool IsGetingMine();
	void SetGetingSkin(CMonsterLogicExt* pMonsterLogicExt);
	void SetGetingMedicine(CMonsterLogicExt* pMonsterLogicExt);
	void SetGetingMine(CMonsterLogicExt* pMonsterLogicExt);
	void CaneclGetingSkin();
	void CaneclGetingMedicine();
	void CaneclGetingMine();
	bool HaveGetSkinSkill();
	bool IsHaveItem(unsigned long dwID);
	int  GetSkillLevel(unsigned short SubID);
	SSkill*  GetSkill(unsigned short SubID);
	void SendGetSkinError(char cErr);
	void AddLifeSkillExp(int nExp, unsigned short wSkillID);
	void StopGetLifeItem();
	void DonotGetLifeItem();
	int  UseTaskItem(int page, int gridI, int gridJ);
	bool IsDoingTask(int nTaskID);
	bool IsUsingTaskItem();
	void StartUseTaskItem();
	void EndUsingTaskItem();

	void TrackTask(long nTaskID);
	void CaneclTrackTask(long nTaskID);
	int  GetCurMainStory();
	int  GetCurMainStoryID();
	int  SetCurMainStoryID(int nMainStoryDataID);
	void AfterFinishTask(STaskDesc *task);
	int  GetNextMainStoryID();
	void DoGuide(GuideType type);
	void SetOnLineTime(unsigned long dwTime);
	void OnRunNewPlayer(unsigned long dwTime);
	void OnLevelUp();
	void ClientGetOnLineTime();
	unsigned long GetOnLineTime();

	unsigned long GetNextOverHeadTime();
	void SetNextOverHeadTime(unsigned long dwTime);
	unsigned long GetNextGongTaskTime();
	void SetNextGongTaskTime(unsigned long dwTime);
	void AddJobRecord(long nAdd);
	void AddContribute(long nAdd);
	void AddUnionRecord(int add);
	void AddGold(int nAdd);
	void OnUseItem(SItemID item);
	unsigned long GetCanUseYaoPingHpTime();
	unsigned long GetCanUseYaoPingMpTime();
	unsigned long GetCanUseYaoPingPetHpTime();
	unsigned long GetCanUseYaoPingPetMpTime();
	unsigned long GetCanUseYaoPingEnergyTime();
	void SetCanUseYaoPingHpTime(unsigned long dwTime);
	void SetCanUseYaoPingMpTime(unsigned long dwTime);
	void SetCanUseYaoPingPetHpTime(unsigned long dwTime);
	void SetCanUseYaoPingPetMpTime(unsigned long dwTime);
	void SetCanUseYaoPingEnergyTime(unsigned long dwTime);
	bool IsInPShop();
	short FindInRandNPCMap(unsigned long dwNPCID);
	void  AddInRandNPCMap(unsigned long dwNPCID, short nTaskID);

	bool IsAroundMe(CRegionCreature* pCre);
	void OnPetChange();
	void OnTrumpLevUp(const SItemID &idS, int nNewLev);
	void OnSkillNumChange();
	void OnFriendNumChange();

	virtual void OnSetItem(const SItemID &idS, const SItemID &idD);
	virtual void OnRemoveItem(const SItemID &idS);

	virtual void OnInsert(unsigned short int vConnection, DWORD vDBID);
	virtual void OnErase(unsigned short int vConnection, DWORD vDBID);
	ETaskOverHead GetTaskOverHead(DWORD npcOID);
	void OnAreaEnter(const char* szAreName);
private:
	enum {eEmptyDBID = 0, c_nNewNum = 7};
	typedef std::map<unsigned long, unsigned long> InviteMap;
	InviteMap& GetInviteMap();

	typedef std::map<unsigned long, short> NPCTaskMap;
	NPCTaskMap& GetNPCTaskMap();
	

	CMonsterLogicExt* GetGetingSkin();
	CMonsterLogicExt* GetGetingMine();
	CMonsterLogicExt* GetGetingMedicine();

	void SendNewPlayerMail(int n);
	void OnItemChangeForTask(const SItemID &idS);
private:
	CRegionCreature& m_rPlayer;
	SAttackParam	m_SAttackParam;
	unsigned long   m_dwFireTime;
	unsigned long	m_dwEndUsingTaskItemTime;
	SSkill* m_pCurUseSkill;
	bool	m_bMiss;
	bool	m_bTeamFree;
	InviteMap m_InviteMap;
	ForTaskType m_enForTaskType;
	LiveType	m_LiveType;
	CMonsterLogicExt* m_pMonsterLogicExt;

	unsigned long m_dwGetingSkinID;
	unsigned long m_dwGetingMedicineID;
	unsigned long m_dwGetingMineID;
	unsigned long m_dwNextOverHeadTime;
	unsigned long m_dwNextGongTaskTime;//time_t型（秒）
	unsigned long m_dwCanUseYaoPingHpTime;
	unsigned long m_dwCanUseYaoPingMpTime;
	unsigned long m_dwCanUseYaoPingPetHpTime;
	unsigned long m_dwCanUseYaoPingPetMpTime;
	unsigned long m_dwCanUseYaoPingEnergyTime;

	ItemTaskData* m_pUsingItem;
	int m_nCurMainStory;
	unsigned long long m_dwOnLineTime;
	unsigned long m_dwEnterTime;
	CNewPlayerGuide* m_NewPlayerGuide;
	int m_nNewW;

	static unsigned long s_Time[c_nNewNum];
	//MakeTeamType m_enMakeTeamType;
	NPCTaskMap m_NPCTaskMap;
};

class CNpcTaskLogicExt
{
public:
	CNpcTaskLogicExt(CRegionNpcTask& rNPC);
	int ShowTaskList(CRegionCreature *target);
	int DefTalk(CRegionCreature *target);
	int ProcessTask(CRegionCreature *target, const char *str);
	STaskDesc* FindTaskInAccept(int nID);
	STaskDesc* FindTaskInFinish(int nID);
	void	OnRun(unsigned long dwTime);
	char*	GetDefTalk();
	int		GetDefTalkLen();
	void	AcceptTaskByID(CRegionCreature *target, short nID);
	void	FinishTaskByID(CRegionCreature *target, short nID, long nIndex);
	bool	IsInFinishTask(short nID);
	bool	IsInAcceptTask(short nID);
private:
	int SendList(CRegionCreature *target);
	int ProcessRandTask(CRegionCreature *target);
	int GetCanFinishShowTask(CRegionCreature *target, STaskDesc** ppBuff, int nBufNum, bool bMain);
	int GetDoingShowTask(CRegionCreature *target, STaskDesc** ppBuff, int nBufNum, bool bMain);
	int GetAcceptShowTask(CRegionCreature *target, STaskDesc** ppBuff, int nBufNum, bool bMain);
	int GetShowTask(CRegionCreature *target, STaskDesc** ppBuff, int nBufNum);
	int ProcessFinishTask(CRegionCreature *target, STaskDesc* pDesc);
	int ProcessAcceptTask(CRegionCreature *target, STaskDesc* pDesc);
	int ProcessDingTask(CRegionCreature *target, STaskDesc* pDesc);
public:
	int GetTaskList(std::ostrstream &os, CRegionCreature *target, bool bMain, int &nFinish, int &nDoing, int &nAccept, bool bWriteText = true);
	int DefTalkRand(std::ostrstream &os, CRegionCreature *target, int &nFinish, int &nDoing, int &nAccept, bool bWriteText = true);
private:
	typedef std::set<STaskDesc*> SDSet;
	SDSet& GetSDSet();
private:
	SDSet m_SDSet;
	CRegionNpcTask& m_rNPC;
	enum{eMaxTexSize = 1024 * 512, eTempBuffSize = 2048};
	//static char s_m_DefTalkBuff[eMaxTexSize];
	static int  s_m_nCurIndex;
	//static char s_m_TempBuff[eTempBuffSize];
	S_MEMDEF(s_m_DefTalkBuff, eMaxTexSize)
	S_MEMDEF(s_m_TempBuff, eTempBuffSize)
	char*	m_pTalk;
	int		m_nTalkLen;
};

class CRegionSceneFB;
class CMonsterLogicExt
{
public:
	friend class CPlayerLogic;
	CMonsterLogicExt(CRegionCreature& rPlayer);
	~CMonsterLogicExt();
	CPlayerLogic::LiveType DropItemS(CRegionCreature *killer, CRegionSceneFB* pFB = NULL);
	CPlayerLogic::LiveType DropMineMedicineS(/*CRegionCreature *killer*/);
	int	DropTaskLifeItem();
	int PickMyItem(CRegionCreature *Picker, unsigned long dwID);
	int PickAllMyItem(CRegionCreature *Picker);
	int SendItemList(CRegionCreature *Picker, CG_CmdPacket* packet);
	void SetEraseTime(unsigned long dwTime);
	void SetGetSkinTime(unsigned long dwTime);
	int  GetSkin(CRegionCreature* pGetSkin);
	int  GetMine(CRegionCreature* pGetMine);
	int  GetMedicine(CRegionCreature* pGetMedicine);
	bool IsBeGetingSkin();
	bool IsBeGetingMine();
	bool IsBeGetingMedicine();
	void OnRun(unsigned long dwTime);
	void GetSkinRun(unsigned long dwTime);
	void GetMineRun(unsigned long dwTime);
	void GetMedicineRun(unsigned long dwTime);
	void OnErase();
	void CaneclGetSkin();
	void CaneclGetMine();
	void CaneclGetMedicine();
	void PickSkin(CRegionCreature* pGetSkin);
	int  GetLifeItemLevel();//所掉生活物品的等级
	bool IsMineNPC();
	bool IsMedicineNPC();
	bool IsLifeSkillItemNPC();
	void OnClosePickwindow(CRegionCreature *pPlayer);
	void UpdateTaskKillMonsterNum(CRegionCreature *killer);
private:
	typedef std::list<SItemID> ItemList;
	enum {eEmptyDBID = -1, LifeItemNPCReLiveTime = 45000};
private:
	ItemList& GetItemList();
	int InTeamCanPick(CRegionCreature *Picker);
	CPlayerLogic::LiveType UpdateItemS();
	void OnBeGetingLifeItemOK(int nExp, unsigned short wSkillID);
	CRegionCreature* GetGetingSkin();
	CRegionCreature* GetGetingMine();
	CRegionCreature* GetGetingMedicine();
	CRegionCreature* GetCreature(unsigned long dwID);
	void GettingMineRun(unsigned long dwTime);
	void GettingMedicineRun(unsigned long dwTime);
	void PickMineRun(unsigned long dwTime);
	void PickMedicineRun(unsigned long dwTime);
	void UpdateTaskKillMonsterNumOnce(CRegionCreature *pWho, int nTaskID);
private:
	CRegionCreature& m_rPlayer;
	ItemList m_ItemList;
	SItemID  m_SkinItem;//皮

	unsigned long m_dwEraseTime;
	unsigned long m_dwGetSkinID;//正在剥皮的人
	unsigned long m_dwSkinTime;//剥皮完成时间
	unsigned long m_dwGetMineID;//正在采矿的人
	unsigned long m_dwMineTime;//采矿完成时间
	unsigned long m_dwGetMedicineID;//正在采药的人
	unsigned long m_dwMedicineTime;//采药完成时间

	unsigned long m_dwCloseTime;

	unsigned long m_dwKillerDBID;
	int		m_nLifeSkillExp;
};

class CNewPlayerGuide:public IHpMpChange
{
public:
	CNewPlayerGuide(CRegionCreature& rPlayer);

	virtual void OnHpChange(int nNewHp);
	virtual void OnMpChange(int nNewHp);
	virtual void OnWarScoresChange(float fOldWarScores, float fNewWarScores);

	bool HitMove();
	bool NPCTaskTalk();
	bool NPCFinishTask();
	bool AttrButFlash();
	bool FirstAttackMonster();
	bool EatHP();
	bool EatMP();
	bool UsePet1();
	bool BuyItem1();
	bool FaBao1();
	bool Mail1();
	bool LifeSkill1();
	bool DoGuide(GuideType type);
	void SendGuide(GuideType type);
private:
	CRegionCreature& m_rPlayer;
};

class MainStoryIDCmp
{
public:
	bool operator () (MainStoryData* pData1, MainStoryData* pData2) const;
};

class CMainStoryMgr
{
public:
	int BuildMgr();
	MainStoryData* GetMainStoryByID(int nID);
	MainStoryData* GetMainStoryByTaskID(int nTaskID);
	int GetMainTaskIDByJobAndIndex(int nJob, int nIndex);
	int GetIndexByMainTaskIDAndJob(int nJob, int nID);
	int GetMainStoryNum(int nJob);
	static CMainStoryMgr& GetMgr();
	enum{eJobNum = 4, eMaxMainTaskNum = 1024};
private:
	typedef std::set<MainStoryData*, MainStoryIDCmp> MainStoryIDSet;
	typedef std::map<int, int> MainStoryTaskIDSet;

	MainStoryIDSet& GetMainStoryIDSet();
	MainStoryTaskIDSet& GetMainStoryTaskIDSet();
private:
	MainStoryIDSet m_MainStoryIDSet;
	MainStoryTaskIDSet m_MainStoryTaskIDSet;
	int m_MainTask[eJobNum][eMaxMainTaskNum];
	int m_MainTaskNum[eJobNum];
};

class CRSLogicExt
{
public:

	void OnRun(DWORD dwTime);
	void InitTaskDesc();
	int GetItemTaskData(unsigned long nIndex, ItemTaskData** ppBuff, int nBuffNum);
	int GetNPCTaskData(unsigned long nIndex, STaskDesc** ppBuff, int nBuffNum);
	void SelectedByDBID(CRegionCreature* pFind, unsigned long dwDBID);
	const char* DoClientCommand(CRegionCreature *cmdGiver,const char* szCommand, bool bPyGMOP, long pyGmAccount, bool& bProce);
	static CRSLogicExt& GetRSLogicExt();
private:
	typedef std::multimap<unsigned long, ItemTaskData> ItemTaskDataMap;
	ItemTaskDataMap m_ItemTaskDataMap;
	typedef std::multimap<unsigned long, STaskDesc*> NPCTaskMap;
	NPCTaskMap m_NPCTaskMap;
private:
	ItemTaskDataMap& GetItemTaskDataMap();
	NPCTaskMap& GetNPCTaskMap();
	bool ProcessTaskKey(STaskKey &key, STaskDesc* pDesc, char &cIndex);
public:
	typedef std::multimap<unsigned long, unsigned long> ItemFinishTaskMap;
	ItemFinishTaskMap& GetItemFinishTaskMap();
private:
	ItemFinishTaskMap m_ItemFinishTaskMap;
};
#endif//__PLAYER_LOGIC_H__

