//********************************************************************
//	created:	2010.05.31
//	filename: 	ui_form_union.cpp
//	author:		ldr123
//	purpose:	游戏中,帮派系统重做
//  other:		version 1.0
//********************************************************************

#include "gc_include.h"
#include "union_base.h"
#include "net_command_union.h"
#include "ui_form_msg.h"
#include "ui_form_textmsg.h"
#include "ui_form_union.h"
#include "ui_form_char.h"

static const DWORD OnlineColor		= 0xffffffff;
static const DWORD OfflineColor		= 0xffafafaf;
static const DWORD OwnColor			= 0xff00ff00;
static const DWORD CaptionColor		= 0xffffffff;

#define CLEANTEXT(X) {X->SetText("");}

RtString GetUOString(EUnionOfficial unionOfficial)
{
	//获取职业对应的字符串
	guard;

	switch (unionOfficial)
	{
	case UO_King:
		// 		return R(MSG_UNION_KING);
		return "帮主";
	case UO_safeguardLeft:
		// 		return R(MSG_UNION_KING);
		return "左护法";
	case UO_safeguardRight:
		return "右护法";
		// 		return R(MSG_UNION_KING);
	case UO_Elder:
		// 		return R(MSG_UNION_KING);
		return "长老";
	case UO_SubLeader:
		return "堂主";
		// 		return R(MSG_UNION_KING);
	default:
		return "帮众";
		// 		return R(MSG_UNION_KING);
	}
	unguard;
}

CUnionIconMgr g_UnionIconMgr;

int nSortFlag = 0;
bool bSortLower[4] = {true, true, true, true};
RtwImage *pSortImage[8];
RtwRect sortRc[8] = {
						RtwRect(78, 492, 111, 512),RtwRect(190, 492, 223, 512),RtwRect(78, 492, 111, 512),RtwRect(190, 492, 223, 512),
						RtwRect(66, 492, 111, 512),RtwRect(178, 492, 223, 512),RtwRect(27, 492, 111, 512),RtwRect(149, 492, 222, 512)
					};

static bool SortFunc(const SUnionCitizen& p1, const SUnionCitizen& p2);

//根据用户id查询用户句柄
SUnionCitizen* UIFormUnion::Find(DWORD playerID)
{
	guard;

	if (playerID == 0)
	{
		return NULL;
	}

	int nSize = (int)m_unionData.vecUnionCitizen.size();
	for (int i=0; i<nSize; ++i)
	{
		if ( m_unionData.vecUnionCitizen[i].m_nPlayerID == playerID)
		{
			return (SUnionCitizen*)(&(m_unionData.vecUnionCitizen[i]));
		}
	}

	return NULL;

	unguard;
}

UIFormUnion::UIFormUnion()
{
	guard;

	//主界面
	m_pFrmFaction	= LOAD_UI("fmfaction");	//帮派管理界面

	m_pLblIcon			= LOAD_UI_T(RtwLabel,		"fmfaction.tabfaction.fmfactioninfo.fmfactioninfo1.lbicon");			//帮派图腾
	m_pLblFacName		= LOAD_UI_T(RtwLabel,		"fmfaction.tabfaction.fmfactioninfo.fmfactioninfo1.lbnum1");			//帮派名字
	m_pLblStrength		= LOAD_UI_T(RtwLabel,		"fmfaction.tabfaction.fmfactioninfo.fmfactioninfo1.lbnum2");			//帮派势力
	m_pLblFacMoney		= LOAD_UI_T(RtwLabel,		"fmfaction.tabfaction.fmfactioninfo.fmfactioninfo1.lbnum3");			//帮派资金
	m_pLblFacRank		= LOAD_UI_T(RtwLabel,		"fmfaction.tabfaction.fmfactioninfo.fmfactioninfo1.lbnum4");			//帮派排名
	m_pLblPeopleNum		= LOAD_UI_T(RtwLabel,		"fmfaction.tabfaction.fmfactioninfo.fmfactioninfo1.lbnum5");			//帮派人数
	m_pBtnPost			= LOAD_UI_T(RtwButton,		"fmfaction.tabfaction.fmfactioninfo.fmfactioninfo2.btnpost");			//发布公告
	m_pChatPost			= LOAD_UI_T(RtwTextBox,		"fmfaction.tabfaction.fmfactioninfo.fmfactioninfo2.edtpost");			//公告显示
	m_pLstMember		= LOAD_UI_T(RtwListBox,		"fmfaction.tabfaction.fmfactioninfo.tabfaction.fmmember.lstmember");	//成员列表
	m_pLstHistory		= LOAD_UI_T(RtwChatBox,		"fmfaction.tabfaction.fmfactioninfo.tabfaction.fmhistory.edthistory");	//历史记录
	// 	m_pBtnPrew			= LOAD_UI_T(RtwButton,		"fmfaction.tabfaction.fmfactioninfo.tabfaction.fmchoose.btnleft");		//前一区域
	// 	m_pBtnNext			= LOAD_UI_T(RtwButton,		"fmfaction.tabfaction.fmfactioninfo.tabfaction.fmchoose.btnright");		//后一区域
	// 	m_pTxtArea			= LOAD_UI_T(RtwTextBox,		"fmfaction.tabfaction.fmfactioninfo.tabfaction.fmchoose.edtchoose");	//显示的区域
	m_pBtnFunction		= LOAD_UI_T(RtwButton,		"fmfaction.tabfaction.fmfactioninfo.fmbtns.btnfunction");				//功能
	m_pBtnExit			= LOAD_UI_T(RtwButton,		"fmfaction.tabfaction.fmfactioninfo.fmbtns.btnclose");					//退出
	m_pBtnContribution	= LOAD_UI_T(RtwButton,		"fmfaction.tabfaction.fmfactioninfo.fmbtns.btnresources");				//贡献资源
	m_pChkOffLineUser	= LOAD_UI_T(CUiCheckButton,	"fmfaction.tabfaction.fmfactioninfo.tabfaction.btnchoose");				//离线用户
	m_pFuncMenu			= LOAD_UI_T(RtwPopupMenu,	"menufunction");														//功能按钮的菜单
	m_pUserList			= LOAD_UI_T(RtwPopupMenu,	"menurightkey");														//右击用户列表的菜单

	m_pBtnPost->Disable();
	CLEANTEXT(m_pLblIcon);
	CLEANTEXT(m_pLblFacName);
	CLEANTEXT(m_pLblStrength);
	CLEANTEXT(m_pLblFacMoney);
	CLEANTEXT(m_pLblFacRank);
	CLEANTEXT(m_pLblPeopleNum);
	m_pLblFacRank->SetText("无排名");
	m_pLstHistory->SetEnableInput(false);
	m_pLblFacMoney->SetText("0");
	m_pLstMember->SetDrawScrollBarV(true);

	m_pChatPost->SetAutoScrollV(true);
	m_pChatPost->SetScrollBarV(true);
	m_pChatPost->SetMultiLine(true);
	m_pChatPost->SetWordWrap(true);


	RtwImage *pImage = g_workspace.getImageFactory()->createImage("ui\\x_textures\\Totem\\Totem_default.tga");
	if (pImage)
	{
		pImage->SetBlend(true);
		m_pLblIcon->SetBackgroundImage(pImage);
	}

	m_pLstMember->SetAutoScrollV(true);

	m_pLstHistory->SetScrollBarV(true);
	m_pLstHistory->SetAutoScrollV(true);
	m_pLstHistory->SetMultiLine(true);
	m_pLstHistory->SetWordWrap(true);
	m_pLstHistory->SetNameColorChange(false);

	// 	m_pBtnPost->Disable();
	// 	m_pBtnContribution->Disable();
	// 	m_pBtnPrew->Hide();
	// 	m_pBtnNext->Hide();
	// 	m_pTxtArea->Hide();


	// 	m_pTxtArea->SetEnableInput(false);
	m_pChatPost->SetEnableInput(false);
	m_pChkOffLineUser->SetSelected(false);

	m_pChkOffLineUser->EvLClick		+= RTW_CALLBACK(this, UIFormUnion, OnCheckOffline);
	m_pBtnPost->EvLClick			+= RTW_CALLBACK(this, UIFormUnion, OnClickPost);
	// 	m_pBtnPrew->EvLClick			+= RTW_CALLBACK(this, UIFormUnion, OnClickPrewBtn);
	// 	m_pBtnNext->EvLClick			+= RTW_CALLBACK(this, UIFormUnion, OnClickNextBtn);
	m_pBtnExit->EvLClick			+= RTW_CALLBACK(this, UIFormUnion, OnClickExit);
	m_pBtnContribution->EvLClick	+= RTW_CALLBACK(this, UIFormUnion, OnClickContribution);
	m_pBtnFunction->EvLClick		+= RTW_CALLBACK(this, UIFormUnion, OnClickFunction);
	m_pLstMember->EvRClickItem		+= RTW_CALLBACK(this, UIFormUnion, OnRClickMember);
	m_pFuncMenu->EvMenuSelect		+= RTW_CALLBACK(this, UIFormUnion, OnFuncPopMenuSelect);
	m_pFuncMenu->EvUnFocus			+= RTW_CALLBACK(this, UIFormUnion, OnFuncPopMenuLoseFocus);
	m_pUserList->EvMenuSelect		+= RTW_CALLBACK(this, UIFormUnion, OnUserListPopMenuSelect);
	m_pUserList->EvUnFocus			+= RTW_CALLBACK(this, UIFormUnion, OnUserListPopMenuLoseFocus);

	int i = 0;

	for (i=0; i<8; ++i)
	{
		pSortImage[i] = g_workspace.getImageFactory()->createImage("ui\\x_textures\\2.tga");
		pSortImage[i]->SetSourceRect(sortRc[i]);
		pSortImage[i]->SetBlend(true);
	}

	m_pLstMember->GetColumnHeader(1)->SetTextAlignment(SUiTextAlignment(alignNear, alignCenter));
	m_pLstMember->GetColumnHeader(2)->SetTextAlignment(SUiTextAlignment(alignNear, alignCenter));
	m_pLstMember->GetColumnHeader(3)->SetTextAlignment(SUiTextAlignment(alignNear, alignCenter));

	for (i=1; i<=4; i++)
	{
		m_pLstMember->GetColumnHeader(i)->EvLClick += RTW_CALLBACK(this, UIFormUnion, OnHeaderClick);
		m_pLstMember->GetColumnHeader(i)->SetNormalImage(pSortImage[(i-1)*2]);
	}


	//禁用暂未开放的功能
	LOAD_UI("fmfaction.tabfaction.btnalliance")->Disable();
	LOAD_UI("fmfaction.tabfaction.btnterritory")->Disable();
	//end

	//调整官职
	m_pFmAppoint			= LOAD_UI("fmchangeposition");
	m_pBtnChangeOk			= LOAD_UI_T(RtwButton,	"fmchangeposition.btnok");
	m_pBtnChangeCancel		= LOAD_UI_T(RtwButton,	"fmchangeposition.btncancel");
	m_pCbxChangeText		= LOAD_UI_T(RtwComboBox,	"fmchangeposition.fmchangeposition1.cbxlist");
	m_pLblChangeTextTips	= LOAD_UI_T(RtwLabel,	"fmchangeposition.fmchangeposition1.lbtext");

	m_pBtnChangeOk->EvLClick		+= RTW_CALLBACK(this, UIFormUnion, OnClickChangeOk);
	m_pBtnChangeCancel->EvLClick	+= RTW_CALLBACK(this, UIFormUnion, OnClickChangeCancel);
	//end

	//添加帮众
	m_pFmAdd		= LOAD_UI("fminvite");;
	m_pBtnAddOk		= LOAD_UI_T(RtwButton,	"fminvite.btninvite");	
	m_pBtnAddCancel	= LOAD_UI_T(RtwButton,	"fminvite.btnclose");
	m_ptxtAddText	= LOAD_UI_T(RtwTextBox,	"fminvite.edtinvite");

	m_pBtnAddOk->EvLClick		+= RTW_CALLBACK(this, UIFormUnion, OnClickAddOk);
	m_pBtnAddCancel->EvLClick	+= RTW_CALLBACK(this, UIFormUnion, OnClickAddCancel);
	//end

	//脱离帮派
	m_pFmLeave			= LOAD_UI("fmleave");
	m_pBtnLeaveOk		= LOAD_UI_T(RtwButton,	"fmleave.btnok");	
	m_pBtnLeaveCancel	= LOAD_UI_T(RtwButton,	"fmleave.btncancel");

	m_pBtnLeaveOk->EvLClick		+= RTW_CALLBACK(this, UIFormUnion, OnClickLeaveOk);
	m_pBtnLeaveCancel->EvLClick	+= RTW_CALLBACK(this, UIFormUnion, OnClickLeaveCancel);
	//end

	//自我介绍
	m_pFmSelfIntro			= LOAD_UI("fmself-introduction");;
	m_pBtnSelfIntroOk		= LOAD_UI_T(RtwButton,	"fmself-introduction.btnok");	
	m_pBtnSelfIntroCancel	= LOAD_UI_T(RtwButton,	"fmself-introduction.btnclose");
	m_pTxtSelfIntro			= LOAD_UI_T(RtwTextBox,	"fmself-introduction.fmtext.edttext");

	m_pTxtSelfIntro->SetEnableInput(false);

	m_pBtnSelfIntroOk->EvLClick		+= RTW_CALLBACK(this, UIFormUnion, OnClickSelfCancel);
	m_pBtnSelfIntroCancel->EvLClick	+= RTW_CALLBACK(this, UIFormUnion, OnClickSelfCancel);
	//end

	//公告发布
	m_pFmBulletin			= LOAD_UI("fmpost");
	m_pBtnBulletinOk		= LOAD_UI_T(RtwButton,	"fmpost.btnpost");	
	m_pBtnBulletinCancel	= LOAD_UI_T(RtwButton,	"fmpost.btnclose");
	m_pTxtBulletin			= LOAD_UI_T(RtwTextBox,	"fmpost.edtpost");

	m_pTxtBulletin->SetWordWrap(true);
	m_pTxtBulletin->SetCapacity(400);
	m_pTxtBulletin->SetMultiLine(true);

	m_pTxtBulletin->SetAutoScrollV(true);
	m_pTxtBulletin->SetScrollBarV(true);

	m_pBtnBulletinOk->EvLClick		+= RTW_CALLBACK(this, UIFormUnion, OnClickBulletiOk);
	m_pBtnBulletinCancel->EvLClick	+= RTW_CALLBACK(this, UIFormUnion, OnClickBulletinCancel);
	//end

	//查看权限
	m_pFmViewPower			= LOAD_UI("fmseepower");
	m_pBtnViewPowerOk		= LOAD_UI_T(RtwButton,	"fmseepower.btnok");	
	m_pBtnViewPowerCancel	= LOAD_UI_T(RtwButton,	"fmseepower.btndefault");

	m_pBtnViewPowerOk->EvLClick		+= RTW_CALLBACK(this, UIFormUnion, OnClickLViewPowerCancel);
	m_pBtnViewPowerCancel->EvLClick	+= RTW_CALLBACK(this, UIFormUnion, OnClickLViewPowerCancel);
	//end

	//贡献资源
	m_pFmMoney			= LOAD_UI("fmresources");
	m_pTxtMoney			= LOAD_UI_T(RtwTextBox,	"fmresources.fmfactionmoney.fmmoney.edtmoney");
	m_pBtnMoneyOk		= LOAD_UI_T(RtwButton,	"fmresources.btnreward");	
	m_pBtnMoneyCancel	= LOAD_UI_T(RtwButton,	"fmresources.btnclose");

	m_pTxtMoney->SetNumberOnly(true);

	m_pBtnMoneyOk->EvLClick		+= RTW_CALLBACK(this, UIFormUnion, OnLClickMoneyOK);
	m_pBtnMoneyCancel->EvLClick	+= RTW_CALLBACK(this, UIFormUnion, OnLClickMoneyCancel);
	m_pTxtMoney->EvInputChar	+= RTW_CALLBACK(this, UIFormUnion, OnInputChar);

	LOAD_UI("fmresources.fmcontribution")->Disable();
	//end

	//帮派图腾
	nSelectTotemID	= 0;
	m_pFmTotem			= LOAD_UI("fmfactionicon");
	m_pBtnTotemUpLoad	= LOAD_UI_T(RtwButton, "fmfactionicon.lbupload");
	m_pBtnTotemOK		= LOAD_UI_T(RtwButton, "fmfactionicon.btnok");
	m_pBtnTotemCancel	= LOAD_UI_T(RtwButton, "fmfactionicon.btnclose");
	m_pBtnTotemPic		= LOAD_UI_T(RtwButton, "fmfactionicon.fmiconinfo.lbicon.btnicon");
	m_pLblTotemPicInfo	= LOAD_UI_T(RtwLabel,  "fmfactionicon.fmiconinfo.lbiconinfo");
	m_pLblTotemPrice	= LOAD_UI_T(RtwLabel,  "fmfactionicon.lbmoney2");
	m_pLblTotemHighlight= LOAD_UI_T(RtwLabel,  "fmfactionicon.fmbackground.lbhighlight");


	// 注册本窗口中的事件
	m_pBtnTotemUpLoad->EvLClick	+= RTW_CALLBACK(this, UIFormUnion, OnLClickTotemUpLoad);
	m_pBtnTotemOK->EvLClick		+= RTW_CALLBACK(this, UIFormUnion, OnLClickTotemOK);
	m_pBtnTotemCancel->EvLClick	+= RTW_CALLBACK(this, UIFormUnion, OnLClickTotemCancel);
	//end

	m_pSelectedUser = NULL;

	InitTotem();
	m_pFrmFaction->Hide();

	m_imgUnionNormal = g_workspace.getImageFactory()->createImage("ui\\x_textures\\1.tga");
	m_imgUnionNormal->SetSourceRect(RtwRect(122,400,159,442));
	m_imgUnionNormal->SetBlend(true);
	m_imgUnionAnimation = g_workspace.getImageFactory()->createImage("ui/ui_texture/xinshou_27.gif");
	m_bShowAnimation = false;

	CM_MEMPROTECTOR(m_ApplyLogoFileName, PATH_STRING_LEN)
	unguard;
}

UIFormUnion::~UIFormUnion()
{
	guard;
	for (int i=0; i<(int)m_vecTotemList.size(); ++i)
	{
		const void* pParam = m_vecTotemList[i].pWidget->getParam1();
		DEL_ONE(pParam);
	}
	CM_MEMUNPROTECTOR(m_ApplyLogoFileName)
	unguard;
}

void UIFormUnion::OnShow(void *,void *)
{
	OnPreDraw();
}

//刷新用户列表
void UIFormUnion::RefreshUserList()
{
	static string pSzMetair[] = {"五台", "花间", "蜀山", "苗疆"};
	bool bOnline = m_pChkOffLineUser->GetChecked();

	m_pLstMember->RemoveAllItems();

	char szTmp[128];
	int i = 0;
	int j = 0;

	while (j<(int)m_unionData.vecUnionCitizen.size())
	{
		SUnionCitizen *pTmp = &(m_unionData.vecUnionCitizen[j]);
		j++;
		if (!bOnline)
		{
			if (!pTmp->m_bOnLine)
			{
				continue;
			}
		}
		m_pLstMember->AddItem(pTmp->m_strPlayerName);

		//等级
		rt2_snprintf(szTmp, 128, "%d", pTmp->m_sLevel);
		m_pLstMember->SetItemText(i, 1, szTmp);

		//门派
		m_pLstMember->SetItemText(i, 2, pSzMetair[pTmp->m_cMetier-1].c_str());

		//职位
		rt2_snprintf(szTmp, 128, "%s", GetUOString(pTmp->m_eUnionOfficial));
		m_pLstMember->SetItemText(i, 3, szTmp);

		//帮派贡献
		rt2_snprintf(szTmp, 128, "%d", pTmp->m_nContribution);
		m_pLstMember->SetItemText(i, 4, szTmp);

		DWORD tmpColor = -1;

		if (!pTmp->m_bOnLine)
		{
			tmpColor = OfflineColor;
		}

		if (pTmp->m_nPlayerID == GetWorld()->m_pPlayer->DBID())
		{
			tmpColor = OwnColor;
		}
		m_pLstMember->SetLineColor(i, tmpColor);

		i++;
	}
}

void UIFormUnion::OnPreDraw()
{
	guard

	SUnionCitizen *own = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	UI_ENSURE(own);

	if (own->m_eUnionOfficial == UO_King)
	{
		m_pBtnPost->Enable();
	}

	char buf[128];
	
	rt2_snprintf(buf, 128, "携带银币:%d", GetWorld()->m_pPlayer->mItem.m_Bag.GetMoney());
	LOAD_UI_T(RtwLabel, "fmresources.fmfactionmoney.lbmoney")->SetText(buf);


	if (m_unionBack.m_nCaptainID != m_unionData.m_nCaptainID)
	{
		//老大更换

		m_unionBack.m_nCaptainID = m_unionData.m_nCaptainID;
	}

	if (m_unionBack.m_nPic != m_unionData.m_nPic)
	{
		//图腾更改
		m_pLblIcon->SetBackgroundImage(GetTotemPicFromID(m_unionData.m_nPic));

		m_unionBack.m_nPic = m_unionData.m_nPic;
	}

	if (m_unionBack.m_nFighting != m_unionData.m_nFighting)
	{
		//帮派势力更改
		rt2_snprintf(buf, 128, "%d", m_unionData.m_nFighting);
		m_pLblStrength->SetText(buf);

		m_unionBack.m_nFighting = m_unionData.m_nFighting;
	}

	if (m_unionBack.dwMoney != m_unionData.dwMoney)
	{
		//帮派资金变化
		rt2_snprintf(buf, 128, "%d", m_unionData.dwMoney);
		m_pLblFacMoney->SetText(buf);

		m_unionBack.dwMoney = m_unionData.dwMoney;
	}

	if (m_unionBack.m_strName != m_unionData.m_strName)
	{
		//帮派名字变化(可能是已经加入帮派的人刚登陆游戏)
		m_pLblFacName->SetText(m_unionData.m_strName);
	}

	if (m_unionBack.m_strChatPost != m_unionData.m_strChatPost)
	{
		//新的帮派公告
		m_pChatPost->SetText(m_unionData.m_strChatPost);

		m_unionBack.m_strChatPost = m_unionData.m_strChatPost;
	}

	if (m_unionBack.vecUnionCitizen != m_unionData.vecUnionCitizen)
	{
		//成员发生变化

		m_unionBack.vecUnionCitizen.clear();
		int nOnline = 0;

		for (int i=0; i<(int)m_unionData.vecUnionCitizen.size(); ++i)
		{
			if (m_unionData.vecUnionCitizen[i].m_bOnLine)
			{
				nOnline++;
			}
			m_unionBack.vecUnionCitizen.push_back(m_unionData.vecUnionCitizen[i]);
		}

		//帮派人数变化
		rt2_snprintf(buf, 128, "%d/%d人，(在线人数:%d)", m_unionData.vecUnionCitizen.size(), UNION_MAX_CITIZEN, nOnline);
		m_pLblPeopleNum->SetText(buf);

		m_unionBack.m_nPeopleNum = m_unionData.m_nPeopleNum = m_unionData.vecUnionCitizen.size();

		RefreshUserList();
	}

	/*
	帮派信息显示
	弹出式菜单的灰化(根据功能和权限)
	ui上无用的东西disable
	*/

	//   if (m_unionData.id!=0)
	//   {
	//       char tmp128[128];
	//       snprintf(tmp128, 127, "%d", m_unionData.level);
	//       tmp128[127] = 0;
	//       m_lbLevel->SetText(tmp128);
	//       m_lbName->SetText(m_unionData.name.c_str());
	//	m_lbUnionScore->SetText(rtFormatNumber(m_unionData.score));

	//	if ( strcmp(m_unionData.icon.c_str(), UNION_DEFAULT_ICON_NAME)!=0 )
	//	{
	//		CUnionIconMgr::SUnionIconData *pIconData = g_UnionIconMgr.Find((char*)m_unionData.icon.c_str());
	//		if (pIconData)
	//		{
	//			RtwImage *pImage = g_workspace.getImageFactory()->createImage(pIconData->data, pIconData->dataLen);
	//			m_lbLogo->SetBackgroundImage(pImage);
	//		}
	//		else 
	//			m_lbLogo->SetBackgroundImage((RtwImage*)NULL);
	//	}
	//	else 
	//		m_lbLogo->SetBackgroundImage((RtwImage*)NULL);
	//   }
	//   else 
	//   {
	//       m_lbLevel->SetText("");
	//       m_lbName->SetText("");
	//	m_lbUnionScore->SetText("");
	//	m_lbLogo->SetBackgroundImage((RtwImage*)NULL);
	//   }

	//// 排序
	//std::sort(m_unionData.arrCitizen.begin(), m_unionData.arrCitizen.end(), SortFunc);

	//m_listCitizen->RemoveAllItems();
	//int nCitizenCount = m_unionData.arrCitizen.size();
	//for (int i=0; i<nCitizenCount; i++)
	//{
	//	m_listCitizen->AddItem("");
	//	m_listCitizen->SetItemText(i, 0, m_unionData.arrCitizen[i].name );
	//	m_listCitizen->SetItemText(i, 1, (char*)GetUOString(m_unionData.arrCitizen[i].official) );
	//	m_listCitizen->SetItemText(i, 2, rtFormatNumber(m_unionData.arrCitizen[i].level));
	//	m_listCitizen->SetItemText(i, 3, m_unionData.arrCitizen[i].m_bOnline ? R(G_ONLINE) : R(G_OFFLINE));

	//	DWORD color = 0xffffffff;
	//	if (!m_unionData.arrCitizen[i].m_bOnline)
	//	{
	//		color = OfflineColor;
	//	}
	//	else 
	//	{
	//		if (m_unionData.arrCitizen[i].playerID == GetWorld()->m_pPlayer->DBID())
	//			color = OwnColor;
	//		else if (m_unionData.arrCitizen[i].playerID == m_unionData.captainID)
	//			color = CaptionColor;
	//		else 
	//			color = OnlineColor;
	//	}
	//	m_listCitizen->SetItemColor(i, color);
	//}

	//if (GetWorld() && GetWorld()->m_pPlayer)
	//{
	//	if (GetWorld()->m_pPlayer->DBID() == m_unionData.captainID)
	//		m_pEditBulletin->Enable();
	//	else
	//		m_pEditBulletin->Disable();

	//	if (GetWorld()->m_pPlayer->DBID() == m_unionData.captainID)
	//		m_pBtnBulletin->Enable();
	//	else
	//		m_pBtnBulletin->Disable();
	//}

	//m_pEditBulletin->SetText(m_unionData.bulletin);
	unguard;
}

bool SortFunc(const SUnionCitizen& p1, const SUnionCitizen& p2)
{
	guard
	bool bResult = false;
	switch (nSortFlag)
	{
	case 1:
		//等级
		bResult =  (p1.m_sLevel > p2.m_sLevel);
		break;
	case 2:
		//门派
		bResult =  (p1.m_cMetier > p2.m_cMetier);
		break;
	case 3:
		//职位
		bResult =  (p1.m_eUnionOfficial < p2.m_eUnionOfficial);
		break;
	case 4:
		//帮贡
		bResult =  (p1.m_nContribution > p2.m_nContribution);
		break;
	}

	return bSortLower[nSortFlag-1] ? bResult:!bResult;
	unguard;
}

void UIFormUnion::OnHeaderClick(RtwWidget* sender, void*)
{
	guard

	if (sender == m_pLstMember->GetColumnHeader(1))
	{
		//等级
		nSortFlag = 1;
	}
	else if (sender == m_pLstMember->GetColumnHeader(2))
	{
		//门派
		nSortFlag = 2;
	}
	else if (sender == m_pLstMember->GetColumnHeader(3))
	{
		//职位
		nSortFlag = 3;
	}
	else if (sender == m_pLstMember->GetColumnHeader(4))
	{
		//帮贡
		nSortFlag = 4;
	}

	std::sort(m_unionData.vecUnionCitizen.begin(), m_unionData.vecUnionCitizen.end(), SortFunc);
	bSortLower[nSortFlag-1] = !bSortLower[nSortFlag-1];

	int nPos = (nSortFlag-1)*2 + (bSortLower[nSortFlag-1]?1:0);
	m_pLstMember->GetColumnHeader(nSortFlag)->SetNormalImage(pSortImage[nPos]);

	RefreshUserList();

	unguard;
}

void UIFormUnion::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmFaction->Show();
	}
	unguard;
}


void UIFormUnion::Hide()
{
	guard;
	m_pFrmFaction->Hide();
	unguard;
}

void UIFormUnion::Show()
{
	guard;
	OnPreDraw();
	m_pFrmFaction->Show();
	unguard;
}

bool UIFormUnion::IsVisible()
{
	guard;
	if ( m_pFrmFaction && m_pFrmFaction->GetFlags(wfVisible) )	
		return true;
	else	
		return false;
	unguard;
}

void UIFormUnion::OnBtnDismiss(RtwWidget* sender, void*)
{
	guard;

	unguard;
}

void UIFormUnion::OnBtnTitle(RtwWidget* sender, void*)
{
	guard;

	// 	if (m_unionData.id==0)
	// 		return;
	// 
	// 	int iSel = m_listCitizen->GetSelectedItem();
	// 	if (iSel>=0 && iSel<m_unionData.arrCitizen.size())
	// 	{
	// 		// CHECK UP
	//         // 检查是否有权限
	//         SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	//         CHECK(pOwn);
	//         if (!Union_CanChangeNickname(pOwn, &(m_unionData.arrCitizen[iSel]) ) )
	//         {
	//             PlayerFailOperate(R(MSG_UNION_NO_RIGHTS));
	//             return;
	//         }
	// 
	// 		m_bkPlayerTitle = m_unionData.arrCitizen[iSel].playerID;
	// 		char tmp128[128];
	// 		snprintf(tmp128, 127, R(MSG_UNION_INPUT_NEW_NICKNAME), m_unionData.arrCitizen[iSel].name.c_str());
	//         tmp128[127] = 0;
	// 		UIFormTextMsg *pFrm;
	// 		pFrm = UIFormTextMsg::ShowStatic(R(MSG_UNION_CHANGE_NICKNAME), tmp128);
	// 		pFrm->EvOK = RTW_CALLBACK_1(this, UIFormUnion, OnTitle, pFrm);
	// 	}
	// 	else
	// 		PlayerFailOperate(R(MSG_UNION_SELECT_CITIZEN_FOR_NICKNAME));

	unguard;
}

void UIFormUnion::OnBtnLogo(RtwWidget* sender, void*)
{
	guard;
	// 
	// 	if (m_unionData.id==0)
	// 		return;
	// 
	//     if (m_unionData.captainID != GetWorld()->m_pPlayer->DBID())
	//     {
	//         PlayerFailOperate(R(MSG_UNION_NO_RIGHTS));
	//         return;
	//     }
	// 
	// 	if (m_unionData.level < 3)
	// 	{
	// 		PlayerFailOperate(R(MSG_UNION_LOGO_LEV3));
	// 		return;
	// 	}
	// 
	// 	char tmp128[128];
	// 	snprintf(tmp128, 127, R(MSG_UNION_INPUT_FILEPATH));
	//     tmp128[127] = 0;
	// 	UIFormTextMsg *pFrm;
	// 	pFrm = UIFormTextMsg::ShowStatic(R(MSG_UNION_CHANGE_ICON), tmp128);
	// 	pFrm->EvOK = RTW_CALLBACK_1(this, UIFormUnion, OnLogo, pFrm);

	unguard;
}

void UIFormUnion::OnBtnAppoint(RtwWidget* sender, void*)
{
	guard;
	unguard;
}

void UIFormUnion::OnBtnSurrender(RtwWidget* sender, void*)
{
	guard;
	// 
	// 	if (m_unionData.id==0)
	// 		return;
	// 
	// 	char tmp128[128];
	// 	snprintf(tmp128, 127, R(MSG_UNION_SURRENDER_MAKESURE));
	//     tmp128[127] = 0;
	// 	UIFormMsg *pFrm = UIFormMsg::ShowStatic(tmp128, UIFormMsg::TYPE_OK_CANCEL);
	// 	pFrm->EvOK = RTW_CALLBACK_1(this, UIFormUnion, OnSurrender,pFrm);

	unguard;
}

void UIFormUnion::OnBtnLeave(RtwWidget* sender, void*)
{
	guard;

	unguard;
}

void UIFormUnion::OnAskInheritor(RtwWidget* sender, void*)
{
	guard;
	// 
	// 	char tmp128[128];
	// 	snprintf(tmp128, 127, R(MSG_UNION_ASK_HAVE_INHERITOR));
	//     tmp128[127] = 0;
	// 	UIFormMsg *pFrm = UIFormMsg::ShowStatic(tmp128, UIFormMsg::TYPE_OK_CANCEL, true, "AskInheritor",false);
	// 	pFrm->EvOK = RTW_CALLBACK_1(this, UIFormUnion, OnSelectInheritor,pFrm);
	// 	pFrm->EvCancel = RTW_CALLBACK_1(this, UIFormUnion, OnLeave,pFrm);

	unguard;
}

void UIFormUnion::OnSelectInheritor(RtwWidget* sender, void*)
{
	guard;
	// 
	// 	char tmp128[128];
	// 	snprintf(tmp128, 127, R(MSG_UNION_INPUT_INHERITOR_NAME));
	//     tmp128[127] = 0;
	// 	UIFormTextMsg *pFrm;
	// 	pFrm = UIFormTextMsg::ShowStatic(R(MSG_UNION_SELECT_INHERITOR), tmp128);
	// 	pFrm->EvOK = RTW_CALLBACK_1(this, UIFormUnion, OnLeaveWithInheritor, pFrm);

	unguard;
}

void UIFormUnion::OnDismiss(RtwWidget* sender, void*)
{
	guard;
	// 
	// 	SUnionCitizen* pCitizen = Find(m_bkPlayerDismiss);
	// 	if ( pCitizen )
	// 	{
	// 		CUnionNetHelper::NetSend_c2r_union_dismiss_other(m_unionData.id, m_bkPlayerDismiss);
	// 		m_bkPlayerDismiss = 0;
	// 	}

	unguard;
}

void UIFormUnion::OnTitle(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;

	// 	if (!IsVisible())
	// 		return;
	// 
	// 	UIFormTextMsg *pFrm = (UIFormTextMsg *)e->GetUser1();
	// 	RtString str = pFrm->GetString();
	// 	int length = str.GetLength();
	// 	if (length >= 0 && length <= UNION_NAME_STRING_MAX)
	// 	{
	// 		if (m_bkPlayerTitle>0)
	// 		{
	// 			CUnionNetHelper::NetSend_c2r_union_change_nickname(m_unionData.id, m_bkPlayerTitle, str);
	// 			m_bkPlayerTitle = 0;
	// 		}
	// 	}
	// 	else
	//     {
	//         char cTmp128[128];
	//         snprintf(cTmp128, 127, R(MSG_UNION_INVALID_NAME));//, 0, UNION_NAME_STRING_MAX/2);
	//         cTmp128[127] = 0;
	//         PlayerFailOperate(cTmp128);
	//     }

	unguard;
}

void UIFormUnion::OnLogo(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	// 
	// 	if (!IsVisible())
	// 		return;
	// 
	// 	UIFormTextMsg *pFrm = (UIFormTextMsg *)e->GetUser1();
	// 	RtString fileName = pFrm->GetString();
	// 	int length = fileName.GetLength();
	// 	if (length == 0)
	// 	{
	// 		PlayerFailOperate(R(MSG_UNION_INPUT_NAME));
	// 		return;
	// 	}
	// 	else if (length > 128)
	// 	{
	// 		PlayerFailOperate(R(MSG_UNION_NAME_TOO_LONG));
	// 		return;
	// 	}
	// 
	// 	RtArchive *pAr = NULL;
	// 	RtString PathName = "";//"ui\\ui_texture\\unionIcon\\";
	// 	PathName += fileName;
	// // change [3/20/2009 tooth.shi]	pAr = g_pFileManager->CreateFileReader(PathName);
	// 	pAr=RtCoreFile().CreateFileReader(PathName);
	// 	if ( !pAr)
	// 	{
	// 		PlayerFailOperate(R(MSG_UNION_FILE_NOT_EXIST));
	// 		return;
	// 	}
	// 	int len = pAr->TotalSize();
	// 	if (len!=822 && len!=824)
	// 	{
	// 		PlayerFailOperate(R(MSG_UNION_INVALID_FILE_FORMAT));
	// 		return;
	// 	}
	// 	int iImageWidth, iImageHeight;
	// 	bool res = RtgImage::Check24bBmpFormat(PathName, iImageWidth, iImageHeight);
	// 	if (!res || iImageHeight!=16 || iImageWidth!=16)
	// 	{
	// 		PlayerFailOperate(R(MSG_UNION_INVALID_FILE_FORMAT));
	// 		return;
	// 	}
	// 
	// 	char *data = (char *)malloc(len+1);
	// 	memset(data, 0, len+1);
	// 	len = pAr->Serialize(data, len);
	// // change [3/20/2009 tooth.shi]	g_pFileManager->CloseFile(pAr);
	// 	RtCoreFile().CloseFile(pAr);
	// 	strcpy(m_ApplyLogoFileName, (char*)fileName);
	// 
	// 	CUnionNetHelper::NetSend_c2r_union_logo(data, len);
	// 
	// 	free(data);
	unguard;
}

void UIFormUnion::OnAppoint(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	// 
	// 	if (!IsVisible())
	// 		return;
	// 
	// 	UIFormTextMsg *pFrm = (UIFormTextMsg *)e->GetUser1();
	// 	RtString str = pFrm->GetString();
	// 	
	// 	EUnionOfficial official;
	// 	if ( str==GetUOString(UO_Guarder) )				official = UO_Guarder;
	// 	else if ( str==GetUOString(UO_Jiang) )			official = UO_Jiang;
	// 	else if ( str==GetUOString(UO_Tai) )			official = UO_Tai;
	// 	else
	// 	{
	// 		PlayerFailOperate(R(MSG_UNION_INVALID_OFFICIAL));
	// 		return;
	// 	}
	// 
	// 	if (official==UO_Jiang)
	// 	{
	// 		if (m_unionData.level<3)
	// 		{
	// 			PlayerFailOperate(R(MSG_UNION_JIANG_LEV3));
	// 			return;
	// 		}
	// 		int i;
	// 		for (i=0; i<(int)m_unionData.arrCitizen.size(); i++)
	// 		{
	// 			if (m_unionData.arrCitizen[i].official == UO_Jiang)
	// 				break;
	// 		}
	// 		if (i!=(int)m_unionData.arrCitizen.size())
	// 		{
	// 			PlayerFailOperate(R(MSG_UNION_ALREADY_HAVE_JIANG));
	// 			return;
	// 		}
	// 	}
	// 	else if (official==UO_Tai)
	// 	{
	// 		if (m_unionData.level<4)
	// 		{
	// 			PlayerFailOperate(R(MSG_UNION_TAI_LEV4));
	// 			return;
	// 		}
	// 		int i;
	// 		for (i=0; i<(int)m_unionData.arrCitizen.size(); i++)
	// 		{
	// 			if (m_unionData.arrCitizen[i].official == UO_Tai)
	// 				break;
	// 		}
	// 		if (i!=(int)m_unionData.arrCitizen.size())
	// 		{
	// 			PlayerFailOperate(R(MSG_UNION_ALREADY_HAVE_TAI));
	// 			return;
	// 		}
	// 	}
	// 
	//     SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	//     if (!pOwn)      return;
	//     if ( official >= pOwn->official )
	//     {
	//         PlayerFailOperate(R(MSG_UNION_NO_RIGHTS));
	//         return;
	//     }
	// 
	// 	if (m_bkPlayerAppoint>0)
	// 	{
	// 		CUnionNetHelper::NetSend_c2r_union_appoint(m_unionData.id, m_bkPlayerAppoint, official);
	// 		m_bkPlayerAppoint = 0;
	// 	}

	unguard;
}

void UIFormUnion::OnSurrender(RtwWidget* sender, void*)
{
	guard;

	//	CUnionNetHelper::NetSend_c2r_union_surrender(m_unionData.id, 0);

	unguard;
}

void UIFormUnion::OnLeave(RtwWidget* sender, void*)
{
	guard;
	//CUnionNetHelper::NetSend_c2r_union_leave(m_unionData.id, 0);
	unguard;
}

void UIFormUnion::OnLeaveWithInheritor(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	unguard;
}

void UIFormUnion::OnBtnInvite(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
	ShowSystemMessage(R(MSG_UNION_SELECT_INVITE_PLAYER));

	/*RtwButton* pButton = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
	pButton->CloneFrom((RtwButton*)vpSender);
	RtwAliasButton* pActive = (RtwAliasButton*)g_workspace.LookupWidget("fmhotkey.lbhotkeyenable");
	if(!pButton || !pActive) return;
	pActive->SetButton(pButton);*/ //gao 2009.12.18 去除原右键技能框
}

//邀请添加帮派
void UIFormUnion::InviteJoin(DWORD playerID)
{
	CUnionNetHelper::NetSend_c2r_union_join(m_unionData.m_nID, playerID);
	ShowSystemMessage(R(MSG_ITEM_WAIT_FOR_MESSAGE));
}

void UIFormUnion::OnPopupMenu(RtwPopupMenu* pPopupMenu, GcActor* pTargetActor, int iData)
{
	guard;
	 
	 	if (pTargetActor==NULL) return;
	 
	 	if (   pTargetActor->NpcType() != GcActor::ENT_NPC_SERVICE
	 		&& pTargetActor->NpcType() != GcActor::ENT_MONSTER
	 		&& pTargetActor->NpcType() != GcActor::ENT_NPC_COMBATIVE)
	 	{
	 		pPopupMenu->AddItem(R(MSG_UNION), NULL, (void*)iData);
	 	}

	unguard;
}

//邀请
bool UIFormUnion::OnCommand(GcActor* pTargetActor)
{
	guard;

	if (pTargetActor==NULL) return false;

	if ( GetWorld()->m_pPlayer->GetUnionID() == 0 )
	{
		PlayerFailOperate(R(MSG_UNION_HAVE_NO_UNION));
		return true;
	}

	if (pTargetActor->NpcType()==GcActor::ENT_NPC_SERVICE)
	{
		PlayerFailOperate(R(MSG_ITEM_NOT_PLAYER));
		return true;
	}

	//if (pTargetActor->m_core.Faction != GetWorld()->m_pPlayer->m_core.Faction)
	//{
	//	PlayerFailOperate(R(MSG_UNION_JOIN_FAIL_FACTION));
	//	return true;
	//}

	SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	if (!pOwn)
		return true;

	//if (!Union_GetEnableFromAuction(UA_INVITEMEMBER, pOwn))
	//{
	//	PlayerFailOperate(R(MSG_UNION_NO_RIGHTS));
	//	return true;
	//}

	if ( m_unionData.vecUnionCitizen.size() >= UNION_MAX_CITIZEN)
	{
		PlayerFailOperate(R(MSG_UNION_CITIZENS_FULL));
		return true;
	}

	int currTime = rtGetMilliseconds();
	if ( abs(currTime - m_timeLastInvite) < 5000 )
	{
		PlayerFailOperate(R(MSG_ITEM_WAITFOR_NEXTINVITE));
	}
	else 
	{
		if (pTargetActor->GetUnionID()!=0)
		{
			PlayerFailOperate(R(MSG_UNION_IS_ALREADY_IN_A_UNION));
			return true;
		}
		m_timeLastInvite = currTime;
		InviteJoin( pTargetActor->ID() );
	}

	return true;

	unguard;
}

bool UIFormUnion::MouseRClick(int x, int y)
{
	guard;
	// 	return OnCommand(GetWorld()->HitTest(x, y, 0xFC));
	return true;
	unguard;
}

void UIFormUnion::OnRecvNewBulletin(char* cNewBulletin)
{
	guard;

	m_unionData.m_strChatPost = cNewBulletin;
	Refresh();

	char cTmp128[128];
	rt2_snprintf(cTmp128, 128, R(MSG_UNION_HAVE_NEW_BULLETIN));
	//cTmp128[127] = 0;
	ShowSystemMessage(cTmp128);
	// 
	unguard;
}

void UIFormUnion::OnTimerEvent(DWORD dwID)
{
	guard;

	if (m_pFrmMsg)
	{
		DWORD *pData = (DWORD*)m_pFrmMsg->GetUserData();

		CUnionNetHelper::NetSend_c2r_union_other_join_response(GetWorld()->m_pPlayer->GetUnionID(), *pData, 0);

		m_pFrmMsg->Hide(); 
		m_pFrmMsg = NULL;
	}
	GetTimer()->RemoveTimer(dwID);
	unguard;
}

void UIFormUnion::ResponseOther_Agree(ui::RtwWidget* vpSender, RtwEventDelegate* e)
{
	guard;

	UIFormMsg *pFrm = (UIFormMsg *)vpSender->GetUser1();
	DWORD *pUserData = (DWORD*)pFrm->GetUserData();

	CUnionNetHelper::NetSend_c2r_union_other_join_response(GetWorld()->m_pPlayer->GetUnionID(), *pUserData, 1);

	DEL_ONE(pUserData);

	SetUnionButtonNormal();

	unguard;
}

void UIFormUnion::ResponseOther_Disagree(ui::RtwWidget* vpSender, RtwEventDelegate* e)
{
	guard;

	UIFormMsg *pFrm = (UIFormMsg *)vpSender->GetUser1();
	DWORD *pUserData = (DWORD*)pFrm->GetUserData();

	CUnionNetHelper::NetSend_c2r_union_other_join_response(GetWorld()->m_pPlayer->GetUnionID(), *pUserData, 0);

	DEL_ONE(pUserData);

	SetUnionButtonNormal();

	unguard;
}

void UIFormUnion::OnRecvActorUnionData(GcActor *pActor)
{
	guard;

	if (!pActor)		    return;
	if (!pActor->m_pHUD)    return;

	if (pActor->GetUnionID()!=0)
	{
		char* tmp = (char*)pActor->m_unionName;
		pActor->m_pHUD->SetGuildText(tmp);
		tmp = (char*)pActor->m_unionNickName;
		pActor->m_pHUD->SetAgnameText(tmp);
// 		if ( strcmp(pActor->m_unionIcon, UNION_DEFAULT_ICON_NAME)!=0 )
// 		{
// 			CUnionIconMgr::SUnionIconData *pIconData = g_UnionIconMgr.Find((char*)pActor->m_unionIcon);
// 			if (pIconData)
// 			{
// 				RtwImage *pImage = g_workspace.getImageFactory()->createImage(pIconData->data, pIconData->dataLen);

		if (pActor->GetUnionID() == m_unionData.m_nID)
		{
			pActor->m_pHUD->SetGuildLogo(GetTotemPicFromID(m_unionData.m_nPic));
		}
		else
		{
			pActor->m_pHUD->SetGuildLogo(GetTotemPicFromID(atoi(pActor->m_unionIcon.c_str())));
		}
// 			}
// 			else
// 			{
// 				CUnionNetHelper::NetSend_c2r_union_query_logo_data(pActor->m_unionIcon);
// 				pActor->m_pHUD->SetGuildLogo(NULL);
// 			}
//		}
	}
	else 
	{
		pActor->m_pHUD->SetGuildText("");
		pActor->m_pHUD->SetAgnameText("");
		pActor->m_pHUD->SetGuildLogo(NULL);
	}

	if (pActor->ID() == GetWorld()->m_pPlayer->ID())
	{
		if (pActor->GetUnionID()==0)
		{
			m_unionData.Clear();
		}

		Refresh();

		if (g_layerMain->m_formCharProp)
			g_layerMain->m_formCharProp->Update();
	}

	unguard;
}

void UIFormUnion::OnRecvLogoData(char* FileName)
{
	guard;
	// 
	// 	if (strcmp(GetWorld()->m_pPlayer->m_unionIcon, FileName)==0)
	// 		OnRecvActorUnionData(GetWorld()->m_pPlayer);
	// 
	// 	EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator iterUser = GetWorld()->m_ActorManager.m_ActorMap.begin();
	// 	for (; iterUser!=GetWorld()->m_ActorManager.m_ActorMap.end(); iterUser++)
	// 	{
	// 		GcSynchroActor* pActor = iterUser->second;
	// 		if (strcmp(pActor->m_unionIcon, FileName)==0)
	// 			OnRecvActorUnionData(pActor);
	// 	}

	unguard;
}

void UIFormUnion::OnRecvChangeLogoSuccess(char* NewFileName)
{
	guard;

	// 	char tmp128[128];
	// 	snprintf(tmp128, 128, R(MSG_UNION_CHANGLOGO_SUCCESS));
	//     tmp128[127] = 0;
	// 	ShowSystemMessage(tmp128);
	// 
	// 	GetWorld()->m_pPlayer->m_unionIcon = NewFileName;
	// 	OnRecvActorUnionData(GetWorld()->m_pPlayer);

	unguard;
}

void UIFormUnion::OnRecvChangeLogoFail()
{
	guard;
	// 
	// 	char tmp128[128];
	// 	snprintf(tmp128, 127, R(MSG_UNION_CHANGLOGO_FAIL));
	//     tmp128[127] = 0;
	// 	PlayerFailOperate(tmp128);

	unguard;
}




CUnionIconMgr::SUnionIconData *CUnionIconMgr::Find(string fileName)
{
	guard;
	// 
	// 	for (int i=0; i<m_arrIcon.size(); i++)
	// 	{
	// 		if (m_arrIcon[i].fileName==fileName)
	// 			return &(m_arrIcon[i]);
	// 	}
	// 	return NULL;
	// 
	return NULL;
	unguard;
}

void UIFormUnion::OnClickFunction(RtwWidget* vpSender, RtwEventDelegate* e)			//点击功能按钮
{
	SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	if (!pOwn)
	{
		//出错了,Y没在帮派咋能打开帮派界面呢
		return;
	}

	EUnionOfficial eOwnOfficial = pOwn->m_eUnionOfficial;

	m_pFuncMenu->RemoveAllItems();
	m_pFuncMenu->AddMenuItem("添加帮众", eOwnOfficial<UO_Masses);
	m_pFuncMenu->AddMenuItem("查看权限");
	m_pFuncMenu->AddMenuItem("帮派图腾", eOwnOfficial==UO_King);
	m_pFuncMenu->AddMenuItem("脱离帮派", eOwnOfficial!=UO_King);
	m_pFuncMenu->AddMenuItem("解散帮派", eOwnOfficial==UO_King);

	SPoint xy = m_pBtnFunction->GetFrameRect().getLeftTop();
	xy.y -= m_pFuncMenu->GetFrameRect().getHeight();
	g_workspace.ShowPopupMenu(m_pFuncMenu, NULL, xy.x, xy.y);
}

void UIFormUnion::OnClickExit(RtwWidget* vpSender, RtwEventDelegate* e)				//点击退出
{
	m_pFrmFaction->Hide();
}

void UIFormUnion::OnClickContribution(RtwWidget* vpSender, RtwEventDelegate* e)		//点击贡献
{	
	m_pTxtMoney->SetText("");
	m_pFmMoney->ShowModal();
	m_pFmMoney->BringToTop();
}

void UIFormUnion::OnClickPost(RtwWidget* vpSender, RtwEventDelegate* e)				//点击发布公告
{
	m_pFmBulletin->ShowModal();
	m_pFmBulletin->BringToTop();
	m_pTxtBulletin->SetText(m_unionData.m_strChatPost);
}

void UIFormUnion::OnClickPrewBtn(RtwWidget* vpSender, RtwEventDelegate* e)			//点击前一区域
{

}

void UIFormUnion::OnClickNextBtn(RtwWidget* vpSender, RtwEventDelegate* e)			//点击后一区域
{

}

void UIFormUnion::OnRClickMember(RtwWidget* vpSender, RtwEventDelegate* e)			//右击成员列表
{
 	SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
 	if (!pOwn)
 	{
 		//出错了,Y没在帮派咋能打开帮派界面呢
 		return;
 	}

	int iSelect = m_pUserList->GetLastSelectItem();
	std::string strUserName = m_pLstMember->GetItemText(m_pLstMember->GetSelectedItem(), 0);

	SUnionCitizen *pTmp = m_unionData.Find(strUserName);
	if (pOwn == pTmp)
	{
		//自己不需要弹出右键菜单
		return;
	}

	m_pSelectedUser = pTmp;
	EUnionOfficial eOwnOfficial = pOwn->m_eUnionOfficial;
	bool bOnline = pTmp->m_bOnLine;


	m_pUserList->RemoveAllItems();
	m_pUserList->AddMenuItem("复制名称");
	m_pUserList->AddMenuItem("密语", bOnline);
	m_pUserList->AddMenuItem("加为好友", bOnline);
	m_pUserList->AddMenuItem("申请组队", bOnline);
	m_pUserList->AddMenuItem("邀请组队", bOnline);
// 	m_pUserList->AddMenuItem(eOwnOfficial==UO_King?"禅让帮主":"弹劾帮主",  bOnline);
	m_pUserList->AddMenuItem("禅让帮主", eOwnOfficial==UO_King && bOnline);
	m_pUserList->AddMenuItem("逐出帮派", eOwnOfficial<UO_SubLeader && bOnline);
	m_pUserList->AddMenuItem("调整职位", eOwnOfficial<UO_SubLeader && bOnline);
	m_pUserList->AddMenuItem("频道禁言", /*eOwnOfficial<UO_SubLeader && bOnline*/false);

	g_workspace.ShowPopupMenu(m_pUserList, NULL, g_workspace.GetMousePosX(), g_workspace.GetMousePosY());
}

void UIFormUnion::OnCheckOffline(RtwWidget* vpSender, RtwEventDelegate* e)				//离线用户
{
	RefreshUserList();
}

void UIFormUnion::OnFuncPopMenuSelect(RtwWidget* vpSender, RtwEventDelegate* e)			//功能菜单处理
{
	int iSelect = m_pFuncMenu->GetLastSelectItem();

	switch (iSelect)
	{
	case 0:
		menu_AddMember();			//添加帮众
		break;
	case 1:
		menu_ViewPower();			//查看权限
		break;	
	case 2:
		menu_UnionPic();			//帮派图腾
		break;
		// 	case 3:
		// 		menu_SelfIntroduction();	//自我介绍
		// 		break;
	case 3:
		menu_LeaveUnion();			//脱离帮派
		break;
	case 4:
		menu_DisbandUnion();		//解散帮派
		break;
	default:
		break;
	}
}

void UIFormUnion::OnFuncPopMenuLoseFocus(RtwWidget* vpSender, RtwEventDelegate* e)		//功能菜单失效
{
	if(m_pFuncMenu->IsVisible())
	{
		m_pFuncMenu->Hide();
	}
}

void UIFormUnion::OnUserListPopMenuSelect(RtwWidget* vpSender, RtwEventDelegate* e)		//用户列表菜单处理
{
	int iSelect = m_pUserList->GetLastSelectItem();
	std::string strUserName = m_pLstMember->GetItemText(m_pLstMember->GetSelectedItem(), 0);

	m_pSelectedUser = m_unionData.Find(strUserName);	//选中的玩家
	UI_ENSURE(m_pSelectedUser);


	switch (iSelect)
	{
	case 0:
		menu_CopyName();	//复制名称
		break;
	case 1:
		menu_ChatTo();		//密语
		break;
	case 2:
		menu_AddFriend();	//加为好友
		break;
	case 3:
		menu_ApplyTeam();	//申请组队
		break;
	case 4:
		menu_InviteTeam();	//邀请组队
		break;
	case 5:
		menu_Demise();		//禅让帮主
		break;
	case 6:
		menu_Banishment();	//逐出帮派
		break;
	case 7:
		menu_Appoint();		//调整职位
		break;
	case 8:
		menu_DisableChannel();//频道禁言
		break;
	default:
		break;
	}
}

void UIFormUnion::OnUserListPopMenuLoseFocus(RtwWidget* vpSender, RtwEventDelegate* e)	//用户列表菜单失效
{
	if(m_pUserList->IsVisible())
	{
		m_pUserList->Hide();
	}
}

void UIFormUnion::menu_Demise()	//禅让
{
	UI_ENSURE(m_pSelectedUser);

	SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	UI_ENSURE(pOwn);

	if (!Union_GetEnableFromAuction(UA_DEMISEUNION, pOwn))
	{
		//非帮主不可以禅让
		return;
	}

	char szBuf[256];
	rt2_snprintf(szBuf, 256, R(MSG_UNION_CHANRANGBANGZHU), m_pSelectedUser->m_strPlayerName.c_str());
	UIFormMsg *pFrm = UIFormMsg::ShowStatic(szBuf, UIFormMsg::TYPE_OK_CANCEL);
	pFrm->EvOK = RTW_CALLBACK_1(this, UIFormUnion, Demise, pFrm);
}

void UIFormUnion::menu_ChatTo()	//密语
{
	UI_ENSURE(m_pSelectedUser);

	GetWorld()->m_Chat.OnMemberChoice(0, m_pSelectedUser->m_strPlayerName.c_str());
}

void UIFormUnion::menu_Appoint()	//调整职位
{
	// 
	// 	if (m_unionData.id==0)
	// 		return;
	// 
	// 	int iSel = m_listCitizen->GetSelectedItem();
	// 	if (iSel>=0 && iSel<m_unionData.arrCitizen.size())
	// 	{
	// 		// CHECK UP
	//         SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	//         CHECK(pOwn);
	//         if (pOwn->playerID == m_unionData.arrCitizen[iSel].playerID)
	//         {
	//             PlayerFailOperate(R(MSG_UNION_CANNOT_USESELF));
	//             return;
	//         }
	//         else if (!Union_CanAppoint(&m_unionData, pOwn, &(m_unionData.arrCitizen[iSel]), UO_Guarder))
	//         {
	//             PlayerFailOperate(R(MSG_UNION_NO_RIGHTS));
	//             return;
	//         }
	// 
	// 		m_bkPlayerAppoint = m_unionData.arrCitizen[iSel].playerID;
	// 		char tmp128[128];
	// 		snprintf(tmp128, 127, R(MSG_UNION_INPUT_OFFICIAL), m_unionData.arrCitizen[iSel].name.c_str());
	//         tmp128[127] = 0;
	// 		UIFormTextMsg *pFrm;
	// 		pFrm = UIFormTextMsg::ShowStatic(R(MSG_UNION_APPOINT), tmp128);
	// 		pFrm->EvOK = RTW_CALLBACK_1(this, UIFormUnion, OnAppoint, pFrm);
	// 	}
	// 	else
	// 		PlayerFailOperate(R(MSG_UNION_SELECT_CITIZEN_FOR_APPOINT));

	UI_ENSURE(m_pSelectedUser);

	SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	UI_ENSURE(pOwn);

	if (!Union_GetEnableFromAuction(UA_LEVELCHANGE, pOwn, m_pSelectedUser))
	{
		//不能提升别人权限
		return;
	}

	m_pCbxChangeText->RemoveAllItems();
	m_vecChange.clear();
	for (int i=(int)pOwn->m_eUnionOfficial+1; i<=(int)UO_Masses; ++i)
	{
		if (i == (int)m_pSelectedUser->m_eUnionOfficial)
		{
			continue;
		}

		if (!m_unionData.Union_ChangeOfficialEnable((EUnionOfficial)i))
		{
			continue;
		}

		m_pCbxChangeText->AddListItem((const char*)GetUOString((EUnionOfficial)i));
		m_vecChange.push_back((EUnionOfficial)i);
	}

	m_pCbxChangeText->SetSelectedItem(0);

	//打开转换界面
	Change();
}

void UIFormUnion::menu_CopyName()	//复制名称
{
	if (!m_pSelectedUser)
	{
		return;
	}

	g_workspace.getClipboard()->Copy(m_pSelectedUser->m_strPlayerName.c_str());
}

void UIFormUnion::menu_AddFriend()	//加为好友
{
	UI_ENSURE(m_pSelectedUser);

	/*CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_mail);
	pPacket->WriteByte(C2R_INVITE_OTHER_FRIEND_BYNAME);
	pPacket->WriteLong(0);
	pPacket->WriteString(m_pSelectedUser->m_strPlayerName.c_str());
	NetSend(pPacket);*/
	GetWorld()->m_Friend.InviteOther(m_pSelectedUser->m_strPlayerName);
}

void UIFormUnion::menu_ApplyTeam()	//申请组队
{
	UI_ENSURE(m_pSelectedUser);
	GetWorld()->m_Team.ApplyOtherTeam(m_pSelectedUser->m_strPlayerName);
}

void UIFormUnion::menu_InviteTeam()	//邀请组队
{
	UI_ENSURE(m_pSelectedUser);
	GetWorld()->m_Team.InviteOther(m_pSelectedUser->m_strPlayerName);
}

void UIFormUnion::menu_Banishment()	//逐出帮派
{
	UI_ENSURE(m_pSelectedUser);
	
	SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
 	UI_ENSURE(pOwn);
 	 
 	if (!Union_GetEnableFromAuction(UA_BANISHMENT, pOwn, m_pSelectedUser))
 	{
 		//不能逐出帮派
 		return;
 	}
	 
	char szBuf[256];
	rt2_snprintf(szBuf, 256, R(MSG_UNION_QUZHUCHENGYUAN), m_pSelectedUser->m_strPlayerName.c_str());
	UIFormMsg *pFrm = UIFormMsg::ShowStatic(szBuf, UIFormMsg::TYPE_OK_CANCEL);
	pFrm->EvOK = RTW_CALLBACK_1(this, UIFormUnion, Banishment, pFrm);
}

void UIFormUnion::menu_DisableChannel()	//禁言
{
	UI_ENSURE(m_pSelectedUser);

	SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	UI_ENSURE(pOwn);

	if (!Union_GetEnableFromAuction(UA_DISABLECHANNEL, pOwn, m_pSelectedUser))
	{
		//不能禁言
		return;
	}

}

void UIFormUnion::menu_AddMember()	//添加帮众
{
	SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	UI_ENSURE(pOwn);

	if (!Union_GetEnableFromAuction(UA_ADDMEMBER, pOwn))
	{
		//不能添加帮众
		return;
	}

	AddMember();
}

void UIFormUnion::menu_ViewPower()	//查看权限
{
	m_pFmViewPower->Show();
}

void UIFormUnion::menu_LeaveUnion()	//脱离帮派
{
	SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	UI_ENSURE(pOwn);

	if (!Union_GetEnableFromAuction(UA_LEAVEUNION, pOwn))
	{
		//不能脱离帮派
		return;
	}

	LeaveUnion();
}

void UIFormUnion::menu_SelfIntroduction()	//自我介绍
{
	//SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	//UI_ENSURE(pOwn);

	SelfIntro();
}

void UIFormUnion::menu_DisbandUnion()	//解散帮派
{
	SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	UI_ENSURE(pOwn);

	if (!Union_GetEnableFromAuction(UA_UNIONDISBAND, pOwn))
	{
		//不能解散帮派
		return;
	}

	char szBuf[256];
	rt2_snprintf(szBuf, 256, R(MSG_UNION_JIESANBANGPAI));
	UIFormMsg *pFrm = UIFormMsg::ShowStatic(szBuf, UIFormMsg::TYPE_OK_CANCEL);
	pFrm->EvOK = RTW_CALLBACK_1(this, UIFormUnion, Disband, pFrm);
}

void UIFormUnion::menu_UnionPic()	//帮派图腾
{
	SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	UI_ENSURE(pOwn);

	if (!Union_GetEnableFromAuction(UA_CHANGEUNIONPIC, pOwn))
	{
		//不能登陆帮派图腾-没有权限登录图腾
		return;
	}

	nSelectTotemID = m_unionData.m_nPic;
	RefreshTotem();
}

//调整职业
void UIFormUnion::Change()	//开始更改职业
{
	m_pFmAppoint->ShowModal();
	m_pFmAppoint->BringToTop();

	char tmpBuf[128];
	rt2_snprintf(tmpBuf, 128, R(MSG_UNION_GENGGAIZHIWEI), GetUOString(m_pSelectedUser->m_eUnionOfficial), m_pSelectedUser->m_strPlayerName.c_str());
	m_pLblChangeTextTips->SetText(tmpBuf);
}

void UIFormUnion::OnClickChangeOk(RtwWidget* vpSender, RtwEventDelegate* e)	//点击ok
{
	int nIndex = m_pCbxChangeText->GetSelectedItem();

	SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	UI_ENSURE(pOwn);

	CUnionNetHelper::NetSend_c2r_union_appoint(m_unionData.m_nID, m_pSelectedUser->m_nPlayerID, m_vecChange[nIndex]);

	m_pFmAppoint->Hide();
}

void UIFormUnion::OnClickChangeCancel(RtwWidget* vpSender, RtwEventDelegate* e)	//点击取消
{
	m_pFmAppoint->Hide();
}
//end

//添加帮众
void UIFormUnion::AddMember()	//添加帮众
{
	m_ptxtAddText->SetText("");

	m_pFmAdd->ShowModal();
	m_pFmAdd->BringToTop();
}

void UIFormUnion::OnClickAddOk(RtwWidget* vpSender, RtwEventDelegate* e)		//点击ok
{
	string strName = m_ptxtAddText->getText();
	if (strName != "")
	{
		CUnionNetHelper::NetSend_c2r_union_join(m_unionData.m_nID, (char*)strName.c_str());
	}

	m_pFmAdd->Hide();
}

void UIFormUnion::OnClickAddCancel(RtwWidget* vpSender, RtwEventDelegate* e)	//点击取消
{
	m_pFmAdd->Hide();
}
//end

//脱离帮派
void UIFormUnion::LeaveUnion()	//脱离帮派
{
	// 
	// 	if (m_unionData.id==0)
	// 		return;
	// 
	// 	char tmp128[128];
	// 	snprintf(tmp128, 127, R(MSG_UNION_LEAVE_MAKESURE), m_unionData.name.c_str());
	//     tmp128[127] = 0;
	// 	UIFormMsg *pFrm = UIFormMsg::ShowStatic(tmp128, UIFormMsg::TYPE_OK_CANCEL);
	// 	if (GetWorld()->m_pPlayer->DBID() == m_unionData.captainID)
	// 	{
	// 		pFrm->EvOK = RTW_CALLBACK_1(this, UIFormUnion, OnAskInheritor,pFrm);
	// 	}
	// 	else
	// 	{
	// 		pFrm->EvOK = RTW_CALLBACK_1(this, UIFormUnion, OnLeave,pFrm);
	// 	}
	// 
	// 	if (!IsVisible())
	// 		return;
	// 
	// 	UIFormTextMsg *pFrm = (UIFormTextMsg *)e->GetUser1();
	// 	RtString name = pFrm->GetString();
	// 	SUnionCitizen* pCitizen = m_unionData.Find((char*)name);
	// 	if (!pCitizen)
	// 	{
	// 		PlayerFailOperate(R(MSG_UNION_NO_THIS_CITIZEN));
	// 		return;
	// 	}
	//     else if (pCitizen->playerID == GetWorld()->m_pPlayer->DBID())
	//     {
	//         PlayerFailOperate(R(MSG_UNION_CANNOT_USESELF));
	//         return;
	//     }
	// 	else
	// 	{
	// 		CUnionNetHelper::NetSend_c2r_union_leave(m_unionData.id, pCitizen->playerID);
	// 	}
	// 

	m_pFmLeave->ShowModal();
	m_pFmLeave->BringToTop();
}

void UIFormUnion::OnClickLeaveOk(RtwWidget* vpSender, RtwEventDelegate* e)		//点击ok
{
	CUnionNetHelper::NetSend_c2r_union_leave(m_unionData.m_nID, 0);
	m_pFmLeave->Hide();
}

void UIFormUnion::OnClickLeaveCancel(RtwWidget* vpSender, RtwEventDelegate* e)	//点击取消
{
	m_pFmLeave->Hide();
}
//end

//自我介绍
void UIFormUnion::OnClickSelfCancel(RtwWidget* vpSender, RtwEventDelegate* e)		//点击ok
{
	m_pFmSelfIntro->Hide();
}

void UIFormUnion::SelfIntro()
{
	m_pFmSelfIntro->ShowModal();
	m_pFmSelfIntro->BringToTop();
}
//end

//禅让
void UIFormUnion::Demise(RtwWidget* sender, void*)
{
	UI_ENSURE(m_pSelectedUser);

	// 	if (m_unionData.id==0)
	// 		return;
	// 
	// 	int iSel = m_listCitizen->GetSelectedItem();
	// 	if (iSel>=0 && iSel<m_unionData.arrCitizen.size())
	// 	{
	// 		// CHECK UP
	// 		// 不是自己
	//         SUnionCitizen *pOwn = m_unionData.Find(GetWorld()->m_pPlayer->DBID());
	//         CHECK(pOwn);
	//         if (m_unionData.arrCitizen[iSel].playerID == GetWorld()->m_pPlayer->DBID())
	//         {
	//             PlayerFailOperate(R(MSG_UNION_CANNOT_USESELF));
	//             return;
	//         }
	// 		else if (!Union_CanDismiss(pOwn, &(m_unionData.arrCitizen[iSel]) ) )
	//         {
	//             PlayerFailOperate(R(MSG_UNION_NO_RIGHTS));
	//             return;
	//         }
	// 
	// 		m_bkPlayerDismiss = m_unionData.arrCitizen[iSel].playerID;
	// 		char tmp128[128];
	// 		snprintf(tmp128, 127, R(MSG_UNION_DISMISS_MAKE_SURE), m_unionData.arrCitizen[iSel].name.c_str());
	//         tmp128[127] = 0;
	// 		UIFormMsg *pFrm = UIFormMsg::ShowStatic(tmp128, UIFormMsg::TYPE_OK_CANCEL);
	// 		pFrm->EvOK = RTW_CALLBACK_1(this, UIFormUnion, OnDismiss,pFrm);
	// 	}
	// 	else
	// 		PlayerFailOperate(R(MSG_UNION_SELECT_DISMISS_PLAYER));

	CUnionNetHelper::NetSend_c2r_union_disband(m_unionData.m_nID, m_pSelectedUser->m_nPlayerID);
}
//end

//驱逐
void UIFormUnion::Banishment(RtwWidget* sender, void*)
{
	UI_ENSURE(m_pSelectedUser);

	CUnionNetHelper::NetSend_c2r_union_dismiss_other(m_unionData.m_nID, m_pSelectedUser->m_nPlayerID);
}
//end

//解散帮派
void UIFormUnion::Disband(RtwWidget* sender, void*)
{
	CUnionNetHelper::NetSend_c2r_union_leave(m_unionData.m_nID, 0);
	m_pFrmFaction->Hide();
}

//发布公告
void UIFormUnion::OnClickBulletiOk(RtwWidget* vpSender, RtwEventDelegate* e)		//点击ok
{
	// 
	//     int currTime = rtGetMilliseconds();
	//     if ( abs(currTime - m_timeLastUpdateBulletin) < 10000 )
	//     {
	//         PlayerFailOperate(R(MSG_UNION_WAITFOR_NEXTSEND));
	//         return;
	//     }
	//     else 
	//     {
	//         m_timeLastUpdateBulletin = currTime;
	//     }
	// 
	//     const string& bulletin = m_pEditBulletin->GetText();
	//     if (bulletin.size() > UNION_BULLETIN_STRING_MAX)
	//     {
	//         char cTmp128[128];
	//         snprintf(cTmp128, 127, R(MSG_UNION_WORDS_OVERFLOW));//, 0, UNION_BULLETIN_STRING_MAX/2);
	//         cTmp128[127] = 0;
	//         PlayerFailOperate(cTmp128);
	//         return;
	//     }
	// 
	// 	CUnionNetHelper::NetSend_c2r_union_bulletin(m_unionData.id, (char*)m_pEditBulletin->GetText().c_str());

	//发送
	string strBulletin = m_pTxtBulletin->getText();
	if (strBulletin == "")
	{
		return;
	}

	CUnionNetHelper::NetSend_c2r_union_bulletin(m_unionData.m_nID, (char *)strBulletin.c_str());

	m_pFmBulletin->Hide();	
}

void UIFormUnion::OnClickBulletinCancel(RtwWidget* vpSender, RtwEventDelegate* e)	//点击取消
{
	m_pFmBulletin->Hide();	
}
//end

//查看权限
void UIFormUnion::OnClickLViewPowerCancel(RtwWidget* vpSender, RtwEventDelegate* e)	//点击ok/取消
{
	m_pFmViewPower->Hide();
}
//end

//贡献资源
void UIFormUnion::OnLClickMoneyOK(RtwWidget* vpSender, RtwEventDelegate* e)
{
	int nMoney = atol(m_pTxtMoney->getText().c_str());
	if (nMoney <= 0)
	{
		return;
	}

	if (GetWorld()->m_pPlayer->mItem.GetMoney() < nMoney)
	{
		//钱不够
		return;
	}

	CUnionNetHelper::NetSend_c2r_union_contribution(m_unionData.m_nID, nMoney);
	m_pFmMoney->Hide();
}

void UIFormUnion::OnLClickMoneyCancel(RtwWidget* vpSender, RtwEventDelegate* e)
{
	m_pTxtMoney->SetText("0");
	m_pFmMoney->Hide();
}

void UIFormUnion::OnInputChar(RtwWidget* vpSender, RtwEventDelegate* e)
{
	int nTmpMoney	= GetWorld()->m_pPlayer->mItem.m_Bag.GetMoney();
	int nInputMoney	= atoi(m_pTxtMoney->getText().c_str());
	if (nTmpMoney < nInputMoney)
	{
		char buf[16];
		rt2_snprintf(buf, 16, "%d", nTmpMoney);
		m_pTxtMoney->SetText(buf);
	}
}
//end	

//帮派图腾
void UIFormUnion::OnLClickTotemOK(RtwWidget* vpSender, RtwEventDelegate* e)
{
	if (nSelectTotemID!=0 && nSelectTotemID!=m_unionData.m_nPic)
	{
		char szBuf[256];
		rt2_snprintf(szBuf, 256, R(MSG_UNION_SHIYONGTUTENG), m_vecTotemList[nSelectTotemID-1].nPrice);
		UIFormMsg *pFrm = UIFormMsg::ShowStatic(szBuf, UIFormMsg::TYPE_OK_CANCEL);
		pFrm->EvOK = RTW_CALLBACK_1(this, UIFormUnion, UpdateTotem, pFrm);
	}
}

void UIFormUnion::UpdateTotem(RtwWidget* sender, void*)
{
	CUnionNetHelper::NetSend_c2r_union_totem(m_unionData.m_nID, nSelectTotemID);
	m_pFmTotem->Hide();
}

void UIFormUnion::OnLClickTotemCancel(RtwWidget* vpSender, RtwEventDelegate* e)
{
	m_pFmTotem->Hide();
}

void UIFormUnion::OnLClickTotemUpLoad(RtwWidget* vpSender, RtwEventDelegate* e)	//点击上传
{
	UIFormMsg::ShowStatic(R(MSG_CANNOTUPLOAD), UIFormMsg::TYPE_OK);
}

void UIFormUnion::RefreshTotem()
{
	m_pBtnTotemPic->SetBackgroundImage(GetTotemPicFromID(m_unionData.m_nPic));
	if (m_unionData.m_nPic<m_vecTotemList.size() && m_unionData.m_nPic>0)
	{
		m_pLblTotemPicInfo->SetText(m_vecTotemList[m_unionData.m_nPic-1].strPicDescription);
	}
	else
	{
		m_pLblTotemPicInfo->SetText("");
	}

	char buf[64];
	rt2_snprintf(buf, 64, "%d", m_unionData.dwMoney);
	m_pLblTotemPrice->SetText(buf);

	m_pFmTotem->Show();
}

void UIFormUnion::InitTotem()	//初始化图腾数据
{
	RtCsv csv;
	std::string stringID, strName, strDescption, strPrice;
	m_vecTotemList.clear();

	if(!csv.LoadTableFile("sv_table\\guild_pic.csv"))
	{
		return;
	}

	int nCount = csv.GetLineCount();
	char buf[1024];

	for(int i=1; i<nCount; ++i)
	{
		stringID		= csv[i][0].SafeStr();
		strName			= csv[i][1].SafeStr();
		strDescption	= csv[i][2].SafeStr();
		strPrice		= csv[i][3].SafeStr();

		TotemItem tmp;
		tmp.nPicID				= atol(stringID.c_str());
		tmp.nPrice				= atol(strPrice.c_str());
		tmp.strName				= strName;
		tmp.strPicDescription	= strDescption;

		rt2_snprintf(buf, 1024, "fmfactionicon.fmbackground.fmbtn%d", i);

		string tmpStr = buf;

		int *pData = new int;
		*pData = i;

		tmp.pWidget = LOAD_UI(tmpStr.c_str());
		tmp.pWidget->SetParam1(pData);
		tmp.pWidget->EvLClick += RTW_CALLBACK(this, UIFormUnion, OnSelectItem);

		m_vecTotemList.push_back(tmp);

		tmpStr = buf;
		tmpStr += ".text1"; //名字
		LOAD_UI(tmpStr.c_str())->SetParam1(pData);
		LOAD_UI(tmpStr.c_str())->EvLClick += RTW_CALLBACK(this, UIFormUnion, OnSelectItem);
		LOAD_UI(tmpStr.c_str())->SetText(strName);

		tmpStr = buf;
		tmpStr += ".text2"; //价格
		LOAD_UI(tmpStr.c_str())->SetParam1(pData);
		LOAD_UI(tmpStr.c_str())->EvLClick += RTW_CALLBACK(this, UIFormUnion, OnSelectItem);
		LOAD_UI(tmpStr.c_str())->SetText(strPrice);

		tmpStr = buf;
		tmpStr += ".fmicon.btnicon"; //图片
		LOAD_UI(tmpStr.c_str())->SetParam1(pData);
		LOAD_UI(tmpStr.c_str())->EvLClick += RTW_CALLBACK(this, UIFormUnion, OnSelectItem);
		LOAD_UI_T(RtwButton, tmpStr.c_str())->SetBackgroundImage(GetTotemPicFromID(tmp.nPicID));

	}
	m_pLblTotemHighlight->ModifyFlag(0, wfVisible);
}

RtwImage *UIFormUnion::GetTotemPicFromID(int id)
{
	if (id<1 || id>(int)m_vecTotemList.size())
	{
		return NULL;
	}
 
	char szFile[512];
	rt2_snprintf(szFile, 512, "ui\\x_textures\\Totem\\Totem_%d.tga", id);
	RtwImage *pImg = g_workspace.getImageFactory()->createImage(szFile);
	if (pImg)
	{
		pImg->SetBlend(true);
	}

	return pImg;
}

void UIFormUnion::OnSelectItem(RtwWidget* vpSender, RtwEventDelegate* e)
{
	int *pTmp = (int*)vpSender->getParam1();
	nSelectTotemID = *pTmp;

	TotemItem *pItem = &m_vecTotemList[nSelectTotemID-1];
	nSelectTotemID = pItem->nPicID;

	m_pLblTotemHighlight->ModifyFlag(wfVisible, 0);
	m_pLblTotemHighlight->Show();
	RtwRect rc = pItem->pWidget->GetFrameRect();
	m_pLblTotemHighlight->MoveResize(RtwRect(rc.left-2, rc.top-2, rc.right+2, rc.bottom+2), true);
	m_pBtnTotemPic->SetBackgroundImage(GetTotemPicFromID(nSelectTotemID));
	m_pLblTotemPicInfo->SetText(pItem->strPicDescription);
}
//end

//日志添加
void UIFormUnion::AddLog(DWORD type, DWORD nTime, const char *pMsg)
{
	UnionLogType eType = (UnionLogType)type;

	char szTime[64];

	time_t time = nTime;
	struct tm *ttime; 
	ttime = localtime(&time);
	strftime(szTime,64,"%Y-%m-%d %H:%M:%S",ttime); 

	char szLog[1024];
	memset(szLog, 0, 1024);

	stringstream strMsg(pMsg);
	vector<string> vecNodeStr;
	string strTmp;

	while (!strMsg.eof())
	{
		strMsg >> strTmp;
		vecNodeStr.push_back(strTmp);
	}

	switch (eType)
	{
	case eCreate:
		//创建帮派
		rt2_snprintf(szLog, 1024, R(MSG_UNION_CREATEUNION), vecNodeStr[0].c_str(), vecNodeStr[1].c_str());
		break;
	case eJobin:
		//加入帮派
		rt2_snprintf(szLog, 1024, R(MSG_UNION_JOINUNION), vecNodeStr[0].c_str());
		break;
	case eLeave:
		//脱离帮派
		rt2_snprintf(szLog, 1024, R(MSG_UNION_LEAVEUNION), vecNodeStr[0].c_str());
		break;
	case eBanish:
		//被驱逐出帮派
		rt2_snprintf(szLog, 1024, R(MSG_UNION_QUZHUCHUBANGPAI), vecNodeStr[0].c_str(), vecNodeStr[1].c_str());
		break;
	case eJobUp:
		//职位提升
		rt2_snprintf(szLog, 1024, R(MSG_UNION_SHENGZHI), vecNodeStr[0].c_str(), vecNodeStr[1].c_str(), 
			GetUOString((EUnionOfficial)atoi(vecNodeStr[2].c_str())), GetUOString((EUnionOfficial)atoi(vecNodeStr[3].c_str())));
		break;
	case eJobDown:
		//职位下降
		rt2_snprintf(szLog, 1024, R(MSG_UNION_BIANZHI), vecNodeStr[0].c_str(), vecNodeStr[1].c_str(), 
			GetUOString((EUnionOfficial)atoi(vecNodeStr[2].c_str())), GetUOString((EUnionOfficial)atoi(vecNodeStr[3].c_str())));
		break;
	case eGiveCaption:
		//禅让帮主
		rt2_snprintf(szLog, 1024, R(MSG_UNION_BANGZHUCHUANWEI), vecNodeStr[0].c_str(), vecNodeStr[1].c_str());
		break;
	case eChangeBulletin:
		//修改公告
		rt2_snprintf(szLog, 1024, R(MSG_UNION_GENGGAIGONGGAO), vecNodeStr[0].c_str());
		break;
	case eGetMoney:
		//取帮派金钱
		rt2_snprintf(szLog, 1024, R(MSG_UNION_QUCHUYINBI), GetUOString((EUnionOfficial)atoi(vecNodeStr[0].c_str())), vecNodeStr[1].c_str(), vecNodeStr[2].c_str());
		break;
	case eDoContribution:
		//帮贡
		rt2_snprintf(szLog, 1024, R(MSG_UNION_GONGXIANYINBI), vecNodeStr[0].c_str(), vecNodeStr[1].c_str());
		break;

	case UnionLogTypeNum:
	default:
		return;
	}

	if (strlen(szLog) > 0)
	{
		m_pLstHistory->Add(1, 0xffffffff, "", szTime, szLog, 0, false);
	}
}
//end

//刷新帮贡
void UIFormUnion::UpdateContribution(DWORD nPlayerID, DWORD nContribution)
{
	SUnionCitizen *pPlayer = m_unionData.Find(nPlayerID);
	UI_ENSURE(pPlayer);

	pPlayer->m_nContribution = nContribution;
	RefreshUserList();
}
//end

//刷新资金
void UIFormUnion::UpdateMoneyItem(DWORD nMoney, DWORD nItem1, DWORD nItem2, DWORD nItem3, DWORD nItem4)
{
	m_unionData.dwMoney		= nMoney;
	m_unionData.m_Item[0]	= nItem1;
	m_unionData.m_Item[1]	= nItem2;
	m_unionData.m_Item[2]	= nItem3;
	m_unionData.m_Item[3]	= nItem4;

	Refresh();
}
//end

//刷新战斗力
void UIFormUnion::UpdateWarScores(DWORD nWarScores)
{
	m_unionData.m_nStrength = nWarScores;

	Refresh();
}
//end

//刷新新的图腾
void UIFormUnion::UpdateIcon(DWORD nIconID, DWORD nMoney)
{
	m_unionData.m_nPic = nIconID;
	m_unionData.dwMoney = nMoney;

	Refresh();
}
//end

void UIFormUnion::SetUnionButtonNormal()
{
	guard;
	if(m_bShowAnimation)
	{
		LOAD_UI_T(RtwButton,"fmsystem.btnbangpai")->SetNormalImage(m_imgUnionNormal);
		m_bShowAnimation = false;
	}
	unguard;
}

void UIFormUnion::SetUnionButtonAnimation()
{
	guard;
	if(!m_bShowAnimation && !m_pFrmFaction->GetFlags(wfVisible))
	{
		LOAD_UI_T(RtwButton,"fmsystem.btnbangpai")->SetNormalImage(m_imgUnionAnimation);
		m_bShowAnimation = true;
	}
	unguard;
}
