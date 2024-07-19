
namespace rt_graph {

struct RtgLightItem : public RtgLight
{
    float           fDistance;  // ���룬��ʾ��Ŀ���ľ��룬����GetRangeLights�����У���������
    RtgLightItem*   pNext;      // ��һ���ƹ⣬ͨ�����ڼ��㶥��ɫ�ĵط�
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
