


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
		NULL_OPERATION=0,//��
		INVITE,//����
		DIVID,//����
	};

	struct SUIData
	{

		SUIData(int vUIListIndex,DWORD vDBID,string& vrName,bool vStudent=false,string vData="")
			:mUIIndex(vUIListIndex),mDBID(vDBID),mName(vrName),mStudent(vStudent),mData(vData){};
		int				mUIIndex;
		DWORD			mDBID;
		string			mName;
		bool			mStudent;//�Ƿ�ѧ��
		string			mData;
	};

	struct WhenFriend//��ʱ���ѽṹ��  
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

	//-------�������-------
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
	void UITemporaryFriendDbClick(ui::RtwWidget*, RtwEventDelegate* vpEvent);//��ʱ����  add by ���� 2010.05.12
	void UITemporaryFriendClick(ui::RtwWidget*, RtwEventDelegate* vpEvent);//��ʱ����  add by ���� 2010.05.12
	void UIMasterPrenticeListDbClick(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void UIMasterPrenticeListClick(ui::RtwWidget* , RtwEventDelegate* vpEvent);
	
	void EditBlackListName(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void EditBlackListNameOK(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	
	void DelPlayer(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void UITitleClick(ui::RtwWidget* vpSender, RtwEventDelegate* );

	void UserHint(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void UIKeyChar(ui::RtwWidget* vpSender, RtwEventDelegate* );

	bool MouseRClick(int vX,int vY);

	void MessageBoxOK(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);		//�û����ȷ��
	void MessageBoxCancel(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);	//�û����ȡ��
	void SetEnableHotkey(ui::RtwButton* vpSender);

	//-----�ͻ������е�-----
	//������ֵ�仯
	virtual void DataChange();
	virtual void MemberDataChange(DWORD vMemberDBID);


	//-----------����ӿ�-----------
	bool OnFriendClick(DWORD vDBID,const char* vpName);//�Լ��ļ�������
	//bool OnEnemyClick(DWORD vDBID,const char* vpName);//�Լ��ļ�������
	bool OnTemporaryFriendClick(DWORD vDBID,const char* vpName);//�Լ��ļ������� add by ���� 2010.05.12
	bool OnBlackListClick(DWORD vDBID,const char* vpName);//�Լ��ļ�������
	bool OnMasterPrenticeListClick(DWORD vDBID,const char* vpName);//�Լ��ļ�������

	//-------���û�---------
	virtual void Message(const std::string& vrMessage);
	virtual void MessageWithChoice(DWORD vFromDBID,const std::string& vrMessage);
	void Run(float vSecond);//��������ʱ�������

	//-----------����ӿ�-----------
	void RealDel(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void StudentOnlineTime(DWORD vDBID,int vTime);



	void OpenUI(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnCloseForm(void* ,void*);
	/* ��Ӻ���ϵͳ */
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
	std::list<WhenFriend>			mUITemporaryFriendList;//��ʱ���� add by ���� 2010.05.27
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