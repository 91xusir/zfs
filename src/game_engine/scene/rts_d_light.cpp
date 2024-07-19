#include "scene/rt_scene.h"

RT_IMPLEMENT_DYNCREATE(RtsDynamicLight, RtsLight, 0, "scene")

RtsDynamicLight::RtsDynamicLight()
{
}

RtsDynamicLight::~RtsDynamicLight()
{
}

long RtsDynamicLight::Serialize(RtArchive& ar)
{
    if (!RtsLight::Serialize(ar)) return FALSE;
    // ar << m_cType;
    return TRUE;
}

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
