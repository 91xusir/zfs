//********************************************************************
//	created:	2010.05.31
//	filename: 	netcommand_union.h
//	author:		ldr123
//	purpose:	游戏中,帮派系统网络操作
//  other:		version 1.0
//********************************************************************

#ifndef NET_COMMAND_UNION_H
#define NET_COMMAND_UNION_H

//帮派网络解析
bool UnionCmdProcess_client(CG_CmdPacket *cmd);

//采邑网络解析(暂未用)
bool FeodCmdProcess_client(CG_CmdPacket *cmd);

//创建帮派返回
bool net_r2c_union_create_result(CG_CmdPacket *pPacket);

//驱逐其他用户返回
bool net_r2c_union_dismiss_other_result(CG_CmdPacket *pPacket);

//被被人驱逐
bool net_r2c_union_dismissed_by_other(CG_CmdPacket *pPacket);

//更改称号(未用到)
bool net_r2c_union_change_nickname_result(CG_CmdPacket *pPacket);

//别人请你加入帮派
bool net_r2c_union_other_join_apply(CG_CmdPacket *pPacket);

//请求别人加入帮派
bool net_r2c_union_join_response(CG_CmdPacket *pPacket);

//任命官职成功
bool net_r2c_union_appoint_result(CG_CmdPacket *pPacket);

//向另一帮派宣战的反馈(貌似无用)
bool net_r2c_union_invitewar_result(CG_CmdPacket *pPacket);

//投降(无用)
bool net_r2c_union_surrender_other_response(CG_CmdPacket *pPacket);

//您已经脱离了******帮派
bool net_r2c_union_leave_result(CG_CmdPacket *pPacket);

//帮派数据刷新
bool net_r2c_union_data_refresh(CG_CmdPacket *pPacket);

//有人加入帮派
bool net_r2c_union_citizen_join(CG_CmdPacket *pPacket);

//有人离开帮派
bool net_r2c_union_citizen_leave(CG_CmdPacket *pPacket);

//帮派里有人上线
bool net_r2c_union_citizen_online(CG_CmdPacket *pPacket);

//帮派里有人下线
bool net_r2c_union_citizen_offline(CG_CmdPacket *pPacket);

//帮派里有人官职改变
bool net_r2c_union_citizen_official_change(CG_CmdPacket *pPacket);

//帮派解散
bool net_r2c_union_disband(CG_CmdPacket *pPacket);

//新公告
bool net_r2c_union_new_bulletin(CG_CmdPacket *pPacket);

//上传帮派标志图片的反馈
bool net_r2c_union_logo_result(CG_CmdPacket *pPacket);

//获取帮派标志图片
bool net_r2c_union_logo_data(CG_CmdPacket *pPacket);

//帮派图腾更改
bool net_r2c_union_icon_change(CG_CmdPacket *pPacket);

//新日志
bool net_r2c_union_new_log(CG_CmdPacket *pPacket);

//更新日志
bool net_r2c_union_log_query_result(CG_CmdPacket *pPacket);

//刷新帮贡
bool net_r2c_union_update_Contribution(CG_CmdPacket *pPacket);

//帮贡返回
bool net_r2c_union_contribution_result(CG_CmdPacket *pPacket);

//帮派的战力评分改变
bool net_r2c_WarScores_Change(CG_CmdPacket *pPacket);

//图腾图标已被修改
bool net_r2c_union_set_icon(CG_CmdPacket *pPacket);

//帮派内用户升级
bool net_r2c_union_player_levelup(CG_CmdPacket *pPacket);

//帮派积分(未用到)
bool net_r2c_union_score_change(CG_CmdPacket *pPacket);

//帮派升级(未用到)
bool net_r2c_union_update_level_result(CG_CmdPacket *pPacket);

//帮派升级(未用到)
bool net_r2c_union_levelup(CG_CmdPacket *pPacket);

bool net_r2c_union_citizen_nickname_change(CG_CmdPacket *pPacket);

//采邑(无用)
bool net_r2c_feod_content(CG_CmdPacket *pPacket);

//采邑税率(无用)
bool net_r2c_feod_taxrate_result(CG_CmdPacket *pPacket);

bool net_r2c_feod_feodbank_withdraw_result(CG_CmdPacket *pPacket);

bool net_r2c_feod_data(CG_CmdPacket *pPacket);

class CUnionNetHelper
{
public:
	//创建帮派
	static bool NetSend_c2r_union_create(DWORD npcID, char* name, char* enounce);

	//解散帮派
	static bool NetSend_c2r_union_disband(DWORD unionID, DWORD playerID);

	//驱逐成员
	static bool NetSend_c2r_union_dismiss_other(DWORD unionID, DWORD playerID);

	//邀请某人加入
	static bool NetSend_c2r_union_join(DWORD unionID, DWORD playerApplyTo);

	//根据名字邀请某人加入
	static bool NetSend_c2r_union_join(DWORD unionID, char *pName);

	//是否确认加入
	static bool NetSend_c2r_union_other_join_response(DWORD unionID, DWORD playerApply, BYTE bAgree);

	//公告发布
	static bool NetSend_c2r_union_bulletin(DWORD unionID, char* bulletin);

	//任命职位
	static bool NetSend_c2r_union_appoint(DWORD unionID, DWORD playerID, BYTE official);

	//离开帮派
	static bool NetSend_c2r_union_leave(DWORD unionID, DWORD playerID);

	//查询帮派信息
	static bool NetSend_c2r_union_data_query(DWORD unionID);

	//上传图腾
	static bool NetSend_c2r_union_logo(char* data, int dataSize);

	//下载图腾数据
	static bool NetSend_c2r_union_query_logo_data(char* FileName);

	//获取日志
	static bool NetSend_c2r_union_query_log(DWORD unionID);

	//帮派贡献
	static bool NetSend_c2r_union_contribution(DWORD unionID, DWORD nMoney, DWORD nItem1=0, DWORD nItem2=0, DWORD nItem3=0, DWORD nItem4=0);

	//更改图腾
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