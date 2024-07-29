#include "gc_include.h"
#include "ui_form_textmsg.h"
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
#include "ui_form_peteat.h"

UIFormPetEat::UIFormPetEat()
{
	guard;

	m_pFrmThis = LOAD_UI("fmbeast");
	m_pBtn_PetFrom = LOAD_UI_T(RtwButton, "fmbeast.aliasBeast2");
    m_pBtn_PetTo = LOAD_UI_T(RtwButton, "fmbeast.aliasBeast1");
	m_pBtn_Ok = LOAD_UI_T(RtwButton, "fmbeast.btnok");
	m_pBtn_Cancel = LOAD_UI_T(RtwButton, "fmbeast.btncancel");

	m_pBtn_Ok->EvLClick += RTW_CALLBACK(this, UIFormPetEat, OnClicked_Ok);
	m_pBtn_Cancel->EvLClick += RTW_CALLBACK(this, UIFormPetEat, OnClicked_Cancel);
    m_pBtn_PetFrom->EvDrag += RTW_CALLBACK(this, UIFormPetEat, OnDragTo_PetFrom);
    m_pBtn_PetFrom->EvDragEnd += RTW_CALLBACK(this, UIFormPetEat, OnDragEnd_PetFrom);
    m_pBtn_PetTo->EvDrag += RTW_CALLBACK(this, UIFormPetEat, OnDragTo_PetTo);
    m_pBtn_PetTo->EvDragEnd += RTW_CALLBACK(this, UIFormPetEat, OnDragEnd_PetTo);
    m_pBtn_PetFrom->EvHint += RTW_CALLBACK(this, UIFormPetEat, OnGetHintText);
    m_pBtn_PetTo->EvHint += RTW_CALLBACK(this, UIFormPetEat, OnGetHintText);

    m_pBtn_PetFrom->ModifyFlags(wfDragClient | wfDragServer, 0);
    m_pBtn_PetTo->ModifyFlags(wfDragClient | wfDragServer, 0);

	m_pFrmThis->Hide();

	unguard;
}

UIFormPetEat::~UIFormPetEat()
{
	guard;
	unguard;
}

void UIFormPetEat::OnPreDraw(bool bComposeList/* = true*/, bool bComposingDetail/* = true*/)
{
	guard;

    SItemBase *pItemClass = NULL;
    if (ItemID_IsValid(m_PetFrom))
    {
        pItemClass = GetWorld()->m_pItemManager->GetItem(m_PetFrom.type);
        if(!pItemClass)			return;
        //m_pBtn_PetFrom->SetBackgroundImage( new RtwImage(pItemClass->icon) );// change [3/18/2009 tooth.shi]
		m_pBtn_PetFrom->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));
    }
    else 
        m_pBtn_PetFrom->SetBackgroundImage((RtwImage*)NULL);

    if (ItemID_IsValid(m_PetTo))
    {
        pItemClass = GetWorld()->m_pItemManager->GetItem(m_PetTo.type);
        if(!pItemClass)			return;
        //m_pBtn_PetTo->SetBackgroundImage( new RtwImage(pItemClass->icon) );// change [3/18/2009 tooth.shi]
		m_pBtn_PetTo->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));
    }
    else
        m_pBtn_PetTo->SetBackgroundImage((RtwImage*)NULL);

	unguard;
}

void UIFormPetEat::OnClicked_Ok(RtwWidget* sender, void*)
{
	guard;

    //if ( g_layerMain->m_formPShop->IsVisible() )
    //{
    //    PlayerFailOperate("请先关闭摆摊");
    //    return;
    //}
    if ( g_layerMain->m_formTrade->IsShowing() )
    {
       // PlayerFailOperate("请先关闭交易");
        return;
    }

    if (ItemID_IsValid(m_PetFrom) && ItemID_IsValid(m_PetTo))
    {
        if (m_PetFrom.params[PET_PARAM_LEV] < m_PetTo.params[PET_PARAM_LEV])
        {
            PlayerFailOperate(R(MSG_PET_LOWCANNOTEATHIGH));
            return;
        }
        CItemNetHelper::NetSend_c2r_player_peteat(m_PetFrom.id, m_PetTo.id);
        m_pBtn_Ok->Disable();
        m_pBtn_Cancel->Disable();
    }
    else 
    {
        PlayerFailOperate(R(MSG_PET_PUTTOKENFIRST));
    }

	unguard;
}

void UIFormPetEat::OnClicked_Cancel(RtwWidget* sender, void*)
{
	guard;
    Hide();
	unguard;
}

void UIFormPetEat::Show()
{
	guard;

    if ( IsVisible() )		return;

    if (g_layerMain)
    {
        //		if (g_layerMain->m_fromPlayerItems->IsShowing())	// 道具栏
        //			g_layerMain->m_fromPlayerItems->Hide();
      	if (g_layerMain->m_formItemUpdate->IsVisible())		// 道具打造
        	g_layerMain->m_formItemUpdate->Hide();
        if (g_layerMain->m_formShop->IsVisible())			// NPC商店
            g_layerMain->m_formShop->Hide();
        //if (g_layerMain->m_formPShop->IsVisible())			// 摆摊
        //    g_layerMain->m_formPShop->Hide();
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
   }

    m_PetFrom = ItemID_CreateInvalid();
    m_PetTo = ItemID_CreateInvalid();

    m_pBtn_Ok->Enable();
    m_pBtn_Cancel->Enable();

    OnPreDraw();
    m_pFrmThis->Show();

	unguard;
}

bool UIFormPetEat::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormPetEat::Hide()
{
	guard;
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormPetEat::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormPetEat::OnDragTo_PetFrom(RtwWidget* sender, void*)
{
    guard;

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
            int ItemIndex_From = GetItemMgr()->GetIndexFromType(item.type);
            if(ItemIndex_From == 476 || ItemIndex_From == 512 || ItemIndex_From == 513)
            {
                m_pBtn_PetFrom->ModifyFlags(wfHint, 0);
                m_PetFrom = item;

                g_workspace.ResetHintText();
                Refresh();
            }
            else 
            {
                PlayerFailOperate(R(MSG_PET_TOKENWRONG));
            }
        }
    }

    unguard;
}

void UIFormPetEat::OnDragTo_PetTo(RtwWidget* sender, void*)
{
    guard;

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
            int ItemIndex_To = GetItemMgr()->GetIndexFromType(item.type);
            if(ItemIndex_To == 1108)
            {
                m_pBtn_PetTo->ModifyFlags(wfHint, 0);
                m_PetTo = item;

                g_workspace.ResetHintText();
                Refresh();
            }
            else 
            {
                PlayerFailOperate(R(MSG_PET_TOKENWRONG));
            }
        }
    }

    unguard;
}

void UIFormPetEat::OnDragEnd_PetFrom(RtwWidget* sender, void*)
{
    guard;

    RtwWidget* pClient = g_workspace.GetDragClient();
    if ( !pClient )// 把图标拖到空白处
    {
        m_PetFrom = ItemID_CreateInvalid();
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
            m_PetFrom = ItemID_CreateInvalid();
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

void UIFormPetEat::OnDragEnd_PetTo(RtwWidget* sender, void*)
{
    guard;

    RtwWidget* pClient = g_workspace.GetDragClient();
    if ( !pClient )// 把图标拖到空白处
    {
        m_PetTo = ItemID_CreateInvalid();
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
            m_PetTo = ItemID_CreateInvalid();
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

void UIFormPetEat::OnGetHintText(RtwWidget* sender, void*)
{
    guard;

    SItemID itemID;
    if (m_pBtn_PetFrom == sender)
    {
        if ( !ItemID_IsValid(m_PetFrom) )
        {
            sender->ModifyFlags(0, wfHint);
            return;
        }
        itemID = m_PetFrom;
    }
    else if (m_pBtn_PetTo == sender)
    {
        if ( !ItemID_IsValid(m_PetTo) )
        {
            sender->ModifyFlags(0, wfHint);
            return;
        }
        itemID = m_PetTo;
    }
    else 
    {
        return;
    }

    int nLines;
    RtString str = GenerateItemHtmlHint(nLines, itemID, GetWorld()->m_pPlayer, &(GetWorld()->m_pPlayer->mItem.m_Bag));
    g_workspace.SetHintText((char*)str, true);
    g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
    g_workspace.AutoHintSize();
    g_workspace.AutoHintPosition();

    unguard;
}


