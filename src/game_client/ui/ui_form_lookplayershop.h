#pragma once
#include "item_pshop.h"

#define BUY_ITEM_LINE_COUNT 4
#define BUY_ITEM_COLUMN_COUNT 6
class GcActor;

class CUIFormLookPlayerShop
{
public:
	CUIFormLookPlayerShop(void);
	~CUIFormLookPlayerShop(void);
	void Show();
	void Hide();
	void Refresh();
	bool IsVisible();
	void Init();
	void SetShopSeller(string name);
	bool Is_Widget_Up(const string& id, int &outI, int &outJ);
	bool Is_Widget_Down(const string& id, int &outI, int &outJ);
	void SetPos(int x, int y) { m_pFrmThis->Move(SPoint(x, y)); }
	RtwRect OnGetFormRect()
	{
		guard;
		return m_pFrmThis->GetFrameRect();
		unguard;
	}

	CPrivateShop m_PShop;		
	int			m_nMaxDistance;

private:
	void OnPreDraw();
	//void OnHideForm(RtwWidget* vpSender, RtwEventDelegate* vpEvent);

	void OnLClick_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnLDClick_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnRClick_SellButton(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnGetHintText_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnDrag_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnDragEnd_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);

	void OnLClick_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnLDClick_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnRClick_BuyButton(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnGetHintText_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnDrag_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnDragEnd_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);

	void OnSetNum(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnEnsurePrice(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnEnsureDoAction(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnCancelOperate(RtwWidget* vpSender, RtwEventDelegate* vpEvent);

	void OnMouseRClickEvent(SItemID item);

	RtwWidget		*m_pFrmThis;		// 整个窗口
	RtwWidget		*m_pFrmGridsUp;	// 上面的格子窗口
	RtwWidget		*m_pFrmGridsDown;	// 下面的格子窗口

	RtwLabel		*m_lbSellerName;
	RtwTextBox		*m_tbxShopName;
	RtwButton		*m_btnStart;
	RtwButton		*m_btnStop;
	RtwButton		*m_btnShopChat;
	RtwButton		*m_btnStopSellOne;

	RtwButton		*m_btnGridsUp[PSHOP_GRID_LINE_COUNT][PSHOP_GRID_COLUMN_COUNT];
	RtwLabel		*m_lbGridsUp[PSHOP_GRID_LINE_COUNT][PSHOP_GRID_COLUMN_COUNT];
	RtwButton		*m_btnGridsDown[PSHOP_GRID_LINE_COUNT_BUY][PSHOP_GRID_COLUMN_COUNT_BUY];
	RtwLabel		*m_lbGridsDown[PSHOP_GRID_LINE_COUNT_BUY][PSHOP_GRID_COLUMN_COUNT_BUY];
	RtwImage::Ptr	m_ptrHoverImage;
	RtwImage::Ptr	m_ptrMaskImage;

	SItemID		m_operateItem;
	SItemID		m_selfItem;
	int			m_iSelectI;
	int			m_iSelectJ;
	bool		m_bOperateSell;
};
