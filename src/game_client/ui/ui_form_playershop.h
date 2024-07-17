#ifndef _INC_UI_PLAYERSHOP_H_
#define _INC_UI_PLAYERSHOP_H_

#include "item_pshop.h"
#define BUY_ITEM_LINE_COUNT 4
#define BUY_ITEM_COLUMN_COUNT 6
class GcActor;

class CUIFormPlayerShop
{
public:
	CUIFormPlayerShop();
	~CUIFormPlayerShop();
	void Show();
	void Refresh();
	bool IsVisible();
	void Init();
	void InitFromCSV();
	void OnStartInitShop();
	void OnMouseRClickEvent(SItemID item);
	bool Is_Widget_Up(const string& id, int &outI, int &outJ);
	bool Is_Widget_Down(const string& id, int &outI, int &outJ);
	bool Is_Widget_ItemList(const string& id, int &outI, int &outJ);
	void LockOperate()		{ m_bLockOperate = true; }
	void UnlockOperate()	{ m_bLockOperate = false; }

	void OnRecvStart();
	void OnRecvStop();
	void OnRecvActorPShop(GcActor *pActor);		// 收到玩家的摆摊数据
	void OnCommonItemDeleted(DWORD itemID);
	void SetPos(int x, int y) { m_pFrmThis->Move(SPoint(x, y)); }
	RtwRect OnGetFormRect()
	{
		guard;
		return m_pFrmThis->GetFrameRect();
		unguard;
	}
	RtwWidget* OnGetThisForm(){return m_pFrmThis;}
	void OnSubBuyMoney(int money){m_iAllBuyItemPrice-= money;}
	void OnAddBuyMoney(int money){m_iAllBuyItemPrice+= money;}

	void OnDeleteItem(SItemID& item);
	void OnDeleteAllItem();

	CPrivateShop m_PShop;		

private:
	void OnPreDraw();
	void OnHideForm(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnLClickBtnStopSellOneLClick(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnLClickBtnChangeName(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnShopNameChange(RtwWidget* vpSender, RtwEventDelegate* vpEvent);

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

	void OnLClick_BuyListItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnLDClick_BuyListItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnRClick_BuyListButton(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnGetHintText_BuyListItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);

	void OnShowItemList();
	void OnPreDrawItemList();
	void OnSetNum(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnSetPrice(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnCancelOperate(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnChangeNum(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnChangePrice(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnLClickBtnCancelItemList(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnSelectComboBox(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnLClickBtnOK(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnAddBuyItem(SItemID item);
	void OnStartShop(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnStopShop(RtwWidget* vpSender, RtwEventDelegate* vpEvent);

	void OnGetFocus(void*,void*);
	void OnChangeBtnStopSellOneText();

	RtwWidget		*m_pFrmThis;		// 整个窗口
	RtwWidget		*m_pFrmGridsUp;	// 上面的格子窗口
	RtwWidget		*m_pFrmGridsDown;	// 下面的格子窗口

	RtwWidget		*m_lbNameUp;
	RtwWidget		*m_lbNameDown;
	RtwWidget		*m_lbMoneyVar;
	RtwWidget		*m_lbMoneyTotal;

	RtwLabel		*m_lbSellerName;
	RtwTextBox		*m_tbxShopName;
	RtwTextBox		*m_tbxTotalMoney;
	RtwButton		*m_btnStart;
	RtwButton		*m_btnStop;
	RtwButton		*m_btnChangeName;
	RtwButton		*m_btnShopChat;
	RtwButton		*m_btnStopSellOne;

	RtwWidget		*m_pFrmBuyItemList;
	RtwWidget		*m_pFrmGridsBuyItem;
	RtwComboBox		*m_cbxComboBoxBuyItem;
	RtwButton		*m_btnOK;
	RtwButton		*m_btnCancel;
	RtwButton		*m_btnGridsBuy[BUY_ITEM_LINE_COUNT][BUY_ITEM_COLUMN_COUNT];
	RtwLabel		*m_lbGridsBuy[BUY_ITEM_LINE_COUNT][BUY_ITEM_COLUMN_COUNT];

	RtwButton		*m_btnGridsUp[PSHOP_GRID_LINE_COUNT][PSHOP_GRID_COLUMN_COUNT];
	RtwLabel		*m_lbGridsUp[PSHOP_GRID_LINE_COUNT][PSHOP_GRID_COLUMN_COUNT];
	RtwButton		*m_btnGridsDown[PSHOP_GRID_LINE_COUNT_BUY][PSHOP_GRID_COLUMN_COUNT_BUY];
	RtwLabel		*m_lbGridsDown[PSHOP_GRID_LINE_COUNT_BUY][PSHOP_GRID_COLUMN_COUNT_BUY];
    RtwImage::Ptr	m_ptrHoverImage;
	RtwImage::Ptr	m_ptrMaskImage;

	int			m_iAllBuyItemPrice;
	bool		m_bLockOperate;
	int			m_iModelID;
	int			m_iHeadModelID;
	SItemID		m_operateItem;
	int			m_iSelectI;
	int			m_iSelectJ;
	bool		m_bOperateSell;
	int			m_iSelectAtItemListI;
	int			m_iSelectAtItemListJ;
	bool		m_bAddSellItemFormDrag;
	CBag		m_itemListBag;
	vector<vector<SItemID>*>		m_vectorSItemBase;
};

#endif // _INC_UI_FORM_PSHOP_H_
