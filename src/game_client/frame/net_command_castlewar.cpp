//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#include "gc_include.h"
#include "region_client.h"
#include "game_assistant.h"
#include "net_command_castlewar.h"
#include "ui_form_castlewar.h"
#include "ui_form_castlewar_settime.h"
#include "ui_form_timer.h"

#define NUM(array)			(sizeof(array) / sizeof((array[0])))
#define CHECK_NULL(p)		if(!(p)) return false;

//----------------------------
//REGION消息与对应处理函数表
//----------------------------
const STCASTLEWAR_GC CastleWarCmdProcTbl_GC[] = 
{
	{r2c_castlewar_war_prepare,			  net_r2c_castlewar_war_prepare},		//通知国民战争准备
	
	{r2c_castlewar_war_start,			  net_r2c_castlewar_war_start},			//通知国民处于城战状态

	{r2c_castlewar_war_pause,			  net_r2c_castlewar_war_pause},			//通知国民处于城战状态

	{r2c_castlewar_war_end,				  net_r2c_castlewar_war_end},			//通知国民城战结束

	{r2c_castlewar_have_apply,			  net_r2c_castlewar_have_apply},		//收到协防请求（仅对诸侯）

	{r2c_castlewar_have_attacker,		  net_r2c_castlewar_have_attacker},		//有登记攻城的诸侯

	{r2c_castlewar_reg_attack_rep,		  net_r2c_castlewar_reg_attack_rep},		//攻城诸侯注册响应

	{r2c_castlewar_reg_attack_cancel_rep, net_r2c_castlewar_reg_attack_cancel_rep},	//攻城诸侯取消注册

	{r2c_castlewar_reg_defend_rep,		  net_r2c_castlewar_reg_defend_rep},		//协助守城诸侯注册响应

	{r2c_castlewar_reg_defend_cancel_rep, net_r2c_castlewar_reg_defend_cancel_rep},	//协助守城诸侯取消注册

	{r2c_castlewar_allow_bund_rep,		  net_r2c_castlewar_allow_bund_rep},	//守城诸侯批准与否响应

	{r2c_castlewar_select_time_rep,		  net_r2c_castlewar_select_time_rep},	//守城诸侯选择时间响应

	{r2c_castlewar_view_info_rep,		  net_r2c_castlewar_view_info_rep},		//玩家查看攻城信息响应

	{r2c_castlewar_insert_flag_rep,		  net_r2c_castlewar_insert_flag_rep},	//诸侯插令旗响应

	{r2c_castlewar_refresh,				  net_r2c_castlewar_refresh},

	{r2c_castlewar_user_data_chg,		  net_r2c_castlewar_user_data_chg},

	{r2c_castlewar_is_transported,		  net_r2c_castlewar_is_transported},

	{r2c_castlewar_apply_flag_rep,		  net_r2c_castlewar_apply_flag_rep},

	{r2c_castlewar_flagnpc_created,		  net_r2c_castlewar_flagnpc_created}

};

//---------------------------------------
//REGION消息匹配函数
//---------------------------------------
bool CastleWarCmdProcess_Client(CG_CmdPacket *cmd)
{
	//参数检查
	short cmd_type;

	CHECK_NULL(cmd)
	CHECK_NULL(cmd->ReadShort(&cmd_type))

	//查找匹配函数
	int i = 0;
	for(; i < NUM(CastleWarCmdProcTbl_GC); i++)
	{
		if(cmd_type == CastleWarCmdProcTbl_GC[i].cmd_type)
		{
			break;
		}
	}

	if(i >= NUM(CastleWarCmdProcTbl_GC))
	{
		return false;
	}

	return CastleWarCmdProcTbl_GC[i].cmd_func(cmd);
}

bool net_r2c_castlewar_war_prepare(CG_CmdPacket *pPacket)
{
	guard;

	char* p_feod_name	= NULL;
	char* p_union_name	= NULL;
	long  total_time = 0;

	READb_string(p_feod_name);
	READb_string(p_union_name);
	READb_long(total_time);

	std::string feod_name = p_feod_name;
	std::string union_name = p_union_name;

	/*int i = 0;
	for(i = 0; i < FEOD_COUNT; i++)
	{
		if(strncmp(p_feod_name, FeodNameInDB[i].c_str(), strlen(p_feod_name)) == 0)
		{			
			std::string msg = CastleWarAreaName[i] + SYSMSG_WAR_PREPARE;

			ShowImportantMessage(msg.c_str());

			break;
		}
	}

	if(i >= FEOD_COUNT)
	{
		return false;
	}*/

	g_layerMain->m_formTimer->Show(feod_name, total_time);

	return true;

	unguard;
}
bool net_r2c_castlewar_war_start(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	char* p_name = NULL;

	READb_string(p_name);

	std::string feod_name = p_name;	
	
	int i = 0;
	for(i = 0; i < FEOD_COUNT; i++)
	{
		if(strncmp(p_name, FeodNameInDB[i].c_str(), strlen(p_name)) == 0)
		{
			if(GetWorld()->m_bCastleWarTime[i] == true)
			{
				return false;
			}

			//PK规则改变
			GetWorld()->m_bCastleWarTime[i] = true;

			std::string msg = CastleWarAreaName[i] + SYSMSG_WAR_START;

			ShowImportantMessage(msg.c_str());

			break;
		}
	}

	if(i >= FEOD_COUNT)
	{
		return false;
	}

	g_layerMain->m_formTimer->Hide();

	return true;

	unguard;
}
bool net_r2c_castlewar_war_pause(CG_CmdPacket *pPacket)
{
	guard;

	char* p_name;
	char* p_union_name;
	long  total_time;

	READb_string(p_name);
	READb_string(p_union_name);
	READb_long(total_time);
	
	std::string feod_name(p_name);
	std::string union_name(p_union_name);

	if(union_name == "")
	{
		//union_name = "守城方";//by fox for string
		union_name = R(MSG_TENABLE);
	}	
	
	int i = 0;
	for(i = 0; i < FEOD_COUNT; i++)
	{
		if(strncmp(p_name, FeodNameInDB[i].c_str(), strlen(p_name)) == 0)
		{
			if(GetWorld()->m_bCastleWarTime[i] == false)
			{
				return false;
			}

			//PK规则改变
			GetWorld()->m_bCastleWarTime[i] = false;

			std::string msg = CastleWarAreaName[i] + SYSMSG_WAR_PAUSE + union_name + SYSMSG_WAR_PAUSE_WIN;

			ShowImportantMessage(msg.c_str());
			break;
		}
	}

	if(i >= FEOD_COUNT)
	{
		return false;
	}

	//g_layerMain->m_formTimer->Show(feod_name, total_time);

	return true;

	unguard;
}
bool net_r2c_castlewar_war_end(CG_CmdPacket *pPacket)
{
	guard;

	char* p_name;
	char* p_union_name;

	READb_string(p_name);
	READb_string(p_union_name);
	
	std::string union_name(p_union_name);

	if((union_name == "npc") || (union_name == ""))
	{
		//union_name = "守城方";//by fox for string
		union_name = R(MSG_TENABLE);
	}

	g_layerMain->m_formCastleWarTimeSet->m_pTimeSetBox->Enable();
	
	int i = 0;
	for(i = 0; i < FEOD_COUNT; i++)
	{
		if(strncmp(p_name, FeodNameInDB[i].c_str(), strlen(p_name)) == 0)
		{
			if(GetWorld()->m_bCastleWarTime[i] == false)
			{
				return false;
			}

			//PK规则改变
			GetWorld()->m_bCastleWarTime[i] = false;

			std::string msg = CastleWarAreaName[i] + SYSMSG_WAR_END + union_name + SYSMSG_WAR_WINNER;

			ShowImportantMessage(msg.c_str());
			break;
		}
	}

	if(i >= FEOD_COUNT)
	{
		return false;
	}

	g_layerMain->m_formTimer->Hide();

	return true;

	unguard;
}
bool net_r2c_castlewar_have_apply(CG_CmdPacket *pPacket)
{
	return true;
}
bool net_r2c_castlewar_have_attacker(CG_CmdPacket *pPacket)
{
	return true;
}
bool net_r2c_castlewar_reg_attack_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	BYTE	err_no;
	READb_byte(err_no);

	switch(err_no)
	{
	//注册成功
	case ERR_NONE_CWAR:
		ShowSystemMessage(STR_ERR_NONE_CWAR);
		g_layerMain->m_formCastleWarReg->Refresh();
		break;
	case ERR_NOT_VASSAL_CWAR:
		ShowSystemMessage(STR_ERR_NOT_VASSAL_CWAR);
		break;
	case ERR_DIFF_FACTION_CWAR:
		ShowSystemMessage(STR_ERR_DIFF_FACTION_CWAR);
		break;
	case ERR_WAR_PREPARE_CWAR:
		ShowSystemMessage(STR_ERR_WAR_PREPARE_STATE_CWAR);
		break;
	case ERR_WARFARE_STATE_CWAR:
		ShowSystemMessage(STR_ERR_WARFARE_STATE_CWAR);
		break;
	case ERR_REPEAT_REG_CWAR:
		ShowSystemMessage(STR_ERR_REPEAT_REG_CWAR);
		break;
	case ERR_MASTER_CWAR:
		ShowSystemMessage(STR_ERR_MASTER_CWAR);
		break;
	case ERR_FULL_CWAR:
		ShowSystemMessage(STR_ERR_FULL_CWAR);
		break;
	case ERR_LEVEL_LOWER_CWAR:
		ShowSystemMessage(STR_ERR_LEVEL_LOWER_CWAR);
		break;
	default:
		g_layerMain->m_formCastleWarReg->UnLock();
		return false;
	}

	g_layerMain->m_formCastleWarReg->UnLock();
	
	return true;

	unguard;
}
bool net_r2c_castlewar_reg_attack_cancel_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	BYTE	err_no;
	READb_byte(err_no);

	switch(err_no)
	{
	//取消注册成功
	case ERR_NONE_CWAR:
		ShowSystemMessage(STR_ERR_NONE_CANCEL_CWAR);
		g_layerMain->m_formCastleWarReg->Refresh();
		break;
	case ERR_NOT_VASSAL_CWAR:
		ShowSystemMessage(STR_ERR_NOT_VASSAL_CANCEL_CWAR);
		break;
	case ERR_NO_REG_CWAR:
		ShowSystemMessage(STR_ERR_ERR_NO_REG_CANCEL_CWAR);
		break;
	case ERR_WAR_PREPARE_CWAR:
		ShowSystemMessage(STR_ERR_WAR_PREPARE_STATE_CANCEL_CWAR);
		break;
	case ERR_WARFARE_STATE_CWAR:
		ShowSystemMessage(STR_ERR_WARFARE_STATE_CANCEL_CWAR);
		break;
	default:
		g_layerMain->m_formCastleWarReg->UnLock();
		return false;
	}

	g_layerMain->m_formCastleWarReg->UnLock();

	return true;

	unguard;
}
bool net_r2c_castlewar_reg_defend_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	BYTE	err_no;
	READb_byte(err_no);

	switch(err_no)
	{
	//注册成功
	case ERR_NONE_CWAR:
		ShowSystemMessage(STR_ERR_NONE_CWAR);
		g_layerMain->m_formCastleWarReg->Refresh();
		break;
	case ERR_NOT_VASSAL_CWAR:
		ShowSystemMessage(STR_ERR_NOT_VASSAL_CWAR);
		break;
	case ERR_DIFF_FACTION_CWAR:
		ShowSystemMessage(STR_ERR_DIFF_FACTION_CWAR);
		break;
	case ERR_WAR_PREPARE_CWAR:
		ShowSystemMessage(STR_ERR_WAR_PREPARE_STATE_CWAR);
		break;
	case ERR_WARFARE_STATE_CWAR:
		ShowSystemMessage(STR_ERR_WARFARE_STATE_CWAR);
		break;
	case ERR_REPEAT_REG_CWAR:
		ShowSystemMessage(STR_ERR_REPEAT_REG_CWAR);
		break;
	case ERR_NPC_OCCUPY_STATE_CWAR:
		ShowSystemMessage(STR_ERR_NPC_OCCUPY_STATE_CWAR);
		break;
	case ERR_MASTER_CWAR:
		ShowSystemMessage(STR_ERR_MASTER_CWAR);
		break;
	case ERR_LEVEL_LOWER_CWAR:
		ShowSystemMessage(STR_ERR_LEVEL_LOWER_CWAR);
		break;
	default:
		g_layerMain->m_formCastleWarReg->UnLock();
		return false;
	}

	g_layerMain->m_formCastleWarReg->UnLock();

	return true;

	unguard;
}
bool net_r2c_castlewar_reg_defend_cancel_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	BYTE	err_no;
	READb_byte(err_no);

	switch(err_no)
	{
	//取消注册成功
	case ERR_NONE_CWAR:
		ShowSystemMessage(STR_ERR_NONE_CANCEL_CWAR);
		g_layerMain->m_formCastleWarReg->Refresh();
		break;
	case ERR_NOT_VASSAL_CWAR:
		ShowSystemMessage(STR_ERR_NOT_VASSAL_CANCEL_CWAR);
		break;
	case ERR_NO_REG_CWAR:
		ShowSystemMessage(STR_ERR_ERR_NO_REG_CANCEL_CWAR);
		break;
	case ERR_WAR_PREPARE_CWAR:
		ShowSystemMessage(STR_ERR_WAR_PREPARE_STATE_CANCEL_CWAR);
		break;
	case ERR_WARFARE_STATE_CWAR:
		ShowSystemMessage(STR_ERR_WARFARE_STATE_CANCEL_CWAR);
		break;
	case ERR_MASTER_CANCEL_CWAR:
		ShowSystemMessage(STR_ERR_MASTER_CANCEL_CWAR);
		break;
	default:
		g_layerMain->m_formCastleWarReg->UnLock();
		return false;
	}

	g_layerMain->m_formCastleWarReg->UnLock();

	return true;

	unguard;
}
bool net_r2c_castlewar_allow_bund_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	BYTE	err_no;
	READb_byte(err_no);

	switch(err_no)
	{
	case ERR_NONE_CWAR:
		ShowSystemMessage(STR_ERR_NONE_ALLOW_CWAR);
		g_layerMain->m_formCastleWarReg->Refresh();
		break;
	case ERR_NOT_VASSAL_CWAR:
		ShowSystemMessage(STR_ERR_NOT_VASSAL_CWAR);
		break;
	case ERR_DIFF_FACTION_CWAR:
		ShowSystemMessage(STR_ERR_DIFF_FACTION_CWAR);
		break;
	case ERR_WAR_PREPARE_CWAR:
		ShowSystemMessage(STR_ERR_WAR_PREPARE_STATE_CWAR);
		break;
	case ERR_WARFARE_STATE_CWAR:
		ShowSystemMessage(STR_ERR_WARFARE_STATE_CWAR);
		break;
	case ERR_NUM_OUTRANGE_CWAR:
		ShowSystemMessage(STR_ERR_NUM_OUTRANGE_CWAR);
		break;
	default:
		g_layerMain->m_formCastleWarReg->UnLock();
		return false;
	}

	g_layerMain->m_formCastleWarReg->UnLock();

	return true;

	unguard;
}
bool net_r2c_castlewar_select_time_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	BYTE	err_no;
	READb_byte(err_no);

	switch(err_no)
	{
	case ERR_NONE_CWAR:
		ShowSystemMessage(STR_SET_TIME_SUCCESS_CWAR);
		g_layerMain->m_formCastleWarTimeSet->m_pTimeSetBox->Disable();
		g_layerMain->m_formCastleWarTimeSet->Refresh();
		break;
	case ERR_NOT_VASSAL_CWAR:
		ShowSystemMessage(STR_ERR_NOT_VASSAL_CWAR);
		break;
	case ERR_DIFF_FACTION_CWAR:
		ShowSystemMessage(STR_ERR_DIFF_FACTION_CWAR);
		break;
	case ERR_WAR_PREPARE_CWAR:
		ShowSystemMessage(STR_ERR_WAR_PREPARE_STATE_CWAR);
		break;
	case ERR_WARFARE_STATE_CWAR:
		ShowSystemMessage(STR_ERR_WARFARE_STATE_CWAR);
		break;
	case ERR_TIME_OUTRANGE_CWAR:
		ShowSystemMessage(STR_ERR_TIME_OUTRANGE_CWAR);
		break;
	default:
		g_layerMain->m_formCastleWarTimeSet->UnLock();
		return false;
	}

	g_layerMain->m_formCastleWarTimeSet->UnLock();

	return true;

	unguard;
}
bool net_r2c_castlewar_view_info_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	if(!(g_layerMain->m_formCastleWarReg->m_war_info.Serialize(pPacket)))
	{
#ifdef _DEBUG
		LOG("command parse error : net_r2c_castlewar_view_info_rep\n");
#endif //_DEBUG
		return false;
	}

	g_layerMain->m_formCastleWarReg->Refresh();

	g_layerMain->m_formCastleWarTimeSet->m_war_info = g_layerMain->m_formCastleWarReg->m_war_info;

	g_layerMain->m_formCastleWarTimeSet->Refresh();

	return true;

	unguard;
}

bool net_r2c_castlewar_refresh(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	if(!(g_layerMain->m_formCastleWarReg->m_war_info.Serialize(pPacket)))
	{
#ifdef _DEBUG
		LOG("command parse error : net_r2c_castlewar_view_info_rep\n");
#endif //_DEBUG
		return false;
	}

	g_layerMain->m_formCastleWarReg->Refresh();

	g_layerMain->m_formCastleWarTimeSet->m_war_info = g_layerMain->m_formCastleWarReg->m_war_info;

	g_layerMain->m_formCastleWarTimeSet->Refresh();

	return true;

	unguard;
}

bool net_r2c_castlewar_is_transported(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	ShowSystemMessage(SYSMSG_IS_TRANSPORTED);

	return true;

	unguard;
}

bool net_r2c_castlewar_apply_flag_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	BYTE	err_no;
	READb_byte(err_no);

	switch(err_no)
	{
	case ERR_NONE_CWAR:
		break;
	case ERR_NOT_VASSAL_CWAR:
		//ShowSystemMessage(STR_ERR_NOT_VASSAL_CWAR);
		break;
	case ERR_DIFF_FACTION_CWAR:
		ShowSystemMessage(STR_ERR_DIFF_FACTION_CWAR);
		break;
	case ERR_MULTI_FLAG:
		ShowSystemMessage(STR_ERR_MULTI_FLAG);
		break;
	case ERR_NO_REG_INFO:
		ShowSystemMessage(STR_ERR_NO_REG_INFO);
		break;
	default:
		return false;
	}

	return true;

	unguard;
}

bool net_r2c_castlewar_user_data_chg(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	char* p_feod_name;
	BYTE  is_attacker;

	READb_string(p_feod_name);
	READb_byte(is_attacker);

	GetWorld()->m_pPlayer->m_feod_name = p_feod_name;

	GetWorld()->m_pPlayer->m_is_attacker = is_attacker;

	return true;

	unguard;
}

bool net_r2c_castlewar_insert_flag_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	BYTE	err_no;
	READb_byte(err_no);

	switch(err_no)
	{
	case ERR_UNKNOWN_FLAG:
		ShowSystemMessage(STR_ERR_UNKNOWN_FLAG);
		break;
	case ERR_HAVE_FLAG_ROUND:
		ShowSystemMessage(STR_ERR_HAVE_FLAG_ROUND);
		break;
	case ERR_TIME_WRONG:
		ShowSystemMessage(STR_ERR_TIME_WRONG);
		break;
	case ERR_PLACE_WRONG:
		ShowSystemMessage(STR_ERR_PLACE_WRONG);
		break;
	case ERR_MULTI_FLAG:
		ShowSystemMessage(STR_ERR_MULTI_FLAG);
		break;
	default:
		return false;
	}

	return true;

	unguard;
}

bool net_r2c_castlewar_flagnpc_created(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	char* p_union_name;
	long  id;

	READb_string(p_union_name);
	READb_long(id);

	std::string union_name = p_union_name;

	GcActor *p_actor = FindAllActor(id);
	
	if(p_actor == NULL)
	{
		CCastleWarNetHelper::NetSend_c2r_castlewar_flagnpc_show(union_name, id);
	}else
	{
		p_actor->m_unionName = union_name.c_str();
        if (p_actor->m_pHUD && g_layerMain->m_eSetting.m_bShowUnionName)
        {
		    p_actor->m_pHUD->SetGuildText(union_name);
        }
	}

	return true;
	unguard;
}

//-----------------------------------------
//以下是客户端消息建构并发送
//-----------------------------------------
bool CCastleWarNetHelper::NetSend_c2r_castlewar_reg_attack(DWORD npc_id)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_castlewar);
	pPacket->WriteShort(c2r_castlewar_reg_attack);
	pPacket->WriteLong(npc_id);
	return NetSend(pPacket);
}

bool CCastleWarNetHelper::NetSend_c2r_castlewar_reg_defend(DWORD npc_id)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_castlewar);
	pPacket->WriteShort(c2r_castlewar_reg_defend);
	pPacket->WriteLong(npc_id);
	return NetSend(pPacket);
}

bool CCastleWarNetHelper::NetSend_c2r_castlewar_reg_defend_cancel(DWORD npc_id)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_castlewar);
	pPacket->WriteShort(c2r_castlewar_reg_defend_cancel);
	pPacket->WriteLong(npc_id);
	return NetSend(pPacket);
}

bool CCastleWarNetHelper::NetSend_c2r_castlewar_select_time(DWORD npc_id, short week_day)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_castlewar);
	pPacket->WriteShort(c2r_castlewar_select_time);
	pPacket->WriteLong(npc_id);
	pPacket->WriteShort(week_day);
	return NetSend(pPacket);
}

bool CCastleWarNetHelper::NetSend_c2r_castlewar_allow_bund(DWORD npc_id, DWORD union_id, BYTE op)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_castlewar);
	pPacket->WriteShort(c2r_castlewar_allow_bund);
	pPacket->WriteLong(npc_id);
	pPacket->WriteLong(union_id);
	pPacket->WriteByte(op);
	return NetSend(pPacket);
}

bool CCastleWarNetHelper::NetSend_c2r_castlewar_reg_attack_cancel(DWORD npc_id)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_castlewar);
	pPacket->WriteShort(c2r_castlewar_reg_attack_cancel);
	pPacket->WriteLong(npc_id);
	return NetSend(pPacket);
}

bool CCastleWarNetHelper::NetSend_c2r_castlewar_view_info(DWORD npc_id)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_castlewar);
	pPacket->WriteShort(c2r_castlewar_view_info);
	pPacket->WriteLong(npc_id);
	return NetSend(pPacket);
}

bool CCastleWarNetHelper::NetSend_c2r_castlewar_apply_flag(DWORD npc_id)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_castlewar);
	pPacket->WriteShort(c2r_castlewar_apply_flag);
	pPacket->WriteLong(npc_id);
	return NetSend(pPacket);
}

bool CCastleWarNetHelper::NetSend_c2r_castlewar_flagnpc_show(std::string union_name, DWORD o_id)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_castlewar);
	pPacket->WriteShort(c2r_castlewar_flagnpc_show);
	pPacket->WriteString((char*)union_name.c_str());
	pPacket->WriteLong(o_id);
	return NetSend(pPacket);
}