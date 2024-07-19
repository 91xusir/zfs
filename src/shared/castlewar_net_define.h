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
//gc 2 region消息
//--------------------
const short c2r_castlewar_reg_attack		= c2r_castlewar_cmd_base + 111;//攻城诸侯注册
const short c2r_castlewar_reg_attack_cancel	= c2r_castlewar_cmd_base + 112;//攻城诸侯取消注册
const short c2r_castlewar_reg_defend		= c2r_castlewar_cmd_base + 113;//协助守城诸侯注册
const short c2r_castlewar_reg_defend_cancel	= c2r_castlewar_cmd_base + 114;//协助守城诸侯取消注册
const short c2r_castlewar_allow_bund		= c2r_castlewar_cmd_base + 115;//守城诸侯批准与否
const short c2r_castlewar_select_time		= c2r_castlewar_cmd_base + 116;//守城诸侯选择时间
const short c2r_castlewar_view_info			= c2r_castlewar_cmd_base + 117;//玩家查看攻城信息
const short c2r_castlewar_insert_flag		= c2r_castlewar_cmd_base + 118;//诸侯插令旗
const short c2r_castlewar_animal_killed		= c2r_castlewar_cmd_base + 119;//城市守护兽被杀死
const short c2r_castlewar_apply_flag		= c2r_castlewar_cmd_base + 120;//申请令旗
const short c2r_castlewar_flagnpc_show		= c2r_castlewar_cmd_base + 121;//令旗显示

//--------------------
//region 2 gc消息
//--------------------
const short r2c_castlewar_war_prepare			= r2c_castlewar_cmd_base + 110;//通知国民战争准备（5分钟前）
const short r2c_castlewar_war_start				= r2c_castlewar_cmd_base + 111;//通知国民处于城战状态
const short r2c_castlewar_war_pause				= r2c_castlewar_cmd_base + 112;//通知国民处于城战状态
const short r2c_castlewar_war_end				= r2c_castlewar_cmd_base + 113;//通知国民处于城战结束
const short r2c_castlewar_have_apply			= r2c_castlewar_cmd_base + 114;//收到协防请求（仅对诸侯）
const short r2c_castlewar_have_attacker			= r2c_castlewar_cmd_base + 115;//有登记攻城的诸侯
const short r2c_castlewar_reg_attack_rep		= r2c_castlewar_cmd_base + 116;//攻城诸侯注册响应
const short r2c_castlewar_reg_attack_cancel_rep	= r2c_castlewar_cmd_base + 117;//攻城诸侯注册响应
const short r2c_castlewar_reg_defend_rep		= r2c_castlewar_cmd_base + 118;//协助守城诸侯注册响应
const short r2c_castlewar_reg_defend_cancel_rep	= r2c_castlewar_cmd_base + 119;//协助守城诸侯注册响应
const short r2c_castlewar_allow_bund_rep		= r2c_castlewar_cmd_base + 120;//守城诸侯批准与否响应
const short r2c_castlewar_select_time_rep		= r2c_castlewar_cmd_base + 121;//守城诸侯选择时间响应
const short r2c_castlewar_view_info_rep			= r2c_castlewar_cmd_base + 122;//玩家查看攻城信息响应
const short r2c_castlewar_insert_flag_rep		= r2c_castlewar_cmd_base + 123;//诸侯插令旗响应
const short r2c_castlewar_animal_killed_rep		= r2c_castlewar_cmd_base + 124;//城市守护兽被杀死响应
const short r2c_castlewar_refresh				= r2c_castlewar_cmd_base + 125;//更新城战数据
const short r2c_castlewar_user_data_chg			= r2c_castlewar_cmd_base + 126;//玩家自身数据更新
const short r2c_castlewar_is_transported		= r2c_castlewar_cmd_base + 127;//玩家登录后处在城战区域
const short r2c_castlewar_apply_flag_rep		= r2c_castlewar_cmd_base + 128;//申请令旗响应
const short r2c_castlewar_flagnpc_created		= r2c_castlewar_cmd_base + 129;//令旗npc被创建出来

//--------------------
//region 2 gw消息
//--------------------
const short r2g_castlewar_update_data			= r2g_castlewar_cmd_base + 111;//同步数据
const short r2g_castlewar_war_prepare			= r2g_castlewar_cmd_base + 112;//准备
const short r2g_castlewar_war_start				= r2g_castlewar_cmd_base + 113;//开始
const short r2g_castlewar_war_pause				= r2g_castlewar_cmd_base + 114;//中止
const short r2g_castlewar_war_end				= r2g_castlewar_cmd_base + 115;//结束
const short r2g_castlewar_time_set				= r2g_castlewar_cmd_base + 116;//城主时间设置
const short r2g_castlewar_reload_time			= r2g_castlewar_cmd_base + 117;//重载入时间设置

//--------------------
//gw 2 region消息
//--------------------
const short g2r_castlewar_data_init				= g2r_castlewar_cmd_base + 111;//初始化
const short g2r_castlewar_user_online			= g2r_castlewar_cmd_base + 112;//玩家上线
const short g2r_castlewar_war_prepare_rep		= g2r_castlewar_cmd_base + 113;//准备响应
const short g2r_castlewar_war_start_rep			= g2r_castlewar_cmd_base + 114;//开始响应
const short g2r_castlewar_war_pause_rep			= g2r_castlewar_cmd_base + 115;//中止响应
const short g2r_castlewar_war_end_rep			= g2r_castlewar_cmd_base + 116;//结束响应
const short g2r_castlewar_data_update			= g2r_castlewar_cmd_base + 117;//同步响应
const short g2r_castlewar_reload_time_rep		= g2r_castlewar_cmd_base + 118;//重载入时间响应

//---------------------------------------------------------
//Error Code(BYTE)
//---------------------------------------------------------
//------------
//注册相关
//------------
#define ERR_NONE_CWAR				0	//无错误
#define ERR_NOT_VASSAL_CWAR			1	//不是诸侯
#define ERR_DIFF_FACTION_CWAR		2	//不在同一阵营
#define ERR_WAR_PREPARE_CWAR		3	//该城池处于准备交战状态
#define ERR_WARFARE_STATE_CWAR		4	//该城池处于交战状态
#define ERR_REPEAT_REG_CWAR			5	//重复注册
#define ERR_NO_REG_CWAR				6	//未注册
#define ERR_NPC_OCCUPY_STATE_CWAR	7	//npc占领
#define ERR_MASTER_CWAR				8	//城主无需注册
#define ERR_MASTER_CANCEL_CWAR		9	//城主不能取消
#define ERR_REPEAT_ALLOW_CWAR		10	//重复批准
#define ERR_NUM_OUTRANGE_CWAR		11	//数量超过8个，或者重复批准
#define ERR_TIME_OUTRANGE_CWAR		12	//超时
#define ERR_NO_REG_INFO				13	//没有注册过
#define ERR_FULL_CWAR				14	//人数已满
#define ERR_LEVEL_LOWER_CWAR		15	//级别不够
//------------
//插令旗相关
//------------
#define ERR_HAVE_FLAG_ROUND		1	//附近已有令旗
#define ERR_TIME_WRONG			2	//插令旗时间不对
#define ERR_PLACE_WRONG			3	//插令旗地点不对
#define ERR_MULTI_FLAG			4	//多个令旗
#define ERR_UNKNOWN_FLAG		5	//

//-------------------------------
//客户端消息定义说明
//-------------------------------

//------------------------------------------------------
//-c2r_castlewar_reg_attack
//-含义：攻城诸侯注册
//-参数：DWORD	npc_id		注册npcID
//------------------------------------------------------

//------------------------------------------------------
//-c2r_castlewar_reg_defend
//-含义：协助守城诸侯注册
//-参数：DWORD	npc_id		注册npcID
//------------------------------------------------------

//------------------------------------------------------
//-c2r_castlewar_allow_bund
//-含义：守城诸侯批准与否
//-参数：DWORD	union_id		请求守城诸侯ID
//-		 string	feod_name		所守城池名称
//-		 BYTE	allow_info		是否允许其协助
//------------------------------------------------------

//------------------------------------------------------
//-c2r_castlewar_select_time
//-含义：守城诸侯选择下次攻城时间
//-参数：DWORD	union_id		守城诸侯ID
//-		 string	feod_name		所守城池名称
//-		 DWORD	time			所选择的时间
//------------------------------------------------------

//------------------------------------------------------
//-c2r_castlewar_view_info
//-含义：玩家查看攻城信息
//-参数：DWORD	user_id			玩家ID
//-		 string	feod_name		城池名称
//------------------------------------------------------

//------------------------------------------------------
//-c2r_castlewar_insert_flag
//-含义：诸侯插令旗
//-参数：DWORD	union_id		诸侯ID
//-		 string	feod_name		城池名称
//------------------------------------------------------
#endif //CASTLEWAR_NET_DEFINE_H

