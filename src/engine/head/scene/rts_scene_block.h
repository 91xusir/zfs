
#pragma once




#define RENDER_MULTI_DRAW   1

class RtSceneBlock : public RtSceneBlockTerrain
{
    friend class RtsSceneBlockManager;

public:
    friend class RtScene;

public :
    RtSceneBlock();
    virtual ~RtSceneBlock();

public:
    bool Init();
    void Clear();

    bool NewBlock(const char* szBlockFileName, float fOffsetX, float fOffsetY);
    bool LoadBlock(const char* szBlockFileName);
    bool SaveBlock(const char* szBlockFileName);

    // ���ص�X��Y�������������XY, fOutDist������ײ��ľ���, ��������ֵ����ײ����ģ�ͣ����û��ģ�Ͷ��ǵر����-1
    RtsObject* PickLine(const RtgVertex3 &v0, const RtgVertex3 &v1, float& fOutDist, int& fOutX, int& fOutY, DWORD dwType);
    RtsObject* PickRay(const RtgVertex3 &v, const RtgVertex3 &dir, float& fOutDist, int& fOutX, int& fOutY, DWORD dwType);
    bool GetHeight(const RtgVertex3 &v, float& fOutDist, int& fOutX, int& fOutY, DWORD dwType);
    bool GetTerrainHeight(float fX, float fY, float& outZ);

    // ����һ�������ڵ�����
    //    iMinX, iMinY, iMaxX, iMaxY ����ʾ�����λ��, ����Min������Max
    void UpdateGrid(int iMinX, int iMinY, int iMaxX, int iMaxY);

    void DrawNormal(RtgCamera& inCamera, RtgDevice& inDevice, DWORD dwColor, float fLength);
    void DrawTerrain(RtgCamera& inCamera, RtgDevice& inDevice, BYTE cAttrMask, DWORD dwNormalColor, DWORD dwHitColor, float fOffset);

    // ȡ��ָ����Χ����������Ϣ
    //     inAABB    Ϊָ���ķ�Χ
    //     pVertices ���ڱ��涥���ָ��
    //     iVertexNum �������Ĵ�С(���Ա����RtgVertex3����)��ִ������������洢���涥��ĸ���
    //     pIndices  ���ڱ���������ָ��
    //     iIndexNum  �������Ĵ�С(���Ա����short����)��ִ������������洢���������ĸ���
    //  ����: true��ʾ�ɹ�, false��ʾʧ��
    //     ���ܳɹ�����ʧ�ܶ���ı� iVertexNum �� iIndexNum ��ֵ
    bool TakeGeometry(const RtgAABB& inAABB, RtgVertex3* pVertices, int& iVertexNum, short* pIndices, int& iIndexNum);

    // ���һ����Ⱦ���嵽�����У����������Ҫ��Ⱦ����ô��ÿ����Ⱦ��ʱ����Ҫ���
    //     ���еĳ�ʼ�����ڵ��ó�����PreRender��ʱ��
    bool AddRenderObject(const RtgVertex3& vPos, RtsObject* pObject);

   // void LoadAllObject();
    void SaveSM2File();

    void GetRenderGrid(gridlist_t* pgrid, RtgCamera& _cam);
    virtual void RecomputeObjectId();


protected:
    virtual bool OnResourceAllocate();
    virtual bool OnResourceFree();
    virtual bool OnLoadSceneComplete();
    virtual bool OnSerializeBlockObject(RtArchive* pFile);
    virtual bool OnSerializeBlockTerrain(RtArchive* pFile, DWORD dwType);
    virtual bool OnSerializeChunk(RtArchive* pFile, DWORD dwType, DWORD dwSize);


    bool OnSerializeSceneContent(RtArchive* pFile, DWORD dwType);
    void FixGridObjectNum();
    void OnChangeBorderline(bool bAdd);

public:
    RtsSRenderMap* GetRenderStartIndex(RtsSTileMap tileMap);
    BYTE IncreaseOpFlag();  // ʹ��������Զ���һ
    BYTE GetLastOpFlag();   // ȡ�����һ�β������
    void ClearOpFlag();

    int GetTerrainMapIdx(float fX, float fY);  // �������ڵ����ͼ��ţ�-1��ʾû���ҵ�
    DWORD GetTerrainColor(float fX, float fY); // �������ڵ�Ķ�����ɫֵ

    void UpdateVertexPositionByHeightmap();
    void UpdateVertexPositionByTerrainHeightmap();
    void UpdateVertexPositionByWaterHeightmap();

    void RecomputeAllLight(RtgDevice& inDevice);
   // void ClearObjectLink();
    void ReIndexObjectList();

    RtsModel* FindFirstModel(const char* szFilename);

    // ����ƹ�
    void ComputeVertexColor(int iX, int iY);
    void ComputeVertexColor(int iX, int iY, RtsSVertex* pVertex);

    float GetOffsetX()  { return m_fOffsetX; }
    float GetOffsetY()  { return m_fOffsetY; }

    bool HasSM() const  { return !!m_texItemSM; }

public:
    RtgVertex3      m_vMin;             // ������AABB
    RtgVertex3      m_vMax;

    // �Ż�������Ϣ
    int             m_iGridSize;        // ���������������Ĵ�С
    RtsSGrid        m_pGrid[(g_iSceneTileCntX/RTS_BLOCK_GRID_SIZE)*(g_iSceneTileCntY/RTS_BLOCK_GRID_SIZE)]; // ��������

    // ������Ϣ
    BYTE            m_pOpFlag[(g_iSceneTileCntX+1)*(g_iSceneTileCntY+1)];           // �������
    BYTE            m_cLastOpFlag;      // ���һ�β������

    // ͼ����Ϣ
    // ���θ߶���Ϣ
    RtsSVertexValue     m_pPositions[(g_iSceneTileCntX+1)*(g_iSceneTileCntY+1)];    // ����λ�úͷ�������Ϣ
    // ��������,�����ĸ߶���Ϣ�� RtsSVertexValue
    RtsSVertex          m_pVertices[g_iSceneTileCntX*g_iSceneTileCntY*4];           // ���㻺����
    RtsSIndex           m_pIndices[g_iSceneBlockFaceSize*3];                        // ����������

    // ������ʾ������Ϣ
    RtsSTileMap     m_pTileMapIdx[g_iSceneTileCntY*g_iSceneTileCntX]; // ��ͼ�����ͼ����

    float                   m_fOffsetX, m_fOffsetY;

    RtgTextItem*        m_texItemSM;
    RtgTextItem*        m_texMinmap[4];
    //char                m_szBlockName[128];
	CM_MEMDEF(m_szBlockName, 128)
};

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
