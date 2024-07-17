#ifndef _INC_UI_FORM_BANK_H_
#define _INC_UI_FORM_BANK_H_

class GcActor;

class UIFormBank
{
public:
	bool m_bStore;
	CBag m_bagUp;
	CBag m_bagDown;
	DWORD m_npcID;			

    int m_BankSize;
	int m_BankItemCount;

	void Refresh();
	void Show(bool bStore, DWORD npcID);
	void Hide();
	bool IsVisible();										// 是否已经显示
	bool Is_Widget_Down(const string& id, int &outI, int &outJ);
	bool Is_Widget_Up(const string& id, int &outI, int &outJ);

	void SetScrollBar();

private:
	RtwWidget *m_pFrmThis;		// 整个窗口
	RtwWidget *m_pFrmGridsUp;	// 上面的格子窗口
	RtwWidget *m_pFrmGridsDown;	// 下面的格子窗口

	RtwWidget *m_lbNameUp;
	RtwWidget *m_lbNameDown;
	RtwWidget *m_lbMoneyUp;
	RtwWidget *m_lbMoneyDown;
	RtwWidget *m_lbMoneyNameUp;
	RtwWidget *m_lbMoneyNameDown;
	RtwButton *m_btnOK;
	RtwButton *m_btnCancel;
	RtwButton *m_btnMoney;
	RtwButton *m_btnGridsUp[BANK_GRID_LINE_COUNT][BANK_GRID_COLUMN_COUNT];
	RtwButton *m_btnGridsDown[BANK_GRID_LINE_COUNT][BANK_GRID_COLUMN_COUNT];
    RtwWidget *m_lbSizeInfo;
    RtwWidget *m_lbSize;

    RtwImage::Ptr m_ptrHoverImage;

	void OnLDClick_Down(RtwWidget* sender, void*);
	void OnGetHintText_Down(RtwWidget* sender, void*);
	void OnDrag_Down(RtwWidget* sender, void*);

	void OnLDClick_Up(RtwWidget* sender, void*);
	void OnGetHintText_Up(RtwWidget* sender, void*);
	void OnDrag_Up(RtwWidget* sender, void*);

	void OnOK(RtwWidget* sender, void*);
	void OnCancel(RtwWidget* sender, void*);
	void OnBtnMoney(RtwWidget* sender, void*);

	void OnMoney(RtwWidget* sender, RtwEventDelegate* e);

	void OnDragEnd_Up(RtwWidget* sender, void*);
	void OnDragEnd_Down(RtwWidget* sender, void*);

	// 滚动条
	int				m_iLineCountUp;
	int				m_iLineCountDown;

	int             m_iScrollUp;
	bool			m_bUseScrollUp;
	int             m_iScrollDown;
	bool			m_bUseScrollDown;
	RtwVScrollBar*   m_pScrollUp;
	RtwVScrollBar*   m_pScrollDown;

	void OnScrollUp(RtwWidget* sender,void*);
	void OnScrollDown(RtwWidget* sender,void*);
private:
	void OnPreDraw();
	bool m_bLockOperate;

public:
	UIFormBank();
	~UIFormBank();

	void LockOperate()		{ m_bLockOperate = true; }
	void UnlockOperate()	{ m_bLockOperate = false; }
};

#endif // _INC_UI_FORM_BANK_H_
