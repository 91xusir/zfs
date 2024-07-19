//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#include "region.h"
//#include "cmd_castlewar.h"

#define NUM(array)			(sizeof(array) / sizeof((array[0])))
#define CHECK_READ(p)		if(!(p)) return CMD_ERROR_READ_DATA;
#define CHECK_ASSERT(p)		if(!(p)) return CMD_ERROR_UNKNOWN;
#define CHECK_NULL(p)		if(!(p)) return CMD_ERROR_UNKNOWN;

//----------------------------
//客户端消息与对应处理函数表 
//----------------------------
const STCASTLEWAR CastleWarCmdProcTbl[] = 
{
	{c2r_castlewar_reg_attack,		  cmd_c2r_castlewar_reg_attack},		//攻城诸侯注册

	{c2r_castlewar_reg_attack_cancel, cmd_c2r_castlewar_reg_attack_cancel},	//攻城诸侯取消注册

	{c2r_castlewar_reg_defend,		  cmd_c2r_castlewar_reg_defend},		//协助守城诸侯注册

	{c2r_castlewar_reg_defend_cancel, cmd_c2r_castlewar_reg_defend_cancel},	//协助守城诸侯取消注册

	{c2r_castlewar_allow_bund,		  cmd_c2r_castlewar_allow_bund},		//守城诸侯批准与否

	{c2r_castlewar_select_time,		  cmd_c2r_castlewar_select_time},		//守城诸侯选择时间

	{c2r_castlewar_view_info,		  cmd_c2r_castlewar_view_info},			//玩家查看攻城信息

	{c2r_castlewar_insert_flag,		  cmd_c2r_castlewar_insert_flag},		//诸侯插令旗

	{c2r_castlewar_animal_killed,	  cmd_c2r_castlewar_animal_killed},		//守护兽被杀死

	{c2r_castlewar_apply_flag,		  cmd_c2r_castlewar_apply_flag},		//诸侯申请令旗

	{c2r_castlewar_flagnpc_show,	  cmd_c2r_castlewar_flagnpc_show}		//

};

//-----------------------------------
//GW消息与对应处理函数表
//一般情况下GW收到消息执行存档后响应
//-----------------------------------
const STCASTLEWAR_GW CastleWarCmdProcTbl_GW[] = 
{
	{g2r_castlewar_data_init,			  cmd_g2r_castlewar_data_init},

	{g2r_castlewar_data_update,			  cmd_g2r_castlewar_data_update},

	{g2r_castlewar_user_online,			  cmd_g2r_castlewar_user_online},

	{g2r_castlewar_war_prepare_rep,		  cmd_g2r_castlewar_war_prepare_rep},
	
	{g2r_castlewar_war_start_rep,		  cmd_g2r_castlewar_war_start_rep},

	{g2r_castlewar_war_pause_rep,		  cmd_g2r_castlewar_war_pause_rep},

	{g2r_castlewar_war_end_rep,			  cmd_g2r_castlewar_war_end_rep},

	{g2r_castlewar_reload_time_rep,		  cmd_g2r_castlewar_reload_time_rep}

};

//---------------------------------------
//客户端消息匹配函数
//---------------------------------------
int CastleWarCmdProcess_FromGC(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	//参数检查
	short cmd_type;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	CHECK_ASSERT(cmd->ReadShort(&cmd_type))

	//查找匹配函数
	int i = 0;
	for(; i < NUM(CastleWarCmdProcTbl); i++)
	{
		if(cmd_type == CastleWarCmdProcTbl[i].cmd_type)
		{
			break;
		}
	}

	if(i >= NUM(CastleWarCmdProcTbl))
	{
		return CMD_ERROR_UNKNOWN;
	}

	return CastleWarCmdProcTbl[i].cmd_func(CmdGiver, cmd);
}

//---------------------------------------
//GW消息匹配函数
//---------------------------------------
int CastleWarCmdProcess_FromGW(CG_CmdPacket *cmd)
{	
	//参数检查
	short cmd_type;

	CHECK_NULL(cmd)
	CHECK_ASSERT(cmd->ReadShort(&cmd_type))

	//查找匹配函数
	int i = 0;
	for(; i < NUM(CastleWarCmdProcTbl_GW); i++)
	{
		if(cmd_type == CastleWarCmdProcTbl_GW[i].cmd_type)
		{
			break;
		}
	}

	if(i >= NUM(CastleWarCmdProcTbl_GW))
	{
		return CMD_ERROR_UNKNOWN;
	}

	return CastleWarCmdProcTbl_GW[i].cmd_func(cmd);
}

//------------------------------------------------------------------------------------------
//以下处理来自客户端消息
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
#define CHECK_IS_USER(cre)		if(!cre->IsUser())	return CMD_ERROR_UNKNOWN;

bool check_is_vassal(CRegionCreature* cre)
{
	if(cre->m_unionID == 0)
	{
		return false;
	}
	EXT_SPACE::hash_map<ULONG, SUnionData>::iterator iter_union;
	iter_union = g_region->m_unionMap.find(cre->m_unionID);
	if(iter_union == g_region->m_unionMap.end())
	{
		return false;
	} 
	if(iter_union->second.m_nCaptainID != cre->m_userInfo->m_userId)
	{
		return false;
	}

	return true;
}

CRegionCreature* find_vassal_by_union_id(DWORD union_id)
{
	EXT_SPACE::hash_map<ULONG, SUnionData>::iterator iter_union;
	iter_union = g_region->m_unionMap.find(union_id);

	if(iter_union == g_region->m_unionMap.end())
	{
		return NULL;
	} 

	CRegionUser* p_user = g_region->FindUser(iter_union->second.m_nCaptainID);
	if ((p_user == NULL) || (p_user->m_dummy == NULL))
	{
		return NULL;
	}

	return p_user->m_dummy;
}
//------------------------------------------------------------------------------------------

//----------------------------------------
//-攻城诸侯申请令旗规则检查
//-1.检查玩家是否属于某诸侯国
//-2.检查玩家是否是诸侯
//-3.检查玩家阵营与城池是否符合
//-4.检查玩家是否属于攻守方
//-5.重复申请
//----------------------------------------
int	cmd_c2r_castlewar_apply_flag(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd)
{
	//参数检查
	long npc_id;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadLong(&npc_id))

	//规则检查
	CHECK_IS_USER(CmdGiver)

	if(check_is_vassal(CmdGiver) == false)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_apply_flag_rep(ERR_NOT_VASSAL_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	//阵营检查
	CRegionCreature *p_npc = g_region->m_defScene->FindCreature(npc_id);
	CHECK_NULL(p_npc)
	CHECK_NULL(p_npc->m_npcInfo)

	if (p_npc->m_npcInfo->Faction != CmdGiver->m_core.Faction)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_apply_flag_rep(ERR_DIFF_FACTION_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	//是否注册过
	if(g_RegionCastleWarMgr.FindByUnionID(CmdGiver->m_unionID) == NULL)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_apply_flag_rep(ERR_NO_REG_INFO);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	//如果注册过
	DWORD flag_id;
	if(g_RegionCastleWarMgr.FindFlagByUnionID(CmdGiver->m_unionID, flag_id))
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_apply_flag_rep(ERR_MULTI_FLAG);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

		
	//待修改
	//...
	int nItemCount, page, gridI, gridJ;
	bool bFindItem = CmdGiver->m_pItemContainer->m_Bag.FindType(ITEM_FLAG_ID, ItemColor_All, &nItemCount, &page, &gridI, &gridJ);
	if(bFindItem == true)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_apply_flag_rep(ERR_MULTI_FLAG);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	//检查通过,添加令旗道具
	CmdGiver->AddItem(ITEM_FLAG_ID, ItemColor_White);

	return CMD_ERROR_NONE;
}
//----------------------------------------
//-攻城诸侯注册规则检查
//-1.检查玩家是否属于某诸侯国
//-2.检查玩家是否是诸侯
//-3.检查玩家阵营与城池是否符合
//-4.检查所攻打城池是否正处于交战状态
//-5.重复注册
//----------------------------------------
int	cmd_c2r_castlewar_reg_attack(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd)
{
	//参数检查
	long npc_id;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadLong(&npc_id))

	//规则检查
	CHECK_IS_USER(CmdGiver)
	
	if(check_is_vassal(CmdGiver) == false)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_rep(ERR_NOT_VASSAL_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	CRegionCreature *p_npc = g_region->m_defScene->FindCreature(npc_id);
	CHECK_NULL(p_npc)
	CHECK_NULL(p_npc->m_npcInfo)
	
	if (p_npc->m_npcInfo->Faction != CmdGiver->m_core.Faction)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_rep(ERR_DIFF_FACTION_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}
	
	STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(p_npc->m_npcInfo->Home);
	CHECK_NULL(p_war_info)

	//城主不能注册
	if(p_war_info->master_union_id == CmdGiver->m_unionID)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_rep(ERR_MASTER_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	//人数已满
	if((int)p_war_info->v_attacker_union_info.size() > 255)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_rep(ERR_FULL_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	//诸侯国等级限制
	EXT_SPACE::hash_map<ULONG, SUnionData>::iterator iter_union;
	iter_union = g_region->m_unionMap.find(CmdGiver->m_unionID);

	if(iter_union == g_region->m_unionMap.end())
	{
		return CMD_ERROR_NONE;
	}
// 
// 	if((int)iter_union->second.level < 2)
// 	{
// 		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_rep(ERR_LEVEL_LOWER_CWAR);
// 
// 		CmdGiver->RecvCmd(g_sendCmd);
// 
// 		return CMD_ERROR_NONE;
// 	}

	/*if(p_war_info->state == WAR_PREPARE_STATE)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_rep(ERR_WAR_PREPARE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}*/

	if(p_war_info->state == WARFARE_STATE)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_rep(ERR_WARFARE_STATE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	for(int i = 0; i < (int)p_war_info->v_attacker_union_info.size(); i++)
	{
		if(p_war_info->v_attacker_union_info[i].union_id == CmdGiver->m_unionID)
		{
			CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_rep(ERR_REPEAT_REG_CWAR);

			CmdGiver->RecvCmd(g_sendCmd);

			return CMD_ERROR_NONE;
		}
	}

	for(int i = 0; i < (int)p_war_info->v_defender_union_info.size(); i++)
	{
		if(p_war_info->v_defender_union_info[i].union_id == CmdGiver->m_unionID)
		{
			CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_rep(ERR_REPEAT_REG_CWAR);

			CmdGiver->RecvCmd(g_sendCmd);

			return CMD_ERROR_NONE;
		}
	}

	//检查通过
	if(g_RegionCastleWarMgr.UpdateAttackerInfo( p_npc->m_npcInfo->Home,
												CmdGiver->m_unionID,
												ADD) == false)
	{
		return CMD_ERROR_UNKNOWN;
	}

#ifdef _DEBUG
	STWarInfo *p = g_RegionCastleWarMgr.FindByName(p_npc->m_npcInfo->Home);
	LOG1("NUMBERS OF ATTACKER IS [%d]\n", p->v_attacker_union_info.size());
#endif //_DEBUG
	
	////添加令旗道具	
	//int nItemCount, page, gridI, gridJ;
	//bool bFindItem = CmdGiver->m_pItemContainer->m_Bag.FindType(ITEM_FLAG_ID, ItemColor_All, &nItemCount, &page, &gridI, &gridJ);
	//if(bFindItem == false)
	//{
	//	CmdGiver->AddItem(ITEM_FLAG_ID, ItemColor_White);
	//}
	//
	////有人注册攻城，给城主添加令旗
	//if((p_war_info->master_union_id != 0) && ((int)p_war_info->v_attacker_union_info.size() == 1))
	//{		
	//	CRegionCreature* p_cre = find_vassal_by_union_id(p_war_info->master_union_id);
	//	
	//	if(p_cre != NULL)
	//	{
	//		p_cre->AddItem(ITEM_FLAG_ID, ItemColor_White);				
	//	}
	//}

	//成功响应，将更新后的攻守信息发送至客户端
	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_rep(ERR_NONE_CWAR);
	CmdGiver->RecvCmd(g_sendCmd);

	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_refresh(p_war_info);
	CmdGiver->RecvCmd(g_sendCmd);

	//更新备份至GW
	CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(p_war_info);

	return CMD_ERROR_NONE;
}

//----------------------------------------
//-攻城诸侯取消注册规则检查
//-1.检查玩家是否属于某诸侯国
//-2.检查玩家是否是诸侯
//-3.检查玩家是否已注册
//-4.检查所攻打城池是否正处于交战状态
//----------------------------------------
int	cmd_c2r_castlewar_reg_attack_cancel(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	//参数检查
	long npc_id;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadLong(&npc_id))

	//规则检查
	CHECK_IS_USER(CmdGiver)

	if(check_is_vassal(CmdGiver) == false)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_cancel_rep(ERR_NOT_VASSAL_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	CRegionCreature *p_npc = g_region->m_defScene->FindCreature(npc_id);
	CHECK_NULL(p_npc)
	CHECK_NULL(p_npc->m_npcInfo)

	
	STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(p_npc->m_npcInfo->Home);
	CHECK_NULL(p_war_info)

	if(p_war_info->state == WAR_PREPARE_STATE)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_cancel_rep(ERR_WAR_PREPARE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	if(p_war_info->state == WARFARE_STATE)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_cancel_rep(ERR_WARFARE_STATE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	int i = 0;
	for(i = 0; i < (int)p_war_info->v_attacker_union_info.size(); i++)
	{
		if(p_war_info->v_attacker_union_info[i].union_id == CmdGiver->m_unionID)
		{
			break;
		}
	}

	if(i >= (int)p_war_info->v_attacker_union_info.size())
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_cancel_rep(ERR_NO_REG_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	//检查通过
	if(g_RegionCastleWarMgr.UpdateAttackerInfo( p_npc->m_npcInfo->Home,
												CmdGiver->m_unionID,
												MINUS) == false)
	{
		return CMD_ERROR_UNKNOWN;
	}

	//// 删除道具
	int nItemCount, page, gridI, gridJ;
	bool bFindItem = CmdGiver->m_pItemContainer->m_Bag.FindType(ITEM_FLAG_ID, ItemColor_All, &nItemCount, &page, &gridI, &gridJ);
	if (bFindItem)
	{
		CmdGiver->m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
		CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ,ITEM_REASON_DELETE);
		CmdGiver->RecvCmd(g_sendCmd);
	}

	////无人攻城删除城主令旗道具
	if((p_war_info->master_union_id != 0) && p_war_info->v_attacker_union_info.empty())
	{
		CRegionCreature* p_cre = find_vassal_by_union_id(p_war_info->master_union_id);

		if(p_cre != NULL && p_cre->m_pItemContainer != NULL)
		{
			int nItemCount, page, gridI, gridJ;
			bool bFindItem = p_cre->m_pItemContainer->m_Bag.FindType(ITEM_FLAG_ID, ItemColor_All, &nItemCount, &page, &gridI, &gridJ);
			if (bFindItem)
			{
				CmdGiver->m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
				CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ,ITEM_REASON_DELETE);
				p_cre->RecvCmd(g_sendCmd);
			}
		}
	}

	//成功响应，将更新后的攻守信息发送至客户端
	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_cancel_rep(ERR_NONE_CWAR);
	CmdGiver->RecvCmd(g_sendCmd);

	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_refresh(p_war_info);
	CmdGiver->RecvCmd(g_sendCmd);

	//更新备份至GW
	CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(p_war_info);

	return CMD_ERROR_NONE;
}
//----------------------------------------
//-守城诸侯注册规则检查
//-1.检查玩家是否属于某诸侯国
//-2.检查玩家是否是诸侯
//-3.检查玩家阵营与城池是否符合
//-4.检查所攻打城池是否正处于交战状态
//-5.重复注册
//----------------------------------------
int	cmd_c2r_castlewar_reg_defend(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	//参数检查
	long npc_id;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadLong(&npc_id))

	//规则检查
	CHECK_IS_USER(CmdGiver)

	if(check_is_vassal(CmdGiver) == false)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_rep(ERR_NOT_VASSAL_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	CRegionCreature *p_npc = g_region->m_defScene->FindCreature(npc_id);
	CHECK_NULL(p_npc)
	CHECK_NULL(p_npc->m_npcInfo)

	if (p_npc->m_npcInfo->Faction != CmdGiver->m_core.Faction)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_rep(ERR_DIFF_FACTION_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(p_npc->m_npcInfo->Home);
	CHECK_NULL(p_war_info)

	//无人占领
	if(p_war_info->master_union_id == 0)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_rep(ERR_NPC_OCCUPY_STATE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	//城主无需注册
	if(p_war_info->master_union_id == CmdGiver->m_unionID)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_rep(ERR_MASTER_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	//诸侯国等级限制
	EXT_SPACE::hash_map<ULONG, SUnionData>::iterator iter_union;
	iter_union = g_region->m_unionMap.find(CmdGiver->m_unionID);

	if(iter_union == g_region->m_unionMap.end())
	{
		return CMD_ERROR_NONE;
	}

// 	if((int)iter_union->second.level < 2)
// 	{
// 		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_rep(ERR_LEVEL_LOWER_CWAR);
// 
// 		CmdGiver->RecvCmd(g_sendCmd);
// 
// 		return CMD_ERROR_NONE;
// 	}
		
	if(p_war_info->state == WAR_PREPARE_STATE)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_rep(ERR_WAR_PREPARE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	if(p_war_info->state == WARFARE_STATE)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_rep(ERR_WARFARE_STATE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	for(int i = 0; i < (int)p_war_info->v_attacker_union_info.size(); i++)
	{
		if(p_war_info->v_attacker_union_info[i].union_id == CmdGiver->m_unionID)
		{
			CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_rep(ERR_REPEAT_REG_CWAR);

			CmdGiver->RecvCmd(g_sendCmd);

			return CMD_ERROR_NONE;
		}
	}

	for(int i = 0; i < (int)p_war_info->v_defender_union_info.size(); i++)
	{
		if(p_war_info->v_defender_union_info[i].union_id == CmdGiver->m_unionID)
		{
			CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_rep(ERR_REPEAT_REG_CWAR);

			CmdGiver->RecvCmd(g_sendCmd);

			return CMD_ERROR_NONE;
		}
	}

	//检查通过
	if(g_RegionCastleWarMgr.UpdateDefenderInfo( p_npc->m_npcInfo->Home,
												CmdGiver->m_unionID,
												ADD) == false)
	{
		return CMD_ERROR_UNKNOWN;
	}

	//添加令旗道具
	//CmdGiver->AddItem(ITEM_FLAG_ID, ItemColor_White);

	//成功响应，将更新后的攻守信息发送至客户端
	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_rep(ERR_NONE_CWAR);
	CmdGiver->RecvCmd(g_sendCmd);

	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_refresh(p_war_info);
	CmdGiver->RecvCmd(g_sendCmd);

	//更新备份至GW
	CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(p_war_info);

	return CMD_ERROR_NONE;
}
//----------------------------------------
//-守城城诸侯取消注册规则检查
//-1.检查玩家是否属于某诸侯国
//-2.检查玩家是否是诸侯
//-3.检查玩家是否已注册
//-4.检查所攻打城池是否正处于交战状态
//----------------------------------------
int	cmd_c2r_castlewar_reg_defend_cancel(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	//参数检查
	long npc_id;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadLong(&npc_id))

	//规则检查
	CHECK_IS_USER(CmdGiver)

	if(check_is_vassal(CmdGiver) == false)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_cancel_rep(ERR_NOT_VASSAL_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	CRegionCreature *p_npc = g_region->m_defScene->FindCreature(npc_id);
	CHECK_NULL(p_npc)
	CHECK_NULL(p_npc->m_npcInfo)


	STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(p_npc->m_npcInfo->Home);
	CHECK_NULL(p_war_info)

	//城主不能取消注册
	if(p_war_info->master_union_id == CmdGiver->m_unionID)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_cancel_rep(ERR_MASTER_CANCEL_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	if(p_war_info->state == WAR_PREPARE_STATE)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_cancel_rep(ERR_WAR_PREPARE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	if(p_war_info->state == WARFARE_STATE)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_cancel_rep(ERR_WARFARE_STATE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	int i = 0;
	for(i = 0; i < (int)p_war_info->v_defender_union_info.size(); i++)
	{
		if(p_war_info->v_defender_union_info[i].union_id == CmdGiver->m_unionID)
		{
			break;
		}
	}

	if(i >= (int)p_war_info->v_defender_union_info.size())
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_cancel_rep(ERR_NO_REG_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	//检查通过
	if(g_RegionCastleWarMgr.UpdateDefenderInfo( p_npc->m_npcInfo->Home,
												CmdGiver->m_unionID,
												MINUS) == false)
	{
		return CMD_ERROR_UNKNOWN;
	}

	//// 删除道具
	int nItemCount, page, gridI, gridJ;
	bool bFindItem = CmdGiver->m_pItemContainer->m_Bag.FindType(ITEM_FLAG_ID, ItemColor_All, &nItemCount, &page, &gridI, &gridJ);
	if (bFindItem)
	{
		CmdGiver->m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
		CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ,ITEM_REASON_DELETE);
		CmdGiver->RecvCmd(g_sendCmd);
	}

	//成功响应，将更新后的攻守信息发送至客户端
	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_cancel_rep(ERR_NONE_CWAR);
	CmdGiver->RecvCmd(g_sendCmd);

	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_refresh(p_war_info);
	CmdGiver->RecvCmd(g_sendCmd);

	//更新备份至GW
	CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(p_war_info);

	return CMD_ERROR_NONE;
}
//----------------------------------------
//-守城诸侯批准/取消批准同盟规则检查
//-1.检查玩家是否属于某诸侯国
//-2.检查玩家是否是诸侯
//-3.检查玩家阵营与城池是否符合
//-4.检查所攻打城池是否正处于交战状态
//-5.重复批准
//-6.只能批准守城方
//----------------------------------------
int	cmd_c2r_castlewar_allow_bund(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	//参数检查
	long	npc_id;
	long	union_id;
	char	op;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadLong(&npc_id))
	CHECK_READ(cmd->ReadLong(&union_id))
	CHECK_READ(cmd->ReadByte(&op))

	//规则检查
	CHECK_IS_USER(CmdGiver)

	if((op != ALLOWED) &&
	   (op != NOTALLOWED))
	{
		return CMD_ERROR_UNKNOWN;
	}

	if(check_is_vassal(CmdGiver) == false)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_allow_bund_rep(ERR_NOT_VASSAL_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	CRegionCreature *p_npc = g_region->m_defScene->FindCreature(npc_id);
	CHECK_NULL(p_npc)
	CHECK_NULL(p_npc->m_npcInfo)

	if (p_npc->m_npcInfo->Faction != CmdGiver->m_core.Faction)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_allow_bund_rep(ERR_DIFF_FACTION_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(p_npc->m_npcInfo->Home);
	CHECK_NULL(p_war_info)

	//无人占领/非城主
	if((p_war_info->master_union_id == 0) ||
	   (p_war_info->master_union_id != CmdGiver->m_unionID))
	{
		return CMD_ERROR_UNKNOWN;
	}

	if(p_war_info->state == WAR_PREPARE_STATE)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_allow_bund_rep(ERR_WAR_PREPARE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	if(p_war_info->state == WARFARE_STATE)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_allow_bund_rep(ERR_WARFARE_STATE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}	
	
	//更新
	if(g_RegionCastleWarMgr.UpdateDefenderInfo( p_npc->m_npcInfo->Home,
												union_id,
												op) == false)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_allow_bund_rep(ERR_NUM_OUTRANGE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_UNKNOWN;
	}
		
	////给被批准的诸侯添加令旗道具
	//CRegionCreature* p_cre = find_vassal_by_union_id(union_id);

	//if(p_cre != NULL)
	//{
	//	int nItemCount, page, gridI, gridJ;
	//	bool bFindItem = CmdGiver->m_pItemContainer->m_Bag.FindType(ITEM_FLAG_ID, ItemColor_All, &nItemCount, &page, &gridI, &gridJ);
	//	if(bFindItem == false)
	//	{
	//		p_cre->AddItem(ITEM_FLAG_ID, ItemColor_White);
	//	}
	//}

	//成功响应，将更新后的攻守信息发送至客户端
	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_allow_bund_rep(ERR_NONE_CWAR);
	CmdGiver->RecvCmd(g_sendCmd);

	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_refresh(p_war_info);
	CmdGiver->RecvCmd(g_sendCmd);

	//更新备份至GW
	CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(p_war_info);

	return CMD_ERROR_NONE;
}
//----------------------------------------
//-城主选择攻城时间规则检查
//-1.检查玩家是否属于某诸侯国
//-2.检查玩家是否是诸侯
//-3.检查玩家阵营与城池是否符合
//-4.检查所攻打城池是否正处于交战状态
//-5.上次结束一小时内
//----------------------------------------
int	cmd_c2r_castlewar_select_time(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	//参数检查
	long	npc_id;
	short	week_day;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadLong(&npc_id))
	CHECK_READ(cmd->ReadShort(&week_day))

	//规则检查
	CHECK_IS_USER(CmdGiver)

	if((week_day > 6) || (week_day < 0))
	{
		return CMD_ERROR_UNKNOWN;
	}
	
	if(check_is_vassal(CmdGiver) == false)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_select_time_rep(ERR_NOT_VASSAL_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	CRegionCreature *p_npc = g_region->m_defScene->FindCreature(npc_id);
	CHECK_NULL(p_npc)
	CHECK_NULL(p_npc->m_npcInfo)

	if (p_npc->m_npcInfo->Faction != CmdGiver->m_core.Faction)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_select_time_rep(ERR_DIFF_FACTION_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(p_npc->m_npcInfo->Home);
	CHECK_NULL(p_war_info)

	//无人占领/非城主
	if((p_war_info->master_union_id == 0) ||
		(p_war_info->master_union_id != CmdGiver->m_unionID))
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_select_time_rep(ERR_DIFF_FACTION_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	if((p_war_info->state == WAR_PREPARE_STATE) || (p_war_info->state == WAR_FIVE_MIN_PAUSE_STATE))
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_select_time_rep(ERR_WAR_PREPARE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}

	if(p_war_info->state == WARFARE_STATE)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_select_time_rep(ERR_WARFARE_STATE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_NONE;
	}	

	//更新
	if(g_RegionCastleWarMgr.SetStartTime(p_npc->m_npcInfo->Home, week_day) == false)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_select_time_rep(ERR_TIME_OUTRANGE_CWAR);

		CmdGiver->RecvCmd(g_sendCmd);

		return CMD_ERROR_UNKNOWN;
	}

	//成功响应，将更新后的攻守信息发送至客户端
	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_select_time_rep(ERR_NONE_CWAR);
	CmdGiver->RecvCmd(g_sendCmd);

	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_refresh(p_war_info);
	CmdGiver->RecvCmd(g_sendCmd);
	
	return CMD_ERROR_NONE;
}

//----------------------------------------
//-玩家查看攻城信息规则检查
//-1.检查是否是玩家
//----------------------------------------
int	cmd_c2r_castlewar_view_info(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	//参数检查
	long npc_id;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadLong(&npc_id))

	//规则检查
	CHECK_IS_USER(CmdGiver)

	CRegionCreature *p_npc = g_region->m_defScene->FindCreature(npc_id);
	CHECK_NULL(p_npc)
	CHECK_NULL(p_npc->m_npcInfo)

	STWarInfo* p_war_info  = g_RegionCastleWarMgr.FindByName(p_npc->m_npcInfo->Home);
	CHECK_NULL(p_war_info)

	//检查通过
	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_view_info_rep(p_war_info);

	CmdGiver->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}
int	cmd_c2r_castlewar_flagnpc_show(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd)
{
	//参数检查
	long  o_id;
	char* p_union_name;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadString(&p_union_name))
	CHECK_READ(cmd->ReadLong(&o_id))

	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_flagnpc_created(std::string(p_union_name), o_id);

	CmdGiver->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}
int	cmd_c2r_castlewar_insert_flag(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	return CMD_ERROR_NONE;
}
int	cmd_c2r_castlewar_animal_killed(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	return CMD_ERROR_NONE;
}

//------------------------------------------------------------------------
//以下处理来自GW消息或产生通知型消息
//------------------------------------------------------------------------
int cmd_g2r_castlewar_data_init(CG_CmdPacket *cmd)
{
	STWarInfo temp;

	CHECK_NULL(cmd)

	if(temp.Serialize(cmd) == false)
	{
		return CMD_ERROR_UNKNOWN;
	}

#ifdef _DEBUG
	LOG ("--------------------------Received Init Data From GW---------------------------\n");
	LOG1("-Feod Name: [%s]\n", temp.feod_name.c_str());
	LOG1("-Master Union ID: [%d]\n", temp.master_union_id);
	LOG1("-Master Union Name: [%s]\n", temp.master_union_name.c_str());
	LOG1("-Vassal Name: [%s]\n", temp.vassal_name.c_str());
	LOG1("-Flag ID: [%d]\n", temp.flag_id);
	LOG1("-War Start Time: [%d]\n", temp.start_time);
	LOG1("-GuardAnimal dead: [%s]\n", (temp.guard_animal_is_dead == true) ? "true" : "false");
	LOG1("-Attacker Number: [%d]\n", (int)temp.v_attacker_union_info.size());
	LOG1("-Defender Number: [%d]\n", (int)temp.v_defender_union_info.size());
	LOG ("-------------------------------------------------------------------------------\n");
#endif //_DEBUG

	g_RegionCastleWarMgr.OnRecvInitData(&temp);

	return CMD_ERROR_NONE;
}

int cmd_g2r_castlewar_data_update(CG_CmdPacket *cmd)
{
	STWarInfo temp;

	CHECK_NULL(cmd)

		if(temp.Serialize(cmd) == false)
		{
			return CMD_ERROR_UNKNOWN;
		}

		LOG ("--------------------------Received Update Data From GW---------------------------\n");

#ifdef _DEBUG
		LOG ("--------------------------Received Update Data From GW---------------------------\n");
		LOG1("-Feod Name: [%s]\n", temp.feod_name.c_str());
		LOG1("-Master Union ID: [%d]\n", temp.master_union_id);
		LOG1("-Master Union Name: [%s]\n", temp.master_union_name.c_str());
		LOG1("-Vassal Name: [%s]\n", temp.vassal_name.c_str());
		LOG1("-Flag ID: [%d]\n", temp.flag_id);
		LOG1("-War Start Time: [%d]\n", temp.start_time);
		LOG1("-GuardAnimal dead: [%s]\n", (temp.guard_animal_is_dead == true) ? "true" : "false");
		LOG1("-Attacker Number: [%d]\n", (int)temp.v_attacker_union_info.size());
		LOG1("-Defender Number: [%d]\n", (int)temp.v_defender_union_info.size());
		LOG ("-------------------------------------------------------------------------------\n");
#endif //_DEBUG

		g_RegionCastleWarMgr.OnRecvUpdateData(&temp);

		return CMD_ERROR_NONE;
}

int cmd_g2r_castlewar_reload_time_rep(CG_CmdPacket *cmd)
{
	char* p_feod_name = NULL;
	long  start_time;
	long  def_duration;

	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadString(&p_feod_name))
	CHECK_READ(cmd->ReadLong(&start_time))
	CHECK_READ(cmd->ReadLong(&def_duration))

	//消息建构并发送
	std::string feod_name = p_feod_name;

	STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(feod_name);

	if(p_war_info == NULL)
	{
		return CMD_ERROR_UNKNOWN;
	}

	p_war_info->start_time = start_time;

	p_war_info->end_time = start_time + def_duration * 60 * 60;

	//CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_refresh(p_war_info);

	return CMD_ERROR_NONE;
}

int cmd_g2r_castlewar_user_online(CG_CmdPacket *cmd)
{
	long user_db_id;

	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadLong(&user_db_id))

	g_RegionCastleWarMgr.OnUserOnline(user_db_id, true);

	return CMD_ERROR_NONE;
}

int cmd_g2r_castlewar_war_prepare_rep(CG_CmdPacket *cmd)
{
	char* p_feod_name = NULL;
	char* p_union_name = NULL;
	char  faction;
	long total_time;

	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadString(&p_feod_name))
	CHECK_READ(cmd->ReadString(&p_union_name))
	CHECK_READ(cmd->ReadLong(&total_time))

	//消息建构并发送
	std::string feod_name = p_feod_name;
	std::string union_name = p_union_name;

	/*STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(feod_name);

	if(p_war_info == NULL)
	{
		return CMD_ERROR_UNKNOWN;
	}

	DWORD total_time = p_war_info->start_time - g_RegionCastleWarMgr.GetCurrTime();*/

	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_war_prepare(feod_name, union_name, total_time);

	if(feod_name == "XiQi")
	{
		faction = FACTION_ZHOU;
	}else if(feod_name == "ZhaoGe")
	{
		faction = FACTION_SHANG;
	}

	g_region->BroadcastCmd(g_sendCmd, NULL, faction);

	return CMD_ERROR_NONE;
}

int cmd_g2r_castlewar_war_start_rep(CG_CmdPacket *cmd)
{
	char* p_feod_name = NULL;
	char* p_union_name = NULL;
	char  faction;

	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadString(&p_feod_name))

	//消息建构并发送
	std::string feod_name = p_feod_name;

	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_war_start(feod_name);

	if(feod_name == "XiQi")
	{
		faction = FACTION_ZHOU;
	}else if(feod_name == "ZhaoGe")
	{
		faction = FACTION_SHANG;
	}

	g_region->BroadcastCmd(g_sendCmd, NULL, faction);

	return CMD_ERROR_NONE;
}

int cmd_g2r_castlewar_war_pause_rep(CG_CmdPacket *cmd)
{
	char* p_feod_name = NULL;
	char* p_union_name = NULL;
	char  faction;

	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadString(&p_feod_name))
	CHECK_READ(cmd->ReadString(&p_union_name))

	//消息建构并发送
	std::string feod_name = p_feod_name;
	std::string union_name = p_union_name;

	STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(feod_name);

	if(p_war_info == NULL)
	{
		return CMD_ERROR_UNKNOWN;
	}

	DWORD total_time = p_war_info->start_time - g_RegionCastleWarMgr.GetCurrTime();

	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_war_pause(feod_name, union_name, total_time);

	if(feod_name == "XiQi")
	{
		faction = FACTION_ZHOU;
	}else if(feod_name == "ZhaoGe")
	{
		faction = FACTION_SHANG;
	}

	g_region->BroadcastCmd(g_sendCmd, NULL, faction);

	return CMD_ERROR_NONE;
}

int cmd_g2r_castlewar_war_end_rep(CG_CmdPacket *cmd)
{
	char* p_feod_name = NULL;
	char* p_union_name = NULL;
	char  faction;

	CHECK_NULL(cmd)
	CHECK_READ(cmd->ReadString(&p_feod_name))
	CHECK_READ(cmd->ReadString(&p_union_name))

	//消息建构并发送
	std::string feod_name = p_feod_name;
	std::string union_name = p_union_name;

	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_war_end(feod_name, union_name);

	if(feod_name == "XiQi")
	{
		faction = FACTION_ZHOU;
	}else if(feod_name == "ZhaoGe")
	{
		faction = FACTION_SHANG;
	}

	g_region->BroadcastCmd(g_sendCmd, NULL, faction);

	return CMD_ERROR_NONE;
}

//------------------------------------------
//以下是R2C消息建构
//------------------------------------------
void CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin()
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_castlewar);
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_rep(BYTE err_no)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_reg_attack_rep);
	g_sendCmd->WriteByte(err_no);
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_attack_cancel_rep(BYTE err_no)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_reg_attack_cancel_rep);
	g_sendCmd->WriteByte(err_no);
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_rep(BYTE err_no)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_reg_defend_rep);
	g_sendCmd->WriteByte(err_no);
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_reg_defend_cancel_rep(BYTE err_no)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_reg_defend_cancel_rep);
	g_sendCmd->WriteByte(err_no);
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_allow_bund_rep(BYTE err_no)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_allow_bund_rep);
	g_sendCmd->WriteByte(err_no);
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_select_time_rep(BYTE err_no)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_select_time_rep);
	g_sendCmd->WriteByte(err_no);
}


void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_view_info_rep(STWarInfo* p_war_info)
{
	if(p_war_info == NULL) return;
	
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_view_info_rep);
	p_war_info->Serialize(g_sendCmd);
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_war_prepare(string feod_name, string def_union_name, DWORD total_time)
{		
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_war_prepare);
	g_sendCmd->WriteString((char*)feod_name.c_str());
	g_sendCmd->WriteString((char*)def_union_name.c_str());
	g_sendCmd->WriteLong(total_time);
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_war_start(string feod_name)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_war_start);
	g_sendCmd->WriteString((char *)feod_name.c_str());
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_war_pause(string feod_name, string win_union_name, DWORD total_time)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_war_pause);
	g_sendCmd->WriteString((char *)feod_name.c_str());
	g_sendCmd->WriteString((char *)win_union_name.c_str());
	g_sendCmd->WriteLong(total_time);
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_war_end(string feod_name, string win_union_name)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_war_end);
	g_sendCmd->WriteString((char *)feod_name.c_str());
	g_sendCmd->WriteString((char *)win_union_name.c_str());
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_insert_flag_rep(BYTE err_no)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_insert_flag_rep);
	g_sendCmd->WriteByte(err_no);
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_apply_flag_rep(BYTE err_no)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_apply_flag_rep);
	g_sendCmd->WriteByte(err_no);
}


void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_is_transported()
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_is_transported);
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_refresh(STWarInfo* p_war_info)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_refresh);
	p_war_info->Serialize(g_sendCmd);
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_user_data_chg(string feod_name, BYTE is_attacker)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_user_data_chg);
	g_sendCmd->WriteString((char*)feod_name.c_str());
	g_sendCmd->WriteByte(is_attacker);
}

void CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_flagnpc_created(string union_name, DWORD o_id)
{
	CCastleWarCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_castlewar_flagnpc_created);
	g_sendCmd->WriteString((char *)union_name.c_str());
	g_sendCmd->WriteLong(o_id);
}

//------------------------------------------
//以下是R2G消息建构发送
//------------------------------------------
void CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(STWarInfo* p_war_info)
{
	if(p_war_info == NULL) return;

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_castlewar);
	packet.WriteShort(r2g_castlewar_update_data);
	p_war_info->Serialize(&packet);
	g_region->m_gws.EndSend();
}

void CCastleWarCmdSend_R2G::Send_r2g_castlewar_time_set(string feod_name, DWORD time_set)
{	
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_castlewar);
	packet.WriteShort(r2g_castlewar_time_set);
	packet.WriteString((char*)feod_name.c_str());
	packet.WriteLong(time_set);
	g_region->m_gws.EndSend();
}

void CCastleWarCmdSend_R2G::Send_r2g_castlewar_reload_time(string feod_name)
{	
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_castlewar);
	packet.WriteShort(r2g_castlewar_reload_time);
	packet.WriteString((char*)feod_name.c_str());
	g_region->m_gws.EndSend();
}

void CCastleWarCmdSend_R2G::Send_r2g_castlewar_war_prepare(string feod_name, string def_union_name)
{
	STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(feod_name);

	if(p_war_info == NULL)
	{
		return;
	}

	DWORD total_time = p_war_info->start_time - g_RegionCastleWarMgr.GetCurrTime();
	
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_castlewar);
	packet.WriteShort(r2g_castlewar_war_prepare);
	packet.WriteString((char*)feod_name.c_str());
	packet.WriteString((char*)def_union_name.c_str());
	packet.WriteLong(total_time);
	g_region->m_gws.EndSend();
}

void CCastleWarCmdSend_R2G::Send_r2g_castlewar_war_start(string feod_name)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_castlewar);
	packet.WriteShort(r2g_castlewar_war_start);
	packet.WriteString((char*)feod_name.c_str());
	g_region->m_gws.EndSend();
}

void CCastleWarCmdSend_R2G::Send_r2g_castlewar_war_pause(string feod_name, string win_union_name)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_castlewar);
	packet.WriteShort(r2g_castlewar_war_pause);
	packet.WriteString((char*)feod_name.c_str());
	packet.WriteString((char*)win_union_name.c_str());
	g_region->m_gws.EndSend();
}

void CCastleWarCmdSend_R2G::Send_r2g_castlewar_war_end(string feod_name, string win_union_name)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_castlewar);
	packet.WriteShort(r2g_castlewar_war_end);
	packet.WriteString((char*)feod_name.c_str());
	packet.WriteString((char*)win_union_name.c_str());
	g_region->m_gws.EndSend();
}

