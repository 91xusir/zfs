#include "gc_include.h"
#include "ui_form_pet.h"
#include "ui_form_pet_aptitude.h"
#include "ui_form_playeritems.h"
#include "ui_form_skill.h"
#include "gc_pet.h"
#include "ui_form_msg.h"

char PET_SOMELEV_IMAGE_1[] = "ui/ui_texture/pic_petlev01.gif";
char PET_SOMELEV_IMAGE_2[] = "ui/ui_texture/pic_petlev.gif";

char PET_BEADS_IMAGE_1[] = "ui/ui_texture/pic_petyhz.dds";
char PET_BEADS_IMAGE_2[] = "ui/ui_texture/pic_petyhz_1.dds";
char PET_BEADS_IMAGE_3[] = "ui/ui_texture/pic_petyhz_2.dds";
char PET_BEADS_IMAGE_4[] = "ui/ui_texture/pic_petyhz_3.dds";

char PET_NORMAL_IMAGE[] = "ui/ui_texture/pic_pettype_01.dds";
char PET_FAIRY_IMAGE[] = "ui/ui_texture/pic_pettype_02.dds";
char PET_SPECTRE_IMAGE[] = "ui/ui_texture/pic_pettype_03.dds";

UIFromPetMain::UIFromPetMain(void)
{
	m_pFormMain			= LOAD_UI_T(RtwForm, "fmconjure");
	m_pFormView			= LOAD_UI_T(Rtw3DView, "fmconjure.conjureform02");

	m_pBtnCallOut		= LOAD_UI_T(RtwButton, "fmconjure.fightblt");
	m_pBtnCallBack		= LOAD_UI_T(RtwButton, "fmconjure.changenameblt");
	m_pBtnAptitude		= LOAD_UI_T(RtwButton, "fmconjure.Qualityblt");
	m_pBtnChangeName	= LOAD_UI_T(RtwButton, "fmconjure.callforblt");

	m_pListPet			= LOAD_UI_T(RtwListBox, "fmconjure.conjureform01.conjurelist");
	m_pTexChangetName	= LOAD_UI_T(RtwTextBox, "fmconjure.conjuenamedit");

	m_pLabelLev			= LOAD_UI_T(RtwLabel, "fmconjure.lbtext03");
	m_pLabelHeartNeed		= LOAD_UI_T(RtwLabel, "fmconjure.lbtext04");
	m_pLabelHp			= LOAD_UI_T(RtwLabel, "fmconjure.conjureform03.lbhpcount");
	m_pLabelMp			= LOAD_UI_T(RtwLabel, "fmconjure.conjureform03.lbmpcount");
	m_pLabelExp			= LOAD_UI_T(RtwLabel, "fmconjure.conjureform03.lbexpcount");

	m_pLabelAtt			= LOAD_UI_T(RtwLabel, "fmconjure.conjureform03.lbpydamcount");
	m_pLabelDef			= LOAD_UI_T(RtwLabel, "fmconjure.conjureform03.lbpydefcount");
	m_pLabelHit			= LOAD_UI_T(RtwLabel, "fmconjure.conjureform03.lbhitcount");
	m_pLabelDodge		= LOAD_UI_T(RtwLabel, "fmconjure.conjureform03.lbdogecount");
	m_pLabelFireAtt		= LOAD_UI_T(RtwLabel, "fmconjure.conjureform03.lbfiredamcount");
	m_pLabelWaterAtt	= LOAD_UI_T(RtwLabel, "fmconjure.conjureform03.lbwaterdamcount");
	m_pLabelPosionAtt	= LOAD_UI_T(RtwLabel, "fmconjure.conjureform03.lbpoisondamcount");
	m_pLabelFireDef		= LOAD_UI_T(RtwLabel, "fmconjure.conjureform03.lbfiredefcount");
	m_pLabelWaterDef	= LOAD_UI_T(RtwLabel, "fmconjure.conjureform03.lbwaterdefcount");
	m_pLabelPosionDef	= LOAD_UI_T(RtwLabel, "fmconjure.conjureform03.lbpoisondefcount");

	m_pProExp = LOAD_UI_T(RtwProgressBar,"fmconjure.conjureform03.EXPBar");
	m_pProHp = LOAD_UI_T(RtwProgressBar,"fmconjure.conjureform03.HPBar");
	m_pProMp = LOAD_UI_T(RtwProgressBar,"fmconjure.conjureform03.MPBar");

	m_pLbPetTypeImage	= LOAD_UI_T(RtwLabel, "fmconjure.conjuetype");
	m_pLbPetTypeImage->ModifyFlags(wfGrabMouse|wfMouseMove|wfHint, 0);
	m_pLbPetTypeImage->EvHint += RTW_CALLBACK(this, UIFromPetMain, GetHintPetType);

	m_pLabelPetNum = LOAD_UI_T(RtwLabel,"fmconjure.lbtext2");

	m_pFormMain->EvHide += RTW_CALLBACK(this,UIFromPetMain,OnHide);
	m_pBtnCallOut->EvLClick		+= RTW_CALLBACK(this, UIFromPetMain, CallOutPet);
	m_pBtnCallBack->EvLClick	+= RTW_CALLBACK(this, UIFromPetMain, DeletePet);
	m_pBtnAptitude->EvLClick	+= RTW_CALLBACK(this, UIFromPetMain, CallAptitudeUI);
	m_pBtnChangeName->EvLClick	+= RTW_CALLBACK(this, UIFromPetMain, ChangePetName);

	m_pListPet->EvSelect		+= RTW_CALLBACK(this, UIFromPetMain, OnClickPetName);
	Reset();
	m_pTexChangetName->SetCapacity(PET_NAME_CHAR_NUM_MAX);
	m_dwSelectPet = 0;
}

UIFromPetMain::~UIFromPetMain(void)
{
}
void UIFromPetMain::GetHintPetType(void *,void *)
{
	if (ItemIsPet(m_CurPetItem))
	{
		string szType;
		if (ItemIsPetSpectre(m_CurPetItem))
			szType = "幽鬼";
		else if (ItemIsPetFairy(m_CurPetItem))
			szType = "仙兽";
		else
			szType = "普通";

		g_workspace.SetHintText(szType, true);
		g_workspace.AutoHintSize();
		g_workspace.AutoHintPosition();
	}
}
void UIFromPetMain::CallOutPet(void *,void *)
{
	GetWorld()->HideGuideForm();

	RtwListBox::SLine *pLine =  m_pListPet->getSelectLineItem();
	if (pLine)
	{
		int *id = (int*)pLine->param;
		if (id)
		{
			if (*id == GetPlayerPet()->GetPetItem().id)
			{
				SendCtrlToServer(PET_CTRL_CALLBACK);
			}
			else
			{
				SendCtrlToServer(PET_CTRL_CALLOUT,*id);
			}
		}
	}
}

void UIFromPetMain::DeletePet(void *,void *)
{
	RtwListBox::SLine *pLine =  m_pListPet->getSelectLineItem();
	if (pLine)
	{
		int *id = (int*)pLine->param;
		if (id)
		{
			CBag *pBag = &GetPlayer()->mItem.m_PetBag;
			if (!pBag) return;
			m_CurPetItem = pBag->GetItemFromID(*id);
			char str[128];
			rt2_sprintf(str,"您确定要删除 %s 吗?",m_CurPetItem.name.c_str());
			UIFormMsg *pMsgBox = UIFormMsg::ShowStatic(str, UIFormMsg::TYPE_OK_CANCEL,true);
			pMsgBox->EvOK += RTW_CALLBACK(this, UIFromPetMain, RealDeletePet);
			pMsgBox->GetButtonOK(UIFormMsg::TYPE_OK_CANCEL)->SetParam1(id);
		}
	}
}

void UIFromPetMain::RealDeletePet(RtwWidget *sender,void *)
{
	int *id = (int*)sender->getParam1();
	if (id)
	{
		SendCtrlToServer(PET_CTRL_DELETE_PET,*id);
	}
}

void UIFromPetMain::CallAptitudeUI(void *,void *)
{
	if (g_layerMain->m_formPetAptitude->IsVisible())
	{
		g_layerMain->m_formPetAptitude->Hide();
	}
	else if (ItemIsPet(m_CurPetItem))
	{
		g_layerMain->m_formPetAptitude->Show(m_CurPetItem);
	}
}

void UIFromPetMain::ChangePetName(void *,void *)
{
	std::string name = m_pTexChangetName->getText();
	// 检查名字

	if (name.size() > PET_NAME_CHAR_NUM_MAX)
	{
		return;
	}
	if (name.size() < PET_NAME_CHAR_NUM_MIN)
	{
		PlayerFailOperate(R(PET_NAME_TOO_SHORT));
		return;
	}

	RtwListBox::SLine *pLine =  m_pListPet->getSelectLineItem();
	if (pLine)
	{
		DWORD *id = (DWORD*)pLine->param;
		if (id)
		{
			CG_CmdPacket *cmd = NetBeginWrite();
			cmd->WriteShort(c2r_ctrl_pet);
			cmd->WriteByte(PET_CTRL_CHANGE_NAME);
			cmd->WriteLong(*id);
			cmd->WriteString(name.c_str());
			NetSend(cmd);
		}
	}
}

void SendCtrlToServer(const unsigned char command,DWORD param1/* = 0*/,DWORD param2/* = 0*/)
{
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_ctrl_pet);
	cmd->WriteByte(command);
	if (command == PET_CTRL_SET_DEFSKILL || 
		command == PET_CTRL_USE_SKILL || 
		command == PET_CTRL_CALLOUT || 
		command == PET_CTRL_DELETE_PET ||
		command == PET_CTRL_ATTACK)
	{
		cmd->WriteLong(param1);
	}
	if (command == PET_CTRL_USE_SKILL)
	{
		cmd->WriteLong(param2);
	}
	NetSend(cmd);
}

void UIFromPetMain::OnClickPetName(void *,void *)
{
	CBag *pBag = &GetPlayer()->mItem.m_PetBag;
	if (!pBag) return;

	RtwListBox::SLine *pLine =  m_pListPet->getSelectLineItem();
	if (pLine)
	{
		int *id = (int*)pLine->param;
		if (id)
		{
			m_dwSelectPet = *id;
			m_pFormView->ReleaseAllActor();
			m_CurPetItem = pBag->GetItemFromID(*id);
			if (ItemIsPet(m_CurPetItem))
			{
				SItemPet *pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(m_CurPetItem.type);
				SPet* info = g_TablePet.FindPet(pItemPet->petID);
				int modelID = 0;

				if(info)
				{
					for(int i=0; i<MAX_PET_MODEL_CHANGE; i++)
					{
						if(m_CurPetItem.params[PET_PARAM_LEV] <= info->ModelLevel[i])
						{
							modelID = info->ModelChange[i];
							break;
						}
					}
					SCreModel *model = g_TableCreModel.FindCreModel(modelID);
					if (model)
					{
						int cont = m_pFormView->AddCreateActor((char*)model->Act.c_str());
						CRT_ActorInstance *p = m_pFormView->GetActorByCont(cont);
						p->PlayPose("wait",true);
					}
				}
				Refresh();
			}
		}
	}
}

void UIFromPetMain::OnPreDraw()
{
	if (!IsVisible()) return;
	Reset();
	m_pListPet->RemoveAllItems();
	m_listPet.clear();

	CBag *pBag = &GetPlayer()->mItem.m_PetBag;

	if (!pBag) return;

	char str[128];
	rt2_snprintf(str,128,"%d/%d",pBag->GetBagItemNum(),3 + GetPlayer()->m_core.GradeLev / 20);
	m_pLabelPetNum->SetText(str);

	SItemID item;
	SItemPet *pItemPet = NULL;
	SPet *pPet = NULL;
	int id = 0;

	DWORD SelectID = 0;
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
					std::string name = item.name;
					//if (item.id == GetPlayerPet()->GetPetItem().id)
					//	name += " (出战)";
					m_pListPet->AddItem(name);
					m_pListPet->SetLineParam(id,&m_listPet.back().id);
					id++;

					if (GetPlayerPet()->GetPetActor() && GetPlayerPet()->GetPetItem().id == m_listPet.back().id)
						SelectID = m_listPet.back().id;
				}
			}
		}
	}

	if (m_dwSelectPet) 
	{
		SelectID = m_dwSelectPet;
		m_dwSelectPet = 0;
	}

	if (SelectID)
		m_pListPet->SetSelectLineByParam(SelectID);
	else
		m_pListPet->SetSelectedItem(0);

	RtwListBox::SLine *pLine =  m_pListPet->getSelectLineItem();
	if (pLine)
	{
		int *id = (int*)pLine->param;
		if (id)
		{
			m_CurPetItem = pBag->GetItemFromID(*id);
			if (ItemIsPet(m_CurPetItem))
			{
				Refresh();
			}
		}
	}
}

void UIFromPetMain::Refresh()
{
	if (!ItemIsPet(m_CurPetItem)) 
	{
		if (GetPlayerPet()->GetPetActor() && ItemIsPet(GetPlayerPet()->GetPetItem()))
			m_CurPetItem = GetPlayerPet()->GetPetItem();
		else
			return;
	}

	SItemPet *pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(m_CurPetItem.type);
	if (!pItemPet) return;

	SPet *pPet = g_TablePet.FindPet(pItemPet->petID);
	if (!pPet) return;

	char str[128];

	if (GetPlayerPet()->GetPetActor() && m_CurPetItem.id == GetPlayerPet()->GetPetItem().id)
	{
		m_pBtnCallOut->SetText("收回");

		SCreature *pCore = &GetPlayerPet()->GetPetActor()->m_core;

		if (!pCore) return;

		if (g_workspace.GetFocusWidget() != m_pTexChangetName)
		{
			m_pTexChangetName->SetText(pCore->Name);
		}
		m_pLabelHeartNeed->SetText(rtFormatNumber(pPet->NeedCulLev));

		m_pLabelLev->SetText(rtFormatNumber(pCore->Lev));

		rt2_sprintf(str,"%d/%d",pCore->Hp,pCore->MaxHp.GetValue());
		m_pLabelHp->SetText(str);
		m_pProHp->SetValue((float)pCore->Hp / pCore->MaxHp.GetValue());

		rt2_sprintf(str,"%d/%d",pCore->Mp,pCore->MaxMp.GetValue());
		m_pLabelMp->SetText(str);
		m_pProMp->SetValue((float)pCore->Mp / (float)pCore->MaxMp.GetValue());

		rt2_sprintf(str,"%d/%d",pCore->Exp,g_TableExp.GetNextLevExp(pCore->Lev) / 3);
		m_pLabelExp->SetText(str);
		m_pProExp->SetValue((float)pCore->Exp / ((float)g_TableExp.GetNextLevExp(pCore->Lev) / 3.f));


		rt2_sprintf(str,"%d-%d",pCore->DamageMin.GetValue(),pCore->DamageMax.GetValue());
		m_pLabelAtt->SetText(str);
		m_pLabelDef->SetText(rtFormatNumber(pCore->Armor.GetValue()));
		m_pLabelHit->SetText(rtFormatNumber(pCore->Attack.GetValue()));
		m_pLabelDodge->SetText(rtFormatNumber(pCore->Dodge.GetValue()));

		m_pLabelWaterAtt->SetText(rtFormatNumber(pCore->ElementDamage[ELEMENT_WATER].GetValue() / 4));
		m_pLabelFireAtt->SetText(rtFormatNumber(pCore->ElementDamage[ELEMENT_FIRE].GetValue() / 4));
		m_pLabelPosionAtt->SetText(rtFormatNumber(pCore->ElementDamage[ELEMENT_POISON].GetValue() / 4));
		m_pLabelWaterDef->SetText(rtFormatNumber(pCore->ElementArmor[ELEMENT_WATER].GetValue()));
		m_pLabelFireDef->SetText(rtFormatNumber(pCore->ElementArmor[ELEMENT_FIRE].GetValue()));
		m_pLabelPosionDef->SetText(rtFormatNumber(pCore->ElementArmor[ELEMENT_POISON].GetValue()));
	}
	else
	{
		m_pBtnCallOut->SetText("出战");

		float FHp = 0.f;
		float FMp = 0.f;
		float FAttack = 0.f;
		float FDamage = 0.f;
		float FDodge = 0.f;
		float FArmor = 0.f;
		float ElementArmorRate[3];
		float ElementAttackRate[3];

		if (!GetPetInfoFromItem(m_CurPetItem,FHp,FMp,FAttack,FDamage,FDodge,FArmor,ElementArmorRate,ElementAttackRate))
			return;

		SBaseValue *base = g_TableBaseValue.Find(GetRealLevel(m_CurPetItem.params[PET_PARAM_LEV]));
		if(!base) return;

		CAttrValue DamageMin;
		CAttrValue DamageMax;
		CAttrValue Attack;
		CAttrValue Armor;
		CAttrValue Dodge;
		CAttrValue MaxHp;
		CAttrValue MaxMp;
		DamageMin.Base = (base->FDamage * FDamage + pPet->AddedDamage) * (1-pPet->DamageFloat);
		DamageMax.Base = (base->FDamage * FDamage + pPet->AddedDamage) * (1+pPet->DamageFloat);
		Attack.Base    = base->FAttack * FAttack + pPet->AddedAttack;
		Armor.Base     = base->FArmor * FArmor + pPet->AddedArmor;
		Dodge.Base     = base->FDodge * FDodge + pPet->AddedDodge;
		MaxHp.Base     = base->FHp * FHp + pPet->AddedHp;
		MaxMp.Base     = base->FMp * FMp + pPet->AddedMp;

		m_pTexChangetName->SetText(m_CurPetItem.name);
		m_pLabelHeartNeed->SetText(rtFormatNumber(pPet->NeedCulLev));

		m_pLabelLev->SetText(rtFormatNumber(m_CurPetItem.params[PET_PARAM_LEV]));

		rt2_sprintf(str,"%d/%d",m_CurPetItem.params[PET_PARAM_HP],MaxHp.GetValue());
		m_pLabelHp->SetText(str);
		m_pProHp->SetValue((float)m_CurPetItem.params[PET_PARAM_HP] / (float)MaxHp.GetValue());

		rt2_sprintf(str,"%d/%d",m_CurPetItem.params[PET_PARAM_MP],MaxMp.GetValue());
		m_pLabelMp->SetText(str);
		m_pProMp->SetValue((float)m_CurPetItem.params[PET_PARAM_MP] / (float)MaxMp.GetValue());

		rt2_sprintf(str,"%d/%d",m_CurPetItem.params[PET_PARAM_EXP],g_TableExp.GetNextLevExp(m_CurPetItem.params[PET_PARAM_LEV]) / 3);
		m_pLabelExp->SetText(str);
		m_pProExp->SetValue((float)m_CurPetItem.params[PET_PARAM_EXP] / ((float)g_TableExp.GetNextLevExp(m_CurPetItem.params[PET_PARAM_LEV]) / 3.f));


		rt2_sprintf(str,"%d-%d",DamageMin.GetValue(),DamageMax.GetValue());
		m_pLabelAtt->SetText(str);
		m_pLabelDef->SetText(rtFormatNumber(Armor.GetValue()));
		m_pLabelHit->SetText(rtFormatNumber(Attack.GetValue()));
		m_pLabelDodge->SetText(rtFormatNumber(Dodge.GetValue()));

		int nValue = (m_CurPetItem.params[PET_PARAM_LEV]-1+10) * 2 * ElementAttackRate[ELEMENT_WATER];
		m_pLabelWaterAtt->SetText(rtFormatNumber(nValue / 4));
		nValue = (m_CurPetItem.params[PET_PARAM_LEV]-1+10) * 2 * ElementAttackRate[ELEMENT_FIRE];
		m_pLabelFireAtt->SetText(rtFormatNumber(nValue / 4));
		nValue = (m_CurPetItem.params[PET_PARAM_LEV]-1+10) * 2 * ElementAttackRate[ELEMENT_POISON];
		m_pLabelPosionAtt->SetText(rtFormatNumber(nValue / 4));
		nValue = ((m_CurPetItem.params[PET_PARAM_LEV]-1)*1+10) * 2 * ElementArmorRate[ELEMENT_WATER];
		m_pLabelWaterDef->SetText(rtFormatNumber(nValue));
		nValue = ((m_CurPetItem.params[PET_PARAM_LEV]-1)*1+10) * 2 * ElementArmorRate[ELEMENT_FIRE];
		m_pLabelFireDef->SetText(rtFormatNumber(nValue));
		nValue = ((m_CurPetItem.params[PET_PARAM_LEV]-1)*1+10) * 2 * ElementArmorRate[ELEMENT_POISON];
		m_pLabelPosionDef->SetText(rtFormatNumber(nValue));
	}

	if (ItemIsPetNormal(m_CurPetItem))
	{
		RtwImage *pImage = g_workspace.getImageFactory()->createImage(PET_NORMAL_IMAGE);
		if (pImage)
		{
			pImage->SetBlend(true);
			m_pLbPetTypeImage->SetBackgroundImage(pImage);
		}
	}
	else if (ItemIsPetFairy(m_CurPetItem))
	{
		RtwImage *pImage = g_workspace.getImageFactory()->createImage(PET_FAIRY_IMAGE);
		if (pImage)
		{
			pImage->SetBlend(true);
			m_pLbPetTypeImage->SetBackgroundImage(pImage);
		}
	}
	else if (ItemIsPetSpectre(m_CurPetItem))
	{
		RtwImage *pImage = g_workspace.getImageFactory()->createImage(PET_SPECTRE_IMAGE);
		if (pImage)
		{
			pImage->SetBlend(true);
			m_pLbPetTypeImage->SetBackgroundImage(pImage);
		}
	}

	if (g_layerMain->m_formPetAptitude->IsVisible())
	{
		g_layerMain->m_formPetAptitude->Show(m_CurPetItem);
	}
}
void UIFromPetMain::Show()
{
	if (IsVisible() && m_pBtnCallOut->getEnable())
	{
		Refresh();
	}
	else
	{
		m_pBtnCallOut->Enable();
		m_pBtnCallBack->Enable();
		m_pBtnChangeName->Enable();
		m_pFormMain->Show();
		OnPreDraw();
	}
}

void UIFromPetMain::SetSelectPet(long id)
{
	if (!IsVisible())
		return;
}

bool UIFromPetMain::IsSelectCalledPet()
{
	if (GetPlayerPet()->GetPetActor() && m_CurPetItem.id == GetPlayerPet()->GetPetItem().id)
		return true;
	return false;
}

void UIFromPetMain::Show(SItemID pet)
{
	if (!IsVisible() || pet.id != m_CurPetItem.id)
	{
		m_pFormMain->Show();
		m_pBtnCallOut->Disable();
		m_pBtnCallBack->Disable();
		m_pBtnChangeName->Disable();
		Reset();
		m_pListPet->RemoveAllItems();
		m_listPet.clear();

		CBag *pBag = &GetPlayer()->mItem.m_PetBag;

		if (!pBag) return;

		SItemPet *pItemPet = NULL;
		SPet *pPet = NULL;
		int id = 0;

		if (ItemIsPet(pet))
		{
			pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(pet.type);
			if (pItemPet)
			{
				pPet = g_TablePet.FindPet(pItemPet->petID);
				if (pPet)
				{
					m_listPet.push_back(pet);
					m_pListPet->AddItem(pet.name);
					m_pListPet->SetLineParam(id,&m_listPet.back().id);
				}
			}
		}

		m_pListPet->SetSelectedItem(0);

		if (ItemIsPet(pet))
		{
			m_CurPetItem = pet;
			m_pFormView->ReleaseAllActor();
			SItemPet *pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(pet.type);
			SNpc* pNpc = g_TableNpc.FindNpc(pItemPet->petID);
			if(pNpc)
			{
				SCreModel *model = g_TableCreModel.FindCreModel(pNpc->ModelId);
				if (model)
				{
					int cont = m_pFormView->AddCreateActor((char*)model->Act.c_str());
					CRT_ActorInstance *p = m_pFormView->GetActorByCont(cont);
					p->PlayPose("wait",true);
				}
			}
			Refresh();
		}
	}
	else
	{
		Hide();
	}
}
void UIFromPetMain::Hide()
{
	GetWorld()->HideGuideForm();

	m_pFormMain->Hide();
	Reset();
	m_dwSelectPet = 0;
}

bool UIFromPetMain::IsVisible()
{
	return m_pFormMain->IsVisible();
}

void UIFromPetMain::Reset()
{
	m_pTexChangetName->SetText("");

	m_pLabelLev->SetText("");
	m_pLabelHeartNeed->SetText("");
	m_pLabelHp->SetText("");
	m_pLabelMp->SetText("");
	m_pLabelExp->SetText("");

	m_pLabelAtt->SetText("");
	m_pLabelDef->SetText("");
	m_pLabelHit->SetText("");
	m_pLabelDodge->SetText("");
	m_pLabelFireAtt->SetText("");
	m_pLabelWaterAtt->SetText("");
	m_pLabelPosionAtt->SetText("");
	m_pLabelFireDef->SetText("");
	m_pLabelWaterDef->SetText("");
	m_pLabelPosionDef->SetText("");
	m_pBtnCallOut->SetText("出战");

	m_pProExp->SetValue(0.f);
	m_pProHp->SetValue(0.f);
	m_pProMp->SetValue(0.f);
	ItemID_SetInvalid(m_CurPetItem);
	m_pLbPetTypeImage->SetBackgroundImage((RtwImage*)NULL);
	m_pFormView->ReleaseAllActor();
}

void UIFromPetMain::OnHide(RtwWidget *sender,void *)
{
	m_pFormView->ReleaseAllActor();
}