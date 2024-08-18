
#include "graph/rt_graph.h"
#include "gc_dnumber.h"
#include "gc_include.h"

GcDynamicNumber::GcDynamicNumber() {
    // 设置数字或符号面板的宽度和高度
    m_fSizeX = 3.f;  // 面板的宽度
    m_fSizeY = 5.f;  // 面板的高度

    // 设置字符面板的宽度和高度
    m_fSizeCharX = 5;  // 字符面板的宽度
    m_fSizeCharY = 3;  // 字符面板的高度

    // 初始化面板的起始和结束索引，以及计数器
    m_iFirst   = 0;  // 起始索引
    m_iLast    = 0;  // 结束索引
    m_iCnt     = 0;  // 数字计数
    m_iCntChar = 0;  // 字符计数

    // 初始化速度和时间参数
    m_fSpeed = 30.f;  // 动画速度
    m_fTime  = 1.f;   // 动画持续时间

    // 清空数字节点数组，将所有元素设置为 0
    memset(m_Node, 0, sizeof(SNumNode) * MAX_NUM_NODE);

    // 清空字符 UV 指针数组，将所有指针设置为 nullptr
    memset(m_pUVs, 0, sizeof(SCharUV*) * 128);

    // 清空字符 UV 指针数组，将所有指针设置为 nullptr
    memset(m_pUVsChar, 0, sizeof(SCharUV*) * 128);

    // 设置数字或符号面板的顶点坐标
    m_pPaneVertex[0].Set(-m_fSizeX, -m_fSizeY, 0.f);  // 左下角
    m_pPaneVertex[1].Set(-m_fSizeX, m_fSizeY, 0.f);   // 左上角
    m_pPaneVertex[2].Set(m_fSizeX, m_fSizeY, 0.f);    // 右上角
    m_pPaneVertex[3].Set(m_fSizeX, m_fSizeY, 0.f);    // 右上角（重复）
    m_pPaneVertex[4].Set(m_fSizeX, -m_fSizeY, 0.f);   // 右下角
    m_pPaneVertex[5].Set(-m_fSizeX, -m_fSizeY, 0.f);  // 左下角（重复）

    // 设置字符面板的顶点坐标
    m_pPaneVertexChar[0].Set(-m_fSizeCharX, -m_fSizeCharY, 0.f);  // 左下角
    m_pPaneVertexChar[1].Set(-m_fSizeCharX, m_fSizeCharY, 0.f);   // 左上角
    m_pPaneVertexChar[2].Set(m_fSizeCharX, m_fSizeCharY, 0.f);    // 右上角
    m_pPaneVertexChar[3].Set(m_fSizeCharX, m_fSizeCharY, 0.f);    // 右上角（重复）
    m_pPaneVertexChar[4].Set(m_fSizeCharX, -m_fSizeCharY, 0.f);   // 右下角
    m_pPaneVertexChar[5].Set(-m_fSizeCharX, -m_fSizeCharY, 0.f);  // 左下角（重复）
}

GcDynamicNumber::~GcDynamicNumber() {}

void GcDynamicNumber::SetNumberTexture(int iXCnt, int iYCnt, const char* szChars) {
    // 计算字符数组的长度
    int iCnt = (int)strlen(szChars);

    // 如果字符数组的长度超过了最大字符数量，则限制为最大字符数量
    if (iCnt > MAX_CHAR)
        iCnt = MAX_CHAR;

    // 清空 UV 指针数组
    memset(m_pUVs, 0, sizeof(SCharUV*) * 128);

    int   i;
    float x, y, xStep, yStep;
    // 计算每个纹理单元的宽度和高度
    xStep = 1.f / iXCnt;
    yStep = 1.f / iYCnt;

    // 遍历每个字符，计算其对应的 UV 坐标
    for (i = 0; i < iCnt; i++) {
        // 计算当前字符的纹理位置
        x = (i % iXCnt) * xStep;
        y = (i / iXCnt) * yStep;

        // 设置当前字符的 UV 坐标
        m_UVs[i].uvs[0].Set(x, 1 - (y));                  // 左上角
        m_UVs[i].uvs[1].Set(x, 1 - (y + yStep));          // 左下角
        m_UVs[i].uvs[2].Set(x + xStep, 1 - (y + yStep));  // 右下角
        m_UVs[i].uvs[3].Set(x + xStep, 1 - (y + yStep));  // 右下角（重复）
        m_UVs[i].uvs[4].Set(x + xStep, 1 - (y));          // 右上角
        m_UVs[i].uvs[5].Set(x, 1 - (y));                  // 左上角（重复）

        // 将字符的 UV 数据存储到指针数组中
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
    // 设置数字纹理，参数分别为纹理的宽高和纹理内容字符串
    // 这里设置了数字和符号的纹理
    // SetNumberTexture(4, 4, "1234567890-+miss");
    SetNumberTexture(4, 4, "ssim90-+56781234");  // 设置的纹理内容包括自定义字符和数字

    // 设置字符纹理，参数分别为纹理的宽高和纹理内容字符串
    // 这里设置了字符的纹理
    SetCharTexture(3, 5, "0123456789abcde");  // 设置的纹理内容包括数字和字符

    // 可能用于调试或显示额外的字符纹理（被注释掉）
    // SetCharTexture(3, 5, "经帮烫吸门烫抵声烫格修烫闪灵烫");

    // 初始化所有 Shader 设置
    for (int i = 0; i < PIC_MAX; i++) {
        // 允许双面渲染
        m_Shader[i].bTwoSide = true;
        // 设置渲染阶段数为 1
        m_Shader[i].iNumStage = 1;

        char path[32];
        // 格式化字符串，生成纹理文件路径
        rt2_sprintf(path, "num%d", i);
        // 从文件创建纹理并设置到 Shader
        m_Shader[i].Shaders[0].SetTexture(GetDevice()->GetTextMgr()->CreateTextureFromFile(path));

        // 设置 Shader 的纹理操作
        // m_Shader[i].Shaders[0].eAlphaOp   = RTGTOP_MODULATE;  // (已注释) Alpha 操作模式
        m_Shader[i].Shaders[0].eAlphaArg2 = RTGTA_TEXTURE;  // Alpha 第二个参数为纹理
        m_Shader[i].Shaders[0].eColorOp = RTGTOP_SELECTARG1;  // 颜色操作模式选择第一个参数
        m_Shader[i].Shaders[0].eColorArg1 = RTGTA_TEXTURE;  // 颜色第一个参数为纹理
        m_Shader[i].Shaders[0].eColorArg2 = RTGTA_DIFFUSE;  // 颜色第二个参数为漫反射

        // 启用混合功能
        m_Shader[i].bEnableBlendFunc = true;
        // 设置混合源和目标因子
        m_Shader[i].eBlendSrcFunc = RTGBLEND_SRCALPHA;     // 混合源因子为源 Alpha 值
        m_Shader[i].eBlendDstFunc = RTGBLEND_INVSRCALPHA;  // 混合目标因子为 1 - 源 Alpha 值
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

    // 如果与上一次数字距离过近，就把上一个数字顶上去
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

    // 如果与上一次数字距离过近，就把上一个数字顶上去
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
    // 检查当前节点数量是否已达到最大值
    if (m_iCnt >= MAX_NUM_NODE) {
        // 如果节点数量达到最大值，移除最早添加的节点
        m_iCnt--;                                           // 减少节点计数
        m_iCntChar -= (int)strlen(m_Node[m_iFirst].szNum);  // 减少字符计数
        m_iFirst++;                                         // 更新最早节点的索引
        m_iFirst = m_iFirst % MAX_NUM_NODE;                 // 确保索引在有效范围内
    }

    // 设置当前节点的属性
    m_Node[m_iLast].vPos    = vPos;                    // 设置位置
    m_Node[m_iLast].fTime   = m_fTime;                 // 设置时间（当前时间）
    m_Node[m_iLast].dwColor = dwColor;                 // 设置颜色
    m_Node[m_iLast].spec    = true;                    // 设置特殊效果标志
    rt2_strncpy(m_Node[m_iLast].szNum, szString, 16);  // 复制字符串到节点，最大长度为 16
    // m_Node[m_iLast].szNum[15] = 0;  // 确保字符串结束（已被 `rt2_strncpy` 确保）
    m_Node[m_iLast].cType  = type;        // 设置类型
    m_Node[m_iLast].cDir   = DIR_MIDDLE;  // 设置方向为中间
    m_Node[m_iLast].fScale = fScale;      // 设置缩放比例

    // 更新字符计数
    m_iCntChar += (int)strlen(m_Node[m_iLast].szNum);

    // 如果与上一个数字的距离过近，就把上一个数字顶上去
    int prev = (m_iLast - 1 + MAX_NUM_NODE) % MAX_NUM_NODE;  // 计算上一个节点的索引
    if (m_Node[prev].cDir == DIR_MIDDLE)  // 如果上一个节点的方向是中间
    {
        // 如果上一个节点的 z 位置大于或等于当前节点的 z 位置，并且它们之间的距离小于 5
        if (m_Node[prev].vPos.z >= vPos.z && m_Node[prev].vPos.z - vPos.z < 5) {
            m_Node[prev].vPos.z += 5;  // 将上一个节点向上移动 5 单位
        }
    }

    m_iCnt++;                          // 增加节点计数
    m_iLast++;                         // 移动到下一个节点
    m_iLast = m_iLast % MAX_NUM_NODE;  // 确保索引在有效范围内
}

void GcDynamicNumber::Render(RtgCamera& inCamera, RtgDevice& inDevice, float fSecond) {
    int i, j, k, c, iCnt;
    //DWORD dwRes;
    BYTE*      pVB;
    SCharUV*   pUV;
    RtgVertex3 vOffset;
    vOffset.Zero();
    RtgDeviceD3D9* dev = RtGetRender();  // 获取渲染设备实例

    // 如果有待渲染的数字节点
    if (m_iCnt) {
        RtgMatrix16 m16;  // 用于变换的矩阵
        DWORD dwStride = sizeof(float) * 3 + sizeof(DWORD) + sizeof(float) * 2;  // 顶点数据的步长

        RtgShader*             render  = NULL;  // 用于渲染的着色器
        int                    ishader = 0;
        std::vector<SNumNode*> vNumNode[PIC_MAX];  // 存储不同类型的数字节点

        // 将有效的数字节点分类到 vNumNode 中
        for (i = 0; i < MAX_NUM_NODE; i++) {
            if (m_Node[i].fTime > 0.f) {
                vNumNode[m_Node[i].cType].push_back(&m_Node[i]);
            }
        }

        // 遍历每种图片类型
        for (int count = 0; count < PIC_MAX; count++) {
            iCnt = 0;  // 重置顶点计数
            RtgShareBufferItem _vbItem;
            pVB = (BYTE*)dev->GetBuffMgr()->Lock_Shared_vb(m_iCntChar * dwStride * 6,
                                                           &_vbItem);  // 锁定共享顶点缓冲区

            // 遍历每种图片类型中的数字节点
            for (int i = 0; i < vNumNode[count].size(); i++) {
                m16       = inCamera.m_matBillboard;  // 获取摄像机的 Billboard 矩阵
                int alpha = (vNumNode[count][i]->fTime) * 255;  // 计算 alpha 值
                if (alpha > 255)
                    alpha = 255;

                // 设置节点颜色（包括 alpha）
                vNumNode[count][i]->dwColor =
                    vNumNode[count][i]->dwColor & 0x00ffffff | (alpha << 24);

                // 根据节点的方向和时间设置矩阵变换
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

                // 计算字符的偏移量
                c = (int)strlen(vNumNode[count][i]->szNum);
                if (count == PIC_CHAR)
                    vOffset.x =
                        0.f - (m_fSizeCharX * (c / 2)) - ((c % 2) ? (m_fSizeCharX / 2.f) : 0);
                else
                    vOffset.x = 0.f - (m_fSizeX * (c / 2)) - ((c % 2) ? (m_fSizeX / 2.f) : 0);

                // 遍历字符串中的每个字符，设置顶点数据
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

            dev->GetBuffMgr()->Unlock(&_vbItem);  // 解锁顶点缓冲区

            // 如果有有效的顶点数据，则执行渲染
            if (iCnt > 0) {

                render = &m_Shader[count];
                m16.Unit();
                GetDevice()->SetMatrix(RTGTS_WORLD, &m16);

                // 保存当前的渲染状态

                DWORD dwLighting = GetDevice()->GetRenderState(RTGRS_LIGHTING);
                DWORD dwZTest    = GetDevice()->GetRenderState(RTGRS_Z_TEST);
                DWORD dwZWrite   = GetDevice()->GetRenderState(RTGRS_Z_WRITE);

                // 关闭光照、深度测试和深度写入
                GetDevice()->SetRenderState(RTGRS_LIGHTING, FALSE);
                GetDevice()->SetRenderState(RTGRS_Z_TEST, FALSE);
                GetDevice()->SetRenderState(RTGRS_Z_WRITE, FALSE);

                inDevice.SetShader(render);         // 设置着色器
                inDevice.SetVertexFormat(RTG_VCT);  // 设置顶点格式
                dev->DrawPrimitive(D3DPT_TRIANGLELIST, &_vbItem, dwStride,
                                   iCnt * 2);    // 绘制三角形列表
                inDevice.RestoreShader(render);  // 恢复之前的着色器

                // 恢复之前的渲染状态
                GetDevice()->SetRenderState(RTGRS_LIGHTING, dwLighting);
                GetDevice()->SetRenderState(RTGRS_Z_TEST, dwZTest);
                GetDevice()->SetRenderState(RTGRS_Z_WRITE, dwZWrite);
            }
        }
    }

    // 删除已过期的节点
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
    m_iFirst = m_iFirst % MAX_NUM_NODE;  // 确保 m_iFirst 在有效范围内
}

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
