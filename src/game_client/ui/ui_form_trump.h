#pragma once
#include "ui/rtw_ui.h"

#define MAX_PROGRESS_NUM 8

enum TRUMPTYPE
{
	Fire_Att,
	Fire_Def,
	Water_Att,
	Water_Def,
	Poision_Att,
	Poision_Def,
	Undef
};
struct TRUMPINFO 
{
	TRUMPINFO(int lev,int mp,int nowmp,TRUMPTYPE type,int trumpadd,int levnum,int mpnum,int trumpaddnumm):
	m_iLevel(lev),
	m_iMP(mp),
	m_iNowMP(nowmp),
	m_eTrumpType(type),
	m_iTrumpAdd(trumpadd),
	m_iLevNum(levnum),
	m_iMPNum(mpnum),
	m_iTrumpAddNum(trumpaddnumm)
	{
	}
	TRUMPINFO()
	{
		m_iLevel = 0;
		m_iMP = 0;
		m_iNowMP = 0;
		m_eTrumpType = Undef;
		m_iTrumpAdd = 0;
		m_iLevNum = 0;
		m_iMPNum = 0;
		m_iTrumpAddNum = 0;
	}
	int m_iLevel;
	int m_iMP;
	int m_iNowMP;
	TRUMPTYPE m_eTrumpType;
	int m_iTrumpAdd;
	int m_iLevNum;
	int m_iMPNum;
	int m_iTrumpAddNum;
};
class CUIForm_Trump
{
public:
	CUIForm_Trump(void);
	~CUIForm_Trump(void);
	void Show();
	void Hide();
	void StartRifine();
	bool IsVisiable();
	void DoFrame();
	void OnClose(RtwWidget* sender, RtwEventDelegate* pEvent);
	void OnLClickRifine(RtwWidget* sender, RtwEventDelegate* pEvent);
	void OnGetHintText(RtwWidget* sender, RtwEventDelegate* pEvent);
	void OnPreDraw();
	void Refresh();
	void SetItemID(SItemID id);
	SItemID GetItemID() { return m_itemID; };
	void OnDragEndItem(RtwWidget* sender, RtwEventDelegate* pEvent);
	void Tick();
	bool IsRifining(){ return m_bIsRifine; };
	void SetAttachType(int attach);
	char * GetRifineName(int attach);
	void Reset();
	void ReceiveCmd( int rifine, int succeed, int vigour,  int soul);

	int m_iRifine;
private:
	SItemID		m_itemID;

	RtwWidget *m_pfrmTrump;
	RtwButton *m_pbtnTrumpABut;
	RtwWidget *m_plbTrumpLevel;
	RtwWidget *m_plbVigourPoint;
	RtwWidget *m_plbTrumpAttactType;
	RtwWidget *m_plbTrumpAttact;

	RtwProgressBar *m_pproActorSoul;
	RtwWidget *m_pproSoulRate;
	RtwWidget *m_plbNowMoney;

	RtwProgressBar *m_pproTrumpPro[MAX_PROGRESS_NUM];
	RtwWidget *m_plbTrumpInfo[MAX_PROGRESS_NUM];
	RtwWidget *m_plbTrumpRate[MAX_PROGRESS_NUM];
	RtwButton *m_pbtnTrumpButton[MAX_PROGRESS_NUM];

	SCreature m_core;
	float m_iSpeed;
	float m_fGap;	// 扣除的人物灵力值
	bool m_bIsRifine;
	float m_iLastGap;
	long m_lTime;

	float m_iValue;
	float m_interval;
	bool m_bCanFull;
	float m_iSoulNeed;
	
};
