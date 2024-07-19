#ifndef _RTG_LIB_H_
#define _RTG_LIB_H_

/*------------------------------------------------------------------------
    API definition.
------------------------------------------------------------------------*/

#ifdef RT_GRAPH_DLL  // 是否生成DLL
#   ifdef RT_GRAPH_EXPORTS
#       define GRAPH_API DLL_EXPORT
#   else
#       define GRAPH_API DLL_IMPORT
#   endif
#else
#   define GRAPH_API
#endif

// #define RTG_LEFT_HANDED // 可以在这里定义为左手坐标系，如果不定义就默认为右手

namespace rt_graph {

/*------------------------------------------------------------------------
  Classes.
------------------------------------------------------------------------*/

class RtgVertex2;
class RtgVertex3;
class RtgVertex4;
class RtgOrientation;
class RtgMatrix12;
class RtgMatrix16;
class RtgDevice;
class RtgDeviceEvent;
class RtgCamera;
class RtgCameraFPS;

struct RtgGeometryNode;
class RtgGeometryFactory;

// Resource manager
class RtgConfig;
class RtgStreamEntry;
class RtgStreamBuffer;
class RtgStreamManager;
class RtgRenderStreamManager;

class RtgTextureEntry;
class RtgRenderTextureInfo;
class RtgRenderTextureManager;


} // namespace rt_graph
//using namespace rt_graph;

/*----------------------------------------------------------------------------
  Include.
----------------------------------------------------------------------------*/

#include "rtg_vertex.h"
#include "rtg_matrix.h"
#include "rtg_triangle.h"
#include "rtg_math.h"
#include "rtg_collision.h"
#include "rtg_node.h"
#include "rtg_enum.h"
#include "rtg_type.h"
#include "rtg_config.h"
#include "rtg_buffer.h"
#include "rtg_geometry.h"
#include "rtg_shader.h"
#include "rtg_image.h"
#include "rtg_texture.h"
#include "rtg_texture_manager.h"
#include "rtg_ligth.h"
#include "rtg_util.h"
#include "rtg_camera.h"
#include "rtg_device_event.h"
#include "rtg_effect_code.h"
#include "rtg_device.h"
#include "rtg_thread_lock.h"
#include "rtg_frame.h"
#include "rtg_plane.h"
#include "rtg_frustum.h"
#include "rtg_device_d3d9.h"

/*----------------------------------------------------------------------------
-   Global function.
----------------------------------------------------------------------------*/
namespace rt_graph {

    bool rtGraphInit();
    void rtGraphExit();
    bool rtCreateDeviceD3D9();
    void rtDestroyDevice();

    inline RtgDeviceD3D9* RtGetRender() {
        extern RtgDeviceD3D9* g_pDevice;
        return g_pDevice;
    }

    inline RtgDevice* RtGetRenderBase() {
        extern RtgDevice* g_pDeviceBase;
        return g_pDeviceBase;
    }

    extern RtPerformance g_GraphPerf;
    extern RtgRenderOption g_RenderOption;
    extern string _call_create_buffer_func_name;
    extern string _call_create_buffer_func_line;
} // namespace rt_graph

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
#endif // _RTG_LIB_H_
