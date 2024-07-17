//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#ifndef CASTLEWAR_NET_DEFINE_H
#define CASTLEWAR_NET_DEFINE_H

const short c2r_castlewar_cmd_base = 10000;
const short r2c_castlewar_cmd_base = 15000;
const short r2g_castlewar_cmd_base = 20000;
const short g2r_castlewar_cmd_base = 25000;

//--------------------
//gc 2 region��Ϣ
//--------------------
const short c2r_castlewar_reg_attack		= c2r_castlewar_cmd_base + 111;//�������ע��
const short c2r_castlewar_reg_attack_cancel	= c2r_castlewar_cmd_base + 112;//�������ȡ��ע��
const short c2r_castlewar_reg_defend		= c2r_castlewar_cmd_base + 113;//Э���س����ע��
const short c2r_castlewar_reg_defend_cancel	= c2r_castlewar_cmd_base + 114;//Э���س����ȡ��ע��
const short c2r_castlewar_allow_bund		= c2r_castlewar_cmd_base + 115;//�س������׼���
const short c2r_castlewar_select_time		= c2r_castlewar_cmd_base + 116;//�س����ѡ��ʱ��
const short c2r_castlewar_view_info			= c2r_castlewar_cmd_base + 117;//��Ҳ鿴������Ϣ
const short c2r_castlewar_insert_flag		= c2r_castlewar_cmd_base + 118;//��������
const short c2r_castlewar_animal_killed		= c2r_castlewar_cmd_base + 119;//�����ػ��ޱ�ɱ��
const short c2r_castlewar_apply_flag		= c2r_castlewar_cmd_base + 120;//��������
const short c2r_castlewar_flagnpc_show		= c2r_castlewar_cmd_base + 121;//������ʾ

//--------------------
//region 2 gc��Ϣ
//--------------------
const short r2c_castlewar_war_prepare			= r2c_castlewar_cmd_base + 110;//֪ͨ����ս��׼����5����ǰ��
const short r2c_castlewar_war_start				= r2c_castlewar_cmd_base + 111;//֪ͨ�����ڳ�ս״̬
const short r2c_castlewar_war_pause				= r2c_castlewar_cmd_base + 112;//֪ͨ�����ڳ�ս״̬
const short r2c_castlewar_war_end				= r2c_castlewar_cmd_base + 113;//֪ͨ�����ڳ�ս����
const short r2c_castlewar_have_apply			= r2c_castlewar_cmd_base + 114;//�յ�Э�����󣨽�����
const short r2c_castlewar_have_attacker			= r2c_castlewar_cmd_base + 115;//�еǼǹ��ǵ����
const short r2c_castlewar_reg_attack_rep		= r2c_castlewar_cmd_base + 116;//�������ע����Ӧ
const short r2c_castlewar_reg_attack_cancel_rep	= r2c_castlewar_cmd_base + 117;//�������ע����Ӧ
const short r2c_castlewar_reg_defend_rep		= r2c_castlewar_cmd_base + 118;//Э���س����ע����Ӧ
const short r2c_castlewar_reg_defend_cancel_rep	= r2c_castlewar_cmd_base + 119;//Э���س����ע����Ӧ
const short r2c_castlewar_allow_bund_rep		= r2c_castlewar_cmd_base + 120;//�س������׼�����Ӧ
const short r2c_castlewar_select_time_rep		= r2c_castlewar_cmd_base + 121;//�س����ѡ��ʱ����Ӧ
const short r2c_castlewar_view_info_rep			= r2c_castlewar_cmd_base + 122;//��Ҳ鿴������Ϣ��Ӧ
const short r2c_castlewar_insert_flag_rep		= r2c_castlewar_cmd_base + 123;//����������Ӧ
const short r2c_castlewar_animal_killed_rep		= r2c_castlewar_cmd_base + 124;//�����ػ��ޱ�ɱ����Ӧ
const short r2c_castlewar_refresh				= r2c_castlewar_cmd_base + 125;//���³�ս����
const short r2c_castlewar_user_data_chg			= r2c_castlewar_cmd_base + 126;//����������ݸ���
const short r2c_castlewar_is_transported		= r2c_castlewar_cmd_base + 127;//��ҵ�¼���ڳ�ս����
const short r2c_castlewar_apply_flag_rep		= r2c_castlewar_cmd_base + 128;//����������Ӧ
const short r2c_castlewar_flagnpc_created		= r2c_castlewar_cmd_base + 129;//����npc����������

//--------------------
//region 2 gw��Ϣ
//--------------------
const short r2g_castlewar_update_data			= r2g_castlewar_cmd_base + 111;//ͬ������
const short r2g_castlewar_war_prepare			= r2g_castlewar_cmd_base + 112;//׼��
const short r2g_castlewar_war_start				= r2g_castlewar_cmd_base + 113;//��ʼ
const short r2g_castlewar_war_pause				= r2g_castlewar_cmd_base + 114;//��ֹ
const short r2g_castlewar_war_end				= r2g_castlewar_cmd_base + 115;//����
const short r2g_castlewar_time_set				= r2g_castlewar_cmd_base + 116;//����ʱ������
const short r2g_castlewar_reload_time			= r2g_castlewar_cmd_base + 117;//������ʱ������

//--------------------
//gw 2 region��Ϣ
//--------------------
const short g2r_castlewar_data_init				= g2r_castlewar_cmd_base + 111;//��ʼ��
const short g2r_castlewar_user_online			= g2r_castlewar_cmd_base + 112;//�������
const short g2r_castlewar_war_prepare_rep		= g2r_castlewar_cmd_base + 113;//׼����Ӧ
const short g2r_castlewar_war_start_rep			= g2r_castlewar_cmd_base + 114;//��ʼ��Ӧ
const short g2r_castlewar_war_pause_rep			= g2r_castlewar_cmd_base + 115;//��ֹ��Ӧ
const short g2r_castlewar_war_end_rep			= g2r_castlewar_cmd_base + 116;//������Ӧ
const short g2r_castlewar_data_update			= g2r_castlewar_cmd_base + 117;//ͬ����Ӧ
const short g2r_castlewar_reload_time_rep		= g2r_castlewar_cmd_base + 118;//������ʱ����Ӧ

//---------------------------------------------------------
//Error Code(BYTE)
//---------------------------------------------------------
//------------
//ע�����
//------------
#define ERR_NONE_CWAR				0	//�޴���
#define ERR_NOT_VASSAL_CWAR			1	//�������
#define ERR_DIFF_FACTION_CWAR		2	//����ͬһ��Ӫ
#define ERR_WAR_PREPARE_CWAR		3	//�óǳش���׼����ս״̬
#define ERR_WARFARE_STATE_CWAR		4	//�óǳش��ڽ�ս״̬
#define ERR_REPEAT_REG_CWAR			5	//�ظ�ע��
#define ERR_NO_REG_CWAR				6	//δע��
#define ERR_NPC_OCCUPY_STATE_CWAR	7	//npcռ��
#define ERR_MASTER_CWAR				8	//��������ע��
#define ERR_MASTER_CANCEL_CWAR		9	//��������ȡ��
#define ERR_REPEAT_ALLOW_CWAR		10	//�ظ���׼
#define ERR_NUM_OUTRANGE_CWAR		11	//��������8���������ظ���׼
#define ERR_TIME_OUTRANGE_CWAR		12	//��ʱ
#define ERR_NO_REG_INFO				13	//û��ע���
#define ERR_FULL_CWAR				14	//��������
#define ERR_LEVEL_LOWER_CWAR		15	//���𲻹�
//------------
//���������
//------------
#define ERR_HAVE_FLAG_ROUND		1	//������������
#define ERR_TIME_WRONG			2	//������ʱ�䲻��
#define ERR_PLACE_WRONG			3	//������ص㲻��
#define ERR_MULTI_FLAG			4	//�������
#define ERR_UNKNOWN_FLAG		5	//

//-------------------------------
//�ͻ�����Ϣ����˵��
//-------------------------------

//------------------------------------------------------
//-c2r_castlewar_reg_attack
//-���壺�������ע��
//-������DWORD	npc_id		ע��npcID
//------------------------------------------------------

//------------------------------------------------------
//-c2r_castlewar_reg_defend
//-���壺Э���س����ע��
//-������DWORD	npc_id		ע��npcID
//------------------------------------------------------

//------------------------------------------------------
//-c2r_castlewar_allow_bund
//-���壺�س������׼���
//-������DWORD	union_id		�����س����ID
//-		 string	feod_name		���سǳ�����
//-		 BYTE	allow_info		�Ƿ�������Э��
//------------------------------------------------------

//------------------------------------------------------
//-c2r_castlewar_select_time
//-���壺�س����ѡ���´ι���ʱ��
//-������DWORD	union_id		�س����ID
//-		 string	feod_name		���سǳ�����
//-		 DWORD	time			��ѡ���ʱ��
//------------------------------------------------------

//------------------------------------------------------
//-c2r_castlewar_view_info
//-���壺��Ҳ鿴������Ϣ
//-������DWORD	user_id			���ID
//-		 string	feod_name		�ǳ�����
//------------------------------------------------------

//------------------------------------------------------
//-c2r_castlewar_insert_flag
//-���壺��������
//-������DWORD	union_id		���ID
//-		 string	feod_name		�ǳ�����
//------------------------------------------------------
#endif //CASTLEWAR_NET_DEFINE_H

