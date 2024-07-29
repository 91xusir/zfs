//********************************************************************
//	created:	2010.05.31
//	filename: 	ui_form_union_create.cpp
//	author:		ldr123
//	purpose:	帮派创建类
//  other:		
//********************************************************************

#include "gc_include.h"
#include "union_base.h"
#include "net_command_union.h"
#include "ui_form_playeritems.h"
#include "ui_form_item_update.h"
#include "ui_form_shop.h"
#include "ui_form_lookplayershop.h"
#include "ui_form_trade.h"
#include "ui_form_bank.h"
#include "ui_form_compose.h"
#include "ui_form_fuse.h"
#include "UIForm_Gem.h"
#include "ui_form_decompound.h"
#include "ui_form_union_create.h"
#include "ui_form_msg.h"
#include "ui_form_char.h"

UIFormUnionCreate::UIFormUnionCreate()
{
	guard;

	m_pFrmThis		= LOAD_UI("fmcreateunion");
	m_pUnionName	=	LOAD_UI_T(RtwTextBox, "fmcreateunion.edtunionname");	
	m_pBtnOK		= LOAD_UI_T(RtwButton, "fmcreateunion.btnok");
	m_pBtnCancel	= LOAD_UI_T(RtwButton, "fmcreateunion.btnclose");

	m_pUnionName->SetCapacity(UNION_NAME_STRING_MAX);

	// 注册本窗口中的事件
	m_pBtnOK->EvLClick		+= RTW_CALLBACK(this, UIFormUnionCreate, OnBtnOK);
	m_pBtnCancel->EvLClick	+= RTW_CALLBACK(this, UIFormUnionCreate, OnBtnCancel);

	m_pFrmThis->Hide();

	unguard;
}

UIFormUnionCreate::~UIFormUnionCreate()
{
	guard;
	unguard;
}

void UIFormUnionCreate::OnPreDraw()
{
	guard;

    //const DWORD YES_COLOR = 0xFFFFFFFF;
    //const DWORD NO_COLOR = 0xFFFF0000;

    //CItemContainer& con = GetWorld()->m_pPlayer->mItem;
    //if (con.GetMoney() < UNION_CREATE_USE_MONEY)
    //{
    //    m_lbMoneyNeed->SetTextColor(NO_COLOR);
    //}
    //else 
    //{
    //    m_lbMoneyNeed->SetTextColor(YES_COLOR);
    //}

    ////if (!con.m_Bag.FindType(UNION_CREATE_USE_ITEMID) && !con.m_MissionBag.FindType(UNION_CREATE_USE_ITEMID))
    ////{
    ////    m_lbItemNeed->SetTextColor(NO_COLOR);
    ////}
    ////else 
    ////{
    ////    m_lbItemNeed->SetTextColor(YES_COLOR);
    ////}

    //if (GetWorld()->m_pPlayer->m_core.Lev < UNION_CREATE_USE_LEVEL)
    //{
    //    m_lbLevelNeed->SetTextColor(NO_COLOR);
    //}
    //else 
    //{
    //    m_lbLevelNeed->SetTextColor(YES_COLOR);
    //}

	unguard;
}

void UIFormUnionCreate::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormUnionCreate::Show(DWORD npcID)
{
	guard;

	if (g_layerMain)
	{
		if (g_layerMain->m_fromPlayerItems->IsShowing())	// 道具栏
			g_layerMain->m_fromPlayerItems->Hide();
		if (g_layerMain->m_formItemUpdate->IsVisible())		// 道具打造
			g_layerMain->m_formItemUpdate->Hide();
		if (g_layerMain->m_formGem->IsVisible())		// 宝石合成
			g_layerMain->m_formGem->Hide();
		if (g_layerMain->m_formShop->IsVisible())			// NPC商店
			g_layerMain->m_formShop->Hide();
		if (g_layerMain->m_formLookPlayerShop->IsVisible())		// 察看摆摊
			g_layerMain->m_formLookPlayerShop->Hide();
		if (g_layerMain->m_formTrade->IsShowing())			// 交易
			g_layerMain->m_formTrade->Hide();
		if (g_layerMain->m_formBank->IsVisible())			// 仓库
			g_layerMain->m_formBank->Hide();
		if (g_layerMain->m_formItemCompose->IsVisible())	// 配方合成
			g_layerMain->m_formItemCompose->Hide();
		if (g_layerMain->m_formItemFuse->IsVisible())		// 道具融合
			g_layerMain->m_formItemFuse->Hide();
		if (g_layerMain->m_formItemDecompound->IsVisible())
			g_layerMain->m_formItemDecompound->Hide();
		if (g_layerMain->m_formRecoverPet->GetVisible())
			g_layerMain->m_formRecoverPet->SetVisible(false);
	}

	m_npcID = npcID;

	OnPreDraw();
	m_pFrmThis->Show();

	unguard;
}

bool UIFormUnionCreate::IsVisible()
{
	guard;
	if (m_pFrmThis && m_pFrmThis->GetFlags(wfVisible))
		return true;
	else	
		return false;
	unguard;
}

void UIFormUnionCreate::Hide()
{
	guard;
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormUnionCreate::OnBtnOK(RtwWidget* sender, void*)
{
	guard;

	const string& name = m_pUnionName->GetText();
    char cTmp256[256];
	
	if (name.empty())
	{
		//名字为空
		PlayerFailOperate(R(MSG_UNION_UNIONNAME_IS_NULL));
		return;
	}

    if (name.size() > UNION_NAME_STRING_MAX || name.size() < 4)
    {
		//名字长度越界
        rt2_snprintf(cTmp256, 256, R(MSG_UNION_NAME_WORDS_OVERFLOW));//, UNION_NAME_STRING_MAX);
        //cTmp256[255] = 0;
        PlayerFailOperate(cTmp256);
		return;
    }

    //if (enounce.size() > UNION_ENOUNCE_STRING_MAX)
    //{
    //    snprintf(cTmp256, 255, R(MSG_UNION_ENOUNCE_WORDS_OVERFLOW));//, UNION_ENOUNCE_STRING_MAX);
    //    cTmp256[255] = 0;
    //    PlayerFailOperate(cTmp256);
    //    return;
    //}

	CItemContainer& con = GetWorld()->m_pPlayer->mItem;
    //if (!con.m_Bag.FindType(UNION_CREATE_USE_ITEMID) && !con.m_MissionBag.FindType(UNION_CREATE_USE_ITEMID))
    //{
    //    PlayerFailOperate(R(MSG_UNION_CREATE_UNMEASURE));
    //    return;
    //}

    if (GetWorld()->m_pPlayer->m_core.Lev < UNION_CREATE_USE_LEVEL)
    {
		//用户等级不够
		PlayerFailOperate(R(MSG_UNION_CREATE_UNMEASURE));
		goto exit1;
    }

    if (con.GetMoney() < UNION_CREATE_USE_MONEY)
    {
		//钱不够
        PlayerFailOperate(R(MSG_UNION_CREATE_UNMEASURE));
		goto exit1;
    }

	CUnionNetHelper::NetSend_c2r_union_create(m_npcID, (char*)name.c_str(), "");
	g_layerMain->m_fromPlayerItems->LockOperate(true);

exit1:
	m_pUnionName->SetText("");
	m_pFrmThis->Hide();
	unguard;
}

void UIFormUnionCreate::OnBtnCancel(RtwWidget* sender, void*)
{
	guard;
	m_pUnionName->SetText("");
	Hide();
	unguard;
}
