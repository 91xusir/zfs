
#include "graph/rt_graph.h"
#include "gc_dnumber.h"
#include "gc_include.h"

GcDynamicNumber::GcDynamicNumber()
{
    m_fSizeX = 3.f;
	m_fSizeY = 4.f;
	m_fSizeCharX = 5;
	m_fSizeCharY = 3;
    m_iFirst = 0;
    m_iLast = 0;
    m_iCnt = 0;
    m_iCntChar = 0;
    m_fSpeed = 20.f;
    m_fTime  = 2.f;
    memset(m_Node, 0, sizeof(SNumNode)*MAX_NUM_NODE);
	memset(m_pUVs, 0, sizeof(SCharUV*)*128);
	memset(m_pUVsChar, 0, sizeof(SCharUV*)*128);

    m_pPaneVertex[0].Set(-m_fSizeX, -m_fSizeY, 0.f);
    m_pPaneVertex[1].Set(-m_fSizeX,  m_fSizeY, 0.f);
    m_pPaneVertex[2].Set( m_fSizeX,  m_fSizeY, 0.f);
    m_pPaneVertex[3].Set( m_fSizeX,  m_fSizeY, 0.f);
    m_pPaneVertex[4].Set( m_fSizeX, -m_fSizeY, 0.f);
	m_pPaneVertex[5].Set(-m_fSizeX, -m_fSizeY, 0.f);

	m_pPaneVertexChar[0].Set(-m_fSizeCharX, -m_fSizeCharY, 0.f);
	m_pPaneVertexChar[1].Set(-m_fSizeCharX,  m_fSizeCharY, 0.f);
	m_pPaneVertexChar[2].Set( m_fSizeCharX,  m_fSizeCharY, 0.f);
	m_pPaneVertexChar[3].Set( m_fSizeCharX,  m_fSizeCharY, 0.f);
	m_pPaneVertexChar[4].Set( m_fSizeCharX, -m_fSizeCharY, 0.f);
	m_pPaneVertexChar[5].Set(-m_fSizeCharX, -m_fSizeCharY, 0.f);
}

GcDynamicNumber::~GcDynamicNumber()
{
}

void GcDynamicNumber::SetNumberTexture(int iXCnt, int iYCnt, const char* szChars)
{
    int iCnt = (int)strlen(szChars);
    if (iCnt>MAX_CHAR) iCnt = MAX_CHAR;
    memset(m_pUVs, 0, sizeof(SCharUV*)*128);

    int i;
    float x, y, xStep, yStep;
    xStep = 1.f/iXCnt;
    yStep = 1.f/iYCnt;
    for (i=0; i<iCnt; i++)
    {
        x = (i%iXCnt)*xStep;
        y = (i/iXCnt)*yStep;

		m_UVs[i].uvs[0].Set(      x, 1-(      y));
		m_UVs[i].uvs[1].Set(      x, 1-(y+yStep));
		m_UVs[i].uvs[2].Set(x+xStep, 1-(y+yStep));
		m_UVs[i].uvs[3].Set(x+xStep, 1-(y+yStep));
		m_UVs[i].uvs[4].Set(x+xStep, 1-(      y));
		m_UVs[i].uvs[5].Set(      x, 1-(      y));

        m_pUVs[szChars[i]] = m_UVs+i;
    }
}

void GcDynamicNumber::SetCharTexture(int iXCnt, int iYCnt, const char* szChars)
{
	int iCnt = (int)strlen(szChars);
	if (iCnt>MAX_CHAR) iCnt = MAX_CHAR;
	memset(m_pUVsChar, 0, sizeof(SCharUV*)*128);

	int i;
	float x, y, xStep, yStep;
	xStep = 1.f/iXCnt;
	yStep = 1.f/iYCnt;
	for (i=0; i<iCnt; i++)
	{
		x = (i%iXCnt)*xStep;
		y = (i/iXCnt)*yStep;

		m_UVsChar[i].uvs[0].Set(      x, 1-(      y));
		m_UVsChar[i].uvs[1].Set(      x, 1-(y+yStep));
		m_UVsChar[i].uvs[2].Set(x+xStep, 1-(y+yStep));
		m_UVsChar[i].uvs[3].Set(x+xStep, 1-(y+yStep));
		m_UVsChar[i].uvs[4].Set(x+xStep, 1-(      y));
		m_UVsChar[i].uvs[5].Set(      x, 1-(      y));

		m_pUVsChar[szChars[i]] = m_UVsChar+i;
	}
}

void GcDynamicNumber::InitOnce()
{
	//SetNumberTexture(4, 4, "1234567890-+miss");
	SetNumberTexture(4, 4, "ssim90-+56781234");
	SetCharTexture(3, 5, "0123456789abcde");
	//SetCharTexture(3, 5, "经帮烫吸门烫抵声烫格修烫闪灵烫");

	for (int i=0;i<PIC_MAX;i++)
	{
		m_Shader[i].bTwoSide = true;
		m_Shader[i].iNumStage = 1;

		char path[32];
		rt2_sprintf(path,"num%d",i);
		m_Shader[i].Shaders[0].SetTexture(GetDevice()->GetTextMgr()->CreateTextureFromFile(path));

		//m_Shader[i].Shaders[0].eAlphaOp   = RTGTOP_MODULATE;
		m_Shader[i].Shaders[0].eAlphaArg2 = RTGTA_TEXTURE;
		m_Shader[i].Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
		m_Shader[i].Shaders[0].eColorArg1 = RTGTA_TEXTURE;
		m_Shader[i].Shaders[0].eColorArg2 = RTGTA_DIFFUSE;

		m_Shader[i].bEnableBlendFunc = true;
		m_Shader[i].eBlendSrcFunc = RTGBLEND_SRCALPHA;
		m_Shader[i].eBlendDstFunc = RTGBLEND_INVSRCALPHA;
	}
}

void GcDynamicNumber::ClearOnce()
{
	for (int i=0;i<PIC_MAX;i++)
		GetDevice()->DeleteShader(&m_Shader[i]);
}

void GcDynamicNumber::AddString(const char* szString, const char* szNumber, const RtgVertex3& vPos,float fScale/* = 1.f*/, bool spec/* = false*/, char cDir/* = DIR_MIDDLE*/, char type/* = PIC_NORMAL */)
{
	if (m_iCnt>=MAX_NUM_NODE)
	{
		m_iCnt --;
		m_iCntChar -= (int)strlen(m_Node[m_iFirst].szNum);
		m_iFirst ++;
		m_iFirst = m_iFirst%MAX_NUM_NODE;
	}

	m_Node[m_iLast].vPos = vPos;
	m_Node[m_iLast].fTime = m_fTime;
	m_Node[m_iLast].dwColor = 0xFFFFFFFF;
	m_Node[m_iLast].spec = spec;
	//strncpy(m_Node[m_iLast].szNum, szString, 15);
	//m_Node[m_iLast].szNum[15] = 0;
	rt2_strncpy(m_Node[m_iLast].szNum, szString, 16);
	m_Node[m_iLast].cType = PIC_CHAR;
	m_Node[m_iLast].cDir  = cDir;
	m_Node[m_iLast].fScale = 2.f;
	m_iCntChar += (int)strlen(m_Node[m_iLast].szNum);

	m_iCnt ++;
	m_iLast ++;
	m_iLast = m_iLast%MAX_NUM_NODE;

	if (m_iCnt>=MAX_NUM_NODE)
	{
		m_iCnt --;
		m_iCntChar -= (int)strlen(m_Node[m_iFirst].szNum);
		m_iFirst ++;
		m_iFirst = m_iFirst%MAX_NUM_NODE;
	}

	m_Node[m_iLast].vPos = vPos;
	m_Node[m_iLast].fTime = m_fTime;
	m_Node[m_iLast].dwColor = 0xFFFFFFFF;
	m_Node[m_iLast].spec = spec;
	/*strncpy(m_Node[m_iLast].szNum, szNumber, 15);
	m_Node[m_iLast].szNum[15] = 0;*/
	rt2_strncpy(m_Node[m_iLast].szNum, szNumber, 16);
	m_Node[m_iLast].cType = PIC_NORMAL;
	m_Node[m_iLast].cDir  = cDir;
	m_Node[m_iLast].fScale = fScale;
	m_iCntChar += (int)strlen(m_Node[m_iLast].szNum);

	// 如果与上一次数字距离过近，就把上一个数字顶上去
	int prev = (m_iLast-1+MAX_NUM_NODE)%MAX_NUM_NODE;
	m_Node[m_iLast].pBindNode = &m_Node[prev];
	m_iCnt ++;
	m_iLast ++;
	m_iLast = m_iLast%MAX_NUM_NODE;
}

void GcDynamicNumber::AddString(const char* szString, const RtgVertex3& vPos,float fScale/* = 1.f*/, bool spec/* = false*/, char cDir/* = DIR_MIDDLE*/, char type/* = PIC_NORMAL */)
{
	if (m_iCnt>=MAX_NUM_NODE)
	{
		m_iCnt --;
		m_iCntChar -= (int)strlen(m_Node[m_iFirst].szNum);
		m_iFirst ++;
		m_iFirst = m_iFirst%MAX_NUM_NODE;
	}

	m_Node[m_iLast].vPos = vPos;
	m_Node[m_iLast].fTime = m_fTime;
	m_Node[m_iLast].dwColor = 0xFFFFFFFF;
	m_Node[m_iLast].spec = spec;
	rt2_strncpy(m_Node[m_iLast].szNum, szString, 16);
	//m_Node[m_iLast].szNum[15] = 0;
	m_Node[m_iLast].cType = type;
	m_Node[m_iLast].cDir  = cDir;
	m_Node[m_iLast].fScale = fScale;
	m_iCntChar += (int)strlen(m_Node[m_iLast].szNum);

	// 如果与上一次数字距离过近，就把上一个数字顶上去
	int prev = (m_iLast-1+MAX_NUM_NODE)%MAX_NUM_NODE;
	if (m_Node[prev].cDir == DIR_MIDDLE || m_Node[prev].cDir == cDir)
	{
		if(m_Node[prev].vPos.z >= vPos.z && m_Node[prev].vPos.z - vPos.z < 5)
			m_Node[prev].vPos.z += 5;
	}
	m_iCnt ++;
	m_iLast ++;
	m_iLast = m_iLast%MAX_NUM_NODE;
}

void GcDynamicNumber::AddString(const char* szString, const RtgVertex3& vPos, DWORD dwColor, float fScale, bool spec, char type)
{
    if (m_iCnt>=MAX_NUM_NODE)
    {
        m_iCnt --;
        m_iCntChar -= (int)strlen(m_Node[m_iFirst].szNum);
        m_iFirst ++;
        m_iFirst = m_iFirst%MAX_NUM_NODE;
    }

    m_Node[m_iLast].vPos = vPos;
    m_Node[m_iLast].fTime = m_fTime;
    m_Node[m_iLast].dwColor = dwColor;
	m_Node[m_iLast].spec = true;
    rt2_strncpy(m_Node[m_iLast].szNum, szString, 16);
    //m_Node[m_iLast].szNum[15] = 0;
	m_Node[m_iLast].cType = type;
	m_Node[m_iLast].cDir  = DIR_MIDDLE;
	m_Node[m_iLast].fScale = fScale;
    m_iCntChar += (int)strlen(m_Node[m_iLast].szNum);

	// 如果与上一次数字距离过近，就把上一个数字顶上去
	int prev = (m_iLast-1+MAX_NUM_NODE)%MAX_NUM_NODE;
	if (m_Node[prev].cDir == DIR_MIDDLE)
		if(m_Node[prev].vPos.z >= vPos.z && m_Node[prev].vPos.z - vPos.z < 5)
			m_Node[prev].vPos.z += 5;
    m_iCnt ++;
    m_iLast ++;
    m_iLast = m_iLast%MAX_NUM_NODE;
}

void GcDynamicNumber::Render(RtgCamera& inCamera, RtgDevice& inDevice, float fSecond)
{
    int i, j, k, c, iCnt;
    //DWORD dwRes;
    BYTE* pVB;
    SCharUV* pUV;
    RtgVertex3 vOffset;
    vOffset.Zero();
    RtgDeviceD3D9* dev = RtGetRender();

	/*
	DWORD op;
	op = inDevice.GetRenderState(RTGRS_DIFFUSEMATERIALSOURCE);
	inDevice.SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,RTGMCS_COLOR1);
    */
	// Render
    if (m_iCnt) 
    {
        RtgMatrix16 m16;
        DWORD dwStride = sizeof(float)*3 + sizeof(DWORD) + sizeof(float)*2;

		RtgShader *render = NULL;
		int ishader = 0;
		std::vector<SNumNode*> vNumNode[PIC_MAX];

		for (i=0; i<MAX_NUM_NODE; i++)
		{
			if (m_Node[i].fTime > 0.f)
			{
				vNumNode[m_Node[i].cType].push_back(&m_Node[i]);
			}
		}

		for (int count=0;count<PIC_MAX;count++)
        {
			iCnt = 0;
            RtgShareBufferItem _vbItem;
            pVB = (BYTE*)dev->GetBuffMgr()->Lock_Shared_vb( m_iCntChar*dwStride*6, &_vbItem);

			for (int i=0;i<vNumNode[count].size();i++)
			{
				m16 = inCamera.m_matBillboard;
				int alpha = (vNumNode[count][i]->fTime)*255;
				if(alpha > 255) alpha = 255;

				vNumNode[count][i]->dwColor =  vNumNode[count][i]->dwColor & 0x00ffffff | (alpha << 24) ;

				if (vNumNode[count][i]->cDir == DIR_RIGHTUP)
				{
					m16.SetRow(3, vNumNode[count][i]->vPos);
					float fTmpX = 0.f;
					float fTmpY = 0.f;
					m16.TranslateY(-8.f);
					fTmpX = m_fTime - vNumNode[count][i]->fTime;
					fTmpY = -1.f * fTmpX * fTmpX + 2.f * fTmpX;
					m16.TranslateX(fTmpX * 10.f);
					m16.TranslateY(fTmpY * 10.f);
					m16.Scale(vNumNode[count][i]->fScale, vNumNode[count][i]->fScale, vNumNode[count][i]->fScale);
				}
				else if (vNumNode[count][i]->cDir == DIR_LEFTUP)
				{
					m16.SetRow(3, vNumNode[count][i]->vPos);
					float fTmpX = 0.f;
					float fTmpY = 0.f;
					m16.TranslateY(-8.f);
					fTmpX = vNumNode[count][i]->fTime - m_fTime;
					fTmpY = -1.f * fTmpX * fTmpX - 2.f * fTmpX;
					m16.TranslateX(fTmpX * 10.f);
					m16.TranslateY(fTmpY * 10.f);
					m16.Scale(vNumNode[count][i]->fScale, vNumNode[count][i]->fScale, vNumNode[count][i]->fScale);
				}
				else if (vNumNode[count][i]->cDir == DIR_LEFT)
				{
					m16.SetRow(3, vNumNode[count][i]->vPos);
					m16.TranslateX((m_fTime - vNumNode[count][i]->fTime) * (m_fTime - vNumNode[count][i]->fTime) * -10.f);
					m16.TranslateY(-10.f);
					if (vNumNode[count][i]->cType != PIC_CHAR)
					{
						m16.TranslateX(9.f);
					}
					m16.Scale(vNumNode[count][i]->fScale, vNumNode[count][i]->fScale, vNumNode[count][i]->fScale);
				}
				else if (vNumNode[count][i]->cDir == DIR_RIGHT)
				{
					m16.SetRow(3, vNumNode[count][i]->vPos);
					m16.TranslateX((m_fTime - vNumNode[count][i]->fTime) * (m_fTime - vNumNode[count][i]->fTime) * 10.f);
					m16.TranslateY(-10.f);
					if (vNumNode[count][i]->cType != PIC_CHAR)
					{
						m16.TranslateX(9.f);
					}
					m16.Scale(vNumNode[count][i]->fScale, vNumNode[count][i]->fScale, vNumNode[count][i]->fScale);
				}
				else
				{
					vNumNode[count][i]->vPos.z += m_fSpeed * fSecond;
					m16.SetRow(3, vNumNode[count][i]->vPos);
					if(vNumNode[count][i]->spec)
					{
						float fScale = (fsin(DegreeToRadian((vNumNode[count][i]->fTime*225.f/m_fTime)-90.f))+1.f);
						m16.Scale(fScale, fScale, fScale);
					}
				}

				c = (int)strlen(vNumNode[count][i]->szNum);
				if (count == PIC_CHAR)
					vOffset.x = 0.f - (m_fSizeCharX * (c/2)) - ((c%2)?(m_fSizeCharX/2.f):0);
				else
					vOffset.x = 0.f - (m_fSizeX * (c/2)) - ((c%2)?(m_fSizeX/2.f):0);
				for (k=0; k<c; k++)
				{
					if (count == PIC_CHAR)
						pUV = m_pUVsChar[vNumNode[count][i]->szNum[k]];
					else
						pUV = m_pUVs[vNumNode[count][i]->szNum[k]];
					if (pUV)
					{
						iCnt ++;
						for (j=0; j<6; j++)
						{
							if (count == PIC_CHAR)
								*((RtgVertex3*)pVB) = (m_pPaneVertexChar[j]+vOffset) * m16;
							else
								*((RtgVertex3*)pVB) = (m_pPaneVertex[j]+vOffset) * m16;
							pVB += sizeof(float)*3;
							*((DWORD*)pVB) = vNumNode[count][i]->dwColor;
							pVB += sizeof(DWORD);
							memcpy(pVB, pUV->uvs+j, sizeof(RtgVertex2));
							pVB += sizeof(RtgVertex2);
						}
					}
					vOffset.x += m_fSizeX;
				}
			}

            dev->GetBuffMgr()->Unlock(&_vbItem);

			if (iCnt > 0)
			{
				render = &m_Shader[count];
				m16.Unit();
				GetDevice()->SetMatrix(RTGTS_WORLD, &m16);


				DWORD dwLighting = GetDevice()->GetRenderState(RTGRS_LIGHTING);
				DWORD dwZTest    = GetDevice()->GetRenderState(RTGRS_Z_TEST);
				DWORD dwZWrite   = GetDevice()->GetRenderState(RTGRS_Z_WRITE);

				GetDevice()->SetRenderState(RTGRS_LIGHTING, FALSE);
				GetDevice()->SetRenderState(RTGRS_Z_TEST, FALSE);
				GetDevice()->SetRenderState(RTGRS_Z_WRITE, FALSE);

                inDevice.SetShader(render);
                inDevice.SetVertexFormat(RTG_VCT);
                dev->DrawPrimitive(D3DPT_TRIANGLELIST, &_vbItem, dwStride, iCnt * 2);
				inDevice.RestoreShader(render);

				GetDevice()->SetRenderState(RTGRS_LIGHTING, dwLighting);
				GetDevice()->SetRenderState(RTGRS_Z_TEST, dwZTest);
				GetDevice()->SetRenderState(RTGRS_Z_WRITE, dwZWrite);
			}
		}
	}

    // Delete
    for (i=0; i<MAX_NUM_NODE; i++)
    {
        if (m_Node[i].fTime>0.f)
        {
            m_Node[i].fTime -= fSecond;
            if (m_Node[i].fTime<=0.f)
            {
                m_iCnt --;
                m_iCntChar -= (int)strlen(m_Node[i].szNum);
                m_iFirst ++;
            }
        }
    }
    CHECK(m_iCnt>=0);
    m_iFirst = m_iFirst%MAX_NUM_NODE;
}

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
