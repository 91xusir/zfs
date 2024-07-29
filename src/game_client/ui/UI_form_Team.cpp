#include "gc_include.h"
#include "UI_form_Team.h"
#include "gc_team.h"
#include "ui_form_minimap.h"
#include "game_team_share_const.h"

CUI_form_Team::CUI_form_Team(void):
m_pTeam(NULL),
m_pBuff(NULL),
m_pLookInfo(NULL),
m_pAddFriend(NULL),
m_pKick(NULL),
m_pTransFer(NULL),
m_pDisband(NULL),
m_pRequest(NULL),
m_pOpenBuff(NULL),
m_pSelectRect(NULL),
m_SelectMember(-1)
{
	Init();
}

CUI_form_Team::~CUI_form_Team(void)
{
	map<string,ApplyIfo*> ::iterator it;
	for(it = m_mapApply.begin();it != m_mapApply.end();++it)
	{
		DEL_ONE((*it).second);// 删除前，先释放空间
	}
	m_mapApply.clear();
}
/* 客户端相关函数 */
void CUI_form_Team::Init()
{
	guard;
	//队伍成员界面
	m_pTeam								=	LOAD_UI("fmteam");
	m_pCloseForm						=	LOAD_UI_T(RtwButton,	 "fmteam.btnclose");
	m_pLookInfo							=	LOAD_UI_T(RtwButton,	 "fmteam.fmteaminfo.fmbtns.btn1");
	m_pAddFriend						=	LOAD_UI_T(RtwButton,	 "fmteam.fmteaminfo.fmbtns.btn2");
	m_pKick								=	LOAD_UI_T(RtwButton,	 "fmteam.fmteaminfo.fmbtns.btn3");
	m_pTransFer							=	LOAD_UI_T(RtwButton,	 "fmteam.fmteaminfo.fmbtns.btn4");
	m_pDisband							=	LOAD_UI_T(RtwButton,	 "fmteam.fmteaminfo.fmbtns.btn5");
	m_pRequest							=	LOAD_UI_T(RtwButton,	 "fmteam.fmteaminfo.fmbtns.btn6");
	m_pOpenBuff							=	LOAD_UI_T(RtwButton,	 "fmteam.fmteaminfo.btnaddition");
	m_pSelectRect						=	LOAD_UI_T(RtwLabel,	 "fmteam.fmteaminfo.fmmember.lbhighlight");
	m_pPickMode							=	LOAD_UI_T(RtwComboBox,"fmteam.fmteaminfo.fmbtns.fmdistribute.cbxlist");
	m_pPickModeHint						=	LOAD_UI_T(RtwLabel,	 "fmteam.fmteaminfo.fmbtns.fmdistribute.lbdistribute");

	m_pCloseForm->EvLClick				+=	RTW_CALLBACK(this, CUI_form_Team, OnCloseForm);
	m_pLookInfo->EvLClick				+=	RTW_CALLBACK(this, CUI_form_Team, OnBtnLookInfo);
	m_pAddFriend->EvLClick				+=	RTW_CALLBACK(this, CUI_form_Team, OnBtnAddFriend);
	m_pKick->EvLClick					+=	RTW_CALLBACK(this, CUI_form_Team, OnBtnKick);
	m_pTransFer->EvLClick				+=	RTW_CALLBACK(this, CUI_form_Team, OnBtnTransfer);	
	m_pDisband->EvLClick				+=	RTW_CALLBACK(this, CUI_form_Team, OnBtnDisband);
	m_pRequest->EvLClick				+=	RTW_CALLBACK(this, CUI_form_Team, OnBtnGetaway);
	m_pOpenBuff->EvLClick				+=	RTW_CALLBACK(this, CUI_form_Team, OnBtnShowBuff);
	m_pPickMode->EvUpdateText			+=	RTW_CALLBACK(this, CUI_form_Team, OnChangePickMode);
	m_pPickModeHint->EvHint				+=  RTW_CALLBACK(this, CUI_form_Team, OnGetPickModeHide);
	m_pPickModeHint->ModifyFlag(wfHint|wfGrabMouse,0);

	//组队BUFF界面
	m_pBuff								=	LOAD_UI("fmaddition");
	m_pBuffIntroduce					=	LOAD_UI_T(RtwButton,"fmaddition.fmadditioninfo.fmanimal.fmanimal.btnillustrate");
	m_pTeamBuff							=	LOAD_UI_T(RtwLabel,"fmaddition.fmadditioninfo.fmanimal.fmanimal.lbicon");
	m_pNearTeamMember					=	LOAD_UI_T(RtwLabel,"fmaddition.fmadditioninfo.fmexperience.fmexperience1.lb2");
	m_pEXPUp							=	LOAD_UI_T(RtwLabel,"fmaddition.fmadditioninfo.fmexperience.fmexperience1.lb4");
	m_pIntimatePlayer					=	LOAD_UI_T(RtwLabel,"fmaddition.fmadditioninfo.fmfriend.fmfriend1.lb2");
	m_pAttackUp							=	LOAD_UI_T(RtwLabel,"fmaddition.fmadditioninfo.fmfriend.fmfriend1.lb4");

	m_pBuffIntroduce->EvLClick			+=	RTW_CALLBACK(this, CUI_form_Team, OnBtnLookIntroduce);

	//附近玩家列表
	m_pNearPlayer						=	LOAD_UI("fmteamprep");
	m_pNearListBox						=	LOAD_UI_T(RtwListBox,"fmteamprep.fmprep.lbxmembers");
	m_pBtnNearClose						=	LOAD_UI_T(RtwButton,"fmteamprep.fmprep.btnclose");

	//请求列表
	m_pApply							=	LOAD_UI("fmteamrequest");
	m_pApplyListBox						=	LOAD_UI_T(RtwListBox,"fmteamrequest.fmprep.lbxmembers");
	m_pBtnAgree							=	LOAD_UI_T(RtwButton,"fmteamrequest.btnaccept");
	m_pBtnReject						=	LOAD_UI_T(RtwButton,"fmteamrequest.btnrefuse");
	m_pBtnApplyClose					=	LOAD_UI_T(RtwButton,"fmteamrequest.btnclose");
	m_pApplyListPopMenu					=	LOAD_UI_T(RtwPopupMenu,"RDMenu");

	m_pApplyListBox->EvRClickItem		+=	RTW_CALLBACK(this, CUI_form_Team,OnApplyListPopUpMenu);
	m_pApplyListBox->EvHintItem			+=	RTW_CALLBACK(this, CUI_form_Team,OnApplyListHint);
	m_pBtnAgree->EvLClick				+=	RTW_CALLBACK(this, CUI_form_Team,OnApplyListBtnAgree);
	m_pBtnReject->EvLClick				+=	RTW_CALLBACK(this, CUI_form_Team,OnApplyListBtnDisagree);
	m_pBtnApplyClose->EvLClick			+=	RTW_CALLBACK(this, CUI_form_Team,OnApplyListBtnClear);
	m_pApplyListPopMenu->EvMenuSelect	+=  RTW_CALLBACK(this, CUI_form_Team,OnApplyListRDMenuSelect);
	m_pApplyListPopMenu->EvUnFocus		+=  RTW_CALLBACK(this, CUI_form_Team,OnApplyListRDMenuUnFocus);
	LOAD_UI("lbrequest")->EvLClick	+=	RTW_CALLBACK(this, CUI_form_Team,OnBtnShowApply);

	RtwRect Rect;
	for(int i = 0; i < TEAM_MAX_MEMBER; ++i)
		TeamMembers[i] = RT_NEW TeamMember();
	char* name = "fmteam.fmteaminfo.fmmember.fmmember";
	for (int i = 0; i < TEAM_MAX_MEMBER; i++)
	{
		char form[128];rt2_snprintf(form,128,"%s%d%s",name,i+1,"");
		char head[128];rt2_snprintf(head,128,"%s%d%s",name,i+1,".lbicon");
		char anim[128];rt2_snprintf(anim,128,"%s%d%s",name,i+1,".lbanimal");
		char Name[128];rt2_snprintf(Name,128,"%s%d%s",name,i+1,".lbname");
		char leve[128];rt2_snprintf(leve,128,"%s%d%s",name,i+1,".lbgrade");
		char cpow[128];rt2_snprintf(cpow,128,"%s%d%s",name,i+1,".lbnumber");
		char prof[128];rt2_snprintf(prof,128,"%s%d%s",name,i+1,".lbunion");
		TeamMembers[i]->TeamMemberForm			=	LOAD_UI_T(RtwForm,form);
		TeamMembers[i]->TeamMemberHead			=	LOAD_UI_T(RtwLabel,head);
		TeamMembers[i]->TeamMemberAnimal		=	LOAD_UI_T(RtwLabel,anim);
		TeamMembers[i]->TeamMemberName			=	LOAD_UI_T(RtwLabel,Name);
		TeamMembers[i]->TeamMemberLevel			=	LOAD_UI_T(RtwLabel,leve);
		TeamMembers[i]->TeamMemberCombatPower	=	LOAD_UI_T(RtwLabel,cpow);
		TeamMembers[i]->TeamMemberProfession	=	LOAD_UI_T(RtwLabel,prof);
		TeamMembers[i]->IsShow					=	false;
		TeamMembers[i]->TeamMemberForm->ModifyFlags(wfInput | wfHint,0);
		TeamMembers[i]->TeamMemberForm->EvLClick	+=	RTW_CALLBACK(this, CUI_form_Team, OnSelectMember);
	}
	char* animalname = "fmaddition.fmadditioninfo.fmanimal.fmanimal.lb";
	for (int i = 0; i < 12; ++i)
	{
		char animal[128];rt2_snprintf(animal,128,"%s%d",animalname,i+(!i)*12);
		m_pAnimel[i] = LOAD_UI_T(RtwLabel,animal);
	}
	

	m_imgTeamNormal = g_workspace.getImageFactory()->createImage("ui\\textures\\1.tga");
	m_imgTeamNormal->SetSourceRect(RtwRect(357,355,399,397));
	m_imgTeamNormal->SetBlend(true);
	m_imgTeamAnimation = g_workspace.getImageFactory()->createImage("ui_texture/xinshou_10.gif");
	m_bShowAnimation = false;
	m_pTeamBuff->Hide();
	m_pPickMode->SetSelectedItem(0);
	unguard;
}

void CUI_form_Team::OnChangePickMode(CUiCheckButton* sender,void*)
{
	guard;
	int iselect = m_pPickMode->GetSelectedItem();
	if(iselect != -1)
	{
		GetWorld()->m_Team.SendToServer(C2R_SET_PICK_TYPE,iselect);
	}
	unguard;
}

void CUI_form_Team::Show()
{
	guard;
	if ( !GetWorld()->m_Team.mData.Size() )
	{
		PlayerFailOperate(R(MSG_TEAM_YOUDONNOTHAVEATEAM));
		return;
	}
	m_pTeam->Show();
	OnPreDraw();
	m_pSelectRect->Hide();
	SetTeamButtonNormal();
	unguard;

}

void CUI_form_Team::Hide()
{
	guard;
	m_pTeam->Hide();
	unguard;

}

void CUI_form_Team::OnRefresh()
{
	guard;
	if ( m_pTeam->IsVisible() )
	{
		if (!GetWorld()->m_Team.mData.Size())
		{
			m_pTeam->Hide();
			return;
		}
		m_pTeam->Show();
		OnPreDraw();
	}
	unguard;
}
/* 如果UI已经显示了，那么就让它刷新 */
void CUI_form_Team::OnPreDraw()
{
	guard;
	if(m_pTeam->IsVisible())
	{
		OnShowTeammember();
		if(GetWorld()->m_Team.mData.GetCaptain() == GetPlayer()->DBID())
		{
			m_pKick->Enable();
			m_pTransFer->Enable();
			m_pDisband->Enable();
			m_pRequest->Enable();
			m_pPickMode->Enable();
		}else
		{
			m_pKick->Disable();
			m_pTransFer->Disable();
			m_pDisband->Disable();
			m_pPickMode->Disable();
		}
	}
	unguard;
}

void CUI_form_Team::OnShowTeammember()//队伍成员 显示
{
	guard;
	static int Index = 0;
	STeamData* teamData = GetWorld()->m_Team.mData.GetTeamData();
	for(int i =0;i< GetWorld()->m_Team.mData.Size();i++)
	{
		RtwImage* ProfessionImage = NULL;
		RtwImage* HeadImage = NULL;
		RtwImage* AnimalImage = NULL;
		vector<SCharImage> images;
		if (teamData[i].mDBID == GetPlayer()->DBID())
		{
			char level[32];rt2_snprintf(level,32,"%d",GetPlayer()->m_core.Lev);
			char Powor[32];rt2_snprintf(Powor,32,"%d",GetPlayer()->m_core.WarScores);
			TeamMembers[i]->IsShow = true;
			TeamMembers[i]->TeamMemberLevel->SetText(level);
			TeamMembers[i]->TeamMemberName->SetText(teamData[i].mName);
			TeamMembers[i]->TeamMemberCombatPower->SetText(Powor);
			if (!GetTeamImage(GetPlayer()->m_core.actorID,GetPlayer()->m_core.HeadImageID,images,&HeadImage,GetPlayer()->m_core.Metier,&ProfessionImage,GetPlayer()->m_core.BornTag,&AnimalImage))
				return;
			TeamMembers[i]->TeamMemberProfession->SetBackgroundImage(ProfessionImage);
			TeamMembers[i]->TeamMemberHead->SetBackgroundImage(HeadImage);
			TeamMembers[i]->TeamMemberAnimal->SetBackgroundImage(AnimalImage);
			continue;
		}
		char level[32];rt2_snprintf(level,32,"%d",teamData[i].mLevel);
		char Powor[32];rt2_snprintf(Powor,32,"%d",teamData[i].mCombatPower);
		TeamMembers[i]->IsShow = true;
		TeamMembers[i]->TeamMemberLevel->SetText(level);
		TeamMembers[i]->TeamMemberName->SetText(teamData[i].mName);
		TeamMembers[i]->TeamMemberCombatPower->SetText(Powor);
		if(!GetTeamImage(teamData[i].mActorID,teamData[i].mHead,images,&HeadImage,teamData[i].mSchool,&ProfessionImage,teamData[i].mAnimal,&AnimalImage))
			return;	
		TeamMembers[i]->TeamMemberProfession->SetBackgroundImage(ProfessionImage);
		TeamMembers[i]->TeamMemberHead->SetBackgroundImage(HeadImage);
		TeamMembers[i]->TeamMemberAnimal->SetBackgroundImage(AnimalImage);
		
	}
	/*if (GetWorld()->m_Team.mData.GetCaptain() == GetPlayer()->DBID())
	{
		m_pPickMode->Enable();
	} 
	else
	{
		m_pPickMode->Disable();
	}*/
	for (int i = 0; i < TEAM_MAX_MEMBER; ++i)
	{
		if (TeamMembers[i]->IsShow)
			TeamMembers[i]->TeamMemberForm->Show();
		else
			TeamMembers[i]->TeamMemberForm->Hide();
		TeamMembers[i]->IsShow = false;
	}
	unguard;
}

void CUI_form_Team::OnSelectMember(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)//队员选中
{
	guard;
	if(!GetWorld()->m_Team.mData.Size())
	{
		m_pSelectRect->Hide();
		return;
	}
	if(!m_pSelectRect->IsVisible())
		m_pSelectRect->Show();
	for (int i =0;i< GetWorld()->m_Team.mData.Size();i++)
	{
		if (TeamMembers[i]->TeamMemberForm == vpSender)
		{
			m_SelectMember = i;
			break;
		}
	}
	if(!m_pSelectRect)
		return;
	if (TeamMembers[m_SelectMember]->TeamMemberForm->IsVisible())
	{
		RtwRect Rect = TeamMembers[m_SelectMember]->TeamMemberForm->getFrameRectRef();
		m_pSelectRect->MoveResize(Rect.left-4,Rect.top-4,Rect.right+4,Rect.bottom+4,true);
		m_pSelectRect->Show();
	}
	else
		m_pSelectRect->Hide();
	STeamData* tmpData = GetWorld()->m_Team.mData.GetTeamData();
	if (tmpData[m_SelectMember].mDBID == GetPlayer()->DBID())
	{
		m_pLookInfo->Disable();
		m_pKick->Disable();
		m_pAddFriend->Disable();
	}
	else
	{
		if (GetWorld()->m_Team.mData.GetCaptain() == GetPlayer()->DBID()) 
			m_pKick->Enable();
		else
			m_pKick->Disable();

		m_pLookInfo->Enable();
		m_pAddFriend->Enable();
	}
	unguard;
}

void CUI_form_Team::OnBtnShowBuff(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)//显示组队加成
{
	guard;
	if (!m_pBuff)//指针非空检查
		return;
	if (m_pBuff->IsVisible())//如果界面已显示
		m_pBuff->Hide();//隐藏界面     
	else
	{
		m_pBuff->Show();//否则显示界面
		OnShowAnimalBuff();
	}
	unguard;
}

void CUI_form_Team::OnBtnLookInfo(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)//密语
{
	guard;
	if(m_SelectMember == -1)
	{
		ShowSystemMessage(R(MSG_TEAM_SELECTONEPLAYER));
	}else
	{
		STeamData* tmpData = GetWorld()->m_Team.mData.GetTeamData();
		GetWorld()->m_Chat.OnMemberChoice(0, tmpData[m_SelectMember].mName.c_str());
	}
	unguard;
}

void CUI_form_Team::OnBtnAddFriend(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)//添加好友
{
	guard;
	if(m_SelectMember == -1)
	{
		ShowSystemMessage(R(MSG_TEAM_SELECTONEPLAYER));
	}else
	{
		STeamData* tmpData = GetWorld()->m_Team.mData.GetTeamData();
		GetWorld()->m_Friend.InviteOther(tmpData[m_SelectMember].mName);
	}
	unguard;
}

void CUI_form_Team::OnBtnKick(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)//队伍成员 驱逐队员
{
	guard;
	if(m_SelectMember == -1)
	{
		ShowSystemMessage(R(MSG_TEAM_SELECTONEPLAYER));
	}else
	{
		STeamData* tmpData = GetWorld()->m_Team.mData.GetTeamData();
		if(tmpData[m_SelectMember].mDBID == GetPlayer()->DBID())
		{
			ShowSystemMessage(R(MSG_TEAM_CANNOTKICKSELF));
			return;
		}
		GetWorld()->m_Team.SendToServer(1,tmpData[m_SelectMember].mDBID);
	}
	OnRefresh();
	unguard;
}

void CUI_form_Team::OnBtnTransfer(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)//转让队长
{
	guard;
	if(m_SelectMember == -1)
	{
		ShowSystemMessage(R(MSG_TEAM_SELECTONEPLAYER));
		return;
	}
	STeamData* tmpData = GetWorld()->m_Team.mData.GetTeamData();
	if(tmpData[m_SelectMember].mDBID == GetPlayer()->DBID())
	{
		ShowSystemMessage(R(MSG_TEAM_CANNOTCHANGELEADER));	
		return;
	}
	if (GetWorld()->m_Team.mData.GetCaptain() == GetPlayer()->DBID())
	{
		GetWorld()->m_Team.SendToServer(C2R_TRANSFER_CAPTAIN,tmpData[m_SelectMember].mDBID);
		OnRefresh();
		m_pKick->Disable();
		m_pTransFer->Disable();
		m_pDisband->Disable();
	}
	else
		return;
	unguard;
}

void CUI_form_Team::OnBtnDisband(ui::RtwWidget *vpSender, ui::RtwEventDelegate *vpEvent)//解散队伍
{
	guard;
	GetWorld()->m_Team.Dismiss();
	m_pTeam->Hide();
	unguard;
}

void CUI_form_Team::OnBtnGetaway(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)//离开队伍
{
	guard;
	GetWorld()->m_Team.SelfLeave();
	m_pTeam->Hide();
	unguard;
}

void CUI_form_Team::OnBtnLookIntroduce(ui::RtwWidget *vpSender, ui::RtwEventDelegate *vpEvent)//组队BUFF说明
{
	guard;
	unguard;
}

void CUI_form_Team::OnCloseForm(void* ,void*)//关闭界面
{
	guard;
	m_pTeam->Hide();
	m_pBuff->Hide();
	unguard;
}
/* 消息相关函数 */
//void CUI_form_Team::OnReceiveApply()
//{
//	//OnInsertNew(1,"丁三",10,"无职业","");
//}

void CUI_form_Team::SetTeamButtonNormal()
{
	guard;
	if(m_bShowAnimation)
	{
		LOAD_UI_T(RtwButton,"fmsystem.btnteam")->SetNormalImage(m_imgTeamNormal);
		m_bShowAnimation = false;
	}
	unguard;
}

void CUI_form_Team::SetTeamButtonAnimation()
{
	guard;
	if(!m_bShowAnimation && !IsVisibale())
	{
		LOAD_UI_T(RtwButton,"fmsystem.btnteam")->SetNormalImage(m_imgTeamAnimation);
		m_bShowAnimation = true;
	}
	unguard;
}

void CUI_form_Team::OnBtnDownRefresh(ui::RtwWidget *vpSender, ui::RtwEventDelegate *vpEvent)
{
	guard;
	OnRefresh();
	unguard;
}

bool CUI_form_Team::GetTeamImage(short ModelID, int HeadImageID,vector<SCharImage>& models,RtwImage** HeadImage,short Profession,RtwImage** ProfessionImage,short Animal,RtwImage** AnimalImage)//加载职业图片 李泽华
{
	guard;
	RtwRect Rect;
	if (!g_TableCharImage.GetCharImage(ModelID, models))
	{
		(*HeadImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\4.tga");
		Rect = RtwRect(270,457,324,511);
		(*HeadImage)->SetSourceRect(Rect);
	}
	else
	{
		std::string strImage = "ui_texture\\";
		strImage += models[HeadImageID].imageGame;
		strImage += ".dds";
		(*HeadImage) = g_workspace.getImageFactory()->createImage(strImage);
	}
	if(!*HeadImage)
		return false;
	(*HeadImage)->SetBlend(true);

	(*ProfessionImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\4.tga");
	if (!*ProfessionImage)
		return false;
	
	switch (Profession)
	{
	case 1:
		Rect = RtwRect(199,384,239,435);//五台
		(*ProfessionImage)->SetSourceRect(Rect);
		break;
	case 2:
		Rect = RtwRect(246,384,286,435);//花间
		(*ProfessionImage)->SetSourceRect(Rect);
		break;
	case 3:
		Rect = RtwRect(103,384,143,435);//蜀山
		(*ProfessionImage)->SetSourceRect(Rect);
		break;
	case 4:
		Rect = RtwRect(151,384,191,435);//苗疆
		(*ProfessionImage)->SetSourceRect(Rect);
		break;
	default:
		Rect = RtwRect(165,463,210,511);
		(*ProfessionImage)->SetSourceRect(Rect);
		break;
	}
	(*ProfessionImage)->SetBlend(true);

	switch(Animal)
	{
	case 0:
		(*AnimalImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\animal1.tga");
		break;
	case 1:
		(*AnimalImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\animal2.tga");
		break;
	case 2:
		(*AnimalImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\animal3.tga");
		break;
	case 3:
		(*AnimalImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\animal4.tga");
		break;
	case 4:
		(*AnimalImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\animal5.tga");
		break;
	case 5:
		(*AnimalImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\animal6.tga");
		break;
	case 6:
		(*AnimalImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\animal7.tga");
		break;
	case 7:
		(*AnimalImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\animal8.tga");
		break;
	case 8:
		(*AnimalImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\animal9.tga");
		break;
	case 9:
		(*AnimalImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\animal10.tga");
		break;
	case 10:
		(*AnimalImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\animal11.tga");
		break;
	case 11:
		(*AnimalImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\animal12.tga");
		break;
	default:
		(*AnimalImage) = g_workspace.getImageFactory()->createImage("ui\\textures\\4.tga");
		Rect = RtwRect(215,463,264,511 );
		(*AnimalImage)->SetSourceRect(Rect);
		break;
	}
	if(!(*AnimalImage))
		return false;
	(*AnimalImage)->SetBlend(true);
	return true;
	unguard;
}

void CUI_form_Team::OnApplyListInsertNew(long id, std::string na, int lev, std::string prf, int ani, int pow)//申请列表 添加新的
{
	guard;
	static int Index = 0;

	map<string,ApplyIfo*> ::iterator it;
	it = m_mapApply.find(na);
	if(it != m_mapApply.end())
		return;

	char tmpStr[30];
	time_t t = time(NULL);
	tm* Time = localtime(&t);
	rt2_sprintf(tmpStr, "时间：%04d.%02d.%02d %02d:%02d", Time->tm_year+1900, Time->tm_mon+1, Time->tm_mday,Time->tm_hour,Time->tm_min);
	ApplyIfo* tmp = RT_NEW ApplyIfo(id,na,lev,prf,ani,pow,tmpStr);

	m_pApplyListBox->AddItem(0);
	m_pApplyListBox->SetItemText(Index,0,tmp->name);
	memset(tmpStr,0,30);
	rt2_sprintf(tmpStr,"%d",tmp->level);
	m_pApplyListBox->SetItemText(Index,1,tmpStr);
	m_pApplyListBox->SetItemText(Index,2,tmp->profession);
	switch(tmp->animal)
	{
	case 0:
		m_pApplyListBox->SetItemText(Index,3,"鼠");
		break;
	case 1:
		m_pApplyListBox->SetItemText(Index,3,"牛");
		break;
	case 2:
		m_pApplyListBox->SetItemText(Index,3,"虎");
		break;
	case 3:
		m_pApplyListBox->SetItemText(Index,3,"兔");
		break;
	case 4:
		m_pApplyListBox->SetItemText(Index,3,"龙");
		break;
	case 5:
		m_pApplyListBox->SetItemText(Index,3,"蛇");
		break;
	case 6:
		m_pApplyListBox->SetItemText(Index,3,"马");
		break;
	case 7:
		m_pApplyListBox->SetItemText(Index,3,"羊");
		break;
	case 8:
		m_pApplyListBox->SetItemText(Index,3,"猴");
		break;
	case 9:
		m_pApplyListBox->SetItemText(Index,3,"鸡");
		break;
	case 10:
		m_pApplyListBox->SetItemText(Index,3,"狗");
		break;
	case 11:
		m_pApplyListBox->SetItemText(Index,3,"猪");
		break;
	default:
		m_pApplyListBox->SetItemText(Index,3,"无");
		break;
	}
	memset(tmpStr,0,30);
	rt2_sprintf(tmpStr,"%d",tmp->power);
	m_pApplyListBox->SetItemText(Index,4,tmpStr);
	m_mapApply.insert(make_pair(string(tmp->name), tmp));
	unguard;
}

void CUI_form_Team::OnApplyListHint(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)// 申请列表 提示
{
	guard;
	int line = vpEvent->menuSelect.index;
	if(line == -1)
		return;
	map<string,ApplyIfo*> ::iterator it;
	it = m_mapApply.find(m_pApplyListBox->GetItemText(line,0));
	g_workspace.SetHintText(((*it).second)->time);
	g_workspace.AutoHintSize();
	g_workspace.SetHintPosition(g_workspace.GetMousePosX(),g_workspace.GetMousePosY());
	unguard;

}

void CUI_form_Team::OnApplyListBtnAgree(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)//请求列表 同意
{
	guard;
	int iSelectItem = m_pApplyListBox->GetSelectedItem();
	if(iSelectItem == -1)
	{
		ShowSystemMessage(R(MSG_TEAM_SELECTPERSONFIRST));
	}else
	{
		STeamData* teamData = GetWorld()->m_Team.mData.GetTeamData();
		for(int i =0;i< GetWorld()->m_Team.mData.Size();i++)
		{
			if(!strcmp(teamData[i].mName.c_str(),(m_pApplyListBox->GetItemText(iSelectItem,0).c_str())))
				return;
		}
		string tmpShow = "你选择了" + m_pApplyListBox->GetItemText(iSelectItem,0);
		ShowSystemMessage(tmpShow.c_str());

		/* map的删除操作，虽然键应该不存在重复的，但是，我们这里仍然按全部删除来进行操作 */
		map<string,ApplyIfo*> ::iterator it;
		it = m_mapApply.find(m_pApplyListBox->GetItemText(iSelectItem,0));
		if(it != m_mapApply.end())
		{
			GetWorld()->m_Team.AgreeOtherApply((*it).second->ID);
			DEL_ONE((*it).second);// 删除前，先释放空间
			m_mapApply.erase(it);

			// 删除操作，包括从list删除，以及multimap中删除
			m_pApplyListBox->RemoveLine(iSelectItem,true);
			if (m_mapApply.empty())
			{
				m_pApply->Hide();
				LOAD_UI("lbrequest")->Hide();
			}
		}
		/* end */
	}
	unguard;
}

void CUI_form_Team::OnApplyListBtnDisagree(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)//请求列表 拒绝
{
	guard;
	int iSelectItem = m_pApplyListBox->GetSelectedItem();
	if(iSelectItem == -1)
	{
		ShowSystemMessage(R(MSG_TEAM_SELECTPERSONFIRST));
	}else
	{
		STeamData* teamData = GetWorld()->m_Team.mData.GetTeamData();
		for(int i =0;i< GetWorld()->m_Team.mData.Size();i++)
		{
			if(!strcmp(teamData[i].mName.c_str(),(m_pApplyListBox->GetItemText(iSelectItem,0).c_str())))
				return;
		}
		char tmpShow[1024];
		rt2_sprintf(tmpShow,R(MSG_TEAM_MASTERREFUSEJOIN),m_pApplyListBox->GetItemText(iSelectItem,0));

		ShowSystemMessage(tmpShow);

		/* map的删除操作，虽然键应该不存在重复的，但是，我们这里仍然按全部删除来进行操作 */
		map<string,ApplyIfo*> ::iterator it;
		it = m_mapApply.find(m_pApplyListBox->GetItemText(iSelectItem,0));
		if(it != m_mapApply.end())
		{
			GetWorld()->m_Team.RefuseOtherApply((*it).second->ID);
			DEL_ONE((*it).second);// 删除前，先释放空间
			m_mapApply.erase(it);

			// 删除操作，包括从list删除，以及multimap中删除
			m_pApplyListBox->RemoveLine(iSelectItem,true);
			if (m_mapApply.empty())
			{
				m_pApply->Hide();
				LOAD_UI("lbrequest")->Hide();
			}
		}
		/* end */
	}
	unguard;

}
void CUI_form_Team::OnApplyListBtnClear(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)//请求列表 关闭
{
	guard;
	map<string,ApplyIfo*>::iterator it;
	for(it = m_mapApply.begin();it != m_mapApply.end();++it)
	{
		DEL_ONE((*it).second);// 删除前，先释放空间
	}
	m_mapApply.clear();
	m_pApplyListBox->RemoveAllLines(true);
	m_pApply->Hide();
	unguard;
}

void CUI_form_Team::OnApplyListRDMenuSelect(RtwWidget* vpSender, RtwEventDelegate* vpEvent)//申请列表 右键菜单选中
{
	guard;
	int iSelectItem = ((RtwPopupMenu*)vpSender)->GetLastSelectItem();
	if (iSelectItem>=0)
	{
		switch(iSelectItem)
		{
		case 0:
			GetWorld()->m_Chat.OnMemberChoice(0, m_pApplyListBox->GetItemText(m_pApplyListBox->GetSelectedItem(),0).c_str());
			break;
		case 1:
			STeamData* teamData = GetWorld()->m_Team.mData.GetTeamData();
			//STeamData* tmpData = GetWorld()->m_Team.mData.GetTeamData();
			GetWorld()->m_Friend.InviteOther(teamData[m_SelectMember].mName);
			break;
		}
	}
	unguard;

}
void CUI_form_Team::OnApplyListPopUpMenu(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)//右键菜单 
{
	guard;
	// 激活菜单
		if (m_pApplyListPopMenu)
		{
			m_pApplyListPopMenu->RemoveAllItems();
			m_pApplyListPopMenu->AddItem(("密    语"), 0, (void*)1);
			m_pApplyListPopMenu->AddItem(("添加好友"), 0, (void*)2);
			g_workspace.ShowPopupMenu(m_pApplyListPopMenu, NULL, g_workspace.GetMousePosX(), g_workspace.GetMousePosY());
		}
	unguard;

}
void CUI_form_Team::OnApplyListRDMenuUnFocus(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)//右键菜单隐藏
{
	guard;
	m_pApplyListPopMenu->Hide();
	unguard;
}

void CUI_form_Team::OnBtnShowApply(ui::RtwWidget *vpSender, ui::RtwEventDelegate *vpEvent)
{
	guard;
	if (LOAD_UI("lbrequest")->IsVisible())
	{
		if (GetWorld()->m_Team.mData.GetCaptain() == GetPlayer()->DBID())
		{
			if (!m_pApply->IsVisible())
			{
				m_pApply->Show();
			}
			
		}
	}
	unguard;
}

void CUI_form_Team::OnShowAnimalBuff()//组队加成显示
{
	guard;
	if ( !GetWorld()->m_Team.mData.Size() )
	{
		PlayerFailOperate(R(MSG_TEAM_YOUDONNOTHAVEATEAM));
		return;
	}
	else
	{
		m_pTeamBuff->Hide();
		m_pBuffIntroduce->Disable();
		for (int i = 0;i < 12;++i)
		{
			RtwImage* strImage = NULL;RtwRect Rect;
			strImage = g_workspace.getImageFactory()->createImage("ui\\textures\\26.tga");
			if (strImage)
			{
				strImage->SetBlend(true);
				m_pAnimel[i]->SetBackgroundImage(strImage);
				Rect = RtwRect(342,1+42*i,384,43+42*i);
				strImage->SetSourceRect(Rect);
				m_pAnimel[i]->Show();
			}
		}
	}
	STeamData* teamData = GetWorld()->m_Team.mData.GetTeamData();
	int NearMemberCount = 0;
	int MemberCount = GetWorld()->m_Team.mData.Size();
	for (int i = 0; i < MemberCount; ++i)
	{
		if (teamData[i].mDBID == GetPlayer()->DBID())
			continue;
		float fX,fY,fx,fy;
		const float* pPos = &(GetWorld()->m_pPlayer->GetMatrix()->_30);
		fX = pPos[0];fx = teamData[i].X;
		fY = pPos[1];fy = teamData[i].Y;
		int iRange = sqrt(float((fX-fx)*(fX-fx)+(fY-fy)*(fY-fy)));
		if ( 0 <= iRange && iRange <= 800)
			++NearMemberCount;
	}
    int ExpUp = (0.2 + 0.02 * MemberCount)*100;
	char exp[32];rt2_snprintf(exp,32,"%d%s",ExpUp,"%");
	char num[32];rt2_snprintf(num,32,"%d",NearMemberCount);
	m_pNearTeamMember->SetText(num);
	m_pEXPUp->SetText(exp);
	m_pIntimatePlayer->SetText("");
	m_pAttackUp->SetText("");
	unguard;
}

void CUI_form_Team::OnGetPickModeHide(ui::CUiCheckButton *sender, void *)
{
	guard;
	ReadHtml(R(MSG_TEAM_HINT_DISTRIBUTION));
	unguard;
}