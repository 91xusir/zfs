
#include "scene/rt_scene.h"

/*------------------------------------------------------------------------
-   Class RtsRenderAgent.
------------------------------------------------------------------------*/

RT_IMPLEMENT_DYNAMIC(RtsRenderAgent, RtsObject, 0, "scene")

RtsRenderAgent::RtsRenderAgent()
{
}

RtsRenderAgent::~RtsRenderAgent()
{
    CHECK(g_pScene->CheckObjectIsLastRenderList(this)==false);
}

/*------------------------------------------------------------------------
-   class RtsRenderAgentShadow.
------------------------------------------------------------------------*/


RT_IMPLEMENT_DYNAMIC(RtsRenderAgentShadow, RtsActor, 0, "scene")

RtsRenderAgentShadow::RtsRenderAgentShadow()
{
}

RtsRenderAgentShadow::~RtsRenderAgentShadow()
{
    CHECK(g_pScene->CheckObjectIsLastRenderList(this)==false);
}

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
