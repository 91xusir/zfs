#ifndef __CMD_H__
#define __CMD_H__


#define CMD_FUNCTION(func) int func(CG_CmdPacket *cmd)
CMD_FUNCTION(cmd_c2r_login);
CMD_FUNCTION(cmd_c2r_start);
CMD_FUNCTION(cmd_c2r_chat);
CMD_FUNCTION(cmd_c2r_update_position);
CMD_FUNCTION(cmd_c2r_attack);
CMD_FUNCTION(cmd_c2r_update_model);
CMD_FUNCTION(cmd_c2r_ride);

CMD_FUNCTION(cmd_c2r_p_change_model);
CMD_FUNCTION(cmd_c2r_select_char);
CMD_FUNCTION(cmd_c2r_p_change_item);
CMD_FUNCTION(cmd_c2r_get_model_info);
CMD_FUNCTION(cmd_c2r_add_attr_point);
CMD_FUNCTION(cmd_c2r_subsystem_mail);
CMD_FUNCTION(cmd_c2r_subsystem_team);
CMD_FUNCTION(cmd_c2r_subsystem_item);
CMD_FUNCTION(cmd_c2r_subsystem_union);
CMD_FUNCTION(cmd_c2r_subsystem_feod);
CMD_FUNCTION(cmd_c2r_interactive_npc);
CMD_FUNCTION(cmd_c2r_message_dialog_sure);
CMD_FUNCTION(cmd_c2r_herolist_query);
CMD_FUNCTION(cmd_c2r_cancel_task);
CMD_FUNCTION(cmd_c2r_study_skill);
CMD_FUNCTION(cmd_c2r_study_life_skill);
CMD_FUNCTION(cmd_c2r_shortcut);
CMD_FUNCTION(cmd_c2r_save_config);
CMD_FUNCTION(cmd_c2r_query_task_overhead);
CMD_FUNCTION(cmd_c2r_mw_add_mp);
CMD_FUNCTION(cmd_c2r_mw_add_ele);
CMD_FUNCTION(cmd_c2r_print_my_data);
CMD_FUNCTION(cmd_c2r_recover_pet);
CMD_FUNCTION(cmd_c2r_play_pose);
CMD_FUNCTION(cmd_c2r_teleport);
CMD_FUNCTION(cmd_c2r_quit_game);
CMD_FUNCTION(cmd_c2r_confirm_switch_region);
CMD_FUNCTION(cmd_c2r_request_clear_skill);
CMD_FUNCTION(cmd_c2r_confirm_clear_skill);
CMD_FUNCTION(cmd_c2r_set_pet_info);
//----------add start by tony 05.06.02---------------------------------------//
CMD_FUNCTION(cmd_c2r_subsystem_castlewar);		//castle war related//
//----------add end   by tony 05.06.02---------------------------------------//
//rendh 2010.01.21
CMD_FUNCTION(cmd_c2r_subsystem_linesys);
//rendh 2010.01.21
CMD_FUNCTION(cmd_c2r_get_next_gift);
CMD_FUNCTION(cmd_c2r_query_player_info);
CMD_FUNCTION(cmd_c2r_finish_fb_task);
CMD_FUNCTION(cmd_c2r_enter_fb);
CMD_FUNCTION(cmd_c2r_leave_fb);
CMD_FUNCTION(cmd_c2r_report_info);
CMD_FUNCTION(cmd_c2r_charge_card);
CMD_FUNCTION(cmd_c2r_check_answer);
CMD_FUNCTION(cmd_c2r_master_prentice);
CMD_FUNCTION(cmd_c2r_request_card_list);
CMD_FUNCTION(cmd_c2r_buy_card);
CMD_FUNCTION(cmd_c2r_consign_card);
CMD_FUNCTION(cmd_c2r_exp_card);
CMD_FUNCTION(cmd_c2r_master_list);
CMD_FUNCTION(cmd_c2r_subsystem_auction);
CMD_FUNCTION(cmd_c2r_warfield_enter);
CMD_FUNCTION(cmd_c2r_warfield_leave);
CMD_FUNCTION(cmd_c2r_warfield_view);
CMD_FUNCTION(cmd_c2r_save_circle);
CMD_FUNCTION(cmd_c2r_active_circle);
CMD_FUNCTION(cmd_c2r_rage_burst);
CMD_FUNCTION(cmd_c2r_send_message);

//PZH
CMD_FUNCTION(cmd_c2r_shared_task);
CMD_FUNCTION(cmd_c2r_accept_shared_task);
//
CMD_FUNCTION(cmd_c2r_user_rebirth);
CMD_FUNCTION(cmd_c2r_pickitem_type);
CMD_FUNCTION(cmd_c2r_pickitem_id);
CMD_FUNCTION(cmd_c2r_close_pickwindow);
CMD_FUNCTION(cmd_c2r_stop_getskin);
CMD_FUNCTION(cmd_c2r_upload_taskid);
CMD_FUNCTION(cmd_c2r_upload_finish_taskid);
CMD_FUNCTION(cmd_c2r_task_track);
CMD_FUNCTION(cmd_c2r_task_canecl_track);
//Tianh
CMD_FUNCTION(cmd_c2r_delete_life_skill);

CMD_FUNCTION(cmd_c2r_pk_pick_item);

CMD_FUNCTION(cmd_c2r_Guide);

CMD_FUNCTION(cmd_c2r_change_pkmode);

CMD_FUNCTION(cmd_c2r_SynOnlineTime);

CMD_FUNCTION(cmd_c2r_ctrl_pet);
CMD_FUNCTION(cmd_c2r_fuse_pet);
CMD_FUNCTION(cmd_c2r_catch_pet);

CMD_FUNCTION(cmd_c2r_rebirth_user);
CMD_FUNCTION(cmd_c2r_execute_script);
CMD_FUNCTION(cmd_c2r_get_alterable);
CMD_FUNCTION(cmd_c2r_set_refusesetting);

#endif
