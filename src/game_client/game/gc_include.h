
/*----------------------------------------------------------------------------
-   Class.
----------------------------------------------------------------------------*/
#ifndef GAME_CLIENT
#define GAME_CLIENT
#endif 

class GcBaseActor;
class GcSynchroActor;
class GcPlayer;
class GcSynchroActorManager;

class GcPathNode;
class RtsPathFind;

class GcWorld;
class CGameClientFrame;

typedef GcBaseActor* PGcBaseActor;

/*----------------------------------------------------------------------------
-   Include.
----------------------------------------------------------------------------*/
#include "lua_inf.h"

// begin gc_include.h [3/15/2009 tooth.shi]
#include "gc_timer.h"

// end gc_include [3/15/2009 tooth.shi]
#include "ui/rtw_ui.h"
using namespace ui;
#include "uimain.h"

#include "scene/rts_path_find.h"
#include "scene/rt_scene.h"
#include "gc_types.h"
#include "gc_enums.h"

#include "skill_manage.h"
#include "region_client.h"
#include "gc_base_actor.h"
#include "gc_actor_graph.h"
#include "gc_player_items.h"
#include "gc_skill.h"
#include "pk.h"
#include "gc_actor.h"
#include "gc_base_actor.h"
#include "gc_synchro_actor.h"
#include "gc_player.h"
#include "gc_pet.h"

#include "net/rt_net.h"
#include "net_session.h"
#include "net_command.h"
#include "net_synchro_position.h"
#include "net_command_item.h"
#include "net_command_pet.h"

#include "gc_dnumber.h"
#include "gc_world.h"
#include "gc_frame.h"
#include "gc_global.h"
#include "string_mapping.h"

#include "gc_sequence_image.h"
#include "ui_layer.h"
#include "NesClient.h"

//lyymark add prelog.h 2024.7.27
#include "../preLog.h"
/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
