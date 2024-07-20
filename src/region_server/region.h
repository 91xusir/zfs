#ifndef __CS_REGION_H__
#define __CS_REGION_H__

//#define ENABLE_OPERATOR_NEW




#include "region_config.h"
#include "region_client.h"
#include "rs_auction.h"
#include "server_if.h"
using namespace std;

//
// predefine
//
class CRegionFactory;
class CRegionObject;
class CRegionServer;
class CRegionUser;
class CRegionCreature;
class CRegionScene;
class CRegionAI;
class CRegionSpawn;
class CItemContainerBase;
class CItemManager;
class CTrade;
class CPrivateShop;
class CServerTeam;
class CServerMail;
class CServerFriend;
class CRegionNpcShop;
class SHeroList;
class CRegionItemContainer;
class CRegionState;
class CRsDungeonMgr;
class CAntiRobot;
struct SNpcSellRareItem;
class CMonCastle;//tim.yang  怪物攻城
class CWarZhulu;
class CDeduct;	//tooth.shi  异步扣点
class CRegionActivityBase;	//LeiJun 活动实例
struct SMonsterLair;
typedef int (*TCmdFunction)(CG_CmdPacket *cmd);
typedef std::list<CRegionObject*> TObjectList;
typedef EXT_SPACE::unordered_map<ULONG,CRegionObject*> TObjectHash;

#define OB_TYPE(ob) ((ob)->m_type)


struct SConfigRegion
{
	//
	// system config var
	//
    struct SListenClient
    {
        long    unlogin_timeout;            // 单位秒，用户接入Region服务器后，在unlogin_timeout时间内未收到来自GameWorld服务器的用户数据，用户的连接将会被断开。
        long    r2c_interval;               // 单位毫秒，服务器发送数据至客户端的间隔时间
        long    c2r_interval;               // 单位毫秒，客户端发送数据至服务器的间隔时间
        long    cl_max_buffer;              // max cached frame
        long    cl_pre_buffer;              // cache how many frame before play
    } listenClient ;

	//
	// game play config var
	//
    struct SGame
    {
        long    scene_item_live_time;       // 单位秒，掉落道具在存在的时间
		float   exp_rate;					
		float   gold_rate;
		float   item_drop_rate;
		float   item_green_rate;
		float   item_blue_rate;
		float   item_gold_rate;
        long    bDebugMsg;
    } game ;

	//
	// gws config var
	//
    struct SGWS
    {
        int		update_user_interval;       // 单位秒，保存用户数据的时间间隔
        int		accept_user_timeout;        // 单位秒，从GameWorld服务器收到用户数据后，用户必须在accept_user_timeout时间内登陆Region服务器
    } gws ;
};

//
// include file
//
#include "rs_LineMan.h" //rendh 2010.01.19
#include "scene/rts_sv_scene.h"
#include "server_config.h"
#include "region_object.h"
#include "region_skill.h"
#include "pk.h"
#include "region_state.h"
#include "region_creature.h"
#include "region_scene.h"
#include "cmd_channel.h"
#include "herolist_base.h"
#include "region_gwsession.h"
#include "gwt.h"
#include "union_base.h"
#include "union_net_define.h"
#include "business.h"
#include "rs_card_trade.h"
#include "region_server.h"
#include "region_ai.h"
#include "region_spawn.h"
#include "item_net_define.h"
#include "item_trade.h"
#include "item_pshop.h"
#include "cmd_item.h"
#include "region_team.h"
#include "region_friend.h"
#include "region_mail.h"
#include "cmd.h"
#include "region_union.h"
#include "region_util.h"
#include "region_state_snapshot.h"
#include "cmd_union.h"
#include "cmd_feod.h"
#include "region_npc_shop.h"
#include "string_mapping.h"
#include "region_item.h"
//----------------------add start by tony 05.06.02---------------------------//
#include "castlewar_base.h"
#include "castlewar_net_define.h"
#include "region_castlewar.h"
#include "cmd_castlewar.h"
#include "auction_base.h"
#include "auction_net_base.h"
#include "cmd_auction.h"
//----------------------add end   by tony 05.06.02---------------------------//
#include "rs_cmd_LineSys.h"//rendh 2010.01.21
#include "region_gift.h"
#include "rs_dungeon.h"
#include "region_anti_robot.h"
#include "region_MonCastle.h"//tim.yang  
#include "rs_war_zhulu.h" //cheney.zheng
#include "region_deduct.h"
//
// extern variable 
//
extern  SConfigRegion       g_cfg;
extern  SConfigRegionNet    g_cfgRs;
//extern  SConfigEudemon      g_cfgEudemon;
//extern  SConfigConnect      g_cfgConnectGameWorld;
extern  CG_CmdPacket       *g_sendCmd;
extern  CG_CmdPacket       *g_sendCmdWG;
extern  CRegionFactory     *g_factory;
extern  CRegionServer      *g_region;
extern	bool				g_bUnWg;	//反外挂

//
// function
//
bool		RegionInit();
void		RegionExit();

#endif
