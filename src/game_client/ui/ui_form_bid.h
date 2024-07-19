
#ifndef _INC_UI_FORM_BID_H_
#define _INC_UI_FORM_BID_H_

#include "auction_base.h"

class UIFormBid
{
public:
	UIFormBid();
	~UIFormBid(){};

public:
	void Refresh();
	void Show(CAuctionItem* pItem);
	void ShowDetail(CAuctionItem* pItem);
	void Hide();
	bool IsVisible();

	void Lock()		{ m_b_lock = true; }
	void UnLock()	{ m_b_lock = false; }

public:
	void OnBtnBidClicked(RtwWidget* sender, void*);
	void OnBtnCancelBidClicked(RtwWidget* sender, void*);
	void OnBtnBackClicked(RtwWidget* sender, void*);
	void OnGetBtnHintText(RtwWidget* sender, void*);
	void OnGetListHintItemText(RtwWidget* sender, void*);
	void OnBidPriceChanged(RtwWidget* sender, void*);
	void OnListItemSelect(RtwWidget* sender, void*);
	void OnBtnRefreshClicked(RtwWidget* sender, void*);
	void OnMouseMove(RtwWidget* sender, RtwEventDelegate* pEvent);

	void ClearView();

private:
	RtwWidget*      m_pFrmThis;
	RtwButton*		m_pBtnItem;
	RtwLabel*		m_pLabelName;
	RtwTextBox*		m_pBidPrice;
	RtwLabel*		m_pUniquePrice;
	RtwLabel*		m_pHandlingCharge;
	RtwLabel*		m_pTotal;
	RtwButton*		m_pBtnBid;
	RtwButton*		m_pBtnCancelBid;

	RtwListBox*		m_pList;

	RtwButton*		m_pBtnBack;
	RtwButton*		m_pBtnRefresh;

	void OnPreDraw();

private:
	bool		    m_b_lock;
	SItemID		    m_LastItemID;
    long            m_lLastAuctionId;
	CAuctionItem	m_item;
	int			    m_index;
};

#endif // _INC_UI_FORM_BID_H_