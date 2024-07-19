
#ifndef _INC_UI_FORM_WARFIELD_H_
#define _INC_UI_FORM_WARFIELD_H_

struct SExploit
{
	DWORD	dwUnionID;
	char	faction;	//阵营
	long	lScore;		//积分
	long	lExploit;	//战功
	std::string unionName;

	SExploit()
	{
		dwUnionID = 0;
		faction = FACTION_UNKNOWN;
		lScore = 0;
		lExploit = 0;
	}

	bool Serialize(CG_CmdPacket *cmd)
	{
		*cmd << unionName;
		*cmd << faction;
		*cmd << lExploit;

		if (cmd->IsError())
			return false;
		else 
			return true;
	}

	//bool operator> (const SExploit& rhs)
	//{
	//	return (lExploit > rhs.lExploit);
	//}
};

inline bool operator> (const SExploit& lhs, const SExploit& rhs)
{
	return (lhs.lExploit > rhs.lExploit);
}

struct SCNWar
{
	SWarfieldInfo		stInfo;
	long				lNumZhou;
	long				lNumShang;
	std::list<SExploit>	listExploit;		// 战功列表
	time_t				tRemainTime;		// 剩余时间
	float				lScoreZhou;			// 周朝得分
	float				lScoreShang;		// 商朝得分
	float				lPriHero[3];		// 初级神仙获得的资源
	float				lInterHero[3];		// 中级神仙获得的资源
	float				lSeniorHero[3];		// 高级神仙获得的资源
	float				fRZhou;
	float				fRShang;

	SCNWar()
	{
		listExploit.clear();
		tRemainTime = 0;
		lScoreZhou = 0;
		lScoreShang = 0;
		fRZhou		= 1.0;
		fRShang		= 1.0;

		lPriHero[0] = lPriHero[1] = 0;
		lInterHero[0] = lInterHero[1] = 0;
		lSeniorHero[0] = lSeniorHero[1] = 0;

		lPriHero[2] = 1000*0.6;
		lInterHero[2] = 2000*0.6;
		lSeniorHero[2] = 3000*0.6;
	}

	~SCNWar()
	{
		//if(pScene)
	}
};

class UIFormWarfield
{
public:
	UIFormWarfield();
	~UIFormWarfield(){};

public:
	void Refresh();
	void Show();
	void Hide();
	bool IsVisible();
	bool IsInInfo();
	void Clear();

public:
	void OnRecvUpdateData(	long lRemain, long numZhou, float lScoreZhou, long numShang, float lScoreSHang,
							float* lPriHero, float* lInterHero, float* lSeniorHero,
							std::list<SExploit>* pExploit, float fRZhou, float fRShang);

	void OnBtnInfo(RtwWidget* sender, void*);
	void OnBtnList(RtwWidget* sender, void*);

	void OnReport(long lReason, char faction, long numZhou, float lScoreZhou, long numShang, float lScoreSHang,
				float* lPriHero, float* lInterHero, float* lSeniorHero,
				std::list<SExploit>* pExploit, float fRZhou, float fRShang);
	void Report();

private:
	void OnPreDraw();

public:
	RtwWidget*	m_pFrmThis;		// 整个窗口
	RtwLabel*	m_pLabelTime;
	RtwLabel*	m_pLabelTitle;
	RtwTab*		m_pTab;
	RtwListBox*	m_pListZhou;
	RtwListBox*	m_pListShang;
	RtwButton*	m_pBtnInfo;
	RtwButton*	m_pBtnSort;
	RtwLabel*	m_pLabelNumZhou;
	RtwLabel*	m_pLabelNumShang;
	RtwLabel*	m_pLabelScoreZhou;
	RtwLabel*	m_pLabelScoreShang;
	RtwLabel*	m_pLabelRateZhou;
	RtwLabel*	m_pLabelRateShang;
	RtwProgressBar*		m_pProgressZhou1;
	RtwProgressBar*		m_pProgressZhou2;
	RtwProgressBar*		m_pProgressZhou3;
	RtwProgressBar*		m_pProgressShang1;
	RtwProgressBar*		m_pProgressShang2;
	RtwProgressBar*		m_pProgressShang3;

	RtwLabel*	m_pLabelZhou1Count;
	RtwLabel*	m_pLabelZhou2Count;
	RtwLabel*	m_pLabelZhou3Count;
	RtwLabel*	m_pLabelShang1Count;
	RtwLabel*	m_pLabelShang2Count;
	RtwLabel*	m_pLabelShang3Count;

	SCNWar		m_stWarInfo;
	bool		m_bReport;
	char		m_winFaction;
};

class CWarfieldNetHelper
{
public:
	static bool NetSend_c2r_warfield_view();
};

#endif // _INC_UI_FORM_WARFIELD_H_