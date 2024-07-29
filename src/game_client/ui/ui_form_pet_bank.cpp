#include "gc_include.h"
#include "item_bank.h"
#include "ui_form_textmsg.h"
#include "ui_form_playeritems.h"
#include "ui_form_item_update.h"
#include "ui_form_shop.h"
#include "ui_form_playershop.h"
#include "ui_form_lookplayershop.h"
#include "ui_form_trade.h"
#include "ui_form_pet_bank.h"
#include "ui_form_bank.h"
#include "ui_form_compose.h"
#include "ui_form_fuse.h"
#include "UIForm_Gem.h"
#include "ui_form_decompound.h"
#include "ui_form_union_create.h"
#include "ui_form_char.h"


UIFormBankPet::UIFormBankPet()
:m_BankBag(BANK_PET_LINE_COUNT, BANK_PET_COLUMN_COUNT ,1 , GetWorld()->m_pItemManager)
{
	m_pFmThis = LOAD_UI_T(RtwForm, "fmpetbank");				
	m_pFmPetLookMine = LOAD_UI_T(RtwForm, "fmpetbank.conjureform02");			
	m_pFmPetLookBank = LOAD_UI_T(RtwForm, "fmpetbank.conjureform04");					

	m_pListPetMine = LOAD_UI_T(RtwListBox, "fmpetbank.conjureform01.obpetlist");			
	m_pListPetBank = LOAD_UI_T(RtwListBox, "fmpetbank.conjureform03.mywithlist");		

	m_pBtnToMine = LOAD_UI_T(RtwButton, "fmpetbank.petsellblt");			
	m_pBtnToBank = LOAD_UI_T(RtwButton, "fmpetbank.petreturnblt");	

	m_pBtnToMine->EvLClick += RTW_CALLBACK(this, UIFormBankPet, OnLClickToMine);
	m_pBtnToBank->EvLClick += RTW_CALLBACK(this, UIFormBankPet, OnLClickToBank);
	m_pListPetMine->EvLDClick += RTW_CALLBACK(this, UIFormTrade, OnLDClickListPet);
	m_pListPetBank->EvLDClick += RTW_CALLBACK(this, UIFormTrade, OnLDClickListPet);
	m_nNpcID = 0;
}

UIFormBankPet::~UIFormBankPet()
{

}

void UIFormBankPet::Refresh()
{
	OnPreDraw();
}

void UIFormBankPet::Show(DWORD npcID)
{
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
		//if (g_layerMain->m_formPShop->IsVisible())			// 摆摊
		//	g_layerMain->m_formPShop->Hide();
		if (g_layerMain->m_formLookPlayerShop->IsVisible())		// 察看摆摊
			g_layerMain->m_formLookPlayerShop->Hide();
		if (g_layerMain->m_formTrade->IsShowing())			// 交易
			g_layerMain->m_formTrade->Hide();
		//if (g_layerMain->m_formBank->IsVisible())			// 仓库
		//	g_layerMain->m_formBank->Hide();
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

	if (IsVisible())
	{
		Refresh();
	}
	else
	{
		m_pFmThis->Show();
		OnPreDraw();
	}
	CPetNetHelper::NetSend_c2r_bank_query_pet(npcID, "");
	m_nNpcID = npcID;
}

void UIFormBankPet::OnPreDraw()
{
	Reset();

	CBag *pBag = &GetPlayer()->mItem.m_PetBag;
	if (!pBag) return;

	SItemID item;
	SItemPet *pItemPet = NULL;
	SPet *pPet = NULL;
	int id = 0;

	for (int i=0; i<ITEM_PET_BAG_COLUMN; i++)
	{
		item = pBag->GetItem(0, i);
		if (ItemIsPet(item))
		{
			pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(item.type);
			if (pItemPet)
			{
				pPet = g_TablePet.FindPet(pItemPet->petID);
				if (pPet)
				{
					m_listPetMine.push_back(item);
					m_pListPetMine->AddItem(item.name);
					m_pListPetMine->SetLineParam(id,&m_listPetMine.back());
					id++;
				}
			}
		}
	}

	pBag = &m_BankBag;
	if (!pBag) return;
	id = 0;
	for (int i=0; i<BANK_PET_COLUMN_COUNT; i++)
	{
		item = pBag->GetItem(0, i);
		if (ItemIsPet(item))
		{
			pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(item.type);
			if (pItemPet)
			{
				pPet = g_TablePet.FindPet(pItemPet->petID);
				if (pPet)
				{
					m_listPetBank.push_back(item);
					m_pListPetBank->AddItem(item.name);
					m_pListPetBank->SetLineParam(id,&m_listPetBank.back());
					id++;
				}
			}
		}
	}
}

void UIFormBankPet::Reset()
{
	m_listPetBank.clear();
	m_listPetMine.clear();
	m_pListPetBank->RemoveAllItems();
	m_pListPetMine->RemoveAllItems();
}

void UIFormBankPet::Hide()
{
	Reset();
	m_pFmThis->Hide();
}
bool UIFormBankPet::IsVisible()
{
	return m_pFmThis->IsVisible();
}
bool UIFormBankPet::SerializeBankBag(CG_CmdPacket *cmd)
{
	m_BankBag.Clear();
	return m_BankBag.Serialize(NET_UPDATE_ALL,cmd);
}

void UIFormBankPet::OnLClickListPet(RtwListBox *sender,void *)
{

}


void UIFormBankPet::OnLClickToMine(RtwButton *sender,void *)
{
	RtwListBox::SLine *pLine =  m_pListPetBank->getSelectLineItem();
	if (pLine)
	{
		SItemID *pItem = (SItemID*)pLine->param;
		if (pItem && ItemIsPet(*pItem))
		{
			m_ToMinePet = *pItem;
			CPetNetHelper::NetSend_c2r_bank_withdraw_pet((*pItem).id,m_nNpcID);
		}
	}
}

void UIFormBankPet::OnLClickToBank(RtwButton *sender,void *)
{
	RtwListBox::SLine *pLine =  m_pListPetMine->getSelectLineItem();
	if (pLine)
	{
		SItemID *pItem = (SItemID*)pLine->param;
		if (pItem && ItemIsPet(*pItem))
		{
			if (GetPlayerPet()->GetPetActor() && GetPlayerPet()->GetPetItem().id == pItem->id)
			{
				PlayerFailOperate(R(MSG_CANNOT_STORE_PET_CALLOUT));
				return;
			}
			m_ToBankPet = *pItem;
			CPetNetHelper::NetSend_c2r_bank_store_pet((*pItem).id,m_nNpcID);
		}
	}
}


void UIFormBankPet::ToMineResult(bool secceed)
{
	if (secceed)
	{
		if (m_BankBag.RemoveItem(m_ToMinePet))
		{
			Refresh();
		}
	}
	ItemID_SetInvalid(m_ToMinePet);
}

void UIFormBankPet::ToBankResult(bool secceed)
{
	if (secceed)
	{
		if (m_BankBag.AddItem(m_ToBankPet))
		{
			Refresh();
		}
	}
	ItemID_SetInvalid(m_ToBankPet);
}
