#include "gc_include.h"
#include "ui_form_pet_include.h"
#include "ui_form_pet.h"
#include "ui_form_playeritems.h"
#include "ui_form_skill.h"
#include "gc_pet.h"


UIFormPetMerge::UIFormPetMerge()
{
	m_pFmWindow		= LOAD_UI_T(RtwForm, "fmpetcompose");
	m_pFmPetView1	= LOAD_UI_T(Rtw3DView, "fmpetcompose.fmbaby.fmmianpet");
	m_pFmPetView2	= LOAD_UI_T(Rtw3DView, "fmpetcompose.fmbaby.fmotherpet");
	m_pCbPetList1	= LOAD_UI_T(RtwComboBox, "fmpetcompose.fmbaby.cbtlist1");
	m_pCbPetList2	= LOAD_UI_T(RtwComboBox, "fmpetcompose.fmbaby.cbtlist2");
	m_pBtnItem		= LOAD_UI_T(RtwButton, "fmpetcompose.fmbaby.aliasCombweapon");
	m_pBtnMerge		= LOAD_UI_T(RtwButton, "fmpetcompose.fmbaby.btncompose");
	m_pLbRate		= LOAD_UI_T(RtwLabel, "fmpetcompose.fmbaby.lbmoney");
	m_pFmWindow->EvHide += RTW_CALLBACK(this,UIFormPetMerge,OnHide);

	m_pCbPetList1->EvSelect	+= RTW_CALLBACK(this, UIFormPetMerge, OnSelectPet1);
	m_pCbPetList2->EvSelect	+= RTW_CALLBACK(this, UIFormPetMerge, OnSelectPet2);
	m_pBtnMerge->EvLClick += RTW_CALLBACK(this, UIFormPetMerge, OnClickMerge);

	m_pBtnItem->EvDragEnd	+= RTW_CALLBACK(this, UIFormPetMerge, DragOutItem);
	m_pBtnItem->EvRClick	+= RTW_CALLBACK(this, UIFormPetMerge, DragOutItem);
	m_pBtnItem->EvHint		+= RTW_CALLBACK(this, UIFormPetAbility, OnGetHintItemText);
	m_pBtnItem->SetParam1(&m_item.id);
	m_pBtnItem->ModifyFlags(wfDragOut | wfHint, 0);
	Reset();
	m_pdwSelectPet1 = NULL;
	m_pdwSelectPet2 = NULL;
}

UIFormPetMerge::~UIFormPetMerge()
{

}

void UIFormPetMerge::Refresh()
{
	if (ItemIsPetNormal(m_pet1) && ItemIsPetNormal(m_pet2))
	{
		float fRate;
		FusePetRate(PET_FUSE_TYPE_MERGE,m_pet1,m_pet2,&fRate);
		char str[32];
		rt2_sprintf(str,"%d%%",int(fRate*100.f));
		m_pLbRate->SetText(str);
	}
}

void UIFormPetMerge::Show()
{
	if (g_layerMain->m_formPetMergeBeads->IsVisible())
		g_layerMain->m_formPetMergeBeads->Hide();
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
void UIFormPetMerge::Hide()
{
	m_pFmWindow->Hide();
}
void UIFormPetMerge::OnPreDraw()
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
					m_pCbPetList1->AddListItem(item.name);
					m_pCbPetList1->SetLineParam(id,&m_listPet.back().id);
					m_pCbPetList2->AddListItem(item.name);
					m_pCbPetList2->SetLineParam(id,&m_listPet.back().id);
					id++;
				}
			}
		}
	}
	
}

void UIFormPetMerge::OnFinishMerge(bool succeed)
{
	OnPreDraw();
}

bool UIFormPetMerge::IsVisible()
{
	return m_pFmWindow->IsVisible();
}

void UIFormPetMerge::Reset()
{
	m_pCbPetList1->RemoveAllItems();
	m_pCbPetList2->RemoveAllItems();
	m_pCbPetList1->SetText("请选择宠物");
	m_pCbPetList2->SetText("请选择宠物");
	m_pBtnItem->SetBackgroundImage((RtwImage*)NULL);
	m_pLbRate->SetText("");
	m_listPet.clear();
	ItemID_SetInvalid(m_pet1);
	ItemID_SetInvalid(m_pet2);
	ItemID_SetInvalid(m_item);
	SetMergeButton();
}

void UIFormPetMerge::SetItem(SItemID item)
{
	if (!ItemIsPetNormal(m_pet1) || !ItemIsPetNormal(m_pet2))
	{
		PlayerFailOperate(R(MSG_NEED_MERGE_PET));
		return;
	}
	if (!ItemID_IsValid(item) || CItemManager::GetIndexFromType(item.type) != PET_MERGE_NEED_ITEM)
	{
		PlayerFailOperate(R(MSG_NO_MERGE_NEED_ITEM));
		return;
	}
	//SSorption *pItemNeed = (SSorption*)GetWorld()->m_pItemManager->GetItem(item.type);
	//if (pItemNeed->param[0] < m_pet1.params[PET_PARAM_FUSE_LEV]+1)
	//{
	//	PlayerFailOperate(R(MSG_MERGE_ITEM_LEV_MIN));
	//	return;
	//}
	m_item = item;
	SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(m_item.type);
	if(!pItemClass)	
		return;
	m_pBtnItem->SetBackgroundImage( g_workspace.getImageFactory()->createImage(pItemClass->icon) );
	SetMergeButton();
}

void UIFormPetMerge::DragOutItem(RtwButton *sender,void *)
{
	//if(!g_workspace.getMouseHover())
	{
		m_item = ItemID_CreateInvalid();

		sender->SetBackgroundImage((RtwImage*)NULL);
		SetMergeButton();
	}
}

void UIFormPetMerge::SetMergeButton()
{
	if (CanFusePet(GetWorld()->m_pItemManager,PET_FUSE_TYPE_MERGE,m_pet1,m_pet2,m_item))
	{
		m_pBtnMerge->Enable();
	}
	else
	{
		m_pBtnMerge->Disable();
	}
}
void UIFormPetMerge::OnSelectPet1(RtwComboBox *sender,void *)
{
	CBag *pBag = &GetPlayer()->mItem.m_PetBag;
	if (!pBag) return;
	RtwListBox::SLine *pLine =  m_pCbPetList1->GetSelectLine();
	if (pLine)
	{
		DWORD *id = (DWORD*)pLine->param;
		if (id)
		{
			if (*id == m_pet2.id)
			{
				m_pCbPetList1->SetSelectedItem();
				PlayerFailOperate(R(MSG_SELECT_MERGE_SLAVE_PET));
				return;
			}
			SItemID pet = pBag->GetItemFromID(*id);
			if (ItemIsPetNormal(pet))
			{
				if (GetPlayerPet() && GetPlayerPet()->GetPetActor() && GetPlayerPet()->GetPetItem().id == pet.id)
				{
					m_pCbPetList1->SetSelectedItem();
					PlayerFailOperate(R(MSG_PET_CANNOT_FUSE_FIGHT));
					return;
				}
				if (pet.params[PET_PARAM_LEV] < 30)
				{
					m_pCbPetList1->SetSelectedItem();
					PlayerFailOperate(R(MSG_PET_LEV_NO_30));
					return;
				}
				if (pet.params[PET_PARAM_FUSE_LEV] >= 4)
				{
					m_pCbPetList1->SetSelectedItem();
					PlayerFailOperate(R(MSG_PET_MERGE_LEV_4));
					return;
				}
				if (ItemIsPetNormal(m_pet2) && pet.params[PET_PARAM_FUSE_LEV] != m_pet2.params[PET_PARAM_FUSE_LEV])
				{
					m_pCbPetList1->SetSelectedItem();
					PlayerFailOperate(R(MSG_PET_LEV_NO_SAME));
					return;
				}
				m_pet1 = pet;
				SetMergeButton();
				Refresh();
				m_pdwSelectPet1 = id;
				m_pFmPetView1->ReleaseAllActor();
				SItemPet *pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(m_pet1.type);
				SPet* info = g_TablePet.FindPet(pItemPet->petID);
				int modelID = 0;

				if(info)
				{
					for(int i=0; i<MAX_PET_MODEL_CHANGE; i++)
					{
						if(m_pet1.params[PET_PARAM_LEV] <= info->ModelLevel[i])
						{
							modelID = info->ModelChange[i];
							break;
						}
					}
					SCreModel *model = g_TableCreModel.FindCreModel(modelID);
					if (model)
					{
						int cont = m_pFmPetView1->AddCreateActor((char*)model->Act.c_str());
						CRT_ActorInstance *p = m_pFmPetView1->GetActorByCont(cont);
						p->PlayPose("wait",true);
					}
				}
			}
			else
			{
				m_pCbPetList1->SetSelectedItem();
				PlayerFailOperate(R(MSG_NEED_MERGE_PET));
			}
		}
	}
}

void UIFormPetMerge::OnSelectPet2(RtwComboBox *sender,void *)
{
	CBag *pBag = &GetPlayer()->mItem.m_PetBag;
	if (!pBag) return;
	RtwListBox::SLine *pLine =  m_pCbPetList2->GetSelectLine();
	if (pLine)
	{
		DWORD *id = (DWORD*)pLine->param;
		if (id)
		{
			if (*id == m_pet1.id)
			{
				m_pCbPetList2->SetSelectedItem();
				PlayerFailOperate(R(MSG_SELECT_MERGE_MAIN_PET));
				return;
			}
			SItemID pet = pBag->GetItemFromID(*id);
			if (ItemIsPetNormal(pet))
			{
				if (GetPlayerPet() && GetPlayerPet()->GetPetActor() && GetPlayerPet()->GetPetItem().id == pet.id)
				{
					m_pCbPetList2->SetSelectedItem();
					PlayerFailOperate(R(MSG_PET_CANNOT_FUSE_FIGHT));
					return;
				}
				if (pet.params[PET_PARAM_LEV] < 30)
				{
					m_pCbPetList2->SetSelectedItem();
					PlayerFailOperate(R(MSG_PET_LEV_NO_30));
					return;
				}
				if (pet.params[PET_PARAM_FUSE_LEV] >= 4)
				{
					m_pCbPetList2->SetSelectedItem();
					PlayerFailOperate(R(MSG_PET_MERGE_LEV_4));
					return;
				}
				if (ItemIsPetNormal(m_pet1) && pet.params[PET_PARAM_FUSE_LEV] != m_pet1.params[PET_PARAM_FUSE_LEV])
				{
					m_pCbPetList2->SetSelectedItem();
					PlayerFailOperate(R(MSG_PET_LEV_NO_SAME));
					return;
				}
				m_pet2 = pet;
				SetMergeButton();
				Refresh();
				m_pdwSelectPet2 = id;

				m_pFmPetView2->ReleaseAllActor();
				SItemPet *pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(m_pet2.type);
				SPet* info = g_TablePet.FindPet(pItemPet->petID);
				int modelID = 0;

				if(info)
				{
					for(int i=0; i<MAX_PET_MODEL_CHANGE; i++)
					{
						if(m_pet2.params[PET_PARAM_LEV] <= info->ModelLevel[i])
						{
							modelID = info->ModelChange[i];
							break;
						}
					}
					SCreModel *model = g_TableCreModel.FindCreModel(modelID);
					if (model)
					{
						int cont = m_pFmPetView2->AddCreateActor((char*)model->Act.c_str());
						CRT_ActorInstance *p = m_pFmPetView2->GetActorByCont(cont);
						p->PlayPose("wait",true);
					}
				}
			}
			else
			{
				m_pCbPetList2->SetSelectedItem();
				PlayerFailOperate(R(MSG_NEED_MERGE_PET));
			}
		}
	}
}

void UIFormPetMerge::OnClickMerge(RtwButton *sender,void *)
{
	if (!ItemIsPetNormal(m_pet1))
	{
		return;
	}
	if (!ItemIsPetNormal(m_pet2))
	{
		return;
	}
	if (!ItemID_IsValid(m_item) || CItemManager::GetIndexFromType(m_item.type) != PET_MERGE_NEED_ITEM)
	{
		return;
	}

	CPetNetHelper::NetSend_c2r_fuse_pet_start(PET_FUSE_TYPE_MERGE,m_pet1,m_pet2,m_item);

}

void UIFormPetMerge::OnHide(RtwWidget *sender,void *)
{
	m_pFmPetView1->ReleaseAllActor();
	m_pFmPetView2->ReleaseAllActor();
	g_layerMain->OnDeleteWidget(m_pFmWindow);
	m_pdwSelectPet1 = NULL;
	m_pdwSelectPet2 = NULL;
}
