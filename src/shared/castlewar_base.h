//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#ifndef CASTLEWAR_BASE_H
#define CASTLEWAR_BASE_H

#include <time.h>
#include <string>

using namespace std;

typedef time_t		TIME;
typedef struct tm	ST_TIME;

//--------------------------------------------
//待修改...
//--------------------------------------------
#define ITEM_FLAG_ID			570		//令旗道具ID
#define FLAG_RANGE				200

#define FLAG_NPC_ID_SHANG		719
#define FLAG_NPC_ID_ZHOU		725

#define XIQI_ANIMAL_NPC_ID		723
#define ZHAOGE_ANIMAL_NPC_ID	722
#define CHUANYUN_ANIMAL_NPC_ID	724

//op mode
const BYTE ADD			= 0;
const BYTE MINUS		= 1;
const BYTE ALLOWED		= 2;
const BYTE NOTALLOWED	= 3;

//------------------------------------
//采邑状态(修改只能在其后追加)
//------------------------------------
enum EState
{
	NPC_OCCUPY_STATE			= 0, //NPC占领	（可注册）
	UNION_OCCUPY_STATE			= 1, //诸侯占领	（可注册）
	WAR_PREPARE_STATE			= 2, //交战准备	（城战开始前5分钟）（插旗/不可注册）
	WARFARE_STATE				= 3, //交战中	（插旗/不可注册）
	WAR_FIVE_MIN_PAUSE_STATE	= 4	 //提前结束战斗后，五分钟准备继续攻城 （可注册）
};

struct STAttackerInfo
{
	DWORD	union_id;		//诸侯国id
	string	union_name;		//诸侯国名字
	string	vassal_name;	//诸侯名字
	DWORD	flag_id;		//诸侯国令旗id对应令旗创造出来时m_oId
							/* 0x0000: 未创建令旗
							 * 0xffff: 已创建并被打掉
							 * 其他值: 已创建未被打掉
							 */
	
	STAttackerInfo()
	{ 
		union_id	= 0;
		union_name	= "";
		vassal_name	= "";
		flag_id		= 0;
	}

	bool Serialize(CG_CmdPacket *cmd);
};

struct STDefenderInfo
{
	DWORD	union_id;		//诸侯国id
	string	union_name;		//诸侯国名字
	string	vassal_name;	//诸侯名字
	bool	is_allowed;		//是否被守城诸侯批准
	DWORD	flag_id;		//诸侯国令旗id对应令旗创造出来时m_oId
							/* 0x0000: 未创建令旗
							 * 0xffff: 已创建并被打掉
							 * 其他值: 已创建未被打掉
							 */

	STDefenderInfo()
	{ 
		union_id	= 0; 
		union_name	= "";
		vassal_name	= "";
		flag_id		= 0;
		is_allowed	= false; 
	}

	bool Serialize(CG_CmdPacket *cmd);
};

struct STWarInfo
{
	string					feod_name;				//采邑名字
	DWORD					master_union_id;		//所属诸侯国id
	string					master_union_name;		//所属诸侯国名字
	string					master_union_logo;		//所属诸侯国标志
	string					vassal_name;			//诸侯国诸侯名字
	DWORD					flag_id;				//诸侯国令旗id（同上）
	EState					state;					//采邑目前状态
	DWORD					start_time;				//下次城战开始时间
	DWORD					end_time;				//下次城战结束时间
	DWORD					last_end_time;			//上次城战结束时间
	bool					guard_animal_is_dead;	//城市守护兽是否已死
	vector<STAttackerInfo>	v_attacker_union_info;	//攻城方信息列表
	vector<STDefenderInfo>	v_defender_union_info;	//防守方信息列表(包括未被批准的)
	
	STWarInfo() :
	feod_name(""), master_union_name(""), master_union_logo(""), vassal_name("")
	{
		master_union_id	= 0;
		state			= NPC_OCCUPY_STATE;
		start_time		= 0;
		end_time		= 0;
		flag_id			= 0;
		last_end_time	= 0;
		guard_animal_is_dead	= false;

		v_attacker_union_info.clear();
		v_defender_union_info.clear();
	}
	
	bool Serialize(CG_CmdPacket *cmd);

	const STWarInfo& operator=(const STWarInfo& rhs);
};

//-------------------------------------
//所有采邑信息（与数据库保持一致）
//-------------------------------------
//const int FEOD_COUNT = 3;
//const string FeodNameInDB[FEOD_COUNT] = { "XiQi", "ZhaoGe", "ChuanYun" };
//char* CastleWarAreaName[3] = {"西岐城","朝歌城","穿云关"};


#endif //CASTLEWAR_BASE_H

