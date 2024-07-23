#ifndef __REGION_CLIENT_H__
#define __REGION_CLIENT_H__

#include "core/rt2_core.h"
#include "net/rt_net.h"
#include "skill_manage.h"
#include "rt1tort2.h"
#include "ActivityInfo.h"
#include <time.h>
#include <set>

using namespace std;

//#define	ENABLE_MEMORY_ALLOCATOR_DEBUG

// -------------------------------------------------------------------
// macro define
// -------------------------------------------------------------------
#define _WARFIELD_		//��ս���غ�

// -------------------------------------------------------------------
// object type define
// -------------------------------------------------------------------
const unsigned char OBJECT_TYPE_OBJECT			= 1;
const unsigned char OBJECT_TYPE_CREATURE		= 2;
const unsigned char OBJECT_TYPE_ITEM			= 3;
const unsigned char OBJECT_TYPE_NPC_SERVICE		= 4;	// �������ʵ�NPC�������ṩ���񣬽��ף��ֿ�ȣ�
const unsigned char OBJECT_TYPE_NPC_COMBATIVE	= 5;	// ս��NPC�����Թ����ͱ�����
const unsigned char OBJECT_TYPE_SCENE           = 6;	
const unsigned char OBJECT_TYPE_SCENE_FB        = 7;	
const unsigned char OBJECT_TYPE_SCENE_WARFIELD  = 8;
const unsigned char OBJECT_TYPE_SPIRIT			= 9;
const unsigned char OBJECT_TYPE_ACTIVITY		= 10;
const unsigned char OBJECT_TYPE_MAX				= 11;

/* chat channel type */
const char CHAT_CHANNEL_NORMAL		= 1;
const char CHAT_CHANNEL_TEAM		= 2;
const char CHAT_CHANNEL_PARTY		= 3;
const char CHAT_CHANNEL_COUNTRY		= 4;
const char CHAT_CHANNEL_PUBLIC		= 5;
const char CHAT_CHANNEL_PRIVATE		= 6;
const char CHAT_CHANNEL_BATTLEINFO	= 7;
const char CHAT_CHANNEL_SYSTEM		= 8;
const char CHAT_CHANNEL_GM			= 9;
const char CHAT_CHANNEL_WORLD		= 10;	//heten
const char CHAT_CHANNEL_MAX			= 11;
const char CHAT_CHANNEL_NEWS		= 12;

const char PLAYER_CALL          = 1;  //����ٻ�
const char BOSS_CALL            = 2;  //BOSS����NPC�ٻ�

/* faction type */
const char FACTION_UNKNOWN      = -1; // δ֪
const char FACTION_ZHOU         = 0;  // ��
const char FACTION_SHANG        = 1;  // ��
const char FACTION_ALL          = 2;  // ȫ��
/****lxJian*09.12.03****/
const char FACTION_NEUTRAL      = 3;  // ����

/* do command */
const unsigned char DO_COMMAND_GETSKIN		= 1;
const unsigned char DO_COMMAND_GETMINE		= 2;
const unsigned char DO_COMMAND_GETMEDICINE	= 3;
const unsigned char DO_COMMAND_RIFINE		= 4;
const unsigned char DO_COMMAND_TASKCOLECT	= 5;
const unsigned char DO_COMMAND_CATCHPET		= 6;

/* sync flag */
const long SYNC_FLAG_COMPLEX_MODEL	= 0x00000001;
const long SYNC_FLAG_POSITION		= 0x00000002;
const long SYNC_FLAG_DIRECTION		= 0x00000004;
const long SYNC_FLAG_USER			= 0x00000008;
const long SYNC_FLAG_PET			= 0x00000010;
const long SYNC_FLAG_SPIRIT			= 0x00000020;
const long SYNC_FLAG_WAR			= 0x00000040;
const long SYNC_FLAG_BOSS			= 0x00000080;
const long SYNC_FLAG_TRIGGER		= 0x00000100;
const long SYNC_FLAG_CALL   		= 0x00000200;

const long SYNC_FLAG_METIER_WARRIOR	= 0x00000400;//0x81;
const long SYNC_FLAG_METIER_HUNTER		= 0x00000800;//0x82;
const long SYNC_FLAG_METIER_WIZARD		= 0x00001000;//0x82;
const long SYNC_FLAG_METIER_TAOIST		= 0x00002000;//0x83;

/* move state */
const unsigned char MOVE_STATE_IDLE             = 0x01;
const unsigned char MOVE_STATE_WALK             = 0x02;
const unsigned char MOVE_STATE_RIDE             = 0x04;
const unsigned char MOVE_STATE_STATIC           = 0x08;
const unsigned char PK_STATE_NORMAL             = 0x00; // ���� PK state
const unsigned char PK_STATE_YELLOW_NAME        = 0x10; // ���� PK state
const unsigned char PK_STATE_RED_NAME           = 0x20; // ���� PK state

/* event define */
const short EVENT_ATTACK			= 1;
const short EVENT_SET_POSITION      = 2;
const short EVENT_LEVEL_UP			= 3;
const short EVENT_BORN				= 4;
const short EVENT_DEAD				= 5;
const short EVENT_STATIC_DIALOG     = 6;
const short EVENT_PLAY_POSE         = 7;
const short EVENT_CHECK_PROCESS     = 8;
const short EVENT_CULLEVEL_UP		= 9;

const short GEDANG			        = 1; //��
const short JINGYAN                 = 2; //����
const short DIKANG        			= 3; //�ֿ�
const short XISHOU   				= 4; //����
const short LINGLI   				= 5; //����
const short XIUWEI                  = 6; //��Ϊ
const short SHENGWANG   			= 7; //����
const short MENGONG   				= 8; //�Ź�
const short BANGGONG                = 9; //�ﹱ
const short ZHILIAO                 = 10; //��Ѫ

//Tianh   ��������
const short SMITHINGWEAPONS         = 1;//����
const short SMITHINGARMOR           = 2;//����
const short SMITHINGWEAPONS4        = 3;//���������ǿ���ȼ�+4�Լ�+4��������
const short SMITHINGARMOR4          = 4;//���������ǿ���ȼ�+4�Լ�+4��������
const short STRENGTHENEQUIP         = 5;//װ��ǿ��
const short BRATBOSS                = 6;//����BOSS
const short RENOVATEBOSS            = 7;//BOSSˢ��
const short BRATFINALBOSSl          = 8;//����
const short FIELDKILLPERSON         = 9;//Ұ��ɱ��

//���ͺ͸��������
const int RELIVEPOSTIONX1           = 560;
const int RELIVEPOSTIONY1           = 940;

const int RELIVEPOSTIONX2           = 3960;
const int RELIVEPOSTIONY2           = 12020;

const int RELIVEPOSTIONX3           = -16300;
const int RELIVEPOSTIONY3           = 12240;

const int RELIVEPOSTIONX4           = 17070;
const int RELIVEPOSTIONY4           = 12970;

const int RELIVEPOSTIONX5           = -3065;
const int RELIVEPOSTIONY5           = 5050;

const int RELIVEPOSTIONXOTHER       = 560;
const int RELIVEPOSTIONYOTHER       = 940;

/* ride operate */
const char RIDE_OFF					= 0;
const char RIDE_ON					= 1;

const int SCRIPT_FUNC_STATE_COUNT	= 20;	//�ű��ù���״ֵ̬����

//const char ULTIMATECHANGEl			= 0;  //��������
//const char DUNLAND 					= 1;  //�ݵ�
//const char HIDING					= 2;  //����
/*��������*/
const short XIANYU                  = 1;
const short GUANFU                  = 2;
const short MANZU                   = 3;
const short YISHI                   = 4;
const short YIJUN                   = 5;
const short LIANMENG                = 6;

/* dialog define */
const short DIALOG_HTML             = 1;
const short DIALOG_NPC_TRADE        = 2;

extern char var[100];

/* region send to client */
const short r2c_base				= 1000;         

const short r2c_remove_item			= r2c_base + 1;
const short r2c_force_position      = r2c_base + 2;
const short r2c_snapshot			= r2c_base + 3;
const short r2c_levelup_skill		= r2c_base + 4;
const short r2c_update_show_info	= r2c_base + 5;
const short r2c_subsystem_team      = r2c_base + 6;
const short r2c_login_ret			= r2c_base + 7;
const short r2c_gm_bulletin			= r2c_base + 8;	// �����
const short r2c_warfield_view_rep	= r2c_base + 9;
const short r2c_mw_add_ele          = r2c_base + 10;
const short r2c_model_info			= r2c_base + 11;
const short r2c_warfield_report		= r2c_base + 12;
const short r2c_show_dialog			= r2c_base + 13;
const short r2c_subsystem_union		= r2c_base + 14;
const short r2c_active_pet          = r2c_base + 15;
const short r2c_kick				= r2c_base + 16;
const short r2c_update_creature		= r2c_base + 17;
const short r2c_chat				= r2c_base + 18;
const short r2c_subsystem_item		= r2c_base + 19;
const short r2c_name_state          = r2c_base + 20;
const short r2c_switch_region		= r2c_base + 21;
const short r2c_subsystem_mail      = r2c_base + 22;
const short r2c_you_are_dead        = r2c_base + 23;
const short r2c_task_overhead		= r2c_base + 24; 
const short r2c_buy_card_ret        = r2c_base + 25;
const short r2c_shortcut			= r2c_base + 26;
const short r2c_task_event			= r2c_base + 27;
const short r2c_system_message		= r2c_base + 28;	// ��Ϣ��ʾ
const short r2c_load_config			= r2c_base + 29;	// �ͻ������õļ���
const short r2c_create_char			= r2c_base + 30;
const short r2c_init			    = r2c_base + 31;
const short r2c_learn_skill			= r2c_base + 32;
const short r2c_ride_ret			= r2c_base + 33;
const short r2c_refresh_skill       = r2c_base + 34;
const short r2c_update_object_info	= r2c_base + 35;
const short r2c_mw_add_mp           = r2c_base + 36;
const short r2c_gm_response			= r2c_base + 37;
const short r2c_herolist			= r2c_base + 38;
const short r2c_user_state			= r2c_base + 39;	// ���״̬
const short r2c_match_list			= r2c_base + 40;	// �䷽�б�
const short r2c_get_gift_response	= r2c_base + 41;	// �����Ʒ�ķ���
const short r2c_update_cre_delta    = r2c_base + 42;
const short r2c_adjust_position		= r2c_base + 43;
const short r2c_change_model		= r2c_base + 44;
const short r2c_lock_user			= r2c_base + 45;	// �������
const short r2c_master_prentice		= r2c_base + 46;	// ʦͽϵͳ
const short r2c_user_data			= r2c_base + 47;	// �ͻ��˴�ӡ�����Ϣ(GM����)
const short r2c_refresh_effect      = r2c_base + 48;
const short r2c_finish_fb_task		= r2c_base + 49;
const short r2c_warfield_msg		= r2c_base + 50;
const short r2c_wg_checked          = r2c_base + 51;    // ��鵽���
const short r2c_recover_pet_ret     = r2c_base + 52;
// const short r2c_play_pose           = r2c_base + 45;
//------------add start by tony 05.06.06-------------------------------------//
const short r2c_subsystem_castlewar = r2c_base + 53;
//------------add end   by tony 05.06.06-------------------------------------//
const short r2c_show_clear_skill_ui = r2c_base + 54;
const short r2c_play_effect			= r2c_base + 55;	// ������Ч
const short r2c_update_card_list    = r2c_base + 56;
const short r2c_update_skill_exp    = r2c_base + 57;	// ���ܾ���
const short r2c_subsystem_auction	= r2c_base + 58;
const short r2c_master_list			= r2c_base + 59;	// ʦ���б�
const short r2c_player_info			= r2c_base + 60;
const short r2c_subsystem_feod		= r2c_base + 61;
const short r2c_event				= r2c_base + 62;
const short r2c_check_question      = r2c_base + 63;    // ����һ��ͼƬ���ͻ��ˣ��ʿͻ�������
const short r2c_wg_data             = r2c_base + 64;
const short r2c_save_circle_rep		= r2c_base + 65;
const short r2c_active_circle_rep	= r2c_base + 66;
const short r2c_killmonster_addrige = r2c_base + 67;	//send reborn tell it zhe user kill monster num and rigelevel
//PZH
const short r2c_be_shared_task = r2c_base + 68;	//����Ҫ���Լ�����ĳ������
const short r2c_accept_shared_task_ret			= r2c_base + 69;//����ĳ�����˹�����Լ�������Ľ��
const short r2c_shared_task_ret			= r2c_base + 70;//�����Լ���ĳ������Ľ��
//
const short r2c_user_rebirth			= r2c_base + 71;	// ����
const short r2c_pickitem_type			= r2c_base + 72;	// ʰȡ��Ʒ,��Ƥ,�Ƿ�ɹ�.ʧ��ԭ��
const short r2c_pickitem_id				= r2c_base + 73;	// ʰȡ��Ʒ�Ƿ�ɹ�.ʧ��ԭ��
const short r2c_remove_actor			= r2c_base + 74;	// ɾ��һ������

const short r2c_subsystem_linesys		= r2c_base + 75;	//����ϵͳ��Ϣ
//Tian
const short r2c_Hiding                  = r2c_base + 76;    //����
const short r2c_Lurk                    = r2c_base + 77;    //�ݵ�
const short r2c_start_trigger			= r2c_base + 78;	// ������������
const short r2c_gm_showid				= r2c_base + 79;	// GM��ʾ����ID
const short r2c_enter_dungeon			= r2c_base + 80;	// �Ƿ���븱��
const short r2c_trigger_close			= r2c_base + 81;	// ���¸����Ļ���״̬

const short r2c_start_process_bar		= r2c_base + 82;	// ����ʹ��������ߵĽ�����
const short r2c_refresh_equip_wear		= r2c_base + 83;	// ˢ����������װ�����;�
const short r2c_chat_NewS               = r2c_base + 84;   //ϵͳ����

const short r2c_task_can_finish         = r2c_base + 85;    //�����ĳ����
const short r2c_show_message_dialog     = r2c_base + 86;    //��ʾ�ű��Ի���

const short r2c_do_command				= r2c_base + 87;    // ��ɵ�ǰ����
const short r2c_pk_drop_item			= r2c_base + 88;    // PKʱ������Ʒ

const short r2c_Guide					= r2c_base + 89;	//��������֪�ͻ��˿�ʼĳ����ָ��

const short r2c_SynOnlineTime			= r2c_base + 90;	//ͬ���û�����ʱ��

const short r2c_updata_npc_color		= r2c_base + 91;	//����NPC������ɫ��ָ���ͻ���

const short r2c_ctrl_pet				= r2c_base + 92;	// ���Ƴ���
const short r2c_fuse_pet				= r2c_base + 93;	// �ں�,����,��������
const short r2c_Chang_Body				= r2c_base +94;		// ����
const short r2c_update_DeBuf			= r2c_base +95;		// ����DEBUF��Ϣ

const short r2c_update_npc_animation    = r2c_base +96;		// ����NPCԭ�ز��Ŷ�����Ϣ
const short r2c_catch_pet				= r2c_base +97;		// ��׽����
const short r2c_display                 = r2c_base +98;	    // ��ʾ�� ��Ѫ��
const short r2c_rebirth_user            = r2c_base +99;     // ҽ������
const short r2c_Skill_Series            = r2c_base +100;     // ��ǿ����ϵ��

const short r2c_show_timer				= r2c_base +101;	//�ͻ�����ʾ����ʱ��Ϣ
const short r2c_show_npc_info			= r2c_base +102;	//�ͻ�����ʾ��ǰ����ָ�����ݣ������ɷ�������ȫ����
const short r2c_show_schedule			= r2c_base +103;	//�ͻ�����ʾ1����������������ָ��ʱ��
const short r2c_random_function			= r2c_base +104;	//�ͻ�����ʾ1ת�̣����4����תѡ���ת������ݲ���ֵ������Ϣ֪ͨ������
const short r2c_Ultimate_Change         = r2c_base +105;    //��������
const short r2c_Skill_ChangeBody        = r2c_base +106;    //�����ܴ���
const short r2c_CallNpc_Count           = r2c_base +107;    //���߿ͻ��������ٻ�NPC����
const short r2c_refresh_wResumeTime     = r2c_base +108;    //���¼���CDʱ��

const short r2c_pet_linkup				= r2c_base +109;    //���︽��

/* client send to region */
const short c2r_base					= 1000;

const short c2r_update_model			= c2r_base + 1;
const short c2r_attack					= c2r_base + 2;
const short c2r_default_trigger			= c2r_base + 3;
const short c2r_subsystem_team			= c2r_base + 4;
const short c2r_login					= c2r_base + 5;
const short c2r_update_position			= c2r_base + 6;
const short c2r_p_change_model			= c2r_base + 7;
const short c2r_drop_item				= c2r_base + 8;
const short c2r_subsystem_union			= c2r_base + 9;
const short c2r_quit_game			    = c2r_base + 10;
const short c2r_finish_fb_task			= c2r_base + 11;
const short c2r_leave_fb				= c2r_base + 12;
const short c2r_trigger					= c2r_base + 13;
const short c2r_ride					= c2r_base + 14;
const short c2r_select_char				= c2r_base + 15;
const short c2r_master_list				= c2r_base + 16;	//ʦ���б�
const short c2r_system_auction			= c2r_base + 17;
const short c2r_consign_card			= c2r_base + 18;
const short c2r_p_change_item			= c2r_base + 19;
const short c2r_add_attr_point			= c2r_base + 20;


const short c2r_subsystem_item			= c2r_base + 21;
const short c2r_request_card_list   	= c2r_base + 22;
const short c2r_enter					= c2r_base + 23;
const short c2r_mw_add_ele          	= c2r_base + 24; // ��������
const short c2r_print_my_data			= c2r_base + 25; // ��server�˴�ӡ������ݣ����ڵ���
const short c2r_subsystem_feod			= c2r_base + 26;
const short c2r_exp_card            	= c2r_base + 27;
const short c2r_study_skill         	= c2r_base + 28;
const short c2r_report_info         	= c2r_base + 29;
const short c2r_get_item				= c2r_base + 30;
const short c2r_play_pose           	= c2r_base + 31;
const short c2r_teleport            	= c2r_base + 32;
const short c2r_cancel_task         	= c2r_base + 33;
const short c2r_query_player_info		= c2r_base + 34;
const short c2r_get_model_info			= c2r_base + 35;
const short c2r_herolist_query			= c2r_base + 36;
const short c2r_shortcut				= c2r_base + 37;
const short c2r_mw_add_mp           	= c2r_base + 38; // ����ע��mp
const short c2r_enter_fb            	= c2r_base + 39;
const short c2r_charge_card				= c2r_base + 40;
const short c2r_master_prentice			= c2r_base + 41; // ʦͽϵͳ
const short c2r_request_clear_skill		= c2r_base + 42;
const short c2r_interactive_npc			= c2r_base + 43;
const short c2r_recover_pet				= c2r_base + 44;
const short c2r_query_task_overhead		= c2r_base + 45; // ѯ��NPCͷ�ϵ������־
const short c2r_warfield_leave			= c2r_base + 46;
const short c2r_subsystem_mail			= c2r_base + 47;
const short c2r_save_config				= c2r_base + 48; // �ͻ������õĴ���
const short c2r_set_pet_info			= c2r_base + 49;
const short c2r_chat					= c2r_base + 50;
const short c2r_buy_card			    = c2r_base + 51;
const short c2r_warfield_view			= c2r_base + 52;
const short c2r_change_equip			= c2r_base + 53;
const short c2r_warfield_enter			= c2r_base + 54;
const short c2r_move_item				= c2r_base + 55;
const short c2r_confirm_clear_skill		= c2r_base + 56;
//------------add start by tony 05.06.06-------------------------------------//
const short c2r_subsystem_castlewar		= c2r_base + 57;
//------------add end   by tony 05.06.06-------------------------------------//
const short c2r_confirm_switch_region	= c2r_base + 58;
const short c2r_start					= c2r_base + 59; // ��ʼ����, �ڷ����������֮ǰ����c2r_update_position�ᵼ�¶Ͽ�����
const short c2r_get_next_gift			= c2r_base + 60; // ȡ����һ������
const short c2r_check_answer			= c2r_base + 61;  // �ش������������
const short c2r_wg_data			        = c2r_base + 62;
const short c2r_save_circle				= c2r_base + 63;
const short c2r_active_circle			= c2r_base + 64;
const short c2r_rage_burst				= c2r_base + 65;
const short c2r_send_message			= c2r_base + 66;
//PZH
const short c2r_shared_task				= c2r_base + 67;//�����Լ���ĳ������
const short c2r_accept_shared_task		= c2r_base + 68;//����ĳ�����˹�����Լ�������
//
const short c2r_user_rebirth			= c2r_base + 69;	// ����
const short c2r_pickitem_type			= c2r_base + 70;	// ʰȡ��Ʒ,��Ƥ
const short c2r_pickitem_id				= c2r_base + 71;	// ʰȡ��Ʒ��ID
const short c2r_subsystem_linesys		= c2r_base + 72;	//����ϵͳ��Ϣ
const short c2r_close_pickwindow		= c2r_base + 73;	// �ر�ʰȡ����
const short c2r_start_trigger			= c2r_base + 74;	// ������������
const short c2r_stop_getskin			= c2r_base + 75;	// �жϰ�Ƥ
const short c2r_upload_taskid           = c2r_base + 76;    //�ϴ�����ID
const short c2r_study_life_skill		= c2r_base + 77;    // ѧϰ�����
const short c2r_delete_life_skill       = c2r_base + 78;    // ɾ�������
const short c2r_upload_finish_taskid    = c2r_base + 79;    //�ϴ����������ID
const short c2r_task_track              = c2r_base + 80;    //׷��ĳ����
const short c2r_task_canecl_track       = c2r_base + 81;    //ȡ��׷��ĳ����
const short c2r_message_dialog_sure     = c2r_base + 82;    //�ϴ�����ű�����
const short c2r_repair_item			    = c2r_base + 83;    //װ������
const short c2r_pk_pick_item			= c2r_base + 84;    // PKʱ������Ʒ
const short c2r_Guide					= c2r_base + 85;	//�ͻ��˸�֪��������ʼĳ����ָ��
const short c2r_SynOnlineTime			= c2r_base + 86;	//ͬ���û�����ʱ��
const short c2r_change_pkmode			= c2r_base + 87;	// �ı�PKģʽ
const short c2r_update_item             = c2r_base + 88;    // ��������
const short c2r_ctrl_pet				= c2r_base + 89;	// ���Ƴ���
const short c2r_fuse_pet				= c2r_base + 90;	// �ں�,����,��������
const short c2r_catch_pet				= c2r_base + 91;	// ��׽����
const short c2r_rebirth_user            = c2r_base + 92;	// ���ո���
const short c2r_execute_script			= c2r_base + 93;	//�ͻ�����Ϣ������ָ���û��ű�����
const short c2r_get_alterable           = c2r_base + 94;    //�õ���ҿɸı������
const short c2r_set_refusesetting		= c2r_base + 95;	// ��ҵĽ�������
const short c2r_pet_linkup				= c2r_base + 96;    // ���︽��




// warfield msg
const int msg_warfield_prepare		= 1;	//��ս׼����5����
const int msg_warfield_stop			= 2;	//��ս������5����
const int msg_warfield_kick_timeout	= 3;	//�Ŷ�ʱ��������޳�
const int msg_warfield_enter		= 4;	//�ѽ���ս��
const int msg_warfield_start		= 5;
const int msg_warfield_will_stop	= 6;
const int msg_warfield_leave		= 7;
class CG_CmdPacket;

// update all object data 
const char NET_UPDATE_ALL = 0;
const char NET_INIT_ITEMCONTAINER = 1;
const char NET_UPDATE_ITEM_SHOW   = 2;
const char NET_PLAYER_QUERY_INFO  = 3;

// -------------------------------------------------------------------
// Base data struct
// -------------------------------------------------------------------
const int  SHORT_STRING_LEN = 32;
const int  LONG_STRING_LEN = 128;
const int  SMALL_STRING_LEN = 32;
const int  MAX_CREATE_SKILL_NUM = 8;
const int  MAX_CREATE_ITEM_NUM  = 8;
const int  MAX_CREATE_BODY_NUM  = 8;

const char METIER_ALL     = 0;
const char METIER_WARRIOR = 1;  //սʿ -> ���� ->��̨
const char METIER_HUNTER  = 2;  //������ -> ����
const char METIER_WIZARD  = 3;  //а��SS -> ��ɽ 
const char METIER_TAOIST  = 4;  //��ʿ -> ʥ�� ->�置

const char PART_HEAD   = 1;
const char PART_BODY   = 2;
const char PART_HAND   = 3;
const char PART_FOOT   = 4;
const char PART_WEAPON = 5; 
const char PART_MAX    = 6;

// -------------------------------------------------------------------
// Model table
// -------------------------------------------------------------------
const char MODEL_DEAD_TYPE_CHANGE		= 1;
const char MODEL_DEAD_TYPE_TRANS_FLY    = 2;
const char MODEL_DEAD_TYPE_TRANS_SINK	= 3;
const char MODEL_DEAD_TYPE_TRANS		= 4;
const char MODEL_DEAD_TYPE_SINK			= 5;

bool RS_Load();
void RS_Clear();

struct SItem;

const char NET_CRE_UPDATE_MODEL		            = 1;
const char NET_CRE_UPDATE_ATTR_SIMPLE           = 2;
const char NET_CRE_UPDATE_ATTR_BASE	            = 3;
const char NET_CRE_UPDATE_ATTR_ALL              = 4;
const char NET_CRE_UPDATE_TASK		            = 5;
const char NET_CRE_UPDATE_ATTR_ELEMENT_ARMOR    = 6;
const char NET_CRE_UPDATE_FB_TASK               = 7;
const char NET_CRE_UPDATE_PET_INFO              = 8;


const int  ELEMENT_WATER	= 0;
const int  ELEMENT_FIRE		= 1;
const int  ELEMENT_POISON	= 2;
const int  MAX_ELEMENT_ATTR = 3;

const int  ATTACK_FIRE		= 0;
const int  DEF_FIRE			= 1;
const int  ATTACK_WATER		= 2;
const int  DEF_WATER		= 3;
const int  ATTACK_POISON	= 4;
const int  DEF_POISON		= 5;
const int  MAX_ELEMENT		= 6;
const int  MP_TRUMP			= 6;
const int  PNEUMA_TRUMP		= 7;
const int  MAX_TRUMP_RIFINE	= 8;

const char ELEMENT_AUTOPFM_WATER  = 1;
const char ELEMENT_AUTOPFM_FIRE   = 2;
const char ELEMENT_AUTOPFM_ICE    = 3;
const char ELEMENT_AUTOPFM_POISON = 4;

const int USE_SKILL_OK     = 0;
const int USE_SKILL_FAILED = 1;
const int USE_SKILL_MISS   = 2;

//д�����ܵ�ID

const int LIFE_CHAINS = 1515; //��������
const int SUPER_TREATMAEN_HP = 8084; //����ĳ�������
const int SUPER_TREATMAEN_MP = 8088; //����ĳ�������
const int SUIDSKILLLANG   = 184; //���Ǽ��ܵ�SUBID

//д����EFFID
const int PKPROTECT = 10004; //OK������EFF���ID
const int APPOSEBODY = 10101; //���︽��EFF����ID
/*
network update flow

update creature info
	server: logic code->cre serialize->net command
	client: net command->cre serialize->graph update

update item packet
	server: logic code->update type->item serialize->net command
	client: net command->type check->item serialize->graph update

*/
const char MAX_ATTR = 5;
const char ATTR_STR = 0;
const char ATTR_CON = 1;
const char ATTR_DEX = 2;
const char ATTR_INT = 3;
const char ATTR_HIT = 4;

const int ATTR_ADD = 1;
const int ATTR_SUB = 2;

inline int GetRealLevel(int lev)
{
	return lev + 15;
}

class CAttrValue
{
public:
	CAttrValue()
	{
		Reset();
	}
	void  Reset()
	{
		Base  = 0;
		Temp  = 0;
		Rate  = 1.0f;
		
		// ColorfulValue = 0;
		// Finial = 0;
	}
	long GetValue()
	{
		
//#ifdef REGION_SERVER
//		Finial = (Base + Temp)*Rate;
//#endif
//		return Finial;
		return ((Base + Temp) * Rate);

		
	}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Cmd, CAttrValue &p) 
	{
		/*
		p.GetValue();
		return Cmd << p.Base << p.Temp << p.Rate << p.ColorfulValue << p.Finial;
		*/
		return Cmd << p.Base << p.Temp << p.Rate;
	}
	void SerializeDelta(CG_CmdPacket &cmd,CAttrValue &old)
	{
		// operator == already called
		/*
		GetValue();
		old.GetValue();
		SLZ_DELTA(Finial);
		SLZ_DELTA(Rate);
		*/
		SLZ_DELTA(Base);
		SLZ_DELTA(Temp);
		SLZ_DELTA(Rate);
		// SLZ_DELTA(ColorfulValue);
	}
	/*
	friend bool operator==(const CAttrValue &old const CAttrValue &old)
	{
		return GetValue() == old.GetValue();
		// return Base==old.Base && Temp==old.Temp && Rate==old.Base 
		//	&& ColorfulValue==old.ColorfulValue;
	}
	*/

	long  Base;				
	long  Temp;				// item,skill 
	float Rate;

	char  ColorfulValue;	// �����ڿͻ�����ʾ��ɫ��

private:
	// long Finial;			// finial value
};

const float ARMOR_SCALE			= 35.0f; //����ϵ��
const float ELE_ARMOR_SCALE     = 5.0f;
const float ATT_SPEED_SCALE		= 100.0f;
const int   MAX_FB_LEVEL        = 5;
const int   MAX_TRIGGER_OBJ     = 20;

// reason for save and load
const long  REASON_NORMAL        = 0;
const long  REASON_SWITCH_REGION = 1;

struct IHpMpChange
{
	virtual void OnHpChange(int nNewHp) = 0;
	virtual void OnMpChange(int nNewHp) = 0;
	virtual void OnWarScoresChange(float fOldWarScores, float fNewWarScores) = 0;
};
struct SCreature
{
public:
	SCreature();
    
	/*
	const SCreature& operator=(const SCreature &old)
	{
		*this = old;
	}
	*/

	// model info
	short ModelId;
	bool  ComplexModel;
	short HeadModelId;
	std::string huiyuantime;//tim.yang  ��Աʱ��

	bool	bAdult;			//�Ƿ����	ac.ma

	// complex model begin
	short Cloth;
	char  ClothLev;

	short ArmGuard;
	char  ArmGuardLev;

	short Shoes;
	char  ShoesLev;

	short Weapon;
	char  WeaponLev;
	// complex model end
	long  actorID;

	CAttrValue MaxHp;
	CAttrValue MaxMp;
	CAttrValue MaxEnP;      //�������ֵ
	CAttrValue Attack;      //����
	CAttrValue Dodge;       //����
	CAttrValue DamageMin;   //��С�˺�
	CAttrValue DamageMax;	//����˺�
	CAttrValue Armor;       //����
	CAttrValue DamageAbsorb;//�˺�
	CAttrValue AttSpeed; 	// �����ٶ�
	CAttrValue MovSpeed;	// �ƶ��ٶ�
	CAttrValue AttackRange; //��������
	CAttrValue CriticalHit; // ��ɱ��
	CAttrValue ReCriticalHit;// ����ɱfansom���
	CAttrValue ReSkillCritical;// ������fansom���
	CAttrValue ReFaint;// ���λ�fansom���
	CAttrValue ReParry;// ��fansom���

	/*Tianh 09.12.15*������������*/
	CAttrValue RestoreHp; //ÿ3��ظ���Ѫ������
	CAttrValue RestoreMp; //ÿ3��ظ���ħ��������
	CAttrValue RestoreEnP; //ÿ10��ظ�����������


	float      DamageRate;
	float      PhyDmgAbsorbRate;	// ���հٷֱ��˺�
	//float     AbsorbMpRate;		// ���հٷֱȵ�mp

	//�������� begin
	std::string Name;   	// ����
	char  Metier;			// ְҵ
	char  MetierLev;    	// ְҵ�ȼ�
	BYTE  Lev;				// �ȼ�
    char  Faction;      	// ��Ӫ      !!!Ӧ���ǰ��,country��������!!!
	DWORD Exp;				// ��ǰ����ֵ
	long  AttrPoint; 		// ���Ե�
	long  SkillPoint;		// ���ܵ�
	short SkillClearCnt;	// ����ϴ�����������Ҫͬ����client����Ϊ�е�����UI

	//----Tianh---2009.11.20---
	long         Cul;            //��Ϊ��ֵ
	long         CulExp;         //��Ϊ-������ʣ��ֵ
	long         Soul;           //����
	long         Contribute;     //��������ṱ�ף�1
	long         Point;          //����,��ɻ��õ�1
	long         Record;         //����������ս���1
	long         UnionRecord;    //�ﹱ--����ڰ�ս�л��1
	long         JobRecord;      //�Ź�����ɰ��������õ�1
	long         BornTag;        //12��Ф
	long         Sex;            //��ɫ�Ա� 0���� 1��Ů
	long         GradeLev;       //��Ϊ�ȼ�
	long         Vigour;         //Ԫ��ֵ
	long         VigourPoint;    //Ԫ���� 
	long         RelivePostion;  //�󶨵ĸ������� 
	//unsigned int Credit;        //����ֵ  ---- ������ԭ��������ֵ������������
	long         PlayerOnlineTime;//������ߵ��ۻ�ʱ��  ��5Сʱ����
	int		     HeadImageID;     //����ͷ��
	long         WarScores;       //ս������
	// modify to long
	long         EnP; //����--�����������ޣ����ڼ��ܵ���������

	//��������
	long Xianyu;
	long Guanfu;
	long Manzu;
	long Yishi;
	long Yijun;
	long Lianmeng;
	//�ȼ�
	long XianyuLev;
	long GuanfuLev;
	long ManzuLev;
	long YishiLev;
	long YijunLev;
	long LianmengLev;

	//-------------------------
    long  Exploit;          // ս��
	long  Pk;           	// ɱ��ֵ
	long  Credit;       	// //����ֵ--����ֵ--��������1
	CAttrValue Con;	    	// ����
	CAttrValue Str;			// ����
	CAttrValue Dex;			// ����
	CAttrValue Int;			// ����
	CAttrValue Hit;			// ��׼
	// �������� end

	CAttrValue ConPower;	// ����ǿ��
	CAttrValue StrPower;	// ����ǿ��
	CAttrValue DexPower;	// ��ǿ��
	CAttrValue IntPower;	// �۸�ǿ��
	CAttrValue HitPower;	// ��׼ǿ��
	
	void SerializeToDB(long reason, CG_CmdPacket &cmd);

	CAttrValue ElementDamage[MAX_ELEMENT_ATTR]; // Ԫ���˺�
	CAttrValue ElementArmor[MAX_ELEMENT_ATTR];  // Ԫ�ؿ���
	CAttrValue ElementAbsorb[MAX_ELEMENT_ATTR]; // Ԫ���˺�����

	CAttrValue AbsorbHp;						   // ��Ѫ
    CAttrValue AbsorbHpByRate;                     // ��һ������(5%)��Ѫ
	CAttrValue AbsorbMp;					  	   // ��ħ��
    CAttrValue AbsorbMpRate;                       // ���ٷֱ���ħ��
    CAttrValue RateGold;					  	   // ��ý�Ǯ����	
	CAttrValue RateExp;					  		   // ��þ��鱶��

	bool CheckSkillDelta(SCreature &old)
	{
		if(Metier != old.Metier) return true;
		if(MetierLev != old.MetierLev) return true;
		if(Lev != old.Lev) return true;
		if(SkillPoint != old.SkillPoint) return true;

		return false;
	}

	void CheckHp() { 
		if(Hp > GetMaxHp()) Hp = GetMaxHp();
		else if(Hp < 0) Hp = 0;
	}
	void CheckMp() { 
		if(Mp > GetMaxMp()) Mp = GetMaxMp(); 
		else if(Mp < 0) Mp = 0;
	}
	//------------------Tianh-----------------------------
	void CheckEnP(){
		if (EnP > GetMaxEnP())   EnP = GetMaxEnP();
		else if (EnP < 0)     EnP = 0;
	}
	void CheckShieldHp(){
		if (ShieldHp > GetShieldHp()) ShieldHp = GetShieldHp();
		else if (ShieldHp < 0) ShieldHp = 0;
	}
	void CheckShieldMpR(){
		if (GetShieldMpR() > 0) ShieldMpR = GetShieldMpR();
		else if (ShieldMpR < 0) ShieldMpR = 0;
	}
	//------------------Tianh-----------------------------
	void  SetHp(int v)      { Hp = v; CheckHp(); }
	void  SetMp(int v)      { Mp = v; CheckMp(); }
	void  SetEnP(long v)     { EnP = v; CheckEnP();}
	void  SetShieldHp(float v){ ShieldHp = v;CheckShieldHp();}
	void  SetShieldMpR(int v){ ShieldMpR = v;CheckShieldMpR();}

	long  GetHp()           { return Hp; }
	long  GetMp()           { return Mp; }
	long  GetEnP()          { return EnP;}
	float GetShieldHp()     { return ShieldHp;}
	int   GetShieldMpR()    { return ShieldMpR;}

	void  AddHp(int add)    
	{
		Hp += add; 
		CheckHp(); 
		if (NULL != m_IHpMpChange)
		{
			m_IHpMpChange->OnHpChange(Hp);
		}
	}
	void  AddMp(int add)    
	{ 
		Mp += add;
		CheckMp(); 
		if (NULL != m_IHpMpChange)
		{
			m_IHpMpChange->OnMpChange(Hp);
		}
	}
	void  AddEnp(int add)   { EnP += add; CheckEnP();}
	void  AddShieldHp(float add) { ShieldHp += add;CheckShieldHp();}
	void  AddShieldMpR(int add){ ShieldMpR += add;CheckShieldMpR();}

	long  GetMaxHp()        { return MaxHp.GetValue() * (1+ConPower.GetValue()/1000.0f); }
	long  GetMaxMp()        { return MaxMp.GetValue() * (1+IntPower.GetValue()/1000.0f); }
	long  GetMaxEnP()       { return MaxEnP.GetValue();}
	short GetAttack()       { return Attack.GetValue() * (1+HitPower.GetValue()/1000.0f); }
	short GetDodge()        { return Dodge.GetValue() * (1+DexPower.GetValue()/1000.0f); }
	short GetArmor()        { return Armor.GetValue() * (1+ConPower.GetValue()/1000.0f); }
	short GetDamageMin()	{ return (short)(DamageMin.GetValue() /** DamageRate*/); }
	short GetDamageMax()    { return (short)(DamageMax.GetValue() /** DamageRate*/); }
	//add by Tianh 
	void CheckSoul()
	{
		if (Soul>GetSoulValueMax()) Soul = GetSoulValueMax();
		else if (Soul < 0)  Soul = 0;
	}
	//Tianh 09.12.31 �õ����������ֵ
	long GetSoulValueMax()	{return  500*1*int(GradeLev/5+1)*(GradeLev+1);}
	void SetSoul(int v)	{AddSoul(v - Soul);}
	void AddSoul(int add)	{ Soul += add; CheckSoul(); }

	//4.8 LeiJun ���Խű���������
	void SetPKValue(long v)	{if(v < 0) Pk = 0; else Pk = v;}
	void SetContribute(int v)	{AddContribute(v - Contribute);}
	void SetPoint(int v)	{AddPoint(v - Point);}
	void SetRecord(int v)	{AddRecord(v - Record);}
	void SetUnionRecord(int v)	{AddUnionRecord(v - UnionRecord);}
	void SetJobRecord(int v)	{AddJobRecord(v - JobRecord);}
	void AddContribute(int add)
	{
		Contribute += add;
		if(Contribute < 0)
			Contribute = 0;
	}
	void AddPoint(int add)
	{
		Point += add;
		if(Point < 0)
			Point = 0;
	}
	void AddRecord(int add)
	{
		Record += add;
		if(Record < 0)
			Record = 0;
	}
	void AddUnionRecord(int add)
	{
		UnionRecord += add;
		if(UnionRecord < 0)
			UnionRecord = 0;
	}
	void AddJobRecord(int add)
	{
		JobRecord += add;
		if(JobRecord < 0)
			JobRecord = 0;
	}
	void AddPKValue(long add)
	{
		Pk += add;
		if(Pk < 0)
			Pk = 0;
	}

	//GradeLev
	int GetGradeStage(){return (GradeLev/20+1);}//Tianh ��Ϊ�׶�  ����  ��Ϊ�׶�=int(��Ϊ�ȼ�/20+1) �����
	int GetGradeMaxLev(){return Min(99, Lev+4);}//��ɫ�������Ϊ�ȼ�
	
	bool AddBaseStr(long v,bool ChangeBody = false);
	bool AddBaseCon(long v,bool ChangeBody = false);
	bool AddBaseDex(long v,bool ChangeBody = false);
	bool AddBaseInt(long v,bool ChangeBody = false);
	bool AddBaseHit(long v,bool ChangeBody = false);

	void AddAddedStr(short v);
	void AddAddedCon(short v);
	void AddAddedDex(short v);
	void AddAddedInt(short v);
	void AddAddedHit(short v);

    void AddAddedStrR(float v);
    void AddAddedConR(float v);
    void AddAddedDexR(float v);
    void AddAddedIntR(float v);
    void AddAddedHitR(float v);

	// in ms
	float GetAttackInterval()
	{
		if(AttSpeed.GetValue() == 0) return 300;
		return 1000.0f / (AttSpeed.GetValue()/100.0f);
	}
    void RecomputeAttr(bool ChangeBody = false);
    void AddResist(short v);
    void AddResistR(float v);
	void Serialize(char type,CG_CmdPacket *cmd);

	void AddMove() { AbiMove++; }
	void SubMove() { AbiMove--;	}
	bool IsAbiMove() { return AbiMove == 0; }

	void AddDodge() { AbiDodge++; }
	void SubDodge() { AbiDodge--; }
	bool IsAbiDodge() { return AbiDodge == 0; }

	void AddUseSkill() { AbiUseSkill++; }
	void SubUseSkill() { AbiUseSkill--; }
	bool IsAbiUseSkill() { return AbiUseSkill == 0; }

	void AddNormalAttack() { AbiNormalAttack++; }
	void SubNormalAttack() { AbiNormalAttack--;	}
	bool IsAbiNormalAttack() { return AbiNormalAttack == 0; }

	void AddAbiResist(int added) { AbiStateResist += added; }
	void RemoveAbiResist(int remove) { AbiStateResist -= remove; } 
	int  GetAbiResist() { return AbiStateResist; }

	void AddAbiStateHit(int added) { AbiStateHit += added; }
	void RemoveAbiStateHit(int remove) { AbiStateHit -= remove; } 
	int  GetAbiStateHit() { return AbiStateHit; }

	void AddAbiBeAttack(int added) { AbiBeAttack += added; }
	void RemoveAbiBeAttack(int remove) { AbiBeAttack -= remove; } 
	int  IsAbiBeAttack() { return AbiBeAttack; } // ����0��ʾ���Ա������������ܱ�����

	int Hate;				// ����ĳ�޶�
	int HateRate;			// ��ޱ���

	//Tianh
	int CriticalDamage;     // ��ͨ���������˺�HHHHH
	int CriticalDamageR;    // ��ͨ�����������ʣ����Ǳ����˺�����
	int SkillCriRate;       // ���ܻ���������
	int SkillCriDamage;     // ���ܱ����˺��ӳ�
	int SkillCriDamageRate; // ���ܱ����˺����� 
	int AbsorptionHpR;      // �ܵ����˺�ת����HP��

	int nLoginNowTime;   //Tianh   ���߿�ʼ��ʱ

	bool  IsShieldMpR;    //�置�Ƿ������ܵ�״̬��

	float GetSkillCriRate(int enemyLev,float ReSkillCr=0.0);
	void  SetIHpMpChange(IHpMpChange* pIHpMpChange){m_IHpMpChange = pIHpMpChange;}



	void SerializeDelta(CG_CmdPacket &cmd,SCreature &old)
	{
		// value for client UI display
		SLZ_DELTA(Name);

		MaxHp.SerializeDelta(cmd,old.MaxHp);
		MaxMp.SerializeDelta(cmd,old.MaxMp);
		MaxEnP.SerializeDelta(cmd,old.MaxEnP);
		Attack.SerializeDelta(cmd,old.Attack);
		Dodge.SerializeDelta(cmd,old.Dodge);
		DamageMin.SerializeDelta(cmd,old.DamageMin);
		DamageMax.SerializeDelta(cmd,old.DamageMax);
		Armor.SerializeDelta(cmd,old.Armor);
		AttSpeed.SerializeDelta(cmd,old.AttSpeed);
		MovSpeed.SerializeDelta(cmd,old.MovSpeed);
		ReCriticalHit.SerializeDelta(cmd,old.ReCriticalHit);
		ReSkillCritical.SerializeDelta(cmd,old.ReSkillCritical);
		ReFaint.SerializeDelta(cmd,old.ReFaint);
		ReParry.SerializeDelta(cmd,old.ReParry);

		AttackRange.SerializeDelta(cmd,old.AttackRange);
		Con.SerializeDelta(cmd,old.Con);
		Str.SerializeDelta(cmd,old.Str);
		Dex.SerializeDelta(cmd,old.Dex);
		Int.SerializeDelta(cmd,old.Int);
		Hit.SerializeDelta(cmd,old.Hit);

		ConPower.SerializeDelta(cmd,old.ConPower);
		StrPower.SerializeDelta(cmd,old.StrPower);
		DexPower.SerializeDelta(cmd,old.DexPower);
		IntPower.SerializeDelta(cmd,old.IntPower);
		HitPower.SerializeDelta(cmd,old.HitPower);

		SLZ_DELTA(Xianyu);
		SLZ_DELTA(Guanfu);
		SLZ_DELTA(Manzu);
		SLZ_DELTA(Yishi);
		SLZ_DELTA(Yijun);
		SLZ_DELTA(Lianmeng);
		SLZ_DELTA(XianyuLev);
		SLZ_DELTA(GuanfuLev);
		SLZ_DELTA(ManzuLev);
		SLZ_DELTA(YishiLev);
		SLZ_DELTA(YijunLev);
		SLZ_DELTA(LianmengLev);

		SLZ_DELTA(Exploit);
		SLZ_DELTA(Pk);	
		SLZ_DELTA(Credit);	
		SLZ_DELTA(Metier);		
		SLZ_DELTA(MetierLev);    
		SLZ_DELTA(Lev);			
		SLZ_DELTA(Faction);      
		SLZ_DELTA(Exp);		
		SLZ_DELTA(AttrPoint); 	
		SLZ_DELTA(SkillPoint);
		SLZ_DELTA(actorID);	
		SLZ_DELTA(EnP);
		// SLZ_DELTA(SkillClearCnt);	
		SLZ_DELTA(WarScores);
		SLZ_DELTA(DamageRate);
		SLZ_DELTA(Hp);	
		SLZ_DELTA(Mp);	
		SLZ_DELTA(Soul);
		SLZ_DELTA(Vigour);
		SLZ_DELTA(VigourPoint);
		SLZ_DELTA(Sex);
		SLZ_DELTA(Cul);
		SLZ_DELTA(CulExp);
		SLZ_DELTA(GradeLev);
		SLZ_DELTA(HeadImageID);
		SLZ_DELTA(HeadImageID);

		SLZ_DELTA(SkillCriRate);
		CriticalHit.SerializeDelta(cmd,old.CriticalHit);
		ReCriticalHit.SerializeDelta(cmd,old.ReCriticalHit);
		ReSkillCritical.SerializeDelta(cmd,old.ReSkillCritical);

		SLZ_DELTA(Contribute);
		SLZ_DELTA(Point);
		SLZ_DELTA(Record);
		SLZ_DELTA(UnionRecord);
		SLZ_DELTA(JobRecord);
		SLZ_DELTA(BornTag);
		SLZ_DELTA(RelivePostion);
		SLZ_DELTA(HeadImageID);

		for(int i=0; i<MAX_ELEMENT_ATTR; i++)
		{
			ElementDamage[i].SerializeDelta(cmd,old.ElementDamage[i]);
			ElementArmor[i].SerializeDelta(cmd,old.ElementArmor[i]);
		}

		SLZ_DELTA(AbiMove);
		SLZ_DELTA(AbiDodge);
		SLZ_DELTA(AbiUseSkill);
		SLZ_DELTA(AbiNormalAttack);
		SLZ_DELTA(SkillCriRate);
	}

public:
	long Hp;
	long Mp;

private: 
	//-------Tianh*09.12.15----------------//
	int AbiMove;               // �ƶ�
	int AbiDodge;              // ���
	int AbiUseSkill;           // ʹ�ü���
	int AbiNormalAttack;       // ��ͨ����
	int AbiStateResist;        // ״̬�ֿ�
	int AbiStateHit;           // ״̬���ܵ����мӳ�
    int AbiBeAttack;           // ������
	float ShieldHp;            // Tianh �����˺����ֵ--�����������ն�(��������ʵ���Ǹ���Ѫ�Ķ�)
	int ShieldMpR;             // ʥ��ħ������������������

	IHpMpChange* m_IHpMpChange;
};

//
// exp table resource
//
const int  MAX_LEVEL  = 90;
const char SEX_MALE   = 0;
const char SEX_FEMALE = 1;

class CRS_Exp
{
public:
	CRS_Exp();
	virtual ~CRS_Exp();
	
	bool  Load(const char *file);
	ULONG GetNextLevExp(long lev);
	ULONG GetNextLevCulExp(long lev);
	ULONG GetLevCulTable(long lev);

private:
	std::vector<ULONG> m_expTable;
	std::vector<ULONG> m_CulexpTable;
	std::vector<ULONG> m_CulTable;
};

//Tianh ��������
class CRS_Popularity 
{
public:
	CRS_Popularity();
	virtual ~CRS_Popularity();

	bool  Load(const char *file);

	ULONG GetNextXainyuExp(long lev);
	ULONG GetNextGuanfuExp(long lev);
	ULONG GetNextManzuExp(long lev);
	ULONG GetNextYiShiExp(long lev);
	ULONG GetNextYijunExp(long lev);
	ULONG GetNextLianmengExp(long lev);

private:
	std::vector<ULONG> m_Xainyu;
	std::vector<ULONG> m_Guanfu;
	std::vector<ULONG> m_Manzu;
	std::vector<ULONG> m_YiShi;
	std::vector<ULONG> m_Yijun;
	std::vector<ULONG> m_Lianmeng;
};


struct SUserActor
{
	SUserActor() {
		Id = 0;
		for(int i=0; i<MAX_CREATE_SKILL_NUM; i++)
			Skill[i] = 0;
		for(int i=0; i<MAX_CREATE_ITEM_NUM; i++)
			Item[i] = 0;
		for (int i=0; i<MAX_CREATE_BODY_NUM; i++)
			BodyItem[i] = 0;
	}
	short  Id;
	short  ModelId;
	char   Sex;
	char   Age;
	char   Metier;
	char   MetierLev;
	std::string MetierName;
	// bool   bCreate;
	bool   bZCreate;
	bool   bSCreate;
	bool   bZlCreate;
	char   Level;
	short  BaseStr;
	short  BaseDex;
	short  BaseHit;
	short  BaseCon;
	short  BaseInt;

	short  BaseAttack;
	short  BaseDamageMin;
	short  BaseDamageMax;
	short  BaseDodge;
	short  BaseArmor;

	long   Skill[MAX_CREATE_SKILL_NUM];
	long   Item[MAX_CREATE_ITEM_NUM];
	long   BodyItem[MAX_CREATE_BODY_NUM];
	std::string Desc;
};

class CRS_UserActor
{
public:
	CRS_UserActor();
	virtual ~CRS_UserActor();

    bool Load(const char *File);
	SUserActor *FindUserActor(char Metier,char MetierLev);
	SUserActor *FindUserActor(int Id);

    int GetUserCount()                          { return (int)m_table.size(); }
    const SUserActor* GetUserByIdx(int iIdx)    { return &m_table[iIdx]; }

private:
	std::vector<SUserActor> m_table;
};

struct SCreModel
{
	SCreModel() {
		ModelScale[0] = 1;
		ModelScale[1] = 1;
		ModelScale[2] = 1;
	}
	short  Id;
	std::string Act;
	bool   bComplex;
	std::string HeadSkin;
	std::string BodySkin;
	std::string HandSkin;
	std::string FootSkin;
	std::string CaestusSkin;
	char   DeadType;
	std::string DeadAct;
	std::string BornAct;
	std::string ReplaceMtl;
	float  ModelScale[3];
	float  ModelRadius;

	float  AttackRange;
	float  MoveSpeed;
	float  AttackSpeed;

	std::string LinkEffect;
	std::string LinkPoint;

	std::string AddedSkin;
	std::string RidePoseFix;
};

class CRS_CreModel
{
public:
	CRS_CreModel();
	virtual ~CRS_CreModel();

	bool Load(const char *File);
	SCreModel *FindCreModel(int Id);
	
private:
	std::vector<SCreModel> m_table;
};


struct SHeadModel
{
	short			actorModelID;	// ��Ӧ������ģ��ID
	short			id;
	std::string		name;
	std::string		skin;

	SHeadModel()
	{
		actorModelID = 0;
		id = 0;
		name = "";
		skin = "";
	}
};

class CRS_HeadModel
{
public:
	CRS_HeadModel();
	virtual ~CRS_HeadModel();

	bool Load(const char *File);
	bool GetHeadModel(short ModelID, vector<SHeadModel>& models);
	bool GetHeadModelByID(short id, SHeadModel& model);

private:
	std::vector<SHeadModel> m_table;
};

//����2Dͼ����
struct SCharImage
{
	short			actorModelID;	// ��Ӧ������ģ��ID
	short			id;
	std::string		imageCreate;      //����ͼ��
	std::string     imageGame;        //��Ϸͼ��
	std::string     imageTeam;        //���ͼ��
	bool            bValid;

	SCharImage()
	{
		actorModelID = 0;
		id = 0;
		imageCreate = "";
		imageGame = "";
		imageTeam = "";
		bValid = false;
	}
};

class CRS_CharImage
{
public:
	CRS_CharImage();
	virtual ~CRS_CharImage();

	bool Load(const char *File);
	bool GetCharImage(short ModelID, vector<SCharImage>& models);
private:
	std::vector<SCharImage> m_table;
};

const int  MAX_NPC_DROP_ITEM        = 200; //�������
const long NPC_FUNCTION_BUY			= 1;
const long NPC_FUNCTION_SELL		= 2;
const long NPC_FUNCTION_LEARN_SKILL = 4;
const long NPC_FUNCTION_RECOVER_PET = 8;
const long NPC_FUNCTION_BANK		= 16;
const long NPC_FUNCTION_SMELT       = 32;

const long NPC_MAX_SKILL            = 4;
const long NPC_MAX_TASK_ITEM        = 10;
const long NPC_MAX_CALL_SPIRIT      = 10;

/*
const char *NPC_TYPE_MONSTER        = "1";
const char *NPC_TYPE_SYSTEM         = "2";
*/
//-----------------------add start by tony 05.05.08--------------------------//
typedef struct
{
	long	x;	//������
	long	y;	//������
}SPathNode;

const int MAX_TASK_PARAM_SIZE = 19;
struct STaskKey
{
	std::string key;
	std::string param[MAX_TASK_PARAM_SIZE];
};

struct SEventCommand
{
	std::string event;
	STaskKey command;
};

//-----------------------add end by tony 05.05.08----------------------------//
struct SNpc
{
	SNpc() {
		for(int i=0; i<MAX_ELEMENT_ATTR; i++)
		{
			ElementDamage[i] = 0;
			ElementArmor[i] = 0;
		}
	}

	short  Id;
	std::string TypeId;
	char   Faction;
	char   Type;
    std::string Name;
	std::string mOverHead;         //����ͷ����ʾ
	std::string TypeParam;
	std::string AtMiddleMap;
	long		Pos[3];
	short  ModelId;
	std::string Script;
	short  sSkills[NPC_MAX_SKILL];
	float  SkillAttackRate[NPC_MAX_SKILL];

	float  PatrolRange;
	float  ViewRange;
	float  ValidRange;
	float  AssistRange;
	long   CombatType;
	long   BoundType;
	long   PartyMonster;

	float  AttackRange;
	float  AttackSpeed;
	float  MoveSpeed;
	
	char  Level;
	long  Hp;
	long  Mp;
	long  Attack;
	long  Damage;
	long  Dodge;
	long  Armor;

	long ElementDamage[MAX_ELEMENT_ATTR];
	long ElementArmor[MAX_ELEMENT_ATTR];
        
	vector<long> Function;
	
	// reward
	long  Exp;
	long  Credit;
	long  lSP;
	long  Gold;
	float GoldRate;
	long   Item[MAX_NPC_DROP_ITEM];
	double ItemDropRate[MAX_NPC_DROP_ITEM];
	long   ItemNum;
	long   ItemBindType;

	double ItemGreenRate;
	double ItemBlueRate;
	double ItemGoldRate;
	
	long   RandItemNum;
	long   RandItem[MAX_NPC_DROP_ITEM];
	double RandItemDropRate[MAX_NPC_DROP_ITEM];

	long   FbTask;
	long   nTaskNum;
	long  TaskId[NPC_MAX_TASK_ITEM];
	long  TaskItemId[NPC_MAX_TASK_ITEM];
	long  TaskItemLimit[NPC_MAX_TASK_ITEM];
	float TaskItemDropRate[NPC_MAX_TASK_ITEM];
	std::string SellItems;
    std::string SellRareItems;
	std::string DefaultTalk;

	vector<long> GetTaskId;			// ���NPC���Խӵ�������
	vector<long> FinishTaskId;		// ���NPC������ɵ�����
	long RandomTask;				// �Ƿ��������NPC
	vector<long> RandomTaskRate;    // �������
	vector<SEventCommand> EventCmd; // �¼�����

	bool    isPrompt;   //���������Ƿ���ϵͳ����
	long    Dif;
	long	Boss;
	std::string  HeadPic;
	std::string  taskPic;
	
	std::string AIType;
	DWORD		AILevel;
	DWORD		ProtecterID;
	DWORD		ConjureTarID;
	std::string Home;
//-----------------------add start by tony 05.05.08--------------------------//
	std::list<SPathNode> PatrolPath;	//Ѳ��·��
//-----------------------add end by tony 05.05.08----------------------------//
//-----------------------add start by tony 05.05.23--------------------------//
	int Interval;	//��������
//-----------------------add end by tony 05.05.23----------------------------//

	//PZH
	char cTaskType;//����������ͣ�0������û�κι�ϵ��1����������б������2�������������2����ܻ��������
	unsigned long dwSkinID;
	unsigned long dwDiwKeetTime;//�������ʱ��
	unsigned long dwGetSkinTime;//��Ƥ����ʱ��
	//

	// add by zzh
	std::string szDeadEffect;
	std::string szDeadLink;
	long  CallSpiritID[NPC_MAX_CALL_SPIRIT];
	float CallSpiritRate[NPC_MAX_CALL_SPIRIT];
	int	  CallSpiritNum;
	//

	//4.15 LeiJun
	bool HaveEventCommand(const char *sCommand)
	{
		if(!sCommand)
			return false;
		for(size_t iIndex = 0; iIndex < EventCmd.size(); ++iIndex)
		{
			if(EventCmd[iIndex].event == sCommand)
				return true;
		}
		return false;
	}
	bool AddEventCommand(const char *sEvent, const char *sCommand,
						const char *sParam1 = 0, const char *sParam2 = 0,
						const char *sParam3 = 0, const char *sParam4 = 0)
	{
		if(!sEvent || !sCommand)
			return false;
		if(HaveEventCommand(sEvent))
			return false;

		int iCurIndex = 0;
		SEventCommand newCommand;
		newCommand.event = sEvent;
		newCommand.command.key = sCommand;
		if(sParam1)
			newCommand.command.param[iCurIndex++] = sParam1;
		if(sParam2)
			newCommand.command.param[iCurIndex++] = sParam2;
		if(sParam3)
			newCommand.command.param[iCurIndex++] = sParam3;
		if(sParam4)
			newCommand.command.param[iCurIndex++] = sParam4;

		EventCmd.push_back(newCommand);
		return true;
	}
};

class CRS_Npc
{
public:
	CRS_Npc();
	virtual ~CRS_Npc();

	bool GetEventCommand(const char *str,vector<SEventCommand> &cmdList);
	bool Load(const char *File);
	SNpc *FindNpc(int Id);
	void GetNpcForMiddleMap(vector<vector<SNpc*>*>& vectorSNpc,map<string,int>& stringToInt);
//-----------------------add start by tony 05.05.23--------------------------//
	//int CountNpc() { return (int)m_table.size(); }	//����npc������
	const std::vector<SNpc>& GetNpcTable() { return m_table;}
//-----------------------add end by tony 05.05.23----------------------------//

private:
	std::vector<SNpc> m_table;
};
#include "pet_base.h"

//-----------------------Add LeiJun NPC Random Talk----------------------------//
class CRS_NPCTalkText
{
	typedef	std::map<long, std::vector<std::string> >	RANDTEXT;

public:
	CRS_NPCTalkText();
	~CRS_NPCTalkText();

	bool		Load(const char *File);
	const char*	FindRandomTalkTextByID(int iID);
	const char*	FindRandomSuccorTextByID(int iID);
	const char*	FindRandomOverTextByID(int iID);

private:
	RANDTEXT	m_TalkTexts;
	RANDTEXT	m_SuccorTexts;
	RANDTEXT	m_OverTexts;
};
//-----------------------End LeiJun NPC Random Talk----------------------------//

//-----------------------Add LeiJun Problem----------------------------//
struct ProblemData
{
	static const int OPTION_COUNT = 5;
	std::string sTitle;
	std::string sDescription;
	std::string sOption[OPTION_COUNT];
};
class CRS_Problem
{
	typedef	std::map<long, ProblemData >	PROBLEMLIST;

public:
	CRS_Problem();
	~CRS_Problem();

	bool			Load(const char *File);
	ProblemData*	FindProblemByID(long lProblemID);

private:
	PROBLEMLIST	vProblems;
};
//-----------------------End LeiJun Problem----------------------------//

struct SPet;
class CRS_Pet
{
public:
	CRS_Pet();
	virtual ~CRS_Pet();

	bool Load(const char *File);
	SPet *FindPet(int Id);

private:
	std::vector<SPet> m_table;
};

struct SBaseValue
{
	float FHp;
	float FMp;
	float FAttack;
	float FDamage;
	float FDodge;
	float FArmor;
};

class CRS_BaseValue
{
public:
	CRS_BaseValue();
	virtual ~CRS_BaseValue();

	bool Load(const char *File);
	SBaseValue *Find(int lev);

private:
	std::vector<SBaseValue> m_table;
};


// Task Key
const int TYPE_CHECK  = 0;
const int TYPE_ADD    = 1;
const int TYPE_REMOVE = 2;

const int TASK_FINISH_TYPE_NORMAL = 0;
const int TASK_FINISH_TYPE_SELECT = 1;
const int MAX_TASK_SELECT_NUM     = 20;//PZH ���ݲ߻���Ҫ��ĳ�20��ԭ����5

//------------add start by tony 05.06.16-------------------------//
//-�ܻ�������
const int TASK_CIRCLE_ITEM = 1;
const int TASK_CIRCLE_FINDNPC = 2;
const int TASK_CIRCLE_BATTLE = 3;
//const int TASK_CIRCLE_TOTAL = 10;

extern std::vector<long> g_tcexpRate;
extern std::vector<long> g_tcrndNpcID;
extern std::vector<long> g_tcrndItemID;
extern std::vector<long> g_tcrndMonsterID;
extern int g_tcTotalCircle;
//------------add end   by tony 05.06.16-------------------------//
struct STaskDesc
{
	short id;
	short group;
	std::string name;
	std::string desc;
	vector<STaskKey> taskReq;
	vector<STaskKey> acceptReq;
	bool  access;

	int reqMinLev;
	int reqMaxLev;

	int reqUnion;			            // ����λ��0=�ޣ�1=����

	int metierLimit;
	int countryLimit;
	int repeat;
	int getTimeLimit;					// �����ڶ�ʱ�����ظ��ӵ�����
	int finishTimeLimit;				// ��Ҫ�ڹ涨ʱ������ɵ�����
	int canCancel;						// �Ƿ��ܹ�ȡ��

	int moneyTask;                      // ���������Ѷȣ�0��ʾ���Ǵ�������
	int wenTask;                        // �Ĺ�����
	int wenTaskCnt;						// �Ĺ��������

	//---------add start by tony 06.05.09-------------------------//
	//-reason: ������ܻ�����, ��circleTask!=0ȷ��
	//-circleTask==1: Ѱ��
	//-circleTask==2: ����
	//-circleTask==3: ս��	
	int circleTask;						// �ܻ�����
	//int currCircle;						// ��ǰ����, 50��Ϊ1�ֽ���
	//int currMonsterNum;					// ��ǰ��ɱ����������--����ս��������Ч
	//int itemID;							// Ѱ�ҵ���ID--����Ѱ��������Ч
	//int totalItemNum;					// Ѱ�ҵ�������--����Ѱ��������Ч
	//int npcID;							// Ѱ��npcID--��������������Ч
	//int monsterID;						// ɱ������ID--����ս��������Ч	
	//---------add end   by tony 06.05.09-------------------------//

	vector<STaskKey> flagItem;
	std::string acceptFlag;
	int finishType;
	std::string npc1PreAcceptTalk;
	std::string npc1AfterAcceptTalk;
	std::string npc1InTaskTalk;
	std::string npc2InTaskTalk;
	std::string npc2AfterFinishTalk;
	std::string npc2SelectFailedTalk;

	// std::string selectHtml;
	vector<STaskKey> fixReward;
	float            randomRewardRate;
	vector<STaskKey> randomReward;
	
	float            specRewardRate;
	std::string		 specFlag[MAX_TASK_SELECT_NUM];
	vector<STaskKey> specReq[MAX_TASK_SELECT_NUM];
	vector<STaskKey> specReward[MAX_TASK_SELECT_NUM];

    vector<STaskKey> dynamicText;
	vector<STaskKey> dynamicText2;//��̬��ʾ2
	vector<STaskKey> dynamicItem;
	vector<STaskKey> dynamicNum;

	//PZH
	std::string m_TaskSencenID; //��������
	int m_TaskGroupID; //����������� 
	bool m_bHasMax;//�Ƿ������޸�������
	bool m_bShare;//�Ƿ�ɹ���
	std::string mAcceptDesc;//���������Ϣ
	// gao
	int		m_posTaskBegin[2];
	int		m_posTaskEnd[2];

	vector<STaskKey> selectReward;
	vector<STaskKey> jobReward;//ְҵ����

	bool IsWenTask(){return 1 == wenTask;}
	bool IsGongTask(){return 2 == wenTask;}
	bool IsUnionTask(){return 3 == wenTask;}
	bool IsMouTask(){return 4 == wenTask;}
	bool IsMainTask(){return m_TaskGroupID < 40;}
};

struct SFbTask
{
	int                 id;
	std::string              name;
	int                 fbId;
	int                 visDif;
	int                 taskDif;
	std::string              taskDesc[MAX_FB_LEVEL];
	vector<STaskKey>    taskFinReq[MAX_FB_LEVEL];
	vector<STaskKey>    taskReward[MAX_FB_LEVEL];
};

class CRS_Task
{
public:
	CRS_Task();
	virtual ~CRS_Task();

	bool		Load(const char *File);
	bool        LoadFbTask(const char *file);

	bool        GetTaskKey(const char *str,vector<STaskKey> &key);
	STaskDesc  *FindTask(int Id);
	SFbTask    *FindFbTask(int id);
	void        GetTaskByFb(int fbId,int dif,vector<long> &taskId);	
	void		GetTaskDescForMiddleMap(vector<vector<STaskDesc*>*>& vectorTaskDesc,map<string,int>& stringToInt);
	std::vector<STaskDesc>& GetTaskTable(){ return m_table; }

private:
	std::vector<STaskDesc> m_table;
	std::vector<SFbTask>   m_fbTable;
};

//�޸�����---3-������ܲ���� 4-�������δ���--LxJian
enum ETaskOverHead
{
	TOH_Unknown,
	TOH_CanNotAccept,
	TOH_CanAccept,
	TOH_AcceptAndFinish,
	TOH_AcceptNoFinish
};


//ADD BY Tianh
//��������
struct AtrribPoint
{
	int   Metier;         //ְҵ
	float PlayerLev;      //����ȼ�
	float GradeLev;       //��Ϊ�ȼ�
	float PlayerHP;       //���Ѫֵ
	float PlayerMp;       //���ħ��ֵ
	float PhysicalDamage; //�����˺�
	float PhysicalDefense;//�������
	float FireDamage;     //���˺�
	float FireDefense;    //�����
	float WaterDamage;    //ˮ�˺�
	float WaterDefense;   //ˮ����
	float PoisonDamage;   //���˺�
	float PoisonDefense;  //������
	float Attack;         //����
	float Dodge;          //����
	float DamagesSpeed;   //�����ٶ�
	float MoveSpeed;      //�ƶ��ٶ�
	float DamageDistance; //��������
	float SlayR;          //����һ����
	float SkillCriRate;   //����һ����
	float SkillCriDamage; //����һ���˺��ӳ�
	float ReCriticalHit;  //�ֿ�����һ����
	float ReSkillCritical;//�ֿ�����һ����
	float ReParry;        //��
	float AbsorbHp;       //��Ѫ
	float AbsorbMp;       //����
	float RestoreHp;      //ÿ3��ظ���Ѫ��
	float RestoreMp;      //ÿ3��ظ���ħ����
};

class CRS_AtrribPoint
{
public:
	CRS_AtrribPoint();
	virtual ~CRS_AtrribPoint();

	bool  Load(const char *File);
	AtrribPoint* FindPet(int Metier);

private:
	std::vector<AtrribPoint> m_AtPtable;
};

// ----------------------------------------------------------------------------------
// scene info 
// ----------------------------------------------------------------------------------

struct SSceneInfo
{
	std::string     szSceneFileName;            // ����������
	std::string     szSceneCombatMusic;         // ����ս������
    float           fRebirth1X, fRebirth1Y;     // ������1��������
    float           fRebirth2X, fRebirth2Y;     // ������2��������
	float           fRebirth3X, fRebirth3Y;     // ������3��������
	float           fX_Oppose1, fY_Oppose1;     // ������1���Է���
	float           fX_Oppose2, fY_Oppose2;     // ������2���Է���
	float           fX_Oppose3, fY_Oppose3;     // ������3���Է���
    float           fRebirthRedX1, fRebirthRedY1; // ����������1
    float           fRebirthRedX2, fRebirthRedY2; // ����������2
    float           fRebirthRedX3, fRebirthRedY3; // ����������3
    float           fRebirthRedX4, fRebirthRedY4; // ����������4
    float           fRebirthRedX5, fRebirthRedY5; // ����������5
	/* gao 2010.1.20
	������ͼ�ļ����������
	*/
	std::string		szSceneMiddleMapFileName;	// ������ͼ��������
	std::string		szMiddleMapName;			// ������ͼ����
	int				iMiddleMapHeight;			// ������ͼ�߶ȷ����ϵĵ�ͼ����
	int				iMiddleMapWidth;			// ������ͼ��ȷ����ϵĵ�ͼ����
	int				iMiddleMapHeightOffset;		// ������ͼ�߶ȷ����ϵĵ�ǰblock��ƫ��
	int				iMiddleMapWidthOffset;		// ������ͼ��ȷ����ϵĵ�ǰblock��ƫ��
	int				iMiddleMapGroup;			// ������ͼ�ķ��飬��Ѱ·���
	/* end */
    int             iPKAttr;                    // PK����
	char			sceneFaction;				// ��������Ӫ
};

class CRS_SceneInfo
{
public:
    CRS_SceneInfo();
    virtual ~CRS_SceneInfo();

    bool Load(const char *File);
    SSceneInfo* FindScene(std::string& szBlockName);
	void GetAllMiddleMapName(map<string,SSceneInfo>& lstName);

private:
    HASH_MULTISTR(std::string, SSceneInfo) m_table;

};

// -------------------------------------------------------------------
// Task info manage
// -------------------------------------------------------------------

const int TaskMaxAccept = 3;		// ������ͬʱ�Ӷ�������

struct STaskInfo
{
	STaskInfo()
	{
		Desc = NULL;

		m_currMonsterNum = 0;
		m_finditemID = 0;
		m_findnpcID = 0;
		m_srcNpcID = 0;
		m_killmonsterID = 0;
		dwUseItem = 0;
	}
	
	DWORD Id;
	DWORD IsDoing; //PZH ԭ��ֻ������״̬��1��ʾ��������0��ʾ�������������ָĳ�3״̬��1��ʾ��������3���������������, 4���˹�����������
	DWORD FinishNum;
	DWORD BeginTime;
	DWORD LastFinishTime;
	DWORD DayTimeTaskFinishTime;//tim.yang  �ճ�����  ��¼�ϴ����ʱ��
	//---------add start by tony 06.05.09-------------------------//
	//-reason: ������ܻ�����, ��circleTask!=0ȷ��
	//-circleTask==1: Ѱ��
	//-circleTask==2: ����
	//-circleTask==3: ս��
	//-���½����ܻ�������Ч
	//int m_currCircle;						// ��ǰ����, 50��Ϊ1�ֽ���,�����ڴ洢
	int m_currMonsterNum;					// ��ǰ��ɱ����������--����ս��������Ч
	int m_finditemID;						// Ѱ�ҵ���ID--����Ѱ��������Ч
	int m_findnpcID;						// Ѱ��npcID--��������������Ч
	int m_srcNpcID;							// ��������npcID
	int m_killmonsterID;					// ɱ������ID--����ս��������Ч	

	//int m_tmpcurrCircle;						// ��ǰ����, 50��Ϊ1�ֽ���
	//int m_tmpcurrMonsterNum;					// ��ǰ��ɱ����������--����ս��������Ч
	//int m_tmpfinditemID;						// Ѱ�ҵ���ID--����Ѱ��������Ч
	//int m_tmpfindnpcID;						// Ѱ��npcID--��������������Ч
	//int m_tmpsrcNpcID;							// ��������npcID
	//int m_tmpkillmonsterID;					// ɱ������ID--����ս��������Ч	
	//---------add end   by tony 06.05.09-------------------------//

	STaskDesc *Desc;
	STaskDesc* GetDesc(){return Desc;}
	//PZH
	bool IsTrack()
	{
		bool bRet = false;
		do 
		{
			if (!FIsDoing())
			{
				break;
			}
			unsigned long dwMark = 1 << 31;
			bRet = (dwUseItem & dwMark) != 0;
		} while (false);
		return bRet;
	}
	void SetTrack()
	{
		unsigned long dwMark = 1 << 31;
		dwUseItem = dwUseItem | dwMark;
	}
	void CaneclTrack()
	{
		unsigned long dwMark = 1 << 31;
		dwMark = ~dwMark;
		dwUseItem = dwUseItem & dwMark;
	}
	bool FIsDoing()
	{
		return 1 == IsDoing || 3 == IsDoing;
	}
	void FSetDoing()
	{
		IsDoing = 1;
	}
	bool FIsCanecl()
	{
		return false;//0 == IsDoing;
	}
	void FSetCanecl()
	{
		//IsDoing = 0;
	}
	bool FIsDone()
	{
		return false;//2 == IsDoing;
	}
	void FSetDone()
	{
		//IsDoing = 2;
	}
	void FSetCanFinish()
	{
		IsDoing = 3;
	}
	bool FIsCanFinish()
	{
		return 3 == IsDoing;
	}
	bool FIsShared()
	{
		return 4 == IsDoing;
	}
	void FSetShared()
	{
		IsDoing = 4;
	}
	bool IsUseItem(unsigned char cIndex)
	{
		bool bRet = false;
		unsigned long dwMark = 1 << cIndex;
		bRet = (dwUseItem & dwMark) != 0;
		return bRet;
	}

	bool IsUseItemByID(int nID);

	void SetUseItem(unsigned char cIndex)
	{	
		unsigned long dwMark = 1 << cIndex;
		dwUseItem = dwUseItem | dwMark;
	}

	unsigned long GetCurKillMonsterNum()
	{
		unsigned long dwMark = 0x3f << 4;
		unsigned long dwTemp = dwUseItem & dwMark;
		dwTemp = dwTemp >> 4;
		return dwTemp;
	}
	void SetCurKillMonsterNum(unsigned long dwNum)
	{
		unsigned long dwMark = 0x3f << 4;
		dwMark = ~dwMark;
		unsigned long dwTemp = dwNum << 4;
		unsigned long dwTemp2 = dwUseItem & dwMark;
		dwUseItem = dwTemp2 | dwTemp;
	}
	
	unsigned long dwUseItem;
	//
};

enum EFbTaskState
{
	FBT_UNVALID,
	FBT_UNFINISH,
	FBT_FINISHED
};

struct SFbTaskInfo
{
	int fbId;
	int curDif;
	vector<long> finTask;

	bool IsTaskFinished(int taskId);
	bool FinishTask(int taskId);
	bool CanLevelUp();
	void LevelUp();
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Cmd, SFbTaskInfo &t) 
	{
		Cmd << t.fbId << t.curDif;
		long num = 0;
		if(Cmd.IsWritting())
		{
			num = (long)t.finTask.size();
			Cmd.WriteLong(num);
			for(int i=0; i<num; i++)
				Cmd.WriteLong(t.finTask[i]);
		}
		else
		{
			long tmp = 0;
			Cmd.ReadLong(&num);
			for(int i=0; i<num; i++)
			{
				Cmd.ReadLong(&tmp);
				if(Cmd.IsError()) return Cmd;
				t.finTask.push_back(tmp);
			}
		}
		return Cmd;
	}
};

const int c_g_nMaxGuideTafNum = 3;
enum GuideType
{
	eHitMove,//������������ƶ�
	eKeyMoveFront,//������aswd��w����������������ǰ�ӽǵ�ǰ���ƶ�
	eKeyMoveBack,//����aswd��s����������������ǰ�ӽǵĺ��ƶ�
	eKeyMoveLeft,//����aswd��a����������������ǰ�ӽǵ�����ƶ�
	eKeyMoveRight,//����aswd��d����������������ǰ�ӽǵ��Ҳ��ƶ�
	eViewChange,//����Ҽ���ͼƬ������ס���ſɶ��ӽǽ��е���
	eNPCTalk,//�ƶ���NPCС���������仯Ϊ���Ի�ָ��(ani)��֮�󣬵�����������򿪶Ի�ҳ��
	eNPCTaskTalk,//����������ƣ����������ҳ��
	eNPCAcceptTask,//����ð�ť���ɽ��ܸ�����
	eMoveToNPC,//�������Զ��ƶ�����NPC��
	eShowKeyBoard,//����F10�����Դ򿪼��̲�����ʾ
	eNPCFinishTask,//����ð�ť������ɸ�����
	eAttrButFlash,//�������2��֮�� ��
	eAddAttr,//��Ҵ�����ҳ��֮��
	eFinishAddAttr,//����ȷ����ť��������Ե�����
	eOpenSkillUI,//����״δ򿪼���ҳ��֮��
	eSkillChangePane,//��ҽ���ҳ���л�����5��֮�������һ��ʾ
	eFirstAttackMonster,//����״ι�������
	eEatHP,//����״�hp����50%
	eEatMP,//����״�mp����50%
	eUsePet1,//��ҽ�������11001�����ұ�������ID16000Ϊ����Ʒ
	eUsePet2,//����Ҽ���������Լ��ͼƬ�������ٻ����
	eUsePet3,//��ҵ�ǰ�ٻ�������ĵȼ�����ֵ��Ϣ���ڴ˴���ʾ
	eBuyItem1,//��������11004����Ҵ�npc��ʵ��һ���Ի�ҳ��֮��
	eBuyItem2,//��������Ʒ�����й���
	eBuyItem3,//��ҵ��������֮��
	eFaBao1,//���ܰ�ť��˸
	eFaBao2,//����ð�ť�������ۺϼ���ҳ��
	eFaBao3,//�����ü��ܣ��򿪷�������ҳ��
	eFaBao4,//�����ң��ҵ����ɺ�«
	eFaBao5,//����������Ʒ����������ڷ�������ҳ�浱��
	eFaBao6,//�����˰�ť�����������ȼ�
	eMail1,//�ӵ����ʼ���������
	//eMail2,//����������������������ʼ���
	eMail3,//�����ð�ť���鿴���ʼ���
	//eMail4,//�ż����������ڴ˴���ʾ
	eMail5,//�ż�������Ʒ�ͽ�Ǯ���ڴ˴���ʾ��
	eLifeSkill1,//����������������ѧϰҳ��
	eLifeSkill2,//��Ҵ�ѧϰ�����ҳ��֮��
	eFriend1,//��ҽ�������11007������ѡ�����֮��
	eFriend2,//��һ�ú���֮��
	eAutoMove,//�����ƶ�
	eMidMap1,//����״δ��е�ͼ
	eMidMap2,//����״δ��е�ͼ
	eHidePlayer,//ͬ����ҳ���10��֮��
	eAreaEnter,//��һ�ν����񿤵�ĳ����ʱ
	eGuideTypeNum
};

class CTaskInfo
{
public:
	CTaskInfo();
	virtual ~CTaskInfo();

	bool AddTask(DWORD TaskId);
	bool RemoveTask(DWORD TaskId);
	bool AcceptTask(DWORD TaskId);
	bool FinishTask(STaskDesc *task);
	bool CancelTask(DWORD TaskId);
	bool IsTaskAccept(DWORD TaskId);
	bool IsTaskGroupAccept(DWORD GroupId);
	int  IsDoingMoneyTask();
	int  IsDoingWenTask();

	int  GetTaskFinishNum(DWORD TaskId);
	int	 GetTaskAcceptNum();
	void CleanTask();
	int  FindTimeoutTask();
	void SerializeTask(CG_CmdPacket *cmd);
	void SerializeFbTask(CG_CmdPacket *cmd);

	STaskInfo *FindTask(DWORD TaskId);
	STaskInfo *GetTaskByIndex(DWORD idx);

	std::list<STaskInfo> m_taskList;
	std::list<STaskInfo> m_taskAcceptList;                //---------�ɽ������б�
	int GetTaskInfosForID(std::vector<int> &oInfos);

	bool SaveToString(std::string& str);
	bool LoadFromString(const std::string& str);
	
	// fb task info
	bool IsFbTaskAccept(long taskId,long dif);
	bool CheckAllFbTask();
	bool SaveFbTask(std::string &str);
	bool LoadFbTask(std::string &str);

	SFbTaskInfo *GetFbTaskInfo(int fbTid);
	void AddFb(int fbTid);

	std::vector<SFbTaskInfo> m_fbTaskList;

	//---------add start by tony 06.05.09-------------------------//
	int m_currCircle;		// ��ǰ����, 50��Ϊ1�ֽ���
	BYTE	m_isActive;		// �Ƿ�����ܻ�����
	DWORD	m_passTime;	// ��ǰ��ʣ��ʱ��--ֻ�е������ܻ�ʱ��Ч
	STaskInfo* FindCircleTask();
	//---------add end   by tony 06.05.09-------------------------//

	bool m_bisDayTimeTaskFinish;//ĳһ���ճ������Ƿ����
	void RemoveMCTask();//tim.yang  MC  �Ƴ����﹥������

	int m_nMainTaskData;//���ڽ��е���������Ľ�

	bool IsDoneGuild(GuideType type);
	void SetDoneGuild(GuideType type, bool bDone);
	bool IsSendNewPlayerMail(int type);
	void SetSendNewPlayerMail(int type, bool bDone);
	unsigned long m_nGuideTag[c_g_nMaxGuideTafNum];

	typedef std::set<unsigned short> DoneTaskSet;
	DoneTaskSet& GetDoneTaskSet();
	bool IsDoneTask(unsigned long TaskId);
private:
	DoneTaskSet m_DoneTaskSet;
};

// ������
struct SPeriodicity
{
    // ʱ��
    int     iStartSecond;   // ��ʼʱ��(��00:00��ʱ��,0��ʾû������)
    int     iEndSecond;     // ����ʱ��(��00:00��ʱ��,0��ʾû������)

    // ����
    int     cycMode;        // ����ģʽ, 0 �κ�ʱ��, 1 ÿ��, 2 ÿ��, 3 ÿ��
    union
    {
        int cycWeek[7];     // ��ʾһ��֮�е��ļ����յ�Ӱ�� cycWeek[0]Ϊ����
        int cycMonth[2];    // ��ʾ����֮�е�ʱ�䷶Χ, ��ʼ����������. ��: 20,25 ��ʾÿ���µ�20�ŵ�25��
    };

    // �ظ���Χ
    int iStartYear, iStartMonth, iStartDay;     // ��ʼʱ��
    int iEndYear, iEndMonth, iEndDay;           // ����ʱ��

    bool NowIsTime();
};

struct SRebirthPos
{
	short	npcID;
	long	objID;
	short	optionalNpcID[2][2];
	long	pos[3];
	char	faction;
	char	old_faction;
	time_t	tBeAttacked;
	bool	bKilled;

	SRebirthPos()
	{
		npcID		= 0;
		objID		= 0;
		pos[0]		= pos[1] = pos[2] = 0;
		faction		= FACTION_UNKNOWN;
		old_faction = FACTION_UNKNOWN;
		tBeAttacked = 0;
		bKilled		= false;
		memset((void*)optionalNpcID, 0, sizeof(optionalNpcID));
	}
};

struct SWarfieldInfo
{
	std::string  html;                   // ����
	std::string  sceneFile;              // �����ļ�(����Ҫ��.rts)
	std::string  script;                 // �ű�

	int	lev;

	// �������˳�
	int     bornPosZhou[3];             // ������(x,y,z)
	int     exitPosZhou[3];             // �˳���(x,y,z)
	int     bornPosShang[3];             // ������(x,y,z)
	int     exitPosShang[3];             // �˳���(x,y,z)
	std::string  exitSceneBlockZhou;         // �˳���ĳ�����(����Region��)
	std::string  exitSceneBlockShang;         // �˳���ĳ�����(����Region��)

	// ����
	int     minChr;                 // ��������
	int     maxChr;                 // �������

	//�����
	SRebirthPos		posZhou;
	SRebirthPos		posShang;

	//std::list<SRebirthPos>		posTmpZhou;
	//std::list<SRebirthPos>		posTmpShang;
	std::list<SRebirthPos>		posTmp;

public:
	SWarfieldInfo()
	{
		posZhou.pos[0] = 0;
		posZhou.pos[1] = 0;
		posZhou.faction = FACTION_ZHOU;

		posShang.pos[0] = 0;
		posShang.pos[1] = 0;
		posShang.faction = FACTION_SHANG;

		//posTmpShang.clear();
		//posTmpZhou.clear();
		posTmp.clear();
	}
};

struct RtgPoint
{
	long  x;
	long  y;
};

struct STrigger
{
	STrigger(){id=0;}
	int id;								// ���ص�ID
	std::string name;					// ���ص�����
	int fbID;							// ��������ID
	int type;							// ���ص�����
	int trigger[MAX_TRIGGER_OBJ];		// �������
	long triggerUID[MAX_TRIGGER_OBJ];	// �������������ΨһID
	RtgPoint triggerPos[MAX_TRIGGER_OBJ];	// ���������λ��
	int triggerDir[MAX_TRIGGER_OBJ];	// ��������ķ���
	int triggerNum;						// ��������ĸ���
	int triggerCurNum;					// �Ѿ��������Ĵ����������
	int opendoor[MAX_TRIGGER_OBJ];		// �������
	long opendoorUID[MAX_TRIGGER_OBJ];	// �������������ΨһID
	RtgPoint opendoorPos[MAX_TRIGGER_OBJ];	// ���������λ��
	int opendoorNum;					// ��������ĸ���
	int opendoorDir[MAX_TRIGGER_OBJ];	// ��������ķ���
	int opendoorMain;					// ��Ҫ�������
	int opendoorMainUID;				// ��Ҫ�������������ΨһID
	int opendoorCurNum;					// �Ѿ������ֵı����������
	int block[MAX_TRIGGER_OBJ];			// �赲���
	long blockUID[MAX_TRIGGER_OBJ];		// �赲���������ΨһID
	RtgPoint blockPos[MAX_TRIGGER_OBJ];	// �赲��
	int blockCount;						// �赲�����
	int blockDir[MAX_TRIGGER_OBJ];		// �赲��ķ���
	int closeTrigger[MAX_TRIGGER_OBJ];	// Ҫ�رյĻ���
	int closeTriggerNum;				// Ҫ�رյĻ��صĸ���
	int	process;						// �����������ֵ
	int	active;							// �Ƿ񼤻�״̬
	int	close;							// �Ƿ��ڹر�״̬
	int curUseNum;						// ��ǰʹ�ô���
	int maxUseNum;						// ���ʹ�ô���
	vector<STaskKey> startKey;			// �������� check
	vector<STaskKey> startKeyRemove;	// �������� remove
	vector<STaskKey> startKeyAdd;		// �������� add
	long StartTime;						// ����ʱ��(����󴥷���ɾ���赲)
	long UseTime;						// �ٴδ������ʱ��
	char state;							// ��ǰ״̬(0����,1�ر�,2,���ڿ���,3,���ڹر�)
};

// ����
struct SFuBen
{
    // ����
	int     id;                     // ��������ID
	int     faction;                // ��Ӫ, 0 ��, 1 ��, 2 ȫ�� 3����
	std::string  name;                   // ��������
	std::string  html;                   // ��������
	std::string  sceneFile;              // ���������ļ�(����Ҫ��.rts)
	int     reqLev[MAX_FB_LEVEL];   // �ȼ�����(1,2,3,4,5)
	std::string  script;                 // �ű�

    // �������˳�
	int     bornPos[3];             // ������(x,y,z)
	int     exitPos[3];             // �˳���(x,y,z)
    std::string  bornSceneBlock;         // �˳���ĳ�����(����Region��)

    // ����
    int     enterType;              // ����ģʽ, 0 �������, 1 ���Ž���
    int     enterNum;               // ��������, ����˵���������Ҫ�Ķ�����߾��ŵ�����
    int     waitPos[3];             // �ȴ��㣬����Ƕ��������߾��ţ���ô�ȵ���������֮ǰ���ڵ�λ��

    // ʱ�������
    int     timeLimitSecond;        // �ʱ��, �������ʱ��ĸ����ᱻǿ���˳�
    int     timeOutSecond;          // ��ʱʱ��, ����������������������ʱ��ͻᵼ�¸���ǿ���˳�
    int     minChr;                 // ��������
    int     maxChr;                 // �������

    // ������
    SPeriodicity    cyc;            // ����

    // λ�õ�
    int     pos1[3];
    int     pos2[3];
    int     pos3[3];
    int     pos4[3];
    int     pos5[3];

    // ����
    int     reqLeaderItem;          // �ӳ����ߵ����󣬾��Ƕӳ�����Ҫ����Щ���߲��ܽ���
	int     reqMemberItem;          // ��Ա���ߵ����󣬾������еĳ�Ա����Ҫ����Щ���߲��ܽ���
};

class CRS_FuBen
{
public:
	CRS_FuBen() {}
	virtual ~CRS_FuBen() {}

	bool		Load(const char *File);
	SFuBen*     FindPos(std::string SenceName)
	{
		for(int i=0;i<(int)m_table.size();i++)
			if(m_table[i].bornSceneBlock == SenceName) return &m_table[i];	
		return NULL;
	}
	SFuBen     *Find(int Id) {
		for(int i=0;i<(int)m_table.size();i++)
			if(m_table[i].id == Id) return &m_table[i];
		return NULL;
	}
	void        GetByLevel(int level,int faction,vector<long> &fbId)
	{
		for(int i=0;i<(int)m_table.size();i++)
			if(m_table[i].reqLev[0] == level && m_table[i].faction == faction)
				fbId.push_back(m_table[i].id);
	}
	void        GetByLevelBelow(int level,int faction,vector<long> &fbId)
	{
		for(int i=0;i<(int)m_table.size();i++)
			if(m_table[i].reqLev[0] <= level && m_table[i].faction == faction) 
				fbId.push_back(m_table[i].id);
	}
	bool		GetTriggerKey(const char *str,vector<STaskKey> &key);
	bool		LoadTrigger(const char *File);	// ��ȡ���ر��
	std::map<int,STrigger> m_trigger;

private:
	std::vector<SFuBen> m_table;
};

struct SFBReward
{
	int id;
	int fbId;
	int exp;
	int gold;
	
	int itemId;
	int bindType;		// 0 not bind, 1 get bind, 2 equip bind
    std::string color;
	int colorRate;
	int colorParam1;
	int colorParam2;
};

class CRS_FBReward
{
public:
	CRS_FBReward() {}
	virtual ~CRS_FBReward() {}

	bool		Load(const char *file);
	SFBReward  *Find(int id) {
		for(int i=0; i<(int)m_table.size(); i++)
			if(m_table[i].id == id) return &m_table[i];
		return NULL;
	}

private:
	std::vector<SFBReward> m_table;
};


// LineSys
struct SLineSys
{
	// ����
	int     id;                     // GroupID
	std::string  name;                   // Group����
	std::string  html;                   // Group����
	std::string  sceneFile;              // Group�����ļ�(����Ҫ��.rts)
	std::string  script;                 // �ű�

	// �������˳�
	int     bornPos[3];             // ������(x,y,z)

	// ����
	int     enterType;              // ����ģʽ, 0 �������, 1 ���Ž���

	//����
	int     maxChr;                 //�������
	int		iPercnet;				//�л�����
	int		iLineCount;				//����

};

class CRS_LineSys
{
public:
	CRS_LineSys() {}
	virtual ~CRS_LineSys() {}

	bool		Load(const char *File);
	SLineSys    *Find(int Id) {
		for(int i=0;i<(int)m_table.size();i++)
			if(m_table[i].id == Id) return &m_table[i];
		return NULL;
	}

	DWORD m_dwGroupCount;

private:
	std::vector<SLineSys> m_table;
};

bool GetProb(double Probability);
int  GetRand(int max, int min = 0);


///*Tianh ��ҽ�Ǯ���Ļ���ʹ�õ����ͣ���ô���Ļ�ʹ�ã�*/
//enum PlayerMoneyType
//{
//	PLMT_MonsterPK             = 1;//��Ҵ�����Ļ�ʹ��
//	PLMT_Shop                  = 2;//���ͨ���̵����Ļ�ʹ��
//	PLMT_Forging               = 3;//��Ҷ������Ļ�ʹ��
//	PLMT_Skill                 = 4;//���ѧϰ�������Ļ�ʹ��
//};

enum EPlayerLogType
{
	PLT_Trade					= 1,    //����
	PLT_MoveTo					= 2,    //�ƶ�
	PLT_FollowPlayer			= 3,    //�������
	PLT_MovePlayer				= 4,    //����ƶ�
	PLT_AddItem					= 5,    //���ӵ���
	PLT_AddSkill				= 6,    //���Ӽ���
	PLT_Monitor					= 7,    //�������
	PLT_GmCommand				= 8,    //GM����ͳ��
	PLT_HackOperate				= 9,    //�Ƿ�����
	PLT_DeleteItem				= 10,   //ɾ������
	PLT_ThrowItem				= 11,   //��������
	PLT_PickItem				= 12,   //ʰȡ����
	PLT_BuyFromPShop			= 13,   //���ĸ��̵����
	PLT_PShopSell				= 14,   //�����ĸ��̵�
	PLT_UserState				= 15,   //���״̬
	PLT_Bank					= 16,   //����
	PLT_BuyFromNpc				= 17,   //���ĸ�NPC����
	PLT_SellToNpc				= 18,   //�����ĸ�NPC
	PLT_ItemUpdate				= 19,   //����װ���ĸ���
	PLT_WithdrawFeodBank		= 20,   //����������ȡ��
	PLT_FeodBank				= 21,   //����������
	PLT_UnionOperate			= 22,   //����������
	PLT_PKDrop					= 23,	// PK����
	PLT_RemoveIllegalItem		= 24,	// ϵͳɾ���Ƿ����ߣ���ͬid����ȡʧ�ܵȣ�
	PLT_WithdrawGift			= 25,	// ȡ����Ʒ
	PLT_UserOperate				= 26,	// ��ɫ����
	PLT_PrenticeSystem			= 27,	// ʦͽ����
	PLT_ServerOperate			= 28,	// ����������   //δʵ��
	PLT_CardTrade				= 29,	// �㿨����
	PLT_MagicEgg				= 30,	// ����ʵ�
	PLT_Message					= 31,	// ϵͳ��ʾ��Ϣ   //δʵ��
	PLT_ComposeItem				= 32,	// �ϳɵ���
	PLT_DecomposeItem			= 33,	// �ֽ����
	PLT_FuseItem				= 34,	// �ںϵ���
	PLT_EnchantItem				= 35,	// ��ħ����
	PLT_Auction					= 36,	// ��������
	PLT_ItemsSnapshot			= 37,	// ��ҵ���(�����ֿ�)
	PLT_ItemSaleStat			= 38,	// ϵͳ�������ߵ�ͳ��
	PLT_Warfield				= 39,	// ��ս�ɵõ����������
	PLT_WarfieldResult			= 40,	// ��ս���
	PLT_WarfieldToke			= 41,	// ��ս�ɵõ��������Ƶ����
	PLT_PointCast				= 42,	// ����ʧȥ
	PLT_PointGet				= 43,	// �������
	PLT_UseItem					= 44,	// ʹ�õ���
	PLT_LevelChanges            = 45,   // ��������仯  //�ɲ鿴��������ٶ�
	PLT_SystemMessages          = 46,   // ϵͳ�ʼ�
	PLT_FuseGem					= 47,   // ϵͳ�ʼ�
	PLT_Count					= 48
};

#define ITEM_MASTER_KEEP_TIME			15000			// (ms) 
// #define ITEM_SCENE_LIVE_TIME			20000			// (ms) 
#define SYSMESSAGE_TASK_REACH_MAX		1001
#include "item_base.h"

inline float ComputeExpRate(int userLev,int monsterLev)
{
	float final = 1;
	if(userLev > monsterLev + 5 && userLev > 15)
	{
		final = ((float)GetRealLevel(monsterLev)) / GetRealLevel(userLev);
		// if(final < 0.5) final = 0.5;
	}
	else if(userLev < monsterLev - 5)
	{
		final = ((float)userLev) / monsterLev;
	}
	
	if(final < 0) final = 0;
	if(final > 1) final = 1;
	// RtCoreLog().Info("userlev = %d,monsterlev = %d,exp rate = %f\n",userLev,monsterLev,final);
	return final;
}

inline float ComputeGoldRate(int userLev,int monsterLev)
{
	float final = 1;
	if(userLev > monsterLev + 5 && userLev > 15)
	{
		final = ((float)GetRealLevel(monsterLev)) / GetRealLevel(userLev);
		final = final; // Cheney, ��ǿ�ȼ�����ɵ�Ч��
		// if(final < 0.5) final = 0.5;
	}
	else if(userLev < monsterLev - 5)
	{
		final = ((float)userLev) / monsterLev;
	}

	if(final < 0.3) final = 0.3; //�趨������˥��,���ⷢ�������������
	if(final > 1) final = 1;
	// RtCoreLog().Info("userlev = %d,monsterlev = %d,gold rate = %f\n",userLev,monsterLev,final);
	return final;
}

/*
// magic weapon in item
const int MW_MODEL_NUM		= 1;

const int MW_PARAM_SIZE		= 5;
const int MW_PARAM_LEV		= 0;
const int MW_PARAM_EXP		= 1;
const int MW_PARAM_MP		= 2;
const int MW_PARAM_ELE		= 3; // ÿ��Ԫ��1byte 0x00ˮ��
const int MW_PARAM_MAX_LEV	= 4;
*/
// magic weapon
//
const int MW_MP_SCALE = 1;
void  MW_GetEleValue(SItemID &item,int ele[MAX_ELEMENT_ATTR]);
void  MW_SetEleValue(SItemID &item,int ele[MAX_ELEMENT_ATTR]);
int   MW_GetEleNum(int ele[MAX_ELEMENT_ATTR]);
bool  MW_GetMainEle(int ele[MAX_ELEMENT_ATTR],int &main);
void  MW_ComputeAttenuation(int ele[MAX_ELEMENT_ATTR],int val[MAX_ELEMENT_ATTR]);

void  MW_GetAddEleInfo(SItemID &item,int element,float &rate,long &time,long &mp,bool &lost);
void  MW_GetInfo(SItemID &item,int &maxMp);
void  MW_GetInfo(SItemID &item,int &maxMp);

#ifdef REGION_SERVER
void  MW_RecomputAttr(SItem &item);
#endif
void GetTrumpElement(SItemID &item,int Damage[MAX_ELEMENT_ATTR],int Armor[MAX_ELEMENT_ATTR]);

const int RifineToTrumpParamExp(int iRifine);	// �ӷ����������͵������������Ծ���COUNT��ת��
const int RifineToTrumpParam(int iRifine);	// �ӷ����������͵�������������COUNT��ת��
int	TRUMP_ActorSoulSec(SItemID &itemID, int rifine, STrump *pTrump = NULL);	// ��������ʱ��������ÿ�����ֵ
float	TRUMP_CentiSoul( SItemID &itemID, int rifine );		// ������������ֵ�����ٷֵ��������������ֵ	6��������,7Ԫ��ֵ,0-5��������Ԫ��
// magic weapon end

inline long  GetTotalClearSkillChance(int lev)
{
	return lev/10;
}

inline long  GetLevelSkillPoint(int lev)
{
	//if(lev < 5) return 0;
	//return lev - 4;
	return lev;
}

inline long  GetClearSPCost(int clearCnt)
{
	int cost = 1000;
	if(clearCnt > 10)
		clearCnt = 10;
	while(clearCnt > 0)
	{
		cost*=2;
		clearCnt--;
	}
	return cost;
}


// help function
bool TestAttackRange(long attRange,long srcPos[3],SCreModel *srcModel,long tarPos[3],SCreModel *tarModel);
bool TestCollision(long srcPos[3],SCreModel *srcModel,long tarPos[3],SCreModel *tarModel);

const int BASE_MOVE_SPEED	= 100;  //��������ƶ��ٶ�

const int HEARTBEAT_INTERVAL			= 500;

inline bool CloneCheck(vector<DWORD> &v)
{
	for(size_t i=0; i<v.size(); i++)
	{
		for(size_t j=0; j<i; j++)
		{
			if(v[j] == v[i]) return true;
		}
	}
	return false;
}

enum ENpcFunction
{
	NF_Buy				= 1, // ���̵���
	NF_Sell				= 2, // ���̵���
	NF_SkillStudy		= 3, // ѧϰ����
	NF_RecoverPet		= 4, // ��������
	NF_Bank				= 5, // ����
	NF_ItemUpdate		= 6, // ���ߴ���
	NF_CreateUnion		= 7, // ��������
	NF_UnionRecord      = 8, // �ﹱ�̵�
	NF_JobRecord        = 9, // �Ź��̵�
	NF_Credit           = 10,// �����̵�
	NF_Record           = 11,// �����̵�
	NF_Contribute       = 12,// ����
	NF_Point            = 13,// ����

	NF_Count
};

// NPC�Ƿ����������
bool NpcHaveFunction(SNpc* pNpcInfo, ENpcFunction func);

// Ⱥ���ʼ��ṹ
struct SMailEx
{
public:
	enum EReceiverType
	{
		RT_All,
		RT_Selected,
		RT_Count
	};
	DWORD senderID;
	std::string senderName;
	std::string title;
	std::string content;

	char receiverType;
	std::vector<DWORD> arrReceiverID;
	std::vector<std::string> arrReceiverName;
	std::vector<std::string> arrReceiverUnionName;

	SMailEx() { Clear(); }
	~SMailEx() { }
	void Clear()
	{
		senderID = 0;		senderName = "";		title = "";		content = "";
		receiverType = RT_Selected;
		arrReceiverID.clear();
		arrReceiverName.clear();
		arrReceiverUnionName.clear();
	}
	bool Serialize(CG_CmdPacket* pPacket)
	{
		if (pPacket->IsWritting())
		{
			short i = 0, counter = 0;

			*pPacket << senderID;
			*pPacket << senderName;
			*pPacket << title;
			*pPacket << content;

			*pPacket << receiverType;
			switch (receiverType)
			{
			case RT_All:
				break;
			case RT_Selected:
				counter = (short)arrReceiverID.size();
				*pPacket << counter;
				for (i=0; i<counter; i++)
					*pPacket <<arrReceiverID[i];
				counter = (short)arrReceiverName.size();
				*pPacket << counter;
				for (i=0; i<counter; i++)
					*pPacket <<arrReceiverName[i];
				counter = (short)arrReceiverUnionName.size();
				*pPacket << counter;
				for (i=0; i<counter; i++)
					*pPacket <<arrReceiverUnionName[i];
				break;
			default:
				return false;
				break;
			}
		}
		else 
		{
			short i = 0, counter = 0, counterForSafe = 1000;
			long nTmp = 0;
            std::string strTmp = "";

			*pPacket << senderID;
			*pPacket << senderName;
			*pPacket << title;
			*pPacket << content;

			*pPacket << receiverType;
			switch (receiverType)
			{
			case RT_All:
				break;
			case RT_Selected:
				*pPacket << counter;
				for (i=0; i<counter && counterForSafe>0; i++,counterForSafe--)
				{
					*pPacket << nTmp;	arrReceiverID.push_back(nTmp);
				}
				*pPacket << counter;
				for (i=0; i<counter && counterForSafe>0; i++,counterForSafe--)
				{
					*pPacket << strTmp;	arrReceiverName.push_back(strTmp);
				}
				*pPacket << counter;
				for (i=0; i<counter && counterForSafe>0; i++,counterForSafe--)
				{
					*pPacket << strTmp;	arrReceiverUnionName.push_back(strTmp);
				}
				break;
			default:
				return false;
				break;
			}
		}
		if (pPacket->IsError())		return false;
		else						return true;
	}
};

struct SCreatureInfo
{
	std::string name;
	std::string scene;
	long DbID;
	char metier;
	char metierLev;
	char faction;
	short level;
	long exp;
	short hp;
	short mp;
	short EnP;
    long exploit;
	long pkValue;
	long credit;
	long money;
	long bankMoney;
	short DamMin;
	short DamMax;
	short EleDamage[MAX_ELEMENT_ATTR];
	short EleArmor[MAX_ELEMENT_ATTR];
	std::string unionName;
	short petLevel;
	short petHP;

	//-------Tianh----2009.11.20-------
	long         cul; 
	long         culExp;
	long         soul;          
	long         contribute;     
	long         point;         
	long         record;         
	long         unionRecord;    
	long         jobRecord;      
	//unsigned int credit;      //����ֵ  ---- ������ԭ��������ֵ������������
	int          bornTag;      
	int          sex;
	int          gradeLev;            //��Ϊ�ȼ�
	long         vigour;
	long         vigourPoint;
	int          headImageID;
	//string       areaHonors;     //����--�������� Ԥ��128������
	//string       relivePostion;  //�󶨵ĸ����
	//---------------------------------


	bool Serialize(CG_CmdPacket *cmd)
	{
		*cmd << name;
		*cmd << scene;
		*cmd << DbID;
		*cmd << metier;
		*cmd << metierLev;
		*cmd << faction;
		*cmd << level;
		*cmd << exp;
		*cmd << hp;
		*cmd << mp;
        *cmd << exploit;
		*cmd << pkValue;
		*cmd << credit;
		*cmd << money;
		*cmd << bankMoney;
		*cmd << DamMin;
		*cmd << DamMax;
		*cmd << cul; 
		*cmd << culExp;
		*cmd << soul;          
		*cmd << contribute;     
		*cmd << point;         
		*cmd << record;         
		*cmd << unionRecord;    
		*cmd << jobRecord;      
		*cmd << bornTag;      
		*cmd << sex;
		*cmd << gradeLev;            //��Ϊ�ȼ�
		*cmd << vigour;
		*cmd << vigourPoint;
		*cmd << headImageID;
		for (int i=0; i<MAX_ELEMENT_ATTR; i++)
		{
			*cmd << EleDamage[i];
			*cmd << EleArmor[i];
		}
		*cmd << unionName;
		*cmd << petLevel;
		*cmd << petHP;
		*cmd << soul;

		return !cmd->IsError();
	}
};

const std::string COLOR_FIRE = "#dd5511";
const std::string COLOR_WATOR = "#00aaff";
const std::string COLOR_POISON = "#30e512";

enum ECreatureState
{
	CreState_Visible		= 0x00000001,
	CreState_CanSpeak		= 0x00000002,
	CreState_CanPShop		= 0x00000004,
	CreState_CanMove		= 0x00000008,
};

struct SGift
{
	DWORD	index;
	DWORD	userID;
	int		level;
	int		type;
	std::string	attribs;
	DWORD	addTime;
	DWORD	limitTime;
	std::string message;

	SGift()
	{
		index = 0; userID = 0; level = 0; type = 0; attribs = ""; addTime = 0; limitTime = 0; message = "";
	}

	bool Serialize(CG_CmdPacket* pPacket)
	{
		*pPacket << index;
		*pPacket << userID;
		*pPacket << level;
		*pPacket << type;
		*pPacket << attribs;
		*pPacket << addTime;
		*pPacket << limitTime;
		*pPacket << message;
		return !(pPacket->IsError());
	}
};

const char GIFT_TYPE_ITEMS			= 1;
const char GIFT_TYPE_MONEY			= 2;
const char GIFT_TYPE_EXP			= 3;
const char GIFT_TYPE_EXPLOIT        = 4;

const char GIFT_SUCCESS				= 0;
const char GIFT_ERROR_UNKNOWN		= 1;
const char GIFT_NONE				= 2;
const char GIFT_CAN_NOT_WITHDRAW	= 3;
const char GIFT_BAG_NOT_ENOUGH		= 4;


// ����CHECK������ݲ����Ƿ��޸ĵĽṹ
#define SUserDataAssert_Version		2
struct SUserDataAssert
{
	short		version;

	int			userID;
	int			bankMoney;
	long        money;

	short		metier;
	short		metierLevel;
	short		faction;

	long		level;
	long		exp;
	long		ap;
	long		sp;
	long		hp;
	long		mp;
	long        EnP;
	long		x;
	long		y;

	//Tianh 09.11.20
	long         Cul;  
	long         CulExp;
	long         Soul;          
	long         Contribute;     
	long         Point;         
	long         Record;         
	long         UnionRecord;    
	long         JobRecord;      
	long         BornTag;      
	long         Sex;
	int          HeadImageID;
	long         GradeLev;   
	long         Vigour;
	long         VigourPoint;
	long         RelivePostion;
	long         PlayerOnlineTime;

	short		constitution;
	short		strength;
	short		dexterity;
	short		intellect;
	short		hit;
	long        Exploit;
	long        pk;
	long        credit;
	short       spClearCnt;
};

void SUserDataAssert_CreateMD5String12(SUserDataAssert& uda, std::string& outString);

extern CRS_Popularity  g_TablePopularity;
extern CRS_Exp		 g_TableExp;
extern CRS_UserActor g_TableUserActor;
extern CRS_CreModel  g_TableCreModel;
extern CRS_Npc		 g_TableNpc;
extern CRS_Pet       g_TablePet;
extern CRS_SceneInfo g_TableScene;
extern CRS_Task		 g_TableTask;
extern CRS_HeadModel g_TableHeadModel;
extern CRS_CharImage g_TableCharImage;
extern CRS_BaseValue g_TableBaseValue;
extern CRS_FuBen     g_TableFB;
extern CRS_FBReward  g_TableFBReword;
extern CRS_LineSys	 g_TableLineSys;
extern CRS_AtrribPoint	g_TableAtrribPoint;

#ifdef WIN32
#   define KICK_CHEAT
#else
#   define KICK_CHEAT
#endif

#define USE_LUA_CHECK

int ha_P2(char const*s, int maxn);
int ha_P3(char const*s, int maxn, int hashs);

struct SUserAttr
{
	SUserAttr() {
		spClearCnt = 0;
		EnP = 0;
	}
	short       actorID;
	short		metier;
	short		metierLevel;
	short		faction;

	long		level;
	long		exp;
	long		ap;
	long		sp;
	long		hp;
	long		mp;
	long        EnP;
	long		x;
	long		y;

	//----Tianh---2009.11.20---
	long         Cul;  
	long         CulExp;
	long         Soul;          
	long         Contribute;     
	long         Point;         
	long         Record;         
	long         UnionRecord;    
	long         JobRecord;      
	long         BornTag;      
	long         Sex;            //�����ɫ�Ա�
	int          HeadImageID;
	long         GradeLev;   
	long         Vigour;
	long         VigourPoint;
	long         RelivePostion;
	long         PlayerOnlineTime;
   // string       AreaHonors;    
	//string       RelivePostion; 
	//unsigned int Credit;        //����ֵ  ---- ������ԭ��������ֵ������������
	//-------------------------


	short		constitution;
	short		strength;
	short		dexterity;
	short		intellect;
	short		hit;

    long        exploit;    // ս��
	long        pk;
	long        credit;
	short       spClearCnt;

	void Reset() {
		hp = 1000;
		mp = 1000;
	}

	friend CG_CmdPacket& operator<<( CG_CmdPacket& cmd, SUserAttr &attr)
	{
		cmd << attr.actorID << attr.metier << attr.metierLevel << attr.faction;
		cmd << attr.level << attr.exp << attr.ap << attr.sp << attr.hp << attr.mp <<attr.EnP;
		cmd << attr.x << attr.y;
		cmd << attr.Cul << attr.CulExp << attr.Soul << attr.Contribute << attr.Point <<attr.Record <<attr.GradeLev;
		cmd << attr.UnionRecord << attr.JobRecord << attr.BornTag ;
		cmd << attr.RelivePostion <<attr.exploit<<attr.Sex;
		cmd << attr.constitution << attr.strength << attr.dexterity << attr.intellect;
		cmd << attr.hit << attr.exploit << attr.pk << attr.credit << attr.spClearCnt;
		cmd << attr.Vigour << attr.VigourPoint << attr.HeadImageID << attr.PlayerOnlineTime;
		return cmd;
	}
};

struct SUserData
{
	int					userID;
	int					unionID;
	string				unionName;
	string				unionNickName;
	string				unionIcon;
	string				scene;
	string				account;
	string				name;
	string				strSkills;
	string              strEffects;
	string				items;
	int					money;
	string				friends;
	string				teams;
	string				shortcut;
	string				configData;
	string				task;
	short				headModelID;
	char                cSwitchRegion;
	string				bank;
	int					bankMoney;
	string				bankPass;
	long                dungeonUID;     // ����ΨһID
	long                dungeonTID;     // ��������ID
	DWORD				LineSYSGroupID; //��������Ϣ
	DWORD				LineSYSLineID; //��������Ϣ
	long                dungeonLastQuit;
	string              fbTask;
	SUserAttr			attr;
	time_t				timeStamp;
	long				seed;
	long                privileges;     // Ȩ�� (enum EUserPrivileges [gw_protocol.h])
	long				dwState;
	long                lastUpdateTime;
	RtMemoryBlock       extraData;
	long                onlineTime;
	bool				IsInWarfield;
	long				lWarfieldLastQuit;
	string				huiyuanTime;//tim.yang  ��Աʱ��
	bool				bAdult;			//�Ƿ����	ac.ma
	char				isVip;
	bool				IsOnline;
	bool				bTeamFree;
	long				petItemID;
	string              popularity;//����
	
	short               isFirstLogin; //�Ƿ��һ�ε�½
	short               isDead;      //�Ƿ�����
	int                 ChangeModelID;  //����ģ��ID

	string				pet;
	string				bankPet;

	long				WarScores;       //ս������(GWSʹ��)

	//long                UpdateSCDTime;  //����CD����ʱ��
	long				lFunctionState[SCRIPT_FUNC_STATE_COUNT];	//�ű��ù���״̬��20������

	SUserData()
	{
		//�ű�����ֵ��ʼ��
		for(int ii=0; ii<SCRIPT_FUNC_STATE_COUNT; ii++)
			lFunctionState[ii] = 0;
	}

	void Serialize(CG_CmdPacket &packet,bool download)
	{
		// download means gws send data to region
		packet << scene << attr << strSkills << strEffects << items << pet << bankPet;
		packet << money << friends << shortcut << configData << task << bank << bankMoney;
		packet << dwState << dungeonUID << dungeonTID << dungeonLastQuit << fbTask;
		packet << IsInWarfield << lWarfieldLastQuit<<LineSYSGroupID<<LineSYSLineID<<petItemID;
		packet << extraData <<popularity<<isFirstLogin<<isDead<<ChangeModelID<<WarScores/*<<UpdateSCDTime*/;
		for(int ii=0; ii<SCRIPT_FUNC_STATE_COUNT; ii++)
			packet << lFunctionState[ii];

		if(download)
		{
			packet << userID << name << account << huiyuanTime << isVip << bAdult;	//tim.yang  ��Աʱ��
			packet << unionID << unionName << unionNickName << unionIcon;
			packet << seed << privileges << cSwitchRegion << bankPass;
			packet << headModelID << lastUpdateTime << onlineTime<<bTeamFree<<popularity<<isFirstLogin<<isDead;

			if(packet.IsReading())
			{	
				timeStamp  = time(NULL);
				privileges = (privileges&0x7FFFFFFF);
			}
		}
	}
};

// LogDB Table
const int LOGTABLE_INDEX_MAX = 10;
const std::string LOGTABLE_NAME_BASE = "T_log_user_history";


struct SNpcSellRareItem
{
    DWORD itemType;         // ��������id
    SItemID item;           // ����
    short count;            // Ŀǰ�����ĸ���
    short maxCount;         // ���������
    int interval;           // ÿ������ʱ�䷷����һ��
    int nextSpawnTime;      // ��һ����ʲôʱ��

    SNpcSellRareItem()
    {
        itemType = 0; count = 0; maxCount = 0; interval = 0; nextSpawnTime = 0;
    }
    bool Serialize(CG_CmdPacket* pPacket)
    {
        item.Serialize(pPacket);
        *pPacket << count;
        *pPacket << maxCount;
        *pPacket << interval;
        *pPacket << nextSpawnTime;
        return !pPacket->IsError();
    }

    bool Load(const std::string& str);
    bool Create(CItemManager* pItemMgr);
};



extern bool LoadConfig(std::string& vrError);
extern RtIni* GetConfigBoot();
extern RtIni* GetGameRule();


class CIniVariable
{
public:

	template<class T>
		CIniVariable(RtIni* vpIni,const string& vrSection,const string& vrEntry,T& vrOut)
	{
		vpIni->GetEntry(vrSection.c_str(),vrEntry.c_str(),&vrOut);
	};
	//template<>
		CIniVariable(RtIni* vpIni,const string& vrSection,const string& vrEntry,int& vrOut)
	{
		vpIni->GetEntry(vrSection.c_str(),vrEntry.c_str(),(long*)&vrOut);
	}


	//�ַ���
	CIniVariable(RtIni* vpIni,const string& vrSection,const string& vrEntry,const char** vppOut)
	{
		*vppOut = vpIni->GetEntry(vrSection.c_str(),vrEntry.c_str());
	}

	~CIniVariable(){};
};


const int c_nMaxData = 10;
struct MainStoryData
{
	int nDataID;//ID
	std::string strDataName;//������
	int nJob;//ְҵ
	int nGroupID;//����������
	int nTaskNum;
	long nTaskID[c_nMaxData];//�������ID
	std::string strImage1;//��������ͼƬ1
	std::string strImage2;//��������ͼƬ1
};

class CRS_MainStory
{
public:
	CRS_MainStory();
	virtual ~CRS_MainStory();

	bool Load(const char *File);
	MainStoryData* FindMainStory(int Id);
public:
	typedef std::vector<MainStoryData> MainStoryDataV;
	MainStoryDataV& GetMainStoryDataV();
private:
	MainStoryDataV m_MainStoryDataV;
};

struct TaskGroupData 
{
	int nGroupID;//ID
	std::string strGroupName;
	std::string strImage1;
	std::string strImage2;
	std::string strImage3;
	std::string strChapter1;
	std::string strChapter2;
};

class CRS_TaskGroup
{
public:
	CRS_TaskGroup();
	virtual ~CRS_TaskGroup();

	bool Load(const char *File);
	TaskGroupData* FindTaskGroup(int Id);
	bool GetMainLineID(vector<int>& taskId);
public:
	typedef std::vector<TaskGroupData> TaskGroupDataV;
	TaskGroupDataV& GetMainStoryDataV();
private:
	TaskGroupDataV m_TaskGroupDataV;
};

struct SUnionIcon
{
	long nIconID;
	long nMoney;
	std::string strIconName;
	std::string strDesc;
};

class CRS_UnionIcon
{
public:
	CRS_UnionIcon();
	virtual ~CRS_UnionIcon();

	bool Load(const char *File);
	SUnionIcon* FindUnionIcon(int Id);
	
public:
	typedef std::vector<SUnionIcon> UnionIconV;
	UnionIconV& GetUnionIconV();
private:
	UnionIconV m_UnionIconV;
};

//�̳�
struct SShopCenter
{
	long id;
	std::string itemName;
	DWORD itemId;
	short type;
	short subType;
	short commendType;
	bool  bHotSale;
	float fSubPriceRate;
	bool  bPresent;
	short presentType;
	short timeLimitType;
	bool  bSetNumber;
	long  goldPrice;
	long  pointPrice;
	std::string texName;      //ʱװ��ͼ
	bool  bValid;

	SShopCenter()
	{
		id = 0;
		itemName = "";
		itemId = 0;
		type = 0;
		subType = 0;
		commendType = 0;
		bHotSale = false;
		fSubPriceRate = 0.f;
		bPresent = false;
		presentType = 0;
		timeLimitType = 0;
		bSetNumber = false;
		goldPrice = 0.f;
		pointPrice = 0.f;
		texName = "";
		bValid = false;
	}
};

class CRS_ShopCenter
{
public:
	CRS_ShopCenter();
	virtual ~CRS_ShopCenter();

	bool Load(const char* fileName);
	SShopCenter* FindShopCenter(int ID);

public:
	typedef std::vector<SShopCenter> ShopCenterV;
	ShopCenterV& GetShopCenterV();
private:
	ShopCenterV m_ShopCenterV;
};
extern CRS_MainStory   g_TableMainStory;
extern CRS_UnionIcon   g_TableUnionIcon;
extern CRS_TaskGroup   g_TaskGroup;
extern CRS_ShopCenter  g_TableShopCenter;


//add by yz 2010-6-11:shared�е�ȫ���ڴ汣��ע��
inline void RegisterSharedMemProtector(void)
{
	G_MEMPROTECTOR(g_strItemBuffer10240, 10240)  //item_base.cpp
	G_MEMPROTECTOR(g_strItemBuffer20480, 20480)
	G_MEMPROTECTOR(g_strItemBuffer256, 256)
	G_MEMPROTECTOR(g_strItemBufferBD, 256)	
	G_MEMPROTECTOR(g_strItemBuffer4096, 4096)  

#if defined(REGION_SERVER) || defined(GAME_CLIENT)
	G_MEMPROTECTOR(g_szSkillErrMsg, 100)         //skill_base.cpp
#endif

//#ifdef GAME_CLIENT
//	extern void	RegAnimMemory(void);
//	extern void	RegActorMgrMemory(void);
//	RegAnimMemory();
//	RegActorMgrMemory();
//#endif
}
//end

#endif
