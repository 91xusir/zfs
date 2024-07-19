//********************************************************************
//	created:	2010.05.31
//	filename: 	netcommand_union.h
//	author:		ldr123
//	purpose:	��Ϸ��,����ϵͳ�������
//  other:		version 1.0
//********************************************************************

#ifndef NET_COMMAND_UNION_H
#define NET_COMMAND_UNION_H

//�����������
bool UnionCmdProcess_client(CG_CmdPacket *cmd);

//�����������(��δ��)
bool FeodCmdProcess_client(CG_CmdPacket *cmd);

//�������ɷ���
bool net_r2c_union_create_result(CG_CmdPacket *pPacket);

//���������û�����
bool net_r2c_union_dismiss_other_result(CG_CmdPacket *pPacket);

//����������
bool net_r2c_union_dismissed_by_other(CG_CmdPacket *pPacket);

//���ĳƺ�(δ�õ�)
bool net_r2c_union_change_nickname_result(CG_CmdPacket *pPacket);

//��������������
bool net_r2c_union_other_join_apply(CG_CmdPacket *pPacket);

//������˼������
bool net_r2c_union_join_response(CG_CmdPacket *pPacket);

//������ְ�ɹ�
bool net_r2c_union_appoint_result(CG_CmdPacket *pPacket);

//����һ������ս�ķ���(ò������)
bool net_r2c_union_invitewar_result(CG_CmdPacket *pPacket);

//Ͷ��(����)
bool net_r2c_union_surrender_other_response(CG_CmdPacket *pPacket);

//���Ѿ�������******����
bool net_r2c_union_leave_result(CG_CmdPacket *pPacket);

//��������ˢ��
bool net_r2c_union_data_refresh(CG_CmdPacket *pPacket);

//���˼������
bool net_r2c_union_citizen_join(CG_CmdPacket *pPacket);

//�����뿪����
bool net_r2c_union_citizen_leave(CG_CmdPacket *pPacket);

//��������������
bool net_r2c_union_citizen_online(CG_CmdPacket *pPacket);

//��������������
bool net_r2c_union_citizen_offline(CG_CmdPacket *pPacket);

//���������˹�ְ�ı�
bool net_r2c_union_citizen_official_change(CG_CmdPacket *pPacket);

//���ɽ�ɢ
bool net_r2c_union_disband(CG_CmdPacket *pPacket);

//�¹���
bool net_r2c_union_new_bulletin(CG_CmdPacket *pPacket);

//�ϴ����ɱ�־ͼƬ�ķ���
bool net_r2c_union_logo_result(CG_CmdPacket *pPacket);

//��ȡ���ɱ�־ͼƬ
bool net_r2c_union_logo_data(CG_CmdPacket *pPacket);

//����ͼ�ڸ���
bool net_r2c_union_icon_change(CG_CmdPacket *pPacket);

//����־
bool net_r2c_union_new_log(CG_CmdPacket *pPacket);

//������־
bool net_r2c_union_log_query_result(CG_CmdPacket *pPacket);

//ˢ�°ﹱ
bool net_r2c_union_update_Contribution(CG_CmdPacket *pPacket);

//�ﹱ����
bool net_r2c_union_contribution_result(CG_CmdPacket *pPacket);

//���ɵ�ս�����ָı�
bool net_r2c_WarScores_Change(CG_CmdPacket *pPacket);

//ͼ��ͼ���ѱ��޸�
bool net_r2c_union_set_icon(CG_CmdPacket *pPacket);

//�������û�����
bool net_r2c_union_player_levelup(CG_CmdPacket *pPacket);

//���ɻ���(δ�õ�)
bool net_r2c_union_score_change(CG_CmdPacket *pPacket);

//��������(δ�õ�)
bool net_r2c_union_update_level_result(CG_CmdPacket *pPacket);

//��������(δ�õ�)
bool net_r2c_union_levelup(CG_CmdPacket *pPacket);

bool net_r2c_union_citizen_nickname_change(CG_CmdPacket *pPacket);

//����(����)
bool net_r2c_feod_content(CG_CmdPacket *pPacket);

//����˰��(����)
bool net_r2c_feod_taxrate_result(CG_CmdPacket *pPacket);

bool net_r2c_feod_feodbank_withdraw_result(CG_CmdPacket *pPacket);

bool net_r2c_feod_data(CG_CmdPacket *pPacket);

class CUnionNetHelper
{
public:
	//��������
	static bool NetSend_c2r_union_create(DWORD npcID, char* name, char* enounce);

	//��ɢ����
	static bool NetSend_c2r_union_disband(DWORD unionID, DWORD playerID);

	//�����Ա
	static bool NetSend_c2r_union_dismiss_other(DWORD unionID, DWORD playerID);

	//����ĳ�˼���
	static bool NetSend_c2r_union_join(DWORD unionID, DWORD playerApplyTo);

	//������������ĳ�˼���
	static bool NetSend_c2r_union_join(DWORD unionID, char *pName);

	//�Ƿ�ȷ�ϼ���
	static bool NetSend_c2r_union_other_join_response(DWORD unionID, DWORD playerApply, BYTE bAgree);

	//���淢��
	static bool NetSend_c2r_union_bulletin(DWORD unionID, char* bulletin);

	//����ְλ
	static bool NetSend_c2r_union_appoint(DWORD unionID, DWORD playerID, BYTE official);

	//�뿪����
	static bool NetSend_c2r_union_leave(DWORD unionID, DWORD playerID);

	//��ѯ������Ϣ
	static bool NetSend_c2r_union_data_query(DWORD unionID);

	//�ϴ�ͼ��
	static bool NetSend_c2r_union_logo(char* data, int dataSize);

	//����ͼ������
	static bool NetSend_c2r_union_query_logo_data(char* FileName);

	//��ȡ��־
	static bool NetSend_c2r_union_query_log(DWORD unionID);

	//���ɹ���
	static bool NetSend_c2r_union_contribution(DWORD unionID, DWORD nMoney, DWORD nItem1=0, DWORD nItem2=0, DWORD nItem3=0, DWORD nItem4=0);

	//����ͼ��
	static bool NetSend_c2r_union_totem(DWORD unionID, DWORD picID);


	static bool NetSend_c2r_union_change_nickname(DWORD unionID, DWORD playerID, char* newNickname);
	static bool NetSend_c2r_union_invitewar(DWORD unionInvite, DWORD unionInvited);
	static bool NetSend_c2r_union_surrender(DWORD unionSurrender, DWORD unionSurrendered);
	static bool NetSend_c2r_feod_content_query(DWORD npcID, char* feodName);
	static bool NetSend_c2r_feod_taxrate(DWORD npcID, char* feodName, short nRate);
	static bool NetSend_c2r_feod_feodbank_withdraw(DWORD npcID, char* feodName, long money);
};

string GetFeodName(const string& NameInDB);
#endif 