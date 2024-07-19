#include "gc_include.h"
#include "ui_form_pet_include.h"
#include "ui_form_pet.h"
#include "ui_form_playeritems.h"
#include "ui_form_skill.h"
#include "gc_pet.h"

UIFormPetAbility::UIFormPetAbility()
{
	m_pFmWindow		= RTW_WIDGET_T(RtwForm, "fmpetcombine");
	m_pFmPetView	= RTW_WIDGET_T(Rtw3DView, "fmpetcombine.fmbaby.fmmianpet");
	m_pCbPetList	= RTW_WIDGET_T(RtwComboBox, "fmpetcombine.fmbaby.cbtlist1");
	m_pBtnItem1		= RTW_WIDGET_T(RtwButton, "fmpetcombine.fmbaby.btnitem1");
	m_pBtnItem2		= RTW_WIDGET_T(RtwButton, "fmpetcombine.fmbaby.btnitem2");
	m_pBtnAbility	= RTW_WIDGET_T(RtwButton, "fmpetcombine.fmbaby.btncompose");
	m_pLbRate		= RTW_WIDGET_T(RtwLabel, "fmpetcombine.fmbaby.lbmoney");
	m_pLbLev		= RTW_WIDGET_T(RtwLabel, "fmpetcombine.fmbaby.1253");
	m_pFmWindow->EvHide += RTW_CALLBACK(this,UIFormPetAbility,OnHide);

	m_pCbPetList->EvSelect	+= RTW_CALLBACK(this, UIFormPetAbility, OnSelectPet);
	m_pBtnAbility->EvLClick += RTW_CALLBACK(this, UIFormPetAbility, OnClickAbility);
	m_pBtnItem1->EvDragEnd	+= RTW_CALLBACK(this, UIFormPetAbility, DragOutItem1);
	m_pBtnItem2->EvDragEnd	+= RTW_CALLBACK(this, UIFormPetAbility, DragOutItem2);
	m_pBtnItem1->EvRClick	+= RTW_CALLBACK(this, UIFormPetAbility, DragOutItem1);
	m_pBtnItem2->EvRClick	+= RTW_CALLBACK(this, UIFormPetAbility, DragOutItem2);
	m_pBtnItem1->EvHint		+= RTW_CALLBACK(this, UIFormPetAbility, OnGetHintItemText);
	m_pBtnItem2->EvHint		+= RTW_CALLBACK(this, UIFormPetAbility, OnGetHintItemText);
	m_pBtnItem1->SetParam1(&m_item1.id);
	m_pBtnItem2->SetParam1(&m_item2.id);
	m_pBtnItem1->ModifyFlags(wfDragOut | wfHint, 0);
	m_pBtnItem2->ModifyFlags(wfDragOut | wfHint, 0);
	Reset();
	m_pdwSelectPet = NULL;
}
UIFormPetAbility::~UIFormPetAbility()
{

}

void UIFormPetAbility::OnGetHintItemText(RtwButton *sender,void *)
{
	guard;

	CBag *pBag = &GetPlayer()->mItem.m_Bag;
	if (!pBag) return;

	int *id = (int*)sender->getParam1();
	if (id)
	{
		SItemID item = pBag->GetItemFromID(*id);
		if (ItemID_IsValid(item))
		{
			int nLines;
			RtString str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, pBag);
			g_workspace.SetHintText((char*)str, true);
			g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
			g_workspace.AutoHintSize();
			g_workspace.AutoHintPosition();
			return;
		}
	}

	//ldr123
	//button中没有道具时,hint清空
	g_workspace.SetHintText("");
	//end
	unguard;
}

void UIFormPetAbility::SetItem1(SItemID item1)
{
	if (!ItemID_IsValid(item1) || CItemManager::GetIndexFromType(item1.type) != PET_ABILITY_NEED_ITEM1)
	{
		PlayerFailOperate(R(MSG_NO_ABILITY_ITEM1));
		return;
	}
	m_item1 = item1;
	SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(m_item1.type);
	if(!pItemClass)	
		return;
	m_pBtnItem1->SetBackgroundImage( g_workspace.getImageFactory()->createImage(pItemClass->icon) );
	SetAbilityButton();
}

void UIFormPetAbility::SetItem2(SItemID item2)
{
	if (!ItemID_IsValid(item2) || CItemManager::GetIndexFromType(item2.type) != PET_ABILITY_NEED_ITEM2)
	{
		PlayerFailOperate(R(MSG_NO_ABILITY_ITEM2));
		return;
	}
	m_item2 = item2;
	SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(m_item2.type);
	if(!pItemClass)	
		return;
	m_pBtnItem2->SetBackgroundImage( g_workspace.getImageFactory()->createImage(pItemClass->icon) );
	SetAbilityButton();
}

void UIFormPetAbility::DragOutItem1(RtwButton *sender,void *)
{
	//if(!g_workspace.getMouseHover())
	{
		m_item1 = ItemID_CreateInvalid();

		sender->SetBackgroundImage((RtwImage*)NULL);
		SetAbilityButton();
	}
}

void UIFormPetAbility::DragOutItem2(RtwButton *sender,void *)
{
	//if(!g_workspace.getMouseHover())
	{
		m_item2 = ItemID_CreateInvalid();

		sender->SetBackgroundImage((RtwImage*)NULL);
		SetAbilityButton();
	}
}

void UIFormPetAbility::SetAbilityButton()
{
	if (CanFusePet(GetWorld()->m_pItemManager,PET_FUSE_TYPE_ABILITY,m_pet,m_item1,m_item2))
	{
		m_pBtnAbility->Enable();
	}
	else
	{
		m_pBtnAbility->Disable();
	}
}

void UIFormPetAbility::OnClickAbility(RtwButton *sender,void *)
{
	if (!ItemIsPet(m_pet))
	{
		return;
	}
	if (!ItemID_IsValid(m_item1))
	{
		return;
	}

	CPetNetHelper::NetSend_c2r_fuse_pet_start(PET_FUSE_TYPE_ABILITY,m_pet,m_item1,m_item2);

}
void UIFormPetAbility::OnSelectPet(RtwComboBox *sender,void *)
{
	CBag *pBag = &GetPlayer()->mItem.m_PetBag;
	if (!pBag) return;
	RtwListBox::SLine *pLine =  m_pCbPetList->GetSelectLine();
	if (pLine)
	{
		DWORD *id = (DWORD*)pLine->param;
		if (id)
		{
			SItemID pet = pBag->GetItemFromID(*id);
			if (ItemIsPet(pet))
			{
				if (pet.params[PET_PARAM_ABILITY_LEV] >= 10)
				{
					m_pCbPetList->SetSelectedItem();
					PlayerFailOperate(R(MSG_ABILITY_LEV_MAX));
					return;
				}
				if (GetPlayerPet() && GetPlayerPet()->GetPetActor() && GetPlayerPet()->GetPetItem().id == pet.id)
				{
					m_pCbPetList->SetSelectedItem();
					PlayerFailOperate(R(MSG_PET_CANNOT_FUSE_FIGHT));
					return;
				}
				m_pet = pet;
				SetAbilityButton();
				Refresh();
				m_pdwSelectPet = id;

				m_pFmPetView->ReleaseAllActor();
				SItemPet *pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(m_pet.type);
				SPet* info = g_TablePet.FindPet(pItemPet->petID);
				int modelID = 0;

				if(info)
				{
					for(int i=0; i<MAX_PET_MODEL_CHANGE; i++)
					{
						if(m_pet.params[PET_PARAM_LEV] <= info->ModelLevel[i])
						{
							modelID = info->ModelChange[i];
							break;
						}
					}
					SCreModel *model = g_TableCreModel.FindCreModel(modelID);
					if (model)
					{
						int cont = m_pFmPetView->AddCreateActor((char*)model->Act.c_str());
						CRT_ActorInstance *p = m_pFmPetView->GetActorByCont(cont);
						p->PlayPose("wait",true);
					}
				}
			}
		}
	}
}

void UIFormPetAbility::Reset()
{
	m_pCbPetList->RemoveAllItems();
	m_pCbPetList->SetText("请选择宠物");
	m_pBtnItem1->SetBackgroundImage((RtwImage*)NULL);
	m_pBtnItem2->SetBackgroundImage((RtwImage*)NULL);
	m_pLbRate->SetText("");
	m_pLbLev->SetText("");
	m_listPet.clear();
	ItemID_SetInvalid(m_pet);
	ItemID_SetInvalid(m_item1);
	ItemID_SetInvalid(m_item2);
	SetAbilityButton();
}

void UIFormPetAbility::Show()
{
	if (g_layerMain->m_formPetMerge->IsVisible())
		g_layerMain->m_formPetMerge->Hide();
	if (g_layerMain->m_formPetMergeBeads->IsVisible())
		g_layerMain->m_formPetMergeBeads->Hide();
	if (g_layerMain->m_formPetToBeads->IsVisible())
		g_layerMain->m_formPetToBeads->Hide();
	g_layerMain->OnSetNewWidget(m_pFmWindow);

	if (IsVisible())
	{
		Refresh();
	}
	else
	{
		m_pFmWindow->Show();
		OnPreDraw();
	}
}
bool UIFormPetAbility::IsVisible()
{
	return m_pFmWindow->IsVisible();
}

void UIFormPetAbility::OnPreDraw()
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
					m_listPet.push_back(item);
					m_pCbPetList->AddListItem(item.name);
					m_pCbPetList->SetLineParam(id,&m_listPet.back().id);
					id++;
				}
			}
		}
	}
}

void UIFormPetAbility::OnFinishAbility(bool succeed)
{
	ItemID_SetInvalid(m_item1);
	ItemID_SetInvalid(m_item2);
	m_pBtnItem1->SetBackgroundImage((RtwImage*)NULL);
	m_pBtnItem2->SetBackgroundImage((RtwImage*)NULL);
	m_pet = GetPlayer()->mItem.m_PetBag.GetItemFromID(m_pet.id);
	Refresh();
}

void UIFormPetAbility::Refresh()
{
	if (ItemIsPet(m_pet))
	{
		float fRate;
		FusePetRate(PET_FUSE_TYPE_ABILITY,m_pet,m_pet,&fRate);
		char str[32];
		rt2_sprintf(str,"%d%%",int(fRate*100.f));
		m_pLbRate->SetText(str);
		rt2_sprintf(str,"%d",m_pet.params[PET_PARAM_ABILITY_LEV]);
		m_pLbLev->SetText(str);
	}
}

void UIFormPetAbility::Hide()
{
	m_pFmWindow->Hide();
}

void UIFormPetAbility::OnHide(RtwWidget *sender,void *)
{
	m_pFmPetView->ReleaseAllActor();
	g_layerMain->OnDeleteWidget(m_pFmWindow);
	m_pdwSelectPet = NULL;
}