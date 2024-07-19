#ifndef _INC_GW_NATIONALWAR_H_
#define _INC_GW_NATIONALWAR_H_

/*****************************************************************************/
#define WF_T					3			//��ս����ʱ�䣨Сʱ��
#define WF_NUM_LIMIT			200			//��ս��������
#define WF_END_TIMELIMIT		0			//ʱ�䵽����ս����
#define WF_END_FLAGDESTROYED	1			//���챻�ݻ٣���ս����
#define WF_SCORE_MAX			0xffff		//�������
/*****************************************************************************/
#define WF_APP					0			//�������������б�
#define WF_IN					1			//�������Բ�ս�б�
#define WF_ALL					2			//�����������й�ս������
/*****************************************************************************/
#define WF_JOIN_NUM				5			//ÿ�δ�������
/*****************************************************************************/
#define WF_HSCORE_P				1000*0.6	//�������ɷ���
#define WF_HSCORE_I				2000*0.6	//�м����ɷ���
#define WF_HSCORE_S				3000*0.6	//�߼����ɷ���
/*****************************************************************************/
#define WF_EXP_20_40(lev)	g_TableExp.GetNextLevExp((lev)) * 20 / 100  //20-40
#define WF_EXP_40_60		2000000										//40-60
#define WF_EXP_60_80		5000000										//60-80
#define WF_EXP_80_100		40000000									//80-100
/*****************************************************************************/
struct SExploit
{
	DWORD	dwUnionID;
	char	faction;	//��Ӫ
	long	lScore;		//����
	long	lExploit;	//ս��

	SExploit()
	{
		dwUnionID = 0;
		faction = FACTION_NEUTRAL;
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

class CGwNationalWarMgr
{
public:
	enum EState
	{
		eStarting		= 0,
		eWaitStarting	= 1,
		eStop			= 2,
		eWaitStopping	= 3
	};
private:
	struct SUser
	{
		DWORD dwUserID;                // �û���ID
		bool  bInNWar;                 // �Ƿ��ڹ�ս����
		DWORD dwTimeout;			   // ���߳�ʱ����
		DWORD dwLevel;				   // ��ҵȼ�

		SUser()
		{
			dwUserID = 0;
			bInNWar = false;
			dwTimeout = 0;
			dwLevel = 0;
		}
	};

	struct SAppUser
	{
		DWORD	dwUserID;               // �û���ID
		time_t	tStartTime;				// ��ʼ�Ŷ�ʱ��
	};

	typedef std::list<SAppUser>		TLSApp;
	typedef std::list<SUser>		TLSUser;
	typedef std::list<SExploit>		TLSExploit;
	typedef std::list<SPerExploit>	TLSPerExploit;

	struct SWarField
	{
		long				lRsidx;				// ս������region������
		EState				eState;				// ��ʼ��
		TLSApp				listAppZhou;		// �����б���
		TLSApp				listAppShang;		// �����б���
		TLSUser				listUserZhou;		// ��ս�б���
		TLSUser				listUserShang;		// ��ս�б���
		TLSExploit			listExploit;		// ����ս���б�
		TLSPerExploit		listPersonExploit;	// ����ս���б�
		time_t				tStartTime;			// ��ʼʱ��
		int					iDuration;			// ����ʱ�䣨Сʱ��
		time_t				tEndTime;			// ����ʱ��
		float				lScoreZhou;			// �ܷ���
		float				lScoreShang;		// �̷���
		float				lPriHero[3];		// �������ɻ�õ���Դ
		float				lInterHero[3];		// �м����ɻ�õ���Դ
		float				lSeniorHero[3];		// �߼����ɻ�õ���Դ
		SWarfieldInfo		stInfo;				// ս��������Ϣ
		float				fRateZhou;			// ��ս������
		float				fRateShang;			// ��ս������

		SWarField()
		{
			lRsidx			= 0;
			eState			= eStop;
			tStartTime		= 0;
			iDuration		= WF_T;
			tEndTime		= 0;
			lScoreShang		= 0.f;
			lScoreZhou		= 0.f;
			fRateZhou		= 1.0;
			fRateShang		= 1.0;
			lPriHero[0]		= lPriHero[1]	 = 0.f;
			lInterHero[0]	= lInterHero[1]	 = 0.f;
			lSeniorHero[0]	= lSeniorHero[1] = 0.f;

			lPriHero[2]		= WF_HSCORE_P;
			lInterHero[2]	= WF_HSCORE_I;
			lSeniorHero[2]	= WF_HSCORE_S;

			listAppZhou.clear();
			listAppShang.clear();
			listUserZhou.clear();
			listUserShang.clear();
			listExploit.clear();
			listPersonExploit.clear();
		}

		void Reset()
		{
			eState			= eStop;
			iDuration		= WF_T;
			lScoreShang		= 0.f;
			lScoreZhou		= 0.f;
			fRateZhou		= 1.0f;
			fRateShang		= 1.0f;
			lPriHero[0]		= lPriHero[1]	 = 0.f;
			lInterHero[0]	= lInterHero[1]	 = 0.f;
			lSeniorHero[0]	= lSeniorHero[1] = 0.f;
			lPriHero[2]		= WF_HSCORE_P;
			lInterHero[2]	= WF_HSCORE_I;
			lSeniorHero[2]	= WF_HSCORE_S;

			listAppZhou.clear();
			listAppShang.clear();
			listUserZhou.clear();
			listUserShang.clear();
			listExploit.clear();
			listPersonExploit.clear();
		}
	};

public:
	CGwNationalWarMgr();
	~CGwNationalWarMgr();

public:
	bool	Init();
	bool	Load(const char* file);
	bool	Start();
	bool	Stop();
	void	Run();
	bool	CanStart();
	int		EnterWarfield(DWORD dwUserID);
	void	GetStartTime(time_t& ret);
	void	BroadcastSystemMsg(const char* s_szMsg);		//�����
	void	BroadcastWarfieldMsg(int toWho, int iMsgID);	//�����
	void	SendWarfieldMsg(long dwUserID, int iMsgID);		//�����
	int		GetUserNum(int faction);
	void	BroadcastCmd();
	void	BroadcastUpdateData();	//��region
	bool	Log(DWORD dwUserID);
	SUser*	FindUser(DWORD dwUserID);
	bool	FindUserInApp(DWORD dwUserID);
	bool	RemoveUserInApp(DWORD dwUserID);
	void	CalcRate();
	void	UpdateRateToRs();
	void	AddAppUserToWarfield();
	//void	ClearDB();
	void	AddHeroItem(char faction);
	void	AddHeroItem(char faction, DWORD dwUnionID, int lev);

public:
	void	OnRsStartRep(bool bResult);
	void	OnUserLeave(DWORD dwUserID);
	void	OnRsStopRep(bool bResult);
	void	OnRegionRegister(int regionID);
	void	OnRegionDisconnected(int regionID);
	void	OnUserOffline(DWORD dwUserID);
	void	OnAddScore(char faction, float score);
	void	OnUserOnline(DWORD dwUserID);
	void	OnRecvConjureProgress(char faction, float progress, long lev);
	void	OnRecvExploit(char faction, long lUnionID, long lExploit);
	void	OnRecvPerExploit(DWORD dwUserID, DWORD dwExploit);
	void	OnRecvUpdateData();
	void	OnAddExp(DWORD dwUserID, char faction);
	void	OnAddPersonExploit(DWORD dwUserID, long lExploit);
	//void	OnCreateArms(DWORD dwUserID, long dwLev, long lType);

	void	OnGMStart();
	void	OnGMStop();

public:
	SWarField		m_stWarField;
	const char*		m_szErrorInfo;  
	bool			m_bCanStart;
	bool			m_bHad;			//�������Ƿ��Ѿ����
	bool			m_bFlagDestroyedZhou;
	bool			m_bFlagDestroyedShang;
	char			m_Winner;

	//-----------------------------
	//Ĭ�Ϲ�սʱ��
	//-----------------------------
	int m_default_weekday;

	int m_default_hour;

	int m_default_min;

	int m_default_duration;
};

extern CGwNationalWarMgr g_GWWarfieldMgr;

#endif //_INC_GW_NATIONALWAR_H_
