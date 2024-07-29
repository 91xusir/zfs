#include "gc_include.h"
#include "ui_form_pet_include.h"
#include "ui_form_pet.h"
#include "ui_form_playeritems.h"
#include "ui_form_skill.h"
#include "gc_pet.h"

UIFormPetMergeBeads::UIFormPetMergeBeads()
{
	m_pFmWindow		= LOAD_UI_T(RtwForm, "fmmergebeads");
	m_pFmPetView	= LOAD_UI_T(Rtw3DView, "fmmergebeads.fmbaby.fmmianpet");
	m_pCbPetList	= LOAD_UI_T(RtwComboBox, "fmmergebeads.fmbaby.cbtlist1");
	m_pBtnItem		= LOAD_UI_T(RtwButton, "fmmergebeads.fmbaby.btnitem1");

	m_pBtnMergeBeads	= LOAD_UI_T(RtwButton, "fmmergebeads.fmbaby.btncompose");
	m_pLbRate		= LOAD_UI_T(RtwLabel, "fmmergebeads.fmbaby.lbmoney");

	m_pFmPetView->EvHide += RTW_CALLBACK(this,UIFormPetMergeBeads,OnHide);
	char str[128];
	for (int i=0;i<UIPET_LINKUP_BEADS_NUM;i++)
	{
		rt2_sprintf(str,"fmmergebeads.fmbaby.lbJewelry%d",i+1);
		m_pLbBeads[i]		= LOAD_UI_T(RtwLabel, str);
		m_pLbBeads[i]->ModifyFlags(wfHint, 0);
		m_pLbBeads[i]->SetParam1(&m_pet.params[PET_PARAM_BEADS_1+i]);
	}

	m_pCbPetList->EvSelect	+= RTW_CALLBACK(this, UIFormPetMergeBeads, OnSelectPet);
	m_pBtnMergeBeads->EvLClick += RTW_CALLBACK(this, UIFormPetMergeBeads, OnClickMergeBeads);

	m_pBtnItem->EvDragEnd	+= RTW_CALLBACK(this, UIFormPetMergeBeads, DragOutItem);
	m_pBtnItem->EvRClick	+= RTW_CALLBACK(this, UIFormPetMergeBeads, DragOutItem);
	m_pBtnItem->EvHint		+= RTW_CALLBACK(this, UIFormPetAbility, OnGetHintItemText);
	m_pBtnItem->SetParam1(&m_item.id);
	m_pBtnItem->ModifyFlags(wfDragOut | wfHint, 0);
	Reset();
	m_pdwSelectPet = NULL;
}
UIFormPetMergeBeads::~UIFormPetMergeBeads()
{

}

void UIFormPetMergeBeads::Refresh()
{
	if (ItemIsPetSpectre(m_pet))
	{
		float fRate;
		FusePetRate(PET_FUSE_TYPE_MERGEBEADS,m_pet,m_pet,&fRate);
		char str[32];
		rt2_snprintf(str,32,"%d%%",int(fRate*100.f));
		m_pLbRate->SetText(str);

		for (int i=0;i<UIPET_LINKUP_BEADS_NUM;i++)
		{
			if (m_pet.params[PET_PARAM_BEADS_1+i])
			{
				RtwImage *pImage = g_workspace.getImageFactory()->createImage(g_layerMain->m_formPetAptitude->GetPetBeadsImage(m_pet.params[PET_PARAM_BEADS_1+i]));
				if (pImage)
					m_pLbBeads[i]->SetBackgroundImage(pImage);
			}
			else
			{
				m_pLbBeads[i]->SetBackgroundImage((RtwImage*)NULL);
			}
		}
	}
}
void UIFormPetMergeBeads::Show()
{
	if (g_layerMain->m_formPetMerge->IsVisible())
		g_layerMain->m_formPetMerge->Hide();
	if (g_layerMain->m_formPetAbility->IsVisible())
		g_layerMain->m_formPetAbility->Hide();
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
void UIFormPetMergeBeads::Hide()
{
	m_pFmWindow->Hide();
}
void UIFormPetMergeBeads::OnPreDraw()
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

void UIFormPetMergeBeads::OnFinishMergeBeads(bool succeed)
{
	m_pBtnItem->SetBackgroundImage((RtwImage*)NULL);
	ItemID_SetInvalid(m_item);
	SetMergeBeadsButton();
	m_pet = GetPlayer()->mItem.m_PetBag.GetItemFromID(m_pet.id);
	Refresh();
}

bool UIFormPetMergeBeads::IsVisible()
{
	return m_pFmWindow->IsVisible();
}

void UIFormPetMergeBeads::Reset()
{
	m_pCbPetList->RemoveAllItems();
	m_pCbPetList->SetText("ÇëÑ¡Ôñ³èÎï");
	m_pBtnItem->SetBackgroundImage((RtwImage*)NULL);
	m_pLbRate->SetText("");
	for (int i=0;i<UIPET_LINKUP_BEADS_NUM;i++)
	{
		m_pLbBeads[i]->SetBackgroundImage((RtwImage*)NULL);
	}
	m_listPet.clear();
	ItemID_SetInvalid(m_pet);
	ItemID_SetInvalid(m_item);
	SetMergeBeadsButton();
}


void UIFormPetMergeBeads::DragOutItem(RtwButton *sender,void *)
{
	//if(!g_workspace.getMouseHover())
	{
		m_item = ItemID_CreateInvalid();

		sender->SetBackgroundImage((RtwImage*)NULL);
		SetMergeBeadsButton();
	}
}

void UIFormPetMergeBeads::OnSelectPet(RtwComboBox *sender,void *)
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
				m_pet = pet;
				SetMergeBeadsButton();
				Refresh();

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
			m_pdwSelectPet = id;
		}
	}
}

void UIFormPetMergeBeads::OnClickMergeBeads(RtwButton *sender,void *)
{
	if (!ItemIsPetSpectre(m_pet))
	{
		return;
	}
	if (!ItemID_IsValid(m_item) || CItemManager::GetIndexFromType(m_item.type) != PET_BEADS)
	{
		return;
	}

	CPetNetHelper::NetSend_c2r_fuse_pet_start(PET_FUSE_TYPE_MERGEBEADS,m_pet,m_item,m_item);
}

void UIFormPetMergeBeads::SetMergeBeadsButton()
{
	if (CanFusePet(GetWorld()->m_pItemManager,PET_FUSE_TYPE_MERGEBEADS,m_pet,m_item,m_item))
	{
		m_pBtnMergeBeads->Enable();
	}
	else
	{
		m_pBtnMergeBeads->Disable();
	}
}

void UIFormPetMergeBeads::SetItem(SItemID item)
{
	if (!ItemID_IsValid(item) || CItemManager::GetIndexFromType(item.type) != PET_BEADS)
	{
		PlayerFailOperate(R(MSG_NO_PET_BEADS));
		return;
	}
	m_item = item;
	SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(m_item.type);
	if(!pItemClass)	
		return;
	m_pBtnItem->SetBackgroundImage( g_workspace.getImageFactory()->createImage(pItemClass->icon) );
	SetMergeBeadsButton();
}

void UIFormPetMergeBeads::OnHide(RtwWidget *sender,void *)
{
	m_pFmPetView->ReleaseAllActor();
	g_layerMain->OnDeleteWidget(m_pFmWindow);
	m_pdwSelectPet = NULL;
}
