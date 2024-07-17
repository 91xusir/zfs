#include "scene/rt_scene.h"

RT_IMPLEMENT_DYNCREATE(RtsLight, RtsObject, 0, "scene")

RtsLight::RtsLight()
{
    m_bEnable = FALSE;        // 是否使用灯光
    fRange = 200.f;
    SetAttenuation(0.f, 0.005f, 0.f);
    vDiffuse.Set(1.f);

//#if RTS_DRAW_HELPER
//    SetShowType(RtGetRender(),SO_SPRITE);
//#endif
}

RtsLight::~RtsLight()
{
}
long RtsLight::Serialize(RtArchive& ar)
{
    if (!RtsObject::Serialize(ar)) return FALSE;
    ar << m_bEnable;
    if (!RtgLight::Serialize(ar)) return FALSE;
    return TRUE;
}
void RtsLight::ResetPosition()
{
	//RtgVertex3 vPos(GetMatrix().Position());
	//vPosition = vPos;
	const float* pU = m_Matrix.U();
	const float* pv = m_Matrix.Position();
	const float* pLast = pU + 12;
	//RtCoreLog().Info("U:%d,P:%d,L:%d\n", pU, pv, pLast);

	float f1 = 0.0, f2 = 0.0, f3 = 0.0;
	f1 = pLast[0];
	f2 = pLast[1];
	f3 = pLast[2];
	//RtCoreLog().Info("1:%f,2:%f,3:%f\n", f1, f2, f3);

	RtgVertex3 vPos(pLast);
	vPosition = vPos;
	
}

/*------------------------------------------------------------------------
  The End.
------------------------------------------------------------------------*/
