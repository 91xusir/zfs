#ifndef _INC_UI_FORM_TRADE_H_
#define _INC_UI_FORM_TRADE_H_

#include "item_trade.h"

class GcActor;
class UIFormMsg;

class UIFormTrade : public GcTimerListener
{
public:
	UIFormMsg *m_pFrmOtherInvite;
	virtual void OnTimerEvent(DWORD dwID);		// 10����֮��������û����Ӧ,�͵����ܾ�����

//	DWORD m_UnresponsePlayer;			// �����˵�û�л�Ӧ�����
	int m_timeLastInvite;
	void SetPlayer(GcActor* pPlayerUp, GcActor* pPlayerDown)
	{
		m_pPlayerUp = pPlayerUp;
		m_pPlayerDown = pPlayerDown;
		if (pPlayerDown && pPlayerUp)
			m_trade.Reset( &(pPlayerUp->mItem), &(pPlayerDown->mItem), pPlayerUp->ID(), pPlayerDown->ID());
	}
	void Show();
	void Hide();
	bool IsShowing();										// �Ƿ��Ѿ���ʾ
	void Refresh();
	void SetPos(int x, int y) { m_pFrmThis->Move(SPoint(x, y)); }
	bool Is_Widget_Down(const string& id, int &outI, int &outJ);
	bool Is_Widget_Up(const string& id, int &outI, int &outJ);

	bool MouseRClick(int x, int y);
    bool OnCommand(GcActor* pTargetActor);
    void OnPopupMenu(RtwPopupMenu* pPopupMenu, GcActor* pTargetActor, int iData);

	void OnBtnInvite(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void InviteOther(DWORD actorID);
	void ResponseOther_Agree(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void ResponseOther_Disagree(ui::RtwWidget* vpSender, RtwEventDelegate* );

	CTrade m_trade;
	DWORD m_actorInviteU;		// ���������������ʱ����ֵΪ���ID

	GcActor* m_pPlayerUp;
	GcActor* m_pPlayerDown;

	bool GetRefuseState()				{ return m_bRefuseInvite; }
	void SetRefuseState(bool bRefuse)	{ m_bRefuseInvite = bRefuse; }

private:
	RtwWidget *m_pFrmThis;		// ��������
	RtwWidget *m_pFrmGridsUp;	// ����ĸ��Ӵ���
	RtwWidget *m_pFrmGridsDown;	// ����ĸ��Ӵ���

	RtwWidget *m_lbNameUp;
	RtwWidget *m_lbNameDown;
	RtwWidget *m_lbMoneyUp;
	RtwWidget *m_lbMoneyDown;
	RtwButton *m_btnMoney;
	RtwButton *m_btnDeal;
	RtwButton *m_btnOK;
	RtwButton *m_btnGridsUp[TRADE_GRID_LINE_COUNT][TRADE_GRID_COLUMN_COUNT];
	RtwButton *m_btnGridsDown[TRADE_GRID_LINE_COUNT][TRADE_GRID_COLUMN_COUNT];

    RtwImage::Ptr m_ptrHoverImage;

	void OnGetHintText_Down(RtwWidget* sender, void*);
	void OnDrag_Down(RtwWidget* sender, void*);

	void OnGetHintText_Up(RtwWidget* sender, void*);

	void OnMoney(RtwWidget* sender, void*);
	void OnOK(RtwWidget* sender, void*);
	void OnDeal(RtwWidget* sender, void*);
	void OnClose(RtwWidget* sender, void*);
	void OnTradeMoney(RtwWidget* sender, RtwEventDelegate* e);

	bool m_bPressDeal;

	bool m_bRefuseInvite;

    void cbLock(RtwWidget* sender, RtwEventDelegate* e);
    void DoLock(const char* strBankPass);

private:
	void OnPreDraw();

public:
	UIFormTrade();
	~UIFormTrade();

private:
	RtwForm		*m_pFmPetTrade;
	RtwButton	*m_pBtnToTrade;
	RtwButton	*m_pBtnToMine;
	RtwListBox	*m_pListPetMine;
	RtwListBox	*m_pListPetTrade;
	RtwListBox	*m_pListPetOther;
	RtwButton	*m_pBtnTradePet;
	RtwButton	*m_pBtnLookPet;
	Rtw3DView	*m_p3DView;

	list<SItemID> m_listPetMine;
	list<SItemID> m_listPetTrade;
	list<SItemID> m_listPetOther;

public:
	void OnLClickTradePet(RtwButton *sender,void *);
	void OnLClickPetToTrade(RtwButton *sender,void *);
	void OnLClickPetToMine(RtwButton *sender,void *);
	void OnHidePetTrade(RtwWidget *sender,void *);
	void OnLDClickListPet(RtwListBox *sender,void *);
	void OnLClickLookPet(RtwButton *sender,void *);
	void OnSelectPet(RtwButton *sender,void *);

	void OnPreDrawPetTrade();
	void ResetPetTrade();
	void ShowPetTrade();
	void RefreshPetTrade();
	void HidePetTrade();
};

#endif // _INC_UI_FORM_TRADE_H_
