
#include "gc_include.h"
#include "gc_login.h"
#include "region_client.h"
#include "game_script_interface.h"
#include "game_assistant.h"
#include "gc_track_bomb_missile.h"
#include "net_command_union.h"
#include "gc_constant.h"
#include "herolist_base.h"
#include "ui_form_gm.h"
#include "ui_form_herolist.h"
#include "ui_form_union.h"
#include "ui_form_char.h"
#include "ui_form_playeritems.h"
//----------------add start by tony 05.06.06---------------------------------//
#include "net_command_castlewar.h"
//----------------add end   by tony 05.06.06---------------------------------//
#include "net_command_auction.h"
#include "ui_form_itemsinfo.h"
#include "ui_form_analyse_image.h"
#include "lgs_gws.h"
#include "gc_prentice.h"
#include "ui_form_compose.h"
#include "ui_form_warfield.h"
#include "ui_form_minimap.h"
/* gao 2010.1.29 分线定义头文件 */
#include "../shared/linesys_net_define.h"
/* end */
#include "ui_form_playershop.h"
#include "ui_form_trump.h"
#include "UIForm_Pickup.h"
#include "ui_form_cre_info.h"
#include "ui_form_task.h"
#include "ui_form_timebar.h"
#include "ui_form_skill.h"
#include "UIForm_Char.h"


/*------------------------------------------------------------------------
-   网络命令.
------------------------------------------------------------------------*/

bool net_r2c_login_ret(short cmdID, CG_CmdPacket *pPacket); // 登录成功
bool net_r2c_chat(short cmdID, CG_CmdPacket *pPacket);      // 聊天信息
bool net_r2c_world_news(short cmdID, CG_CmdPacket *pPacket);      //世界公告
bool net_r2c_init(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_create_char(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_update_object_info(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_snapshot(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_active_pet(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_ride_ret(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_event(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_adjust_position(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_show_dialog(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_model_info(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_change_model(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_update_show_info(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_update_creature(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_subsystem_mail(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_subsystem_team(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_subsystem_item(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_subsystem_union(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_subsystem_feod(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_herolist(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_switch_region(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_force_position(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_name_state(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_kick(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_you_are_dead(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_refresh_skill(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_gm_response(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_shortcut(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_task_event(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_task_overhead(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_gm_bulletin(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_system_message(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_load_config(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_update_cre_delta(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_mw_add_mp(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_mw_add_ele(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_refresh_effect(short cmdID, CG_CmdPacket* pPacket);
//------------add start by tony 05.06.06-------------------------------------//
bool net_r2c_subsystem_castlewar(short cmdID, CG_CmdPacket* pPacket);
//------------add end   by tony 05.06.06-------------------------------------//
bool net_r2c_show_clear_skill_ui(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_user_data(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_user_state(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_get_gift_response(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_play_effect(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_player_info(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_finish_fb_task(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_check_question(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_wg_checked(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_lock_user(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_master_prentice(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_update_card_list(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_buy_card_ret(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_subsystem_auction(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_match_list(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_update_skill_exp(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_master_list(short cmdID,CG_CmdPacket* pPacket);
bool net_r2c_warfield_msg(short cmdID,CG_CmdPacket* pPacket);
bool net_r2c_warfield_view_rep(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_warfield_report(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_save_circle_rep(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_active_circle_rep(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_updata_npc_color(short cmdID, CG_CmdPacket* pPacket);
/* gao 2010.1.29 分线系统的网络消息*/
bool net_r2c_subsystem_linesys(short cmdID, CG_CmdPacket* pPacket);
/* end */
bool net_r2c_user_rebirth(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_pickitem_type(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_pickitem_id(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_remove_actor(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_visible_actor(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_restore_model(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_start_trigger(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_gm_showid(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_enter_dungeon(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_trigger_close(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_start_process_bar(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_refresh_equip_wear(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_show_message_dialog(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_pk_drop_item(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_Guide(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_Chang_Body(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_update_DeBuf(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_update_npc_animation(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_display(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_rebirth_user(short cmdID,CG_CmdPacket* pPacket);
bool net_r2c_Skill_Series(short cmdID,CG_CmdPacket* pPacket);
bool net_r2c_show_timer(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_show_npc_info(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_show_schedule(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_Ultimate_Change(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_Skill_ChangeBody(short cmdID, CG_CmdPacket* pPacket);
bool net_r2c_CallNpc_Count(short cmdID, CG_CmdPacket* pPacket);

//added by ldr123
bool net_r2c_SynOnlineTime(short cmdID, CG_CmdPacket* pPacket);	//同步用户在线累计时间



#define PRINT_NET_MSG

#ifdef PRINT_NET_MSG
#   define net_print_cmdname    LOG(__FUNCTION__"\n")
#else
#   define net_print_cmdname 
#endif

void net_register()
{
    NetBindCmd(r2c_login_ret,           	net_r2c_login_ret);
    NetBindCmd(r2c_chat,                	net_r2c_chat);
	NetBindCmd(r2c_chat_NewS,               net_r2c_world_news);
    NetBindCmd(r2c_init,                	net_r2c_init);
    NetBindCmd(r2c_create_char,         	net_r2c_create_char);
    NetBindCmd(r2c_update_object_info,  	net_r2c_update_object_info);
    NetBindCmd(r2c_snapshot,            	net_r2c_snapshot);
    NetBindCmd(r2c_active_pet,          	net_r2c_active_pet);
    NetBindCmd(r2c_ride_ret,            	net_r2c_ride_ret);
    NetBindCmd(r2c_event,               	net_r2c_event);
    NetBindCmd(r2c_adjust_position,     	net_r2c_adjust_position);
    NetBindCmd(r2c_show_dialog,         	net_r2c_show_dialog);
    NetBindCmd(r2c_model_info,          	net_r2c_model_info);
	NetBindCmd(r2c_change_model,        	net_r2c_change_model);
    NetBindCmd(r2c_update_show_info,    	net_r2c_update_show_info);
	NetBindCmd(r2c_update_creature,			net_r2c_update_creature);
	NetBindCmd(r2c_subsystem_mail,			net_r2c_subsystem_mail);
	NetBindCmd(r2c_subsystem_team,			net_r2c_subsystem_team);
	NetBindCmd(r2c_subsystem_item,			net_r2c_subsystem_item);
	NetBindCmd(r2c_subsystem_union,			net_r2c_subsystem_union);
	NetBindCmd(r2c_subsystem_feod,			net_r2c_subsystem_feod);
	NetBindCmd(r2c_herolist,				net_r2c_herolist);
	NetBindCmd(r2c_switch_region,			net_r2c_switch_region);
    NetBindCmd(r2c_force_position,      	net_r2c_force_position);
    NetBindCmd(r2c_name_state,          	net_r2c_name_state);
    NetBindCmd(r2c_kick,                	net_r2c_kick);
    NetBindCmd(r2c_you_are_dead,        	net_r2c_you_are_dead);
    NetBindCmd(r2c_refresh_skill,       	net_r2c_refresh_skill);
	NetBindCmd(r2c_gm_response,				net_r2c_gm_response);
	NetBindCmd(r2c_shortcut,				net_r2c_shortcut);
	NetBindCmd(r2c_task_event,				net_r2c_task_event);
	NetBindCmd(r2c_task_overhead,			net_r2c_task_overhead);
	NetBindCmd(r2c_gm_bulletin,				net_r2c_gm_bulletin);
	NetBindCmd(r2c_system_message,			net_r2c_system_message);
	NetBindCmd(r2c_load_config,				net_r2c_load_config);
	NetBindCmd(r2c_update_cre_delta,		net_r2c_update_cre_delta);
	NetBindCmd(r2c_mw_add_mp,       		net_r2c_mw_add_mp);
	NetBindCmd(r2c_mw_add_ele,				net_r2c_mw_add_ele);
    NetBindCmd(r2c_refresh_effect,      	net_r2c_refresh_effect);
//------------add start by tony 05.06.06-----------------------------------------//
	NetBindCmd(r2c_subsystem_castlewar, 	net_r2c_subsystem_castlewar);
//------------add end   by tony 05.06.06-----------------------------------------//
	NetBindCmd(r2c_show_clear_skill_ui, 	net_r2c_show_clear_skill_ui);
	NetBindCmd(r2c_user_data,				net_r2c_user_data);
	NetBindCmd(r2c_user_state,				net_r2c_user_state);
	NetBindCmd(r2c_get_gift_response,		net_r2c_get_gift_response);
	NetBindCmd(r2c_play_effect,				net_r2c_play_effect);
	NetBindCmd(r2c_player_info,				net_r2c_player_info);
	NetBindCmd(r2c_finish_fb_task,			net_r2c_finish_fb_task);
	NetBindCmd(r2c_check_question,			net_r2c_check_question);
	NetBindCmd(r2c_wg_checked,		    	net_r2c_wg_checked);
	NetBindCmd(r2c_lock_user,				net_r2c_lock_user);
	NetBindCmd(r2c_master_prentice,			net_r2c_master_prentice);
	NetBindCmd(r2c_update_card_list,		net_r2c_update_card_list);
	NetBindCmd(r2c_buy_card_ret,	    	net_r2c_buy_card_ret);
	NetBindCmd(r2c_subsystem_auction,		net_r2c_subsystem_auction);
	NetBindCmd(r2c_master_list,				net_r2c_master_list);
	NetBindCmd(r2c_match_list,          	net_r2c_match_list);
	NetBindCmd(r2c_update_skill_exp,    	net_r2c_update_skill_exp);
	NetBindCmd(r2c_warfield_msg,			net_r2c_warfield_msg);
	NetBindCmd(r2c_warfield_view_rep,		net_r2c_warfield_view_rep);
	NetBindCmd(r2c_warfield_report,			net_r2c_warfield_report);
	NetBindCmd(r2c_save_circle_rep,			net_r2c_save_circle_rep);
	NetBindCmd(r2c_active_circle_rep,		net_r2c_active_circle_rep);
	NetBindCmd(r2c_updata_npc_color,        net_r2c_updata_npc_color);
	/* gao 2010.1.29 分线相关消息 */
	NetBindCmd(r2c_subsystem_linesys,		net_r2c_subsystem_linesys);
	/* end */
	NetBindCmd(r2c_user_rebirth,		net_r2c_user_rebirth);
	NetBindCmd(r2c_pickitem_type,		net_r2c_pickitem_type);
	NetBindCmd(r2c_pickitem_id,			net_r2c_pickitem_id);
	NetBindCmd(r2c_remove_actor,		net_r2c_remove_actor);
	NetBindCmd(r2c_Hiding,				net_r2c_visible_actor);
	NetBindCmd(r2c_Lurk,				net_r2c_restore_model);
	NetBindCmd(r2c_start_trigger,		net_r2c_start_trigger);
	NetBindCmd(r2c_gm_showid,			net_r2c_gm_showid);
	NetBindCmd(r2c_enter_dungeon,		net_r2c_enter_dungeon);
	NetBindCmd(r2c_trigger_close,		net_r2c_trigger_close);
	NetBindCmd(r2c_start_process_bar,   net_r2c_start_process_bar);
	NetBindCmd(r2c_refresh_equip_wear,  net_r2c_refresh_equip_wear);
	NetBindCmd(r2c_show_message_dialog, net_r2c_show_message_dialog);
	NetBindCmd(r2c_pk_drop_item,		net_r2c_pk_drop_item);
	NetBindCmd(r2c_SynOnlineTime,		net_r2c_SynOnlineTime);
	NetBindCmd(r2c_Guide,               net_r2c_Guide);
	NetBindCmd(r2c_Chang_Body,          net_r2c_Chang_Body);
	NetBindCmd(r2c_update_DeBuf,        net_r2c_update_DeBuf);
	NetBindCmd(r2c_ctrl_pet,			net_r2c_ctrl_pet);
	NetBindCmd(r2c_fuse_pet,			net_r2c_fuse_pet);
	NetBindCmd(r2c_update_npc_animation,net_r2c_update_npc_animation);
	NetBindCmd(r2c_catch_pet,			net_r2c_catch_pet);
	NetBindCmd(r2c_display,             net_r2c_display);	
	NetBindCmd(r2c_rebirth_user,		net_r2c_rebirth_user);
	NetBindCmd(r2c_Skill_Series,		net_r2c_Skill_Series);
	NetBindCmd(r2c_show_timer,          net_r2c_show_timer);
	NetBindCmd(r2c_show_npc_info,       net_r2c_show_npc_info);
	NetBindCmd(r2c_show_schedule,       net_r2c_show_schedule);
	NetBindCmd(r2c_Ultimate_Change,     net_r2c_Ultimate_Change);
	NetBindCmd(r2c_Skill_ChangeBody,    net_r2c_Skill_ChangeBody);
	NetBindCmd(r2c_CallNpc_Count,       net_r2c_CallNpc_Count);
	
	
}
bool net_r2c_CallNpc_Count(short cmdID, CG_CmdPacket* pPacket)
{
	long actorId = 0;
	long Count = 0;
	READb_long(actorId);
	READb_long(Count);
	
	GcActor *pActor = FindAllActor(actorId);
	if (!pActor)
	{
		return false;
	}
	pActor->m_CallNpcCount = Count;
	return true;
}

bool net_r2c_Skill_ChangeBody(short cmdID, CG_CmdPacket* pPacket)
{
	long actorId = 0;
	long UltimateChange  = 0;
	long IsHiding = 0;
	READb_long(actorId);
	READb_long(UltimateChange);
	READb_long(IsHiding);

	GcActor *pActor = FindAllActor(actorId);
	if (!pActor)
	{
		return false;
	}

	pActor->vUltimateChange(UltimateChange);
	pActor->SkillHideModel(IsHiding);

	return true;
}

//超级变身  武台技能
bool net_r2c_Ultimate_Change(short cmdID, CG_CmdPacket* pPacket)
{
	long actorId = 0;
	long UltimateChange = 0;
	READb_long(actorId);
	pPacket->SerializeBit(UltimateChange,1);


	GcActor *pActor = FindAllActor(actorId);
	if (!pActor)
	{
		return false;
	}
	pActor->vUltimateChange(UltimateChange);
	return true;
}

bool net_r2c_show_schedule(short cmdID, CG_CmdPacket* pPacket)
{
	long lActorID;
	long iTime;
	char* sCommandText;

	READb_long(lActorID);
	READb_long(iTime);
	READb_string(sCommandText);

	g_layerMain->strCommand = sCommandText;

	GcActor* pActor = FindAllActor(lActorID);
	if (pActor)
	{
		pActor->PlayActivityProgressBar(lActorID, iTime);
	}

	return true;
}

bool net_r2c_show_npc_info(short cmdID, CG_CmdPacket* pPacket)
{
	long num = 0;
	char* infoName;
	char* infoNum;

	READb_long(num);

	if (num == 0)
	{
		g_layerMain->m_formSceneInfo->Hide();
		g_layerMain->m_formSceneInfo1->Hide();
		g_layerMain->m_formSceneInfo2->Hide();
	}

	for (DWORD i = 0; i < num; i++)
	{
		std::string infoTotal = "";
		READb_string(infoName);
		READb_string(infoNum);
		infoTotal += infoName;
		infoTotal += infoNum;

		g_layerMain->ShowSceneInfo(num, i, infoTotal);
	}

	return true;
}

bool net_r2c_show_timer(short cmdID, CG_CmdPacket* pPacket)
{
	char* sActivityText;
	long dwActivityTime = 0;

	READb_string(sActivityText);
	READb_long(dwActivityTime);

	g_layerMain->ResetActivityTime();
	g_layerMain->bStartActivity = true;
	g_layerMain->strActivityText = sActivityText;
	g_layerMain->lActivityTime = dwActivityTime;

	return true;
}

bool net_r2c_update_npc_animation(short cmdID, CG_CmdPacket* pPacket)
{
	long id;
	char *posename;
	READb_long(id);
	READb_string(posename);

	GcActor *pActor = FindAllActor(id);
	if (pActor)
	{
		pActor->mBaseActor.PlayPose(posename);
	}
	return true;
}

bool net_r2c_Skill_Series(short cmdID, CG_CmdPacket* pPacket)
{	
	long actorId = 0;
	long SkillSeries = 0;

	READb_long(actorId);
	READb_long(SkillSeries);

	GcActor *pActor = FindAllActor(actorId);
	if (!pActor)
	{
		return false;
	}

	pActor->m_SkillSeries = SkillSeries;
	return true;
}

bool net_r2c_display(short cmdID, CG_CmdPacket* pPacket)
{
	short type = 0;
	long actorId = 0;

	READb_short(type);
	READb_long(actorId);

	GcActor *pActor = FindAllActor(actorId);
	if (!pActor)
	{
		return false;
	}


	switch (type)
	{
	case GEDANG://格挡
		{
			pActor->ShowHeadNotify(CHAR_GEDANG,0);
		}
		break;
	case JINGYAN://经验
		{
			long exp = 0;
			READb_long(exp);

			pActor->ShowHeadNotify(CHAR_JINGYAN,-exp);//这里只有传成负的  显示才是+
		}
		break;
	case DIKANG: //抵抗
		{
		}
		break;
	case XISHOU://吸收
		{
			long Dmg = 0;
			READb_long(Dmg);
			pActor->ShowHeadNotify(CHAR_XISHOU,Dmg);
		}
		break;
	case LINGLI://灵力
		{
			long vSoul = 0;
			READb_long(vSoul);
			pActor->ShowHeadNotify(CHAR_LINGLI,-vSoul);
		}
		break;
	case XIUWEI://修为
		{
			long Cul = 0;
			READb_long(Cul);
			pActor->ShowHeadNotify(CHAR_XIUWEI,-Cul);
		}
		break;
	case SHENGWANG: //声望
		{
			long Popularity = 0;
			READb_long(Popularity);
			pActor->ShowHeadNotify(CHAR_SHENGWANG,-Popularity);
		}
		break;
	case MENGONG: //门贡
		{
		}
		break;
	case BANGGONG://帮贡
		{
		}
		break;
	case ZHILIAO: //加血
		{
			long hp = 0;
			char Hit;
			short eleDmg[MAX_ELEMENT_ATTR] = {0,0,0};

			READb_long(hp);
			READb_byte(Hit);

			if (Hit)
			{
				pActor->ShowHeadNotify(CHAR_BAOJIJIAXUE,-hp);
			}
			else
			{
				pActor->ShowDamageNumber(USE_SKILL_OK,-hp,eleDmg);
			}
		}
		break;
	}

	return true;
}

bool net_r2c_update_DeBuf(short cmdID, CG_CmdPacket* pPacket)
{
	long actorId = 0;
	short iDmgPhy = 0;
	short iDmgElement[MAX_ELEMENT_ATTR];

	READb_long(actorId);
	READb_short(iDmgPhy);
	READb_short(iDmgElement[ELEMENT_WATER]);
	READb_short(iDmgElement[ELEMENT_FIRE]);
	READb_short(iDmgElement[ELEMENT_POISON]);

	GcActor *pActor = FindAllActor(actorId);
	if (pActor)
	{
		pActor->ShowDamageNumber(0,iDmgPhy,iDmgElement);
	}

	return true;

}

bool net_r2c_Chang_Body(short cmdID, CG_CmdPacket* pPacket)
{
	long actorId = 0;
	long modelId = 0;
	char headId = 0;
	long lShapeshift = 0;
	READb_long(actorId);
	READb_long(modelId);
	READb_byte(headId);
	pPacket->SerializeBit(lShapeshift,1);
	GcActor *pActor = FindAllActor(actorId);
	if (pActor)
	{
		pActor->m_cShapeshiftNet = lShapeshift == 0 ? lShapeshift : lShapeshift+1;
		pActor->m_core.HeadModelId = headId;
		if (pActor->m_cShapeshift == 1)
		{
			pActor->m_dwShapeshiftModelID = modelId;
		}
		else
		{
			if (pActor != GetPlayer())
			{
				pActor->m_dwShapeshiftModelID = modelId;
			}
			else
			{
				pActor->m_dwShapeshiftModelID = modelId;
				pActor->m_cShapeshift = 1;
				if (!pActor->GetGraph()->p()->IsPlayingPose() || pActor->GetGraph()->p()->GetCurrentPose().Name != "skill_q3")
				{
					pActor->m_cShapeshift = pActor->m_cShapeshiftNet;
					if (pActor->m_dwShapeshiftModelID)
					{
						pActor->SetModel(pActor->m_dwShapeshiftModelID, pActor->m_core.HeadModelId);
						pActor->mItem.UpdateGraph();
						pActor->m_dwShapeshiftModelID = 0;
						pActor->m_cShapeshift = pActor->m_cShapeshiftNet;
						pActor->GetGraph()->p()->ClearComnandList();
					}
				}
			}
		}
	}
	return true;
}

bool net_r2c_Guide(short cmdID, CG_CmdPacket* pPacket)
{
	short sType;
	if(!pPacket->ReadShort(&sType)) return false;

	GetWorld()->OnNetDownloadCharGuide(sType);
	return true;
}
bool net_r2c_show_message_dialog(short cmdID, CG_CmdPacket* pPacket)
{
	short sType;
	char* sShowText;
	char* sCommandText;

	if(!pPacket->ReadShort(&sType)) return false;
	if(!pPacket->ReadString(&sShowText)) return false;
	if(!pPacket->ReadString(&sCommandText)) return false;

	GetWorld()->OnNetDownloadDialogScript(sType, sShowText, sCommandText);
	return true;
}

bool net_r2c_refresh_equip_wear(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
	char count = 0;
	char part[16];
	long wear[16];

	READb_byte(count);
	for (char i = 0;i<count;++i)
	{
		READb_byte(part[i]);
		READb_long(wear[i]);
		GetPlayer()->mItem.SetEquipWear((CItemContainerBase::EEquipParts)part[i],wear[i]);
	}
	return true;
	unguard;
}

bool net_r2c_start_process_bar(short cmdID, CG_CmdPacket* pPacket)
{
	g_layerMain->m_fromPlayerItems->PlayTaskItem(2);
	GetPlayer()->mBaseActor.PlayPose("make_n0", false);
	return true;
}
bool net_r2c_trigger_close(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
	long dungeonUID;
	long dungeonTID;
	long process;
	READb_long(dungeonUID);
	READb_long(dungeonTID);
	READb_long(process);

	std::map<int,STrigger> &triggerMap = g_TableFB.m_trigger;
	std::map<int,STrigger>::iterator iter;
	for (iter=triggerMap.begin();iter!=triggerMap.end();++iter)
	{
		if (iter->second.fbID == dungeonTID && iter->second.id != 0)
		{
			long close = 0;
			pPacket->SerializeBit(close,1);
			iter->second.close = close;

			if (process != 0 && iter->second.type != 3 && process >= iter->second.process)
			{
				GetWorld()->SetTriggerBlock(iter->second.id);
			}
		}
	}
	return true;
	unguard;
}

bool net_r2c_enter_dungeon(short cmdID, CG_CmdPacket* pPacket)
{
	long dungeonUID;
	long dungeonTID;
	long process;
	READb_long(dungeonUID);
	READb_long(dungeonTID);
	READb_long(process);

	GetPlayer()->m_dungeonTID = dungeonTID;
	if (dungeonUID && dungeonTID)
	{
		GetWorld()->EnterDungon(dungeonUID,dungeonTID,process);
	}
	else
	{
		GetWorld()->LeaveDungon();
	}

	return true;
}

bool net_r2c_gm_showid(short cmdID, CG_CmdPacket* pPacket)
{
	GetPlayer()->m_bShowID = !GetPlayer()->m_bShowID;
	return true;
}
bool net_r2c_start_trigger(short cmdID, CG_CmdPacket* pPacket)
{
	if (!GetWorld()->GetDungeon())
	{
		ERR("客户端副本信息不存在\n");
		return true;
	}

	short id;
	long triggerID;
	long opendoorID;
	long blockID;
	char close;
	long start = 0;
	long changeBlock = 0;

	READb_short(id);

	STrigger *pTrigger = GetWorld()->GetDungeon()->GetTrigger(id);
	if (!pTrigger)
	{
		ERR("机关信息不存在\n");
		return true;
	}

	pPacket->SerializeBit(changeBlock,1);

	if (changeBlock == 1)
	{
		GetWorld()->SetTriggerBlock(id);
	}
	else
	{
		READb_byte(close);

		// 触发
		READb_long(triggerID);
		// 是否启动
		pPacket->SerializeBit(start,2);

		if (start == 2)
		{
			GetPlayer()->CheckCanStartTrigger(id,true);
			return true;
		}

		GcActor *pTriggerActor = FindAllActor(triggerID);
		if ( pTriggerActor )
		{
			pTriggerActor->triggerID = id;
		}

		if ( start )
		{
			if (pTriggerActor)
				pTriggerActor->m_bTriggerStart = true;

			pTrigger->close = close;
			pTrigger->state = close + 2;

			// 表现
			for (int i=0;i<pTrigger->opendoorNum;++i)
			{
				READb_long(opendoorID);
				pTrigger->opendoorUID[i] = opendoorID;
				GcActor *pActor = FindAllActor(opendoorID);
				if ( pActor )
				{
					pActor->triggerID = id;
					//pActor->m_lTriggerCloseNet = pTrigger->close;
					if ( !pTriggerActor )
					{
						pActor->StartTriggerAdd(pTrigger->close,true);
						pActor->m_bTriggerOver = true;
						pTrigger->opendoorCurNum ++;
					}
				}else if( !pTriggerActor )
				{
					pTrigger->opendoorCurNum ++;
				}
			}
			//READb_long(opendoorID);
			//GcActor *pOpenDoor = FindAllActor(opendoorID);
			//if ( pOpenDoor )
				//pOpenDoor->triggerMain = 1;

			if (pTrigger->type == 3)
			{
				// 阻挡
				for (int i=0;i<pTrigger->blockCount;++i)
				{
					READb_long(blockID);
					pTrigger->blockUID[i] = blockID;
					GcActor *pActor = FindAllActor(blockID);
					if ( pActor )
					{
						pActor->triggerID = id;
					}
				}
			}

			if ( pTriggerActor )
			{
				pTriggerActor->StartTriggerAdd(close,true);
			}
		}else if ( pTriggerActor )
			pTriggerActor->StartTriggerAdd(close,false);
	}

	return true;
}
bool net_r2c_restore_model(short cmdID, CG_CmdPacket* pPacket)
{
	long id;
	long restore;
	READb_long(id);
	pPacket->SerializeBit(restore,1);
	GcActor* pActor = FindAllActor(id);
	if ( pActor )
	{
		pActor->ChangeEffectToActor();
	}
	return true;
}
bool net_r2c_visible_actor(short cmdID, CG_CmdPacket* pPacket)
{
	long id;
	long hide;
	READb_long(id);
	pPacket->SerializeBit(hide,1);

	GcActor* pActor = FindAllActor(id);
	if ( pActor )
	{
		pActor->SkillHideModel(hide);
	}
	return true;
}
bool net_r2c_remove_actor(short cmdID, CG_CmdPacket* pPacket)
{
	long id;

	READb_long(id);

	GetWorld()->m_ActorManager.ActorDie(id,4);
	if (id == GetPlayer()->m_lPickActorID)
	{
		if (g_layerMain->m_formPickItem->IsVisible())
		{
			g_layerMain->m_formPickItem->Hide();
			PlayerFailOperate(R(MSG_ITEM_DISAPPEAR));
		}
	}
	return true;
}

bool net_r2c_pk_drop_item(short cmdID, CG_CmdPacket* pPacket)
{
	char count;
	READb_byte(count);
	for (char i=0;i<count;++i)
	{
		SItemID itemID;
		BOOL_ENSURE(itemID.Serialize(pPacket));
		g_layerMain->m_formPickItem->OnInsertNewItem(itemID);
	}
	if (g_layerMain->m_formPickItem->IsVisible())
	{
		g_layerMain->m_formPickItem->Hide();
	}
	GetPlayer()->m_dwGetDropTime = rtGetMilliseconds() + PK_DROP_TIME_MILLSECOND;
	g_layerMain->m_formPickItem->Reset();
	g_layerMain->m_formPickItem->Show(true);
	return true;
}

bool net_r2c_pickitem_type(short cmdID, CG_CmdPacket* pPacket)
{
	//拾取还是扒皮
	//成功与否
	//拾取则发道具数组.或已经消失等失败原因.
	//扒皮则发送成功道具皮,或失败原因
	char cType;
	char cFail;

	READb_byte(cType);
	READb_byte(cFail);

	switch(cType)
	{
	case 1:
		{
			switch(cFail)
			{
			case 0:
				{
					char abc;
					READb_byte(abc);
					for (char i=0;i<abc;++i)
					{
						SItemID itemID;
						BOOL_ENSURE(itemID.Serialize(pPacket))
						// 加入UI
						g_layerMain->m_formPickItem->OnInsertNewItem(itemID);
					}
					g_layerMain->m_formPickItem->Reset();
					g_layerMain->m_formPickItem->Show();
				}
				break;
			case 1:
				{
					g_layerMain->m_formPickItem->Hide();
					ShowBattleInfoMessage(R(MSG_ITEM_ALLHAVEGETBYOTHERS));
				}
				break;
			case 2:
				{
					ShowBattleInfoMessage(R(MSG_ITEM_NOITEMCANGET));
				}
				break;
			}
		}
		break;
	case 2:
		{
			long pMonsterID;
			long pActorID;
			switch(cFail)
			{
			case 0:
				{
					READb_long(pMonsterID);
					READb_long(pActorID);
					GcActor* pActor = FindAllActor(pActorID);
					if ( pActor )
					{
						pActor->SetWeaponTrumpVisible(true,true);
						pActor->m_bIsCollect = false;
						pActor->RemoveCurCommand();
					}
					if( pActorID == GetPlayer()->ID() )
					{
						SItemID itemID;
						BOOL_ENSURE(itemID.Serialize(pPacket))
						g_layerMain->m_formPickItem->OnInsertNewItem(itemID);
						g_layerMain->m_formPickItem->Reset();
						g_layerMain->m_formPickItem->Show();
						g_layerMain->m_formCollectBar->Hide();
						pActor->SetIsTaskCollect(false);
					}
				}
				break;
			case 1:
				{
					GetPlayer()->m_bIsCollect = false;
					g_layerMain->m_formCollectBar->Hide();
					ShowBattleInfoMessage(R(MSG_ITEM_ALLHAVEGETBYOTHERS));
				}
				break;
			case 2:
				{
					GetPlayer()->m_bIsCollect = false;
					g_layerMain->m_formCollectBar->Hide();
					GetPlayer()->SetWeaponTrumpVisible(true,true);
					ShowBattleInfoMessage(R(MSG_ITEM_NOITEMCANGET));
					GetPlayer()->RemoveCurCommand();
				}
				break;
			case 3:
				{
					GetPlayer()->m_bIsCollect = false;
					g_layerMain->m_formCollectBar->Hide();
					GetPlayer()->SetWeaponTrumpVisible(true,true);
					PlayerFailOperate(R(MSG_ITEM_CANNOTGETCAUSEKILLED));
					GetPlayer()->RemoveCurCommand();
				}
				break;
			case 4:
				{
					READb_long(pMonsterID);
					READb_long(pActorID);
					GcActor* pActor = FindAllActor(pActorID);
					if ( pActor )
					{
						if (pActor->GetIsTaskCollect())
						{
							PlayerFailOperate(R(MSG_TASK_PLAYCOLLECTING));
							pActor->PlayCollectTask(pMonsterID);
							pActor->SetWeaponTrumpVisible(false,false);
						} 
						else
						{
							pActor->CollectAdd(pMonsterID);
							pActor->SetWeaponTrumpVisible(false,false);
						}
					}
				}
				break;
			case 5:
				PlayerFailOperate(R(MSG_ITEM_CANNOTGETCAUSEMONSTER));
				GetPlayer()->m_bIsCollect = false;
				g_layerMain->m_formCollectBar->Hide();
				GetPlayer()->SetWeaponTrumpVisible(true,true);
				GetPlayer()->RemoveCurCommand();
				break;
			case 9:
				//PlayerFailOperate(R(MSG_SKILL_BOPILOW));
				GetPlayer()->m_bIsCollect = false;
				g_layerMain->m_formCollectBar->Hide();
				GetPlayer()->SetWeaponTrumpVisible(true,true);
				GetPlayer()->RemoveCurCommand();
				break;
			case 10:
				PlayerFailOperate(R(MSG_ITEM_OTHERSUSING));
				GetPlayer()->m_bIsCollect = false;
				g_layerMain->m_formCollectBar->Hide();
				GetPlayer()->SetWeaponTrumpVisible(true,true);
				GetPlayer()->RemoveCurCommand();
				break;
			case 11:
				PlayerFailOperate(R(MSG_ITEM_NOTOOLS));
				GetPlayer()->m_bIsCollect = false;
				g_layerMain->m_formCollectBar->Hide();
				GetPlayer()->SetWeaponTrumpVisible(true,true);
				GetPlayer()->RemoveCurCommand();
				break;
			case 18:
				GetPlayer()->m_bIsCollect = false;
				g_layerMain->m_formCollectBar->Hide();
				GetPlayer()->SetWeaponTrumpVisible(true,true);
				PlayerFailOperate(R(MSG_TASK_ITEMFULL));
				GetPlayer()->RemoveCurCommand();
				break;
			case 13:
				GetPlayer()->m_bIsCollect = false;
				g_layerMain->m_formCollectBar->Hide();
				GetPlayer()->SetWeaponTrumpVisible(true,true);
				PlayerFailOperate(R(MSG_CAIJI_FAIL));
				GetPlayer()->RemoveCurCommand();
				break;
			case 12:
				PlayerFailOperate(R(MSG_ITEM_OTHERSUSING));
				GetPlayer()->m_bIsCollect = false;
				g_layerMain->m_formCollectBar->Hide();
				GetPlayer()->SetWeaponTrumpVisible(true,true);
				GetPlayer()->RemoveCurCommand();
				break;
			case 24:
				{
					READb_long(pMonsterID);
					READb_long(pActorID);
					GcActor* pActor = FindAllActor(pActorID);
					if ( pActor )
					{

					}
					break;
				}
			default:
				{
					GetPlayer()->m_bIsCollect = false;
					g_layerMain->m_formCollectBar->Hide();
					GetPlayer()->SetWeaponTrumpVisible(true,true);
					GetPlayer()->RemoveCurCommand();
				}
				break;
			}
		}
		break;
	}
	return true;
}
bool net_r2c_pickitem_id(short cmdID, CG_CmdPacket* pPacket)
{
	long lMouseRD;
	pPacket->SerializeBit(lMouseRD,1);
	char cFail;
	READb_byte(cFail);
	long itemID;
	if (lMouseRD == 0)
	{
		READb_long(itemID);
	}else
	{
		if (g_layerMain->m_formPickItem->IsVisible())
		{
			g_layerMain->m_formPickItem->Hide();
		}
	}
	switch(cFail)
	{
	case 0:
		{
			if (lMouseRD == 0)
			{
				g_layerMain->m_formPickItem->OnDelOneItem(itemID);
			}
		}
		break;
	case 1:
		{
			ShowBattleInfoMessage(R(MSG_ITEM_ALLHAVEGETBYOTHERS));
		}
		break;
	case 2:
		ShowBattleInfoMessage(R(MSG_ITEM_ITEMHASDISAPPEAR));
		break;
	}
	if (g_layerMain->m_formCollectBar->IsVisible())
	{
		g_layerMain->m_formCollectBar->Hide();
	}
	return true;
}

bool net_r2c_user_rebirth(short cmdID, CG_CmdPacket *pPacket)
{
	char cType;
	long pActorID;
	READb_long(pActorID);

	GcActor* pActor = FindAllActor(pActorID);
	if (!pActor)
		return true;

	pActor->mBaseActor.GetGraph()->EnableShadow(true);

	if(pPacket->ReadByte(&cType))
	{
		if (cType == 1 || cType == 2 || cType == 4)
		{
			GcActor::SCmd cmd;
			cmd.eCmd = GcActor::ACMD_PLAY_POSE;
			cmd.dw[0] = GcBaseActor::POSE_RELIVE;
			pActor->AddCommand(cmd);
			pActor->m_bIsDead = 0;
		}
		if (pActor == GetPlayer())
		{
			g_layerMain->m_formRebirth->Hide();
			if(g_layerMain->m_formChar->IsVisible() && g_layerMain->m_formChar->GetNowShowType() == CUIForm_Char::EQUIP)
			{
				g_layerMain->m_formChar->ReCreate3DViewGraph();
			}
		}
		return true;
	}
	return false;
}

bool net_r2c_login_ret(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
    long lLoginReturn;
    if(pPacket->ReadLong(&lLoginReturn))
    {
        LOG1("网络: 登陆登陆服务器 [%s]\n", (lLoginReturn==0?R(G_SUCCEED):R(G_FAIL)));
    }
    if (GetWorld())
    {
        GetWorld()->m_bLogined = (lLoginReturn==0);
        GetWorld()->m_tLogoutTime = 0;
    }
    if (lLoginReturn==0)
    {
        if (GetNet())
        {
            GetNet()->ResetDisconnectCnt();
        }
    }
    return true;
	unguard;
}


bool net_r2c_chat(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
/*	r2c_chat		
		说明：聊天信息
		参数：
		类型		含义
		byte        	所属频道
		long        	发言人id
		string		发言人名字
		string       	内容
*/
    long lID;
    BYTE bChannel;
    char* pStr, *pName;
    READb_byte(bChannel);
    READb_long(lID);
    READb_string(pName);
    READb_string(pStr);

	if (bChannel==CHAT_CHANNEL_GM)
	{
		g_layerMain->m_formGm->OnRecvMsg(lID, pName, pStr);
	}
	else
	{
		GetWorld()->m_Chat.Receive(bChannel,lID,pName,pStr);

		GcActor* pActor = FindAllActor(lID);
		if (pActor && pActor->m_pHUD)
		{
			if(pActor == GetPlayer() && g_layerMain->m_eSetting.m_bShowSelfSpeak)
				pActor->m_pHUD->SetSpeakText(pStr);
			else if(g_layerMain->m_eSetting.m_bShowOtherSpeak)
				pActor->m_pHUD->SetSpeakText(pStr);
		}
	}
    return true;
	unguard;
}

bool net_r2c_world_news(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	char *pPlayerName, *pOtherName, *pBossName, *pSceneName, *pItemName;
	long type;

	READb_string(pPlayerName);
	READb_string(pOtherName);
	READb_string(pBossName);
	READb_string(pSceneName);
	READb_long(type);
	READb_string(pItemName);

	char str[2048];
	switch (type)
	{
	case SMITHINGWEAPONS:     //铸兵
		/*str = "[打造兵器][";
		str += pPlayerName;
		str += "]两手一戳，衣袖一卷，抡起大铁锤乒乒乓乓一阵乱敲乱打。不多时，便见一件兵器在手中成形，直看众人惊声长叹：“偶的乖乖，这快便打出一件人间凶器";
		str += pItemName;
		str += "，不亏是凶器量产师。";*/
		rt2_sprintf(str,R(MSG_PUBLIC_DAZAOBINGQI),pPlayerName,pItemName);
		break;
	case SMITHINGARMOR:       //铸甲
		/*str = "[打造护甲][";
		str += pPlayerName;
		str += "]对着熔炉边锤边唱：“我屋子破，炉子破，我手上的锤儿破，你笑我，他笑我，可我出的都是超级货。”歌声停，锤子停，只见霞光冲天，一件超绝古今的";
		str += pItemName;
		str += "就此出世。";*/
		rt2_sprintf(str,R(MSG_PUBLIC_DAZAOHUJIA),pPlayerName,pItemName);
		break;
	case SMITHINGWEAPONS4:     //铸兵制造出强化等级+4以及+4以上武器
		/*str = "[铸兵制造]一滴汗，两滴汗，小汗，大汗，汗如雨下……沉默地[";
		str += pPlayerName;
		str += "]突然右手甩锤，蹬脚上桌，仰天大笑：“通过我成吉思汗的努力，终于打造出了威力绝伦的";
		str += pItemName;*/
		rt2_sprintf(str,R(MSG_PUBLIC_ZHUBING),pPlayerName,pItemName);
		break;
	case SMITHINGARMOR4:     //铸甲制造出强化等级+4以及+4以上武器
		/*str = "[铸甲制造][";
		str += pPlayerName;
		str += "]盯着炉火愣愣发呆，恍恍惚惚做起梦来。梦中仙宫美景无数，星君仙蛾穿梭，好不惬意。忽然一阵剧痛，左手被自个锤儿砸出鲜血，落在锻材上，就此造出一件天下无双";
		str += pItemName;
		str += "来";*/
		rt2_sprintf(str,R(MSG_PUBLIC_ZHUJIA),pPlayerName,pItemName);
		break;
	case STRENGTHENEQUIP:     //装备强化
		/*str = "[装备强化]乌云滚滚，天雷阵阵，突然一道闪电落下，砸中正在镶嵌宝石的[";
		str += pPlayerName;
		str += "]，使得他头发曲卷，脸色焦黑，衣裳褴褛。虽然有些破相，机缘之下却炼成了";
		str += pItemName;*/
		rt2_sprintf(str,R(MSG_PUBLIC_QIANGHUA),pPlayerName,pItemName);
		break;
	case BRATBOSS:            //击败BOSS
		/*str = "[击败BOSS][";
		str += pBossName;
		str += "]终因实力不济，被[";
		str += pPlayerName;
		str += "]一番蹂躏后，在[";
		str += pSceneName;
		str += "]不甘地倒下，离开了这忘不掉的美好人间。";*/
		rt2_sprintf(str,R(MSG_PUBLIC_JIBAIBOSS),pBossName,pPlayerName,pSceneName);
		break;
	case RENOVATEBOSS:        //BOSS刷新
		/*str = "[BOSS刷新]在[";
		str += pSceneName;
		str += "]刮起一阵妖风，风吹沙走，昏天暗地，一个邪恶的身影从地底冒出，[";
		str += pBossName;
		str += "]又回到人间了。";*/
		rt2_sprintf(str,R(MSG_PUBLIC_BOSSSHUAXIN),pSceneName,pBossName);
		break;
	case BRATFINALBOSSl:      //打败终极BOSS
		/*str = "[终极BOSS]剑已断，甲已破，面对无比强大的[";
		str += pBossName;
		str += "]，[";
		str += pPlayerName;
		str += "]仍旧毫无惧色，凭着无比坚定的斗志，全力施为，历经一番苦斗，终于将邪恶再次送入了轮回。";*/
		rt2_sprintf(str,R(MSG_PUBLIC_JIBAIZHONGJIBOSS),pBossName,pPlayerName);
		break;
	case FIELDKILLPERSON:     //野外杀人
		/*str = "[武林战事][";
		str += pPlayerName;
		str += "]修为高深，手段犀利，手起刀落间便将[";
		str += pOtherName;
		str += "]斩杀[";
		str += pSceneName;
		str += "]，临走时还冷冷地对着尸体道：“若要报仇，月圆之夜，红枫树下，我等你。”";*/
		rt2_sprintf(str,R(MSG_PUBLIC_WULINZHANSHI),pPlayerName,pOtherName,pSceneName);
		break;
	default:
		//str = NULL;
		rt2_sprintf(str," ");
		break;
	}

	PlayerFailOperate(str);
	return true;
	unguard;
}

bool net_r2c_init(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
    net_print_cmdname;
    GetWorld()->m_NetSynachro.DownloadInitPacket(pPacket);
    return true;
	unguard;
}

bool net_r2c_create_char(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
    net_print_cmdname;
	GetWorld()->OnNetDownloadCreateChar(pPacket);
    return true;
	unguard;
}

bool net_r2c_update_object_info(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
    //net_print_cmdname;
    return true;
	unguard;
}

bool net_r2c_snapshot(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
    //net_print_cmdname;
    GcWorld* pWorld = GetWorld();
    pWorld->m_NetSynachro.DownloadNetSnapshot(pPacket);
    if (!pWorld->m_bLogined)
    {
        if (time(NULL)-pWorld->m_tLogoutTime > 10)
        {
            pWorld->m_bLogined = true;
            pWorld->m_tLogoutTime = 0;
        }
    }
    return true;
	unguard;
}

bool net_r2c_active_pet(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
    long oid;
	long itemid = 0;
	if(!pPacket->ReadLong(&oid)) return false;
	if (oid != 0)
	{
		if(!pPacket->ReadLong(&itemid)) return false;
	}
	GetWorld()->m_nPetItemID = itemid;
    GetWorld()->OnPetActive((DWORD)oid,(DWORD)itemid);
    return true;
	unguard;
}

bool net_r2c_ride_ret(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
    GetWorld()->OnNetDownloadRidePet(pPacket);
    return true;
	unguard;
}

bool net_r2c_event(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
    //net_print_cmdname;
    GetWorld()->m_NetSynachro.DownloadNetEvent(pPacket);
    return true;
	unguard;
}

bool net_r2c_adjust_position(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
    net_print_cmdname;
    return true;
	unguard;
}

bool net_r2c_show_dialog(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
    short sType;
    long oid;
    char *str, *name, *value;
	long taskid;
	char cFinished;
    if(!pPacket->ReadLong(&oid)) return false;
    if(!pPacket->ReadShort(&sType)) return false;
    switch (sType)
    {
    case DIALOG_HTML:
        if(!pPacket->ReadString(&str)) return false;
		if(!pPacket->ReadLong(&taskid)) return false;
		if(!pPacket->ReadByte(&cFinished)) return false;
        if(pPacket->ReadString(&name))
        {
            if (!pPacket->ReadString(&value)) return false;
        }else
        {
            name = NULL;
            value = NULL;
        }
        GetWorld()->OnNetDownloadDialogHTML((DWORD)oid, str, name, value, taskid, cFinished);
        break;
    case DIALOG_NPC_TRADE:
        GetWorld()->OnNetDownloadDialogByType((DWORD)oid, (WORD)sType);
        break;
    }
    return true;
	unguard;
}

bool net_r2c_model_info(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
    net_print_cmdname;

    SItemID     item;
	long		ID;						//物体ID
	short		ModelID;				//模型的ID
	short		HeadModelID;			//人物的头部ID
	byte		ModelChangeCount;		//模型改变计数	
    char*       pName;
	char		metier;
	char        sex;
	char		bPShop;
	long		unionID;
	char*		strPShopName;
	char*		strUnionName;
	char*		strUnionNickName;
	char*		strUnionIcon;
	char*		p_feod_name;
	BYTE		is_attacker;
	long		dwState;
	long		shapeshift;

	READb_long(ID);
    READb_string(pName);

	GcActor* pActor=FindAllActor(ID);
	if(pActor==NULL)
	{
		return false;
	}
	if (pActor->NpcType() == GcActor::ENT_PET)
	{
		char type = 0;
		long fuselev = 0;
		long abilitylev = 0;
		long masterid = 0;

		READb_byte(type);
		READb_long(fuselev);
		READb_long(abilitylev);
		READb_long(masterid);

		pActor->m_ePetType = (PetType)type;
		pActor->m_dwPetMasterID = masterid;
		pActor->m_dwPetFuseLev = fuselev;
		pActor->m_dwPetAbilityLev = abilitylev;

		pActor->SetName(pName);
		return true;
	}
	//if (pActor->NpcType() == GcActor::ENT_CALLNPC)
	//{
	//	pActor->SetName(pName);
	//	return true;
	//}

	READb_short(ModelID);
	READb_short(HeadModelID);
	READb_byte(ModelChangeCount);
	pPacket->SerializeBit(shapeshift,1);
	if (shapeshift)
		pActor->m_cShapeshiftNet = 2;
	else
		pActor->m_cShapeshiftNet = 0;
	//pActor->m_cShapeshiftNet = (char)((shapeshift == 1) ? 2 : 0);


	pActor->SetModel(ModelID, HeadModelID);
	if ( !pActor->m_mapSetEffect.empty() )
	{
		pActor->m_Skill.NetReadEffect(pActor->m_mapSetEffect);
		pActor->m_mapSetEffect.clear();
	}
    if (!pActor->mItem.Serialize(NET_UPDATE_ITEM_SHOW, pPacket)) {NET_READ_ERROR;return false;}
	READb_byte(metier);
	READb_byte(sex);
	READb_byte(bPShop);
	READb_string(strPShopName);
	READb_long(unionID);
	READb_string(strUnionName);
	READb_string(strUnionNickName);
	READb_string(strUnionIcon);
	READb_string(p_feod_name);
	READb_byte(is_attacker);
	READb_long(dwState);

    pActor->SetName(pName);
	pActor->m_bPShop = bPShop;
	pActor->m_PShopName = strPShopName;
	g_layerMain->m_formPlayerShop->OnRecvActorPShop(pActor);

	pActor->SetUnionID(unionID);
	pActor->m_unionName = strUnionName;
	pActor->m_unionNickName = strUnionNickName;
	pActor->m_unionIcon = strUnionIcon;
	g_layerMain->m_formUnion->OnRecvActorUnionData(pActor);

    pActor->mItemChangeCount = ModelChangeCount;

	pActor->m_core.Metier = metier;
	pActor->m_core.Sex = sex;
	pActor->m_feod_name = p_feod_name;
	pActor->m_is_attacker = is_attacker;
	pActor->m_dwState = dwState;

	if(pActor->m_bPShop)
	{
		pActor->OnSetWeaponShow(false);
		pActor->SetModel(5501,0);
	}
	else if (!pActor->GetRidePet())
	{
		pActor->OnSetWeaponShow(true);
		pActor->mItem.UpdateGraph();
	}
	return true;
	unguard;
}

bool net_r2c_change_model(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	net_print_cmdname;
	short sModelID;
	if(!pPacket->ReadShort(&sModelID)) return false;
	if (GetPlayer()->m_cShapeshift == 1)
	{
		GetPlayer()->m_dwShapeshiftModelID = sModelID;
	}
	else
	{
		GetPlayer()->SetModel(sModelID, GetPlayer()->m_core.HeadModelId);
		GetPlayer()->mItem.UpdateGraph();
		GetPlayer()->mBaseActor.PlayEffect("elevup",true,NULL,RtgVertex3(0,0,0));
	}

	return true;
	unguard;
}

bool net_r2c_update_show_info(short cmdID, CG_CmdPacket *pPacket)
{
    guard;
    //net_print_cmdname;
    GetWorld()->OnNetDownloadShowInfo(pPacket);
    return true;
    unguard;
}

bool net_r2c_update_creature(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	//net_print_cmdname;
	
	long oid;
	READb_long(oid);

	GcActor *actor = FindAllActor(oid);
	if(actor)
    {
        actor->UpdateFromServer(pPacket);
    }

	return true;
	unguard;
}

bool net_r2c_update_cre_delta(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	//net_print_cmdname;

	long oid;
	READb_long(oid);

	GcActor *actor = FindAllActor(oid);
	if(actor)
	{
		actor->UpdateFromServerDelta(pPacket);
	}
	return true;
	unguard;
}

bool net_r2c_force_position(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	char  flag;
    long  x, y, id;
    char* pSceneName;
	long bTeleSkill = 0;

	READb_long(id);
	READb_byte(flag);
	READb_string(pSceneName); // 场景名字
    READb_long(x);
    READb_long(y);
	pPacket->SerializeBit(bTeleSkill,1);

	if(id == GetPlayer()->ID())
	{
		GetWorld()->SetSceneFile(pSceneName);
		GetWorld()->m_bLogined = true;

		GetPlayer()->m_posFlag = flag;
		if (!bTeleSkill || !GetPlayer()->m_bTeleEvent)
		{
			//GetWorld()->m_ActorManager.Clear();
			//GetWorld()->m_pPlayer->mBaseActor.RemoveSelf();
			GetPlayer()->MoveToNow((float)x, (float)y);
		}
		else
		{
			//GetPlayer()->m_vTelePoint.x = x;
			//GetPlayer()->m_vTelePoint.y = y;
			GetPlayer()->MoveToNow((float)x, (float)y);
		}
	}
	else
	{
		GcActor *actor = FindAllActor(id);
		if(actor)
		{
			if (!bTeleSkill)
			{
				actor->MoveToNow((float)x, (float)y);
			}
			else
			{
				//actor->m_vTelePoint.x = x;
				//actor->m_vTelePoint.y = y;
				actor->MoveToNow((float)x, (float)y);
			}
		}
    }
	return true;
	unguard;
}

bool net_r2c_name_state(short cmdID, CG_CmdPacket *pPacket)
{
    guard;
    /*
    char cPKState;
    long lPKChangeTime;
    READb_char(cPKState);
    READb_long(lPKChangeTime);
    GcPlayer* pPlayer = GetPlayer();
    if (pPlayer)
    {
        pPlayer->SetPKState(cPKState);
    }
    */
    return true;
    unguard;
}

bool net_r2c_you_are_dead(short cmdID, CG_CmdPacket *pPacket)
{
    guard;
    char cKillerType; // 0 未知, 1 玩家, 2 怪
    char* szKillName = "";
    READb_char(cKillerType);
    if (cKillerType)
    {
        READb_string(szKillName);
    }

    //ShowMessage(R(MSG_YOUAREDEAD));
	GetWorld()->m_pPlayer->AddCommand(GcActor::ACMD_DEATH);

	if (g_layerMain)
	{
		time_t iNowTime;
		time(&iNowTime);
		GetWorld()->m_pPlayer->m_iRebirthTime = iNowTime + 300;

		g_layerMain->m_fromPlayerItems->RefreshQuickStart();
		if (GetWorld()->m_pPlayer)
		{
			GetWorld()->m_pPlayer->SetAutobattle(false);
			g_layerMain->m_btnAutoBattle->SetChecked(false);
			GetWorld()->m_iDeadport = 1;	//heten
		}
	}

    return true;
    unguard;
}

bool net_r2c_subsystem_mail(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	// net_print_cmdname;
	GetWorld()->m_Mail.Receive(pPacket);
	return true;
	unguard;
}

bool net_r2c_subsystem_team(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	// net_print_cmdname;
	GetWorld()->m_Team.ReceiveFromServer(pPacket);
	return true;
	unguard;
}

bool net_r2c_master_list(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
	GetWorld()->m_MasterList.ReceiveFromServer(pPacket);
	return true;
	unguard;
}

bool net_r2c_subsystem_item(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	// net_print_cmdname;
	return ItemCmdProcess_client(pPacket);
	unguard;
}

bool net_r2c_subsystem_union(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	// net_print_cmdname;
	return UnionCmdProcess_client(pPacket);
	unguard;
}

bool net_r2c_subsystem_feod(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	// net_print_cmdname;
	return FeodCmdProcess_client(pPacket);
	unguard;
}

bool net_r2c_herolist(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	long version;
	SHeroListItem heroItem;
	SUnionListItem unionItem;

	READb_long(version);
	g_layerMain->m_formHeroList->m_HeroList.m_version = version;
	g_layerMain->m_formHeroList->m_HeroList.Serialize(pPacket);
	g_layerMain->m_formHeroList->Refresh();

	return true;

	unguard;
}

bool net_r2c_switch_region(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
    GetWorld()->OnNetDownloadSwitchRegion(pPacket);
	return true;
	unguard;
}

bool net_r2c_kick(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	char cCause;
    char* szCause;

	if (!pPacket->ReadByte(&cCause))
		return false;

    if (cCause==KICK_USER_USERDEFINE)
    {
	    if (!pPacket->ReadString(&szCause))
		    return false;
    }else
    {
        szCause = "";
    }

	GetWorld()->OnNetKick(cCause, szCause);

	return true;

	unguard;
}

bool net_r2c_refresh_skill(short cmdID, CG_CmdPacket* pPacket)
{
    guard;
    GetPlayer()->m_Skill.NetReadSkill(pPacket);
	g_layerMain->m_fromSkill->OnRefreshShortcut();
    GetPlayer()->m_Skill.UpdateUI();
    return true;
    unguard;
}

bool net_r2c_refresh_effect(short cmdID, CG_CmdPacket* pPacket)
{
    guard;
    /*if (GetPlayer()->m_Skill.NetReadEffect(pPacket))
    {
		if(GetPlayer()->ID()== g_layerMain->m_formMonInfo->GetActorID())
			GetPlayer()->m_Skill.UpdateEffectUIForActor(0);
        GetPlayer()->m_Skill.UpdateEffectUI();
    }*/
    return true;
    unguard;
}

bool net_r2c_gm_response(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	long GmID;
	char* GmName;
	short QuestionCount;
	short Order;
	READb_long(GmID);
	READb_string(GmName);
	READb_short(QuestionCount);
	READb_short(Order);
	g_layerMain->m_formGm->OnRecvResponse(GmID, GmName, QuestionCount, Order);
	return true;
	unguard;
}

bool net_r2c_shortcut(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	GetWorld()->OnNetDownloadShortcut(pPacket);

	return true;

	unguard;
}

bool net_r2c_load_config(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
	GetWorld()->OnNetDownloadConfigData(pPacket);
	return true;
	unguard;
}

bool net_r2c_task_event(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	char res;
	READb_char(res);
	RtgVertex3 pos;
	if (res==0)			// cancel task
	{
		g_pSoundMgr->PlayOnce("task_fail.wav", false, 0, pos);
	}
	else if (res==1)	// task success
	{
		GetWorld()->m_pPlayer->mBaseActor.PlayEffect("eff_task_finish.act",true,NULL,RtgVertex3(0,0,0));
		g_pSoundMgr->PlayOnce("task_success.wav", false, 0, pos);
	}
	else if (res==2)	// accept task
	{
		GetWorld()->m_pPlayer->mBaseActor.PlayEffect("eff_task.act",true,NULL,RtgVertex3(0,0,0));
		g_pSoundMgr->PlayOnce("task_accept.wav", false, 0, pos);
	}

    if (g_layerMain)
    {
        g_layerMain->m_fromPlayerItems->Refresh();
		g_layerMain->m_fromPlayerItems->RefreshQuickStart();
		g_layerMain->m_formTask->Update();
		g_layerMain->m_formCharSimple->UpdateTaskTrackList();
    }

	return true;
	
	unguard;
}

bool net_r2c_task_overhead(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	long npcOID;
	char cTmp;
	READb_long(npcOID);
	READb_char(cTmp);
	ETaskOverHead state = (ETaskOverHead)cTmp;

	GcActor *pActor = GetWorld()->m_ActorManager.FindAll(npcOID);
	if (pActor && pActor->mBaseActor.m_pStateOverHead)
	{
		pActor->mBaseActor.m_pStateOverHead->RefreshGraph(state);
	}

	return true;

	unguard;
}

bool net_r2c_gm_bulletin(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	long GmID;
	char* content;
	long color;
	char style;
	READb_long(GmID);
	READb_string(content);
	READb_long(color);
	READb_char(style);
	
	GetWorld()->m_Bulletin.OnRecvNewBulletin(content, GmID, color, style);
	return true;

	unguard;
}

bool net_r2c_system_message(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	long msgID;
	READb_long(msgID);

	string str;
	switch(msgID)
	{
	case SYSMESSAGE_TASK_REACH_MAX:
		str = R(MSG_TASK_FULL);
		PlayerFailOperate(str.c_str());
		break;

	default:
		break;
		str = "";
		ShowSystemMessage(str.c_str());
	}

	return true;
	unguard;
}

bool net_r2c_mw_add_mp(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
	
	char ret;
	READb_char(ret);
	switch(ret)
	{
		case 0: // in progress
			break;
		case 1: // mp not enough
			if(GetWorld() && GetWorld()->m_pPlayer)
			{
				GetWorld()->m_pPlayer->AutoEatMPBottle();
			}
			break;
	}

	if(g_layerMain && g_layerMain->m_formMwAddMp)
	{
		g_layerMain->m_formMwAddMp->Update();
	}
	return true;
	unguard;
}

bool net_r2c_mw_add_ele(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
	long succeed;
	char rifine;
	long temp = 0;

	READb_char(rifine);
	pPacket->SerializeBit(succeed,2);
	
	switch (succeed)
	{
	case 3:		// 中断
		break;
	default:
		READb_long(temp);
		break;
	}

	if (g_layerMain && g_layerMain->m_formChar)
	{
		g_layerMain->m_formChar->ReceiveTrumpCmd(rifine,succeed,temp);
	}

	return true;
	unguard;
}
//----------------add start by tony 05.06.06---------------------------------//
bool net_r2c_subsystem_castlewar(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
	// net_print_cmdname;
	return CastleWarCmdProcess_Client(pPacket);
	unguard;
}
//----------------add end   by tony 05.06.06---------------------------------//

bool net_r2c_subsystem_auction(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
	// net_print_cmdname;
	return AuctionCmdProcess_Client(pPacket);
	unguard;
}

bool net_r2c_warfield_msg(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
	long lMsgID;

	READb_long(lMsgID);

	switch(lMsgID)
	{
	case msg_warfield_prepare:
		ShowSystemMessage(R(MSG_WARFIELD_PREPARE));
		break;
	case msg_warfield_will_stop:
		ShowSystemMessage(R(MSG_WARFIELD_WILL_STOP));
		break;
	case msg_warfield_kick_timeout:
		ShowSystemMessage(R(MSG_WARFIELD_KICK_TIMEOUT));
		break;
	case msg_warfield_enter:
		g_layerMain->m_fromMiniMap->WarViewShow();
		g_layerMain->m_formWarfield->m_bReport = false;
		break;
	case msg_warfield_leave:
		g_layerMain->m_fromMiniMap->WarViewHide();
		g_layerMain->m_formWarfield->Clear();
		break;
	case msg_warfield_start:
		ShowSystemMessage(R(MSG_WARFIELD_START));
		g_layerMain->m_fromMiniMap->WarViewShow();
		break;
	case msg_warfield_stop:
		ShowSystemMessage(R(MSG_WARFIELD_STOP));
		g_layerMain->m_fromMiniMap->WarViewHide();
		g_layerMain->m_formWarfield->Clear();
		break;
	//case msg_warfield_start:		
		//break;
	default:
		break;
	}

	return true;
	unguard;
}

bool net_r2c_warfield_view_rep(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	long lRemain;
	long numZhou;
	long numShang;
	float lScoreZhou;
	float lScoreShang;
	float fRZhou;
	float fRShang;

	float lPriHero[3];
	float lInterHero[3];
	float lSeniorHero[3];

	long lUnionNum;
	std::list<SExploit> listExploit;

	READb_long(lRemain);
	READb_long(numZhou);
	READb_float(lScoreZhou);
	READb_float(fRZhou);
	READb_long(numShang);
	READb_float(lScoreShang);
	READb_float(fRShang);
	
	READb_float(lPriHero[0]);
	READb_float(lPriHero[1]);
	READb_float(lPriHero[2]);
	READb_float(lInterHero[0]);
	READb_float(lInterHero[1]);
	READb_float(lInterHero[2]);
	READb_float(lSeniorHero[0]);
	READb_float(lSeniorHero[1]);
	READb_float(lSeniorHero[2]);

	READb_long(lUnionNum);
	for(int i = 0; i < lUnionNum; ++i)
	{
		SExploit tmp;
		tmp.Serialize(pPacket);
		listExploit.push_back(tmp);
	}

	g_layerMain->m_formWarfield->OnRecvUpdateData(lRemain, numZhou, lScoreZhou, numShang, lScoreShang,
								lPriHero, lInterHero, lSeniorHero, &listExploit, fRZhou, fRShang);
	return true;
	unguard;
}

bool net_r2c_save_circle_rep(short cmdID, CG_CmdPacket* pPacket)
{
	long ret;

	READb_long(ret);

	if(!ret)
	{
		ShowSystemMessage(R(MSG_CIRCLE_SAVE_FAILED));
	}else
	{
		ShowSystemMessage(R(MSG_CIRCLE_SAVED));
	}

	return true;
}

bool net_r2c_active_circle_rep(short cmdID, CG_CmdPacket* pPacket)
{
	long ret;

	READb_long(ret);

	if(!ret)
	{
		ShowSystemMessage(R(MSG_CIRCLE_ACTIVE_FAILED));
	}else
	{
		ShowSystemMessage(R(MSG_CIRCLE_ACTIVE));
	}

	return true;
}

bool net_r2c_updata_npc_color(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
	long NPCID;
	long  NameColor;

	READb_long(NPCID);
	READb_long(NameColor);

	GetWorld()->m_dNPCID = NPCID;
	GetWorld()->m_dNPCNameColor = NameColor;
	return true;
	unguard;
}

bool net_r2c_warfield_report(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	long lReason;
	char faction;
	long numZhou;
	long numShang;
	float lScoreZhou;
	float lScoreShang;
	float fRZhou;
	float fRShang;

	float lPriHero[3];
	float lInterHero[3];
	float lSeniorHero[3];

	long lUnionNum;
	std::list<SExploit> listExploit;

	READb_long(lReason);
	READb_byte(faction);
	READb_long(numZhou);
	READb_float(lScoreZhou);
	READb_float(fRZhou);
	READb_long(numShang);
	READb_float(lScoreShang);
	READb_float(fRShang);

	READb_float(lPriHero[0]);
	READb_float(lPriHero[1]);
	READb_float(lPriHero[2]);
	READb_float(lInterHero[0]);
	READb_float(lInterHero[1]);
	READb_float(lInterHero[2]);
	READb_float(lSeniorHero[0]);
	READb_float(lSeniorHero[1]);
	READb_float(lSeniorHero[2]);

	READb_long(lUnionNum);
	for(int i = 0; i < lUnionNum; ++i)
	{
		SExploit tmp;
		tmp.Serialize(pPacket);
		listExploit.push_back(tmp);
	}

	g_layerMain->m_formWarfield->OnReport(lReason, faction, numZhou, lScoreZhou, numShang, lScoreShang,
		lPriHero, lInterHero, lSeniorHero, &listExploit, fRZhou, fRShang);

	return true;
	unguard;
}

bool net_r2c_match_list(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
    GcPlayer* pPlayer = GetPlayer();
    if (pPlayer)
    {
        pPlayer->ReadMatchFromNet(pPacket);
		if (g_layerMain->m_formItemCompose->IsVisible())
		{
			g_layerMain->m_formItemCompose->RefreshPlayerComposeList();
			g_layerMain->m_formItemCompose->Refresh();
		}
    }
    return true;
	unguard;
}

bool net_r2c_update_skill_exp(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
    GcPlayer* pPlayer = GetPlayer();
    if (pPlayer)
    {
        pPlayer->m_Skill.UpdateSkillExp(pPacket);
    }
    return true;
	unguard;
}

bool net_r2c_show_clear_skill_ui(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
	// net_print_cmdname;
	
	short usedCnt,leftCnt;
	long cost;
	READb_short(leftCnt);
	READb_short(usedCnt);
	READb_long(cost);

	g_layerMain->m_formClearSkill->m_spClearLeft  = leftCnt;
	g_layerMain->m_formClearSkill->m_spClearUsed  = usedCnt;
	g_layerMain->m_formClearSkill->m_costGold = cost;

	g_layerMain->m_formClearSkill->Update();
	g_layerMain->m_formClearSkill->SetVisible(true);

	return true;
	unguard;
}

bool net_r2c_user_data(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	SCreatureInfo info;
	if (!info.Serialize(pPacket))
		return false;

	string strMetier = "Unknown";
	SUserActor* pActor = g_TableUserActor.FindUserActor(info.metier, info.metierLev);
	if (pActor)
	{
		strMetier = pActor->MetierName.c_str();
	}

	string strFaction = "Unknown";
	if (info.faction==FACTION_ZHOU)
		//strFaction = "周";//by fox for string
		strFaction = R(G_ZHOU);
	else if(info.faction==FACTION_SHANG)
	{
		//strFaction = "商";//by fox for string
		strFaction = R(G_SHANG);
	}

	/****lxJian*09.12.03****/
	else if(info.faction==FACTION_NEUTRAL)
	{
		strFaction = R(G_NEUTRAL);
	}

	int pkValue = info.pkValue / 1000 / 60;

	char cTmpMsg2048[2048];
//	sprintf(cTmpMsg2048, "%s  ID=%d 场景=(%s) 等级=%d 经验=%d 职业=%s 阵营=%s 诸侯国=(%s) 宠物(等级=%d,HP=%d) HP=%d MP=%d Pk=%d 金钱=%d 仓库金钱=%d 伤害=(%d-%d) 元素伤害=(%d,%d,%d) 元素抵抗=(%d,%d,%d)",
//		info.name.c_str(), info.DbID, info.scene.c_str(), info.level, info.exp, strMetier.c_str(), strFaction.c_str(), info.unionName.c_str(), info.petLevel, info.petHP, info.hp, info.mp, pkValue, info.money, info.bankMoney, info.DamMin, info.DamMax, info.EleDamage[ELEMENT_WATER], info.EleDamage[ELEMENT_FIRE], info.EleDamage[ELEMENT_POISON], info.EleArmor[ELEMENT_WATER], info.EleArmor[ELEMENT_FIRE], info.EleArmor[ELEMENT_POISON]);
	rt2_sprintf(cTmpMsg2048, "%s  场景=(%s) 等级=%d 经验=%d 职业=%s HP=%d MP=%d 诸侯国=(%s) 宠物(等级=%d,HP=%d) 声望=%d 金钱=%d 仓库金钱=%d",
		info.name.c_str(), info.scene.c_str(), info.level, info.exp, strMetier.c_str(), info.hp, info.mp, info.unionName.c_str(), info.petLevel, info.petHP, info.credit, info.money, info.bankMoney);
	ShowSystemMessage(cTmpMsg2048);

	return true;

	unguard;
}

bool net_r2c_user_state(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	long dwState;
	READb_long(dwState);

	if (!GetWorld() || !GetWorld()->m_pPlayer)
		return false;

	GetWorld()->m_pPlayer->m_dwState = dwState;

	return true;

	unguard;
}

bool net_r2c_get_gift_response(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	char errorCode;
	char* giftMsg;
	READb_char(errorCode);
	READb_string(giftMsg);

	switch (errorCode)
	{
	case GIFT_SUCCESS:
		{
			char cTmp1024[1024];
			if (giftMsg && giftMsg[0]!='\0')
			{
				string strTmp1;
				strTmp1 += "(";
				strTmp1 += giftMsg;
				strTmp1 += ")";
				rt2_sprintf(cTmp1024, R(MSG_GIFT_SUCCESS), strTmp1.c_str());
			}
			else 
			{
				rt2_sprintf(cTmp1024, R(MSG_GIFT_SUCCESS), "");
			}
			ShowSystemMessage(cTmp1024);
		}
		break;
	case GIFT_ERROR_UNKNOWN:
		ShowSystemMessage(R(MSG_GIFT_ERROR_UNKNOWN));
		break;
	case GIFT_NONE:
		ShowSystemMessage(R(MSG_GIFT_NONE));
		break;
	case GIFT_CAN_NOT_WITHDRAW:
		ShowSystemMessage(R(MSG_GIFT_CAN_NOT_WITHDRAW));
		break;
	case GIFT_BAG_NOT_ENOUGH:
		ShowSystemMessage(R(MSG_GIFT_BAG_NOT_ENOUGH));
		break;
	default:
		break;
	}

	return true;

	unguard;
}

bool net_r2c_play_effect(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	char Style;
	char* EffectName;
	long ActorID;

	READb_char(Style);
	if (Style==1)
	{
		READb_string(EffectName);
		READb_long(ActorID);
	}
	else 
	{
		return false;
	}

	if (Style==1)
	{
		GcActor* pActor = NULL;
		if (GetWorld()->m_pPlayer->ID() == ActorID)
			pActor = GetWorld()->m_pPlayer;
		else 
			pActor = GetWorld()->m_ActorManager.FindActive(ActorID);

		if (pActor)
            pActor->mBaseActor.PlayEffect(EffectName, false, NULL, RtgVertex3(0,0,0));
	}
	else 
	{
		return false;
	}

	return true;

	unguard;
}

bool net_r2c_player_info(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	long playerOID;
	char* playerName;
	READb_long(playerOID);
	READb_string(playerName);
	g_layerMain->m_formItemsInfo->m_PlayerID = playerOID;
	g_layerMain->m_formItemsInfo->m_PlayerName = playerName;
	if (!g_layerMain->m_formItemsInfo->m_ItemsContainter.Serialize(NET_PLAYER_QUERY_INFO, pPacket))
		return false;

	GcActor *pActor = GetWorld()->m_ActorManager.FindAll(playerOID);
	if (!pActor)
		return true;

	if (pActor->Distance(GetWorld()->m_pPlayer) > g_layerMain->m_formItemsInfo->m_nMaxDistance)
		return true;

	if (g_layerMain->m_formItemsInfo->IsShowing())
		g_layerMain->m_formItemsInfo->Refresh();
	else 
		g_layerMain->m_formItemsInfo->Show();

	return true;

	unguard;
}

bool net_r2c_finish_fb_task(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	long taskId;
	READb_long(taskId);
	RtgVertex3 pos;

	GetWorld()->m_pPlayer->mBaseActor.PlayEffect("eff_task_finish",true,NULL,RtgVertex3(0,0,0));

	return true;
	unguard;
}

bool net_r2c_check_question(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	char type;
	char* data;
	int dataLen;
	short nErrorCode;
	char* strSelections;
	short nSelectNum;
	short nSecondsLeft;
	short nResendLeft;
	short nErrorLeft;

	READb_char(type);
	if (type==0) // ask question
	{
		if (!pPacket->ReadBinary(&data, &dataLen))	// 图片内容
			return false;
		READb_string(strSelections);				// 选择内容
		READb_short(nSelectNum);					// 选择第几个字母
		READb_short(nSecondsLeft);					// 剩余时间
		READb_short(nResendLeft);					// 可重发次数
		READb_short(nErrorLeft);					// 可识别错误次数

		if (g_layerMain)
		{
			g_layerMain->m_formAnalyseImage->SetData(data, dataLen, strSelections, nSelectNum, nSecondsLeft, nResendLeft, nErrorLeft);
			g_layerMain->m_formAnalyseImage->Show();
		}
	}
	else if (type==1) // check result
	{
		READb_short(nErrorCode);

		switch (nErrorCode)
		{
		case 0: // Right!
			ShowSystemMessage(R(MSG_IMAGE_SELECTION_RIGHT_SELECT));
			break;
		case 1: // Wrong!
			ShowSystemMessage(R(MSG_IMAGE_SELECTION_WRONG_SELECT));
			break;
		default:
			break;
		}

		if (g_layerMain)
		{
			g_layerMain->m_formAnalyseImage->ClearData();
			g_layerMain->m_formAnalyseImage->Hide();
		}
	}
	else // invalid
	{
		return false;
	}

	return true;

	unguard;
}

bool net_r2c_wg_checked(short cmdID, CG_CmdPacket* pPacket)
{
    PostQuitMessage(23);
    return true;
}

bool net_r2c_lock_user(short cmdID, CG_CmdPacket* pPacket)
{
	guard;

	char cLock;
	READb_char(cLock);

	switch (cLock)
	{
	case 1:
		PlayerFailOperate(R(MSG_IMAGE_SELECTION_LOCKED));
		break;
	case 2:
		//PlayerFailOperate("您已经被解除锁定了");//by fox for string
		PlayerFailOperate(R(MSG_IMAGE_SELECTION_UNLOCKED));
		break;
	default:
		break;
	}
	return true;

	unguard;
}

bool net_r2c_master_prentice(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
	return PrenticeCmdProcess_client(pPacket);
	unguard;
}

bool net_r2c_update_card_list(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
    if (g_layerMain)
    {
        g_layerMain->UpdateBuyCardList(pPacket);
    }
    return true;
	unguard;
}

bool net_r2c_buy_card_ret(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
    if (g_layerMain)
    {
        g_layerMain->OnBuyCardResult(pPacket);
    }
    return true;
	unguard;
}

/* gao 2010.1.29 分线相关网络消息 */
bool net_r2c_subsystem_linesys(short cmdID, CG_CmdPacket* pPacket)
{
	guard;
	g_layerMain->m_fromMiniMap->OnReceiveLineCmd(pPacket);
	return true;
	unguard;
}

//ldr123
//1.同步在线累计时间 2.新手升级帮助
bool net_r2c_SynOnlineTime(short cmdID, CG_CmdPacket* pPacket)
{
	long dwOnlineTime = 0;
	if (!pPacket->ReadLong(&dwOnlineTime))
	{
		return false;
	}

	g_layerMain->m_bCountDownStart = true;
	g_layerMain->m_nCountDownMillisecond = dwOnlineTime * 1000;
	return true;
}
bool net_r2c_rebirth_user(short cmdID,CG_CmdPacket* pPacket)
{
	long ObjectId;
	long iHPrate;
	long iMPrate;
	pPacket->ReadLong(&ObjectId);
	pPacket->ReadLong(&iHPrate);
	pPacket->ReadLong(&iMPrate);
	g_layerMain->m_formRebirth->OnRebirthFromOther(ObjectId,iHPrate,iMPrate);
	return true;
}
//end

/* end */
/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
