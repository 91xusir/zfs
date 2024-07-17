
#ifndef GC_TEAM_WITH_UI_H
#define GC_TEAM_WITH_UI_H

#include "ui/rtw_ui.h"
#include "gc_team.h"
#include "game_listener.h"
#include <string>
#include <list>


class GcActor;




class CTeamWithUI:public CClientTeam
{
public:
	enum Operations
	{
		NULL_OPERATION=0,//空
		INVITE,//邀请
		LEAVE,//离开
		DIVID,//驱逐
		DISMISS,//解散
	};
	struct Data
	{
		Data():mpMemberUI(NULL),mpMemberHpUI(NULL),mpMemberMpUI(NULL),mDBID(0){};
		RtwWidget*		mpMemberUI;
		RtwLabel*		mpMemberHeadUI;
		/*RtwLabel*		mpCaptainUI;*/
		RtwLabel*		mpMemberLevelUI;
		RtwLabel*		mpMemberNameUI;
		RtwProgressBar*	mpMemberHpUI;
		RtwProgressBar*	mpMemberMpUI;
		DWORD			mDBID;
	};

	CTeamWithUI();
	virtual ~CTeamWithUI();
	void Init();
	void Reset();
	void Clear(){};

	virtual void TransferCaptain(DWORD vOtherDBID);

	//-----客户端特有的-----
	//队伍数值变化
	virtual void TeamDataChange();
	virtual void TeamMemberDataChange(DWORD vMemberDBID);

	//-------界面操作-------
	void OnCommand(GcActor* vpActor,const string& vrUserCommand);
	void OnPopupMenu(RtwPopupMenu* vpPopupMenu, GcActor* vpTargetActor,int vIndex);
	void UserInterfaceEvent(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void UserLDBClick(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void OnLClickMemberUI(ui::RtwWidget* vpSender, RtwEventDelegate* );
	bool MouseRClick(int vX,int vY);
	void MemberRClick(int vIndex);
	void MemberDBClick(int vIndex);
	void SetEnableHotkey(ui::RtwButton* vpSender);
	void SetTeamOnOff();

	//-----------程序接口-----------
	bool OnMemberRClick(DWORD vDBID,const char* vpName);
	

	void Run(float fSecond);//用来触发时间等问题
	void MessageBoxOK(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);			//用户点击ok
	void MessageBoxCancel(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);		//用户点击cancel

	//-------对用户---------
	virtual void Message(const std::string& vrMessage);
	virtual void MessageWithChoice(DWORD vDBID,const std::string& vrMessage);
	CGameListenerManager		mMemberChoiceListener;
	CGameListenerManager		mMemberDBClick;
private:
	Operations					mState;
	RtwWidget*					mpUIListGroup;
	RtwButton*					mpTeamOnOff;
	Data						mDataList[MAX_TEAM_MENBER_COUNT-1];
};




#endif//GC_TEAM_WITH_UI_H
