#ifndef _RTK_3D_SCENE_LIB_H_
#define _RTK_3D_SCENE_LIB_H_

/*------------------------------------------------------------------------
  Classes.
------------------------------------------------------------------------*/

class RtsFootPrint;
class RtsObject;
class     RtsLight;
class         RtsDynamicLight;
class     RtsAudio3D;
class     RtsModel;
class     RtsInnervation;
class         RtsInnerveFireFly;
class RtSceneBlockTerrain;
class     RtSceneBlock;
class RtSceneDoc;
class RtsSceneUtil;
class     RtScene;
class   RtsWayPoint;//tim.yang  

#ifdef SCENE_EDITOR
#   ifndef RTS_DRAW_HELPER
#       define RTS_DRAW_HELPER     1
#   endif
#endif


/*----------------------------------------------------------------------------
    Global variables.
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  Include.
----------------------------------------------------------------------------*/
#include <vector>
//using namespace std;

#include "graph/rt_graph.h"
#include "audio/rt_audio.h"

#include "character/actor.h"

#include "scene/rts_scene_type.h"
#include "scene/rts_footprint.h"
#include "scene/rts_path_point.h"//tim.yang °üº¬rts_path_point.h
#include "scene/rts_object.h"
#include    "scene/rts_light.h"
#include       "scene/rts_d_light.h"
#include    "scene/rts_audio_3d.h"
#include    "scene/rts_model.h"
#include    "scene/rts_innerve.h"
#include    "scene/rts_obj_render_agent.h"
#include "scene/rts_object_manger.h"
#include "scene/rts_scene_terrain.h"
#include    "scene/rts_scene_block.h"
#include "scene/rts_grass_render.h"
#include "scene/rts_scene_render.h"
#include "scene\rts_scene_block_manager.h"
#include "scene/rts_scene_doc.h"
#include "scene/rts_scene_util.h"
#include "core/rt2_core.h"
#include "scene/rts_grass_render.h"
#include "scene/rts_scene_render.h"
#include    "scene/rts_scene.h"
#include "scene/rts_scene_area.h"
#include "rts_path_find.h"
#include "scene/rts_path_point.h"

extern RtScene* g_pScene;
extern RtgMatrix16  tmp;
extern RtgVertex3   t_eye;

bool rtSceneInit(const char* szPackage);
void rtScenePreExit();
void rtSceneExit();

#endif

