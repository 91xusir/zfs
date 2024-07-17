#ifndef _INC_GW_PROTOCOL_H_
#define _INC_GW_PROTOCOL_H_

#define GW_MAX_REGION_COUNT		(512)

enum
{
    //////////////////////////////////////////////////////////////////////////
	g2r_base					= 100,
	g2r_shutdown				= 101,
	g2r_chat					= 102,
	g2r_mail					= 103,
	g2r_login					= 104,
	g2r_add_user				= 105,
	g2r_kick_user				= 106,
	g2r_update_region_list		= 107,
	g2r_add_region				= 108,
	g2r_remove_region			= 109,
	g2r_union					= 110,
	g2r_feod					= 111,
	//g2r_bank_query_response	= 112,
	//g2r_bank_store_result		= 113,
	//g2r_bank_withdraw_result	= 114,
	g2r_chat_list				= 115,
	g2r_herolist				= 116,
	g2r_init_region				= 117,
	g2r_reboot_region			= 118,
	g2r_itemid					= 119,
	g2r_pull_user				= 120,
	g2r_move_user				= 121,
	g2r_team					= 122,
	g2r_friend					= 123,
	g2r_gm_response				= 124,
	g2r_gm_bulletin				= 125,	// �����
    g2r_safe_quited             = 127,
	g2r_castlewar				= 128,	//------------add start by tony 05.06.16
	g2r_pull_useraround			= 129,
	g2r_move_useraround			= 130,
    g2r_check_version_ret       = 131,
    g2r_tester                  = 132,
	g2r_query_user_info			= 133,
	g2r_user_info				= 134,
	g2r_user_state				= 135,
	g2r_query_gifts_response	= 136,
	g2r_start_fun               = 137,
	g2r_end_fun                 = 138,
    g2r_create_dungeon          = 139,  // ��������
    g2r_enter_dungeon_ret       = 140,  // ������ҽ��븱��������
    g2r_leave_dungeon_ret       = 141,  // ��������뿪����������
    g2r_destroy_dungeon_ret     = 142,  // ���ٸ�������
    g2r_broadcast_to_region     = 143,  // �㲥һ��������е�Region
    g2r_query_poing_ret         = 144,
    g2r_prededuct_ret           = 145, // Ԥ�۵����ķ���ֵ, �������õ�����NPC�����
    g2r_deduct_ret              = 146, // �۵����ķ���ֵ, �������õ�����NPC�����
	g2r_license_check_ret		= 147, //
	g2r_add_master_prentice_result = 148,
	g2r_remove_master_prentice_result = 149,
	g2r_finish_master_prentice_result = 150,
    g2r_card_consign_ret        = 151,  // �����㿨����
    g2r_card_buy_ret            = 152,  // ����㿨����
    g2r_card_cancel_ret         = 153,  // ȡ�������㿨����
    g2r_card_update             = 154,  // ���¼�����Ϣ
	g2r_master_prentice_update	= 155,	// ����ʦͽ����
	g2r_master_prentice_credit	= 156,	// ͽ������������ʦ��������ֵ
	g2r_bankpass_result			= 157,	// �ֿ�����
	g2r_kick_all_user           = 158,	// �ߵ��������
	g2r_prentice_onlinetime		= 159,	// ͽ�ܵ�����ʱ��
	g2r_card_trade_switch       = 160,
	g2r_master_prentice_gift	= 161,	// ʦͽϵͳ�Ķ��⽱��
	g2r_auction					= 162,  // ����
	g2r_master_list				= 163,  // ʦ���б�
	g2r_auction_failed          = 164,  // ����
	g2r_auction_bid_failed      = 166,  // ����
	g2r_auction_update          = 168,  // ����
	g2r_auction_event           = 169,  // ����
    g2r_auction_get_result      = 170,
	g2r_warfield_start			= 171,
	g2r_warfield_stop			= 172,
	g2r_warfield_flag_destroyed_rep	= 173,
	g2r_warfield_leave_rep		= 174,
	g2r_warfield_prepare		= 175,
	g2r_warfield_msg			= 176,
	g2r_warfield_update_data	= 177,
	g2r_warfield_addscore_rep	= 178,
	g2r_warfield_update_rate	= 179,
    g2r_update_poing_ret        = 180,
	g2r_add_point_ret			= 181,
	g2r_execute_gmcmd			= 182,
	g2r_add_account_time_ret	= 183,	//ac.ma  ���ӻ�Աʱ��
	g2r_linesys					= 184,	//rendh 2010.01.12
	g2r_chatNew_list            = 185,//Tianh ϵͳ����
	g2r_Item_Mail               = 186,
	g2r_friend_UserID           = 187,
	g2r_one_refresh             = 188,
	g2r_onefriend_return        = 189,
	g2r_not_found               = 190,
	g2r_notall_found            = 191,
	g2r_UserID_return           = 192,
	g2r_when_fiend              = 193,
	g2r_Black_List              = 194,
	g2r_team_Picker				= 195,
	g2r_PakeBlack_List          = 196,
	g2r_public_chat             = 197,
	g2r_SetRefuse_friend        = 198,

    //////////////////////////////////////////////////////////////////////////
	r2g_base					= 200,
	r2g_register				= 201,
	r2g_update_user				= 202,
	r2g_query_user				= 203,
	r2g_add_mail				= 204,
	r2g_remove_mail				= 205,
	r2g_chat					= 206,
	r2g_accept_user				= 207,
	r2g_repatriate_user			= 208,
	r2g_union					= 209,
	r2g_feod					= 210,
	r2g_logout					= 211,
	//r2g_bank_query			= 212,
	//r2g_bank_store			= 213,
	//r2g_bank_withdraw			= 214,
	r2g_switch_region			= 215,
	r2g_herolist_query			= 216,
	r2g_log_event				= 217,
	r2g_gm_additem_event		= 218,
	r2g_query_itemid			= 219,
	r2g_save_itemid				= 220,
	r2g_move_user				= 221,
	r2g_gm_move					= 222,
    r2g_move_to                 = 223,
	r2g_team					= 224,
	r2g_friend					= 225,
    r2g_update_region_state     = 227,
    r2g_safe_quited             = 228,
    r2g_castlewar				= 229,  //------------add start by tony 05.06.16
	r2g_move_useraround			= 230,
    r2g_check_version           = 231,
	r2g_kick					= 232,
    r2g_tester_ret              = 233,
	r2g_query_user_info			= 234,
	r2g_user_info				= 235,
	r2g_forbid_user				= 236,
	r2g_query_gifts				= 237,
	r2g_withdraw_gift_success	= 238,
	r2g_start_fun               = 239,
	r2g_end_fun                 = 240,
    r2g_create_dungeon_ret      = 241,  // ����������Ӧ
    r2g_enter_dungeon           = 242,  // ��ҽ��븱��������
    r2g_leave_dungeon           = 243,  // ����뿪����������
    r2g_destroy_dungeon         = 244,  // ���ٸ���
    r2g_broadcast_to_region     = 245,  // �㲥һ��������е�Region, ��ͬһ��Gameworld�е�
    r2g_login_server_command    = 246,  // Login Server ����
    r2g_charge_card             = 247,
    r2g_query_poing             = 248,  // ��ѯ�������
    r2g_keep_online             = 249,  // ��ұ�������״̬
    r2g_broadcast_to_all_region = 250,  // �㲥һ��������е�Region, ��ͬһ��Login�е�
    r2g_broadcast_to_gameworld  = 251,  // �㲥һ��������е�Gameworld, ��ͬһ��Login�е�
    r2g_prededuct               = 252,  // Ԥ�۵���, �������õ�����NPC�����
    r2g_deduct                  = 253,  // �۵���, �������õ�����NPC�����
	r2g_lock_account            = 254,  // �����ʺ�
	r2g_license_check			= 255,	//
	r2g_add_master_prentice		= 256,	// ��ʦ
	r2g_remove_master_prentice	= 257,	// ���ʦͽ
	r2g_finish_master_prentice	= 258,	// ��ʦ
    r2g_card_consign            = 259,  // �����㿨
    r2g_card_buy                = 260,  // ����㿨
    r2g_card_cancel             = 261,  // ȡ�������㿨
    r2g_card_request_update     = 262,  // ���������Ϣ
	r2g_master_prentice_check	= 263,	// ��֤ʦͽ�����Ƿ���ȷ
	r2g_master_prentice_credit	= 264,	// ͽ�������ˣ�����ʦ��������ֵ
	r2g_bankpass				= 265,	// �ֿ�����
    r2g_broadcast_bulletin      = 266,  // ���͹���
	r2g_prentice_onlinetime		= 267,	// ����ͽ�ܵ�����ʱ��
	r2g_card_trade_switch       = 268,
	r2g_master_prentice_gift	= 269,  // ʦͽϵͳ�Ķ��⽱��
	r2g_auction					= 270,  // ����
	r2g_master_list				= 271,  // ʦ���б�
    r2g_auction_auction         = 272,
    r2g_auction_cancel_auction  = 273,
    r2g_auction_cancel_bid      = 274,
    r2g_auction_bid             = 275,
    r2g_auction_get_items       = 276,
    r2g_auction_get_items_ret   = 277,

	r2g_warfield_enter			= 278,
	r2g_warfield_start_rep		= 279,
	r2g_warfield_leave			= 280,
	r2g_warfield_flag_destroyed	= 281,
	r2g_warfield_stop_rep		= 282,
	r2g_warfield_addscore		= 283,
	r2g_warfield_open			= 284,
	r2g_warfield_update_data	= 285,
	r2g_warfield_conjure_progress = 286,
	r2g_warfield_update_exploit	= 287,
	r2g_warfield_stop			= 288,
	r2g_warfield_add_exp		= 289,
	r2g_warfield_add_per_exploit= 290,
	r2g_warfield_create_arms	= 291,
	r2g_warfield_tmp_per_exploit= 292,
	r2g_user_change_block		= 293,
	r2g_modify_maxdungeon		= 294,//tim.yang  ���ø����������
	r2g_add_point				= 295,
	r2g_add_account_time		= 296,		//ac.ma ���ӻ�Աʱ��
	r2g_linesys					= 297,	//rendh 2010.01.12
	r2g_SystemNews              = 298,  //Tianh ϵͳ����
	r2g_MailItem                = 299, 

    //////////////////////////////////////////////////////////////////////////
	g2c_base					= 300,
	g2c_login_ret				= 301,
	g2c_create_char_ret			= 302,
	g2c_delete_char_ret			= 303,
	g2c_query_account_ret		= 304,
	g2c_select_char_ret			= 305,
	g2c_kick					= 306,
    g2c_decline                 = 307, // char = 0 ������ά��, 1 ip�ܾ�, 2 ����������������
    g2c_session_close           = 308, // char = 0 ������ά��
	g2c_char_password_change_ret= 309,
	g2c_char_password_confirm_ret= 310,
	g2c_char_password_set_ret	= 311,
	g2c_char_password_del_ret	= 312,
	//PZH
	g2c_restore_char_ret		= 313,
	//
	r2g_fiend_name_ID           = 314,
	r2g_one_refresh             = 315,
	r2g_OneFriend_Return        = 316,
	r2g_when_fiend              = 317,
	r2g_Black_List              = 318,
	r2g_PakeBlack_List          = 319,
	r2g_activity_system			= 380,
	g2r_activity_system			= 381,
	r2g_Set_Refuse              = 382,

    //////////////////////////////////////////////////////////////////////////
	c2g_base					= 400,
	c2g_login					= 401,
	c2g_logout					= 402,
	c2g_create_char				= 403,    
	c2g_select_char				= 404,
	c2g_delete_char				= 405,
	c2g_restore_char			= 406,
	c2g_query_account			= 407,
	c2g_char_password_change	= 408,
	c2g_char_password_confirm	= 409,
	c2g_char_password_set	    = 410,
	c2g_char_password_del	    = 411,

    //////////////////////////////////////////////////////////////////////////
	gm2gw_base					= 500,
	gm2gw_chat					= 501,
	gm2gw_tickuser				= 502,
	gm2gw_query_itemid			= 503,
	gm2gw_save_itemid			= 504,
	gm2gw_queryuserstate		= 505,
	gm2gw_gm_response			= 506,
	gm2gw_gm_bulletin			= 507,	// �����
	gm2gw_get_init_config		= 508,
    gm2gw_query_herolist        = 509,
	gm2gw_send_mail				= 510,
	gm2gw_query_userstate		= 511,

    //////////////////////////////////////////////////////////////////////////
	gw2gm_base					= 600,
	gw2gm_chat					= 601,
	gw2gm_gm_additem_event		= 602,
	gw2gm_tickuser_result		= 603,
	gw2gm_itemid				= 604,
	gw2gm_userstate				= 605,
	gw2gm_update_config         = 606,
    gw2gm_herolist              = 607,

	//////////////////////////////////////////////////////////////////////////
	auth2r_license_data			= 800,
	
	r2g_Is_Open                 = 801
};

/*
struct GWUserAttribute
{
	GWUserAttribute()
	{
		spClearCnt = 0;
	}

	short		actorID;
	short		metier;
	short		metierLevel;
	short		faction;

	long		level;
	long		exp;
	long		ap;
	long		sp;
	long		hp;
	long		mp;
	long		x;
	long		y;

	short		constitution;
	short		strength;
	short		dexterity;
	short		intellect;
	short		hit;

	long        pk;
	long        credit;
	short       spClearCnt;

	void Reset()
	{
		hp = 1000;
		mp = 1000;
	}
};
*/
enum GWCreateUserResult
{
	gws_create_user_okay = 0,
	gws_create_user_duplicate_name = 1,
	gws_create_user_invalid_name = 2,
	gws_create_user_error = 3,
	gws_create_user_faction_error = 4,
	gws_create_user_banned_name=5,
	gws_create_user_max
};

// �û�Ȩ�� [ע���lgs_gws.h�е� �ʺ�Ȩ�� ͬ������]
//     ����Ȩ��Ĭ��Ϊ 0xFFFF 65535
//     GM  Ȩ��Ĭ��Ϊ 0x07FF 2047
//     ��ͨȨ��Ĭ��Ϊ 0x0001 1

enum EUserPrivileges
{
    EUP_LoginNormal                  = 0x00000001, // ��ͨ��½

    EUP_GM_Mask                      = 0x00000FF0, // GM����
    EUP_LoginGM                      = 0x00000010, // �߼���¼
    EUP_FastMove                     = 0x00000020, // �����ƶ�
    EUP_MoveUser                     = 0x00000040, // ץ�����, ��ҽ���, �޳����
    EUP_AddItems                     = 0x00000080, // ��ӵ���
    EUP_ModifyAttrib                 = 0x00000100, // �޸�����
    EUP_ReloadScript                 = 0x00000400, // ��������ű�
    EUP_Developer                    = 0x00000800, // ������ר��

    EUP_MonitorMask                  = 0x0000F000, // ��س���
    EUP_MonitorState                 = 0x00001000, // ���״̬
    EUP_SendBillboard                = 0x00002000, // ���͹���
    EUP_ServerOperate                = 0x00004000, // �رջ�����������

    EUP_MAX                          = 0x80000000
};

#endif // _INC_GW_PROTOCOL_H_

