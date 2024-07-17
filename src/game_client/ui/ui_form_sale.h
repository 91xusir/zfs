
#ifndef _INC_UI_FORM_SALE_H_
#define _INC_UI_FORM_SALE_H_

#include "auction_base.h"

class UIFormSale
{
public:
	UIFormSale();
	~UIFormSale(){};

public:
	void Refresh();
	void Show();
	//void ShowDetail(CAuctionItem* pItem);
	void Hide();
	bool IsVisible();

	void Lock()		{ m_b_lock = true; }
	void UnLock()	{ m_b_lock = false; }

public:
	DWORD		m_npc_id;

public:
	void OnBtnSellClicked(RtwWidget* sender, void*);
	void OnBtnBackClicked(RtwWidget* sender, void*);
	void OnGetBtnHintText(RtwWidget* sender, void*);
	void OnBtnRefreshClicked(RtwWidget* sender, void*);

	void SetItemID(SItemID id) { m_itemID = id; };

	void OnGetListHintItemText(RtwWidget* sender, void*);
	void OnDragItem(RtwWidget* sender, void*);
	void OnDragEndItem(RtwWidget* sender, void*);
	void OnSelectTime(RtwWidget* sender, void*);
	void OnListItemSelect(RtwWidget* sender, void*);
	void OnMouseMove(RtwWidget* sender, RtwEventDelegate* pEvent);

	void ClearView();

private:
	RtwWidget*      m_pFrmThis;
	RtwButton*		m_pBtnItem;
	RtwLabel*		m_pLabelName;
	RtwTextBox*		m_pBasePrice;
	RtwTextBox*		m_pUniquePrice;
	RtwComboBox*	m_pTimeLimit;
	RtwLabel*		m_pHandlingCharge;
	RtwButton*		m_pBtnSell;
    
	RtwListBox*		m_pList;
	
	RtwButton*		m_pBtnBack;
	RtwButton*		m_pBtnRefresh;

	SItemID			m_itemID;
	int				m_index;

	void OnPreDraw();

private:
	bool		m_b_lock;
};

#endif // _INC_UI_FORM_SALE_H_