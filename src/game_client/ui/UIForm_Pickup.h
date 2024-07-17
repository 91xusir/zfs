#pragma once
#include "ui/rtw_ui.h"

class CUIForm_Pickup
{
public:
	CUIForm_Pickup(void);
	~CUIForm_Pickup(void);
	void Reset();
	void Show(bool pkpick = false);
	void Hide();
	void InHide(RtwWidget* sender, void*);
	bool IsVisible(){guard;return m_pfrmPickup->IsVisible();unguard;}
	void OnInsertNewItem(SItemID ID);
	void OnBtnDown(RtwWidget* sender, void*);
	void OnBtnOtherDown(RtwWidget* sender, void*);
	void OnHintItem(RtwWidget* sender, void*);
	void OnClickPet(RtwWidget* sender, void*);
	void OnDelOneItem(long ID);

private:
	RtwWidget*				m_pfrmPickup;
	RtwButton*				m_pbtnUppage;
	RtwButton*				m_pbtnDownpage;
	RtwButton*				m_pbtnAllPickup;
	RtwWidget*				m_pfrmShowLabel[4];
	RtwButton*				m_pbtnItem[4];
	RtwLabel*				m_plbItem[4];
	RtwImage::Ptr			m_ptrHoverImage;

	int						m_iNowPage;
	vector<SItemID>			m_vocAllItemList;
	bool					m_bAotoClose;
	bool					m_bPkPick;
};
