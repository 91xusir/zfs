//////////////////////////////////////////////////////////////////////////
//
//   FileName : rts_grass_render.cpp
//     Author : zxy
// CreateTime : 2009.12.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

class CRT_ActorInstance;

#include "graph/rt_graph.h"
#include "rt_scene.h"

RtsGrassRender::RtsGrassRender()
{
    m_Shader.bColorKey = true;
    m_Shader.alphaRef = 120;
    m_curIdx = 0;
    memset(m_apText, 0, sizeof(m_apText));
    
}

RtsGrassRender::~RtsGrassRender()
{
    Reset();
    ClearGrassTexture();
}

bool RtsGrassRender::Init(RtgDevice* pDev)
{
    if (!pDev)
        return false;

    m_pDevice = pDev;
    return true;
}

void RtsGrassRender::Render(RtgCamera* pCam)
{
    RTASSERT(pCam);

    if (m_mapGrass.size() <= 0)
        return;

    RtgDeviceD3D9* _Dev = RtGetRender();
    RTASSERT(_Dev);

    DWORD bLight = _Dev->GetRenderState(RTGRS_LIGHTING);
    DWORD dwCull = _Dev->GetRenderState(RTGRS_CULLMODE);

    _Dev->SetRenderState(RTGRS_LIGHTING, FALSE);
    _Dev->SetRenderState(RTGRS_CULLMODE, RTGCULL_NONE);

    const RtgVertex3& _Right = pCam->m_vAxisX;

    for (map<UINT, tileList>::iterator i = m_mapGrass.begin(); i != m_mapGrass.end(); ++i)
    {
        UINT texId = i->first;
        tileList& tlist = i->second;

        if (texId >= 0 && texId < Max_Grass_Text_Id && m_apText[texId])
            if (!m_apText[texId]->texItem)
                m_apText[texId]->texItem = RtGetRender()->GetTextMgr()->CreateTextureFromFile_Direct(m_apText[texId]->file.c_str()); 

        if (!m_apText[texId])
        {
            RtCoreLog().Warn("no grass texture (id:%u)\n", texId);
            continue;
        }

        if (m_apText[texId]->texItem->state != Object_State_Ready)
            continue;

        UINT uNumGrass = 0;
        vector<PassInfo> vecPass;
        PassInfo* pPass = NULL;

        vecPass.push_back(PassInfo());
        pPass = &vecPass.back();

        pPass->begin = tlist.begin();
        pPass->uNumGrass = 0;

        for (tileList::iterator t = tlist.begin(); t != tlist.end(); ++t)
        {
            uNumGrass += (*t)->grassDensity;
        
            if (pPass->uNumGrass + (*t)->grassDensity > Max_Grass_Pre_Pass)
            {
                pPass->end = t;
                vecPass.push_back(PassInfo());
                pPass = &vecPass.back();
                pPass->begin = t;
                pPass->uNumGrass = 0;
            }

            pPass->uNumGrass += (*t)->grassDensity;
        }

        pPass->end = tlist.end();

        if (uNumGrass <= 0)
            break;

        m_Shader.iNumStage = 1;
        if (m_apText[texId])
            m_Shader.Shaders[0].SetTexture(m_apText[texId]->texItem);

        RtgMatrix16 mWorld;
        mWorld.Unit();
        _Dev->SetMatrix(RTGTS_WORLD, &mWorld);

        _Dev->SetFVF(RtgVertexGrass::VF);
        _Dev->SetDefaultMaterial();
        _Dev->SetShader(&m_Shader);

        for (vector<PassInfo>::iterator iPass = vecPass.begin(); iPass != vecPass.end(); ++iPass)
        {
            const PassInfo& pf = (*iPass);

            RtgShareBufferItem vbItem;
            RtgShareBufferItem ibItem;

            RtgVertexGrass* vb = (RtgVertexGrass*)_Dev->GetBuffMgr()->Lock_Shared_vb(
                (DWORD)sizeof(RtgVertexGrass) * pf.uNumGrass * 8, &vbItem);

            WORD* ib = (WORD*)_Dev->GetBuffMgr()->Lock_Shared_ib(
                (DWORD)pf.uNumGrass * 12 * sizeof(short), &ibItem);

            DWORD iv = 0;
            DWORD ii = 0;

            for (tileList::iterator t = pf.begin; t != pf.end; ++t)
            {
                memcpy(&vb[iv], (*t)->grassVertex, sizeof(RtgVertexGrass) * 8 * (*t)->grassDensity);
                
                for (int j = 0; j < (*t)->grassDensity; ++j)
                {
                    ib[ii + 0] = iv + 0;
                    ib[ii + 1] = iv + 1;
                    ib[ii + 2] = iv + 2;

                    ib[ii + 3] = iv + 0;
                    ib[ii + 4] = iv + 2;
                    ib[ii + 5] = iv + 3;

                    ib[ii + 6] = iv + 4;
                    ib[ii + 7] = iv + 5;
                    ib[ii + 8] = iv + 6;

                    ib[ii +  9] = iv + 4;
                    ib[ii + 10] = iv + 6;
                    ib[ii + 11] = iv + 7;

                    iv += 8;
                    ii += 12;
                }               
            }

            _Dev->GetBuffMgr()->Unlock(&vbItem);
            _Dev->GetBuffMgr()->Unlock(&ibItem);
            _Dev->DrawIndexedPrimitive(
                D3DPT_TRIANGLELIST,
                &vbItem,
                sizeof(RtgVertexGrass),
                &ibItem,
                (UINT)pf.uNumGrass * 4
                );

            /*
            _Dev->SetStreamSource(0, _Dev->GetBuffMgr()->GetShared_vb(), sizeof(RtgVertexGrass));
            _Dev->SetIndices(_Dev->GetBuffMgr()->GetShared_ib());
            _Dev->DrawIndexedPrimitive(
                D3DPT_TRIANGLELIST,
                0,
                0,
                (UINT)pf.uNumGrass * 8,
                0,
                (UINT)pf.uNumGrass * 4
                );
                */
        }
    }

    _Dev->SetRenderState(RTGRS_LIGHTING, bLight);
    _Dev->SetRenderState(RTGRS_CULLMODE, dwCull);
}

void RtsGrassRender::Reset()
{
    m_mapGrass.clear();
}

void RtsGrassRender::CalcGrassTextCoord(RtgVertex4* uv, WORD tId)
{
    RTASSERT(uv);

    uv->Set(0.f, 0.f, 1.f, 1.f);

    byte _id = _TexId(tId);
    byte _ix = _SubIx(tId);

    if (_id < 0 || _id >= Max_Grass_Text_Id || !m_apText[_id])
        return;

    int x = m_apText[_id]->sub_x;
    int y = m_apText[_id]->sub_y;

    float xStep = 1.f / x;
    float yStep = 1.f / y;

    if (_ix < 0 || _ix >= x * y)
        return;

    y = _ix / x;
    x = _ix % x;

    uv->Set(x * xStep, y * yStep, x * xStep + xStep, y * yStep + yStep);
    
}

bool RtsGrassRender::LoadGrassTexture(const char* fileName) 
{
    ClearGrassTexture();

    if (!ParserFile(fileName))
        return false;

    return true;
}

void RtsGrassRender::ClearGrassTexture()
{
    for (int i = 0; i < Max_Grass_Text_Id; ++i)
    {
        if (m_apText[i])
        {
            RtGetRender()->GetTextMgr()->ReleaseTexture(m_apText[i]->texItem);
            DEL_ONE(m_apText[i]);
        }
    }        
}

void RtsGrassRender::OnTagStart(const char* tag)
{   
    m_curText.name  = "";
    m_curText.file  = "";
    m_curText.sub_x = 1;
    m_curText.sub_y = 1;
    m_curText.texItem = NULL;
}

void RtsGrassRender::OnAttribute(const char* atr, const char* val)
{
    if (stricmp(atr, "id")==0)
    {
        m_curIdx = atoi(val);

        if (m_curIdx < 0)
            m_curIdx = 0;
        if (m_curIdx >= Max_Grass_Text_Id)
            m_curIdx = Max_Grass_Text_Id - 1;
    }
    else if (stricmp(atr, "name")==0)
    {
        m_curText.name = val;
    }
    else if (stricmp(atr, "file")==0)
    {
        m_curText.file = val;
    }
    else if (stricmp(atr, "x")==0)
    {
        m_curText.sub_x = atoi(val);
    }
    else if (stricmp(atr, "y")==0)
    {
        m_curText.sub_y = atoi(val);
    }
}

void RtsGrassRender::OnTagEnd()
{ 
    char texPath[Text_File_Path_Size];

    if (m_curText.name.empty())
        m_curText.name = m_curText.file;
    if (!m_apText[m_curIdx])
        m_apText[m_curIdx] = RT_NEW RtgGrassText;
    (*m_apText[m_curIdx]) = m_curText; 
    
    RtGetRender()->GetTextMgr()->GetTextFilePath(texPath, m_apText[m_curIdx]->file.c_str());
    m_apText[m_curIdx]->file = texPath;
    RTASSERT(strlen(texPath) < Text_File_Path_Size);

}
