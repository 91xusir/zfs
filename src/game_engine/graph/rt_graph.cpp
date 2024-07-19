
#include "rtg_graph_inter.h"
#include "rtg_device_factory.h"
#include "TextureRLEWrapper.h"
#include "graph\rtg_device.h"

namespace rt_graph {
#if _M_IX86 == 500
#   pragma message( "Pentium processor build" )
#endif

/*----------------------------------------------------------------------------
    Class RtgShader.
----------------------------------------------------------------------------*/
#ifdef _DEBUG
RtgShader::~RtgShader()
{
    iNumStage = 256;
}
#endif

bool rtGraphInit()
{
    RtCoreLog().Info("graph initialized\n");

    RT_STATIC_REGISTRATION_CLASS(RtgDeviceEvent);
    RT_STATIC_REGISTRATION_CLASS(CRtgAppFrame);
    RT_STATIC_REGISTRATION_CLASS(RtgCamera);
    RT_STATIC_REGISTRATION_CLASS(RtgCameraFPS);
    RT_STATIC_REGISTRATION_CLASS(RtgCameraEditor);
    RT_STATIC_REGISTRATION_CLASS(RtgCamera3DMax);
    RT_STATIC_REGISTRATION_CLASS(RtgCameraGame);
    CTextureRLEWrapper::InitTextureRLEWrapper();

    if (!rtCreateDeviceD3D9())
    {
        RtCoreLog().Error("create device fail\n");
        return false;
    }

    return true;
}

void rtGraphExit()
{
    rtDestroyDevice();
	CTextureRLEWrapper::DestroyTextureRLEWrapper();
    RtCoreLog().Info("Graph exit.\n");
}


} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
