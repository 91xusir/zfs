#ifndef _INC_UI_FORM_SHOP_H_
#define _INC_UI_FORM_SHOP_H_

#include "item_npc_shop.h"

class GcActor;

class UIFormShop
{
public:
	bool m_bBuyFromNpc;			// 是从NPC处购买物品，还是向NPC出售物品
	DWORD m_npcID;			
	float m_fPriceRate;

    DWORD m_dwLastRecvData;     // 最后收到数据的时间
    DWORD m_TimeDelta;          // 与服务器的差额时间（秒）  = ServerTime - ClientTime

	CBag m_bagUp;
	CBag m_bagDown;
	CBag m_bagDownSell;
	bool m_bIsRepair;
    std::vector<SNpcSellRareItem> m_RareItems;
    std::vector<SItemID> m_RareItemsToBuy;

	void Refresh();
	void Show(bool bBuyFromNpc, DWORD npcID);
	void Hide();
	bool IsVisible();										// 是否已经显示 = 是否处在买卖状态
	bool Is_Widget_Down(const string& id, int &outI, int &outJ);
	bool Is_Widget_Up(const string& id, int &outI, int &outJ);
	void SetScrollBar();		// 重新设置滚动条数据

    void Run();
	RtwWidget* OnGetThisForm(){return m_pFrmThis;}
	bool OnBagToShop(RtwWidget* sender, RtwEventDelegate* e);
	void OnLClickRepair(SItemID &item);

private:
	// 滚动条
	int				 m_iLineCountUp;
	int				 m_iLineCountDown1;
	int				 m_iLineCountDown2;
	int              m_iScrollUp;
	int              m_iScrollDown1;
	int              m_iScrollDown2;
	bool			 m_bUseScrollUp;
	bool			 m_bUseScrollDown1;
	bool			 m_bUseScrollDown2;
	RtwVScrollBar*   m_pScrollUp;
	RtwVScrollBar*   m_pScrollDown1;
	RtwVScrollBar*   m_pScrollDown2;

	RtwWidget *m_pFrmThis;		// 整个窗口
	RtwWidget *m_pFrmGridsUp;	// 上面的格子窗口
	RtwWidget *m_pFrmGridsDown;	// 下面的格子窗口
	RtwWidget *m_pFrmGridsDown2;	// 下面的格子窗口

	int		   m_iBagI,m_iBagJ,m_iBagPage;
	int        m_irepairMoney;
	int        m_iPay;
	int		   m_iSellType;
	RtwWidget *m_lbMoneyName1;
	RtwLabel  *m_lbWindowsTitle;
	RtwLabel  *m_lbPaidType;
	RtwWidget *m_lbNameUp;
	RtwWidget *m_lbNameDown;
	RtwWidget *m_lbMoneyShopBuy;
	RtwWidget *m_lbMoneyShopSell;
	RtwWidget *m_lbPointShop;	//heten
	RtwWidget *m_lbMoneyOwn;
	RtwWidget *m_lbPointOwn;	//heten
	RtwButton *m_btnOK;
	RtwButton *m_btnSellOK;
	RtwButton *m_btnCancel;
	RtwButton *m_btnSellCancel;
	RtwButton *m_btnBuyRepairOne;
	RtwButton *m_btnSellRepairOne;
	RtwButton *m_btnRepairAll;
	RtwButton *m_btnSellRepairAll;
	RtwButton *m_btnItemRepairOne;
	//RtwButton *m_btnOK;
	RtwTab	  *m_pCheckTab;
	RtwButton *m_btnGridsUp[SHOP_GRID_LINE_UP_COUNT][SHOP_GRID_COLUMN_COUNT];
	RtwButton *m_btnGridsDown[SHOP_GRID_LINE_DOWN_COUNT][SHOP_GRID_COLUMN_COUNT];
	RtwButton *m_btnGridsDown2[SHOP_GRID_LINE_DOWN_COUNT][SHOP_GRID_COLUMN_COUNT];
	SItemID   m_repairItem;
	

    RtwImage::Ptr m_ptrHoverImage;
	DWORD		m_dwnpcID;

	void OnLDClick_Down(RtwWidget* sender, void*);
	void OnGetHintText_Down(RtwWidget* sender, void*);
	void OnDrag_Down(RtwWidget* sender, void*);

	void OnLDClick_Up(RtwWidget* sender, void*);
	void OnGetHintText_Up(RtwWidget* sender, void*);
	void OnDrag_Up(RtwWidget* sender, void*);

	void OnDragEnd_Up(RtwWidget* sender, void*);
	void OnDragEnd_Down(RtwWidget* sender, void*);

	void OnOK(RtwWidget* sender, void*);
	void OnCancel(RtwWidget* sender, void*);

	void OnScrollUp(RtwWidget* sender,void*);
	void OnScrollDown1(RtwWidget* sender,void*);
	void OnScrollDown2(RtwWidget* sender,void*);

    void cbSell(RtwWidget* sender, RtwEventDelegate* e);
	void OnCheckButtonDown(RtwWidget* sender, RtwEventDelegate* e);

    void DoSell(const char* strBankPass);
    void DoBuy();

    void Do_DownToUp(int i, int j);
    void Do_UpToDown(int i, int j);
	void OnOneRepairOK(RtwWidget* sender,void*);
	void OnAllRepairOK(RtwWidget* sender,void*);

	void OnRClickRepair(RtwWidget* sender,void*);
	void OnDragFromBagToDown(RtwWidget* sender, RtwEventDelegate* e);
	void OnHideForm(RtwWidget* vpSender, RtwEventDelegate* e);
	void OnGetFocus(void*,void*);


private:
	bool m_bLockOperate;

	void OnPreDraw();

public:
	UIFormShop();
	~UIFormShop();
	void LockOperate()		{ m_bLockOperate = true; }
	void UnlockOperate()	{ m_bLockOperate = false; }
};

#endif // _INC_UI_FORM_SHOP_H_
