
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtg_effect_code.cpp
//     Author : zxy
// CreateTime : 2009.12.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#include "graph\rtg_effect_code.h"

namespace rt_graph {
const char* terrainfx = R"(
sampler s0 : register(s0); // �����������
sampler s1 : register(s1); // �ڶ������������

bool bUseSM = true; // �Ƿ�ʹ��������
int texMax = 1; // �����������

float4 ps(float4 pos : POSITION, 
           float4 diff : COLOR0, 
           float2 tex0 : TEXCOORD0,
           float2 tex1 : TEXCOORD1
          ) : COLOR0
{
    float4 color = tex2D(s0, tex0); // �ӵ�һ�����������ɫ

    if (texMax > 0) // ����еڶ�������
    {
        float4 c = tex2D(s1, tex1); // �ӵڶ������������ɫ
        color = color * (1.f - c.a) + c * c.a; // ���л��
    }

    if (bUseSM) // �Ƿ�Ӧ��������
    {
        color.rgb *= color.rgb; // ����ɫ�������⴦��ʾ������ɫƽ����
    }
    
    color.rgb *= diff.rgb; // ����ɫ�� diffuse ��ɫ���
    return color; // ����������ɫ
}

technique tec
{
    pass p0
    {          
        VertexShader = NULL; // ��ʹ�ö�����ɫ��
        PixelShader  = compile ps_2_0 ps(); // ����������ɫ��
    }
}

)";
    
const char* bloomfx = R"(
sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);
sampler s4 : register(s4);
sampler s5 : register(s5);

static const int g_cKernelSize = 13;
float g_fIntensity = 0.5f;

float TexelOffset_h[g_cKernelSize];
float TexelOffset_v[g_cKernelSize];

static const float BlurWeights[g_cKernelSize] = 
{
    0.002216,
    0.008764,
    0.026995,
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216,
};

float4 ps_gauss_h(float4 pos : POSITION, float2 tex0 : TEXCOORD0) : COLOR0
{
	float4 color = 0;
	for (int i = 0; i < g_cKernelSize; ++i)
        	color += tex2D(s0, float2(tex0.x + TexelOffset_h[i], tex0.y)) * BlurWeights[i];
	return color;	
}

technique tec_gauss_h
{
    pass p0
    {          
        VertexShader = NULL;
        PixelShader  = compile ps_2_0 ps_gauss_h();
    }
}

float4 ps_gauss_v(float4 pos : POSITION, float2 tex0 : TEXCOORD0) : COLOR0
{
	float4 color = 0;
	for (int i = 0; i < g_cKernelSize; ++i)
        	color += tex2D(s0, float2(tex0.x, tex0.y + TexelOffset_v[i])) * BlurWeights[i];
	return color;	
}

technique tec_gauss_v
{
    pass p0
    {          
        VertexShader = NULL;
        PixelShader  = compile ps_2_0 ps_gauss_v();
    }
}

float4 ps(float4 pos : POSITION, float2 tex0 : TEXCOORD0) : COLOR0
{
	float4 color = 0;
	float4 c0 = tex2D(s0, tex0);
	float4 c1 = tex2D(s1, tex0);
	color = c0 + c1 * g_fIntensity;
	return color;	
}

technique tec
{
    pass p0
    {          
        VertexShader = NULL;
        PixelShader  = compile ps_2_0 ps();
    }
}
)";

const char* GetEffectCodeFile(RtgEffectType _Type) {
    switch (_Type) {
        case Rtg_Effect_Terrain:
            return "shader\\terrain.fx";
        case Rtg_Effect_Bloom:
            return "shader\\bloom.fx";
        default:
            break;
    }

    return 0;
}

const char* GetEffectCodeChars(RtgEffectType _Type) {
    switch (_Type) {
        case Rtg_Effect_Terrain:
            return terrainfx;
        case Rtg_Effect_Bloom:
            return bloomfx;
        default:
            break;
    }

    return 0;
}

//lmk ��Ƕ��ɫ��

}  // namespace rt_graph
