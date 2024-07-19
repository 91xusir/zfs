#ifndef _INC_UI_FORM_UNION_H_
#define _INC_UI_FORM_UNION_H_
//********************************************************************
//	created:	2010.05.31
//	filename: 	ui_form_union.h
//	author:		ldr123
//	purpose:	��Ϸ��,����ϵͳ����
//  other:		version 1.0
//********************************************************************

#include "union_base.h"
class GcActor;

const string strAreaText[] = {"��������", "���ɹ���", "�������ʱ��"};
enum UnionLogType{eCreate, eJobin, eLeave, eBanish, eJobUp, eJobDown, eDoContribution, eGiveCaption, eChangeBulletin, eGetMoney, UnionLogTypeNum};
RtString GetUOString(EUnionOfficial unionOfficial);

class UIFormUnion : public GcTimerListener
{
public:
	UIFormMsg *m_pFrmMsg;

	SUnionData m_unionData;	//��������
	SUnionData m_unionBack;	//�������ݱ���

	SUnionCitizen* Find(DWORD playerID);

	void Refresh();
	void Show();
	void Hide();
	bool IsVisible();
    void OnShow(void *,void *);
	
	void OnRecvNewBulletin(char* cNewBulletin);

	void OnBtnInvite(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void InviteJoin(DWORD playerID);

	int m_timeLastInvite;
    int m_timeLastUpdateBulletin;

	bool MouseRClick(int x, int y);
	bool OnCommand(GcActor* pTargetActor);
	void OnPopupMenu(RtwPopupMenu* pPopupMenu, GcActor* pTargetActor, int iData);

	void ResponseOther_Agree(ui::RtwWidget* vpSender, RtwEventDelegate* e);
	void ResponseOther_Disagree(ui::RtwWidget* vpSender, RtwEventDelegate* e);

	void OnRecvActorUnionData(GcActor *pActor);		// �յ���ҵ���������

	void OnRecvLogoData(char* FileName);			// �յ��µ�Icon

	CM_MEMDEF(m_ApplyLogoFileName, PATH_STRING_LEN)
	void OnRecvChangeLogoSuccess(char* NewFileName);
	void OnRecvChangeLogoFail();

	bool GetRefuseState()				{ return m_bRefuseInvite; }
	void SetRefuseState(bool bRefuse)	{ m_bRefuseInvite = bRefuse; }

	bool Union_Disband(bool bAuto, DWORD nPlayerID);	//��ɢ����,bAutoΪ�Զ����, idΪ�û�id

	// 30����֮��������û����Ӧ,�͵����ܾ�
	void OnTimerEvent(DWORD dwID);
private:
	//������
	//begin
	RtwWidget*	m_pFrmFaction;	//���ɹ������

	RtwLabel*	m_pLblIcon;		//����ͼ��
	RtwLabel*	m_pLblFacName;	//��������
	RtwLabel*	m_pLblStrength; //��������
	RtwLabel*	m_pLblFacMoney; //�����ʽ�
	RtwLabel*   m_pLblFacRank;  //��������
	RtwLabel*   m_pLblPeopleNum;//��������

	RtwButton*	m_pBtnPost;		//��������
	RtwTextBox* m_pChatPost;	//������ʾ

	RtwListBox*	m_pLstMember;	//��Ա�б�
	RtwChatBox*	m_pLstHistory;	//��ʷ��¼

	RtwButton*	m_pBtnPrew;		//ǰһ����
	RtwButton*	m_pBtnNext;		//��һ����

	RtwTextBox*	m_pTxtArea;		//��ʾ������

	RtwButton*  m_pBtnFunction;	//����
	RtwButton*	m_pBtnExit;		//�˳�

	RtwButton*		m_pBtnContribution;//������Դ
	CUiCheckButton* m_pChkOffLineUser; //�����û�

	RtwPopupMenu*	m_pFuncMenu;	//���ܰ�ť�Ĳ˵�
	RtwPopupMenu*	m_pUserList;	//�һ��û��б�Ĳ˵�

	void OnClickFunction(RtwWidget* vpSender, RtwEventDelegate* e);		//������ܰ�ť
	void OnClickExit(RtwWidget* vpSender, RtwEventDelegate* e);			//����˳�
	void OnClickContribution(RtwWidget* vpSender, RtwEventDelegate* e);	//�������
	void OnClickPost(RtwWidget* vpSender, RtwEventDelegate* e);			//�����������

	void OnClickPrewBtn(RtwWidget* vpSender, RtwEventDelegate* e);		//���ǰһ����
	void OnClickNextBtn(RtwWidget* vpSender, RtwEventDelegate* e);		//�����һ����

	void OnRClickMember(RtwWidget* vpSender, RtwEventDelegate* e);		//�һ���Ա�б�
	void OnCheckOffline(RtwWidget* vpSender, RtwEventDelegate* e);		//�����û�

	void OnFuncPopMenuSelect(RtwWidget* vpSender, RtwEventDelegate* e);			//���ܲ˵�����
	void OnFuncPopMenuLoseFocus(RtwWidget* vpSender, RtwEventDelegate* e);		//���ܲ˵�ʧЧ
	void OnUserListPopMenuSelect(RtwWidget* vpSender, RtwEventDelegate* e);		//�û��б�˵�����
	void OnUserListPopMenuLoseFocus(RtwWidget* vpSender, RtwEventDelegate* e);	//�û��б�˵�ʧЧ

	void RefreshUserList();

	//�˵�����
	//begin
	void menu_Demise();				//����
	void menu_ChatTo();				//����
	void menu_Appoint();			//����ְλ
	void menu_CopyName();			//��������
	void menu_AddFriend();			//��Ϊ����
	void menu_ApplyTeam();			//�������
	void menu_InviteTeam();			//�������
	void menu_Banishment();			//�������
	void menu_DisableChannel();		//����
	void menu_AddMember();			//��Ӱ���
	void menu_ViewPower();			//�鿴Ȩ��
	void menu_UnionPic();			//����ͼ��
	void menu_LeaveUnion();			//�������
	void menu_SelfIntroduction();	//���ҽ���
	void menu_DisbandUnion();		//��ɢ����
	//end
	//end

	SUnionCitizen *m_pSelectedUser;	//��ѡ�е��û�

	//����ְλ
	RtwWidget*		m_pFmAppoint;
	RtwButton*		m_pBtnChangeOk;
	RtwButton*		m_pBtnChangeCancel;
	RtwComboBox*	m_pCbxChangeText;
	RtwLabel*		m_pLblChangeTextTips;
	vector<EUnionOfficial> m_vecChange;

	void Change();														//��ʼ����ְҵ
	void OnClickChangeOk(RtwWidget* vpSender, RtwEventDelegate* e);		//���ok
	void OnClickChangeCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//���ȡ��
	//end

	//��Ӱ���
	RtwWidget*		m_pFmAdd;
	RtwButton*		m_pBtnAddOk;
	RtwButton*		m_pBtnAddCancel;
	RtwTextBox*		m_ptxtAddText;

	void AddMember();													//��Ӱ���
	void OnClickAddOk(RtwWidget* vpSender, RtwEventDelegate* e);		//���ok
	void OnClickAddCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//���ȡ��
	//end

	//�������
	RtwWidget*		m_pFmLeave;
	RtwButton*		m_pBtnLeaveOk;
	RtwButton*		m_pBtnLeaveCancel;

	void LeaveUnion();													//�������
	void OnClickLeaveOk(RtwWidget* vpSender, RtwEventDelegate* e);		//���ok
	void OnClickLeaveCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//���ȡ��
	//end

	//���ҽ���
	RtwWidget*		m_pFmSelfIntro;
	RtwButton*		m_pBtnSelfIntroOk;
	RtwButton*		m_pBtnSelfIntroCancel;
	RtwTextBox*		m_pTxtSelfIntro;

	void SelfIntro();													//���ҽ���
	void OnClickSelfCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//���ok/ȡ��
	//end

	//���淢��
	RtwWidget*		m_pFmBulletin;
	RtwButton*		m_pBtnBulletinOk;
	RtwButton*		m_pBtnBulletinCancel;
	RtwTextBox*		m_pTxtBulletin;

	void OnClickBulletiOk(RtwWidget* vpSender, RtwEventDelegate* e);		//���ok
	void OnClickBulletinCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//���ȡ��
	//end

	//����
	void Demise(RtwWidget* sender, void*);
	//end

	//����
	void Banishment(RtwWidget* sender, void*);
	//end

	//��ɢ
	void Disband(RtwWidget* sender, void*);
	//end

	//�鿴Ȩ��
	RtwWidget*		m_pFmViewPower;
	RtwButton*		m_pBtnViewPowerOk;
	RtwButton*		m_pBtnViewPowerCancel;

	void OnClickLViewPowerCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//���ok/ȡ��
	//end
	
	//������Դ
	RtwWidget*		m_pFmMoney;
	RtwTextBox*		m_pTxtMoney;
	RtwButton*		m_pBtnMoneyOk;
	RtwButton*		m_pBtnMoneyCancel;

	void OnLClickMoneyOK(RtwWidget* vpSender, RtwEventDelegate* e);	//���ok
	void OnLClickMoneyCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//ȡ��

	void OnInputChar(RtwWidget* vpSender, RtwEventDelegate* e);	//���빱������ֵ
	//end

	//����ͼ��
	typedef struct TotemItem
	{
		TotemItem()	: nPicID(0), nPrice(0), strName(""), strPicDescription(""),pWidget(NULL){}

		RtwWidget *pWidget;
		int		nPicID;
		int		nPrice;
		string  strName;
		string	strPicDescription;

		TotemItem &operator=(const TotemItem &t)
		{
			pWidget = t.pWidget;
			nPrice	= t.nPrice;
			nPicID	= t.nPicID;
			strName = t.strName;
			strPicDescription = t.strPicDescription;
			return *this;
		}
	}TotemItem;

	RtwWidget*	m_pFmTotem;

	RtwButton*	m_pBtnTotemUpLoad;
	RtwButton*	m_pBtnTotemOK;
	RtwButton*	m_pBtnTotemCancel;

	RtwButton*	m_pBtnTotemPic;
	RtwLabel*	m_pLblTotemPicInfo;
	RtwLabel*	m_pLblTotemPrice;
	RtwLabel*	m_pLblTotemHighlight;

	vector<TotemItem> m_vecTotemList;

	void OnLClickTotemUpLoad(RtwWidget* vpSender, RtwEventDelegate* e);	//����ϴ�
	void OnLClickTotemOK(RtwWidget* vpSender, RtwEventDelegate* e);	//���ok
	void OnLClickTotemCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//ȡ��

	int nSelectTotemID;
	void RefreshTotem();
	RtwImage *GetTotemPicFromID(int id);
	void InitTotem();	//��ʼ��ͼ������
	void OnSelectItem(RtwWidget* vpSender, RtwEventDelegate* e);	//ѡ��ĳ��item
	void UpdateTotem(RtwWidget* sender, void*);
	
	//end

	void OnBtnDismiss(RtwWidget* sender, void*);
	void OnBtnTitle(RtwWidget* sender, void*);
	void OnBtnLogo(RtwWidget* sender, void*);
	void OnBtnAppoint(RtwWidget* sender, void*);
	void OnBtnSurrender(RtwWidget* sender, void*);
	void OnBtnLeave(RtwWidget* sender, void*);
	void OnAskInheritor(RtwWidget* sender, void*);
	void OnSelectInheritor(RtwWidget* sender, void*);

	void OnDismiss(RtwWidget* sender, void*);
	void OnTitle(RtwWidget* sender, RtwEventDelegate* e);
	void OnLogo(RtwWidget* sender, RtwEventDelegate* e);
	void OnAppoint(RtwWidget* sender, RtwEventDelegate* e);
	void OnSurrender(RtwWidget* sender, void*);
	void OnLeave(RtwWidget* sender, void*);
	void OnLeaveWithInheritor(RtwWidget* sender, RtwEventDelegate* e);
public:
	void AddLog(DWORD type, DWORD nTime, const char *pMsg);			//�����־
	void UpdateContribution(DWORD nPlayerID, DWORD nContribution);	//ˢ�°ﹱ
	void UpdateMoneyItem(DWORD nMoney, DWORD nItem1, DWORD nItem2, DWORD nItem3, DWORD nItem4);//ˢǮ
	void UpdateWarScores(DWORD nWarScores);							//ˢ��ս����
	void UpdateIcon(DWORD nIconID, DWORD nMoney);					//ˢ���µ�ͼ��
	void ClearHistory(){if (m_pLstHistory)m_pLstHistory->RemoveAllLines();}			//�����ʷ��¼								
	void SetUnionButtonNormal();
	void SetUnionButtonAnimation();
private:
	void OnPreDraw();

	void OnHeaderClick(RtwWidget* sender, void*);

	void OnUpdateBulletin(RtwWidget* sender, void*);

	// �Ի���ص���ʹ�õı���
	DWORD m_bkPlayerDismiss;
	DWORD m_bkPlayerAppoint;
	DWORD m_bkPlayerTitle;

	bool m_bRefuseInvite;

	RtwImage*		m_imgUnionNormal;
	RtwImage*		m_imgUnionAnimation;
	bool			m_bShowAnimation;

public:
	UIFormUnion();
	~UIFormUnion();
};

class CUnionIconMgr
{
public:
	struct SUnionIconData
	{
		string fileName;
		char* data;
		int dataLen;
	};

	vector<SUnionIconData> m_arrIcon;
	CUnionIconMgr() {}
	~CUnionIconMgr()
	{
		for (int i=0; i<m_arrIcon.size(); i++)
			DEL_ARRAY(m_arrIcon[i].data)
	}
	SUnionIconData *Find(string fileName);
};

extern CUnionIconMgr g_UnionIconMgr;

#endif 