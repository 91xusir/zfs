


#ifndef GC_FRIEND_WITH_UI_H
#define GC_FRIEND_WITH_UI_H



#include "ui/rtw_ui.h"
#include "gc_friend.h"
#include "game_listener.h"
#include <string>
#include <list>


class GcActor;



class CFriendWithUI:public CClientFriend
{
public:
	enum Operations
	{
		NULL_OPERATION=0,//空
		INVITE,//邀请
		DIVID,//驱逐
	};

	struct SUIData
	{

		SUIData(int vUIListIndex,DWORD vDBID,string& vrName,bool vStudent=false,string vData="")
			:mUIIndex(vUIListIndex),mDBID(vDBID),mName(vrName),mStudent(vStudent),mData(vData){};
		int				mUIIndex;
		DWORD			mDBID;
		string			mName;
		bool			mStudent;//是否学生
		string			mData;
	};

	struct WhenFriend//临时好友结构体  
	{
		WhenFriend(string& vrName,long vrDBID,short vrMetier):mName(vrName),mDBID(vrDBID),mMetier(vrMetier){};
		string mName;
		long   mDBID;
		short  mMetier;
	};


	CFriendWithUI();
	virtual ~CFriendWithUI();
	void Init();
	void Reset();
	void Clear(){};

	//-------界面操作-------
	void OnCommand(GcActor* vpActor,const string& vrUserCommand);
	void UserInterfaceEvent(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void OnPopupMenu(RtwPopupMenu* vpPopupMenu, GcActor* vpTargetActor,int vIndex);
	

	void UIClick(RtwListBox* vpUIList,CGameListenerManager& vrEvent,std::list<SUIData>& vrUIDataList);
	void UIClick(RtwListBox* vpUIList,CGameListenerManager& vrEvent,std::list<WhenFriend>& vrUIDataList);

	void UIFriendListDbClick(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void UIFriendListClick(ui::RtwWidget* , RtwEventDelegate* vpEvent);
	//void UIEnemyListDbClick(ui::RtwWidget* vpSender, RtwEventDelegate* );
	//void UIEnemyListClick(ui::RtwWidget* , RtwEventDelegate* vpEvent);
	void UIBlackListDbClick(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void UIBlackListClick(ui::RtwWidget* , RtwEventDelegate* vpEvent);
	void UITemporaryFriendDbClick(ui::RtwWidget*, RtwEventDelegate* vpEvent);//临时好友  add by 李泽华 2010.05.12
	void UITemporaryFriendClick(ui::RtwWidget*, RtwEventDelegate* vpEvent);//临时好友  add by 李泽华 2010.05.12
	void UIMasterPrenticeListDbClick(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void UIMasterPrenticeListClick(ui::RtwWidget* , RtwEventDelegate* vpEvent);
	
	void EditBlackListName(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void EditBlackListNameOK(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	
	void DelPlayer(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void UITitleClick(ui::RtwWidget* vpSender, RtwEventDelegate* );

	void UserHint(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void UIKeyChar(ui::RtwWidget* vpSender, RtwEventDelegate* );

	bool MouseRClick(int vX,int vY);

	void MessageBoxOK(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);		//用户点击确认
	void MessageBoxCancel(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);	//用户点击取消
	void SetEnableHotkey(ui::RtwButton* vpSender);

	//-----客户端特有的-----
	//好友数值变化
	virtual void DataChange();
	virtual void MemberDataChange(DWORD vMemberDBID);


	//-----------程序接口-----------
	bool OnFriendClick(DWORD vDBID,const char* vpName);//自己的监听函数
	//bool OnEnemyClick(DWORD vDBID,const char* vpName);//自己的监听函数
	bool OnTemporaryFriendClick(DWORD vDBID,const char* vpName);//自己的监听函数 add by 李泽华 2010.05.12
	bool OnBlackListClick(DWORD vDBID,const char* vpName);//自己的监听函数
	bool OnMasterPrenticeListClick(DWORD vDBID,const char* vpName);//自己的监听函数

	//-------对用户---------
	virtual void Message(const std::string& vrMessage);
	virtual void MessageWithChoice(DWORD vFromDBID,const std::string& vrMessage);
	void Run(float vSecond);//用来触发时间等问题

	//-----------程序接口-----------
	void RealDel(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void StudentOnlineTime(DWORD vDBID,int vTime);



	void OpenUI(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnCloseForm(void* ,void*);
	/* 添加好友系统 */
	void OnEditAddName(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnEditAddNameOK(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnShowOfflineFriendOrNo(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnPopFriendOperatMenu(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnPopMenuSelect(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnPospMenuLoseFocus(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnEnsureDoAction(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void InsertWhenFriend(string vrName,long vrDBID,short vrMetier);
	void DeleteWhenFriend(DWORD vrDBID);


	CGameListenerManager		mFriendClick;
	CGameListenerManager		mFriendDBClick;
	//CGameListenerManager		mEnemyClick;
	//CGameListenerManager		mEnemyDBClick;
	CGameListenerManager		mTemporaryFriendClick;
	CGameListenerManager		mTemporaryFriendDBClick;
	//CGameListenerManager		mFriendClick;
	CGameListenerManager		mBlackListClick;
	CGameListenerManager		mBlackListDBClick;
	CGameListenerManager		mMasterPrenticeClick;
	CGameListenerManager		mMasterPrenticeDBClick;

private:
	Operations					mState;
	RtwWidget*					mpUI;
	RtwTab*						mpUITab;
	
	RtwListBox*					mpUIFriendList;
	RtwListBox*					mpUITemporaryFriendList;
	//RtwListBox*					mpUIEnemyList;
	RtwListBox*					mpUIBlackList;
	//RtwListBox*					mpUIMasterPrenticeList;// gao

	std::list<SUIData>			mUIFriendList;
	std::list<WhenFriend>			mUITemporaryFriendList;//临时好友 add by 李泽华 2010.05.27
	//std::list<SUIData>          mUIEnemyList;
	std::list<SUIData>          mUIBlackList;
	std::list<SUIData>			mUIMasterPrenticeList;

	RtwButton*					mpUITitle;
	
	
	DWORD						mChoiceFriend;
	DWORD						mChoiceTemporaryFriend;
	//DWORD						mChoiceEnemy;
	string						mChoiceBlackListName;
	DWORD						mChoiceMasterPrentice;
	//
	bool						m_bShowOfflineFriend;
	//popmenu
	RtwPopupMenu*				m_pRDMenu;
};



#endif//GC_FRIEND_WITH_UI_H