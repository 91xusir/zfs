#include "gc_include.h"
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

UIFormItemUpdate::UIFormItemUpdate()
{
	guard;

	m_pFrmThis = RTW_WIDGET("fmcombine");
	m_pFrmThis->ModifyFlags(wfDragClient, 0);
	m_pFrmThis->EvFocus		+= RTW_CALLBACK(this,UIFormItemUpdate,OnGetFocus);
	m_pFrmThis->EvHide		+= RTW_CALLBACK(this,UIFormItemUpdate,OnHide);
	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);

	m_pBtnItem = RTW_WIDGET("fmcombine.fmbaby.aliasCombweapon");
	m_pBtnScroll = RTW_WIDGET("fmcombine.fmbaby.aliasCombscroll");
	m_pBtnLuckStone = RTW_WIDGET("fmcombine.fmbaby.aliasCombadd");

	m_pBtnOK = RTW_WIDGET_T(RtwButton, "fmcombine.btnok");
	m_pBtnCancel = RTW_WIDGET_T(RtwButton, "fmcombine.btncancel");

	m_ptrHoverImage	= g_workspace.getImageFactory()->createImage("ui/textures/highlight_button.tga");
	m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
	m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
	m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
	m_ptrHoverImage->getRenderImage()->SetBlend(true);

	m_pBtnItem->ModifyFlags(wfDragClient | wfDragServer|wfDragIn|wfDragOut, 0);
	m_pBtnItem->EvDrag				+= RTW_CALLBACK(this, UIFormItemUpdate, OnDragToItem);
	m_pBtnItem->EvDragEnd			+= RTW_CALLBACK(this, UIFormItemUpdate, OnDragEndItem);
	m_pBtnItem->EvMouseRDown		+= RTW_CALLBACK(this, UIFormItemUpdate, OnMouseRDown);
	m_pBtnItem->EvHint				+= RTW_CALLBACK(this, UIFormItemUpdate, OnGetHintText);
	((RtwButton*)m_pBtnItem)->SetHoverImage(m_ptrHoverImage);
	m_pBtnScroll->ModifyFlags(wfDragClient | wfDragServer|wfDragIn, 0);
	m_pBtnScroll->EvDrag			+= RTW_CALLBACK(this, UIFormItemUpdate, OnDragToScroll);
	m_pBtnScroll->EvDragEnd			+= RTW_CALLBACK(this, UIFormItemUpdate, OnDragEndScroll);
	m_pBtnScroll->EvMouseRDown		+= RTW_CALLBACK(this, UIFormItemUpdate, OnMouseRDown);
	m_pBtnScroll->EvHint			+= RTW_CALLBACK(this, UIFormItemUpdate, OnGetHintText);
	((RtwButton*)m_pBtnScroll)->SetHoverImage(m_ptrHoverImage);
	m_pBtnLuckStone->ModifyFlags(wfDragClient | wfDragServer|wfDragIn, 0);
	m_pBtnLuckStone->EvDrag			+= RTW_CALLBACK(this, UIFormItemUpdate, OnDragToLuckStone);
	m_pBtnLuckStone->EvDragEnd		+= RTW_CALLBACK(this, UIFormItemUpdate, OnDragEndLuckStone);
	m_pBtnLuckStone->EvMouseRDown	+= RTW_CALLBACK(this, UIFormItemUpdate, OnMouseRDown);
	m_pBtnLuckStone->EvHint			+= RTW_CALLBACK(this, UIFormItemUpdate, OnGetHintText);
	((RtwButton*)m_pBtnLuckStone)->SetHoverImage(m_ptrHoverImage);

    m_ReqMoney = RTW_WIDGET("fmcombine.lbmoney");

	// 注册本窗口中的事件
	m_pBtnOK->EvLClick += RTW_CALLBACK(this, UIFormItemUpdate, OnBtnOK);
	m_pBtnCancel->EvLClick += RTW_CALLBACK(this, UIFormItemUpdate, OnBtnCancel);

	m_pFrmThis->Hide();
	m_pageItem = m_iItem = m_jItem = -1;
	m_pageScroll = m_iScroll = m_jScroll = -1;
	m_pageLuckStone = m_iLuckStone = m_jLuckStone = -1;

	unguard;
}

UIFormItemUpdate::~UIFormItemUpdate()
{
	guard;
	unguard;
}

void UIFormItemUpdate::OnPreDraw()
{
	guard;

	SItemBase *pItemClass = NULL;
	if (ItemID_IsValid(m_UpdateItemID))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_UpdateItemID.type);
		if(!pItemClass)			return;
		//m_pBtnItem->SetBackgroundImage( new RtwImage(pItemClass->icon) );// change [3/17/2009 tooth.shi]
		m_pBtnItem->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));
	}
	else 
	{
		m_pBtnItem->SetBackgroundImage((RtwImage*)NULL);
	}

	if (ItemID_IsValid(m_ScrollID))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_ScrollID.type);
		if(!pItemClass)			return;
		//m_pBtnScroll->SetBackgroundImage( new RtwImage(pItemClass->icon) );// change [3/17/2009 tooth.shi]
		m_pBtnScroll->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));
	}
	else
	{
		m_pBtnScroll->SetBackgroundImage((RtwImage*)NULL);
	}

	if (ItemID_IsValid(m_LuckStoneID))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_LuckStoneID.type);
		if(!pItemClass)			return;
		//m_pBtnLuckStone->SetBackgroundImage( new RtwImage(pItemClass->icon) );// change [3/17/2009 tooth.shi]
		m_pBtnLuckStone->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));
	}
	else
	{
		m_pBtnLuckStone->SetBackgroundImage((RtwImage*)NULL);
	}

	int nReqCounter = 0;
	if (ItemID_IsValid(m_UpdateItemID))
	{
		if (m_UpdateItemID.level>=0 && m_UpdateItemID.level<ITEM_MAX_ITEM_LEVEL)
		{
			SItemID itemNextLev = m_UpdateItemID;
			itemNextLev.level++;

			GcActor* pPlayer = GetWorld()->m_pPlayer;

			const DWORD YES_COLOR = 0xFFFFFFFF;
			const DWORD NO_COLOR = 0xFFFF0000;

            int moneyRequired = 0;
            ItemUpdate_GetMoneyRequirement(m_UpdateItemID, moneyRequired, GetWorld()->m_pItemManager);
            m_ReqMoney->SetText(rtFormatNumber(moneyRequired));
            if (pPlayer->mItem.GetMoney() < moneyRequired)
                m_ReqMoney->SetTextColor(RtwPixel(NO_COLOR));
            else 
                m_ReqMoney->SetTextColor(RtwPixel(YES_COLOR));

		}
	}
    else 
    {
        m_ReqMoney->SetText("");
    }

	m_pBtnOK->Enable();

	unguard;
}

void UIFormItemUpdate::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormItemUpdate::Show()
{
	guard;

	if ( IsVisible() )		return;

	if (g_layerMain)
	{
//		if (g_layerMain->m_fromPlayerItems->IsShowing())	// 道具栏
//			g_layerMain->m_fromPlayerItems->Hide();
//		if (g_layerMain->m_formItemUpdate->IsVisible())		// 道具打造
//			g_layerMain->m_formItemUpdate->Hide();
		if (g_layerMain->m_formGem->IsVisible())		// 宝石合成
			g_layerMain->m_formGem->Hide();
		if (g_layerMain->m_formShop->IsVisible())			// NPC商店
			g_layerMain->m_formShop->Hide();
		//if (g_layerMain->m_formPShop->IsVisible())			// 摆摊
		//	g_layerMain->m_formPShop->Hide();
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
		if (g_layerMain->m_formUnionCreate->IsVisible())	// 创建诸侯国
			g_layerMain->m_formUnionCreate->Hide();
		if (g_layerMain->m_formRecoverPet->GetVisible())
			g_layerMain->m_formRecoverPet->SetVisible(false);
	}

	m_UpdateItemID = ItemID_CreateInvalid();
	m_ScrollID = ItemID_CreateInvalid();
	m_LuckStoneID = ItemID_CreateInvalid();

	OnPreDraw();
	m_pFrmThis->Show();
	unguard;
}

bool UIFormItemUpdate::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormItemUpdate::Hide()
{
	guard;
	g_layerMain->m_fromPlayerItems->OnDisableAllItemMask();
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormItemUpdate::OnBtnOK(RtwWidget* sender, void*)
{
	guard;

	//if ( g_layerMain->m_formPShop->IsVisible() )
	//{
	//	PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_PSHOP));
	//	
	//	return;
	//}
	if ( g_layerMain->m_formTrade->IsShowing() )
	{
		PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_TRADE));
		return;
	}
	
	if (ItemID_IsValid(m_UpdateItemID) && ItemID_IsValid(m_ScrollID))
	{
		if (m_UpdateItemID.level >= ITEM_MAX_ITEM_LEVEL)
		{
			PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_REACHMAX));
            return;
		}
        else if (m_UpdateItemID.level < 0)
        {
            CHECKEX("[UIFormItemUpdate::OnBtnOK] 道具打造等级不合法");
        }

        int moneyRequired = 0;
        ItemUpdate_GetMoneyRequirement(m_UpdateItemID, moneyRequired, GetWorld()->m_pItemManager);
        if (GetWorld()->m_pPlayer->mItem.GetMoney() < moneyRequired)
        {
            PlayerFailOperate(R(MSG_ITEM_NOT_ENOUGH_MONEY));
            return;
        }

		CBag *pBag = &(GetWorld()->m_pPlayer->mItem.m_Bag);
		SItemID item1 = pBag->GetItem(m_pageItem, m_iItem, m_jItem);
		SItemID item2 = pBag->GetItem(m_pageScroll, m_iScroll, m_jScroll);
		if (item1.id != m_UpdateItemID.id)
		{
			if (!SeekUpdateItemPos())
			{
				PlayerFailOperate(R(MSG_ITEMUPDATE_REDRAG_ICON));
				ItemID_SetInvalid(m_UpdateItemID);
				Refresh();
				return;
			}
		}
		if (item2.id != m_ScrollID.id)
		{
			if (!SeekScrollPos())
			{
				PlayerFailOperate(R(MSG_ITEMUPDATE_REDRAG_ICON));
				ItemID_SetInvalid(m_ScrollID);
				Refresh();
				return;
			}
		}
		
		if (ItemID_IsValid(m_LuckStoneID))
		{
			SItemID item3 = pBag->GetItem(m_pageLuckStone, m_iLuckStone, m_jLuckStone);
			if (item3.id != m_LuckStoneID.id)
			{
				if (!SeekLuckStonePos())
				{
					PlayerFailOperate(R(MSG_ITEMUPDATE_REDRAG_ICON));
					ItemID_SetInvalid(m_LuckStoneID);
					Refresh();
					return;
				}
			}

			// 判断道具和卷轴是否匹配
            if (ItemIsGemLuck6Only(m_LuckStoneID.type))
            {
                if (item1.level != 5)
                {
                    PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_SCROLL_NOTPAIR));
                    return;
                }
            }
            else if (ItemIsGemLuck7Only(m_LuckStoneID.type))
            {
                if (item1.level < 6 || item1.level > 8)
                {
                    PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_SCROLL_NOTPAIR));
                    return;
                }
            }
            else 
            {
                if (m_UpdateItemID.level>=0 && m_UpdateItemID.level<=2)
                {
                    PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_SCROLL_NOTPAIR));
                    return;
                }
                else if (m_UpdateItemID.level>=3 && m_UpdateItemID.level<=5)
                {
                    if (!ItemIsGemLuck2(m_LuckStoneID.type) && !ItemIsGemAntiBroken(m_LuckStoneID.type))
                    {
                        PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_SCROLL_NOTPAIR));
                        return;
                    }
                }
                else if (m_UpdateItemID.level>=6 && m_UpdateItemID.level<=8)
                {
                    if (!ItemIsGemLuck3(m_LuckStoneID.type) && !ItemIsGemAntiBroken(m_LuckStoneID.type))
                    {
                        PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_SCROLL_NOTPAIR));
                        return;
                    }
                }
                else if (m_UpdateItemID.level==9)
                {
                    PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_SCROLL_NOTPAIR));
                    return;
                }
                else 
                {
                    PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_SCROLL_NOTPAIR));
                    return;
                }
            }
		}

		// 判断道具和符咒是否匹配(盾牌使用防御符咒)
		if (ItemIsWeapon(m_UpdateItemID) && !ItemIsShield(m_UpdateItemID))
		{
			if (m_UpdateItemID.level>=0 && m_UpdateItemID.level<=2)
			{
				if (!ItemIsGemWeapon1(m_ScrollID))
				{
					PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_NOTPAIR));
					return;
				}
			}
			else if (m_UpdateItemID.level>=3 && m_UpdateItemID.level<=5)
			{
				if (!ItemIsGemWeapon2(m_ScrollID))
				{
					PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_NOTPAIR));
					return;
				}
			}
			else if (m_UpdateItemID.level>=6 && m_UpdateItemID.level<=8)
			{
				if (!ItemIsGemWeapon3(m_ScrollID))
				{
					PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_NOTPAIR));
					return;
				}
			}
			else if (m_UpdateItemID.level==9)
			{
				PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_NOTPAIR));
				return;
			}
			else 
			{
				PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_NOTPAIR));
				return;
			}
		}
		else if (ItemIsArmor(m_UpdateItemID) || ItemIsShield(m_UpdateItemID))
		{
			if (m_UpdateItemID.level>=0 && m_UpdateItemID.level<=2)
			{
				if (!ItemIsGemArmor1(m_ScrollID))
				{
					PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_NOTPAIR));
					return;
				}
			}
			else if (m_UpdateItemID.level>=3 && m_UpdateItemID.level<=5)
			{
				if (!ItemIsGemArmor2(m_ScrollID))
				{
					PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_NOTPAIR));
					return;
				}
			}
			else if (m_UpdateItemID.level>=6 && m_UpdateItemID.level<=8)
			{
				if (!ItemIsGemArmor3(m_ScrollID))
				{
					PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_NOTPAIR));
					return;
				}
			}
			else if (m_UpdateItemID.level==9)
			{
				PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_NOTPAIR));
				return;
			}
			else 
			{
				PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_NOTPAIR));
				return;
			}
		}

		int pageL=-1, iL=-1, jL=-1;
		if (ItemID_IsValid(m_LuckStoneID))
		{
			pageL = m_pageLuckStone;
			iL = m_iLuckStone;
			jL = m_jLuckStone;
		}
		CItemNetHelper::NetSend_c2r_player_itemupdate(m_pageItem, m_iItem, m_jItem, m_pageScroll, m_iScroll, m_jScroll, pageL, iL, jL);
		m_UpdateItemID = ItemID_CreateInvalid();
		m_ScrollID = ItemID_CreateInvalid();
		m_LuckStoneID = ItemID_CreateInvalid();
		g_layerMain->m_fromPlayerItems->OnDisableAllItemMask();
		m_pBtnOK->Disable();
	}
	else 
	{
		PlayerFailOperate(R(MSG_ITEMUPDATE_FAIL_PUTIN));
	}

	unguard;
}

void UIFormItemUpdate::OnBtnCancel(RtwWidget* sender, void*)
{
	guard;
	Hide();
	unguard;
}

void UIFormItemUpdate::OnDragToItem(RtwWidget* sender, void*)
{
	guard;

	if(ItemID_IsValid(m_UpdateItemID))
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_UpdateItemID);
	}
	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	const string& WidgetID = pDragSvr->GetID();

	// 是不是道具栏的格子
	int i,j;
	if (g_layerMain->m_fromPlayerItems && g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	{
		CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
		SItemID item = pCon->m_Bag.GetItem(i, j);
		if (ItemID_IsValid(item))
		{
			if(ItemCanUpdate(item.type) && !ItemIsTrump(item.type))
			{
				m_pBtnItem->ModifyFlags(wfHint, 0);
				m_UpdateItemID = item;
				m_pageItem = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
				m_iItem = i;
				m_jItem = j;
				g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageItem,m_iItem,m_jItem,true,1.0f,0xb5000000,128,true);
				g_workspace.ResetHintText();
				Refresh();
			}
			else 
			{
				PlayerFailOperate(R(MSG_ITEM_CAN_NOT_ITEMUPDATE));
			}
		}	
	}

	unguard;
}
bool UIFormItemUpdate::OnBagToItem(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	if(ItemID_IsValid(m_UpdateItemID))
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_UpdateItemID);
	}
	// 是不是道具栏的格子
	int i,j;
	i = e->mouse.x;
	j = e->mouse.y;
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemID item = pCon->m_Bag.GetItem(i, j);
	if (ItemID_IsValid(item))
	{
		if(ItemCanUpdate(item.type) && !ItemIsTrump(item.type))
		{
			m_pBtnItem->ModifyFlags(wfHint, 0);
			m_UpdateItemID = item;
			m_pageItem = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
			m_iItem = i;
			m_jItem = j;
			g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageItem,m_iItem,m_jItem,true,1.0f,0xb5000000,128,true);
			g_workspace.ResetHintText();
			Refresh();
			return true;
		}
		else 
		{
			if (!ItemIsGem(item.type))
			{
				PlayerFailOperate(R(MSG_ITEM_CAN_NOT_ITEMUPDATE));
			}
			
		}
	}
	return false;
	unguard;
}

void UIFormItemUpdate::OnDragToScroll(RtwWidget* sender, void*)
{
	guard;
	if(ItemID_IsValid(m_UpdateItemID))
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_ScrollID);
	}
	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	const string& WidgetID = pDragSvr->GetID();

	// 是不是道具栏的格子
	int i,j;
	if (g_layerMain->m_fromPlayerItems && g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	{
		CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
		SItemID item = pCon->m_Bag.GetItem(i, j);
		if (ItemIsGemWeapon(item) || ItemIsGemArmor(item))
		{
			m_pBtnScroll->ModifyFlags(wfHint, 0);
			m_ScrollID = item;
			m_pageScroll = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
			m_iScroll = i;
			m_jScroll = j;
			g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageItem,m_iItem,m_jItem,true,1.0f,0xb5000000,128,true);
			g_workspace.ResetHintText();
			Refresh();
		}
	}

	unguard;
}
bool UIFormItemUpdate::OnBagToScroll(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	if(ItemID_IsValid(m_UpdateItemID))
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_ScrollID);
	}
	// 是不是道具栏的格子
	int i,j;
	i = e->mouse.x;
	j = e->mouse.y;
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemID item = pCon->m_Bag.GetItem(i, j);
	if (ItemIsGemWeapon(item) || ItemIsGemArmor(item))
	{
		m_pBtnScroll->ModifyFlags(wfHint, 0);
		m_ScrollID = item;
		m_pageScroll = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
		m_iScroll = i;
		m_jScroll = j;
		g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageItem,m_iItem,m_jItem,true,1.0f,0xb5000000,128,true);
		g_workspace.ResetHintText();
		Refresh();
		return true;
	}
	return false;
	unguard;
}

void UIFormItemUpdate::OnDragToLuckStone(RtwWidget* sender, void*)
{
	guard;
	if(ItemID_IsValid(m_UpdateItemID))
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_LuckStoneID);
	}
	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	const string& WidgetID = pDragSvr->GetID();

	// 是不是道具栏的格子
	int i,j;
	if (g_layerMain->m_fromPlayerItems && g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	{
		CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
		SItemID item = pCon->m_Bag.GetItem(i, j);

		if (ItemID_IsValid(item) && (ItemIsGemLuck2(item.type) || ItemIsGemLuck3(item.type) || ItemIsGemAntiBroken(item.type) || ItemIsGemLuck6Only(item.type) || ItemIsGemLuck7Only(item.type)) )
		{
			m_pBtnLuckStone->ModifyFlags(wfHint, 0);
			m_LuckStoneID = item;
			m_pageLuckStone = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
			m_iLuckStone = i;
			m_jLuckStone = j;
			g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageLuckStone,m_iLuckStone,m_jLuckStone,true,1.0f,0xb5000000,128,true);
			g_workspace.ResetHintText();
			Refresh();
		}
	}

	unguard;
}
bool UIFormItemUpdate::OnBagToLuckStone(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	if(ItemID_IsValid(m_UpdateItemID))
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_LuckStoneID);
	}
	// 是不是道具栏的格子
	int i,j;
	i = e->mouse.x;
	j = e->mouse.y;
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemID item = pCon->m_Bag.GetItem(i, j);

	if (ItemID_IsValid(item) && (ItemIsGemLuck2(item.type) || ItemIsGemLuck3(item.type) || ItemIsGemAntiBroken(item.type) || ItemIsGemLuck6Only(item.type) || ItemIsGemLuck7Only(item.type)) )
	{
		m_pBtnLuckStone->ModifyFlags(wfHint, 0);
		m_LuckStoneID = item;
		m_pageLuckStone = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
		m_iLuckStone = i;
		m_jLuckStone = j;
		g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageLuckStone,m_iLuckStone,m_jLuckStone,true,1.0f,0xb5000000,128,true);
		g_workspace.ResetHintText();
		Refresh();
		return true;
	}
	return false;
	unguard;
}

void UIFormItemUpdate::OnDragEndItem(RtwWidget* sender, void*)
{
	guard;
	g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_UpdateItemID);
	RtwWidget* pClient = g_workspace.GetDragClient();
	if ( !pClient )// 把图标拖到空白处
	{
		m_UpdateItemID = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);
		Refresh();
	}
	else
	{
		int i,j;
		if (g_layerMain->m_fromPlayerItems->Is_Widget_Common(pClient->GetID(), i, j) || 
			g_layerMain->m_fromPlayerItems->Is_Widget_Mission(pClient->GetID(), i, j))
		{
			m_UpdateItemID = ItemID_CreateInvalid();
			sender->ModifyFlags(0, wfHint);

			g_workspace.ResetHintText();
			Refresh();
		}
		else
		{
			g_workspace.SetDragClient(NULL);
		}
	}

	unguard;
}

void UIFormItemUpdate::OnDragEndScroll(RtwWidget* sender, void*)
{
	guard;
	g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_ScrollID);
	RtwWidget* pClient = g_workspace.GetDragClient();
	if ( !pClient )// 把图标拖到空白处
	{
		m_ScrollID = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);

		g_workspace.ResetHintText();
		Refresh();
	}
	else
	{
		int i,j;
		if (g_layerMain->m_fromPlayerItems->Is_Widget_Common(pClient->GetID(), i, j) || 
			g_layerMain->m_fromPlayerItems->Is_Widget_Mission(pClient->GetID(), i, j))
		{
			m_ScrollID = ItemID_CreateInvalid();
			sender->ModifyFlags(0, wfHint);

			g_workspace.ResetHintText();
			Refresh();
		}
		else
		{
			g_workspace.SetDragClient(NULL);
		}
	}

	unguard;
}

void UIFormItemUpdate::OnDragEndLuckStone(RtwWidget* sender, void*)
{
	guard;
	g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_LuckStoneID);
	RtwWidget* pClient = g_workspace.GetDragClient();
	if ( !pClient )// 把图标拖到空白处
	{
		m_LuckStoneID = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);

		g_workspace.ResetHintText();
		Refresh();
	}
	else
	{
		int i,j;
		if (g_layerMain->m_fromPlayerItems->Is_Widget_Common(pClient->GetID(), i, j) || 
			g_layerMain->m_fromPlayerItems->Is_Widget_Mission(pClient->GetID(), i, j))
		{
			m_LuckStoneID = ItemID_CreateInvalid();
			sender->ModifyFlags(0, wfHint);

			g_workspace.ResetHintText();
			Refresh();
		}
		else
		{
			g_workspace.SetDragClient(NULL);
		}
	}

	unguard;
}
void UIFormItemUpdate::OnMouseRDown(RtwWidget* sender, void*)
{
	guard;
	if(m_pBtnItem == sender)
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_UpdateItemID);
		m_UpdateItemID = ItemID_CreateInvalid();
		m_iItem = -1;
		m_jItem = -1;
		m_pageItem = -1;
	}else if(m_pBtnScroll == sender)
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_ScrollID);
		m_ScrollID = ItemID_CreateInvalid();
		m_iScroll = -1;
		m_jScroll = -1;
		m_pageScroll = -1;
	}else if(m_pBtnLuckStone == sender)
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_LuckStoneID);
		m_LuckStoneID = ItemID_CreateInvalid();
		m_iLuckStone = -1;
		m_jLuckStone = -1;
		m_pageLuckStone = -1;
	}else
		return;
	sender->ModifyFlags(0, wfHint);
	sender->SetBackgroundImage((RtwImage*)NULL);
	unguard;
}

void UIFormItemUpdate::OnGetHintText(RtwWidget* sender, void*)
{
	guard;

	SItemID itemID;
	int page, i, j;
	if (m_pBtnItem==sender)
	{
		if ( !ItemID_IsValid(m_UpdateItemID) )
		{
			sender->ModifyFlags(0, wfHint);
			return;
		}

		itemID = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(m_pageItem, m_iItem, m_jItem);
		if (itemID.id != m_UpdateItemID.id)
		{
			if ( !SeekUpdateItemPos() )
				return;
			itemID = m_UpdateItemID;
		}
		page = m_pageItem;
		i = m_iItem;
		j = m_jItem;
	}
	else if (m_pBtnScroll==sender)
	{
		if ( !ItemID_IsValid(m_ScrollID) )
		{
			sender->ModifyFlags(0, wfHint);
			return;
		}

		itemID = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(m_pageScroll, m_iScroll, m_jScroll);
		if (itemID.id != m_ScrollID.id)
		{
			if ( !SeekScrollPos() )
				return;
			itemID = m_ScrollID;
		}
		page = m_pageScroll;
		i = m_iScroll;
		j = m_jScroll;
	}
	else if (m_pBtnLuckStone==sender)
	{
		if ( !ItemID_IsValid(m_LuckStoneID) )
		{
			sender->ModifyFlags(0, wfHint);
			return;
		}

		itemID = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(m_pageLuckStone, m_iLuckStone, m_jLuckStone);
		if (itemID.id != m_LuckStoneID.id)
		{
			if ( !SeekLuckStonePos() )
				return;
			itemID = m_LuckStoneID;
		}
		page = m_pageLuckStone;
		i = m_iLuckStone;
		j = m_jLuckStone;
	}
	else 
	{
		return;
	}

	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, itemID, GetWorld()->m_pPlayer, &(GetWorld()->m_pPlayer->mItem.m_Bag), page, i, j);
	g_workspace.SetHintText((char*)str, true);
//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();

	unguard;
}


bool UIFormItemUpdate::SeekUpdateItemPos()
{
	guard;

	if (!ItemID_IsValid(m_UpdateItemID))
		return false;

	if ( GetWorld()->m_pPlayer->mItem.m_Bag.Find(m_UpdateItemID.id, &m_pageItem, &m_iItem, &m_jItem) )
		return true;
	else 
		return false;

	unguard;
}

bool UIFormItemUpdate::SeekScrollPos()
{
	guard;

	if (!ItemID_IsValid(m_ScrollID))
		return false;

	if ( GetWorld()->m_pPlayer->mItem.m_Bag.Find(m_ScrollID.id, &m_pageScroll, &m_iScroll, &m_jScroll) )
		return true;
	else 
		return false;

	unguard;
}

bool UIFormItemUpdate::SeekLuckStonePos()
{
	guard;

	if (!ItemID_IsValid(m_LuckStoneID))
		return false;

	if ( GetWorld()->m_pPlayer->mItem.m_Bag.Find(m_LuckStoneID.id, &m_pageLuckStone, &m_iLuckStone, &m_jLuckStone) )
		return true;
	else 
		return false;

	unguard;
}

void UIFormItemUpdate::OnCommonItemDeleted(DWORD itemID)
{
	guard;

	if (m_UpdateItemID.id == itemID)
	{
		ItemID_SetInvalid(m_UpdateItemID);
		m_pageItem = m_iItem = m_jItem = -1;

		g_workspace.ResetHintText();
		Refresh();
		return;
	}
	else if (m_ScrollID.id == itemID)
	{
		ItemID_SetInvalid(m_ScrollID);
		m_pageScroll = m_iScroll = m_jScroll = -1;

		g_workspace.ResetHintText();
		Refresh();
		return;
	}
	else if (m_LuckStoneID.id == itemID)
	{
		ItemID_SetInvalid(m_LuckStoneID);
		m_pageLuckStone = m_iLuckStone = m_jLuckStone = -1;

		g_workspace.ResetHintText();
		Refresh();
		return;
	}

	unguard;
}

void UIFormItemUpdate::OnGetFocus(void*,void*)
{
	guard;
	g_layerMain->OnSetNewWidget(m_pFrmThis);
	unguard;
}

void UIFormItemUpdate::OnHide(void*,void*)
{
	guard;
	Hide();
	g_layerMain->OnDeleteWidget(m_pFrmThis);
	unguard;
}
