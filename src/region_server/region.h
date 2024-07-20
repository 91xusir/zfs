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
class CMonCastle;//tim.yang  ���﹥��
class CWarZhulu;
class CDeduct;	//tooth.shi  �첽�۵�
class CRegionActivityBase;	//LeiJun �ʵ��
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
        long    unlogin_timeout;            // ��λ�룬�û�����Region����������unlogin_timeoutʱ����δ�յ�����GameWorld���������û����ݣ��û������ӽ��ᱻ�Ͽ���
        long    r2c_interval;               // ��λ���룬�����������������ͻ��˵ļ��ʱ��
        long    c2r_interval;               // ��λ���룬�ͻ��˷����������������ļ��ʱ��
        long    cl_max_buffer;              // max cached frame
        long    cl_pre_buffer;              // cache how many frame before play
    } listenClient ;

	//
	// game play config var
	//
    struct SGame
    {
        long    scene_item_live_time;       // ��λ�룬��������ڴ��ڵ�ʱ��
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
        int		update_user_interval;       // ��λ�룬�����û����ݵ�ʱ����
        int		accept_user_timeout;        // ��λ�룬��GameWorld�������յ��û����ݺ��û�������accept_user_timeoutʱ���ڵ�½Region������
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
extern	bool				g_bUnWg;	//�����

//
// function
//
bool		RegionInit();
void		RegionExit();

#endif
