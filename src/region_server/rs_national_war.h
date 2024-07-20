#ifndef _INC_RS_NATIONALWAR_H_
#define _INC_RS_NATIONALWAR_H_

// ��սϵͳ
/*****************************************************************************/
#define WARFLAG_ZHOU		1247
#define WARFLAG_SHANG		1246
#define WF_SCORE_MAX		0xffff
#define WF_ONEHOUR			60 * 60
/*****************************************************************************/
#define WF_HSCORE_P			1000*0.6		//�������ɷ���
#define WF_HSCORE_I			2000*0.6		//�м����ɷ���
#define WF_HSCORE_S			3000*0.6		//�߼����ɷ���
/*****************************************************************************/

class CRegionSpawnMgr;

enum EConjureRet
{
	eSuccess		= 0,	// �Ѿ�����Դ�����ٻ�
	eFailed			= 1,	// ʧ��, ��֪��Ϊʲô
	eHaveConjured	= 2,	// ʧ��, ����ٻ�������(ָ����)
	eTimeLimit		= 3,	// ʧ��, ʱ��δ��
	eOverFlow		= 4,	// ʧ��, ��������
	eNoMoney		= 5,	// ʧ��, ��Ǯ����
};

enum EArmsKind
{
	eBuBing		= 0,	//����
	eGongjian	= 1,	//������
	eJiangjun	= 2,	//����
	eNone
};

const int iArmsPrice[3] = {250000, 500000, 3000000};	//�۸�
const int iArmsRate[3]	= {50, 30, 3};					//ÿСʱ���ٸ�

struct SExploit
{
	DWORD	dwUnionID;
	char	faction;	//��Ӫ
	long	lScore;		//����
	long	lExploit;	//ս��

	SExploit()
	{
		dwUnionID = 0;
		faction = FACTION_UNKNOWN;
		lScore = 0;
		lExploit = 0;
	}

	bool Serialize(CG_CmdPacket *cmd)
	{
		*cmd << dwUnionID;
		*cmd << faction;
		*cmd << lExploit;

		if (cmd->IsError())
			return false;
		else 
			return true;
	}

	bool SerializeToGC(CG_CmdPacket *cmd)
	{
		EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union;
		iter_union = g_region->m_unionMap.find(dwUnionID);
		if(iter_union == g_region->m_unionMap.end())
		{
			return false;
		}

		*cmd << iter_union->second.m_strName;
		//*cmd << dwUnionID;
		*cmd << faction;
		*cmd << lExploit;

		if (cmd->IsError())
			return false;
		else 
			return true;
	}
};

inline bool operator> (const SExploit& lhs, const SExploit& rhs)
{
	return (lhs.lExploit > rhs.lExploit);
}

struct SPerExploit
{
	DWORD	dwUserID;
	DWORD	dwExploit;

	SPerExploit()
	{
		dwExploit = 0;
		dwUserID = 0;
	}
};

struct SArms
{
	EArmsKind	kind;
	char		faction;
	time_t		tStartTime;
	long		lCurrNum;

	SArms()
	{
		kind = eBuBing;
		faction = FACTION_UNKNOWN;
		tStartTime = 0;
		lCurrNum = 0;
	}
};

struct SRsNWar
{
	CRsSceneWarfield*	pScene;
	SWarfieldInfo		stInfo;
	std::list<DWORD>			listUserZhou;		// �ܳ����
	std::list<DWORD>			listUserShang;		// �̳����
	std::list<SExploit>			listExploit;		// ս���б�
	std::list<SPerExploit>		listPersonExploit;	// ����ս���б�
	time_t				tEndTime;			// ����ʱ��(60�뻺��)
	time_t				tRemainTime;		// ʣ��ʱ�䳤�ȣ��룩
	float				lScoreZhou;			// �ܳ��÷�
	float				lScoreShang;		// �̳��÷�
	float				lPriHero[3];		// �������ɻ�õ���Դ
	float				lInterHero[3];		// �м����ɻ�õ���Դ
	float				lSeniorHero[3];		// �߼����ɻ�õ���Դ
	std::list<SArms>	listArms;			// ˫���ٻ���Npc
	float				fRZhou;				// �ܱ���
	float				fRShang;			// �̱���

	SRsNWar()
	{
		pScene = NULL;
		listUserZhou.clear();
		listUserShang.clear();
		listExploit.clear();
		listArms.clear();
		listPersonExploit.clear();
		tEndTime		= 0;
		lScoreZhou		= 0.f;
		lScoreShang		= 0.f;
		tRemainTime		= 0;
		fRZhou			= 1.0;
		fRShang			= 1.0;
		lPriHero[0]		= lPriHero[1]	 = 0.f;
		lInterHero[0]	= lInterHero[1]  = 0.f;
		lSeniorHero[0]	= lSeniorHero[1] = 0.f;
		lPriHero[2]		= WF_HSCORE_P;
		lInterHero[2]	= WF_HSCORE_I;
		lSeniorHero[2]	= WF_HSCORE_S;
	}

	~SRsNWar()
	{
		//if(pScene)
	}

	void Reset()
	{
		listUserZhou.clear();
		listUserShang.clear();
		listExploit.clear();
		listArms.clear();
		listPersonExploit.clear();
		tEndTime		= 0;
		lScoreZhou		= 0.f;
		lScoreShang		= 0.f;
		tRemainTime		= 0;
		fRZhou			= 1.0;
		fRShang			= 1.0;
		lPriHero[0]		= lPriHero[1]	 = 0.f;
		lInterHero[0]	= lInterHero[1]  = 0.f;
		lSeniorHero[0]	= lSeniorHero[1] = 0.f;
		lPriHero[2]		= WF_HSCORE_P;
		lInterHero[2]	= WF_HSCORE_I;
		lSeniorHero[2]	= WF_HSCORE_S;		
	}

	bool Serialize(CG_CmdPacket *cmd);
};

class CRsNwarMgr : public CRegionObject
{
public:
    CRsNwarMgr();
    virtual ~CRsNwarMgr();

public:
	bool Load(const char* file);
	bool Exit();
    
	//������region�Ϸ������¼�
	void Run();

    bool CreateWarfield();
	void Stop();
    void DestroyWarfield();


    // ������
	bool AddUserToNWar(DWORD userId, char cBorn, long x, long y);
	bool _AddUserToNWar(CRegionUser *user, char cBorn, long x, long y);
	
	//Ѱ�����Լ�����ļ��������   
	void FindRebirthPos(long* pos, CRegionCreature* cre);
    //�Ƴ����
	CRegionUser* RemoveUser(CRegionUser *user);
	//�������	
	const char* RequestEnter(CRegionUser *user,char Faction);
	//�����뿪
	bool RequestLeave(CRegionUser *user);
	
	//����
	void Reward();	
	//��Ӿ���
	void AddExp(DWORD dwUserID, char faction);	
	//��Ӹ���ս��������gw
	void AddPersonExploit(DWORD dwUserID, DWORD dwExploit);	
	//�������ս��
	void AddExploit(DWORD dwUnionID, char faction, long lExploit);	
	//��ʱ��Ӹ���ս��
	void AddPerTmpExploit(DWORD dwUserID, DWORD dwExploit, bool IsWinnerReward = false);
	//��ʱ�������ս��
	void AddTmpExploit(DWORD dwUnionID, char faction, DWORD dwExploit);
	//��Ӹ��˻���(���ۼӵ�����Ӫ������)��ͬʱ�������Ӧս��
	void AddUserScore(DWORD dwUserID, long res);
	//�����Ӫ����
	void AddScore(char faction, float score);
	//���±���
	void UpdateRate(float fRZhou, float fRShang);

	void TransmitMsg(long dwUserID, long iMsgID);
	long Distance(long *pos, CRegionCreature* cre);	
	bool IsInWarfield(CRegionCreature* p_cre);		//��ɫ��ս����
	bool CreateRebirthNpc(SRebirthPos* pos);
	bool RemoveNpc(long lObjID);
	bool RemoveNpc(CRegionCreature* cre);
	bool AddNpc(SRebirthPos* pos);
	void TransUser();

	//����������
	void UpdateClientData(CRegionCreature* CmdGiver);
	//��������������
	void SendUpdateData();
	//ս������ʱ����ս��
	void SendReport();

	//����ս������
	void SaveDataToGW();

	EConjureRet	 ConjureHero(long dwUserID, long dwResource, int lev);
	EConjureRet	 ConjureHero(char faction, long dwResource, int lev);
	EConjureRet	 ConjureArms(long dwUserID, EArmsKind kind);
	
	//check
	bool	CanConjureHero(char faction, int lev);		//�ɷ��ٻ�����
	bool	IsRebirthNpc(short npcID);
	bool	IsFlagNpc(short npcID);
	EArmsKind	IsArmsNpc(short npcID);

private:
	//�����������(���ۼӵ�����Ӫ������)
	//void AddUnionScore(DWORD dwUnionID, char faction, long score);
	void Reward(char faction);

public:
    // 
    bool OnNWarUserLeave(CRegionUser *user);

	void OnFlagIsDestroy(long flag_id);

	void OnUserDead(CRegionCreature* cre);

	void OnRebirthPosKilled(CRegionCreature* cre, CRegionCreature* killer);

	void OnRebirthPosBeAttacked(CRegionCreature* cre);

	void OnFlagBeAttacked(long flag_id, CRegionCreature* cre);

	void OnConjureHero(char faction, int lev);	//�����ٻ�������

	void OnConjureArms(CRegionCreature* pUser, char faction, EArmsKind kind);	//�ٻ�����ͨ����npc

	void OnRecvAddScore(float lScoreZhou, float lScoreShang);

    void BroadcastSystemMessage(char* szMessage, char faction = FACTION_ALL);

	void BroadcastMessage(char* szMessage, BYTE channel, long from_id, const char* from_name, char faction = FACTION_ALL);

	void OnRecvUpdateData(  long lRemain, long numZhou, std::list<DWORD>* pUserZhou, float lScoreZhou, long numShang, std::list<DWORD>* pUserShang, float lScoreSHang,
							float* lPriHero, float* lInterHero, float* lSeniorHero,
							std::list<SExploit>* pExploit, std::list<SPerExploit>* pPerExploit);

public:
    static int s_iMaxLastQuitTime;

public:
    //char			m_szMsg[256];
	CM_MEMDEF(m_szMsg, 256)
	SRsNWar*		m_pWarfield;
	bool			m_bFlagDestroyedZhou;
	bool			m_bFlagDestroyedShang;
	bool			m_bIsOpen;				//ս���߼����Ƿ���
	bool			m_bIsWaitingStop;		/* ս��ʵ�����Ƿ���
											 * ���m_bIsOpen==false�����������ر�
											 * �˱���Ϊtrueʱ��ʾ���ڽ���ս������Ԥ����
											 */
	CRegionSpawnMgr* m_pSpawnMgr;
	bool			m_bCanTrans;

	//..................
	int	m_iNumZhou;
	int m_iNumShang;
 };

extern CRsNwarMgr	g_WarfieldMgr;

#endif // _INC_RS_NATIONALWAR_H_
