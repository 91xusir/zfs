//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#ifndef _INC_UI_FORM_CASTLEWAR_TIMESET_H_
#define _INC_UI_FORM_CASTLEWAR_TIMESET_H_

#include "castlewar_base.h"

#define STR_EMPTY_ERR_MSG	R(MSG_CASTLEWAR_EMPTY_ERR)//"时间不能为空"
#define STR_FORMAT_ERR_MSG	R(MSG_CASTLEWAR_FORMAT_ERR)//"时间格式错误，请参照指定格式"
#define STR_NOT_VASSAL_MSG	R(MSG_CASTLEWAR_STR_NOT_VASSAL)//"只有本采邑诸侯有权调整时间"

class UIFormCastleWarTimeSet
{
public:
	UIFormCastleWarTimeSet();
	~UIFormCastleWarTimeSet();

public:
	void Refresh();
	void Show(DWORD npc_id);
	void Hide();
	bool IsVisible();

	void Lock()		{ m_b_lock = true; }
	void UnLock()	{ m_b_lock = false; }

public:
	DWORD		m_npc_id;
	STWarInfo	m_war_info;

public:
	RtwWidget*	m_pFrmThis;		// 整个窗口
	
	RtwButton*	m_pBtnOk;
	RtwButton*	m_pBtnCancel;

	RtwLabel*	 m_pLabelDefaultTime;
	RtwComboBox* m_pTimeSetBox;

	void OnBtnOk(RtwWidget* sender, void*);
	void OnBtnCancel(RtwWidget* sender, void*);

	void OnPreDraw();

private:
	bool		m_b_lock;
};

#endif // _INC_UI_FORM_CASTLEWAR_TIMESET_H_
