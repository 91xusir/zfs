//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#ifndef REGION_CCASTLEWARMGR_H
#define REGION_CCASTLEWARMGR_H

using namespace std;

struct SFeod;

//#define WAR_SWITCH   //tim.yang   ���γ�ս

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
	
	bool isStartWar;//tim.yang  ��ս�Ƿ��Ѿ���ʼ
	//-------------------------------
	//���ݲ������
	//-------------------------------

	/* ��ʼ�������޸ģ��ݸ��ݶ�ȡ�ļ���Ϣ�� */
	void Init();

	DWORD GetCurrTime() { return m_current_time; }

	int GetDurationTime(int index) { return m_duration[index]; }

	/* ���ݲ������Ʋ��Ҹò������ */
	STWarInfo* FindByName(string name);

	/* b_side:���Ƿ�true  ����ֵNULL��ʾ���� �سǷ�δ����׼��Ҳ������ */
	STWarInfo* FindByUnionID(DWORD union_id, bool* b_side = NULL);

	/* ���ݳ�ս��ص�(�����سǷ�δ����׼��)����id����������Ϣ */
	STWarInfo* FindAllByUnionID(DWORD union_id);
	
	/* b_side:���Ƿ�true ����ֵNULL���� */
	STWarInfo* FindByFlagID(DWORD flag_id, DWORD* union_id = NULL, bool* b_side = NULL);
	
	/* �޸ĳ�����Ϣ */
	bool UpdateMasterInfo(string name, DWORD union_id);

	/* �޸�״̬ */
	bool UpdateState(string name, EState state);

	/* ���޸�������أ�ֻ�޸��������� */
	bool UpdateAttackerInfo(string name, DWORD union_id, BYTE op);

	/* ���޸�������أ�ֻ�޸��������� */
	bool UpdateDefenderInfo(string name, DWORD union_id, BYTE op);
	
	/* ���ò�����صĹ��س�˫�����г�Ա��ս��Ϣ */
	void ReSetAllUserWarInfo(STWarInfo* p_war_info);
	
	/* ���������´γ�սʱ�� */
	bool SetStartTime(string name, short week_day);

	/* ����Ĭ�Ϲ���ʱ�� */
	void GetCastleWarDefaultStartTime(DWORD& ret, int index);

	/* ������ɢ���� */
	void OnUnionDisband(DWORD union_id);

	/* ���˴��� */
	void OnUnionLeave(DWORD union_id);

	/* ������ߴ��� */
	void OnUserOnline(DWORD playerDB_id, bool b_flag);

	/* ������ߴ��� */
	void OnUserOnline(CRegionCreature* p_cre);

	/* ���ݳ�ʼ����regionע��ʱ���У�*/
	void OnRecvInitData(STWarInfo* p_war_info);

	/* ���ݸ��� */
	void OnRecvUpdateData(STWarInfo* p_war_info);

	//-------------------------------
	//�������
	//-------------------------------
	
	/* p_sender_cre������������ */
	bool CreateFlagNpc(CRegionCreature* p_sender_cre);

	/* p_flag_npc ����ɱ��������npc
	 * b_is_killed: true  �����ɱ����
	 *				false ��ս������������
	 */
	bool KillFlagNpc(CRegionCreature* p_flag_npc, bool b_is_killed);

	bool CanInsertFlag(CRegionCreature* p_sender_cre);

	/* ��������ID�������� true��ʾ�ҵ����� flag_id = oxffff��ʾ�ѱ���� */
	bool FindFlagByUnionID(DWORD union_id, DWORD& flag_id);

	/* ������Χһ����Χ���������� */
	bool FindHaveFlagAround(CRegionCreature* p_sender_cre);

	/* ��������npc�󣬸�npc�봴���߰󶨣�����������ߣ�*/
	bool AddFlag(DWORD union_id, DWORD flag_id);

	/* �������npc�󣬸�npc�봴����ȡ���󶨣�����������ߣ�*/
	bool DelFlag(DWORD flag_id, bool b_is_killed);

	bool IsFlagNpc(short flag_id);

	/* ���� */
	void ClearAllFlagNpc(int index);

	//-------------------------------
	//�����ػ������
	//-------------------------------
	bool CreateGuardAnimalNpc(string feod_name);

	bool KillAnimalNpc(CRegionCreature* p_killer, CRegionCreature* p_self);

	bool IsGuardAnimal(short animal_id);

	//---------------------------------
	//�������
	//---------------------------------
	bool CanAttack(DWORD target_type, CRegionCreature* p_attacker, DWORD target_id);
	
	bool Rebirth(CRegionCreature* p_cre);
	
	/* ĳ����Ҵ��ڳ�սʱ�� */
	bool IsCastleWarTime(CRegionCreature* p_cre);

	/* ĳ�����ش��ڳ�սʱ�� */
	bool IsCastleWarTime(string feod_name);

	/* �ڳ�ս����س����� */
	bool InCastleWarArea(CRegionCreature* p_cre);

	/* �ڳ�ս��ʱ����� */
	bool IsInTimeZone(std::string feod_name);

	//---------------------------------
	//�س�NPC���
	//---------------------------------
	bool CreateCastleGuardNpc(std::string feod_name);

	bool KillCastleGuardNpc(std::string feod_name);
	
	//----------------------------------
	//״̬����
	//----------------------------------
	void OnTimer();	
	
public:
	bool LoadCastleWarInfo(const char* file_name);

public:
	void StartWar(string feod_name);

	void EndWar(string feod_name, DWORD winner_id);

	void PauseWar(string feod_name, DWORD winner_id);

	/* �޸��������г�Ա�ĳ�ս��Ϣ 1 : ���Ƿ� 0���سǷ� */
	bool UpdateCreWarInfo(string name, DWORD union_id, BYTE is_attacker);

	/* �ò��س�սʱ���ͳ�ս�������� */
	bool TransUser(string feod_name);

	/* ��սʱ����ĳһ��� */
	bool TransUser(CRegionCreature* p_cre);

	/* ���������ָ��λ�ã��Զ�Ѱ�ҿ�λ */
	bool SetPos(CRegionCreature* p_cre, long posx, long posy);

	//void InformAreaUserPrepare(int index);

	//void InformAllRegUserPrepare(int index);

private:
	vector<STWarInfo>	m_war_list;
	DWORD				m_current_time;

	//���ļ��ж�ȡcastlewar_info.csv����ǰ��
	//��GW��Ϣ��ȡ
	int	m_week_day[3];
	int	m_hour[3];
	int	m_min[3];
	int	m_duration[3];	//����ʱ�䣨�룩

	//�����ػ�����ϢΨһ����
	long m_animal_pos[3][3];
	//char m_dir[3];
	CM_MEMDEF(m_dir, 3)
	int  m_animal_id[3];

	//��ѡ������ ��������˳��  ÿ���������������  ����������
	long m_rebirth_pos[3][3][3];

public:
	//...�Ǳ�Ҫ��Ĵ˱���
	int	 m_flag;
};

extern CRegionCastleWarMgr g_RegionCastleWarMgr;

#endif //REGION_CCASTLEWARMGR_H
