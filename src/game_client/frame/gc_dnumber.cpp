
#include "graph/rt_graph.h"
#include "gc_dnumber.h"
#include "gc_include.h"

GcDynamicNumber::GcDynamicNumber() {
    // �������ֻ�������Ŀ�Ⱥ͸߶�
    m_fSizeX = 3.f;  // ���Ŀ��
    m_fSizeY = 5.f;  // ���ĸ߶�

    // �����ַ����Ŀ�Ⱥ͸߶�
    m_fSizeCharX = 5;  // �ַ����Ŀ��
    m_fSizeCharY = 3;  // �ַ����ĸ߶�

    // ��ʼ��������ʼ�ͽ����������Լ�������
    m_iFirst   = 0;  // ��ʼ����
    m_iLast    = 0;  // ��������
    m_iCnt     = 0;  // ���ּ���
    m_iCntChar = 0;  // �ַ�����

    // ��ʼ���ٶȺ�ʱ�����
    m_fSpeed = 30.f;  // �����ٶ�
    m_fTime  = 1.f;   // ��������ʱ��

    // ������ֽڵ����飬������Ԫ������Ϊ 0
    memset(m_Node, 0, sizeof(SNumNode) * MAX_NUM_NODE);

    // ����ַ� UV ָ�����飬������ָ������Ϊ nullptr
    memset(m_pUVs, 0, sizeof(SCharUV*) * 128);

    // ����ַ� UV ָ�����飬������ָ������Ϊ nullptr
    memset(m_pUVsChar, 0, sizeof(SCharUV*) * 128);

    // �������ֻ�������Ķ�������
    m_pPaneVertex[0].Set(-m_fSizeX, -m_fSizeY, 0.f);  // ���½�
    m_pPaneVertex[1].Set(-m_fSizeX, m_fSizeY, 0.f);   // ���Ͻ�
    m_pPaneVertex[2].Set(m_fSizeX, m_fSizeY, 0.f);    // ���Ͻ�
    m_pPaneVertex[3].Set(m_fSizeX, m_fSizeY, 0.f);    // ���Ͻǣ��ظ���
    m_pPaneVertex[4].Set(m_fSizeX, -m_fSizeY, 0.f);   // ���½�
    m_pPaneVertex[5].Set(-m_fSizeX, -m_fSizeY, 0.f);  // ���½ǣ��ظ���

    // �����ַ����Ķ�������
    m_pPaneVertexChar[0].Set(-m_fSizeCharX, -m_fSizeCharY, 0.f);  // ���½�
    m_pPaneVertexChar[1].Set(-m_fSizeCharX, m_fSizeCharY, 0.f);   // ���Ͻ�
    m_pPaneVertexChar[2].Set(m_fSizeCharX, m_fSizeCharY, 0.f);    // ���Ͻ�
    m_pPaneVertexChar[3].Set(m_fSizeCharX, m_fSizeCharY, 0.f);    // ���Ͻǣ��ظ���
    m_pPaneVertexChar[4].Set(m_fSizeCharX, -m_fSizeCharY, 0.f);   // ���½�
    m_pPaneVertexChar[5].Set(-m_fSizeCharX, -m_fSizeCharY, 0.f);  // ���½ǣ��ظ���
}

GcDynamicNumber::~GcDynamicNumber() {}

void GcDynamicNumber::SetNumberTexture(int iXCnt, int iYCnt, const char* szChars) {
    // �����ַ�����ĳ���
    int iCnt = (int)strlen(szChars);

    // ����ַ�����ĳ��ȳ���������ַ�������������Ϊ����ַ�����
    if (iCnt > MAX_CHAR)
        iCnt = MAX_CHAR;

    // ��� UV ָ������
    memset(m_pUVs, 0, sizeof(SCharUV*) * 128);

    int   i;
    float x, y, xStep, yStep;
    // ����ÿ������Ԫ�Ŀ�Ⱥ͸߶�
    xStep = 1.f / iXCnt;
    yStep = 1.f / iYCnt;

    // ����ÿ���ַ����������Ӧ�� UV ����
    for (i = 0; i < iCnt; i++) {
        // ���㵱ǰ�ַ�������λ��
        x = (i % iXCnt) * xStep;
        y = (i / iXCnt) * yStep;

        // ���õ�ǰ�ַ��� UV ����
        m_UVs[i].uvs[0].Set(x, 1 - (y));                  // ���Ͻ�
        m_UVs[i].uvs[1].Set(x, 1 - (y + yStep));          // ���½�
        m_UVs[i].uvs[2].Set(x + xStep, 1 - (y + yStep));  // ���½�
        m_UVs[i].uvs[3].Set(x + xStep, 1 - (y + yStep));  // ���½ǣ��ظ���
        m_UVs[i].uvs[4].Set(x + xStep, 1 - (y));          // ���Ͻ�
        m_UVs[i].uvs[5].Set(x, 1 - (y));                  // ���Ͻǣ��ظ���

        // ���ַ��� UV ���ݴ洢��ָ��������
        m_pUVs[szChars[i]] = m_UVs + i;
    }
}

void GcDynamicNumber::SetCharTexture(int iXCnt, int iYCnt, const char* szChars) {
    int iCnt = (int)strlen(szChars);
    if (iCnt > MAX_CHAR)
        iCnt = MAX_CHAR;
    memset(m_pUVsChar, 0, sizeof(SCharUV*) * 128);

    int   i;
    float x, y, xStep, yStep;
    xStep = 1.f / iXCnt;
    yStep = 1.f / iYCnt;
    for (i = 0; i < iCnt; i++) {
        x = (i % iXCnt) * xStep;
        y = (i / iXCnt) * yStep;

        m_UVsChar[i].uvs[0].Set(x, 1 - (y));
        m_UVsChar[i].uvs[1].Set(x, 1 - (y + yStep));
        m_UVsChar[i].uvs[2].Set(x + xStep, 1 - (y + yStep));
        m_UVsChar[i].uvs[3].Set(x + xStep, 1 - (y + yStep));
        m_UVsChar[i].uvs[4].Set(x + xStep, 1 - (y));
        m_UVsChar[i].uvs[5].Set(x, 1 - (y));

        m_pUVsChar[szChars[i]] = m_UVsChar + i;
    }
}

void GcDynamicNumber::InitOnce() {
    // �����������������ֱ�Ϊ����Ŀ�ߺ����������ַ���
    // �������������ֺͷ��ŵ�����
    // SetNumberTexture(4, 4, "1234567890-+miss");
    SetNumberTexture(4, 4, "ssim90-+56781234");  // ���õ��������ݰ����Զ����ַ�������

    // �����ַ����������ֱ�Ϊ����Ŀ�ߺ����������ַ���
    // �����������ַ�������
    SetCharTexture(3, 5, "0123456789abcde");  // ���õ��������ݰ������ֺ��ַ�

    // �������ڵ��Ի���ʾ������ַ�������ע�͵���
    // SetCharTexture(3, 5, "�����������̵����̸�����������");

    // ��ʼ������ Shader ����
    for (int i = 0; i < PIC_MAX; i++) {
        // ����˫����Ⱦ
        m_Shader[i].bTwoSide = true;
        // ������Ⱦ�׶���Ϊ 1
        m_Shader[i].iNumStage = 1;

        char path[32];
        // ��ʽ���ַ��������������ļ�·��
        rt2_sprintf(path, "num%d", i);
        // ���ļ������������õ� Shader
        m_Shader[i].Shaders[0].SetTexture(GetDevice()->GetTextMgr()->CreateTextureFromFile(path));

        // ���� Shader ���������
        // m_Shader[i].Shaders[0].eAlphaOp   = RTGTOP_MODULATE;  // (��ע��) Alpha ����ģʽ
        m_Shader[i].Shaders[0].eAlphaArg2 = RTGTA_TEXTURE;  // Alpha �ڶ�������Ϊ����
        m_Shader[i].Shaders[0].eColorOp = RTGTOP_SELECTARG1;  // ��ɫ����ģʽѡ���һ������
        m_Shader[i].Shaders[0].eColorArg1 = RTGTA_TEXTURE;  // ��ɫ��һ������Ϊ����
        m_Shader[i].Shaders[0].eColorArg2 = RTGTA_DIFFUSE;  // ��ɫ�ڶ�������Ϊ������

        // ���û�Ϲ���
        m_Shader[i].bEnableBlendFunc = true;
        // ���û��Դ��Ŀ������
        m_Shader[i].eBlendSrcFunc = RTGBLEND_SRCALPHA;     // ���Դ����ΪԴ Alpha ֵ
        m_Shader[i].eBlendDstFunc = RTGBLEND_INVSRCALPHA;  // ���Ŀ������Ϊ 1 - Դ Alpha ֵ
    }
}

void GcDynamicNumber::ClearOnce() {
    for (int i = 0; i < PIC_MAX; i++)
        GetDevice()->DeleteShader(&m_Shader[i]);
}

void GcDynamicNumber::AddString(const char* szString, const char* szNumber, const RtgVertex3& vPos,
                                float fScale /* = 1.f*/, bool spec /* = false*/,
                                char cDir /* = DIR_MIDDLE*/, char type /* = PIC_NORMAL */) {
    if (m_iCnt >= MAX_NUM_NODE) {
        m_iCnt--;
        m_iCntChar -= (int)strlen(m_Node[m_iFirst].szNum);
        m_iFirst++;
        m_iFirst = m_iFirst % MAX_NUM_NODE;
    }

    m_Node[m_iLast].vPos    = vPos;
    m_Node[m_iLast].fTime   = m_fTime;
    m_Node[m_iLast].dwColor = 0xFFFFFFFF;
    m_Node[m_iLast].spec    = spec;
    //strncpy(m_Node[m_iLast].szNum, szString, 15);
    //m_Node[m_iLast].szNum[15] = 0;
    rt2_strncpy(m_Node[m_iLast].szNum, szString, 16);
    m_Node[m_iLast].cType  = PIC_CHAR;
    m_Node[m_iLast].cDir   = cDir;
    m_Node[m_iLast].fScale = 2.f;
    m_iCntChar += (int)strlen(m_Node[m_iLast].szNum);

    m_iCnt++;
    m_iLast++;
    m_iLast = m_iLast % MAX_NUM_NODE;

    if (m_iCnt >= MAX_NUM_NODE) {
        m_iCnt--;
        m_iCntChar -= (int)strlen(m_Node[m_iFirst].szNum);
        m_iFirst++;
        m_iFirst = m_iFirst % MAX_NUM_NODE;
    }

    m_Node[m_iLast].vPos    = vPos;
    m_Node[m_iLast].fTime   = m_fTime;
    m_Node[m_iLast].dwColor = 0xFFFFFFFF;
    m_Node[m_iLast].spec    = spec;
    /*strncpy(m_Node[m_iLast].szNum, szNumber, 15);
	m_Node[m_iLast].szNum[15] = 0;*/
    rt2_strncpy(m_Node[m_iLast].szNum, szNumber, 16);
    m_Node[m_iLast].cType  = PIC_NORMAL;
    m_Node[m_iLast].cDir   = cDir;
    m_Node[m_iLast].fScale = fScale;
    m_iCntChar += (int)strlen(m_Node[m_iLast].szNum);

    // �������һ�����־���������Ͱ���һ�����ֶ���ȥ
    int prev                  = (m_iLast - 1 + MAX_NUM_NODE) % MAX_NUM_NODE;
    m_Node[m_iLast].pBindNode = &m_Node[prev];
    m_iCnt++;
    m_iLast++;
    m_iLast = m_iLast % MAX_NUM_NODE;
}

void GcDynamicNumber::AddString(const char* szString, const RtgVertex3& vPos,
                                float fScale /* = 1.f*/, bool spec /* = false*/,
                                char cDir /* = DIR_MIDDLE*/, char type /* = PIC_NORMAL */) {
    if (m_iCnt >= MAX_NUM_NODE) {
        m_iCnt--;
        m_iCntChar -= (int)strlen(m_Node[m_iFirst].szNum);
        m_iFirst++;
        m_iFirst = m_iFirst % MAX_NUM_NODE;
    }

    m_Node[m_iLast].vPos    = vPos;
    m_Node[m_iLast].fTime   = m_fTime;
    m_Node[m_iLast].dwColor = 0xFFFFFFFF;
    m_Node[m_iLast].spec    = spec;
    rt2_strncpy(m_Node[m_iLast].szNum, szString, 16);
    //m_Node[m_iLast].szNum[15] = 0;
    m_Node[m_iLast].cType  = type;
    m_Node[m_iLast].cDir   = cDir;
    m_Node[m_iLast].fScale = fScale;
    m_iCntChar += (int)strlen(m_Node[m_iLast].szNum);

    // �������һ�����־���������Ͱ���һ�����ֶ���ȥ
    int prev = (m_iLast - 1 + MAX_NUM_NODE) % MAX_NUM_NODE;
    if (m_Node[prev].cDir == DIR_MIDDLE || m_Node[prev].cDir == cDir) {
        if (m_Node[prev].vPos.z >= vPos.z && m_Node[prev].vPos.z - vPos.z < 5)
            m_Node[prev].vPos.z += 5;
    }
    m_iCnt++;
    m_iLast++;
    m_iLast = m_iLast % MAX_NUM_NODE;
}

void GcDynamicNumber::AddString(const char* szString, const RtgVertex3& vPos, DWORD dwColor,
                                float fScale, bool spec, char type) {
    // ��鵱ǰ�ڵ������Ƿ��Ѵﵽ���ֵ
    if (m_iCnt >= MAX_NUM_NODE) {
        // ����ڵ������ﵽ���ֵ���Ƴ�������ӵĽڵ�
        m_iCnt--;                                           // ���ٽڵ����
        m_iCntChar -= (int)strlen(m_Node[m_iFirst].szNum);  // �����ַ�����
        m_iFirst++;                                         // ��������ڵ������
        m_iFirst = m_iFirst % MAX_NUM_NODE;                 // ȷ����������Ч��Χ��
    }

    // ���õ�ǰ�ڵ������
    m_Node[m_iLast].vPos    = vPos;                    // ����λ��
    m_Node[m_iLast].fTime   = m_fTime;                 // ����ʱ�䣨��ǰʱ�䣩
    m_Node[m_iLast].dwColor = dwColor;                 // ������ɫ
    m_Node[m_iLast].spec    = true;                    // ��������Ч����־
    rt2_strncpy(m_Node[m_iLast].szNum, szString, 16);  // �����ַ������ڵ㣬��󳤶�Ϊ 16
    // m_Node[m_iLast].szNum[15] = 0;  // ȷ���ַ����������ѱ� `rt2_strncpy` ȷ����
    m_Node[m_iLast].cType  = type;        // ��������
    m_Node[m_iLast].cDir   = DIR_MIDDLE;  // ���÷���Ϊ�м�
    m_Node[m_iLast].fScale = fScale;      // �������ű���

    // �����ַ�����
    m_iCntChar += (int)strlen(m_Node[m_iLast].szNum);

    // �������һ�����ֵľ���������Ͱ���һ�����ֶ���ȥ
    int prev = (m_iLast - 1 + MAX_NUM_NODE) % MAX_NUM_NODE;  // ������һ���ڵ������
    if (m_Node[prev].cDir == DIR_MIDDLE)  // �����һ���ڵ�ķ������м�
    {
        // �����һ���ڵ�� z λ�ô��ڻ���ڵ�ǰ�ڵ�� z λ�ã���������֮��ľ���С�� 5
        if (m_Node[prev].vPos.z >= vPos.z && m_Node[prev].vPos.z - vPos.z < 5) {
            m_Node[prev].vPos.z += 5;  // ����һ���ڵ������ƶ� 5 ��λ
        }
    }

    m_iCnt++;                          // ���ӽڵ����
    m_iLast++;                         // �ƶ�����һ���ڵ�
    m_iLast = m_iLast % MAX_NUM_NODE;  // ȷ����������Ч��Χ��
}

void GcDynamicNumber::Render(RtgCamera& inCamera, RtgDevice& inDevice, float fSecond) {
    int i, j, k, c, iCnt;
    //DWORD dwRes;
    BYTE*      pVB;
    SCharUV*   pUV;
    RtgVertex3 vOffset;
    vOffset.Zero();
    RtgDeviceD3D9* dev = RtGetRender();  // ��ȡ��Ⱦ�豸ʵ��

    // ����д���Ⱦ�����ֽڵ�
    if (m_iCnt) {
        RtgMatrix16 m16;  // ���ڱ任�ľ���
        DWORD dwStride = sizeof(float) * 3 + sizeof(DWORD) + sizeof(float) * 2;  // �������ݵĲ���

        RtgShader*             render  = NULL;  // ������Ⱦ����ɫ��
        int                    ishader = 0;
        std::vector<SNumNode*> vNumNode[PIC_MAX];  // �洢��ͬ���͵����ֽڵ�

        // ����Ч�����ֽڵ���ൽ vNumNode ��
        for (i = 0; i < MAX_NUM_NODE; i++) {
            if (m_Node[i].fTime > 0.f) {
                vNumNode[m_Node[i].cType].push_back(&m_Node[i]);
            }
        }

        // ����ÿ��ͼƬ����
        for (int count = 0; count < PIC_MAX; count++) {
            iCnt = 0;  // ���ö������
            RtgShareBufferItem _vbItem;
            pVB = (BYTE*)dev->GetBuffMgr()->Lock_Shared_vb(m_iCntChar * dwStride * 6,
                                                           &_vbItem);  // ���������㻺����

            // ����ÿ��ͼƬ�����е����ֽڵ�
            for (int i = 0; i < vNumNode[count].size(); i++) {
                m16       = inCamera.m_matBillboard;  // ��ȡ������� Billboard ����
                int alpha = (vNumNode[count][i]->fTime) * 255;  // ���� alpha ֵ
                if (alpha > 255)
                    alpha = 255;

                // ���ýڵ���ɫ������ alpha��
                vNumNode[count][i]->dwColor =
                    vNumNode[count][i]->dwColor & 0x00ffffff | (alpha << 24);

                // ���ݽڵ�ķ����ʱ�����þ���任
                if (vNumNode[count][i]->cDir == DIR_RIGHTUP) {
                    m16.SetRow(3, vNumNode[count][i]->vPos);
                    float fTmpX = 0.f;
                    float fTmpY = 0.f;
                    m16.TranslateY(-8.f);
                    fTmpX = m_fTime - vNumNode[count][i]->fTime;
                    fTmpY = -1.f * fTmpX * fTmpX + 2.f * fTmpX;
                    m16.TranslateX(fTmpX * 10.f);
                    m16.TranslateY(fTmpY * 10.f);
                    m16.Scale(vNumNode[count][i]->fScale, vNumNode[count][i]->fScale,
                              vNumNode[count][i]->fScale);
                } else if (vNumNode[count][i]->cDir == DIR_LEFTUP) {
                    m16.SetRow(3, vNumNode[count][i]->vPos);
                    float fTmpX = 0.f;
                    float fTmpY = 0.f;
                    m16.TranslateY(-8.f);
                    fTmpX = vNumNode[count][i]->fTime - m_fTime;
                    fTmpY = -1.f * fTmpX * fTmpX - 2.f * fTmpX;
                    m16.TranslateX(fTmpX * 10.f);
                    m16.TranslateY(fTmpY * 10.f);
                    m16.Scale(vNumNode[count][i]->fScale, vNumNode[count][i]->fScale,
                              vNumNode[count][i]->fScale);
                } else if (vNumNode[count][i]->cDir == DIR_LEFT) {
                    m16.SetRow(3, vNumNode[count][i]->vPos);
                    m16.TranslateX((m_fTime - vNumNode[count][i]->fTime) *
                                   (m_fTime - vNumNode[count][i]->fTime) * -10.f);
                    m16.TranslateY(-10.f);
                    if (vNumNode[count][i]->cType != PIC_CHAR) {
                        m16.TranslateX(9.f);
                    }
                    m16.Scale(vNumNode[count][i]->fScale, vNumNode[count][i]->fScale,
                              vNumNode[count][i]->fScale);
                } else if (vNumNode[count][i]->cDir == DIR_RIGHT) {
                    m16.SetRow(3, vNumNode[count][i]->vPos);
                    m16.TranslateX((m_fTime - vNumNode[count][i]->fTime) *
                                   (m_fTime - vNumNode[count][i]->fTime) * 10.f);
                    m16.TranslateY(-10.f);
                    if (vNumNode[count][i]->cType != PIC_CHAR) {
                        m16.TranslateX(9.f);
                    }
                    m16.Scale(vNumNode[count][i]->fScale, vNumNode[count][i]->fScale,
                              vNumNode[count][i]->fScale);
                } else {
                    vNumNode[count][i]->vPos.z += m_fSpeed * fSecond;
                    m16.SetRow(3, vNumNode[count][i]->vPos);
                    if (vNumNode[count][i]->spec) {
                        float fScale = (fsin(DegreeToRadian(
                                            (vNumNode[count][i]->fTime * 225.f / m_fTime) - 90.f)) +
                                        1.f);
                        m16.Scale(fScale, fScale, fScale);
                    }
                }

                // �����ַ���ƫ����
                c = (int)strlen(vNumNode[count][i]->szNum);
                if (count == PIC_CHAR)
                    vOffset.x =
                        0.f - (m_fSizeCharX * (c / 2)) - ((c % 2) ? (m_fSizeCharX / 2.f) : 0);
                else
                    vOffset.x = 0.f - (m_fSizeX * (c / 2)) - ((c % 2) ? (m_fSizeX / 2.f) : 0);

                // �����ַ����е�ÿ���ַ������ö�������
                for (k = 0; k < c; k++) {
                    if (count == PIC_CHAR)
                        pUV = m_pUVsChar[vNumNode[count][i]->szNum[k]];
                    else
                        pUV = m_pUVs[vNumNode[count][i]->szNum[k]];
                    if (pUV) {
                        iCnt++;
                        for (j = 0; j < 6; j++) {
                            if (count == PIC_CHAR)
                                *((RtgVertex3*)pVB) = (m_pPaneVertexChar[j] + vOffset) * m16;
                            else
                                *((RtgVertex3*)pVB) = (m_pPaneVertex[j] + vOffset) * m16;
                            pVB += sizeof(float) * 3;
                            *((DWORD*)pVB) = vNumNode[count][i]->dwColor;
                            pVB += sizeof(DWORD);
                            memcpy(pVB, pUV->uvs + j, sizeof(RtgVertex2));
                            pVB += sizeof(RtgVertex2);
                        }
                    }
                    vOffset.x += m_fSizeX;
                }
            }

            dev->GetBuffMgr()->Unlock(&_vbItem);  // �������㻺����

            // �������Ч�Ķ������ݣ���ִ����Ⱦ
            if (iCnt > 0) {

                render = &m_Shader[count];
                m16.Unit();
                GetDevice()->SetMatrix(RTGTS_WORLD, &m16);

                // ���浱ǰ����Ⱦ״̬

                DWORD dwLighting = GetDevice()->GetRenderState(RTGRS_LIGHTING);
                DWORD dwZTest    = GetDevice()->GetRenderState(RTGRS_Z_TEST);
                DWORD dwZWrite   = GetDevice()->GetRenderState(RTGRS_Z_WRITE);

                // �رչ��ա���Ȳ��Ժ����д��
                GetDevice()->SetRenderState(RTGRS_LIGHTING, FALSE);
                GetDevice()->SetRenderState(RTGRS_Z_TEST, FALSE);
                GetDevice()->SetRenderState(RTGRS_Z_WRITE, FALSE);

                inDevice.SetShader(render);         // ������ɫ��
                inDevice.SetVertexFormat(RTG_VCT);  // ���ö����ʽ
                dev->DrawPrimitive(D3DPT_TRIANGLELIST, &_vbItem, dwStride,
                                   iCnt * 2);    // �����������б�
                inDevice.RestoreShader(render);  // �ָ�֮ǰ����ɫ��

                // �ָ�֮ǰ����Ⱦ״̬
                GetDevice()->SetRenderState(RTGRS_LIGHTING, dwLighting);
                GetDevice()->SetRenderState(RTGRS_Z_TEST, dwZTest);
                GetDevice()->SetRenderState(RTGRS_Z_WRITE, dwZWrite);
            }
        }
    }

    // ɾ���ѹ��ڵĽڵ�
    for (i = 0; i < MAX_NUM_NODE; i++) {
        if (m_Node[i].fTime > 0.f) {
            m_Node[i].fTime -= fSecond;
            if (m_Node[i].fTime <= 0.f) {
                m_iCnt--;
                m_iCntChar -= (int)strlen(m_Node[i].szNum);
                m_iFirst++;
            }
        }
    }
    CHECK(m_iCnt >= 0);
    m_iFirst = m_iFirst % MAX_NUM_NODE;  // ȷ�� m_iFirst ����Ч��Χ��
}

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
