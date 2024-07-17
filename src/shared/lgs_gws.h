#ifndef __LGS_GWS_H__
#define __LGS_GWS_H__

const int MAX_USER_NAME_SIZE = 20;
const int MAX_USER_PWD_SIZE  = 32;

struct SAccountInfo
{
	DWORD       accountId;
	std::string	accountName;		// 	�ʺ�
	std::string password;
	DWORD	    beginTime;			// 	���ѿ�ʼʱ��,1970�굽���ڵ�����
    long        privileges;         // Ȩ�� (enum EUserPrivileges [gw_protocol.h])
	long	    totalPot;			// 	�ʺ�ʣ��΢����
	char	    chargeType;			// 	�Ʒѷ�ʽ
	
	bool		bAdult;			//�Ƿ����	ac.ma

	std::string		huiYuanTime;//tim.yang  ��Աʱ��
	std::string ip;
	short       port;
	char		isVip;			//�Ƿ��ǻ�Ա������������ǻ�Ա��
	/*
	//	����
	ULONG month_etime;			// 	���µ���ʱ��

	//	ʱ��
	ULONG period_time;			// 	����ʱ��(��)
	ULONG period_pot;			// 	����΢����

	ULONG p_pot_used;			// 	�ѻ���΢����
	ULONG p_end_time;			// 	��Ԥ�۵���ʱ��
	*/

	friend CG_CmdPacket& operator<<(CG_CmdPacket& cmd, SAccountInfo &p)
	{
		cmd << p.accountId << p.accountName << p.password << p.beginTime << p.totalPot << p.chargeType << p.privileges <<p.huiYuanTime << p.bAdult;
		cmd << p.ip << p.port << p.isVip;
		return cmd;
	}
};

// GameWorld Server State

const char gwssNone                         = 0;
const char gwssWaitRegion                   = 1;
const char gwssAcceptLogin                  = 2;

// login server status ��½��������״̬

const long lssLoginNormal                   = 0x0001; // �û�������½
const long lssLoginGM                       = 0x0002; // GM������½
const long lssUpdateList                    = 0x0004; // ����UpdateList��ȥ

// account user privileges �ʺ�Ȩ�� [ע���gw_protocol.h�е�EUserPrivilegesͬ������]

const long gwupLoginNormal                  = 0x0001; // ��ͨ��½

const long gwupLoginGM                      = 0x0010; // �߼���¼
const long gwupFastMove                     = 0x0020; // �����ƶ�
const long gwupMoveUser                     = 0x0040; // ץ�����
const long gwupAddItems                     = 0x0080; // ���ӵ���
const long gwupModifyAttrib                 = 0x0100; // �޸�����
const long gwupReloadScript                 = 0x0400; // ��������ű�
const long gwupDeveloper                    = 0x0800; // ������ר��
const long gwupGM_Mask                      = 0x0FF0; // ����GM����

const long gwupMonitorState                 = 0x1000; // ���״̬
const long gwupSendBillboard                = 0x2000; // ���͹���
const long gwupServerOperate                = 0x4000; // �رջ�����������

// Command

const short lgs2gws_base					= 1000;         
const short lgs2gws_accept_login			= lgs2gws_base + 1;
const short lgs2gws_user_login_ret			= lgs2gws_base + 2;
const short lgs2gws_kick_user				= lgs2gws_base + 3;
const short lgs2gws_shutdown_gws			= lgs2gws_base + 4;
const short lgs2gws_bulletin                = lgs2gws_base + 6;
const short lgs2gws_bulletin_shutdown       = lgs2gws_base + 7;
const short lgs2gws_check_version_ret       = lgs2gws_base + 8;
const short lgs2gws_check_sub_version_ret   = lgs2gws_base + 9;
const short lgs2gws_register_ret            = lgs2gws_base + 10;
const short lgs2gws_tester                  = lgs2gws_base + 11;
const short lgs2gws_user_message            = lgs2gws_base + 12;
const short lgs2gws_add_gift                = lgs2gws_base + 13;
const short lgs2gws_clear_gift              = lgs2gws_base + 14;
const short lgs2gws_charge_card_ret         = lgs2gws_base + 15;
const short lgs2gws_query_point_ret         = lgs2gws_base + 16;
const short lgs2gws_broadcast_to_all_region = lgs2gws_base + 17;
const short lgs2gws_broadcast_to_gameworld  = lgs2gws_base + 18;
const short lgs2gws_card_can_trade_ret      = lgs2gws_base + 19;
const short lgs2gws_deduct_ret              = lgs2gws_base + 20;
const short lgs2gws_prededuct_ret           = lgs2gws_base + 21;
const short lgs2gws_license_check_ret       = lgs2gws_base + 22;
const short lgs2gws_license_check_rs_ret    = lgs2gws_base + 23;
const short lgs2gws_card_charged_event      = lgs2gws_base + 24;
const short lgs2gws_kick_all_user           = lgs2gws_base + 25;
const short lgs2gws_update_point_ret        = lgs2gws_base + 26;
const short lgs2gws_add_point_ret			= lgs2gws_base + 27;
const short lgs2gws_add_account_time_ret	= lgs2gws_base + 28;	//tim.yang   VIP��
const short gws2lgs_base					= 2000;         
const short gws2lgs_user_login				= gws2lgs_base + 1;
const short gws2lgs_user_quit				= gws2lgs_base + 2;
const short gws2lgs_shutdown_ok				= gws2lgs_base + 3;
const short gws2lgs_update_state			= gws2lgs_base + 4;
const short gws2lgs_query_state			    = gws2lgs_base + 6;
const short gws2lgs_update_gws_state        = gws2lgs_base + 7;
const short gws2lgs_check_version           = gws2lgs_base + 8;
const short gws2lgs_check_sub_version       = gws2lgs_base + 9;
const short gws2lgs_register				= gws2lgs_base + 10;
const short gws2lgs_tester_ret				= gws2lgs_base + 11;
const short gws2lgs_user_enter_game         = gws2lgs_base + 12;
const short gws2lgs_gm_command              = gws2lgs_base + 13;
const short gws2lgs_charge_card             = gws2lgs_base + 14;
const short gws2lgs_query_point             = gws2lgs_base + 15;
const short gws2lgs_keep_online             = gws2lgs_base + 16;
const short gws2lgs_broadcast_to_all_region = gws2lgs_base + 17;
const short gws2lgs_broadcast_to_gameworld  = gws2lgs_base + 18;
const short gws2lgs_card_can_trade          = gws2lgs_base + 19;
const short gws2lgs_deduct                  = gws2lgs_base + 20;
const short gws2lgs_prededuct               = gws2lgs_base + 21;
const short gws2lgs_lock_account            = gws2lgs_base + 22;
const short gws2lgs_license_check			= gws2lgs_base + 23;
const short gws2lgs_license_check_rs		= gws2lgs_base + 24;
const short gws2lgs_user_change_block		= gws2lgs_base + 25;
const short gws2lgs_card_failed				= gws2lgs_base + 26;
const short gws2lgs_add_point				= gws2lgs_base + 27;
const short gws2lgs_add_account_time		= gws2lgs_base + 28;//tim.yang  VIP��

const long LOGIN_RET_SUCCESS				= 0;
const long LOGIN_RET_FAILED_UNKNOWN			= -1;
const long LOGIN_RET_FAILED_USER_ONLINE		= -2;
const long LOGIN_RET_FAILED_ACCOUNT_STOP	= -3;
const long LOGIN_RET_FAILED_ACCOUNT_PAUSE	= -4;
const long LOGIN_RET_FAILED_POT_NOT_ENOUGH	= -5;
const long LOGIN_RET_FAILED_PWD_WRONG		= -6;
const long LOGIN_RET_FAILED_NOT_FOUND		= -7;
const long LOGIN_RET_FAILED_SERVER_FULL     = -8;
const long LOGIN_RET_FAILED_SERVER_STOP     = -9;  // ������ά�����߹ر�״̬
const long LOGIN_RET_FAILED_NEW_CARD        = -10; // �������ֿ�
const long LOGIN_RET_FAILED_USERDEFINE      = -11; // �Զ������Ϣ��ʾ
const long LOGIN_RET_FAILED_PUBWIN_CENTER	= -12;//  [8/25/2009 tim.yang] ͬ��pubwinʧ��
const long LOGIN_RET_ACCOUNT_IS_LOCKER		= -13;//  [8/25/2009 tim.yang] ����ʺű���
const long LOGIN_RET_FAILED_ACCOUNT_WRONG5  = -14;//  �������������ʺ��������Ϊ�����û���Ϣ��ȫ�������ʺ�10����

const short LOGIN_RET_CODE_Unknown          = 100; // 
const short LOGIN_RET_CODE_ShutdownTime     = 101; // ����������׼���ر�״̬
const short LOGIN_RET_CODE_LoginPadlock     = 102; // ��������ֹ��ҽ���
const short LOGIN_RET_CODE_DataError        = 103; // ��ֵ����
const short LOGIN_RET_CODE_ChargeTimeout    = 104; // �Ʒѷ�������½��ʱ
const short LOGIN_RET_CODE_ActivingTimeout  = 105; // ���ʱ
const short LOGIN_RET_CODE_CreatingTimeout  = 106; // ������ɫ��ʱ
const short LOGIN_RET_CODE_CreatingFailed   = 107; // ������ɫʧ��
const short LOGIN_RET_CODE_ActivingFailed   = 108; // ����ʧ��
const short LOGIN_RET_CODE_NewCard          = 109; // ���ֿ�
const short LOGIN_RET_CODE_UserOnlined      = 110; // �û��Ѿ�����
const short LOGIN_RET_CODE_PotNotEnough     = 111; // ��������
const short LOGIN_RET_CODE_AccountStoped    = 112; // �ʺ�ֹͣ
const short LOGIN_RET_CODE_AccountPaused    = 113; // �ʺ���ͣ
const short LOGIN_RET_CODE_PasswordWrong    = 114; // �������
const short LOGIN_RET_CODE_DatabaseError    = 115; // ���ݿ����
const short LOGIN_RET_CODE_NotFound         = 116; // û���ҵ���ɫ
const short LOGIN_RET_CODE_GMPaused         = 117; // GM�ڷǷ������½

const char KICK_USER_UNKNOWN                = 0;
const char KICK_USER_MULTI_LOGIN            = 1;
const char KICK_USER_POT_NOT_ENOUGH         = 2;
const char KICK_USER_GM						= 3;
const char KICK_USER_USERDEFINE             = 4;


#endif