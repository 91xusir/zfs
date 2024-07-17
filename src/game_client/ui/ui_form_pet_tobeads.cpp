#include "gc_include.h"
#include "ui_form_pet_include.h"
#include "ui_form_pet.h"
#include "ui_form_playeritems.h"
#include "ui_form_skill.h"
#include "gc_pet.h"

UIFormPetToBeads::UIFormPetToBeads()
{
	m_pFmWindow		= RTW_WIDGET_T(RtwForm, "fmpetbeads");
	m_pFmPetView	= RTW_WIDGET_T(Rtw3DView, "fmpetbeads.fmbaby.fmmianpet");
	m_pCbPetList	= RTW_WIDGET_T(RtwComboBox, "fmpetbeads.fmbaby.cbtlist1");
	m_pBtnItem1		= RTW_WIDGET_T(RtwButton, "fmpetbeads.fmbaby.btnitem1");
	m_pBtnItem2		= RTW_WIDGET_T(RtwButton, "fmpetbeads.fmbaby.btnitem2");
	m_pBtnToBeads	= RTW_WIDGET_T(RtwButton, "fmpetbeads.fmbaby.btncompose");
	m_pLbRate		= RTW_WIDGET_T(RtwLabel, "fmpetbeads.fmbaby.lbmoney");
	m_pLbGrade		= RTW_WIDGET_T(RtwLabel, "fmpetbeads.fmbaby.1253");
	m_pFmWindow->EvHide += RTW_CALLBACK(this,UIFormPetToBeads,OnHide);

	m_pCbPetList->EvSelect	+= RTW_CALLBACK(this, UIFormPetToBeads, OnSelectPet);
	m_pBtnToBeads->EvLClick += RTW_CALLBACK(this, UIFormPetToBeads, OnClickToBeads);

	m_pBtnItem1->EvDragEnd	+= RTW_CALLBACK(this, UIFormPetToBeads, DragOutItem1);
	m_pBtnItem2->EvDragEnd	+= RTW_CALLBACK(this, UIFormPetToBeads, DragOutItem2);
	m_pBtnItem1->EvRClick	+= RTW_CALLBACK(this, UIFormPetToBeads, DragOutItem1);
	m_pBtnItem2->EvRClick	+= RTW_CALLBACK(this, UIFormPetToBeads, DragOutItem2);
	m_pBtnItem1->EvHint		+= RTW_CALLBACK(this, UIFormPetAbility, OnGetHintItemText);
	m_pBtnItem2->EvHint		+= RTW_CALLBACK(this, UIFormPetAbility, OnGetHintItemText);
	m_pBtnItem1->SetParam1(&m_item1.id);
	m_pBtnItem2->SetParam1(&m_item2.id);
	m_pBtnItem1->ModifyFlags(wfDragOut | wfHint, 0);
	m_pBtnItem2->ModifyFlags(wfDragOut | wfHint, 0);
	Reset();
	m_pdwSelectPet = NULL;
}
UIFormPetToBeads::~UIFormPetToBeads()
{

}

void UIFormPetToBeads::SetItem1(SItemID item1)
{
	if (!ItemID_IsValid(item1) || CItemManager::GetIndexFromType(item1.type) != PET_TOBEADS_NEED_ITEM1)
	{
		PlayerFailOperate(R(MSG_NO_TOBEADS_ITEM1));
		return;
	}
	m_item1 = item1;
	SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(m_item1.type);
	if(!pItemClass)	
		return;
	m_pBtnItem1->SetBackgroundImage( g_workspace.getImageFactory()->createImage(pItemClass->icon) );
	SetToBeadsButton();
}

void UIFormPetToBeads::SetItem2(SItemID item2)
{
	if (!ItemID_IsValid(item2) || CItemManager::GetIndexFromType(item2.type) != PET_TOBEADS_NEED_ITEM2)
	{
		PlayerFailOperate(R(MSG_NO_TOBEADS_ITEM2));
		return;
	}
	m_item2 = item2;
	SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(m_item2.type);
	if(!pItemClass)	
		return;
	m_pBtnItem2->SetBackgroundImage( g_workspace.getImageFactory()->createImage(pItemClass->icon) );
	SetToBeadsButton();
}

void UIFormPetToBeads::DragOutItem1(RtwButton *sender,void *)
{
	//if(!g_workspace.getMouseHover())
	{
		m_item1 = ItemID_CreateInvalid();

		sender->SetBackgroundImage((RtwImage*)NULL);
		SetToBeadsButton();
	}
}

void UIFormPetToBeads::DragOutItem2(RtwButton *sender,void *)
{
	//if(!g_workspace.getMouseHover())
	{
		m_item2 = ItemID_CreateInvalid();

		sender->SetBackgroundImage((RtwImage*)NULL);
		SetToBeadsButton();
	}
}

void UIFormPetToBeads::SetToBeadsButton()
{
	if (CanFusePet(GetWorld()->m_pItemManager,PET_FUSE_TYPE_TOBEADS,m_pet,m_item1,m_item2))
	{
		m_pBtnToBeads->Enable();
	}
	else
	{
		m_pBtnToBeads->Disable();
	}
}

void UIFormPetToBeads::OnClickToBeads(RtwButton *sender,void *)
{
	if (!ItemIsPetSpectre(m_pet))
	{
		return;
	}
	if (!ItemID_IsValid(m_item1) || CItemManager::GetIndexFromType(m_item1.type) != PET_TOBEADS_NEED_ITEM1)
	{
		return;
	}

	CPetNetHelper::NetSend_c2r_fuse_pet_start(PET_FUSE_TYPE_TOBEADS,m_pet,m_item1,m_item2);

}
void UIFormPetToBeads::OnSelectPet(RtwComboBox *sender,void *)
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
			if (ItemIsPetSpectre(pet))
			{
				if (GetPlayerPet() && GetPlayerPet()->GetPetActor() && GetPlayerPet()->GetPetItem().id == pet.id)
				{
					m_pCbPetList->SetSelectedItem();
					PlayerFailOperate(R(MSG_PET_CANNOT_FUSE_FIGHT));
					return;
				}
				if (pet.params[PET_PARAM_LEV] < 30)
				{
					m_pCbPetList->SetSelectedItem();
					PlayerFailOperate(R(MSG_PET_LEV_NO_30));
					return;
				}
				m_pet = pet;
				SetToBeadsButton();
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
			else
			{
				m_pCbPetList->SetSelectedItem();
				PlayerFailOperate(R(MSG_PET_NO_SPECTRE));
			}
		}
	}
}

void UIFormPetToBeads::Reset()
{
	m_pCbPetList->RemoveAllItems();
	m_pCbPetList->SetText("ÇëÑ¡Ôñ³èÎï");
	m_pBtnItem1->SetBackgroundImage((RtwImage*)NULL);
	m_pBtnItem2->SetBackgroundImage((RtwImage*)NULL);
	m_pLbRate->SetText("");
	m_pLbGrade->SetText("");
	m_listPet.clear();
	ItemID_SetInvalid(m_pet);
	ItemID_SetInvalid(m_item1);
	ItemID_SetInvalid(m_item2);
	SetToBeadsButton();
}

void UIFormPetToBeads::Show()
{
	if (g_layerMain->m_formPetMerge->IsVisible())
		g_layerMain->m_formPetMerge->Hide();
	if (g_layerMain->m_formPetMergeBeads->IsVisible())
		g_layerMain->m_formPetMergeBeads->Hide();
	if (g_layerMain->m_formPetAbility->IsVisible())
		g_layerMain->m_formPetAbility->Hide();
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

bool UIFormPetToBeads::IsVisible()
{
	return m_pFmWindow->IsVisible();
}
void UIFormPetToBeads::OnPreDraw()
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

void UIFormPetToBeads::OnFinishToBeads(bool succeed)
{
	OnPreDraw();
}

void UIFormPetToBeads::Refresh()
{
	if (ItemIsPetSpectre(m_pet))
	{
		float fRate;
		FusePetRate(PET_FUSE_TYPE_TOBEADS,m_pet,m_pet,&fRate);
		char str[32];
		rt2_sprintf(str,"%d%%",int(fRate*100.f));
		m_pLbRate->SetText(str);

		SItemPet *pItemPet = NULL;
		SPet *pPet = NULL;
		pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(m_pet.type);
		if (pItemPet) 
			pPet = g_TablePet.FindPet(pItemPet->petID);
		if (pPet)
		{
			float grade = GetToBeadsGrade(m_pet,pPet);
			rt2_sprintf(str,"%.2f",grade);
			m_pLbGrade->SetText(str);
		}
	}
}
void UIFormPetToBeads::Hide()
{
	m_pFmWindow->Hide();
}

void UIFormPetToBeads::OnHide(RtwWidget *sender,void *)
{
	m_pFmPetView->ReleaseAllActor();
	g_layerMain->OnDeleteWidget(m_pFmWindow);
	m_pdwSelectPet = NULL;
}
