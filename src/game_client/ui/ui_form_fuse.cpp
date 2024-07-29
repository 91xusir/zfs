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

#include "ui_form_timebar.h"

UIFormItemFuse::UIFormItemFuse()
{
	guard;

	m_pFrmThis = LOAD_UI("fmfuse");
	m_pBtnItem1 = LOAD_UI("fmfuse.aliasCombitem1");
	m_pBtnItem2 = LOAD_UI("fmfuse.aliasCombitem2");
	m_pMoney = LOAD_UI("fmfuse.lbtext2");
	m_pBtnOK = LOAD_UI_T(RtwButton, "fmfuse.btnok");
	m_pBtnCancel = LOAD_UI_T(RtwButton, "fmfuse.btncancel");

	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);
	m_pFrmThis->ModifyFlags(wfDragClient, 0);
	m_pFrmThis->EvFocus			+= RTW_CALLBACK(this,UIFormItemFuse,OnGetFocus);
	m_pFrmThis->EvHide			+= RTW_CALLBACK(this,UIFormItemFuse,OnHide);
	m_ptrHoverImage	= g_workspace.getImageFactory()->createImage("ui/textures/highlight_button.tga");
	m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
	m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
	m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
	m_ptrHoverImage->getRenderImage()->SetBlend(true);

	m_pBtnItem1->ModifyFlags(wfDragClient | wfDragServer|wfDragIn, 0);
	m_pBtnItem2->ModifyFlags(wfDragClient | wfDragServer|wfDragIn, 0);
	m_pBtnOK->EvLClick += RTW_CALLBACK(this, UIFormItemFuse, OnClicked_OK);
	m_pBtnCancel->EvLClick += RTW_CALLBACK(this, UIFormItemFuse, OnClicked_Cancel);
	m_pBtnItem1->EvDrag += RTW_CALLBACK(this, UIFormItemFuse, OnDragToItem1);
	m_pBtnItem1->EvDragEnd += RTW_CALLBACK(this, UIFormItemFuse, OnDragEndItem1);
	m_pBtnItem1->EvMouseRDown += RTW_CALLBACK(this, UIFormItemFuse, OnMouseRDown);
	m_pBtnItem1->EvHint += RTW_CALLBACK(this, UIFormItemFuse, OnGetHintText);
	((RtwButton*)m_pBtnItem1)->SetHoverImage(m_ptrHoverImage);
	m_pBtnItem2->EvDrag += RTW_CALLBACK(this, UIFormItemFuse, OnDragToItem2);
	m_pBtnItem2->EvDragEnd += RTW_CALLBACK(this, UIFormItemFuse, OnDragEndItem2);
	m_pBtnItem2->EvMouseRDown += RTW_CALLBACK(this, UIFormItemFuse, OnMouseRDown);
	m_pBtnItem2->EvHint += RTW_CALLBACK(this, UIFormItemFuse, OnGetHintText);
	((RtwButton*)m_pBtnItem2)->SetHoverImage(m_ptrHoverImage);

	m_pFrmThis->Hide();

	unguard;
}

UIFormItemFuse::~UIFormItemFuse()
{
	guard;

	unguard;
}

void UIFormItemFuse::OnPreDraw()
{
	guard;

	SItemBase *pItemClass = NULL;
	if (ItemID_IsValid(m_Item1))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_Item1.type);
		if(!pItemClass)			return;
		//m_pBtnItem1->SetBackgroundImage( new RtwImage(pItemClass->icon) );// change [3/17/2009 tooth.shi]
		m_pBtnItem1->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));
	}
	else 
		m_pBtnItem1->SetBackgroundImage((RtwImage*)NULL);

	if (ItemID_IsValid(m_Item2))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_Item2.type);
		if(!pItemClass)			return;
		//m_pBtnItem2->SetBackgroundImage( new RtwImage(pItemClass->icon) );// change [3/17/2009 tooth.shi]
		m_pBtnItem2->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));
	}
	else
		m_pBtnItem2->SetBackgroundImage((RtwImage*)NULL);

	if (ItemID_IsValid(m_Item1) && ItemID_IsValid(m_Item2) && pItemClass)
	{
		char tmp[128];
		rt2_sprintf(tmp,"%d",int((float)pItemClass->requirement[ItemRequirement_Level]/10.f * (float)m_Item1.color * 200.f));
		m_pMoney->SetText(tmp);
	}
	else
	{
		m_pMoney->SetText("0");
	}

	m_pBtnOK->Enable();

	unguard;
}

void UIFormItemFuse::Show()
{
	guard;

	if (g_layerMain)
	{
		//if (g_layerMain->m_fromPlayerItems->IsShowing())	// 道具栏
		//	g_layerMain->m_fromPlayerItems->Hide();
		if (g_layerMain->m_formItemUpdate->IsVisible())		// 道具打造
			g_layerMain->m_formItemUpdate->Hide();
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
		//if (g_layerMain->m_formItemFuse->IsVisible())		// 道具融合
		//	g_layerMain->m_formItemFuse->Hide();
		if (g_layerMain->m_formItemDecompound->IsVisible())
			g_layerMain->m_formItemDecompound->Hide();
		if (g_layerMain->m_formUnionCreate->IsVisible())	// 创建诸侯国
			g_layerMain->m_formUnionCreate->Hide();
		if (g_layerMain->m_formRecoverPet->GetVisible())
			g_layerMain->m_formRecoverPet->SetVisible(false);
	}

	m_Item1.Clear();
	m_Item2.Clear();

	OnPreDraw();
	m_pFrmThis->Show();

	unguard;
}

bool UIFormItemFuse::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormItemFuse::Hide()
{
	guard;
	g_layerMain->m_fromPlayerItems->OnDisableAllItemMask();
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormItemFuse::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormItemFuse::OnClicked_OK(RtwWidget* sender, void*)
{
	guard;

	//if ( g_layerMain->m_formPShop->IsVisible() )
	//{
	//	PlayerFailOperate(R(融合失败，请先关闭摆摊));
	//	return;
	//}
	if ( g_layerMain->m_formTrade->IsShowing() )
	{
		PlayerFailOperate(R(MSG_COMPOSE_FAIL_SHUTTRADEFIRST));
		return;
	}

	if (ItemID_IsValid(m_Item1) && ItemID_IsValid(m_Item2))
	{
		if (m_Item1.color == ItemColor_White)
			return;
		if (m_Item1.type != m_Item2.type)
		{
			PlayerFailOperate(R(MSG_COMPOSE_FAIL_ITEMNOTSAME));
			return;
		}
		if (m_Item1.color != m_Item2.color)
		{
			PlayerFailOperate(R(MSG_COMPOSE_FAIL_ITEMQUALITYDIFFERENT));
			return;
		}

		SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(m_Item1.type);
		if (!pItemClass)		return;
		//SSkill* pSkill = GetWorld()->m_pPlayer->m_Skill.FindSkillBySubID(ITEMFUSE_SKILL);
		//if (!pSkill || (pItemClass->requirement[ItemRequirement_Level] > ItemDecompose_MaxItemLevel(pSkill->iLevel)) )
		//{
		//	PlayerFailOperate(R(融合失败，您的技能等级不能融合这种装备));
		//	return;
		//}

		if ( GetPlayer()->mItem.m_Bag.GetMoney() <  int((float)pItemClass->requirement[ItemRequirement_Level]/10.f * (float)m_Item1.color * 200.f) )
		{
			PlayerFailOperate(R(MSG_COMPOSE_FAIL_NOTENOUGHMONEY ));
			return;
		}

		CBag *pBag = &(GetWorld()->m_pPlayer->mItem.m_Bag);
		SItemID item1 = pBag->GetItem(m_pageItem1, m_iItem1, m_jItem1);
		SItemID item2 = pBag->GetItem(m_pageItem2, m_iItem2, m_jItem2);
		if (item1.id != m_Item1.id)
		{
			if (!SeekItem1())
			{
				PlayerFailOperate(R(MSG_RETRY_DRAGITEM));
				ItemID_SetInvalid(m_Item1);
				ItemID_SetInvalid(m_Item2);
				Refresh();
				return;
			}
		}
		if (item2.id != m_Item2.id)
		{
			if (!SeekItem2())
			{
				PlayerFailOperate(R(MSG_RETRY_DRAGITEM));
				ItemID_SetInvalid(m_Item1);
				ItemID_SetInvalid(m_Item2);
				Refresh();
				return;
			}
		}

		//ldr123 装备融合进度条
		g_layerMain->m_formTimeBar->Begin(UIFormTimeBar::tbus_fuse, 3000);
		g_layerMain->m_formTimeBar->EvTimeOver += RTW_CALLBACK(this,UIFormItemFuse,DoFuse);
		GetPlayer()->m_bInCallPet = true;
		//end

		m_pBtnOK->Disable();
	}
	else 
	{
		PlayerFailOperate(R(MSG_COMPOSE_PUTTHESAMEQUALITYITEM));
		return;
	}

	unguard;
}

//ldr123 装备融合进度条
void UIFormItemFuse::DoFuse(RtwWidget* sender, void*)
{
	g_layerMain->m_formTimeBar->End(false);
	CItemNetHelper::NetSend_c2r_fuse_item(m_Item1.id, m_Item2.id);
	g_layerMain->m_fromPlayerItems->OnDisableAllItemMask();
}
//end

void UIFormItemFuse::OnClicked_Cancel(RtwWidget* sender, void*)
{
	guard;
	Hide();
	unguard;
}

void UIFormItemFuse::OnDragToItem1(RtwWidget* sender, void*)
{
	guard;

	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	const string& WidgetID = pDragSvr->GetID();
	RtgVertex3 pos;

	// 是不是道具栏的格子
	int i,j;
	if (g_layerMain->m_fromPlayerItems && g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	{
		CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
		SItemID item = pCon->m_Bag.GetItem(i, j);
		if (item.id == m_Item2.id)
		{
			return;
		}
		if ( (ItemIsWeapon(item) || ItemIsArmor(item) || ItemIsShipin(item)) && (item.color!=ItemColor_White) )
		{
            if (item.cLock != 0)
            {
                PlayerFailOperate(R(MSG_ITEM_PROTECT_STATE));
                return;
            }

			m_pBtnItem1->ModifyFlags(wfHint, 0);
			m_Item1 = item;
			m_pageItem1 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
			m_iItem1 = i;
			m_jItem1 = j;
			g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageItem1,m_iItem1,m_jItem1,true,1.0f,0xb5000000,128,true);
			g_workspace.ResetHintText();
			Refresh();
			g_pSoundMgr->PlayOnce("ui_putok.wav", false, 0, pos);
		}else
		{
			g_pSoundMgr->PlayOnce("ui_putfail.wav", false, 0, pos);
			PlayerFailOperate(R(MSG_ITEM_CANNOTFUSE));
		}
	}

	unguard;
}
bool UIFormItemFuse::OnBagToFuseItem1(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	// 是不是道具栏的格子
	int i,j;
	i = e->mouse.x;
	j = e->mouse.y;
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemID item = pCon->m_Bag.GetItem(i, j);
	RtgVertex3 pos;
	if(ItemID_IsValid(m_Item1) || m_Item1.id == item.id || ( ItemID_IsValid(m_Item2) && m_Item2.id == item.id ))
	{
		g_pSoundMgr->PlayOnce("ui_putfail.wav", false, 0, pos);
		return false;
	}
	if ( (ItemIsWeapon(item) || ItemIsArmor(item) || ItemIsShipin(item)) && (item.color!=ItemColor_White) )
	{
		if (item.cLock != 0)
		{
			PlayerFailOperate(R(MSG_ITEM_PROTECT_STATE));
			g_pSoundMgr->PlayOnce("ui_putfail.wav", false, 0, pos);
			return false;
		}

		m_pBtnItem1->ModifyFlags(wfHint, 0);
		m_Item1 = item;
		m_pageItem1 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
		m_iItem1 = i;
		m_jItem1 = j;
		g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageItem1,m_iItem1,m_jItem1,true,1.0f,0xb5000000,128,true);
		g_workspace.ResetHintText();
		Refresh();
		g_pSoundMgr->PlayOnce("ui_putok.wav", false, 0, pos);
	}else
	{
		g_pSoundMgr->PlayOnce("ui_putfail.wav", false, 0, pos);
		PlayerFailOperate(R(MSG_ITEM_CANNOTFUSE));
	}
	return true;
	unguard;
}

void UIFormItemFuse::OnDragToItem2(RtwWidget* sender, void*)
{
	guard;

	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	const string& WidgetID = pDragSvr->GetID();
	RtgVertex3 pos;

	// 是不是道具栏的格子
	int i,j;
	if (g_layerMain->m_fromPlayerItems && g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	{
		CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
		SItemID item = pCon->m_Bag.GetItem(i, j);
		if (item.id == m_Item1.id)
		{
			return;
		}
		if ( (ItemIsWeapon(item) || ItemIsArmor(item) || ItemIsShipin(item)) && (item.color!=ItemColor_White) )
		{
            if (item.cLock != 0)
            {
                PlayerFailOperate(R(MSG_ITEM_PROTECT_STATE));
                return;
            }

			m_pBtnItem2->ModifyFlags(wfHint, 0);
			m_Item2 = item;
			m_pageItem2 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
			m_iItem2 = i;
			m_jItem2 = j;
			g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageItem2,m_iItem2,m_jItem2,true,1.0f,0xb5000000,128,true);
			g_workspace.ResetHintText();
			Refresh();
			g_pSoundMgr->PlayOnce("ui_putok.wav", false, 0, pos);
		}else
		{
			g_pSoundMgr->PlayOnce("ui_putfail.wav", false, 0, pos);
		}
	}

	unguard;
}
bool UIFormItemFuse::OnBagToFuseItem2(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	// 是不是道具栏的格子
	int i,j;
	i = e->mouse.x;
	j = e->mouse.y;
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemID item = pCon->m_Bag.GetItem(i, j);
	RtgVertex3 pos;
	if(ItemID_IsValid(m_Item2) || m_Item1.id == item.id || m_Item2.id == item.id)
	{
		g_pSoundMgr->PlayOnce("ui_putfail.wav", false, 0, pos);
		return false;
	}
	if ( (ItemIsWeapon(item) || ItemIsArmor(item) || ItemIsShipin(item)) && (item.color!=ItemColor_White) )
	{
		if (item.cLock != 0)
		{
			PlayerFailOperate(R(MSG_ITEM_PROTECT_STATE));
			g_pSoundMgr->PlayOnce("ui_putfail.wav", false, 0, pos);
			return false;
		}

		m_pBtnItem2->ModifyFlags(wfHint, 0);
		m_Item2 = item;
		m_pageItem2 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
		m_iItem2 = i;
		m_jItem2 = j;
		g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageItem2,m_iItem2,m_jItem2,true,1.0f,0xb5000000,128,true);
		g_workspace.ResetHintText();
		Refresh();
		g_pSoundMgr->PlayOnce("ui_putok.wav", false, 0, pos);
	}else
	{
		g_pSoundMgr->PlayOnce("ui_putfail.wav", false, 0, pos);
	}
	unguard;
}

void UIFormItemFuse::OnDragEndItem1(RtwWidget* sender, void*)
{
	guard;

	RtwWidget* pClient = g_workspace.GetDragClient();
	if ( !pClient )// 把图标拖到空白处
	{
		m_Item1 = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);
		Refresh();
	}
	else
	{
		int i,j;
		if (g_layerMain->m_fromPlayerItems->Is_Widget_Common(pClient->GetID(), i, j) || 
			g_layerMain->m_fromPlayerItems->Is_Widget_Mission(pClient->GetID(), i, j))
		{
			m_Item1 = ItemID_CreateInvalid();
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

void UIFormItemFuse::OnDragEndItem2(RtwWidget* sender, void*)
{
	guard;

	RtwWidget* pClient = g_workspace.GetDragClient();
	if ( !pClient )// 把图标拖到空白处
	{
		m_Item2 = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);
		Refresh();
	}
	else
	{
		int i,j;
		if (g_layerMain->m_fromPlayerItems->Is_Widget_Common(pClient->GetID(), i, j) || 
			g_layerMain->m_fromPlayerItems->Is_Widget_Mission(pClient->GetID(), i, j))
		{
			m_Item2 = ItemID_CreateInvalid();
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

void UIFormItemFuse::OnMouseRDown(RtwWidget* sender, void*)
{
	guard;
	if(m_pBtnItem1 == sender)
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_Item1);
		m_Item1 = ItemID_CreateInvalid();
		m_pageItem1 = -1;
		m_iItem1 = -1;
		m_jItem1 = -1;
	}else if(m_pBtnItem2 == sender)
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_Item2);
		m_Item2 = ItemID_CreateInvalid();
		m_pageItem2 = -1;
		m_iItem2 = -1;
		m_jItem2 = -1;
	}else
		return;
	sender->ModifyFlags(0, wfHint);
	sender->SetBackgroundImage((RtwImage*)NULL);
	unguard;
}

void UIFormItemFuse::OnGetHintText(RtwWidget* sender, void*)
{
	guard;

	SItemID itemID;
	int page, i, j;
	if (m_pBtnItem1==sender)
	{
		if ( !ItemID_IsValid(m_Item1) )
		{
			sender->ModifyFlags(0, wfHint);
			return;
		}

		itemID = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(m_pageItem1, m_iItem1, m_jItem1);
		if (itemID.id != m_Item1.id)
		{
			if ( !SeekItem1() )
				return;
			itemID = m_Item1;
		}
		page = m_pageItem1;
		i = m_iItem1;
		j = m_jItem1;
	}
	else if (m_pBtnItem2==sender)
	{
		if ( !ItemID_IsValid(m_Item2) )
		{
			sender->ModifyFlags(0, wfHint);
			return;
		}

		itemID = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(m_pageItem2, m_iItem2, m_jItem2);
		if (itemID.id != m_Item2.id)
		{
			if ( !SeekItem2() )
				return;
			itemID = m_Item2;
		}
		page = m_pageItem2;
		i = m_iItem2;
		j = m_jItem2;
	}
	else 
	{
		return;
	}

	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, itemID, GetWorld()->m_pPlayer, &(GetWorld()->m_pPlayer->mItem.m_Bag), page, i, j);
	g_workspace.SetHintText((char*)str, true);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();

	unguard;
}

bool UIFormItemFuse::SeekItem1()
{
	guard;

	if (!ItemID_IsValid(m_Item1))
		return false;

	if ( GetWorld()->m_pPlayer->mItem.m_Bag.Find(m_Item1.id, &m_pageItem1, &m_iItem1, &m_jItem1) )
		return true;
	else 
		return false;

	unguard;
}

bool UIFormItemFuse::SeekItem2()
{
	guard;

	if (!ItemID_IsValid(m_Item2))
		return false;

	if ( GetWorld()->m_pPlayer->mItem.m_Bag.Find(m_Item2.id, &m_pageItem2, &m_iItem2, &m_jItem2) )
		return true;
	else 
		return false;

	unguard;
}

void UIFormItemFuse::OnCommonItemDeleted(DWORD itemID)
{
	guard;

	if (m_Item1.id == itemID)
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_Item1);
		ItemID_SetInvalid(m_Item1);
		m_pageItem1 = m_iItem1 = m_jItem1 = -1;

		g_workspace.ResetHintText();
		Refresh();
		return;
	}
	else if (m_Item2.id == itemID)
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_Item2);
		ItemID_SetInvalid(m_Item2);
		m_pageItem2 = m_iItem2 = m_jItem2 = -1;

		g_workspace.ResetHintText();
		Refresh();
		return;
	}

	unguard;
}
