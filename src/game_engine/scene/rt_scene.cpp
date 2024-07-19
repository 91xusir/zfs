
#include "scene/rt_scene.h"

RtScene* g_pScene = NULL;
RtScene* g_pScene1 = NULL;
RtgMatrix16 tmp;
RtgVertex3  t_eye;

bool rtSceneInit(const char* szPackage)
{
    RT_STATIC_REGISTRATION_CLASS(RtsObject);
    RT_STATIC_REGISTRATION_CLASS(RtsModel);
    RT_STATIC_REGISTRATION_CLASS(RtsLight);
    RT_STATIC_REGISTRATION_CLASS(RtsDynamicLight);
    RT_STATIC_REGISTRATION_CLASS(RtsAudio3D);

    RT_STATIC_REGISTRATION_CLASS(RtsInnervation);
    RT_STATIC_REGISTRATION_CLASS(RtsInnerveFireFly);

    RtsInnervation::InitInnervation();

    return true;
}

void rtScenePreExit()
{
	if (g_pScene)
	{
		g_pScene->OnceClear();		
	}
	RtsInnervation::ClearInnervation();
}

void rtSceneExit()
{
   DEL_ONE(g_pScene);    
   RtObjectManager()->Clear();
}