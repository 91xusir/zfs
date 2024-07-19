#ifndef UNION_NET_DEFINE_H
#define UNION_NET_DEFINE_H


const short r2c_union					= 11000;
const short c2r_union					= 19000;
const short r2g_union_base 				= 27000;
const short g2r_union_base				= 30000;


const short c2r_union_create				= c2r_union + 101;			// 创建诸侯国
const short c2r_union_dismiss_other			= c2r_union + 102;			// 驱除某人
const short c2r_union_change_nickname		= c2r_union + 103;			// 换称号
const short c2r_union_join					= c2r_union + 104;			// 申请加入诸侯国
const short c2r_union_other_join_response	= c2r_union + 105;			// 对别人加入诸侯国的反馈
const short c2r_union_bulletin				= c2r_union + 106;			// 公告信息
const short c2r_union_appoint				= c2r_union + 110;			// 任命某人官职
const short c2r_union_invitewar				= c2r_union + 111;			// 向另一诸侯国宣战
const short c2r_union_surrender				= c2r_union + 112;			// 向另一诸侯国投降
const short c2r_union_leave					= c2r_union + 113;			// 脱离诸侯国
const short c2r_feod_content_query			= c2r_union + 114;			// 查询采邑收支表
const short c2r_feod_taxrate				= c2r_union + 115;			// 采邑税率调整
const short c2r_feod_feodbank_withdraw		= c2r_union + 121;			// 从采邑国库提取金币
const short c2r_union_logo					= c2r_union + 122;			// 上传诸侯国标志
const short c2r_union_query_logo_data		= c2r_union + 123;			// Client没有LOGO文件，请求下载文件
const short c2r_union_update_level			= c2r_union + 124;			// 升级诸侯国
const short c2r_union_player_levelup		= c2r_union + 125;			// 玩家角色升级

//const short c2r_union_create_vote			= c2r_union + 126;			//提出一个投票罢免
const short c2r_union_do_vote				= c2r_union + 127;			//进行投票
const short c2r_union_log_query				= c2r_union + 128;			// 查询本诸侯国的最近300条历史操作记录
const short c2r_union_give_union			= c2r_union + 129;			// 转让帮主

const short c2r_union_Apply_join					= c2r_union + 130;			// 申请加入诸侯国
const short c2r_union_Apply_join_Agree_Or_Not					= c2r_union + 131;			// 是否同意别人申请加入诸侯国

const short c2r_union_join_By_Name					= c2r_union + 132;			// 通过名字邀请加入诸侯国

const short r2g_union_update_Contribution					= c2r_union + 133;			// 某玩家的帮贡改变了(帮派任务)
const short c2r_union_set_icon					= c2r_union + 134;//修改图腾图标

const short c2r_union_data_query			= c2r_union + 200;			// 查询本诸侯国的信息

const short c2r_union_contribution			= c2r_union + 201;		//帮派贡献


const short r2c_union_create_result				= r2c_union + 101;			// 创建诸侯国的反馈
const short r2c_union_dismiss_other_result		= r2c_union + 102;			// 驱除某人的反馈
const short r2c_union_dismissed_by_other		= r2c_union + 103;			// 你被驱除了
const short r2c_union_change_nickname_result	= r2c_union + 104;			// 修改称号的反馈
//const short r2c_union_other_change_nickname		= r2c_union + 105;			// 此人换了称号
const short r2c_union_other_join_apply			= r2c_union + 106;			// 有人申请加入
const short r2c_union_join_response				= r2c_union + 107;			// 申请加入的反馈
const short r2c_union_appoint_result			= r2c_union + 110;			// 任命某人官职的反馈
const short r2c_union_invitewar_result			= r2c_union + 111;			// 向另一诸侯国宣战的反馈
const short r2c_union_surrender_other_response	= r2c_union + 112;			// 另一诸侯国是否同意你的投降
const short r2c_union_leave_result				= r2c_union + 113;			// 脱离诸侯国的反馈
const short r2c_feod_content					= r2c_union + 114;			// 采邑收支表
const short r2c_feod_taxrate_result				= r2c_union + 115;			// 采邑税率调整的反馈
const short r2c_feod_feodbank_withdraw_result	= r2c_union + 116;			// 从采邑国库提取金币的反馈
const short r2c_feod_data						= r2c_union + 117;			// 采邑数据
const short r2c_union_logo_result				= r2c_union + 122;			// 上传诸侯国标志的反馈
const short r2c_union_logo_data					= r2c_union + 123;			// Logo文件内容
const short r2c_union_update_level_result		= r2c_union + 124;			// 升级诸侯国的反馈

//const short c2r_union_create_vote_result		= r2c_union + 125;			//提出一个投票罢免
const short r2c_union_new_vote					= r2c_union + 126;			//有一个新投票
//const short r2c_union_do_vote_result			= r2c_union + 127;			//进行投票的结果
const short r2c_union_log_query_result			= r2c_union + 128;			// 查询本诸侯国的最近300条历史操作记录的结果

const short r2c_union_Apply_join					= r2c_union + 129;			// 申请加入诸侯国

const short r2c_union_new_log					= r2c_union + 130;			//产生了一条新的Log
const short r2c_union_update_Contribution	    = r2c_union + 131;			//刷新帮贡
//const short r2c_union_update_Money	            = r2c_union + 132;			//刷新帮里的钱物
const short r2c_union_contribution	            = r2c_union + 133;			//有人对帮派做了贡献
const short r2c_WarScores_Change	            = r2c_union + 134;			//帮派的战力评分改变
const short r2c_union_set_icon					= c2r_union + 135;//图腾图标已被修改

// GameWorld广播给region的消息
const short r2c_union_data_refresh				= r2c_union + 200;			// 刷新诸侯国数据
const short r2c_union_citizen_join 				= r2c_union + 201;			// 有人加入了本诸侯国
const short r2c_union_citizen_leave 			= r2c_union + 202;			// 有人离开了本诸侯国
const short r2c_union_citizen_online 			= r2c_union + 203;			// 本诸侯国国民上线啦
const short r2c_union_citizen_offline 			= r2c_union + 204;			// 本诸侯国国民离线啦
const short r2c_union_citizen_official_change	= r2c_union + 205;			// 本诸侯国国民官职变动
const short r2c_union_disband					= r2c_union + 206;			// 诸侯国解散
const short r2c_union_new_bulletin				= r2c_union + 207;			// 有新公告
const short r2c_union_citizen_nickname_change	= r2c_union + 208;			// 本诸侯国国民官职变动
const short r2c_union_icon_change				= r2c_union + 209;			// 玩家可见的诸侯国信息变更
const short r2c_union_score_change				= r2c_union + 210;			// 诸侯国分数变更
const short r2c_union_levelup					= r2c_union + 211;			// 诸侯国升级了
const short g2r_feod_data						= r2c_union + 212;			// 采邑数据
const short r2c_union_player_levelup			= r2c_union + 213;			// 玩家角色升级
const short g2r_set_union_icon_result			= r2c_union + 214;			//更换图标的结果

const short r2g_union_add_score					= r2g_union_base + 101;		// 增加诸侯国分数
const short r2g_feod_new_income					= r2g_union_base + 102;		// 税收
const short r2g_feod_change_union				= r2g_union_base + 103;		
const short r2g_WarScores_Change				= r2g_union_base + 104;		//战力评分改变


/* ERROR CODE (BYTE) */
#define UNION_ERROR_UNKNOWN					0
#define UNION_SUCCESS						1
#define UNION_PLAYER_HAVE_NO_UNION			2		// 玩家不属于任何诸侯国
#define UNION_PLAYER_HAVE_UNION				3		// 玩家已经属于一个诸侯国
#define	UNION_UNION_CITIZEN_FULL			4		// 诸侯国人数已满
#define UNION_NAME_INVALID					5		// 创建诸侯国名字不合法
#define	UNION_CAPTION_LOCK_TIME				6		// 7天之内不准再建新的
#define UNION_NOT_IN_SAME_FACTION			7		// 不同阵营

/* UNION MESSAGE (SHORT) */
#define UNION_MSG_NULL						0
#define UNION_MSG_CAPTAIN_DISBAND			1		// 诸侯解散了自己的诸侯国
#define	UNION_MSG_FEWCITIZEN_DISBAND		2		// 人数太少,诸侯国自动解散
#define UNION_MSG_GM_DISBAND				3		// GM解散了诸侯国


#define FEOD_ERROR_UNKNOWN					0
#define FEOD_SUCCESS						1



/*	c2r_union_create
说明：创建诸侯国
参数：
类型			含义
DWORD			playerID
string 			name
string			enounce
*/

/*	c2r_union_dismiss_other
说明：驱除某人
参数：
类型			含义
DWORD			unionID
DWORD			playerID
*/

/*	c2r_union_change_nickname
说明：换称号
参数：
类型			含义
DWORD			unionID
DWORD			playerID
string 			newNickName
*/

/*	c2r_union_join
说明：申请加入诸侯国
参数：
类型			含义
DWORD			unionID
DWORD			playerID			// 向谁申请
*/

/*	c2r_union_other_join_response
说明：对别人加入诸侯国的反馈
参数：
类型			含义
DWORD			unionID
DWORD			playerID			// 对谁的反馈
BYTE			bAgree
*/

/*	c2r_union_bulletin
说明：公告信息
参数：
类型			含义
DWORD			unionID
string 			NewBulletin
*/

/*	c2r_union_appoint
说明：任命某人官职
参数：
类型			含义
DWORD			unionID
DWORD			playerID
BYTE			official
*/

/*	c2r_union_invitewar
说明：向另一诸侯国宣战
参数：
类型			含义
DWORD			unionIDInvite
DWORD			unionIDInvited
*/

/*	c2r_union_surrender
说明：向另一诸侯国投降
参数：
类型			含义
DWORD			unionSurrender
DWORD			unionSurrendered
*/

/*	c2r_union_leave
说明：脱离诸侯国
参数：
类型			含义
DWORD			unionID
DWORD			playerID		// 接班人
*/

/*	c2r_feod_income_query
说明：查询采邑收支表
参数：
类型			含义
DWORD			unionID
*/

/*	c2r_feod_taxrate
说明：采邑税率调整
参数：
类型			含义
DWORD			unionID
float			taxrate
*/

/*	c2r_feod_unionbank_query
说明：查询采邑国库
参数：
类型			含义
DWORD			unionID
*/

/*	c2r_feod_unionbank_withdraw
说明：从采邑国库提取金币
参数：
类型			含义
DWORD			unionID
long			money
*/

/*	c2r_union_data_query
说明：查询本诸侯国的信息
参数：
类型			含义
DWORD			unionID
*/




/*	r2c_union_create_result
说明：创建诸侯国的反馈
参数：
类型			含义
BYTE 			errorCode
DWORD			unionID
*/

/*	r2c_union_dismiss_other_result
说明：驱除某人的反馈
参数：
类型			含义
BYTE 			errorCode
DWORD			playerID
*/

/*	r2c_union_dismissed_by_other
说明：你被驱除了
参数：
类型			含义
DWORD			playerID				// 被谁驱除的
string			unionName
*/

/*	r2c_union_change_nickname_result
说明：改称号的反馈
参数：
类型			含义
BYTE 			errorCode
DWORD			playerID
string			newNickname
*/

/*	r2c_union_other_join_apply
说明：有人申请加入
参数：
类型			含义
DWORD 			playerID
string			playerName
*/

/*	r2c_union_join_response
说明：申请加入的反馈
参数：
类型			含义
BYTE 			errorCode
DWORD			playerID
DWORD			unionID
string			unionName
string			playerName
*/

/*	r2c_union_appoint_result
说明：任命某人官职的反馈
参数：
类型			含义
BYTE 			errorCode
*/

/*	r2c_union_invitewar_result
说明：向另一诸侯国宣战的反馈
参数：
类型			含义
BYTE 			errorCode
*/

/*	r2c_union_surrender_other_response
说明：另一诸侯国是否同意你的投降
参数：
类型			含义
BYTE 			bAgree
*/

/*	r2c_union_leave_result
说明：脱离诸侯国的反馈
参数：
类型			含义
BYTE 			errorCode
*/

/*	r2c_feod_income_content
说明：采邑收支表
参数：
类型			含义

*/

/*	r2c_feod_taxrate_result
说明：采邑税率调整的反馈
参数：
类型			含义
BYTE 			errorCode
*/

/*	r2c_feod_unionbank_content
说明：采邑国库
参数：
类型			含义
long 			money
*/

/*	r2c_feod_unionbank_withdraw_result
说明：从采邑国库提取金币的反馈
参数：
类型			含义
BYTE 			errorCode
*/

/*	r2c_union_data_refresh
说明：刷新诸侯国数据
参数：
类型			含义
SUnionData		unionData
*/

/*	r2c_union_citizen_join
说明：有人加入了本诸侯国
参数：
类型			含义
SUnionCitizen	citizen
*/

/*	r2c_union_citizen_leave
说明：有人离开了本诸侯国
参数：
类型			含义
DWORD			playerID
*/

/*	r2c_union_citizen_online
说明：本诸侯国国民上线啦
参数：
类型			含义
DWORD			playerID
*/

/*	r2c_union_citizen_offline
说明：本诸侯国国民离线啦
参数：
类型			含义
DWORD			playerID
*/

/*	r2c_union_citizen_official_change
说明：本诸侯国国民官职变动
参数：
类型			含义
DWORD			playerID
BYTE			official
*/

/*	r2c_union_disband
说明：诸侯国解散
参数：
类型			含义
string			UnionName				诸侯国名字
short 			reason
*/

/*	r2c_union_new_bulletin
说明：有新公告
参数：
类型			含义
string			NewBulletin
*/






#endif//UNION_NET_DEFINE_H
