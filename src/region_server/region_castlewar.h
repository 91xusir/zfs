//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#ifndef REGION_CCASTLEWARMGR_H
#define REGION_CCASTLEWARMGR_H

using namespace std;

struct SFeod;

//#define WAR_SWITCH   //tim.yang   屏蔽城战

#define PREPARE_TIME	5

typedef struct{
	string feod_name;	
	string area_name;	
}STCASTLEWAR_AREA_NAME;

class CRegionCastleWarMgr
{
public:
	CRegionCastleWarMgr();
	~CRegionCastleWarMgr();

public:
	
	bool isStartWar;//tim.yang  城战是否已经开始
	//-------------------------------
	//数据操作相关
	//-------------------------------

	/* 初始化（待修改，暂根据读取文件信息） */
	void Init();

	DWORD GetCurrTime() { return m_current_time; }

	int GetDurationTime(int index) { return m_duration[index]; }

	/* 根据采邑名称查找该采邑相关 */
	STWarInfo* FindByName(string name);

	/* b_side:攻城方true  返回值NULL表示中立 守城方未被批准的也算中立 */
	STWarInfo* FindByUnionID(DWORD union_id, bool* b_side = NULL);

	/* 根据城战相关的(包括守城方未被批准的)诸侯国id查找所有信息 */
	STWarInfo* FindAllByUnionID(DWORD union_id);
	
	/* b_side:攻城方true 返回值NULL出错 */
	STWarInfo* FindByFlagID(DWORD flag_id, DWORD* union_id = NULL, bool* b_side = NULL);
	
	/* 修改城主信息 */
	bool UpdateMasterInfo(string name, DWORD union_id);

	/* 修改状态 */
	bool UpdateState(string name, EState state);

	/* 不修改令旗相关，只修改诸侯国数量 */
	bool UpdateAttackerInfo(string name, DWORD union_id, BYTE op);

	/* 不修改令旗相关，只修改诸侯国数量 */
	bool UpdateDefenderInfo(string name, DWORD union_id, BYTE op);
	
	/* 重置采邑相关的攻守城双方所有成员城战信息 */
	void ReSetAllUserWarInfo(STWarInfo* p_war_info);
	
	/* 城主设置下次城战时间 */
	bool SetStartTime(string name, short week_day);

	/* 设置默认攻城时间 */
	void GetCastleWarDefaultStartTime(DWORD& ret, int index);

	/* 诸侯国解散处理 */
	void OnUnionDisband(DWORD union_id);

	/* 诸侯换人处理 */
	void OnUnionLeave(DWORD union_id);

	/* 玩家上线处理 */
	void OnUserOnline(DWORD playerDB_id, bool b_flag);

	/* 玩家上线处理 */
	void OnUserOnline(CRegionCreature* p_cre);

	/* 数据初始化（region注册时进行）*/
	void OnRecvInitData(STWarInfo* p_war_info);

	/* 数据更新 */
	void OnRecvUpdateData(STWarInfo* p_war_info);

	//-------------------------------
	//令旗相关
	//-------------------------------
	
	/* p_sender_cre：插令旗的诸侯 */
	bool CreateFlagNpc(CRegionCreature* p_sender_cre);

	/* p_flag_npc ：被杀死的令旗npc
	 * b_is_killed: true  被玩家杀死的
	 *				false 城战结束重置数据
	 */
	bool KillFlagNpc(CRegionCreature* p_flag_npc, bool b_is_killed);

	bool CanInsertFlag(CRegionCreature* p_sender_cre);

	/* 根据诸侯国ID查找令旗 true表示找到令旗 flag_id = oxffff表示已被打掉 */
	bool FindFlagByUnionID(DWORD union_id, DWORD& flag_id);

	/* 查找周围一定范围内有无令旗 */
	bool FindHaveFlagAround(CRegionCreature* p_sender_cre);

	/* 创建令旗npc后，该npc与创建者绑定（不是令旗道具）*/
	bool AddFlag(DWORD union_id, DWORD flag_id);

	/* 打掉令旗npc后，该npc与创建者取消绑定（不是令旗道具）*/
	bool DelFlag(DWORD flag_id, bool b_is_killed);

	bool IsFlagNpc(short flag_id);

	/* 重置 */
	void ClearAllFlagNpc(int index);

	//-------------------------------
	//城市守护兽相关
	//-------------------------------
	bool CreateGuardAnimalNpc(string feod_name);

	bool KillAnimalNpc(CRegionCreature* p_killer, CRegionCreature* p_self);

	bool IsGuardAnimal(short animal_id);

	//---------------------------------
	//规则相关
	//---------------------------------
	bool CanAttack(DWORD target_type, CRegionCreature* p_attacker, DWORD target_id);
	
	bool Rebirth(CRegionCreature* p_cre);
	
	/* 某个玩家处于城战时间 */
	bool IsCastleWarTime(CRegionCreature* p_cre);

	/* 某个采邑处于城战时间 */
	bool IsCastleWarTime(string feod_name);

	/* 在城战的相关场景内 */
	bool InCastleWarArea(CRegionCreature* p_cre);

	/* 在城战的时间段内 */
	bool IsInTimeZone(std::string feod_name);

	//---------------------------------
	//守城NPC相关
	//---------------------------------
	bool CreateCastleGuardNpc(std::string feod_name);

	bool KillCastleGuardNpc(std::string feod_name);
	
	//----------------------------------
	//状态控制
	//----------------------------------
	void OnTimer();	
	
public:
	bool LoadCastleWarInfo(const char* file_name);

public:
	void StartWar(string feod_name);

	void EndWar(string feod_name, DWORD winner_id);

	void PauseWar(string feod_name, DWORD winner_id);

	/* 修改诸侯国所有成员的城战信息 1 : 攻城方 0：守城方 */
	bool UpdateCreWarInfo(string name, DWORD union_id, BYTE is_attacker);

	/* 该采邑城战时传送城战区相关玩家 */
	bool TransUser(string feod_name);

	/* 城战时传送某一玩家 */
	bool TransUser(CRegionCreature* p_cre);

	/* 传送玩家至指定位置，自动寻找空位 */
	bool SetPos(CRegionCreature* p_cre, long posx, long posy);

	//void InformAreaUserPrepare(int index);

	//void InformAllRegUserPrepare(int index);

private:
	vector<STWarInfo>	m_war_list;
	DWORD				m_current_time;

	//从文件中读取castlewar_info.csv（以前）
	//从GW消息读取
	int	m_week_day[3];
	int	m_hour[3];
	int	m_min[3];
	int	m_duration[3];	//持续时间（秒）

	//城市守护兽信息唯一索引
	long m_animal_pos[3][3];
	//char m_dir[3];
	CM_MEMDEF(m_dir, 3)
	int  m_animal_id[3];

	//可选重生点 按照索引顺序  每个城有三个随机点  共有三个城
	long m_rebirth_pos[3][3][3];

public:
	//...非必要勿改此变量
	int	 m_flag;
};

extern CRegionCastleWarMgr g_RegionCastleWarMgr;

#endif //REGION_CCASTLEWARMGR_H
