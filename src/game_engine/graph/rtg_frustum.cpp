#include "rtg_graph_inter.h"

namespace rt_graph {

RtgFrustum::RtgFrustum()
{
}

RtgFrustum::~RtgFrustum()
{
}

void RtgFrustum::ExtractFrustum(RtgCamera* pCamera)
{
    CHECK(pCamera!=NULL);
    for (int i=0; i<6; i++)
    {
        m_Planes[i] = pCamera->m_Planes[i];
    }
}

// 根据板plane裁剪视锥体
RtgFrustum RtgFrustum::CutByPlane(RtgPlane& plane)
{
    return *this;
}

bool RtgFrustum::PointTest(const RtgVertex3& vPos)
{
	for (int p=0; p<6; p++)
	{
        if (m_Planes[p].x * vPos.x + m_Planes[p].y * vPos.y + m_Planes[p].z * vPos.z + m_Planes[p].w > 0)
			return false;
	}
	return true;
}

bool RtgFrustum::SphereTest(const RtgVertex3& vPos, float radius)
{
	for (int p=0; p<6; p++)
	{
		if (m_Planes[p].x * vPos.x + m_Planes[p].y * vPos.y + m_Planes[p].z * vPos.z + m_Planes[p].w <= -radius)
			return false;
	}
	return true;
}

bool RtgFrustum::CubeTest(const RtgVertex3& vPos, float size)
{
	for(int p = 0; p < 6; p++)
	{
		if ( m_Planes[p].x * (vPos.x-size) + m_Planes[p].y * (vPos.y-size) + m_Planes[p].z * (vPos.z-size) + m_Planes[p].w > 0 ) continue;
		if ( m_Planes[p].x * (vPos.x+size) + m_Planes[p].y * (vPos.y-size) + m_Planes[p].z * (vPos.z-size) + m_Planes[p].w > 0 ) continue;
		if ( m_Planes[p].x * (vPos.x-size) + m_Planes[p].y * (vPos.y+size) + m_Planes[p].z * (vPos.z-size) + m_Planes[p].w > 0 ) continue;
		if ( m_Planes[p].x * (vPos.x+size) + m_Planes[p].y * (vPos.y+size) + m_Planes[p].z * (vPos.z-size) + m_Planes[p].w > 0 ) continue;
		if ( m_Planes[p].x * (vPos.x-size) + m_Planes[p].y * (vPos.y-size) + m_Planes[p].z * (vPos.z+size) + m_Planes[p].w > 0 ) continue;
		if ( m_Planes[p].x * (vPos.x+size) + m_Planes[p].y * (vPos.y-size) + m_Planes[p].z * (vPos.z+size) + m_Planes[p].w > 0 ) continue;
		if ( m_Planes[p].x * (vPos.x-size) + m_Planes[p].y * (vPos.y+size) + m_Planes[p].z * (vPos.z+size) + m_Planes[p].w > 0 ) continue;
		if ( m_Planes[p].x * (vPos.x+size) + m_Planes[p].y * (vPos.y+size) + m_Planes[p].z * (vPos.z+size) + m_Planes[p].w > 0 ) continue;
		return false;
	}
	return true;
}

bool RtgFrustum::AABBTest(const RtgVertex3& vMin, const RtgVertex3& vMax)
{
    float tmp[6];
	for(int i = 0; i < 6; i++)
	{
#ifdef USE_LEFT_HAND
        // a*x + b*y + c*z - d > 0
        tmp[0] = m_Planes[i].x * vMin.x;
        tmp[1] = m_Planes[i].y * vMin.y;
        tmp[2] = m_Planes[i].z * vMin.z - m_Planes[i].w;
		if ( tmp[0] + tmp[1] + tmp[2] > 0 ) continue;
        tmp[3] = m_Planes[i].x * vMax.x;
		if ( tmp[3] + tmp[1] + tmp[2] > 0 ) continue;
        tmp[4] = m_Planes[i].y * vMax.y;
		if ( tmp[0] + tmp[4] + tmp[2] > 0 ) continue;
		if ( tmp[3] + tmp[4] + tmp[2] > 0 ) continue;
        tmp[5] = m_Planes[i].z * vMax.z - m_Planes[i].w;
		if ( tmp[0] + tmp[1] + tmp[5] > 0 ) continue;
		if ( tmp[3] + tmp[1] + tmp[5] > 0 ) continue;
		if ( tmp[0] + tmp[4] + tmp[5] > 0 ) continue;
		if ( tmp[3] + tmp[4] + tmp[5] > 0 ) continue;
#else
        // a*x + b*y + c*z + d <= 0
        tmp[0] = m_Planes[i].x * vMin.x;
        tmp[1] = m_Planes[i].y * vMin.y;
        tmp[2] = m_Planes[i].z * vMin.z + m_Planes[i].w;
		if ( tmp[0] + tmp[1] + tmp[2] <= 0 ) continue;
        tmp[3] = m_Planes[i].x * vMax.x;
		if ( tmp[3] + tmp[1] + tmp[2] <= 0 ) continue;
        tmp[4] = m_Planes[i].y * vMax.y;
		if ( tmp[0] + tmp[4] + tmp[2] <= 0 ) continue;
		if ( tmp[3] + tmp[4] + tmp[2] <= 0 ) continue;
        tmp[5] = m_Planes[i].z * vMax.z + m_Planes[i].w;
		if ( tmp[0] + tmp[1] + tmp[5] <= 0 ) continue;
		if ( tmp[3] + tmp[1] + tmp[5] <= 0 ) continue;
		if ( tmp[0] + tmp[4] + tmp[5] <= 0 ) continue;
		if ( tmp[3] + tmp[4] + tmp[5] <= 0 ) continue;
#endif
		return false;
	}
	return true;
}

/*
// 点(x0, y0, z0)到平面(Ax+By+Cz+D=0)的距离
//
//   d = abs(A*x0+B*y0+C*z0+D)/sqrt(A*A+B*B+C*C);
//
bool RtgFrustum::BoundTest(ELBound* pBound)
{
    const float* fPos = pBound->Position();
    float fRadius = pBound->Radius();
	for(int i=0; i<6; i++)
	{
        float fV = sqrt(m_Planes[i].x*m_Planes[i].x+m_Planes[i].y*m_Planes[i].y+m_Planes[i].z*m_Planes[i].z);
        if ((m_Planes[i].x*fPos[0]+m_Planes[i].y*fPos[1]+m_Planes[i].z*fPos[2]-m_Planes[i].w)/fV > -fRadius) continue;
        return false;
	}
    return true;
}
*/

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
