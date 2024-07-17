#include "gc_include.h"
#include "ui_form_pkmode.h"

UIFormPkMode::UIFormPkMode()
{
	guard;
	m_pfrmThis			= RTW_WIDGET("fmpk");
	m_plstPKmode		= RTW_WIDGET_T(RtwListBox,"fmpk.lstPKmode");
	m_plstPKmode->AddItem();
	m_plstPKmode->SetItemText(0,1,"练级模式");
	RtwImage* pImage0 = g_workspace.getImageFactory()->createImage("ui\\textures\\5.tga");
	pImage0->SetSourceRect(RtwRect(79,250,98,269));
	pImage0->SetBlend(true);
	m_plstPKmode->SetLineItemImage(0, 0, pImage0);

	m_plstPKmode->AddItem();
	m_plstPKmode->SetItemText(1,1,"善恶模式");
	RtwImage* pImage1 = g_workspace.getImageFactory()->createImage("ui\\textures\\5.tga");;
	pImage1->SetSourceRect(RtwRect(79,270,98,289));
	pImage1->SetBlend(true);
	m_plstPKmode->SetLineItemImage(1, 0, pImage1);

	m_plstPKmode->AddItem();
	RtwImage* pImage2 = g_workspace.getImageFactory()->createImage("ui\\textures\\5.tga");;
	m_plstPKmode->SetItemText(2,1,"自由模式");
	pImage2->SetSourceRect(RtwRect(79,290,98,309));
	pImage2->SetBlend(true);
	m_plstPKmode->SetLineItemImage(2, 0, pImage2);
	m_plstPKmode->EvSelect		+= RTW_CALLBACK(this,UIFormPkMode,OnChangePkMode);
	m_plstPKmode->Hide();
	m_dwSwitchFreeTime = 0;
	unguard;
}

UIFormPkMode::~UIFormPkMode()
{
	guard;
	unguard;
}

void UIFormPkMode::OnPreDraw()
{
	guard;
	unguard;
}

void UIFormPkMode::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pfrmThis->Show();
	}
	unguard;
}

void UIFormPkMode::Show()
{
	guard;
	OnPreDraw();
	m_pfrmThis->Show();
	unguard;
}

bool UIFormPkMode::IsVisible()
{
	guard;
	if ( m_pfrmThis && m_pfrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormPkMode::Hide()
{
	guard;
	if (m_pfrmThis)		m_pfrmThis->Hide();
	unguard;
}


void UIFormPkMode::OnChangePkModeImage(EPkMode mode)
{
	if (m_dwSwitchFreeTime > rtGetMilliseconds())
	{
		m_pfrmThis->Hide();
		PlayerFailOperate(R(MSG_CANNOT_SWITCH_PKMODE));
		return;
	}
	RtwRect imgTmp;
	int iSelect = 0;
	switch(mode)
	{
	case PK_Monster:
		GetWorld()->m_pPlayer->m_PkMode = PK_Monster;
		imgTmp = RtwRect(79,250,98,269);
		iSelect = 0;
		break;
	case PK_Goodevil:
		GetWorld()->m_pPlayer->m_PkMode = PK_Goodevil;
		imgTmp = RtwRect(79,270,98,289);
		iSelect = 1;
		break;
	case PK_Free:
		GetWorld()->m_pPlayer->m_PkMode = PK_Free;
		imgTmp = RtwRect(79,290,98,309);
		iSelect = 2;
		m_dwSwitchFreeTime = rtGetMilliseconds() + 60000;
		break;
	}
	static RtwImage* pImage = g_workspace.getImageFactory()->createImage("ui\\textures\\5.tga");
	pImage->SetSourceRect(imgTmp);
	pImage->SetBlend(true);
	RTW_WIDGET("fmhp.btnPKmode")->SetHintText(m_plstPKmode->GetItemText(iSelect,1));
	RTW_WIDGET("fmhp.btnPKmode")->SetBackgroundImage(pImage);
	m_pfrmThis->Hide();
}

void UIFormPkMode::OnChangePkMode(RtwWidget* sender, void*)
{
	guard;
	if ( GetPlayer()->m_bIsCombat )
	{
		m_pfrmThis->Hide();
		PlayerFailOperate(R(MSG_COMBAT_CHANGE_PKMODE));
		return;
	}
	int iSelect = m_plstPKmode->GetSelectedItem();
	if(iSelect < 0 || iSelect > 2)
		return;
	RtwRect imgTmp;

	if (iSelect != 2)
	{
		if (m_dwSwitchFreeTime > rtGetMilliseconds())
		{
			m_pfrmThis->Hide();
			PlayerFailOperate(R(MSG_CANNOT_SWITCH_PKMODE));
			return;
		}
	}

	switch(iSelect)
	{
	case 0:
		GetWorld()->m_pPlayer->m_PkMode = PK_Monster;
		imgTmp = RtwRect(79,250,98,269);
		break;
	case 1:
		GetWorld()->m_pPlayer->m_PkMode = PK_Goodevil;
		imgTmp = RtwRect(79,270,98,289);
		break;
	case 2:
		GetWorld()->m_pPlayer->m_PkMode = PK_Free;
		imgTmp = RtwRect(79,290,98,309);
		m_dwSwitchFreeTime = rtGetMilliseconds() + 60000;
		break;
	}
	static RtwImage* pImage = g_workspace.getImageFactory()->createImage("ui\\textures\\5.tga");
	pImage->SetSourceRect(imgTmp);
	pImage->SetBlend(true);
	RTW_WIDGET("fmhp.btnPKmode")->SetHintText(m_plstPKmode->GetItemText(iSelect,1));
	RTW_WIDGET("fmhp.btnPKmode")->SetBackgroundImage(pImage);
	m_pfrmThis->Hide();

	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_change_pkmode);
	cmd->WriteByte((char)GetWorld()->m_pPlayer->m_PkMode);
	NetSend(cmd);

	unguard;
}

bool UIFormPkMode::CanInteract(GcActor* pActor)
{
	guard;
	if (NULL == pActor)
		return false;

	if (!pActor)
	{
		return true;
	}

	if (pActor->NpcType() == GcActor::ENT_USER && GetPlayer()->m_listTempPkInfo.size())
	{
		std::list<SPkTempInfo>::iterator it = GetPlayer()->m_listTempPkInfo.begin();
		for (;it!=GetPlayer()->m_listTempPkInfo.end();++it)
			if ((*it).dwTempPkID == pActor->ID())
				return true;
	}

	switch (GetWorld()->m_pPlayer->m_PkMode)
	{
	case PK_Monster: // 只能杀怪物
		{
			if (pActor->NpcType()==GcActor::ENT_MONSTER)
			{
				return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_NPC_COMBATIVE)
			{
				if (!pActor->m_pNpc)	return false;
				char npcFaction = pActor->m_pNpc->Faction;
				if (npcFaction==FACTION_UNKNOWN)
					return false;
				else if (npcFaction==GetWorld()->m_pPlayer->m_core.Faction)
					return false;
				else 
					return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_NPC_SERVICE)
			{
				return false;
			}
			else if (pActor->NpcType()==GcActor::ENT_USER || pActor->NpcType()==GcActor::ENT_PET || pActor->NpcType() == GcActor::ENT_CALLNPC)
			{
				if (pActor->m_bPShop)
					return true;
				else 
					return false;
			}
			else 
			{
				ERR("Invalid Actor Type");
				return false;
			}
		}
		break;

	case PK_Union: // 可以杀怪物与非本诸侯国的玩家
		{
			if (pActor->NpcType()==GcActor::ENT_MONSTER)
			{
				return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_NPC_COMBATIVE)
			{
				if (!pActor->m_pNpc)	return false;
				char npcFaction = pActor->m_pNpc->Faction;
				if (npcFaction==FACTION_UNKNOWN)
					return false;
				else if (npcFaction==GetWorld()->m_pPlayer->m_core.Faction)
					return false;
				else 
					return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_NPC_SERVICE)
			{
				return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_USER || pActor->NpcType()==GcActor::ENT_PET || pActor->NpcType() == GcActor::ENT_CALLNPC)
			{
				if (pActor->m_bPShop)
					return true;

				// 自己的召唤兽如何判断
				// @@@@@
				if (GetWorld()->m_pPet == pActor)
					return false;

				if (pActor->GetUnionID() != GetWorld()->m_pPlayer->GetUnionID())
					return true;
				else 
					return false;
			}
			else 
			{
				ERR("Invalid Actor Type");
				return false;
			}
		}
		break;

	case PK_Feod: // 可以杀怪物与非本同盟的玩家
		{
			if (pActor->NpcType()==GcActor::ENT_MONSTER)
			{
				return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_NPC_COMBATIVE)
			{
				if (!pActor->m_pNpc)	return false;
				char npcFaction = pActor->m_pNpc->Faction;

				if (npcFaction==FACTION_UNKNOWN)
					return false;
//-----------------chg start by tony 05.06.13----------------------------//
				else if ((npcFaction==GetWorld()->m_pPlayer->m_core.Faction) &&
						(!GetWorld()->IsCastleWarTime(pActor)))
//						 (pActor->m_pNpc->Home == ""))
				{
#ifdef _DEBUG
					LOG("@@@@@@@@@@@@@ Same Faction or Home is empty @@@@@@@@@@@@@@@@\n");				
#endif //_DEBUG
//-----------------chg end   by tony 05.06.13----------------------------//
					return false;
				}
				else 
					return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_NPC_SERVICE)
			{
				return false;
			}
			else if (pActor->NpcType()==GcActor::ENT_USER || pActor->NpcType()==GcActor::ENT_PET || pActor->NpcType() == GcActor::ENT_CALLNPC)
			{
				if (pActor->m_bPShop)
					return true;

				// 如何判断自己的召唤兽
				// @@@@@
				if (GetWorld()->m_pPet == pActor)
					return false;

				// 如何判断本同盟
				//-----------------add start by tony 05.06.13----------------------------//
				std::string feod_name	= GetWorld()->m_pPlayer->m_feod_name;
				BYTE		is_attacker	= GetWorld()->m_pPlayer->m_is_attacker;
				DWORD		union_id	= GetWorld()->m_pPlayer->GetUnionID();
				GcActor*	p_actor		= pActor;

				if(pActor->NpcType() == GcActor::ENT_PET || pActor->NpcType() == GcActor::ENT_CALLNPC)
				{
					p_actor = pActor->GetPetMaster();

					if(p_actor == NULL)
					{
						p_actor = pActor;
					}
					
				}else
				{
					p_actor = pActor;
				}

				if( GetWorld()->IsCastleWarTime(GetWorld()->m_pPlayer) ||
					GetWorld()->IsCastleWarTime(pActor))
				{
					//双方处于城战区，不属于攻守双方
					if( (union_id == 0)   || (p_actor->GetUnionID() == 0) ||
						(feod_name == "") || (p_actor->m_feod_name == ""))
					{
						if((union_id != 0) && (is_attacker == (BYTE)1) && (feod_name != ""))
						{
							return false;
						}

						if((union_id != 0) && (is_attacker == (BYTE)0) && (feod_name != ""))
						{
							return true;
						}

						return false;
					}

					if( (feod_name == p_actor->m_feod_name) &&
						(is_attacker == p_actor->m_is_attacker))
					{
						return false;
					}else
					{
#ifdef _DEBUG
						LOG2("my [feod_name] = [%s], his [feod_name] = [%s]\n", feod_name, p_actor->m_feod_name);
						LOG2("my [is_attacker] = [%c], his [is_attacker] = [%c]\n", is_attacker, p_actor->m_is_attacker);
#endif //_DEBUG
						return true;
					}
				}
				//-----------------add end   by tony 05.06.13----------------------------//

				return false;
			}
			else 
			{
				ERR("Invalid Actor Type");
				return false;
			}
		}
		break;

	case PK_Faction: // 可以杀怪物与非本国家的玩家
		{
			if (pActor->NpcType()==GcActor::ENT_MONSTER)
			{
				return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_NPC_COMBATIVE)
			{
				if (!pActor->m_pNpc)	return false;
				char npcFaction = pActor->m_pNpc->Faction;
				if (npcFaction==FACTION_UNKNOWN)
					return false;
				else if (npcFaction==GetWorld()->m_pPlayer->m_core.Faction)
					return false;
				else 
					return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_NPC_SERVICE)
			{
				return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_USER || pActor->NpcType()==GcActor::ENT_PET || pActor->NpcType() == GcActor::ENT_CALLNPC)
			{
				if (pActor->m_bPShop)
					return true;

				// 自己的召唤兽如何判断
				// @@@@@
				if (GetWorld()->m_pPet == pActor)
					return false;

				if (pActor->m_core.Faction != GetWorld()->m_pPlayer->m_core.Faction)
					return true;
				else 
                    return false;
			}
			else 
			{
				ERR("Invalid Actor Type");
				return false;
			}
		}
		break;

	case PK_Goodevil:
		{
			if (pActor->NpcType()==GcActor::ENT_MONSTER)
			{
				return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_NPC_COMBATIVE)
			{
				if (!pActor->m_pNpc)	return false;
				char npcFaction = pActor->m_pNpc->Faction;
				if (npcFaction==FACTION_UNKNOWN)
					return false;
				else if (npcFaction==GetWorld()->m_pPlayer->m_core.Faction)
					return false;
				else 
					return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_NPC_SERVICE)
			{
				return false;
			}
			else if (pActor->NpcType()==GcActor::ENT_USER || pActor->NpcType()==GcActor::ENT_PET ||pActor->NpcType() == GcActor::ENT_CALLNPC)
			{
				if (pActor->m_bPShop)
					return true;

				if (GetWorld()->m_pPet == pActor)
					return false;

				if (pActor->GetPKState())
				{
					return true;
				}
				return false;
			}
			else 
			{
				ERR("Invalid Actor Type");
				return false;
			}
		}
		break;
	case PK_Free: // 除系统NPC、本国守卫NPC、自己的宠物或召唤兽外的所有物体
		{
			if (pActor->NpcType()==GcActor::ENT_MONSTER)
			{
				return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_NPC_COMBATIVE)
			{
				if (!pActor->m_pNpc)	return false;
				char npcFaction = pActor->m_pNpc->Faction;
				if (npcFaction==FACTION_UNKNOWN)
					return false;
				else if (npcFaction==GetWorld()->m_pPlayer->m_core.Faction)
					return false;
				else 
					return true;
			}
			else if (pActor->NpcType()==GcActor::ENT_NPC_SERVICE)
			{
				return false;
			}
			else if (pActor->NpcType()==GcActor::ENT_USER || pActor->NpcType()==GcActor::ENT_PET ||pActor->NpcType() == GcActor::ENT_CALLNPC)
			{
				if (pActor->m_bPShop)
					return true;

				if (GetWorld()->m_pPet == pActor)
					return false;

				return true;
			}
			else 
			{
				ERR("Invalid Actor Type");
				return false;
			}
		}
		break;

	default:
		break;
	}

	return false;

	unguard;
}
