
#include "rtg_graph_inter.h"

namespace rt_graph {

RtgVertex3 MultiColor(RtgVertex3 c1,RtgVertex3 c2)
{
	RtgVertex3 ret;
	ret.x = c1.x * c2.x;
	ret.y = c1.y * c2.y;
	ret.z = c1.z * c2.z;
	return ret;
}

DWORD RtgComputeVertexColor(RtgVertex3& vPos, RtgVertex3& vNormal, RtgVertex3 amb, RtgVertex3 dif, RtgVertex3 emi, 
							RtgVertex3& vSkyLight, RtgLightItem* pLights, float fAlpha)
{
    // if (pLights==NULL) return 0xFF000000;

    float fVCos, fVAtt, fVLen;
    RtgVertex3 vLightDir;
    RtgVertex3 vColorValue;
    RtgVertex3 vColorPoint;
    RtgLightItem* pLight;

    // 环境光 = 光的强度
    vColorValue = vSkyLight;
	vColorValue = MultiColor(vColorValue,amb);

    for (pLight=pLights; pLight!=NULL; pLight=pLight->pNext)
    {
        switch (pLight->eMode)
        {
        case RTG_LIGHT_DIRECTION:
            // 平行光 = (光线方向*点的法向量)*光的强度
            vLightDir = pLight->vDirection;
            vLightDir.Negative();
            fVCos = vNormal * vLightDir;
            fVCos = Clamp(fVCos, 0.f, 1.f);
            vColorValue += MultiColor(pLight->vDiffuse,dif) * fVCos;
            break;
        case RTG_LIGHT_POINT:
            // 点光源 = (光线方向*点的法向量)*光的强度*衰减系数
            //     衰减系数 = min(1, 1/(a0+a1*距离+a2*距离*距离)
            fVLen = vPos.Distance(pLight->vPosition);
            if (fVLen<pLight->fRange)
            {
                vLightDir = pLight->vPosition-vPos;
                vLightDir.Normalize();
                fVCos = vNormal * vLightDir;
                fVCos = Clamp(fVCos, 0.f, 1.f);
                fVAtt = 1.f/(pLight->fConstantAttenuation
                    +pLight->fLinearAttenuation*fVLen
                    +pLight->fQuadraticAttenuation*fVLen*fVLen);
                // fVAtt = Clamp(fVAtt, 0.f, 1.f);
                vColorPoint = MultiColor(pLight->vDiffuse,dif) * (fVCos*fVAtt);
                vColorValue += vColorPoint;
            }
            break;
        case RTG_LIGHT_SPOT:
            RtCoreLog().Info("警告: 目前不支持聚光灯.\n");
            break;
        case RTG_LIGHT_PARALLELPOINT:
            RtCoreLog().Info("警告: 目前不支持Parallel Point Light.\n");
            break;
        default:
            CHECKEX("使用了未知的灯光类型");
            break;
        }
    }

	vColorValue += emi;
    // 返回颜色
    return RtgVectorToColor(vColorValue, fAlpha);
}

DWORD RtgAddVertexColor(RtgVertex3& vPos, RtgVertex3& vNormal, DWORD dwColor, RtgLightItem* pLights)
{
    RtgVertex3 vColorValue = RtgColorToVector(dwColor);
    float fVCos, fVLen, fVAtt;
    RtgVertex3 vLightDir, vColorPoint;
    RtgVertex3 dif(1.f, 1.f, 1.f);
    RtgLightItem* pLight;
    for (pLight=pLights; pLight!=NULL; pLight=pLight->pNext)
    {
        switch (pLight->eMode)
        {
        case RTG_LIGHT_DIRECTION:
            // 平行光 = (光线方向*点的法向量)*光的强度
            fVCos = vNormal * pLight->vDirection;
            fVCos = Clamp(fVCos, 0.f, 1.f);
            vColorValue += MultiColor(pLight->vDiffuse,dif) * fVCos;
            break;
        case RTG_LIGHT_POINT:
            // 点光源 = (光线方向*点的法向量)*光的强度*衰减系数
            //     衰减系数 = min(1, 1/(a0+a1*距离+a2*距离*距离)
            fVLen = vPos.Distance(pLight->vPosition);
            if (fVLen<pLight->fRange)
            {
                vLightDir = pLight->vPosition-vPos;
                vLightDir.Normalize();
                fVCos = vNormal * vLightDir;
                fVCos = Clamp(fVCos, 0.f, 1.f);
                fVAtt = 1.f/(pLight->fConstantAttenuation
                    +pLight->fLinearAttenuation*fVLen
                    +pLight->fQuadraticAttenuation*fVLen*fVLen);
                // fVAtt = Clamp(fVAtt, 0.f, 1.f);
                vColorPoint = MultiColor(pLight->vDiffuse,dif) * (fVCos*fVAtt);
                vColorValue += vColorPoint;
            }
            break;
        case RTG_LIGHT_SPOT:
            RtCoreLog().Info("警告: 目前不支持聚光灯.\n");
            break;
        case RTG_LIGHT_PARALLELPOINT:
            RtCoreLog().Info("警告: 目前不支持Parallel Point Light.\n");
            break;
        default:
            CHECKEX("使用了未知的灯光类型");
            break;
        }
    }
    return RtgVectorToColor(vColorValue, ((dwColor&0xFF000000)>>24)/255.f);
}

DWORD RtgAddVertexColor2(RtgVertex3& vPos, RtgVertex3& vNormal, DWORD dwColor1, DWORD dwColor2, float fColor2Rate, RtgLightItem* pLights)
{
    RtgVertex3 vColorValue = RtgColorToVector(dwColor1)*(1.f-fColor2Rate) + RtgColorToVector(dwColor2)*fColor2Rate;
    float fVCos, fVLen, fVAtt;
    RtgVertex3 vLightDir, vColorPoint;
    RtgVertex3 dif(1.f, 1.f, 1.f);
    RtgLightItem* pLight;
    for (pLight=pLights; pLight!=NULL; pLight=pLight->pNext)
    {
        switch (pLight->eMode)
        {
        case RTG_LIGHT_DIRECTION:
            // 平行光 = (光线方向*点的法向量)*光的强度
            fVCos = vNormal * pLight->vDirection;
            fVCos = Clamp(fVCos, 0.f, 1.f);
            vColorValue += MultiColor(pLight->vDiffuse,dif) * fVCos;
            break;
        case RTG_LIGHT_POINT:
            // 点光源 = (光线方向*点的法向量)*光的强度*衰减系数
            //     衰减系数 = min(1, 1/(a0+a1*距离+a2*距离*距离)
            fVLen = vPos.Distance(pLight->vPosition);
            if (fVLen<pLight->fRange)
            {
                vLightDir = pLight->vPosition-vPos;
                vLightDir.Normalize();
                fVCos = vNormal * vLightDir;
                fVCos = Clamp(fVCos, 0.f, 1.f);
                fVAtt = 1.f/(pLight->fConstantAttenuation
                    +pLight->fLinearAttenuation*fVLen
                    +pLight->fQuadraticAttenuation*fVLen*fVLen);
                // fVAtt = Clamp(fVAtt, 0.f, 1.f);
                vColorPoint = MultiColor(pLight->vDiffuse,dif) * (fVCos*fVAtt);
                vColorValue += vColorPoint;
            }
            break;
        case RTG_LIGHT_SPOT:
            RtCoreLog().Info("警告: 目前不支持聚光灯.\n");
            break;
        case RTG_LIGHT_PARALLELPOINT:
            RtCoreLog().Info("警告: 目前不支持Parallel Point Light.\n");
            break;
        default:
            CHECKEX("使用了未知的灯光类型");
            break;
        }
    }
    return RtgVectorToColor(vColorValue, ((dwColor1&0xFF000000)>>24)/255.f);
}

DWORD RtgComputeFogVertexColor(RtgVertex3 &Pos, RtgFog &FogInfo)
{
	float range = FogInfo.End - FogInfo.Start;
	if(range == 0) return 1;

	float delta;
	if(Pos.z < FogInfo.Start) 
		delta = 0;
	else if(Pos.z > FogInfo.End) 
		delta = 1;
	else
		delta = (Pos.z-FogInfo.Start)/range;
	
	delta = 1-delta;
	RtgVertex3 col;
	col = FogInfo.Col*delta;
	
	col.x += (1-delta);
	col.y += (1-delta);
	col.z += (1-delta);
	return RtgVectorToColor(FogInfo.Col, delta);
}

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
