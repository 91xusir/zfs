
#ifndef _INC_UI_FORM_AUCTION_H_
#define _INC_UI_FORM_AUCTION_H_

#include "auction_base.h"

//class CItem
//{
//public:
//	CItem()
//		: m_pBtn(NULL)
//		, m_pLabel1(NULL)
//		, m_pLabel2(NULL)
//		, m_pPanel(NULL)
//		, m_pItem(NULL)
//		, m_bIsSelected(false)
//	{
//	}
//
//	~CItem(){}
//
//public:
//	RtwButton*		m_pBtn;
//	RtwLabel*		m_pLabelTitle[6];
//	RtwLabel*		m_pLabelItem[6];
//	RtwPanel*		m_pPanel;
//	CAuctionItem*	m_pItem;
//	bool			m_bIsSelected;
//};
//
//class CListPanel
//{
//public:
//	CListPanel(){}
//	~CListPanel(){}
//
//public:
//	void	OnItemLabelTitle1Clicked(RtwWidget* sender, void*);
//	void	OnItemLabelTitle2Clicked(RtwWidget* sender, void*);
//	void	OnItemLabelTitle3Clicked(RtwWidget* sender, void*);
//	void	OnItemLabelTitle4Clicked(RtwWidget* sender, void*);
//	void	OnItemLabelTitle5Clicked(RtwWidget* sender, void*);
//	void	OnItemLabelTitle6Clicked(RtwWidget* sender, void*);
//
//	void	OnItemLabel1Clicked(RtwWidget* sender, void*);
//	void	OnItemLabel2Clicked(RtwWidget* sender, void*);
//	void	OnItemLabel3Clicked(RtwWidget* sender, void*);
//	void	OnItemLabel4Clicked(RtwWidget* sender, void*);
//	void	OnItemLabel5Clicked(RtwWidget* sender, void*);
//	void	OnItemLabel6Clicked(RtwWidget* sender, void*);
//
//	void	OnVScrollBarClicked();
//
//public:
//	CItem			m_Item[6];
//	CItem*			m_pCurrItem;
//	RtwVScrollBar*	m_pVScroll;
//};

class UIFormVendor
{
public:
	UIFormVendor();
	~UIFormVendor(){};

public:
	void Refresh();
	void Show();
	void Hide();
	bool IsVisible();

	void Lock()		{ m_b_lock = true; }
	void UnLock()	{ m_b_lock = false; }

public:
	DWORD		m_npc_id;

public:
	void OnBtnSellClicked(RtwWidget* sender, void*);
	void OnBtnBuyClicked(RtwWidget* sender, void*);
	void OnBtnRefreshClicked(RtwWidget* sender, void*);
	void OnTreeNodeSelect(RtwWidget* pSender, RtwEventDelegate* pEvent);
	void ShowItemList(int klass);
	void OnGetListHintItemText(RtwWidget* sender, void*);
	void OnListItemSelect(RtwWidget* sender, void*);
	void OnMouseMove(RtwWidget* sender, RtwEventDelegate* pEvent);
	void OnRefresh();

private:
	RtwWidget*      m_pFrmThis;
	RtwTree*		m_pTree;
	
	int				m_index;

	RtwListBox*		m_pList;

	RtwButton*		m_pBtnSell;
	RtwButton*		m_pBtnBuy;
	RtwButton*		m_pBtnRefresh;
	RtwButton*      m_btn1;
	RtwButton*      m_btn2;
	RtwButton*      m_btn3;
	RtwButton*      m_btn4;
	RtwButton*      m_btn5;
	RtwButton*      m_btn6;
	void OnPreDraw();

private:
	bool		m_b_lock;
};

extern CSalesroom	g_SalesroomGC;
extern CSalesroom	g_SalesroomGCPart1;
extern CSalesroom	g_SalesroomGCPart2;
extern int	GetItemClass(CAuctionItem* item);

#endif // _INC_UI_FORM_AUCTION_H_