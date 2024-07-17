#ifndef _INC_UI_FORM_UNION_H_
#define _INC_UI_FORM_UNION_H_
//********************************************************************
//	created:	2010.05.31
//	filename: 	ui_form_union.h
//	author:		ldr123
//	purpose:	游戏中,帮派系统重做
//  other:		version 1.0
//********************************************************************

#include "union_base.h"
class GcActor;

const string strAreaText[] = {"所在区域", "帮派贡献", "最近上线时间"};
enum UnionLogType{eCreate, eJobin, eLeave, eBanish, eJobUp, eJobDown, eDoContribution, eGiveCaption, eChangeBulletin, eGetMoney, UnionLogTypeNum};
RtString GetUOString(EUnionOfficial unionOfficial);

class UIFormUnion : public GcTimerListener
{
public:
	UIFormMsg *m_pFrmMsg;

	SUnionData m_unionData;	//帮派数据
	SUnionData m_unionBack;	//帮派数据备份

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

	void OnRecvActorUnionData(GcActor *pActor);		// 收到玩家的诸侯国数据

	void OnRecvLogoData(char* FileName);			// 收到新的Icon

	CM_MEMDEF(m_ApplyLogoFileName, PATH_STRING_LEN)
	void OnRecvChangeLogoSuccess(char* NewFileName);
	void OnRecvChangeLogoFail();

	bool GetRefuseState()				{ return m_bRefuseInvite; }
	void SetRefuseState(bool bRefuse)	{ m_bRefuseInvite = bRefuse; }

	bool Union_Disband(bool bAuto, DWORD nPlayerID);	//解散帮派,bAuto为自动或否定, id为用户id

	// 30秒钟之后如果玩家没有相应,就当作拒绝
	void OnTimerEvent(DWORD dwID);
private:
	//主界面
	//begin
	RtwWidget*	m_pFrmFaction;	//帮派管理界面

	RtwLabel*	m_pLblIcon;		//帮派图腾
	RtwLabel*	m_pLblFacName;	//帮派名字
	RtwLabel*	m_pLblStrength; //帮派势力
	RtwLabel*	m_pLblFacMoney; //帮派资金
	RtwLabel*   m_pLblFacRank;  //帮派排名
	RtwLabel*   m_pLblPeopleNum;//帮派人数

	RtwButton*	m_pBtnPost;		//发布公告
	RtwTextBox* m_pChatPost;	//公告显示

	RtwListBox*	m_pLstMember;	//成员列表
	RtwChatBox*	m_pLstHistory;	//历史记录

	RtwButton*	m_pBtnPrew;		//前一区域
	RtwButton*	m_pBtnNext;		//后一区域

	RtwTextBox*	m_pTxtArea;		//显示的区域

	RtwButton*  m_pBtnFunction;	//功能
	RtwButton*	m_pBtnExit;		//退出

	RtwButton*		m_pBtnContribution;//贡献资源
	CUiCheckButton* m_pChkOffLineUser; //离线用户

	RtwPopupMenu*	m_pFuncMenu;	//功能按钮的菜单
	RtwPopupMenu*	m_pUserList;	//右击用户列表的菜单

	void OnClickFunction(RtwWidget* vpSender, RtwEventDelegate* e);		//点击功能按钮
	void OnClickExit(RtwWidget* vpSender, RtwEventDelegate* e);			//点击退出
	void OnClickContribution(RtwWidget* vpSender, RtwEventDelegate* e);	//点击贡献
	void OnClickPost(RtwWidget* vpSender, RtwEventDelegate* e);			//点击发布公告

	void OnClickPrewBtn(RtwWidget* vpSender, RtwEventDelegate* e);		//点击前一区域
	void OnClickNextBtn(RtwWidget* vpSender, RtwEventDelegate* e);		//点击后一区域

	void OnRClickMember(RtwWidget* vpSender, RtwEventDelegate* e);		//右击成员列表
	void OnCheckOffline(RtwWidget* vpSender, RtwEventDelegate* e);		//离线用户

	void OnFuncPopMenuSelect(RtwWidget* vpSender, RtwEventDelegate* e);			//功能菜单处理
	void OnFuncPopMenuLoseFocus(RtwWidget* vpSender, RtwEventDelegate* e);		//功能菜单失效
	void OnUserListPopMenuSelect(RtwWidget* vpSender, RtwEventDelegate* e);		//用户列表菜单处理
	void OnUserListPopMenuLoseFocus(RtwWidget* vpSender, RtwEventDelegate* e);	//用户列表菜单失效

	void RefreshUserList();

	//菜单命令
	//begin
	void menu_Demise();				//禅让
	void menu_ChatTo();				//密语
	void menu_Appoint();			//调整职位
	void menu_CopyName();			//复制名称
	void menu_AddFriend();			//加为好友
	void menu_ApplyTeam();			//申请组队
	void menu_InviteTeam();			//邀请组队
	void menu_Banishment();			//逐出帮派
	void menu_DisableChannel();		//禁言
	void menu_AddMember();			//添加帮众
	void menu_ViewPower();			//查看权限
	void menu_UnionPic();			//帮派图腾
	void menu_LeaveUnion();			//脱离帮派
	void menu_SelfIntroduction();	//自我介绍
	void menu_DisbandUnion();		//解散帮派
	//end
	//end

	SUnionCitizen *m_pSelectedUser;	//被选中的用户

	//更改职位
	RtwWidget*		m_pFmAppoint;
	RtwButton*		m_pBtnChangeOk;
	RtwButton*		m_pBtnChangeCancel;
	RtwComboBox*	m_pCbxChangeText;
	RtwLabel*		m_pLblChangeTextTips;
	vector<EUnionOfficial> m_vecChange;

	void Change();														//开始更改职业
	void OnClickChangeOk(RtwWidget* vpSender, RtwEventDelegate* e);		//点击ok
	void OnClickChangeCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//点击取消
	//end

	//添加帮众
	RtwWidget*		m_pFmAdd;
	RtwButton*		m_pBtnAddOk;
	RtwButton*		m_pBtnAddCancel;
	RtwTextBox*		m_ptxtAddText;

	void AddMember();													//添加帮众
	void OnClickAddOk(RtwWidget* vpSender, RtwEventDelegate* e);		//点击ok
	void OnClickAddCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//点击取消
	//end

	//脱离帮派
	RtwWidget*		m_pFmLeave;
	RtwButton*		m_pBtnLeaveOk;
	RtwButton*		m_pBtnLeaveCancel;

	void LeaveUnion();													//脱离帮派
	void OnClickLeaveOk(RtwWidget* vpSender, RtwEventDelegate* e);		//点击ok
	void OnClickLeaveCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//点击取消
	//end

	//自我介绍
	RtwWidget*		m_pFmSelfIntro;
	RtwButton*		m_pBtnSelfIntroOk;
	RtwButton*		m_pBtnSelfIntroCancel;
	RtwTextBox*		m_pTxtSelfIntro;

	void SelfIntro();													//自我介绍
	void OnClickSelfCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//点击ok/取消
	//end

	//公告发布
	RtwWidget*		m_pFmBulletin;
	RtwButton*		m_pBtnBulletinOk;
	RtwButton*		m_pBtnBulletinCancel;
	RtwTextBox*		m_pTxtBulletin;

	void OnClickBulletiOk(RtwWidget* vpSender, RtwEventDelegate* e);		//点击ok
	void OnClickBulletinCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//点击取消
	//end

	//禅让
	void Demise(RtwWidget* sender, void*);
	//end

	//驱逐
	void Banishment(RtwWidget* sender, void*);
	//end

	//解散
	void Disband(RtwWidget* sender, void*);
	//end

	//查看权限
	RtwWidget*		m_pFmViewPower;
	RtwButton*		m_pBtnViewPowerOk;
	RtwButton*		m_pBtnViewPowerCancel;

	void OnClickLViewPowerCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//点击ok/取消
	//end
	
	//贡献资源
	RtwWidget*		m_pFmMoney;
	RtwTextBox*		m_pTxtMoney;
	RtwButton*		m_pBtnMoneyOk;
	RtwButton*		m_pBtnMoneyCancel;

	void OnLClickMoneyOK(RtwWidget* vpSender, RtwEventDelegate* e);	//点击ok
	void OnLClickMoneyCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//取消

	void OnInputChar(RtwWidget* vpSender, RtwEventDelegate* e);	//输入贡献银币值
	//end

	//帮派图腾
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

	void OnLClickTotemUpLoad(RtwWidget* vpSender, RtwEventDelegate* e);	//点击上传
	void OnLClickTotemOK(RtwWidget* vpSender, RtwEventDelegate* e);	//点击ok
	void OnLClickTotemCancel(RtwWidget* vpSender, RtwEventDelegate* e);	//取消

	int nSelectTotemID;
	void RefreshTotem();
	RtwImage *GetTotemPicFromID(int id);
	void InitTotem();	//初始化图腾数据
	void OnSelectItem(RtwWidget* vpSender, RtwEventDelegate* e);	//选择某个item
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
	void AddLog(DWORD type, DWORD nTime, const char *pMsg);			//添加日志
	void UpdateContribution(DWORD nPlayerID, DWORD nContribution);	//刷新帮贡
	void UpdateMoneyItem(DWORD nMoney, DWORD nItem1, DWORD nItem2, DWORD nItem3, DWORD nItem4);//刷钱
	void UpdateWarScores(DWORD nWarScores);							//刷新战斗力
	void UpdateIcon(DWORD nIconID, DWORD nMoney);					//刷新新的图腾
	void ClearHistory(){if (m_pLstHistory)m_pLstHistory->RemoveAllLines();}			//清空历史记录								
	void SetUnionButtonNormal();
	void SetUnionButtonAnimation();
private:
	void OnPreDraw();

	void OnHeaderClick(RtwWidget* sender, void*);

	void OnUpdateBulletin(RtwWidget* sender, void*);

	// 对话框回调所使用的变量
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