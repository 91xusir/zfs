//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#ifndef _INC_UI_FORM_CASTLEWAR_H_
#define _INC_UI_FORM_CASTLEWAR_H_

#include "castlewar_base.h"

#define STRING_NPC_OCCUPY		R(MSG_CASTLEWAR_NPC_OCCUPY)//"无人占领"
#define STRING_REGISTERED		R(MSG_CASTLEWAR_REGISTERED)//"已注册"
#define STRING_WAIT_FOR_ALLOWED R(MSG_CASTLEWAR_WAIT_FOR_ALLOWED)//"待批准"
#define STRING_IS_ALLOWED		R(MSG_CASTLEWAR_IS_ALLOWED)//"已批准"
#define STRING_FEOD_MASTER		R(MSG_CASTLEWAR_FEOD_MASTER)//"城主"
#define STRING_FIGHTING			R(MSG_CASTLEWAR_FIGHTING)//"(交战中)"

#define BTN_ALLOW			0
#define BTN_APPLY			1
#define BTN_CANCEL_APPLY	2

class UIFormCastleWar
{
public:
	UIFormCastleWar();
	~UIFormCastleWar();

public:
	void Refresh();
	void Show(DWORD npc_id);
	void Hide();
	bool IsVisible();
	bool IsInAttackerList();
		
	void Lock()		{ m_b_lock = true; }
	void UnLock()	{ m_b_lock = false; }

public:
	DWORD		m_npc_id;
	STWarInfo	m_war_info;
	
private:
	RtwWidget*	m_pFrmThis;		// 整个窗口

	RtwTab*		m_pTabGuild;
	RtwButton*	m_pBtnAttacker;
	RtwButton*	m_pBtnDefender;

	RtwButton*	m_pBtnAllow;
	RtwButton*	m_pBtnApply;
	RtwButton*	m_pBtnCancelApply;

	RtwListBox*	m_pListAttacker;
	RtwListBox*	m_pListDefender;

	RtwLabel*	m_pLabelCastleName;
	RtwLabel*	m_pLabelUnionName;
	RtwLabel*	m_pLabelUnionLogo;
	RtwLabel*	m_pLabelStartTime;
    
	
	void OnBtnAttacker(RtwWidget* sender, void*);
	void OnBtnDefender(RtwWidget* sender, void*);

	void OnBtnAllow(RtwWidget* sender, void*);
	void OnBtnApply(RtwWidget* sender, void*);
	void OnBtnCancelApply(RtwWidget* sender, void*);

	void OnPreDraw();

private:
	bool		m_b_lock;
};

#endif // _INC_UI_FORM_CASTLEWAR_H_
