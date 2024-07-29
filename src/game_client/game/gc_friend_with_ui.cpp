

#include "gc_include.h"
#include "gc_friend_with_ui.h"
#include "gc_mail_const.h"
#include "ui_form_msg.h"
#include "ui_form_union.h"
#include "ui_form_textMsg.h"
#include "ui_form_cre_info.h"

const char UI_FRIEND_WHOLE_ID[]="fmnewfriendlist";
const char UI_FRIEND_INVITE_ID[]="btnfriend";
const char UI_SPLIT_FLAG[]=".";

//const char UI_HOTKEY_ENABLE_WHOLE_ID[]="fmhotkey.lbhotkeyenable"; //gao 2009.12.18 ȥ��ԭ�Ҽ����ܿ�
const char UI_FRIEND_DELETE_WHOLE_ID[]="fmnewfriendlist.fmfriendlist.btnadd";
const char UI_FRIEND_TITLE_WHOLE_ID[]="fmnewfriendlist.fmfriendlist.btnoffline";


const char UI_SWITCH_TAB_WHOLE_ID[]="fmnewfriendlist.fmfriendlist.tbcteam";

const char UI_FRIEND_LIST_WHOLE_ID[]="fmnewfriendlist.fmfriendlist.tbcteam.fmfriend.listfriend";
//const char UI_ENEMY_LIST_WHOLE_ID[]="fmnewfriendlist.fmfriendlist.tbcteam.fmenemy.listenemy";
const char UI_TEMPORARYFRIEND_LIST_WHOLE_ID[]="fmnewfriendlist.fmfriendlist.tbcteam.fmenemy.listenemy";
const char UI_BLACKLIST_LIST_WHOLE_ID[]="fmnewfriendlist.fmfriendlist.tbcteam.fmblack.lbxmembers";
//const char UI_MASTER_PRENTICE_WHOLE_ID[]="fmfriendlist.tabfriend.fmmaster.listmaster"; // gao

//const char UI_BLACKLIST_ADD_WHOLE_ID[]="fmfriendlist.tabfriend.fmblack.btnaddblack";// gao

const int MAX_BLACKLIST_NAME_LENGTH=30;


const char* UI_FRIEND_PREFIX;//[]="--";
const char* UI_FRIEND_ONLINE_FLAG;//[]="-----���ߺ���-----";
const char* UI_FRIEND_OFFLINE_FLAG;//[]="-----���ߺ���-----";
const char* UI_TEMPORARYFRIEND_ONLINE_FLAG; // lzh
const char* UI_TEMPORARYFRIEND_OFFLINE_FLAG;// lzh
//const char* UI_ENEMY_ONLINE_FLAG;
//const char* UI_ENEMY_OFFLINE_FLAG;
//const char* UI_BLACKLIST_FLAG;


const int UI_TAB_FRIEND=0;
//const int UI_TAB_ENEMY=1;
const int UI_TAB_TEMPORARYFRIEND = 1;//lzh
const int UI_TAB_BLACKLIST=2;
const int UI_TAB_MASTER_PRENTICE=3;


CFriendWithUI::CFriendWithUI()
:CClientFriend(),
mFriendClick(),
mFriendDBClick(),
//mEnemyClick(),
//mEnemyDBClick(),
mBlackListClick(),
mBlackListDBClick(),
mTemporaryFriendClick(),//lzh
mTemporaryFriendDBClick(),//lzh
mState(NULL_OPERATION),
mpUI(NULL),
mChoiceFriend(0),
//mChoiceEnemy(0),
mChoiceBlackListName(),
mChoiceTemporaryFriend(0),
mChoiceMasterPrentice()
{
}

CFriendWithUI::~CFriendWithUI()
{
}



void CFriendWithUI::Init()
{
    guard;
	//������ʼ��
	UI_FRIEND_PREFIX=R(UI_FRIEND_PREFIX);//"--";
	UI_FRIEND_ONLINE_FLAG=R(UI_FRIEND_ONLINE_FLAG);//"-----���ߺ���-----";
	UI_FRIEND_OFFLINE_FLAG=R(UI_FRIEND_OFFLINE_FLAG);//"-----���ߺ���-----";
	UI_TEMPORARYFRIEND_ONLINE_FLAG = R(UI_TEMPORARYFRIEND_ONLINE_FLAG);//lzh
	UI_TEMPORARYFRIEND_OFFLINE_FLAG= R(UI_TEMPORARYFRIEND_OFFLINE_FLAG);//lzh
	//UI_ENEMY_ONLINE_FLAG=R(UI_ENEMY_ONLINE_FLAG);
	//UI_ENEMY_OFFLINE_FLAG=R(UI_ENEMY_OFFLINE_FLAG);
	//UI_BLACKLIST_FLAG=R(UI_BLACKLIST_FLAG);//������

	
	//������򿪺����б�
	LOAD_UI("fmsystem.btnsysfriend")->EvLClick += 
		RTW_CALLBACK(this, CFriendWithUI, OpenUI);


	//���õ�һҳΪĬ��ҳ��
	LOAD_UI_T(RtwTab,UI_SWITCH_TAB_WHOLE_ID)->SetSelectedItem(UI_TAB_FRIEND);

	//<��Ⱥ������¼�>
	LOAD_UI("fmcharall.tabcharall.fmsoc.btnfriend")->EvLClick+=
		RTW_CALLBACK(&(GetWorld()->m_Friend),CFriendWithUI,UserInterfaceEvent);
	//</��Ⱥ������¼�>

	// 2010.3.11
	/*LOAD_UI(UI_FRIEND_DELETE_WHOLE_ID)->EvLClick+=
		RTW_CALLBACK(&(GetWorld()->m_Friend),CFriendWithUI,DelPlayer);*/

	
	//�����ָ��
	mpUI=LOAD_UI(UI_FRIEND_WHOLE_ID);
	mpUITab=LOAD_UI_T(RtwTab,UI_SWITCH_TAB_WHOLE_ID);

	mpUIFriendList=LOAD_UI_T(RtwListBox,UI_FRIEND_LIST_WHOLE_ID);
	//mpUIEnemyList=LOAD_UI_T(RtwListBox,UI_ENEMY_LIST_WHOLE_ID); // ���˽���ָ�� Ŀǰ���� change by ���� 2010.05.12
	mpUITemporaryFriendList = LOAD_UI_T(RtwListBox,UI_TEMPORARYFRIEND_LIST_WHOLE_ID);// ��ʱ���ѽ���ָ�� add by ���� 2010.05.12
	mpUIBlackList=LOAD_UI_T(RtwListBox,UI_BLACKLIST_LIST_WHOLE_ID);
	//mpUIMasterPrenticeList=LOAD_UI_T(RtwListBox,UI_MASTER_PRENTICE_WHOLE_ID);	//gao
	
	mpUITitle = LOAD_UI_T(RtwButton, UI_FRIEND_TITLE_WHOLE_ID);
	

	//�¼��Ĺҽ�
	mpUI->EvKey+=RTW_CALLBACK(this,CFriendWithUI,UIKeyChar);//Ĭ�ϰ�������//Esc//copy
	mpUITitle->EvLClick += RTW_CALLBACK(this,CFriendWithUI,OnShowOfflineFriendOrNo);

	mpUIFriendList->ModifyFlags(wfDClick,0);
	mpUIFriendList->EvSelect +=RTW_CALLBACK(this,CFriendWithUI,UIFriendListClick);
	mpUIFriendList->EvLDClick  +=RTW_CALLBACK(this,CFriendWithUI,UIFriendListDbClick);
	mpUIFriendList->EvRClickItem		+=RTW_CALLBACK(this,CFriendWithUI,OnPopFriendOperatMenu);
	mpUIFriendList->SetDrawScrollBarV(true);

	//mpUIEnemyList->ModifyFlags(wfDClick,0);											//���˽����
	//mpUIEnemyList->EvSelect +=RTW_CALLBACK(this,CFriendWithUI,UIEnemyListClick);		//�¼�����    change by ���� 2010.05.12
	//mpUIEnemyList->EvLDClick  +=RTW_CALLBACK(this,CFriendWithUI,UIEnemyListDbClick);	//Ŀǰû����

	mpUITemporaryFriendList->ModifyFlag(wfDClick,0);													//��ʱ��
	mpUITemporaryFriendList->EvSelect += RTW_CALLBACK(this, CFriendWithUI,UITemporaryFriendClick);		//�ѽ��� add by ����
	mpUITemporaryFriendList->EvLDClick += RTW_CALLBACK(this,CFriendWithUI,UITemporaryFriendDbClick);	//���¼� at 2010.05.12
	mpUITemporaryFriendList->EvRClickItem += RTW_CALLBACK(this,CFriendWithUI,OnPopFriendOperatMenu);	//�Ĺҽ�
	mpUITemporaryFriendList->SetDrawScrollBarV(true);

	mpUIBlackList->ModifyFlags(wfDClick,0);
	mpUIBlackList->EvSelect +=RTW_CALLBACK(this,CFriendWithUI,UIBlackListClick);
	mpUIBlackList->EvLDClick  +=RTW_CALLBACK(this,CFriendWithUI,UIBlackListDbClick);
	mpUIBlackList->EvRClickItem		+=RTW_CALLBACK(this,CFriendWithUI,OnPopFriendOperatMenu);
	mpUIBlackList->SetDrawScrollBarV(true);

	//mpUIMasterPrenticeList->ModifyFlags(wfDClick,0);			//gao
	//mpUIMasterPrenticeList->EvSelect+=RTW_CALLBACK(this,CFriendWithUI,UIMasterPrenticeListClick);//gao
	//mpUIMasterPrenticeList->EvLDClick+=RTW_CALLBACK(this,CFriendWithUI,UIMasterPrenticeListDbClick);//gao

	//��ʦͽ�б��hint�¼�[��Ϊ������̫����ֻ����hint����ʾ���е�����]
	//mpUIMasterPrenticeList->ModifyFlags(wfHint,0);
	//mpUIMasterPrenticeList->EvHint+=RTW_CALLBACK(this,CFriendWithUI,UserHint);						//gao
	

	//LOAD_UI_T(RtwButton,UI_BLACKLIST_ADD_WHOLE_ID)->EvLClick+=RTW_CALLBACK(this,CFriendWithUI,EditBlackListName);// gao
	

	//�����罻������������
	mFriendClick.AddListener<CFriendWithUI,&CFriendWithUI::OnFriendClick>(*this);//�Լ�ҲҪ�����Լ�//����
	//mEnemyClick.AddListener<CFriendWithUI,&CFriendWithUI::OnEnemyClick>(*this);//�Լ�ҲҪ�����Լ�//����  //Ŀǰû��ʹ�� change by ���� at 2010.05.12
	mTemporaryFriendClick.AddListener<CFriendWithUI,&CFriendWithUI::OnTemporaryFriendClick>(*this);//�Լ�ҲҪ�����Լ�//��ʱ���� add by ���� at 2010.05.12
	mBlackListClick.AddListener<CFriendWithUI,&CFriendWithUI::OnBlackListClick>(*this);//�Լ�ҲҪ�����Լ�//������
	mMasterPrenticeClick.AddListener<CFriendWithUI,&CFriendWithUI::OnMasterPrenticeListClick>(*this);//�Լ�ҲҪ�����Լ�//ʦͽ

	//�����罻������˫��//
	mFriendDBClick.AddListener<CMailWithUI,&CMailWithUI::OnMemberChoice>(GetWorld()->m_Mail);//�ʼ��ķ�����ҲҪ����
	mFriendDBClick.AddListener<CChat,&CChat::OnMemberChoice>(GetWorld()->m_Chat);//�����ҲҪ����
//	mMasterPrenticeClick.AddListener<GcPrenticeMgr,&GcPrenticeMgr::OnMasterPrenticeListDBClick>(g_GcPrenticeMgr);//ʦͽ����ҲҪ����

	/* gao 2010.2.4 �ر��¼���ע��*/
	LOAD_UI("fmnewfriendlist.fmfriendlist.btnclose")->EvLClick			+= RTW_CALLBACK(this,CFriendWithUI,OnCloseForm);
	LOAD_UI("fmnewfriendlist.fmfriendlist.btnadd")->EvLClick				+= RTW_CALLBACK(this,CFriendWithUI,OnEditAddName);
	//
	m_pRDMenu = LOAD_UI_T(RtwPopupMenu, "rdfriendmenu");

	m_pRDMenu->EvMenuSelect	+= RTW_CALLBACK(this, CFriendWithUI, OnPopMenuSelect);
	m_pRDMenu->EvUnFocus += RTW_CALLBACK(this,CFriendWithUI,OnPospMenuLoseFocus);
	m_bShowOfflineFriend = false;
    unguard;
}

void CFriendWithUI::UIKeyChar(ui::RtwWidget* vpSender, RtwEventDelegate* e)
{
	//mpUI->DefaultKeyDown(*e);

	int Tab=mpUITab->GetSelectedItem();
	
	switch(Tab)
	{
	case UI_TAB_FRIEND:
		//mpUIFriendList->DefaultKeyChar(vpSender,e);
		break;
	//case UI_TAB_ENEMY:
		//mpUIEnemyList->DefaultKeyDown(vpSender,e);//��ϣ�����˱����ƣ����һֱɧ�������
		//break;
	case UI_TAB_TEMPORARYFRIEND:
		break;
	case UI_TAB_BLACKLIST:
		//mpUIBlackList->DefaultKeyChar(vpSender,e);
		break;
	case UI_TAB_MASTER_PRENTICE:

		//if (e->GetEvent()->type==weNoFocusKeyChar)
		/*if(e->type == etKeyChar)
		{
			if(e->key.code==('C'-'A'+1))
			{
				if(vpSender->IsKeyPressed(vkCtrl))
				{
					std::list<SUIData>::iterator It;
					for(It=mUIMasterPrenticeList.begin();It!=mUIMasterPrenticeList.end();++It)
					{
						if(It->mUIIndex==mpUIMasterPrenticeList->GetSelectedItem())
						{
							g_workspace.getClipboard()->Copy(It->mName);
						}
					}
				}
			}
		}*/ // gao

		//mpUIMasterPrenticeList->DefaultKeyDown(vpSender,e);//��ϣ��ֱ����Ĭ�ϵ�
		
		break;
	default:
		break;
	}

}


void CFriendWithUI::UserHint(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
		/*string Temp;
		
		if(vpSender==mpUIMasterPrenticeList)
		{
			list<SUIData>::iterator It;
			for(It=mUIMasterPrenticeList.begin();It!=mUIMasterPrenticeList.end();++It)
			{
				if(It->mDBID==mChoiceMasterPrentice)
				{
					if(It->mStudent)
					{
						g_workspace.SetHintText(It->mData);
						g_workspace.AutoHintSize();
						g_workspace.SetHintPosition(g_workspace.GetMousePosX(),g_workspace.GetMousePosY());
					}
				}
			}
		}

		return;*/ // gao
}

void CFriendWithUI::DelPlayer(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
    guard;
	
	int Select=mpUITab->GetSelectedItem();

	if(Select==3)
	{
		Message(R(MASTER_PRENTICE_CAN_NOT_DEL));//"ʦͽ��ϵ����ֱ��ɾ��"
		return;
	}


	if( (Select==0 && mChoiceFriend==0) 
		|| (Select==1 && mChoiceTemporaryFriend==0) ||(Select==2 && mChoiceBlackListName=="") )
	{//û��ѡ�����
		Message(R(PLEASE_CHOICE_ONE_PLAYER));//����ѡ��һ����ң�
		return;
	}
	
	UIFormMsg* pConfirm=UIFormMsg::ShowStatic(R(YOU_SURE_DELETE), UIFormMsg::TYPE_OK_CANCEL,true,"FriendDel");

	pConfirm->EvOK = RTW_CALLBACK_1(this,CFriendWithUI,RealDel,pConfirm);

    unguard;
}

void CFriendWithUI::StudentOnlineTime(DWORD vDBID,int vTime)
{
	SConnection* p= mConnection._Find(SConnection::STUDENT,vDBID);
	if(p==NULL)
	{
		LOG("û�����ѧ��\n");
		return ;
	}

	p->mStudentOnlineTime=vTime/(60*60);//��ʱ����Сʱ

	DataChange();

}

void CFriendWithUI::RealDel(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	int Select=mpUITab->GetSelectedItem();

	
	if(mChoiceFriend!=0 && (Select==0))
	{//ɾ�����Ǻ���
		BanishOther(mChoiceFriend);
		mChoiceFriend=0;
	}

	//if(mChoiceEnemy!=0 && (Select==1))	//Ŀǰ
	//{//ɾ�����ǳ���						//û��
	//	DeleteEnemy(mChoiceEnemy);			//ʹ��
	//	mChoiceEnemy=0;						//change by ���� at 2010.05.12
	//}

	if (mChoiceTemporaryFriend!=0 && (Select == 1))
	{//ɾ��������ʱ����  add by ���� at 2010.05.12
		DeleteWhenFriend(mChoiceTemporaryFriend);
		mChoiceTemporaryFriend = 0;
	}

	if(mChoiceBlackListName!="" && (Select==2))
	{//ɾ�����Ǻ�����
		GetWorld()->m_Chat.mBlackList.Del(mChoiceBlackListName);
		mChoiceBlackListName="";
		DataChange();
	}
}




bool CFriendWithUI::OnFriendClick(DWORD vDBID,const char* vpName)//�Լ��ļ�������
{
	mChoiceFriend=vDBID;
	return false;
}

//bool CFriendWithUI::OnEnemyClick(DWORD vDBID,const char* vpName)//�Լ��ļ�������	//Ŀǰ
//{																					//û��
//	mChoiceEnemy=vDBID;																//ʹ��
//	return false;																	//change by ����
//}																					//at 2010.05.12

bool CFriendWithUI::OnTemporaryFriendClick(DWORD vDBID,const char* vpName)//�Լ��ļ�������  add by ���� 2010.05.12
{
	mChoiceTemporaryFriend = vDBID;
	return false;
}
bool CFriendWithUI::OnBlackListClick(DWORD vDBID,const char* vpName)//�Լ��ļ�������
{
	mChoiceBlackListName=vpName;
	return false;
}

bool CFriendWithUI::OnMasterPrenticeListClick(DWORD vDBID,const char* vpName)//�Լ��ļ�������
{
	/*mChoiceMasterPrentice=vDBID;

	list<SUIData>::iterator It;
	for(It=mUIMasterPrenticeList.begin();It!=mUIMasterPrenticeList.end();++It)
	{
		if(It->mDBID==mChoiceMasterPrentice)
		{
			if(It->mStudent)
			{
				mpUIMasterPrenticeList->ModifyFlags(wfHint,0);//��hint

				g_workspace.SetHintText(It->mData);
				g_workspace.AutoHintSize();
				g_workspace.SetHintPosition(g_workspace.GetMousePosX(),g_workspace.GetMousePosY());
				g_workspace.ShowHint(mpUIMasterPrenticeList,0);

				return false;
			}
			break;
		}
	}

	mpUIMasterPrenticeList->ModifyFlags(0,wfHint);//��hint
	g_workspace.HideHint(0);
	return false;*/ // gao
	return false;
}


void CFriendWithUI::OpenUI(ui::RtwWidget* vpSender, RtwEventDelegate*)
{
    guard;
	if (mpUI->GetFlags(wfVisible))
		mpUI->Hide();
	else
		mpUI->Show();
    unguard;
}

//-----�ͻ������е�-----
//������ֵ�仯

void CFriendWithUI::DataChange()
{
    guard;
	mpUIFriendList->RemoveAllItems();
	//mpUIEnemyList->RemoveAllItems(); //����Ŀǰû��ʹ�� change by ���� at 2010.05.12
	mpUITemporaryFriendList->RemoveAllItems();//��ʱ���� add by ���� 2010.05.13
	mpUIBlackList->RemoveAllItems();
	//mpUIMasterPrenticeList->RemoveAllItems();// gao

	mUIFriendList.clear();
	//mUIEnemyList.clear();//change by ���� at 2010.05.13
	//mUITemporaryFriendList.clear();//��ʱ���� add by ���� at 2010.05.13
	mUIBlackList.clear();
	mUIMasterPrenticeList.clear();

	list<SConnection*> OnlineFriend;
	list<SConnection*> OfflineFriend;
	//list<SConnection*> Enemy;//change by ���� at 2010.05.13
	//list<SConnection*> TemporaryFriend;//��ʱ���� add by ���� at 2010.05.13
	//list<string>& rBlackList=GetWorld()->m_Chat.mBlackList.GetList();
	list<SConnection*> BlackList;//������ change by ���� at 2010.05.14
	
	list<SConnection*> Teacher;
	list<SConnection*> Master;
	list<SConnection*> Student;
	list<SConnection*> Prentice;

	map<DWORD,SConnection>::iterator It;list<string>::iterator iT;
	for(It=mConnection.mMap.begin();It!=mConnection.mMap.end();It++)
	{
		if(It->second.Is(SConnection::FRIEND))
		{
			/*bool isFriend = true;	
			for (iT = GetWorld()->m_Chat.mBlackList.GetList().begin();iT!=GetWorld()->m_Chat.mBlackList.GetList().end();iT++)
			{
				if (It->second.mName == iT->substr())
				{
					isFriend = false;
				}
			}
			if (isFriend)
			{*/
				if(It->second.mOnline)
				{
					OnlineFriend.push_back(&(It->second));
				}
				else
				{
					OfflineFriend.push_back(&(It->second));
				}
			/*}*/

		}
		
		//if(It->second.Is(SConnection::ENEMY)) //Ŀǰ
		//{										//û��
		//	Enemy.push_back(&(It->second));		//ʹ��
		//}										//change by ���� at 2010.05.12

		if (It->second.Is(SConnection::BLACKLIST))
		{
			BlackList.push_back(&(It->second));
		}
		//if (It->second.Is(SConnection::WHENFRIEND))		//��ʱ���� add by ���� at 2010.05.13
		//{
		//	TemporaryFriend.push_back(&(It->second));
		//}
		if(It->second.Is(SConnection::TEACHER))
		{
			Teacher.push_back(&(It->second));
		}

		if(It->second.Is(SConnection::MASTER))
		{
			Master.push_back(&(It->second));
		}

		if(It->second.Is(SConnection::STUDENT))
		{
			Student.push_back(&(It->second));
		}

		if(It->second.Is(SConnection::PRENTICE))
		{
			Prentice.push_back(&(It->second));
		}

	}

    int index = 0;
	//�����ݴ浽��ؽṹ��
	list<SConnection*>::iterator ListIt;
	if(OnlineFriend.size()>0)
	{
		mpUIFriendList->AddItem(UI_FRIEND_ONLINE_FLAG);
		++index;
		for(ListIt=OnlineFriend.begin();ListIt!=OnlineFriend.end();ListIt++)
		{
			mUIFriendList.push_back(SUIData((mpUIFriendList->GetItemCount()),(*ListIt)->mDBID,(*ListIt)->mName));
			mpUIFriendList->AddItem((*ListIt)->mName);
			switch ((*ListIt)->mSchool)
			{
			case 1:
				mpUIFriendList->SetItemText(index,1,"��̨");
				break;
			case 2:
				mpUIFriendList->SetItemText(index,1,"����");
				break;
			case 3:
				mpUIFriendList->SetItemText(index,1,"��ɽ");
				break;
			case 4:
				mpUIFriendList->SetItemText(index,1,"�置");
				break;
			default:
				mpUIFriendList->SetItemText(index,1,"");
				break;
			}
			
			char level[256];rt2_sprintf(level,"%d",(*ListIt)->mLevel);
			mpUIFriendList->SetItemText(index,2,string(level));
			mpUIFriendList->SetItemText(index,3,(*ListIt)->mAddress);
			char resolve[256];rt2_sprintf(resolve,"%d",(*ListIt)->mResolve);
			mpUIFriendList->SetItemText(index,4,string(resolve));
			if((*ListIt)->mDBID==mChoiceFriend)
			{//����//����Ϊѡ��״̬
				mpUIFriendList->SetSelectedItem(mpUIFriendList->GetItemCount()-1);
			}
			++index;
		}
	}

	if(OfflineFriend.size()>0 && m_bShowOfflineFriend)
	{
		mpUIFriendList->AddItem(UI_FRIEND_OFFLINE_FLAG);
		++index;
		for(ListIt=OfflineFriend.begin();ListIt!=OfflineFriend.end();ListIt++)
		{
			mUIFriendList.push_back(SUIData((mpUIFriendList->GetItemCount()),(*ListIt)->mDBID ,(*ListIt)->mName) );
			mpUIFriendList->AddItem((*ListIt)->mName);
			switch ((*ListIt)->mSchool)
			{
			case 1:
				mpUIFriendList->SetItemText(index,1,"��̨");
				break;
			case 2:
				mpUIFriendList->SetItemText(index,1,"����");
				break;
			case 3:
				mpUIFriendList->SetItemText(index,1,"��ɽ");
				break;
			case 4:
				mpUIFriendList->SetItemText(index,1,"�置");
				break;
			default:
				mpUIFriendList->SetItemText(index,1,"");
				break;
			}

			char level[256];rt2_sprintf(level,"%d",(*ListIt)->mLevel);
			mpUIFriendList->SetItemText(index,2,string(level));
			mpUIFriendList->SetItemText(index,3,(*ListIt)->mAddress);
			char resolve[256];rt2_sprintf(resolve,"%d",(*ListIt)->mResolve);
			mpUIFriendList->SetItemText(index,4,string(resolve));
			if((*ListIt)->mDBID==mChoiceFriend)
			{//����//����Ϊѡ��״̬
				mpUIFriendList->SetSelectedItem(mpUIFriendList->GetItemCount()-1);
			}
			mpUIFriendList->SetItemColor(index,0xff8f8f8f);
			++index;
		}
	}

	if(mpUIFriendList->GetSelectedItem()==-1)
	{//û��ѡ��//���
		mChoiceFriend=0;
	}
    ///////////////////////////////�����б�Ŀǰû��ʹ�� change by ���� at 2010.05.12////////////////////////////////////////
	//if(Enemy.size()>0)																									//
	//{																														//
	//	for(ListIt=Enemy.begin();ListIt!=Enemy.end();ListIt++)																//
	//	{																													//
	//																														//
	//		Temp=(*ListIt)->mName;																							//
	//		Temp=Temp+"["+fox::Convert<string>((*ListIt)->mKilledCount)+R(UI_ENEMY_KILLED) + "]";							//
	//																														//
	//		mUIEnemyList.push_back(SUIData((mpUIEnemyList->GetItemCount()),(*ListIt)->mDBID ,(*ListIt)->mName,false,Temp));	//
	//		mpUIEnemyList->AddItem(Temp);																					//
	//		if((*ListIt)->mDBID==mChoiceEnemy)																				//
	//		{//����//����Ϊѡ��״̬																							//
	//			mpUIEnemyList->SetSelectedItem(mpUIEnemyList->GetItemCount()-1);											//
	//		}																												//
	//	}																													//
	//}																														//
	//if(mpUIEnemyList->GetSelectedItem()==-1)																				//
	//{//û��ѡ��//���																										//
	//	mChoiceEnemy=0;																										//
	//}																														//
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int indexTemporary = 0;
    list<WhenFriend>::iterator it;
	if (mUITemporaryFriendList.size()>0)// ��ʱ���� add by ���� at 2010.05.12
	{
		for(it = mUITemporaryFriendList.begin();it != mUITemporaryFriendList.end();++it)
		{
			//mUITemporaryFriendList.push_back(SUIData((mpUITemporaryFriendList->GetItemCount()),(*ListIt)->mDBID,(*ListIt)->mName));
			mpUITemporaryFriendList->AddItem(it->mName);
			switch (it->mMetier)
			{
			case 1:
				mpUITemporaryFriendList->SetItemText(indexTemporary,1,"��̨");
				break;
			case 2:
				mpUITemporaryFriendList->SetItemText(indexTemporary,1,"����");
				break;
			case 3:
				mpUITemporaryFriendList->SetItemText(indexTemporary,1,"��ɽ");
				break;
			case 4:
				mpUITemporaryFriendList->SetItemText(indexTemporary,1,"�置");
				break;
			default:
				mpUITemporaryFriendList->SetItemText(indexTemporary,1,"");
				break;
			}

			/*char level[256];sprintf(level,"%d",(*ListIt)->mLevel);
			mpUITemporaryFriendList->SetItemText(indexTemporary,2,string(level));*/
			/*mpUITemporaryFriendList->SetItemText(indexTemporary,3,(*ListIt)->mAddress);*/
			/*char resolve[256];sprintf(resolve,"%d",(*ListIt)->mResolve);*/
			/*mpUITemporaryFriendList->SetItemText(indexTemporary,4,string(resolve));*/
			if (it->mDBID==mChoiceTemporaryFriend)
			{
				mpUITemporaryFriendList->SetSelectedItem(mpUITemporaryFriendList->GetItemCount()-1);
			}
			++indexTemporary;
		}
	}
	if(mpUITemporaryFriendList->GetSelectedItem()==-1)
	{
		mChoiceTemporaryFriend=0;
	}
	int IndexBlackList = 0;
	if(BlackList.size()>0)
	{
		/*list<string>::iterator It;*/
		for(ListIt=BlackList.begin();ListIt!=BlackList.end();++ListIt)
		{
			mUIBlackList.push_back(SUIData((mpUIBlackList->GetItemCount()),(*ListIt)->mDBID,(*ListIt)->mName));
			mpUIBlackList->AddItem((*ListIt)->mName);
			switch ((*ListIt)->mSchool)
			{
			case 1:
				mpUIBlackList->SetItemText(index,1,"��̨");
				break;
			case 2:
				mpUIBlackList->SetItemText(index,1,"����");
				break;
			case 3:
				mpUIBlackList->SetItemText(index,1,"��ɽ");
				break;
			case 4:
				mpUIBlackList->SetItemText(index,1,"�置");
				break;
			default:
				mpUIBlackList->SetItemText(index,1,"");
				break;
			}

			char level[256];rt2_sprintf(level,"%d",(*ListIt)->mLevel);
			mpUIBlackList->SetItemText(index,2,string(level));
			mpUIBlackList->SetItemText(index,3,"δ֪");
			mpUIBlackList->SetItemText(index,4,"��");
			if ((*ListIt)->mDBID==mChoiceTemporaryFriend)
			{
				mpUIBlackList->SetSelectedItem(mpUIBlackList->GetItemCount()-1);
			}
			if((*ListIt)->mName==mChoiceBlackListName)
			{//����//����Ϊѡ��״̬
				mpUIBlackList->SetSelectedItem(mpUIBlackList->GetItemCount()-1);
			}
			++IndexBlackList;
		}
	}
	if(mpUIBlackList->GetSelectedItem()==-1)
	{//û��ѡ��//���
		mChoiceBlackListName="";
	}

	//��ʦ
	/*if(Teacher.size()>0)
	{
		mpUIMasterPrenticeList->AddItem(R(UI_MASTER_FLAG));//---ʦ��---
		for(ListIt=Teacher.begin();ListIt!=Teacher.end();ListIt++)
		{
			mUIMasterPrenticeList.push_back(SUIData((mpUIMasterPrenticeList->GetItemCount()),
				(*ListIt)->mDBID ,(*ListIt)->mName) );

			mpUIMasterPrenticeList->AddItem((*ListIt)->mName);

			if((*ListIt)->mDBID==mChoiceMasterPrentice)
			{//����//����Ϊѡ��״̬
				mpUIMasterPrenticeList->SetSelectedItem(mpUIMasterPrenticeList->GetItemCount()-1);
			}
		}
	}

	//ʦ��
	if(Master.size()>0)
	{
		mpUIMasterPrenticeList->AddItem(R(UI_MASTER_FLAG));//---ʦ��---
		for(ListIt=Master.begin();ListIt!=Master.end();++ListIt)
		{
			mUIMasterPrenticeList.push_back(SUIData((mpUIMasterPrenticeList->GetItemCount()),
				(*ListIt)->mDBID ,(*ListIt)->mName) );

			mpUIMasterPrenticeList->AddItem((*ListIt)->mName);

			if((*ListIt)->mDBID==mChoiceMasterPrentice)
			{//����//����Ϊѡ��״̬
				mpUIMasterPrenticeList->SetSelectedItem(mpUIMasterPrenticeList->GetItemCount()-1);
			}
		}
	}

	//ѧ��
	if(Student.size()>0 || Prentice.size()>0)
	{
		mpUIMasterPrenticeList->AddItem(R(UI_PRENTICE_FLAG));//---ͽ��---
	}

	if(Student.size()>0)
	{
		for(ListIt=Student.begin();ListIt!=Student.end();++ListIt)
		{

			Temp=(*ListIt)->mName;
			Temp=Temp+"["+fox::Convert<string>((*ListIt)->mStudentOnlineTime)+R(UI_STUDENT_TIME)+"]";
			
			mUIMasterPrenticeList.push_back(SUIData((mpUIMasterPrenticeList->GetItemCount()),
				(*ListIt)->mDBID ,(*ListIt)->mName,true,Temp) );

			mpUIMasterPrenticeList->AddItem(Temp);

			if((*ListIt)->mDBID==mChoiceMasterPrentice)
			{//����//����Ϊѡ��״̬
				mpUIMasterPrenticeList->SetSelectedItem(mpUIMasterPrenticeList->GetItemCount()-1);
			}
		}
	}

	//ͽ��
	if(Prentice.size()>0)
	{
		for(ListIt=Prentice.begin();ListIt!=Prentice.end();++ListIt)
		{
			mUIMasterPrenticeList.push_back(SUIData((mpUIMasterPrenticeList->GetItemCount()),
				(*ListIt)->mDBID ,(*ListIt)->mName) );

			int Index=mpUIMasterPrenticeList->AddItem((*ListIt)->mName);
			mpUIMasterPrenticeList->SetItemColor(Index,0xff00aaff);//��ɫ

			if((*ListIt)->mDBID==mChoiceMasterPrentice)
			{//����//����Ϊѡ��״̬
				mpUIMasterPrenticeList->SetSelectedItem(mpUIMasterPrenticeList->GetItemCount()-1);
			}
		}
	}

	if(mpUIMasterPrenticeList->GetSelectedItem()==-1)
	{//û��ѡ��//���
		mChoiceMasterPrentice=0;
	}*/ // gao


    unguard;
}

void CFriendWithUI::MemberDataChange(DWORD vMemberDBID)
{
    guard;
	DataChange();
    unguard;
}



//-------�������-------
void CFriendWithUI::UserInterfaceEvent(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
    guard;
	const string& Sender=vpSender->GetID();

	if(Sender==UI_FRIEND_INVITE_ID)
	{//����
		mState=INVITE;
		SetEnableHotkey((RtwButton*)vpSender);
		Message(gMailStr[CHOOSE_PLAYER_ADD_FRIEND]);
	}
    unguard;
}






//const string POP_MENU_INVITE_STRING("��������"); 
//const string POP_MENU_BANISH_STRING=("ɾ������"); 

const string INVITE_COMMAND("invite");
//const string BANISH_COMMAND("divid");

void CFriendWithUI::OnCommand(GcActor* vpActor,const string& vrUserCommand)
{
	if(vrUserCommand==INVITE_COMMAND)
	{
		InviteOther(vpActor->ID());
	}
	
	return;
}


void CFriendWithUI::OnPopupMenu(ui::RtwPopupMenu* vpPopupMenu, GcActor* vpTargetActor,int vIndex)
{
	if(vpTargetActor!=NULL)
	{//�㵽���
		vpPopupMenu->AddItem(R(POP_MENU_FRIEND_INVITE_STRING),NULL,(void*)vIndex,INVITE_COMMAND);//����
	}
	
	return;
}


void CFriendWithUI::UIClick(RtwListBox* vpUIList,CGameListenerManager& vrEvent,std::list<SUIData>& vrUIDataList)
{
	int Index=vpUIList->GetSelectedItem();
	if(Index==-1)
	{//Ҫ�����¼����������ܱ�֤���û��ѡ�������ʵ������
		vrEvent.OnEvent(0,"");
		return;
	}


	list<SUIData>::iterator It;
	for(It=vrUIDataList.begin();It!=vrUIDataList.end();It++)
	{
		if(It->mUIIndex==Index)
		{
			vrEvent.OnEvent(It->mDBID,It->mName.c_str());//���汻���£�ת��Ϊ��Ա������
			break;
		}
	}

	if(It==vrUIDataList.end())
	{//Ҫ�����¼����������ܱ�֤���û��ѡ�������ʵ������
		vrEvent.OnEvent(0,"");
	}
}

void CFriendWithUI::UIClick(RtwListBox* vpUIList,CGameListenerManager& vrEvent,std::list<WhenFriend>& vrUIDataList)
{
	int Index=vpUIList->GetSelectedItem();
	if(Index==-1)
	{//Ҫ�����¼����������ܱ�֤���û��ѡ�������ʵ������
		vrEvent.OnEvent(0,"");
		return;
	}


	list<WhenFriend>::iterator It;
	for(It=vrUIDataList.begin();It!=vrUIDataList.end();It++)
	{
		if(1==Index)
		{
			vrEvent.OnEvent(It->mDBID,It->mName.c_str());//���汻���£�ת��Ϊ��Ա������
			break;
		}
	}

	if(It==vrUIDataList.end())
	{//Ҫ�����¼����������ܱ�֤���û��ѡ�������ʵ������
		vrEvent.OnEvent(0,"");
	}
}

void CFriendWithUI::EditBlackListName(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
	UIFormTextMsg *pFrm;
	pFrm = UIFormTextMsg::ShowStatic(R(MSG_BLACKLIST_TITLE), R(MSG_BLACKLIST_CONTENT));
	pFrm->EvOK = RTW_CALLBACK_1(this, CFriendWithUI, EditBlackListNameOK,pFrm);
}

void CFriendWithUI::EditBlackListNameOK(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	RtString Name;
	UIFormTextMsg* pForm=(UIFormTextMsg* )vpEvent->GetUser1();
	Name=pForm->GetString();

	string NameString=Name;

	if(NameString.size()>MAX_BLACKLIST_NAME_LENGTH)
	{
		GetWorld()->m_Chat.SystemMessage(R(MSG_BLACKLIST_NAME_OVERFLOW),true);
	}
	else
	{
		/*GetWorld()->m_Chat.AddBlackList(string(Name));*/
		//SendToServer(C2R_ADD_BLACKLIST,)
		AddOtherToBlackList(string(Name));
	}
}

void CFriendWithUI::UIMasterPrenticeListClick(ui::RtwWidget* , RtwEventDelegate* vpEvent)
{
	//UIClick(mpUIMasterPrenticeList,mMasterPrenticeClick,mUIMasterPrenticeList);// gao
}

void CFriendWithUI::UIMasterPrenticeListDbClick(ui::RtwWidget* , RtwEventDelegate* vpEvent)
{
	//UIClick(mpUIMasterPrenticeList,mMasterPrenticeDBClick,mUIMasterPrenticeList);// gao
}

//void CFriendWithUI::UIEnemyListClick(ui::RtwWidget* , RtwEventDelegate* vpEvent)//lzh
//{
//	UIClick(mpUIEnemyList,mEnemyClick,mUIEnemyList);
//}

//void CFriendWithUI::UIEnemyListDbClick(ui::RtwWidget* , RtwEventDelegate* vpEvent)//lzh
//{
//	UIClick(mpUIEnemyList,mEnemyDBClick,mUIEnemyList);
//}

void CFriendWithUI::UITemporaryFriendClick(ui::RtwWidget *, ui::RtwEventDelegate *vpEvent)//��ʱ���� add by ���� 2010.05.12
{
	UIClick(mpUITemporaryFriendList,mTemporaryFriendClick,mUITemporaryFriendList);
}
void CFriendWithUI::UITemporaryFriendDbClick(ui::RtwWidget *,ui::RtwEventDelegate *vpEvent)//��ʱ���� add by ���� 2010.05.12
{
	UIClick(mpUITemporaryFriendList,mTemporaryFriendDBClick,mUITemporaryFriendList);
}
void CFriendWithUI::UIBlackListClick(ui::RtwWidget* , RtwEventDelegate* vpEvent)
{
	UIClick(mpUIBlackList,mBlackListClick,mUIBlackList);
}

void CFriendWithUI::UIBlackListDbClick(ui::RtwWidget* , RtwEventDelegate* vpEvent)
{
	UIClick(mpUIBlackList,mBlackListDBClick,mUIBlackList);
}

void CFriendWithUI::UIFriendListClick(ui::RtwWidget* , RtwEventDelegate* vpEvent)
{
	UIClick(mpUIFriendList,mFriendClick,mUIFriendList);
}

void CFriendWithUI::UIFriendListDbClick(ui::RtwWidget* , RtwEventDelegate* vpEvent)
{
	UIClick(mpUIFriendList,mFriendDBClick,mUIFriendList);
}

void CFriendWithUI::UITitleClick(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
    guard;
	//GetWorld()->m_Mail.OpenMailUI(NULL, NULL);// gao ȡ��ԭ���ĺ���ֱ���ҿ��ʼ��İ�ť
    unguard;
}



void CFriendWithUI::SetEnableHotkey(ui::RtwButton* vpSenderButton)
{
    guard;
	/*RtwButton* pButton = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
	pButton->CloneFrom((RtwButton*)vpSenderButton);
	LOAD_UI_T(RtwAliasButton,UI_HOTKEY_ENABLE_WHOLE_ID)->SetButton(pButton);*/ //gao 2009.12.18 ȥ��ԭ�Ҽ����ܿ�
    unguard;
}


//-------���û�---------
void CFriendWithUI::Message(const std::string& vrMessage)
{
	ShowSystemMessage(vrMessage.c_str());
}

const float FRIEND_INVITE_LIMIT_TIME=30.0;//���������ʱ��

struct SUIFormData
{
	DWORD		mDBID;
	float		mTime;
	UIFormMsg*	mpUI;
	string		m_strMassage;
};


static list<SUIFormData> FriendConfirmList;


void CFriendWithUI::Run(float vSecond)
{
    guard;
	list<SUIFormData>::iterator It;
	for(It=FriendConfirmList.begin();It!=FriendConfirmList.end();)
	{
		(*It).mTime+=vSecond;
		if((*It).mTime>=FRIEND_INVITE_LIMIT_TIME)
		{
			(*It).mpUI->Hide();//hide
			RefuseOtherInvite((*It).mDBID);
			It=FriendConfirmList.erase(It);
		}
		else
		{
			It++;
		}
	}
    unguard;
}


void CFriendWithUI::MessageWithChoice(DWORD vDBID,const std::string& vrMessage)
{
    guard;
	//����˷���Ϣ
	list<SUIFormData>::iterator It;

	for(It=FriendConfirmList.begin();It!=FriendConfirmList.end();It++)
	{
		if((*It).mDBID==vDBID)
		{
			return;
		}
	}

	UIFormMsg* pConfirm=UIFormMsg::ShowStatic(vrMessage.c_str(), UIFormMsg::TYPE_OK_CANCEL,true,"Friend");

	SUIFormData Temp;
	Temp.mDBID=vDBID;
	Temp.mTime=0.0;
	Temp.mpUI=pConfirm;
	Temp.m_strMassage = vrMessage;

	//LOG1("[add:%d]\n",pConfirm);

	FriendConfirmList.push_back(Temp);


	pConfirm->EvOK =
		RTW_CALLBACK_1(this,CFriendWithUI,MessageBoxOK,pConfirm);
	pConfirm->EvCancel =
		RTW_CALLBACK_1(this,CFriendWithUI,MessageBoxCancel,pConfirm);

	return;







/*

	UIFormMsg* pConfirm=UIFormMsg::ShowStatic(vrMessage.c_str(), UIFormMsg::TYPE_OK_CANCEL);

	pConfirm->EvOK+=
		RTW_CALLBACK_1(this,CFriendWithUI,MessageBoxOK,(void*)vDBID);
	pConfirm->EvCancel+=
		RTW_CALLBACK_1(this,CFriendWithUI,MessageBoxCancel,(void*)vDBID);
	return;
	*/
    unguard;
}



void CFriendWithUI::MessageBoxOK(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
    guard;
	void*  pUI=const_cast<void*>(vpSender->GetUser1());
	list<SUIFormData>::iterator It;
	for(It=FriendConfirmList.begin();It!=FriendConfirmList.end();)
	{
		std::string strMessage;
		strMessage =LOAD_UI("ConfirmBox2.lbinfoexit")->GetText();
		if((*It).m_strMassage==strMessage)
		{
			AgreeOtherInvite((*It).mDBID);
			It=FriendConfirmList.erase(It);
			break;
		}
		else
		{
			It++;
		}
	}
    unguard;
}


void CFriendWithUI::MessageBoxCancel(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
    guard;
	void*  pUI=const_cast<void*>(vpSender->GetUser1());
	list<SUIFormData>::iterator It;
	for(It=FriendConfirmList.begin();It!=FriendConfirmList.end();)
	{
		if((*It).mpUI==(UIFormMsg*)pUI)
		{

			RefuseOtherInvite((*It).mDBID);
			It=FriendConfirmList.erase(It);
			break;
		}
		else
		{
			It++;
		}
	}
    unguard;
}


bool CFriendWithUI::MouseRClick(int vX,int vY)
{
    guard;
	bool Return=false;
	switch(mState)
	{
	case INVITE:
		{
			GcActor* pActor=NULL;
			pActor=GetWorld()->HitTest(vX,vY,0xFC);
			if(pActor!=NULL)
			{
				InviteOther(pActor->ID() );
				//mState=NULL_OPERATION;
				Return=true;
			}
			
		}
		break;
	case NULL_OPERATION:
		break;
	default:
		break;
	}
	return Return;
    unguard;
}

void CFriendWithUI::OnCloseForm(void* ,void*)
{
	guard;
	mpUI->Hide();
	unguard;
}
void CFriendWithUI::OnEditAddName(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
	guard;
	if(GetPlayer()->OnGetTarget())
	{
		EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>&  rPlayerMap=GetWorld()->m_ActorManager.m_ActorMap;     // ���н�ɫ����
		EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator It;

		GcActor* pTargetActor;
		for(It=rPlayerMap.begin();It!=rPlayerMap.end();++It)
		{
			if(g_layerMain->m_formMonInfo->GetActorID() == It->second->ID())
				pTargetActor = It->second;
		}

		if (pTargetActor && pTargetActor->NpcType() != GcActor::ENT_MONSTER && pTargetActor->NpcType() != GcActor::ENT_NPC_COMBATIVE && pTargetActor->NpcType() != GcActor::ENT_NPC_SERVICE)
		{
			// ����˵�
			InviteOther(pTargetActor->ID() );
		}
		/*PlayerFailOperate(R(MSG_FRIEND_NOTEXIST));*/
	}else
	{
		UIFormTextMsg *pFrm;
		pFrm = UIFormTextMsg::ShowStatic(R(MSG_BLACKLIST_TITLE), R(MSG_ADDFRIEND_CONTENT));
		pFrm->EvOK = RTW_CALLBACK_1(this, CFriendWithUI, OnEditAddNameOK,pFrm);
		/*PlayerFailOperate(R(MSG_FRIEND_NOTEXIST));*/
	}
	unguard;
}

void CFriendWithUI::OnEditAddNameOK(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	RtString Name;
	UIFormTextMsg* pForm=(UIFormTextMsg* )vpEvent->GetUser1();
	Name=pForm->GetString();
	string NameString=Name;
	if(strlen(NameString.c_str()) == 0)
		return;
	//CG_CmdPacket* pPacket = NetBeginWrite();
	//pPacket->WriteShort(c2r_subsystem_mail);
	//pPacket->WriteByte(C2R_INVITE_OTHER_FRIEND_BYNAME);
	//pPacket->WriteLong(0);
	//pPacket->WriteString(NameString.c_str());
	//NetSend(pPacket);
	//if (!mConnection._Find(SConnection::FRIEND,NameString))
	//{
	//	/*Message(gMailStr[FRIEND_ALREADY_IN_LIST]);
	//	return;*/
	//	char info[128];
	//	rt2_sprintf(info,R(MSG_FRIEND_INVITE),Name.c_str());
	//	PlayerFailOperate(info);
	//}
	if(strcmp(NameString.c_str(),(GetPlayer()->m_core.Name).c_str()))
		InviteOther(NameString);
	else
		Message(gMailStr[SELF_CAN_NOT_ADD_SELF]);
	

	/*if(NameString.size()>MAX_BLACKLIST_NAME_LENGTH)
	{
		GetWorld()->m_Chat.SystemMessage(R(MSG_BLACKLIST_NAME_OVERFLOW),true);
	}*/
	/*else
	{
		GetWorld()->m_Chat.AddBlackList(string(Name));
	}*/
	unguard;
}
void CFriendWithUI::OnShowOfflineFriendOrNo(ui::RtwWidget* vpSender, RtwEventDelegate*)
{
	guard;
	if(m_bShowOfflineFriend)
	{
		m_bShowOfflineFriend = false;
		LOAD_UI("fmnewfriendlist.fmfriendlist.btnoffline")->SetText("��ʾ����");
	}else
	{
		m_bShowOfflineFriend = true;
		LOAD_UI("fmnewfriendlist.fmfriendlist.btnoffline")->SetText("��������");
	}
	DataChange();
	unguard;
}
void CFriendWithUI::OnPopFriendOperatMenu(ui::RtwWidget* vpSender, RtwEventDelegate*)
{
	guard;
	int select = mpUITab->GetSelectedItem();
	switch((APPLYLISTTYPE)select)
	{
	case 0:
		if(mpUIFriendList->GetItemText(mpUIFriendList->GetSelectedItem(),0) == UI_FRIEND_ONLINE_FLAG 
			|| mpUIFriendList->GetItemText(mpUIFriendList->GetSelectedItem(),0) == UI_FRIEND_OFFLINE_FLAG)
			return;
		m_pRDMenu->RemoveAllItems();
		m_pRDMenu->SetText(mpUIFriendList->GetItemText(mpUIFriendList->GetSelectedItem(),0));
		m_pRDMenu->AddItem(R(POP_MENU_DELFRIEND), 0, (void*)1);
		m_pRDMenu->AddItem(R(POP_MENU_FRIENDBLACK), 0, (void*)2);
		m_pRDMenu->AddItem(R(POP_MENU_WHISPER), 0, (void*)3);
		m_pRDMenu->AddItem(R(POP_MENU_SENDMAIL), 0, (void*)4);
		/*m_pRDMenu->AddItem(R(G_SEEITEMS), 0, (void*)5);*/
		if(!GetWorld()->m_Team.mData.Size()||GetWorld()->m_Team.mData.GetCaptain() == GetPlayer()->DBID())
			m_pRDMenu->AddItem(R(POP_MENU_INVITE_STRING),NULL,(void*)5);//����
		if(!GetWorld()->m_Team.mData.Size())
			m_pRDMenu->AddItem(R(POP_MENU_APPLYTEAM),NULL,(void*)6);//����
		m_pRDMenu->AddItem(R(POP_MENU_COPY), 0, (void*)7);
		g_workspace.ShowPopupMenu(m_pRDMenu, NULL, g_workspace.GetMousePosX(), g_workspace.GetMousePosY());
		break;
	case 1:
		m_pRDMenu->RemoveAllItems();
		m_pRDMenu->SetText(mpUITemporaryFriendList->GetItemText(mpUITemporaryFriendList->GetSelectedItem(),0));
		m_pRDMenu->AddItem(R(POP_MENU_DELFRIEND), 0, (void*)1);
		m_pRDMenu->AddItem(R(POP_MENU_FRIENDBLACK), 0, (void*)2);
		m_pRDMenu->AddItem(R(POP_MENU_WHISPER), 0, (void*)3);
		m_pRDMenu->AddItem(R(POP_MENU_SENDMAIL), 0, (void*)4);
		/*m_pRDMenu->AddItem(R(G_SEEITEMS), 0, (void*)5);*/
		if(!GetWorld()->m_Team.mData.Size()||GetWorld()->m_Team.mData.GetCaptain() == GetPlayer()->DBID())
			m_pRDMenu->AddItem(R(POP_MENU_INVITE_STRING),NULL,(void*)5);//����
		if(!GetWorld()->m_Team.mData.Size())
			m_pRDMenu->AddItem(R(POP_MENU_APPLYTEAM),NULL,(void*)6);//����
		m_pRDMenu->AddItem(R(POP_MENU_COPY), 0, (void*)7);
		m_pRDMenu->AddItem(R(POP_MENU_ADDFRIEND), 0, (void*)8);
		g_workspace.ShowPopupMenu(m_pRDMenu, NULL, g_workspace.GetMousePosX(), g_workspace.GetMousePosY());
		break;
	case 2:
		m_pRDMenu->RemoveAllItems();
		m_pRDMenu->SetText(mpUIBlackList->GetItemText(mpUIBlackList->GetSelectedItem(),0));
		m_pRDMenu->AddItem(R(POP_MENU_PULLOUTBLACK), 0, (void*)2);
		m_pRDMenu->AddItem(R(POP_MENU_ADDFRIEND), 0, (void*)1);
		g_workspace.ShowPopupMenu(m_pRDMenu, NULL, g_workspace.GetMousePosX(), g_workspace.GetMousePosY());
		break;
	}
	unguard;
}
void CFriendWithUI::OnPopMenuSelect(ui::RtwWidget* vpSender, RtwEventDelegate*)
{
	guard;
	int iSelect = m_pRDMenu->GetLastSelectItem();
	int select = mpUITab->GetSelectedItem();
	switch((APPLYLISTTYPE)select)
	{
	case 0:
		switch(iSelect)
		{
		case 0:
			DelPlayer(mpUIFriendList,NULL);
			break;
		case 1:
			if(mpUIFriendList->GetSelectedItem() != -1)
			{
				UIFormMsg* pConfirm=UIFormMsg::ShowStatic(R(YOU_ESURE_ADDBLACK), UIFormMsg::TYPE_OK_CANCEL,true,"FriendDel");
				pConfirm->EvOK =RTW_CALLBACK_1(this,CFriendWithUI,OnEnsureDoAction,pConfirm);
			}
			break;
		case 2:
			if(mpUIFriendList->GetSelectedItem() != -1)
			{
				GetWorld()->m_Chat.OnMemberChoice(0, mpUIFriendList->GetItemText(mpUIFriendList->GetSelectedItem(),0).c_str());
			}
			break;
		case 3:
			if(mpUIFriendList->GetSelectedItem() != -1)
			{
				GetWorld()->m_Mail.NewMail(NULL,NULL);
				LOAD_UI("fmmail4.fmcontent.ebpeople1")->SetText(mpUIFriendList->GetItemText(mpUIFriendList->GetSelectedItem(),0));
			}
			break;
		//case 4:
		//	{
		//		//CG_CmdPacket* pPacket = NetBeginWrite();
		//		//pPacket->WriteShort(c2r_query_player_info);
		//		//pPacket->WriteLong(pTargetActor->ID());
		//		//NetSend(pPacket);
		//	}
		//	break;
		case 4:
			if(mpUIFriendList->GetSelectedItem() != -1)
			{
				GetWorld()->m_Team.InviteOther(m_pRDMenu->getText());
			}
			break;
		case 5:
			if(mpUIFriendList->GetSelectedItem() != -1)
			{
				GetWorld()->m_Team.ApplyOtherTeam(m_pRDMenu->getText());
			}
			break;
		case 6:
			g_workspace.getClipboard()->Copy(mpUIFriendList->GetItemText(mpUIFriendList->GetSelectedItem(),0));
			break;
		default:
			break;
		}
		break;
	case 1:
		switch(iSelect)
		{
		case 0:
			DeleteWhenFriend(mChoiceTemporaryFriend);
			break;
		case 1:
			if(mpUITemporaryFriendList->GetSelectedItem() != -1)
			{
				UIFormMsg* pConfirm=UIFormMsg::ShowStatic(R(YOU_ESURE_ADDBLACK), UIFormMsg::TYPE_OK_CANCEL,true,"FriendDel");
				pConfirm->EvOK =RTW_CALLBACK_1(this,CFriendWithUI,OnEnsureDoAction,pConfirm);
			}
			break;
		case 2:
			if(mpUITemporaryFriendList->GetSelectedItem() != -1)
			{
				GetWorld()->m_Chat.OnMemberChoice(0, mpUITemporaryFriendList->GetItemText(mpUITemporaryFriendList->GetSelectedItem(),0).c_str());
			}
			break;
		case 3:
			if(mpUITemporaryFriendList->GetSelectedItem() != -1)
			{
				GetWorld()->m_Mail.NewMail(NULL,NULL);
				LOAD_UI("fmmail4.fmcontent.ebpeople1")->SetText(mpUITemporaryFriendList->GetItemText(mpUITemporaryFriendList->GetSelectedItem(),0));
			}
			break;
		//case 4:
		//	{
		//		//CG_CmdPacket* pPacket = NetBeginWrite();
		//		//pPacket->WriteShort(c2r_query_player_info);
		//		//pPacket->WriteLong(pTargetActor->ID());
		//		//NetSend(pPacket);
		//	}
		//	break;
		case 4:
			if(mpUITemporaryFriendList->GetSelectedItem() != -1)
			{
				GetWorld()->m_Team.InviteOther(m_pRDMenu->getText());
			}
			break;
		case 5:
			if(mpUITemporaryFriendList->GetSelectedItem() != -1)
			{
				GetWorld()->m_Team.ApplyOtherTeam(m_pRDMenu->getText());
			}
			break;
		case 6:
			g_workspace.getClipboard()->Copy(mpUITemporaryFriendList->GetItemText(mpUITemporaryFriendList->GetSelectedItem(),0));
			break;
		case 7:
			if(mpUITemporaryFriendList->GetSelectedItem() != -1)
			{
				InviteOther(m_pRDMenu->getText());
			}
			break;
		default:
			break;
		}
		break;
	case 2:
		switch(iSelect)
		{
		case 0:
			if(mpUIBlackList->GetSelectedItem() != -1)
			{
				UIFormMsg* pConfirm=UIFormMsg::ShowStatic(R(YOU_SURE_DELETE), UIFormMsg::TYPE_OK_CANCEL,true,"FriendDel");
				pConfirm->EvOK = RTW_CALLBACK_1(this,CFriendWithUI,OnEnsureDoAction,pConfirm);
			}
			break;
		case 1:
			/*if(mpUIBlackList->GetSelectedItem() != -1)
			{
				UIFormMsg* pConfirm=UIFormMsg::ShowStatic(R(YOU_SURE_DELETE), UIFormMsg::TYPE_OK_CANCEL,true,"FriendDel");
				pConfirm->EvOK+=RTW_CALLBACK(this,CFriendWithUI,OnEnsureDoAction);
			}*/
			if(mpUIBlackList->GetSelectedItem() != -1)
			{
				/*CG_CmdPacket* pPacket = NetBeginWrite();
				if (!pPacket)
					return;
				pPacket->WriteShort(c2r_subsystem_mail);
				pPacket->WriteByte(C2R_INVITE_OTHER_FRIEND_BYNAME);
				pPacket->WriteLong(0);
				pPacket->WriteString(mpUIBlackList->GetItemText(mpUIBlackList->GetSelectedItem(),0).c_str());
				NetSend(pPacket);
				char info[128];
				rt2_sprintf(info,R(MSG_FRIEND_INVITE),mpUIBlackList->GetItemText(mpUIBlackList->GetSelectedItem(),0).c_str());
				PlayerFailOperate(info);*/
				InviteOther(mpUIBlackList->GetItemText(mpUIBlackList->GetSelectedItem(),0));
				DWORD ID = GetDBID(SConnection::BLACKLIST,mpUIBlackList->GetItemText(mpUIBlackList->GetSelectedItem(),0));
				DeleteOtherFromBlackList(ID);
			}
			break;
		default:
			break;
		}
		break;
	}
	unguard;
}
void CFriendWithUI::OnPospMenuLoseFocus(ui::RtwWidget* vpSender, RtwEventDelegate*)
{
	guard;
	if(m_pRDMenu->IsVisible())
		m_pRDMenu->Hide();
	unguard;
}
void CFriendWithUI::OnEnsureDoAction(ui::RtwWidget* vpSender, RtwEventDelegate*)
{
	guard;
	int iSelect = m_pRDMenu->GetLastSelectItem();
	int select = mpUITab->GetSelectedItem();
	switch((APPLYLISTTYPE)select)
	{
	case 0:
		switch(iSelect)
		{
		case 0:
			break;
		case 1:
			if(mpUIFriendList->GetSelectedItem() != -1)
			{
				//GetWorld()->m_Chat.AddBlackList(mpUIFriendList->GetItemText(mpUIFriendList->GetSelectedItem(),0));
				AddOtherToBlackList(mpUIFriendList->GetItemText(mpUIFriendList->GetSelectedItem(),0));
			}
			break;
		default:
			break;
		}
		break;
	case 1:
		switch(iSelect)
		{
		case 1:
			if(mpUITemporaryFriendList->GetSelectedItem() != -1)
			{
				//GetWorld()->m_Chat.AddBlackList(mpUIFriendList->GetItemText(mpUIFriendList->GetSelectedItem(),0));
				AddOtherToBlackList(mpUITemporaryFriendList->GetItemText(mpUIFriendList->GetSelectedItem(),0));
			}
			break;
		default:
			break;
		}
		break;
	case 2:
		switch(iSelect)
		{
		case 0:
			if(mpUIBlackList->GetSelectedItem() != -1)
			{
				/*GetWorld()->m_Chat.mBlackList.Del(mpUIBlackList->GetItemText(mpUIBlackList->GetSelectedItem(),0));
				DataChange();*/
				DWORD ID = GetDBID(SConnection::BLACKLIST,mpUIBlackList->GetItemText(mpUIBlackList->GetSelectedItem(),0));
				DeleteOtherFromBlackList(ID);
			}
			break;
		case 1:
			break;
		default:
			break;
		}
		break;
	}
	unguard;
}

void CFriendWithUI::InsertWhenFriend(std::string vrName, long vrDBID, short vrMetier)
{
	guard;
	list<WhenFriend>::iterator WFit;
	for (WFit = mUITemporaryFriendList.begin(); WFit != mUITemporaryFriendList.end();++WFit)
		if ((*WFit).mDBID == vrDBID)
			return;
	WhenFriend sWhenFriend(vrName,vrDBID,vrMetier);
	mUITemporaryFriendList.push_back(sWhenFriend);
	DataChange();
	unguard;
}

void CFriendWithUI::DeleteWhenFriend(DWORD vrDBID)
{
	guard;
	list<WhenFriend>::iterator WFIt;
	for (WFIt = mUITemporaryFriendList.begin(); WFIt != mUITemporaryFriendList.end();)
	{
		if (WFIt->mDBID == vrDBID)
			WFIt = mUITemporaryFriendList.erase(WFIt);
		else
			++WFIt;
	}
	unguard;
}