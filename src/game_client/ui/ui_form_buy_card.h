#ifndef _INC_UI_FORM_BUY_CARD_H_
#define _INC_UI_FORM_BUY_CARD_H_

class CCardTradeList;

class UIFormBuyCard
{
public:
    enum
    {
        CARD_SHOP_GRID_LINE_COUNT   = 3,
        CARD_SHOP_GRID_COLUMN_COUNT = 6,
        CARD_SHOP_LINE_MAX          = 5,
    };

public:
	void Refresh();
	void Show();
	void Hide();
	bool IsVisible();   // 是否已经显示

    void CopyPageData();
    bool GetSelectGridPos(const string& id, int &outI, int &outJ);
    void SetUserData(long lNpcID, char cCardType);
    void OnNetDataUpdate(CG_CmdPacket* pPacket);
    void OnBuyCardResult(CG_CmdPacket* pPacket);

	int             m_iScrollUp;
	bool			m_bUseScrollUp;
	int             m_iScrollDown;
	bool			m_bUseScrollDown;

public:
    static CCardTradeList*  m_pList;

private:
    // 网上回传数据
    long    m_lNpcID;
    char    m_cCardType;
    short   m_sTotal, m_sStart, m_sCount;

    // 计算数据
    bool    m_bWaitingForUser;
    int     m_iLastSelX, m_iLastSelY;
    int     m_iLastCmdX, m_iLastCmdY;
    short   m_sPage, m_sPageCount;
    long    m_lLastRefreshTime;

	RtwWidget *m_pFrmThis;		// 整个窗口
	RtwWidget *m_pFrmGrids;	// 上面的格子窗口

	RtwWidget *m_lbMoneyTotal;
	RtwWidget *m_lbPageInfo;

	RtwButton *m_btnBuy;
	RtwButton *m_btnCancel;
	RtwButton *m_btnPageLeft;
    RtwButton *m_btnPageRight;

    long        m_lConsignationID[CARD_SHOP_GRID_LINE_COUNT][CARD_SHOP_GRID_COLUMN_COUNT];
    SItemBase*  m_pItems[CARD_SHOP_GRID_LINE_COUNT][CARD_SHOP_GRID_COLUMN_COUNT];
	RtwButton*  m_btnGrids[CARD_SHOP_GRID_LINE_COUNT][CARD_SHOP_GRID_COLUMN_COUNT];

    RtwImage::Ptr m_ptrHoverImage;

    void OnGridLClick(RtwWidget* sender, void*);
	void OnGridLDClick(RtwWidget* sender, void*);
	void OnGridGetHintText(RtwWidget* sender, void*);

	void OnBuy(RtwWidget* sender, void*);
	void OnCancel(RtwWidget* sender, void*);
	void OnPageLeft(RtwWidget* sender, void*);
	void OnPageRight(RtwWidget* sender, void*);

	void cbOnBuy(RtwWidget* sender, RtwEventDelegate* e);
    void cbOnCancelBuy(RtwWidget* sender, RtwEventDelegate* e);

    void Buy();
    void RequestPage(short sPage);

	void OnPreDraw();


public:
	UIFormBuyCard();
	~UIFormBuyCard();
};

#endif // _INC_UI_FORM_BUY_CARD_H_
