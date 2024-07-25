#include "core/rt2_core.h"
#include "graph/rt_graph.h"
#include "gc_include.h"
#include "gc_sequence_image.h"
// µÇÂ¼Ò³Ãæ¼ÓÔØÍ¼Æ¬
GcSequenceImage		g_LoadingMapRenderer;
GcSequenceImage		g_WorldMapRenderer;


GcSequenceImage::GcSequenceImage()
{
    m_bRenderFlag = false;
}

GcSequenceImage::~GcSequenceImage()
{
}

void GcSequenceImage::SetRenderFlag(bool bRender)
{
    m_bRenderFlag = bRender;

    if (!bRender && !m_listTex.empty())
    {

        map<string, RtgTextItem*>::iterator iter = m_listTex.begin();
        for (; iter!=m_listTex.end(); iter++)
        {
            RtgTextItem* texItem = iter->second;
            GetDevice()->GetTextMgr()->ReleaseTexture(texItem);
        }

        m_listTex.clear();
    }
}

void GcSequenceImage::DrawSingleImage(const char* szFileName, float x, float y, float width, float height)
{
    string strFilename(szFileName);

    RtgTextItem* texItem = NULL;
    map<string, RtgTextItem *>::iterator iter = m_listTex.find(strFilename);
    if (iter==m_listTex.end())
    {
        texItem = GetDevice()->GetTextMgr()->CreateUiTexture(szFileName);
        if (!texItem)
            return;
        m_listTex[szFileName] = texItem;
    }
    else 
    {
        texItem = iter->second;
    }

    DWORD dwColor = 0xFF303030;
    RtgNVertexVCT p[4];
    float iDltx = x;
    float iDlty = y;
    float iDrtx = iDltx + width;
    float iDrty = iDlty;
    float iDlbx = iDltx;
    float iDlby = iDlty + height;
    float iDrbx = iDrtx;
    float iDrby = iDlby;

    const float fUVMin = 0.004f, fUVMax = 0.996f;

    p[0] = RtgNVertexVCT( iDltx, iDlty, dwColor, fUVMin, 1-fUVMin);
    p[1] = RtgNVertexVCT( iDlbx, iDlby, dwColor, fUVMin, 1-fUVMax);
    p[2] = RtgNVertexVCT( iDrbx, iDrby, dwColor, fUVMax, 1-fUVMax);
    p[3] = RtgNVertexVCT( iDrtx, iDrty, dwColor, fUVMax, 1-fUVMin);

    RtgShader shader;
    shader.bEnableBlendFunc = false;
    shader.iNumStage = 1;
    shader.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
    //	shader.Shaders[0].eAlphaArg1 = RTGTA_DIFFUSE;
    shader.Shaders[0].SetTexture(texItem);
    BOOL bFog = GetDevice()->GetFogEnable();
    GetDevice()->SetFogEnable(FALSE);
    GetDevice()->SetShader(&shader);
    GetDevice()->SetVertexFormat(RTG_TRANSFORMED_VCT);
    GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &p, sizeof(RtgNVertexVCT), 2);
    GetDevice()->RestoreShader(&shader);
    GetDevice()->SetFogEnable(bFog);

//    g_pDevice->m_Texture.DeleteTexture(dwMapTexture);
}

void GcSequenceImage::DrawSingleImage(SequenceImageItem& sequence)
{
    DrawSingleImage(sequence.filename.c_str(), sequence.x, sequence.y, sequence.width, sequence.height);
}

void GcSequenceImage::SetImages(vector<SequenceImageItem>& sequence)
{
    m_images = sequence;
}

void GcSequenceImage::Render()
{
    if (!m_bRenderFlag)
        return;

    RTGTextureFilter magfliter = RtGetRender()->GetTextureFilterMag();
    RTGTextureFilter minfliter = RtGetRender()->GetTextureFilterMin();
    
    RtGetRender()->SetTextureFilterMag(RTGTEXF_LINEAR);
    RtGetRender()->SetTextureFilterMin(RTGTEXF_LINEAR);

    for (int i=0; i<(int)m_images.size(); i++)
        DrawSingleImage(m_images[i]);
    
    RtGetRender()->SetTextureFilterMag(magfliter);
    RtGetRender()->SetTextureFilterMin(minfliter);
}
