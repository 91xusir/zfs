
#include "gc_team_with_ui.h"
#include "gc_include.h"
#include "gc_team_const.h"
#include "ui_form_msg.h"
#include "ui_form_union.h"


using std::string;

class GcActor;

const float TEAM_INVITE_LIMIT_TIME=30.0;//���������ʱ��

//�����Ψһ��ID
const char UI_INVITE_ID[]="btninvite";			//���밴ť
const char UI_LEAVE_ID[]="btnleave";			//�뿪��ť
const char UI_DIVID_ID[]="btndivid";			//����ť
const char UI_DISMISS_ID[]="btnteamclose";		//��ɢ��ť

const char TEAM_WHOLE_ID[]="fmhplist";

const char TEAM_NAME_ID[]="lblvt";
const char TEAM_HP_ID[]="barhp";
const char TEAM_MP_ID[]="barmp";
const char TEAM_HEAD_ID[]="lbpartface";
const char TEAM_LEVEL_ID[]="lbpartlv";
const char TEAM_CAPTAIN[]="lbcaptain";

const char UI_SPLIT_FLAG[]=".";

//const char UI_HOTKEY_ENABLE_WHOLE_ID[]="fmhotkey.lbhotkeyenable"; //gao 2009.12.18 ȥ��ԭ�Ҽ����ܿ�

const char* TEAM_LIST_WHOLE_ID[MAX_TEAM_MENBER_COUNT]=
{
	"fmhplist.fmhplist1",
	"fmhplist.fmhplist2",
	"fmhplist.fmhplist3",
	"fmhplist.fmhplist4",
	"fmhplist.fmhplist5"
};

CTeamWithUI::CTeamWithUI()
:CClientTeam(),
mMemberChoiceListener(),
mState(NULL_OPERATION),
mpUIListGroup(NULL)
{
	//g_workspace.Load("ui/ui_layer_main.xml");
	//mpTeamOnOff = LOAD_UI_T(RtwButton, "fmdialg.btnPKmode");
	//mpTeamOnOff->CenterScreen();
	//mpTeamOnOff->EvLClick += RTW_CALLBACK(this, CTeamWithUI, SetTeamOnOff);
	//mpTeamOnOff->Show();
}

 

CTeamWithUI::~CTeamWithUI()
{
}

void CTeamWithUI::SetTeamOnOff()
{
	//GetPlayer->m_bTeamOnOff = !GetPlayer->m_bTeamOnOff;
	//if (GetPlayer->m_bTeamOnOff)
	//{
	//	SendToServer(C2R_TEAM_ONOFF,1);
	//}else{
	//	SendToServer(C2R_TEAM_ONOFF,0);
	//}
}

void CTeamWithUI::Reset()
{
	
}

void CTeamWithUI::Init()
{
    guard;
	//<��Ⱥ������¼�>
	LOAD_UI("fmcharall.tabcharall.fmsoc.btninvite")->EvLClick+=
		RTW_CALLBACK(&(GetWorld()->m_Team),CTeamWithUI,UserInterfaceEvent);
	LOAD_UI("fmcharall.tabcharall.fmsoc.btnleave")->EvLClick+=
		RTW_CALLBACK(&(GetWorld()->m_Team),CTeamWithUI,UserInterfaceEvent);
	LOAD_UI("fmhplist.btnleave")->EvLClick+=
		RTW_CALLBACK(&(GetWorld()->m_Team),CTeamWithUI,UserInterfaceEvent);
	LOAD_UI("fmcharall.tabcharall.fmsoc.btndivid")->EvLClick+=
		RTW_CALLBACK(&(GetWorld()->m_Team),CTeamWithUI,UserInterfaceEvent);
	LOAD_UI("fmcharall.tabcharall.fmsoc.btnteamclose")->EvLClick+=
		RTW_CALLBACK(&(GetWorld()->m_Team),CTeamWithUI,UserInterfaceEvent);
	//</��Ⱥ������¼�>



	//������б��Ҽ����
	for(int i=0;i<MAX_TEAM_MENBER_COUNT-1;i++)
	{
		LOAD_UI(TEAM_LIST_WHOLE_ID[i])->ModifyFlags(wfGrabMouse|wfClick|wfDClick,0);
		LOAD_UI(TEAM_LIST_WHOLE_ID[i])->EvRClick+=
			RTW_CALLBACK(this,CTeamWithUI,UserInterfaceEvent);
		LOAD_UI(TEAM_LIST_WHOLE_ID[i])->EvLDClick+=
			RTW_CALLBACK(this,CTeamWithUI,UserLDBClick);
	}
	//</������б��Ҽ����>


	mMemberChoiceListener.AddListener<CTeamWithUI,&CTeamWithUI::OnMemberRClick>(*this);//�Լ�ҲҪ�����Լ��Ķ�Ա�����ϵ�����Ҽ�������
	mMemberChoiceListener.AddListener<GcWorld,&GcWorld::OnMemberRClick>((*GetWorld()));//��Ϸ����һЩ�ط�������Ա���ϵ�����Ҽ�������


	//���������б�������˫��//
	mMemberDBClick.AddListener<CMailWithUI,&CMailWithUI::OnMemberChoice>(GetWorld()->m_Mail);//�ʼ��ķ�����ҲҪ����
	mMemberDBClick.AddListener<CChat,&CChat::OnMemberChoice>(GetWorld()->m_Chat);//�����ҲҪ����

	
	string Temp1;
	string Temp2;
	for(int i=0;i<MAX_TEAM_MENBER_COUNT-1;i++)
	{
		mDataList[i].mpMemberUI=LOAD_UI(TEAM_LIST_WHOLE_ID[i]);

		mDataList[i].mpMemberUI->EvLClick += RTW_CALLBACK(this,CTeamWithUI,OnLClickMemberUI);
		
		Temp1=TEAM_LIST_WHOLE_ID[i];

		Temp2=Temp1+UI_SPLIT_FLAG+TEAM_NAME_ID;
		mDataList[i].mpMemberNameUI=LOAD_UI_T(RtwLabel, Temp2.c_str());
		
		Temp2=Temp1+UI_SPLIT_FLAG+TEAM_HP_ID;
		mDataList[i].mpMemberHpUI=LOAD_UI_T(RtwProgressBar, Temp2.c_str());
		
		Temp2=Temp1+UI_SPLIT_FLAG+TEAM_MP_ID;
		mDataList[i].mpMemberMpUI=LOAD_UI_T(RtwProgressBar, Temp2.c_str());

		Temp2=Temp1+UI_SPLIT_FLAG+TEAM_HEAD_ID;
		mDataList[i].mpMemberHeadUI=LOAD_UI_T(RtwLabel, Temp2.c_str());

		Temp2=Temp1+UI_SPLIT_FLAG+TEAM_LEVEL_ID;
		mDataList[i].mpMemberLevelUI=LOAD_UI_T(RtwLabel, Temp2.c_str());

		/*Temp2=Temp1+UI_SPLIT_FLAG+TEAM_CAPTAIN;
		mDataList[i].mpCaptainUI=LOAD_UI_T(RtwLabel, Temp2.c_str());*/

		mDataList[i].mpMemberUI->Hide();
	}
	mpUIListGroup=LOAD_UI(TEAM_WHOLE_ID);
	mpUIListGroup->Hide();
	//LOAD_UI("fmhplistla")->Hide();//max// gao 2009.12.25 �˴�һ������ͼ��
	TeamDataChange();
    unguard;
}

void CTeamWithUI::SetEnableHotkey(ui::RtwButton* vpSenderButton)
{
    guard;
	//	RtwWidget*	GetWidget(const std::string& id);

//	RtwButton* pButton=LOAD_UI_T(RtwAliasButton,UI_HOTKEY_ENABLE_WHOLE_ID)->GetButton();
//	pButton->GetQualifiedID();
	/*RtwButton* pButton = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
	pButton->CloneFrom((RtwButton*)vpSenderButton);
	LOAD_UI_T(RtwAliasButton,UI_HOTKEY_ENABLE_WHOLE_ID)->SetButton(pButton);*/ //gao 2009.12.18 ȥ��ԭ�Ҽ����ܿ�
    unguard;
}

//const string POP_MENU_INVITE_STRING("����������"); 
//const string POP_MENU_LEAVE_STRING=("�뿪����"); 
//const string POP_MENU_BANISH_STRING=("�Ӷ���������"); 
//const string POP_MENU_DISMISS_STRING=("��ɢ����"); 

const string INVITE_COMMAND("invite");
const string LEAVE_COMMAND("leave");
const string BANISH_COMMAND("divid");
const string DISMISS_COMMAND("dismiss");
const string APPLY_COMMAND("apply");



void CTeamWithUI::OnCommand(GcActor* vpActor,const string& vrUserCommand)
{
    guard;
	if(vrUserCommand==INVITE_COMMAND)
	{
		InviteOther(vpActor->ID());
	}
	else if(vrUserCommand==LEAVE_COMMAND)
	{
		SelfLeave();
	}
	else if(vrUserCommand==BANISH_COMMAND)
	{
		BanishOtherByRegionID(vpActor->ID());
	}
	else if(vrUserCommand==DISMISS_COMMAND)
	{
		Dismiss();
	}else if (vrUserCommand == APPLY_COMMAND)
	{
		ApplyOtherTeam(vpActor->ID());
	}
	return;
    unguard;
}

void CTeamWithUI::OnPopupMenu(RtwPopupMenu* vpPopupMenu, GcActor* vpTargetActor,int vIndex)
{
    guard;
	
	if(mData.Empty())
	{//û�ж���
		if(vpTargetActor==NULL)
		{//û�е㵽���//�������κ���
			return;
		}
		else
		{//�㵽���//������������
			if (vpTargetActor->m_cInTeam == 0)
			{
				vpPopupMenu->AddItem(R(POP_MENU_INVITE_STRING),NULL,(void*)vIndex,INVITE_COMMAND);//����
			}else{
				vpPopupMenu->AddItem("�������",NULL,(void*)vIndex,APPLY_COMMAND);//����
			}
		}
	}
	else
	{//�ж���
		if(vpTargetActor==NULL)
		{//û�е㵽���
			if(mData.GetCaptain()==mSelfDBID)
			{//�Ƕӳ�
				vpPopupMenu->AddItem(R(POP_MENU_DISMISS_STRING),NULL,(void*)vIndex,DISMISS_COMMAND);//��ɢ����
			}
			vpPopupMenu->AddItem(R(POP_MENU_LEAVE_STRING),NULL,(void*)vIndex,LEAVE_COMMAND);//�뿪����
		}
		else
		{//�㵽���
			if(mData.GetCaptain()==mSelfDBID)
			{//�Ƕӳ�
				if(mData.Exist(vpTargetActor->GetName()))
				{//������ڶ�����
					vpPopupMenu->AddItem(R(POP_MENU_BANISH_STRING),NULL,(void*)vIndex,BANISH_COMMAND);//����
				}
				else
				{//����Ҳ��ڶ�����
					vpPopupMenu->AddItem(R(POP_MENU_INVITE_STRING),NULL,(void*)vIndex,INVITE_COMMAND);//����
				}
			}
			else
			{//���Ƕӳ�//����
				std::string strItemName;
				if(mData.Exist(vpTargetActor->GetName()))
				{//������ڶ�����//�������κ�����
					vpPopupMenu->AddItem(R(POP_MENU_BANISH_STRING),NULL,(void*)vIndex,BANISH_COMMAND);//����
					strItemName = R(POP_MENU_BANISH_STRING);
				}
				else
				{//����Ҳ��ڶ�����//�������κ�����
					vpPopupMenu->AddItem(R(POP_MENU_INVITE_STRING),NULL,(void*)vIndex,INVITE_COMMAND);//����
					strItemName = R(POP_MENU_INVITE_STRING);
				}
				vpPopupMenu->EnableIndex(strItemName, false);
			}
		}
	}
	return;
    unguard;
}

void CTeamWithUI::UserLDBClick(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
	for(int i=0;i<MAX_TEAM_MENBER_COUNT-1;i++)
	{
		if(vpSender==mDataList[i].mpMemberUI)
		{
			MemberDBClick(i);
			break;
		}
	}
	return;
}

void CTeamWithUI::UserInterfaceEvent(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
    guard;
	const string& Sender=vpSender->GetID();

	if(Sender==UI_INVITE_ID)
	{//����
		if(mData.Empty() || mData.GetCaptain()==mSelfDBID )
		{//û�ж�����Ƕӳ�
			SetEnableHotkey((RtwButton*)vpSender);
			mState=INVITE;
			Message(gTeamStr[CHOOSE_MEMBER_ADD_TEAM]);
		}
		else
		{//���Ƕӳ�
			Message(gTeamStr[NOT_TEAM_CAPTAIN]);
		}
	}
	else if(Sender==UI_LEAVE_ID)
	{//�뿪
		//SetEnableHotkey((RtwButton*)vpSender);
		mState=LEAVE;
		SelfLeave();
	}
	else if(Sender==UI_DIVID_ID)
	{//����
		SetEnableHotkey((RtwButton*)vpSender);
		if(mData.Empty())
		{
			Message(gTeamStr[TEAM_NOT_EXIST]);
		}
		else
		{
			if(mData.GetCaptain()==mSelfDBID )
			{//�Ƕӳ�
				mState=DIVID;
				Message(gTeamStr[CHOOSE_MEMBER_BANISH_FROM_TEAM]);
			}
			else
			{//���Ƕӳ�
				Message(gTeamStr[NOT_TEAM_CAPTAIN]);
			}
		}
	}
	else if(Sender==UI_DISMISS_ID)
	{//��ɢ
		SetEnableHotkey((RtwButton*)vpSender);
		mState=DISMISS;
		Dismiss();
	}
	else
	{
		for(int i=0;i<MAX_TEAM_MENBER_COUNT-1;i++)
		{
			if(vpSender==mDataList[i].mpMemberUI)
			{
				MemberRClick(i);
				break;
			}
		}
	}
    unguard;
}

void CTeamWithUI::MemberDBClick(int vIndex)
{
	int Index=0;
	int i;
	for(i=mData.Begin();i!=mData.End();i++)
	{
		if(mData[i].mDBID!=mSelfDBID)
		{
			if(vIndex==Index)
			{
				break;
			}
			Index++;
		}
	}

	if(i==mData.End())
	{
		return;
	}
	else
	{
		mMemberDBClick.OnEvent(mData[i].mDBID,mData[i].mName.c_str());
	}
	return;
}

void CTeamWithUI::MemberRClick(int vIndex)
{
    guard;
	int Index=0;
	int i;
	for(i=mData.Begin();i!=mData.End();i++)
	{
		if(mData[i].mDBID!=mSelfDBID)
		{
			if(vIndex==Index)
			{
				break;
			}
			Index++;
		}
	}

	if(i==mData.End())
	{
		return;
	}
	else
	{
		mMemberChoiceListener.OnEvent(mData[i].mDBID,mData[i].mName.c_str());
	}
	return;
    unguard;
}


bool CTeamWithUI::OnMemberRClick(DWORD vDBID,const char* vpName)
{
    guard;
	if(mState==DIVID)
	{
		BanishOtherByDBID(vDBID);
		//mState=NULL_OPERATION;
		return true;
	}
	else
	{
		return false;
	}
    unguard;
}

bool CTeamWithUI::MouseRClick(int vX,int vY)
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
	case DIVID:
		{
			GcActor* pActor=NULL;
			pActor=GetWorld()->HitTest(vX,vY,0xFC);
			if(pActor!=NULL)
			{
				BanishOtherByRegionID(pActor->ID());
				//mState=NULL_OPERATION;
				Return=true;
			}
		}
		break;
	case NULL_OPERATION:
	default:
		break;
	}

	return Return;
    unguard;
}


void CTeamWithUI::TransferCaptain(DWORD vOtherDBID)
{
	if (mData.TransferCaptain(vOtherDBID))
	{
		if (mData.GetCaptain() == mSelfDBID)
		{
			rt2_sprintf(g_strStaticBuffer,R(MSG_TRANSFER_CAPTAIN_SELF));
		}
		else
		{
			rt2_sprintf(g_strStaticBuffer,R(MSG_TRANSFER_CAPTAIN_OTHER),mData.GetCaptainName());
		}
		PlayerFailOperate(g_strStaticBuffer);
		TeamDataChange();
	}
}

void CTeamWithUI::TeamDataChange()
{
    guard;
	
	if(mData.Size()==0)
	{
		mpUIListGroup->Hide();
		LOAD_UI("fmhp.lbcaptain")->Hide();
		LOAD_UI("fmhplist.fmhplist1.lbcaptain")->Hide();
		//LOAD_UI("fmhplistla")->Hide();// gao 2009.12.25 �˴�һ������ͼ��
		return;
	}


	//�����б���ĸ߶�
	static RtwRect TeamGroupRect=mpUIListGroup->GetFrameRect();
	static RtwRect OneMemberRect=mDataList[0].mpMemberUI->GetFrameRect();
	
	RtwRect NewRect;
	NewRect=mpUIListGroup->GetFrameRect();
	NewRect.bottom = NewRect.top + TeamGroupRect.getHeight()-(MAX_TEAM_MENBER_COUNT-1-mData.Size())*(OneMemberRect.getHeight());
	mpUIListGroup->MoveResize(NewRect);
	mpUIListGroup->Show();
	//LOAD_UI("fmhplistla")->Show();// gao 2009.12.25 �˴�һ������ͼ��
	
	int Index=0;
	for(int i=mData.Begin();i!=mData.End();i++)
	{
		if(mData[i].mDBID!=mSelfDBID)
		{
			mDataList[Index].mpMemberUI->Show();
			mDataList[Index].mpMemberNameUI->SetText(mData[i].mName.c_str());

			
			/*RtwImage *pHead = GcActor::GetHeadImage(mData[i].mHead);*/
			/*if ( pHead )
			{
				mDataList[i].mpMemberHeadUI->SetBackgroundImage(pHead);
			}*/
     /*	��	��	��	��	��	Ա	ͷ	��	��	��	add	by	��	��	��	2010 06	21	*/
			vector<SCharImage> images;RtwRect Rect;RtwImage *pHead = NULL;
			if (!g_TableCharImage.GetCharImage(mData[i].mActorID, images))
			{
				pHead = g_workspace.getImageFactory()->createImage("ui\\textures\\4.tga");
				if (pHead)
				{
					Rect = RtwRect(270,457,324,511);
					pHead->SetSourceRect(Rect);
					pHead->SetBlend(true);
					mDataList[Index].mpMemberHeadUI->SetBackgroundImage(pHead);
				}
			}
			else
			{
				std::string strImage = "ui_texture\\";
				strImage += images[mData[i].mHead].imageGame;
				strImage += ".dds";
				pHead = g_workspace.getImageFactory()->createImage(strImage);
				if(pHead)
				{
					pHead->SetBlend(true);
					mDataList[Index].mpMemberHeadUI->SetBackgroundImage(pHead);
				}
			}
			
			
	/**********************************************************************end*/

			mDataList[Index].mDBID=mData[i].mDBID;

			/*if (mData.GetCaptain() == mData[i].mDBID && mData.GetCaptain() != mSelfDBID )
				mDataList[Index].mpCaptainUI->Show();
			else
				mDataList[Index].mpCaptainUI->Hide();*/
			

			Index++;
		}
	}
	if (mData.GetCaptain() != mSelfDBID)
	{
		LOAD_UI("fmhp.lbcaptain")->Hide();
		LOAD_UI("fmhplist.fmhplist1.lbcaptain")->Show();
	}
	else
	{
		LOAD_UI("fmhp.lbcaptain")->Show();
		LOAD_UI("fmhplist.fmhplist1.lbcaptain")->Hide();
	}
	for(;Index!=MAX_TEAM_MENBER_COUNT-1;Index++)
	{
		
		mDataList[Index].mpMemberUI->Hide();
		mDataList[Index].mDBID=0;
	}
	
    unguard;
}

void CTeamWithUI::TeamMemberDataChange(DWORD vMemberDBID)
{
    guard;
	for(int i=0;i<MAX_TEAM_MENBER_COUNT-1;i++)
	{
		if(mDataList[i].mDBID==vMemberDBID)
		{//������ҵ���
			
			int Index=mData.Find(vMemberDBID);
			if(Index==mData.End())
			{
				return;
			}
			else
			{//�����ҵ�
				double HPvalue = (double)mData[Index].mHp/(double)mData[Index].mMaxHp;
				double MPvalue = (double)mData[Index].mMp/(double)mData[Index].mMaxMp;
				mDataList[i].mpMemberHpUI->SetValue(HPvalue);
				mDataList[i].mpMemberMpUI->SetValue(MPvalue);
				char lev[10];
				rt2_sprintf(lev,"%d",mData[Index].mLevel);
				mDataList[i].mpMemberLevelUI->SetText(lev);
			}

			break;
		}
	}
	return;
    unguard;
}



void CTeamWithUI::Message(const string& vrMessage)
{
	ShowSystemMessage(vrMessage.c_str(),true);
}

struct SUIFormData
{
	DWORD		mDBID;
	float		mTime;
	UIFormMsg*	mpUI;
	string		m_strMassage;
};

static list<SUIFormData> TeamConfirmList;

void CTeamWithUI::MessageWithChoice(DWORD vDBID,const std::string& vrMessage)
{
    guard;
	//����˷���Ϣ
	list<SUIFormData>::iterator It;

	for(It=TeamConfirmList.begin();It!=TeamConfirmList.end();It++)
	{
		if((*It).mDBID==vDBID)
		{
			return;
		}
	}



	UIFormMsg* pConfirm=UIFormMsg::ShowStatic(vrMessage.c_str(), UIFormMsg::TYPE_OK_CANCEL,true,"team");

	SUIFormData Temp;
	Temp.mDBID=vDBID;
	Temp.mTime=0.0;
	Temp.mpUI=pConfirm;

	LOG1("[add:%d]\n",pConfirm);

	TeamConfirmList.push_back(Temp);


	pConfirm->EvOK =
		RTW_CALLBACK_1(this,CTeamWithUI,MessageBoxOK,pConfirm);
	pConfirm->EvCancel =
		RTW_CALLBACK_1(this,CTeamWithUI,MessageBoxCancel,pConfirm);
    unguard;
}

void CTeamWithUI::Run(float fSecond)
{
    guard;
	list<SUIFormData>::iterator It;
	for(It=TeamConfirmList.begin();It!=TeamConfirmList.end();)
	{
		(*It).mTime+=fSecond;
		if((*It).mTime>=TEAM_INVITE_LIMIT_TIME)
		{
			(*It).mpUI->Hide();//hide
			RefuseOtherInvite((*It).mDBID);
			It=TeamConfirmList.erase(It);
		}
		else
		{
			It++;
		}
	}
    unguard;
}

void CTeamWithUI::MessageBoxOK(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
    guard;
	void*  pUI=const_cast<void*>(vpSender->GetUser1());
	LOG1("[ok:%d]\n",vpSender);
	list<SUIFormData>::iterator It;
	for(It=TeamConfirmList.begin();It!=TeamConfirmList.end();)
	{
		if((*It).mpUI==(UIFormMsg*)pUI)
		
		{
			AgreeOtherInvite((*It).mDBID);
			It=TeamConfirmList.erase(It);
		}
		else
		{
			It++;
		}
	}
    unguard;
}

void CTeamWithUI::MessageBoxCancel(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
    guard;
	void*  pUI=const_cast<void*>(vpSender->GetUser1());
	list<SUIFormData>::iterator It;
	for(It=TeamConfirmList.begin();It!=TeamConfirmList.end();)
	{
		if((*It).mpUI==(UIFormMsg*)pUI)
		{

			RefuseOtherInvite((*It).mDBID);
			It=TeamConfirmList.erase(It);
		}
		else
		{
			It++;
		}
	}
    unguard;
}

void CTeamWithUI::OnLClickMemberUI( ui::RtwWidget* vpSender, RtwEventDelegate* )
{
	for(int i=0;i<MAX_TEAM_MENBER_COUNT-1;i++)
	{
		if (mDataList[i].mpMemberUI == vpSender)
		{
			SendToServer(C2R_SELECTED_BY_DBID,mDataList[i].mDBID);
		}
	}
}


