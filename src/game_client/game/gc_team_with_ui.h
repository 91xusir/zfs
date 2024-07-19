
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
		NULL_OPERATION=0,//��
		INVITE,//����
		LEAVE,//�뿪
		DIVID,//����
		DISMISS,//��ɢ
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

	//-----�ͻ������е�-----
	//������ֵ�仯
	virtual void TeamDataChange();
	virtual void TeamMemberDataChange(DWORD vMemberDBID);

	//-------�������-------
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

	//-----------����ӿ�-----------
	bool OnMemberRClick(DWORD vDBID,const char* vpName);
	

	void Run(float fSecond);//��������ʱ�������
	void MessageBoxOK(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);			//�û����ok
	void MessageBoxCancel(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);		//�û����cancel

	//-------���û�---------
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
