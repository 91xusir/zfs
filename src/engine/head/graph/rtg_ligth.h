
namespace rt_graph {

struct RtgLightItem : public RtgLight
{
    float           fDistance;  // 距离，表示和目标点的距离，用在GetRangeLights函数中，进行排序
    RtgLightItem*   pNext;      // 下一个灯光，通常用在计算顶点色的地方
};

DWORD RtgComputeVertexColor(RtgVertex3& vPos, RtgVertex3& vNormal, RtgVertex3 amb, RtgVertex3 dif, RtgVertex3 emi, 
							RtgVertex3& vSkyLight, RtgLightItem* pLights, float fAlpha=1.f);
DWORD RtgComputeFogVertexColor(RtgVertex3 &Pos, RtgFog &FogInfo);
DWORD RtgAddVertexColor(RtgVertex3& vPos, RtgVertex3& vNormal, DWORD dwColor, RtgLightItem* pLights);
DWORD RtgAddVertexColor2(RtgVertex3& vPos, RtgVertex3& vNormal, DWORD dwColor1, DWORD dwColor2, float fColor2Rate, RtgLightItem* pLights);

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
