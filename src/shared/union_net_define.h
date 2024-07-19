#ifndef UNION_NET_DEFINE_H
#define UNION_NET_DEFINE_H


const short r2c_union					= 11000;
const short c2r_union					= 19000;
const short r2g_union_base 				= 27000;
const short g2r_union_base				= 30000;


const short c2r_union_create				= c2r_union + 101;			// ��������
const short c2r_union_dismiss_other			= c2r_union + 102;			// ����ĳ��
const short c2r_union_change_nickname		= c2r_union + 103;			// ���ƺ�
const short c2r_union_join					= c2r_union + 104;			// �����������
const short c2r_union_other_join_response	= c2r_union + 105;			// �Ա��˼��������ķ���
const short c2r_union_bulletin				= c2r_union + 106;			// ������Ϣ
const short c2r_union_appoint				= c2r_union + 110;			// ����ĳ�˹�ְ
const short c2r_union_invitewar				= c2r_union + 111;			// ����һ������ս
const short c2r_union_surrender				= c2r_union + 112;			// ����һ����Ͷ��
const short c2r_union_leave					= c2r_union + 113;			// ��������
const short c2r_feod_content_query			= c2r_union + 114;			// ��ѯ������֧��
const short c2r_feod_taxrate				= c2r_union + 115;			// ����˰�ʵ���
const short c2r_feod_feodbank_withdraw		= c2r_union + 121;			// �Ӳ��ع�����ȡ���
const short c2r_union_logo					= c2r_union + 122;			// �ϴ�������־
const short c2r_union_query_logo_data		= c2r_union + 123;			// Clientû��LOGO�ļ������������ļ�
const short c2r_union_update_level			= c2r_union + 124;			// ��������
const short c2r_union_player_levelup		= c2r_union + 125;			// ��ҽ�ɫ����

//const short c2r_union_create_vote			= c2r_union + 126;			//���һ��ͶƱ����
const short c2r_union_do_vote				= c2r_union + 127;			//����ͶƱ
const short c2r_union_log_query				= c2r_union + 128;			// ��ѯ�����������300����ʷ������¼
const short c2r_union_give_union			= c2r_union + 129;			// ת�ð���

const short c2r_union_Apply_join					= c2r_union + 130;			// �����������
const short c2r_union_Apply_join_Agree_Or_Not					= c2r_union + 131;			// �Ƿ�ͬ����������������

const short c2r_union_join_By_Name					= c2r_union + 132;			// ͨ�����������������

const short r2g_union_update_Contribution					= c2r_union + 133;			// ĳ��ҵİﹱ�ı���(��������)
const short c2r_union_set_icon					= c2r_union + 134;//�޸�ͼ��ͼ��

const short c2r_union_data_query			= c2r_union + 200;			// ��ѯ����������Ϣ

const short c2r_union_contribution			= c2r_union + 201;		//���ɹ���


const short r2c_union_create_result				= r2c_union + 101;			// ���������ķ���
const short r2c_union_dismiss_other_result		= r2c_union + 102;			// ����ĳ�˵ķ���
const short r2c_union_dismissed_by_other		= r2c_union + 103;			// �㱻������
const short r2c_union_change_nickname_result	= r2c_union + 104;			// �޸ĳƺŵķ���
//const short r2c_union_other_change_nickname		= r2c_union + 105;			// ���˻��˳ƺ�
const short r2c_union_other_join_apply			= r2c_union + 106;			// �����������
const short r2c_union_join_response				= r2c_union + 107;			// �������ķ���
const short r2c_union_appoint_result			= r2c_union + 110;			// ����ĳ�˹�ְ�ķ���
const short r2c_union_invitewar_result			= r2c_union + 111;			// ����һ������ս�ķ���
const short r2c_union_surrender_other_response	= r2c_union + 112;			// ��һ�����Ƿ�ͬ�����Ͷ��
const short r2c_union_leave_result				= r2c_union + 113;			// ���������ķ���
const short r2c_feod_content					= r2c_union + 114;			// ������֧��
const short r2c_feod_taxrate_result				= r2c_union + 115;			// ����˰�ʵ����ķ���
const short r2c_feod_feodbank_withdraw_result	= r2c_union + 116;			// �Ӳ��ع�����ȡ��ҵķ���
const short r2c_feod_data						= r2c_union + 117;			// ��������
const short r2c_union_logo_result				= r2c_union + 122;			// �ϴ�������־�ķ���
const short r2c_union_logo_data					= r2c_union + 123;			// Logo�ļ�����
const short r2c_union_update_level_result		= r2c_union + 124;			// ���������ķ���

//const short c2r_union_create_vote_result		= r2c_union + 125;			//���һ��ͶƱ����
const short r2c_union_new_vote					= r2c_union + 126;			//��һ����ͶƱ
//const short r2c_union_do_vote_result			= r2c_union + 127;			//����ͶƱ�Ľ��
const short r2c_union_log_query_result			= r2c_union + 128;			// ��ѯ�����������300����ʷ������¼�Ľ��

const short r2c_union_Apply_join					= r2c_union + 129;			// �����������

const short r2c_union_new_log					= r2c_union + 130;			//������һ���µ�Log
const short r2c_union_update_Contribution	    = r2c_union + 131;			//ˢ�°ﹱ
//const short r2c_union_update_Money	            = r2c_union + 132;			//ˢ�°����Ǯ��
const short r2c_union_contribution	            = r2c_union + 133;			//���˶԰������˹���
const short r2c_WarScores_Change	            = r2c_union + 134;			//���ɵ�ս�����ָı�
const short r2c_union_set_icon					= c2r_union + 135;//ͼ��ͼ���ѱ��޸�

// GameWorld�㲥��region����Ϣ
const short r2c_union_data_refresh				= r2c_union + 200;			// ˢ����������
const short r2c_union_citizen_join 				= r2c_union + 201;			// ���˼����˱�����
const short r2c_union_citizen_leave 			= r2c_union + 202;			// �����뿪�˱�����
const short r2c_union_citizen_online 			= r2c_union + 203;			// ����������������
const short r2c_union_citizen_offline 			= r2c_union + 204;			// ����������������
const short r2c_union_citizen_official_change	= r2c_union + 205;			// �����������ְ�䶯
const short r2c_union_disband					= r2c_union + 206;			// ������ɢ
const short r2c_union_new_bulletin				= r2c_union + 207;			// ���¹���
const short r2c_union_citizen_nickname_change	= r2c_union + 208;			// �����������ְ�䶯
const short r2c_union_icon_change				= r2c_union + 209;			// ��ҿɼ���������Ϣ���
const short r2c_union_score_change				= r2c_union + 210;			// �����������
const short r2c_union_levelup					= r2c_union + 211;			// ����������
const short g2r_feod_data						= r2c_union + 212;			// ��������
const short r2c_union_player_levelup			= r2c_union + 213;			// ��ҽ�ɫ����
const short g2r_set_union_icon_result			= r2c_union + 214;			//����ͼ��Ľ��

const short r2g_union_add_score					= r2g_union_base + 101;		// ������������
const short r2g_feod_new_income					= r2g_union_base + 102;		// ˰��
const short r2g_feod_change_union				= r2g_union_base + 103;		
const short r2g_WarScores_Change				= r2g_union_base + 104;		//ս�����ָı�


/* ERROR CODE (BYTE) */
#define UNION_ERROR_UNKNOWN					0
#define UNION_SUCCESS						1
#define UNION_PLAYER_HAVE_NO_UNION			2		// ��Ҳ������κ�����
#define UNION_PLAYER_HAVE_UNION				3		// ����Ѿ�����һ������
#define	UNION_UNION_CITIZEN_FULL			4		// ������������
#define UNION_NAME_INVALID					5		// �����������ֲ��Ϸ�
#define	UNION_CAPTION_LOCK_TIME				6		// 7��֮�ڲ�׼�ٽ��µ�
#define UNION_NOT_IN_SAME_FACTION			7		// ��ͬ��Ӫ

/* UNION MESSAGE (SHORT) */
#define UNION_MSG_NULL						0
#define UNION_MSG_CAPTAIN_DISBAND			1		// ����ɢ���Լ�������
#define	UNION_MSG_FEWCITIZEN_DISBAND		2		// ����̫��,�����Զ���ɢ
#define UNION_MSG_GM_DISBAND				3		// GM��ɢ������


#define FEOD_ERROR_UNKNOWN					0
#define FEOD_SUCCESS						1



/*	c2r_union_create
˵������������
������
����			����
DWORD			playerID
string 			name
string			enounce
*/

/*	c2r_union_dismiss_other
˵��������ĳ��
������
����			����
DWORD			unionID
DWORD			playerID
*/

/*	c2r_union_change_nickname
˵�������ƺ�
������
����			����
DWORD			unionID
DWORD			playerID
string 			newNickName
*/

/*	c2r_union_join
˵���������������
������
����			����
DWORD			unionID
DWORD			playerID			// ��˭����
*/

/*	c2r_union_other_join_response
˵�����Ա��˼��������ķ���
������
����			����
DWORD			unionID
DWORD			playerID			// ��˭�ķ���
BYTE			bAgree
*/

/*	c2r_union_bulletin
˵����������Ϣ
������
����			����
DWORD			unionID
string 			NewBulletin
*/

/*	c2r_union_appoint
˵��������ĳ�˹�ְ
������
����			����
DWORD			unionID
DWORD			playerID
BYTE			official
*/

/*	c2r_union_invitewar
˵��������һ������ս
������
����			����
DWORD			unionIDInvite
DWORD			unionIDInvited
*/

/*	c2r_union_surrender
˵��������һ����Ͷ��
������
����			����
DWORD			unionSurrender
DWORD			unionSurrendered
*/

/*	c2r_union_leave
˵������������
������
����			����
DWORD			unionID
DWORD			playerID		// �Ӱ���
*/

/*	c2r_feod_income_query
˵������ѯ������֧��
������
����			����
DWORD			unionID
*/

/*	c2r_feod_taxrate
˵��������˰�ʵ���
������
����			����
DWORD			unionID
float			taxrate
*/

/*	c2r_feod_unionbank_query
˵������ѯ���ع���
������
����			����
DWORD			unionID
*/

/*	c2r_feod_unionbank_withdraw
˵�����Ӳ��ع�����ȡ���
������
����			����
DWORD			unionID
long			money
*/

/*	c2r_union_data_query
˵������ѯ����������Ϣ
������
����			����
DWORD			unionID
*/




/*	r2c_union_create_result
˵�������������ķ���
������
����			����
BYTE 			errorCode
DWORD			unionID
*/

/*	r2c_union_dismiss_other_result
˵��������ĳ�˵ķ���
������
����			����
BYTE 			errorCode
DWORD			playerID
*/

/*	r2c_union_dismissed_by_other
˵�����㱻������
������
����			����
DWORD			playerID				// ��˭������
string			unionName
*/

/*	r2c_union_change_nickname_result
˵�����ĳƺŵķ���
������
����			����
BYTE 			errorCode
DWORD			playerID
string			newNickname
*/

/*	r2c_union_other_join_apply
˵���������������
������
����			����
DWORD 			playerID
string			playerName
*/

/*	r2c_union_join_response
˵�����������ķ���
������
����			����
BYTE 			errorCode
DWORD			playerID
DWORD			unionID
string			unionName
string			playerName
*/

/*	r2c_union_appoint_result
˵��������ĳ�˹�ְ�ķ���
������
����			����
BYTE 			errorCode
*/

/*	r2c_union_invitewar_result
˵��������һ������ս�ķ���
������
����			����
BYTE 			errorCode
*/

/*	r2c_union_surrender_other_response
˵������һ�����Ƿ�ͬ�����Ͷ��
������
����			����
BYTE 			bAgree
*/

/*	r2c_union_leave_result
˵�������������ķ���
������
����			����
BYTE 			errorCode
*/

/*	r2c_feod_income_content
˵����������֧��
������
����			����

*/

/*	r2c_feod_taxrate_result
˵��������˰�ʵ����ķ���
������
����			����
BYTE 			errorCode
*/

/*	r2c_feod_unionbank_content
˵�������ع���
������
����			����
long 			money
*/

/*	r2c_feod_unionbank_withdraw_result
˵�����Ӳ��ع�����ȡ��ҵķ���
������
����			����
BYTE 			errorCode
*/

/*	r2c_union_data_refresh
˵����ˢ����������
������
����			����
SUnionData		unionData
*/

/*	r2c_union_citizen_join
˵�������˼����˱�����
������
����			����
SUnionCitizen	citizen
*/

/*	r2c_union_citizen_leave
˵���������뿪�˱�����
������
����			����
DWORD			playerID
*/

/*	r2c_union_citizen_online
˵��������������������
������
����			����
DWORD			playerID
*/

/*	r2c_union_citizen_offline
˵��������������������
������
����			����
DWORD			playerID
*/

/*	r2c_union_citizen_official_change
˵���������������ְ�䶯
������
����			����
DWORD			playerID
BYTE			official
*/

/*	r2c_union_disband
˵����������ɢ
������
����			����
string			UnionName				��������
short 			reason
*/

/*	r2c_union_new_bulletin
˵�������¹���
������
����			����
string			NewBulletin
*/






#endif//UNION_NET_DEFINE_H
