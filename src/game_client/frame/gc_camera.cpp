
#include "graph/rt_graph.h"
#include "gc_camera.h"

RT_IMPLEMENT_DYNCREATE(GcCamera,RtgCameraGame,NULL,"")

GcCamera::GcCamera()
{
    SetUpOffset(15.f);
}

GcCamera::~GcCamera()
{

}

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
