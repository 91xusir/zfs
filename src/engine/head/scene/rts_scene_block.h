
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

    // 返回的X和Y都是行走网格的XY, fOutDist是离碰撞点的距离, 函数返回值是碰撞到的模型，如果没有模型而是地表就是-1
    RtsObject* PickLine(const RtgVertex3 &v0, const RtgVertex3 &v1, float& fOutDist, int& fOutX, int& fOutY, DWORD dwType);
    RtsObject* PickRay(const RtgVertex3 &v, const RtgVertex3 &dir, float& fOutDist, int& fOutX, int& fOutY, DWORD dwType);
    bool GetHeight(const RtgVertex3 &v, float& fOutDist, int& fOutX, int& fOutY, DWORD dwType);
    bool GetTerrainHeight(float fX, float fY, float& outZ);

    // 更新一定区域内的网格
    //    iMinX, iMinY, iMaxX, iMaxY 是显示网格的位置, 包含Min不包含Max
    void UpdateGrid(int iMinX, int iMinY, int iMaxX, int iMaxY);

    void DrawNormal(RtgCamera& inCamera, RtgDevice& inDevice, DWORD dwColor, float fLength);
    void DrawTerrain(RtgCamera& inCamera, RtgDevice& inDevice, BYTE cAttrMask, DWORD dwNormalColor, DWORD dwHitColor, float fOffset);

    // 取得指定范围的三角形信息
    //     inAABB    为指定的范围
    //     pVertices 用于保存顶点的指针
    //     iVertexNum 缓冲区的大小(可以保存的RtgVertex3数量)，执行这个函数后会存储保存顶点的个数
    //     pIndices  用于保存索引的指针
    //     iIndexNum  缓冲区的大小(可以保存的short数量)，执行这个函数后会存储保存索引的个数
    //  返回: true表示成功, false表示失败
    //     不管成功或者失败都会改变 iVertexNum 和 iIndexNum 的值
    bool TakeGeometry(const RtgAABB& inAABB, RtgVertex3* pVertices, int& iVertexNum, short* pIndices, int& iIndexNum);

    // 添加一个渲染物体到场景中，如果物体需要渲染，那么在每次渲染的时候都需要添加
    //     队列的初始化是在调用场景的PreRender的时候
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
    BYTE IncreaseOpFlag();  // 使操作标记自动加一
    BYTE GetLastOpFlag();   // 取得最后一次操作标记
    void ClearOpFlag();

    int GetTerrainMapIdx(float fX, float fY);  // 返回所在点的贴图编号，-1表示没有找到
    DWORD GetTerrainColor(float fX, float fY); // 返回所在点的顶点颜色值

    void UpdateVertexPositionByHeightmap();
    void UpdateVertexPositionByTerrainHeightmap();
    void UpdateVertexPositionByWaterHeightmap();

    void RecomputeAllLight(RtgDevice& inDevice);
   // void ClearObjectLink();
    void ReIndexObjectList();

    RtsModel* FindFirstModel(const char* szFilename);

    // 计算灯光
    void ComputeVertexColor(int iX, int iY);
    void ComputeVertexColor(int iX, int iY, RtsSVertex* pVertex);

    float GetOffsetX()  { return m_fOffsetX; }
    float GetOffsetY()  { return m_fOffsetY; }

    bool HasSM() const  { return !!m_texItemSM; }

public:
    RtgVertex3      m_vMin;             // 场景的AABB
    RtgVertex3      m_vMax;

    // 优化网格信息
    int             m_iGridSize;        // 网格索引缓冲区的大小
    RtsSGrid        m_pGrid[(g_iSceneTileCntX/RTS_BLOCK_GRID_SIZE)*(g_iSceneTileCntY/RTS_BLOCK_GRID_SIZE)]; // 场景网格

    // 操作信息
    BYTE            m_pOpFlag[(g_iSceneTileCntX+1)*(g_iSceneTileCntY+1)];           // 操作标记
    BYTE            m_cLastOpFlag;      // 最后一次操作标记

    // 图形信息
    // 地形高度信息
    RtsSVertexValue     m_pPositions[(g_iSceneTileCntX+1)*(g_iSceneTileCntY+1)];    // 包括位置和法向量信息
    // 地形网格,真正的高度信息在 RtsSVertexValue
    RtsSVertex          m_pVertices[g_iSceneTileCntX*g_iSceneTileCntY*4];           // 顶点缓冲区
    RtsSIndex           m_pIndices[g_iSceneBlockFaceSize*3];                        // 索引缓冲区

    // 场景显示网格信息
    RtsSTileMap     m_pTileMapIdx[g_iSceneTileCntY*g_iSceneTileCntX]; // 地图块的贴图索引

    float                   m_fOffsetX, m_fOffsetY;

    RtgTextItem*        m_texItemSM;
    RtgTextItem*        m_texMinmap[4];
    //char                m_szBlockName[128];
	CM_MEMDEF(m_szBlockName, 128)
};

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
