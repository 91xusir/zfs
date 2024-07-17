#ifndef __ACTOR_H__
#define __ACTOR_H__


#ifdef ACTOR_EDITOR
#define _XTP_STATICLINK
#define _XTP_INCLUDE_DEPRECATED
#include <XTP/XTToolkitPro.h>
#endif

#include "core/rt2_core.h"
#include "graph/rt_graph.h"

using namespace std;
using namespace rt2_core;

#ifndef MAX_UTILITY
	#include "audio/rt_audio.h"
//	#include "rts_scene_util.h"
   
#endif

#ifdef CHAR_VIEWER
	#include "stdafx.h"
	#include "tool_character.h"
	#include "Resource.h"
	#include "CustomItems.h"
	#include "DlgPose.h"
	#include "TimeView.h"
	#include "DlgChannelInfo.h"
	#include "DlgKeyInfo.h"
	#include "DlgCreateEffect.h"
	#include "DlgNewShader.h"
	#include "DlgGetBone.h"
	#include "DlgCreateForceField.h"
#endif

using namespace std;
class RtsSceneUtil;
#include "cha_basic.h"
#include "cha_anim.h"
#include "rtc_actor_command.h"
#include "rtc_actor_manager.h"
#include "cha_mtl.h"
#include "cha_effect.h"
#include "scene/rts_scene_util.h"

//tim.yang
#ifdef ACTOR_EDITOR
	#include "MainFrm.h"
#endif

#endif
