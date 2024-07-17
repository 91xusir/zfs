#ifndef __ACTOR_H__
#define __ACTOR_H__

// v = 1 - v
#define FLIP_V

#ifdef ACTOR_EDITOR
	#define _XTP_INCLUDE_DEPRECATED
	#include <XTToolkitPro.h>
#endif

#include "core/rt2_core.h"
#include "graph/rt_graph.h"

using namespace std;
using namespace rt2_core;
#ifndef MAX_UTILITY
	#include "audio/rt_audio.h"
	#include "scene/rts_scene_util.h"
#endif

class CRT_ObjectData;
class CRT_ObjectInst;
class CRT_ActorInstance;
class CRT_SkinInstance;
class CRT_MtlLibInstance;
class CRT_MtlInstance;
class CRT_EffectInstance;

class CRT_ForceField;
class RtEngineSystemChar;

#ifdef ACTOR_EDITOR
	#include "../../tester/tool_character/stdafx.h"
	#include "../../tester/tool_character/tool_character.h"
	#include "../../tester/tool_character/Resource.h"
	#include "../../tester/tool_character/CustomItems.h"
	#include "../../tester/tool_character/DlgPose.h"
	#include "../../tester/tool_character/TimeView.h"
	#include "../../tester/tool_character/DlgChannelInfo.h"
	#include "../../tester/tool_character/DlgKeyInfo.h"
	#include "../../tester/tool_character/DlgCreateEffect.h"
	#include "../../tester/tool_character/DlgNewShader.h"
	#include "../../tester/tool_character/DlgGetBone.h"
	#include "../../tester/tool_character/DlgCreateForceField.h"
#endif

#include "net2/rt_net.h"
#include "net2/rt_net_extend.h"
#include "cha_data.h"
#include "cha_sys.h"
#include "cha_anim.h"
#include "cha_mtl.h"
#include "cha_effect.h"

#ifdef ACTOR_EDITOR
	#include "../../tester/tool_character/MainFrm.h"
#endif

#endif
	