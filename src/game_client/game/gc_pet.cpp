
#include "gc_include.h"
#include "gc_pet.h"
#include "ui_form_pet_include.h"
#include "ui_form_char.h"

GcPet::GcPet(void)
{
	m_pPet = NULL;
	m_bChangeDisplay = false;
	m_lLinkPetItem = 0;
	m_bWaitRide = false;
}

GcPet::~GcPet(void)
{
}

void GcPet::CallOutPet(DWORD id, GcActor *pet)
{
	CBag *pBag = &GetPlayer()->mItem.m_PetBag;
	if (!pBag) return;

	SItemID item = pBag->GetItemFromID(id);
	if (ItemIsPet(item))
	{
		m_CurPetItem = item;
		SetPetActor(pet);
	}

	if (g_layerMain->m_formPetMain->IsVisible())
	{
		g_layerMain->m_formPetMain->Hide();
		g_layerMain->m_formPetMain->Show();
	}
}

void GcPet::SetPetItem(SItemID petItem)
{
	m_CurPetItem = petItem;
}

void GcPet::CallBackPet()
{
	// 首先先将上个宠物的buff清除
	int cCnt = 0;
	RtwButton* pButton;
	for (; cCnt<10; cCnt++)
	{
		rt2_snprintf(g_strStaticBuffer, 2048, "fmhpc.fmcBuff.btnbuff%d", cCnt+1);
		pButton = LOAD_UI_T(RtwButton, g_strStaticBuffer);
		pButton->Hide();
	}

	ItemID_SetInvalid(m_CurPetItem);
	SetPetActor(NULL);
	g_layerMain->m_formPetSimple->SetVisible(false);
	m_bChangeDisplay = false;
	m_bWaitRide = false;
	m_lLinkPetItem = 0;

	if (g_layerMain->m_formPetMain->IsVisible())
	{
		g_layerMain->m_formPetMain->Hide();
		g_layerMain->m_formPetMain->Show();
	}
}

SItemID GcPet::GetPetItem()
{
	return m_CurPetItem;
}

void GcPet::SetPetActor(GcActor *pet)
{ 
	int count = 0;

	if (pet)
	{
		if (m_pPet != pet)
		{
			g_layerMain->m_formPetCtrl->Reset();
		}

		m_pPet = pet;
		GetWorld()->m_pPet = pet;

		SItemID &petItem = m_CurPetItem;
		if ( !ItemIsPet(petItem) )
			return;

		m_pPet->m_Skill.ClearSkill(false);

		for (int i = PET_PARAM_SKILL_1;i<=PET_PARAM_SKILL_6;i++)
		{
			m_pPet->m_Skill.AddSkill(petItem.params[i], 0, false);
			GcSkill* pSkill = (GcSkill*)m_pPet->m_Skill.FindActiveSkill(petItem.params[i]);
			if (pSkill)
			{
				RtwImage *pImage = g_workspace.getImageFactory()->createImage(pSkill->m_pAttr->szIcon);
				if (pImage)
					g_layerMain->m_formPetCtrl->m_pBtnSkill[count]->SetBackgroundImage(pImage);
				g_layerMain->m_formPetCtrl->m_pBtnSkill[count]->SetParam1(&pSkill->m_pAttr->wID);
				pSkill->SetUIButton(g_layerMain->m_formPetCtrl->m_pBtnSkill[count]);
			}
			else
			{
				g_layerMain->m_formPetCtrl->m_pBtnSkill[count]->SetBackgroundImage((RtwImage*)NULL);
				g_layerMain->m_formPetCtrl->m_pBtnSkill[count]->SetParam1(NULL);
			}
			count++;
		}
		m_pPet->m_Skill.StartupSkill();
		g_layerMain->m_formPetCtrl->Show(m_CurPetItem);
	}
	else
	{
		g_layerMain->m_formPetCtrl->Hide();
		m_pPet = pet;
		GetWorld()->m_pPet = pet;
	}
	if (g_layerMain->m_formPetMain->IsVisible())
	{
		g_layerMain->m_formPetMain->Refresh();
	}
}

void GcPet::LinkUpWithPet(long petItemID)
{
	m_lLinkPetItem = petItemID;
	if (m_pPet)
	{
		m_pPet->m_bDisplay = m_lLinkPetItem == 0 ? true : false;
		m_pPet->DisplayActor(m_pPet->m_bDisplay);
		m_pPet->GetGraph()->EnableShadow(m_pPet->m_bDisplay);
		if (GetPlayer()->OnGetTarget() == m_pPet->ID())
		{
			GetPlayer()->OnSetTarget(0);
		}
		m_bChangeDisplay = false;
	}
	else
	{
		m_bChangeDisplay = true;
	}
}

void GcPet::OnRun(float fSecond)
{
	if (m_bChangeDisplay)
	{
		LinkUpWithPet(m_lLinkPetItem);
	}

	if (m_bWaitRide && m_pPet)
	{
		GetPlayer()->Ride(m_pPet,true);
		m_bWaitRide = false;
	}
}

void GcPet::ProcessSwitchScene()
{
	if (GetPlayer()->GetRidePet())
	{
		GetPlayer()->UnRide();
		m_bWaitRide = true;
	}

	if (m_lLinkPetItem)
	{
		m_bChangeDisplay = true;
	}

	GetWorld()->m_pPet = NULL;
	m_pPet = NULL;
}