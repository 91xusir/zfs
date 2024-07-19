
#include "rts_scene_render.h"
#include "rts_grass_render.h"
#include "rts_object_render.h"


void SceneMiniMapRotate(int centerX, int centerY, int inX, int inY, int& outX, int& outY, float fRadian);
void SceneMiniMapRotate2(int centerX, int centerY, int inX, int inY, int& outX, int& outY, float fc, float fs);

class  RtSceneManager;

class RtScene : public RtsSceneDoc, public RtsSceneUtil
{
public:
    enum {MAX_MAP_COUNT = 256};
    enum {MAX_WATER_COUNT = 10};
    enum {MIN_INDEX_COST = -100};
    friend class CXmlFileMapTexture;
    friend class CXmlFileLightSet;
    friend class CXmlFileWater;
    friend class RtSceneBlock;

public:
    RtScene();
    virtual ~RtScene();

    bool OnceInit(RtgDevice* pDevice);  // 一些变量的初始化，这些初始化只做一次
    bool OnceClear();

    bool Init();
    void Clear();

    void OnBlockLoadFinish(RtsSceneBlockMap* _blockMap);

    void BuildShadowMap();
    void BuildShadowMap_ForCurrBlock();
    void BuildShadowMap(const Int2& _blockIndex);
    void ProcessSM(RtgTextItem* texSrc, int sizSrc, RtgTextItem* texDst, int sizDst);
	void ViewCenter();

    // 渲染
    //    inCamera 是渲染的Camera
    //    pDevice 是渲染的驱动
    bool Render(RtgCamera& inCamera, RTGRenderMask eMask);
    void DrawTerrain(RtgCamera& inCamera, BYTE cAttrMask, DWORD dwNormalColor=0xFF88FF88, DWORD dwHitColor=0xFFFF0000, float fOffset=0.5f);
    void DrawMiniMap(int iScreenX, int iScreenY, int iScreenWidth, int iScreenHeight, float fSceneWidth, float fCenterX, float fCenterY);
    void RenderSceneObjectHighLight(RtgCamera& inCamera);
	void SetRenderMode(BOOL bMiniMap);

    void ResetRenderEntity();

    // 逻辑一帧的处理
    void OnFrameMove(float _elapsedTime);

    void RequestBlockReady(const RtgVertex3& _Pos, bool bForcePreLoad = false);
    void RequestBlockObject(const RtgVertex3& _Pos);
    void RequestEnvUpdate(const RtgVertex3& _Pos);

    void RequestBlockReadyForSM(const Int2& _blockIndex);
    void RequestBlockObjectForSM(const Int2& _blockIndex);

    virtual bool LoadMap(const char* szMapFileName, bool bLoadAll); 
    bool OnSerializeObjects(RtArchive* pFile);

    void OnModelCreateFinish(RtsModel* _model, RtsSGrid* _grid);


    void RemoveObjectByLastRenderList(RtsObject* pRenderObject);
    bool CheckObjectIsLastRenderList(RtsObject* pFindObject);

    // 更新灯光, 这个函数根据时间更新当前的颜色，包括环境光和平行光的颜色以及方向
    //    iX, iY 是玩家当前所在的位置
    //    iMinute 是当前的时间(分钟, 从0点开始计算的分钟)
    void UpdateSkyLight(int iX, int iY, int iMinute);

    // 更新听众的位置
    void UpdateAudioListener(const RtgVertex3& vPos);

    bool SaveEnvironmentInfo(const char* szFilename);
    bool LoadEnvironmentInfo(const char* szFilename);

    void RecomputeAllLight();
    void RecomputeObjectBelong();
    int CheckSameObject();       // 检查场景中完全相同的物体(位置和物件都相同)
    int DeleteSameObject();      // 删除场景中完全相同的物体(位置和物件都相同)

    bool DeleteBlockMap(int x, int y);
  //  void GetRenderObject(RtsObject**& plistObj, int& nNum);



public: // Util
   // virtual RtgLight* CreateDynamicLight();
  //  virtual void DestroyDynamicLight(RtgLight* pLight);
  //  virtual void UpdateDynamicLight(RtgLight* pLight);

public:
   // void IncreaseRenderFlag();
    void GetSortTileMapIdx(RtsSTileMap& tileMapIdx);
    RtSceneBlock* GetTerrainByPos(float fPosX, float fPosY, int& iTx, int& iTy); // x, y 位置
    RtSceneBlock* GetTileByPos(float fPosX, float fPosY, int& iTx, int& iTy); // x, y 位置
    RtSceneBlock* GetTileByPos(RtgVertex3& vPos, int& iTx, int& iTy); // x, y 位置
    void GetPosByTile(int iTileX, int iTileY, RtgVertex3& vPos);
    void GetPosByTerrain(int iTerrainX, int iTerrainY, RtgVertex3& vPos);
    RtSceneBlock* GetBlockByPos(float fPosX, float fPosY);  // x, y 为位置
    RtSceneBlock* GetBlockByPos(const RtgVertex3& vPos);    // vPos 为位置
    RtSceneBlock* GetBlockByTile(int iX, int iY, int* pBx=0, int* pBy=0); // x, y 为网格的索引
    RtSceneBlock* GetBlock(int iX, int iY);         // x, y 为块的索引
    RtSceneBlock* GetFirstBlock();
    RtSceneBlock* GetNextBlock(RtSceneBlock* pCur);
    void SetTileMapMtl(int iX, int iY, BYTE cMtl, DWORD dwFlag);    // x, y 为网格的索引
    void SetTileMapGrass(int iX, int iY, int Density, WORD tid, RtgVertex2 szh, float AspectW);
    void SetTileWaterType(int iX, int iY, BYTE cWaterType);         // x, y 为网格的索引
    RtsSVertexValue* GetVertexValue(int x, int y, bool bEdit);  // 取得顶点属性数据, x, y 为顶点的索引
    BYTE GetVertexHeightLevel(int x, int y, bool bTile);                        // x, y 为顶点的索引
    void SetVertexHeightLevel(int x, int y, bool bTerrain, bool bTile, BYTE cHeightLevel);     // x, y 为顶点的索引

    //BYTE GetOpFlag(int x, int y);                   // x, y 为顶点的索引

    int GetTerrainMapIdx(float fX, float fY);           // 返回所在点的贴图编号，-1表示没有找到, x, y 为位置
    int GetTerrainMapType(float fX, float fY);          // x, y 为位置
    const char* GetTerrainMapFile(float fX, float fY);  // x, y 为位置
    const char* GetTerrainMapName(float fX, float fY);  // x, y 为位置
    DWORD GetTerrainColor(float fX, float fY);          // 返回所在点的顶点颜色值
    void GetTerrainColor(float fX, float fY, RtgVertex3& vColor);   // x, y 为位置

    // 返回的X和Y都是行走网格的XY, fOutDist是离碰撞点的距离, 函数返回值是碰撞到的模型，如果没有模型而是地表就是-1
    RtsObject* PickLine(const RtgVertex3 &v0, const RtgVertex3 &v1, float& fOutDist, int& fOutX, int& fOutY, DWORD dwType);
    RtsObject* PickRay(const RtgVertex3 &v, const RtgVertex3 &dir, float& fOutDist, int& fOutX, int& fOutY, DWORD dwType);
    bool GetHeight(const RtgVertex3 &v, float& fOutDist, int& fOutX, int& fOutY, DWORD dwType);
    bool GetTerrainHeight(float fX, float fY, float& outZ); // x, y 为位置

    // 更新一定区域内的网格
    //    iMinX, iMinY, iMaxX, iMaxY 是网格的索引, 包含Min不包含Max
    void UpdateGrid(int iMinX, int iMinY, int iMaxX, int iMaxY);
    void UpdateVertexPositionByHeightmap(int iMinX, int iMinY, int iMaxX, int iMaxY);
    void UpdateVertexPositionByTerrainHeightmap(int iMinX, int iMinY, int iMaxX, int iMaxY);

    void ComputeVertexNormal(int iX, int iY);   // x, y 为顶点的索引，一个块的顶点数为(g_iSceneTileCntX+1)*(g_iSceneTileCntY+1)
    void ComputeVertexColor(int iX, int iY);    // x, y 为顶点的索引，一个块的顶点数为(g_iSceneTileCntX+1)*(g_iSceneTileCntY+1)

    bool AddObject(objectId _objId);
    bool RemoveObject(objectId _objId);

	//物件平滑载入
	//BOOL AddObjToQueue(RtsObject* pObject);
	//void LoadObjInQueue();

    BOOL AddDynamicLightToScene(RtsDynamicLight* pLight);
    BOOL RemoveDynamicLightByScene(RtsDynamicLight* pLight);

    // 取得影响指定位置的灯光列表, cFlag & 0x01静态灯光, 0x02动态灯光
    RtsLight* GetRangeLights(RtgVertex3& vPos, float fRadius=200.f, BYTE cFlag=0x03);

    // 添加一个渲染物体到场景中，如果物体需要渲染，那么在每次渲染的时候都需要添加
    //     队列的初始化是在调用场景的PreRender的时候
    //bool AddRenderObject(const RtgVertex3& vPos, RtsObject* pObject);

    // 取得指定范围的三角形信息
    //     inAABB    为指定的范围
    //     pVertices 用于保存顶点的指针
    //     iVertexNum 缓冲区的大小(可以保存的RtgVertex3数量)，执行这个函数后会存储保存顶点的个数
    //     pIndices  用于保存索引的指针
    //     iIndexNum  缓冲区的大小(可以保存的short数量)，执行这个函数后会存储保存索引的个数
    //  返回: true表示成功, false表示失败
    //     不管成功或者失败都会改变 iVertexNum 和 iIndexNum 的值
    bool TakeGeometry(const RtgAABB& inAABB, RtgVertex3* pVertices, int& iVertexNum, short* pIndices, int& iIndexNum);

    RtsModel* FindFirstModel(const char* szFilename);

    RtsSGrid* GetGrid(int _girdx, int _gridy, bool bedit)
    {
        if (_girdx < 0 || _gridy < 0)
            return NULL;

        int _blockx = _girdx / g_iSceneGridCntX;
        int _blocky = _gridy / g_iSceneGridCntY;

        RtsSceneBlockMap* _block = GetBlockMapbyIndex(_blockx, _blocky);

        if (!_block || !_block->ReourceReady())
            return NULL;

        RTASSERT(_block->m_pBlock);

        if (bedit) 
            _block->m_pBlock->SetModifyFlag(true);

        return ((RtSceneBlock*)_block->m_pBlock)->m_pGrid + ((_gridy - _blocky * g_iSceneGridCntY) * g_iSceneGridCntX + (_girdx - _blockx * g_iSceneGridCntX));
    }
    
	inline RtgVertex2* GetTexUVByMap(const RtsSTileMap& tileMap, int iLevel)
	{
		int iUV = 0;

		if ((iLevel==0 || tileMap.mapCnt==1) && tileMap.uv2)
			iUV = 16;

		switch (iLevel)
		{
		case 0:
			iUV += tileMap.uv1;
			break;
		case 1:
			iUV += tileMap.uv2;
			break;
		case 2:
			iUV += tileMap.uv3;
			break;
		case 3:
			iUV += tileMap.uv4;
			break;
		default:
			CHECKEX("too max texture level");
			break;
		}

		return m_TexUVPattern[iUV].pUV;
	}

    void AddRenderObject(RtsObject* _obj)
    {
        m_ObjectRender.AddObject(_obj);
    }

    // 灯光
    DWORD GetSkyLighting()                                      { return m_lightSkyColor; }
    void SetSkyLighting(DWORD dwLightColor);

    BOOL IsRenderMiniMapMode()                                  { return m_bRenderMiniMapMode; }
    void SetMinimapMaskTexture(const char* szTextureFileName, float fL=0.f, float fT=0.f, float fR=1.f, float fB=1.f);
    void SetMinimapRotate(float fDegree)    { m_fMinimapRotate = DegreeToRadian(fDegree); }
    float GetMinimapRotate()                { return RadianToDegree(m_fMinimapRotate); }
    float GetMinimapRotateRadian()          { return m_fMinimapRotate; }

    BOOL IsShowBlockFlag()                                      { return m_bShowBlockFlag; }
    void ShowSceneFlagTexture(const char* szFlagTexture,bool bSaveLastTexture = true);
public:
    static void ResetTimer();

    // 优化选项
    void OptionSetWaterCatoptricSky(BOOL bTrue);

#if DO_CONSOLE
public:
    void BeginPickRayRecord(RtgVertex3& vSource, RtgVertex3& vDir);
    void EndPickRayRecord();
    void AddPickRayLine(RtgVertex3& v1, RtgVertex3& v2);
    void AddPickRayPoint(float fDist);
    void DrawPickRayRecord();
    BOOL                    m_bEnablePickRayRecord;
protected:
    RtgVertex3              m_vPickRaySource, m_vPickRayDir;
    list<RtgVertex3>        m_listPickRayLine;
    list<float>             m_listPickRayPoint;
#endif

public:
    void SetupWaterShader(BYTE cWaterType);
    bool PreRenderWater(RtgCamera& inCamera);
    void UpdateRenderGridList(RtgCamera& _cam);
    bool PreRender(RtgCamera& inCamera, RtSceneBlock* pBlock = NULL);
    void CreateGrass(RtsSTileMap* tile, RtSceneBlock* block, RtsSVertex* v, WORD tid, WORD density, float minH, float maxH, float aspect);
    void ClearGrass(RtsSTileMap* tile);
    void RecreateGrass(RtSceneBlock* block);


    bool PreRenderDynamicLight();
    virtual void OnAfterPreRender(){};
    void RenderSceneObject(RtgCamera& inCamera, RTGRenderMask eMask);
    void RenderSceneTile(RtgCamera& inCamera);
    void RenderSceneObjectShadow(RtgCamera& inCamera);
    void RenderSceneMirrorObject(RtgCamera& inCamera);
    void RenderSceneWater(RtgCamera& inCamera);
    void PreRenderIndex();
    void WashOutRenderIndex();

    RtsSTileMap* GetTileMap(int iX, int iY);                        // x, y 为网格的索引
    RtsSVertex* GetBlockVertex(int x, int y);       // 取得顶点的数据,  x, y 为顶点的索引
    RtsSGrid* GetGridByPos(float fPosX, float fPosY);       // x, y 为位置
    bool GetGridByPos(RtgVertex3 vPos, int &iGx, int &iGy);

    bool GetGridByPos(float fPosX, float fPosY, int &iGx, int &iGy)
    {
        OffsetToMap(fPosX, fPosY);
        iGx = fPosX / (g_fSceneTileWidth*RTS_BLOCK_GRID_SIZE);
        iGy = fPosY / (g_fSceneTileWidth*RTS_BLOCK_GRID_SIZE);
        if (iGx<0 || iGy<0) return false;
        return true;
    }


public:

	inline void   SetEnableFadeInOut( bool bFade )   { m_bEnableFadeInOut = bFade ; }
	inline bool   GetFadeOutIn() const               { return m_bEnableFadeInOut;   }
    inline RtgDevice* GetDevice()                    { return m_pDevice;            }
    inline bool   SupportTerrainRender_PP() const    { return !!m_pDevice->GetEffect(Rtg_Effect_Terrain); }
public:
    RtgDevice*          m_pDevice;
    float               m_fPreLoadBlockDistance;    // 预载入场景块的距离
    float               m_fLoadBlockDistance;       // 预载入场景物件的距离
    float               m_fPreLoadObjectDistance;   // 载入场景块的距离
    float               m_fLoadObjectDistance;      // 载入场景物件的距离
    long                m_iMaxBufferBlockCount;

    // OnMoveTo信息
    float               m_fLastOnFrameMoveTime;
    //int                 m_iCurrentBlockX;
    //int                 m_iCurrentBlockY;
    //std::list<RtsSceneBlockMap::SArea*> m_listArea;

    // 灯光
    DWORD               m_lightSkyColor;
    BOOL                m_bUseDynamicLight;
    RtsDynamicLight*    m_pFirstDynamicLight;

    BOOL                m_bShowBlockFlag;                   // [Default = TRUE ] 在进入场景的时候是否显示场景标记
    float               m_fShowBlockFlagTime;               // 显示的时间，如果大于0表示正在显示
    RtgShader           m_shadowBlockFlag;
    RtString            m_strShowBlockFlagTextureName;      // 贴图文件名

    BOOL                m_bRenderMiniMapMode;               // 渲染小地图模式(正交投影)
    //char                m_pCameraSaveBuffer[64];            // 用来保存Camera的缓冲区
	CM_MEMDEF(m_pCameraSaveBuffer, 64)

    float               m_fMinimapWidth, m_fMinimapHeight;  // 小地图一张贴图相对于地图的大小
    //char                m_szMinimapFile[256];
	CM_MEMDEF(m_szMinimapFile, 256)
    RtgShader           m_ShaderMinimap;
    RtString            m_strMinimapMaskTexture;            // 小地图背后的那张图，可以使小地图显示为圆的或者其他形状
    float               m_fMinimapMaskLeft;
    float               m_fMinimapMaskTop;
    float               m_fMinimapMaskRight;
    float               m_fMinimapMaskBottom;
    float               m_fMinimapRotate;
	bool                m_bEnableFadeInOut;
	bool                m_bSceneModify;

	multimap<short,RtsModel* , std::less<short> >  m_mapSceneObj;                     // 需要载入的物件表，按优先级排序
	
public:
	BOOL                m_bAsynchronism;                    //是否异步载入物件

    // 灯光
    int                 m_iCustomLightCount;                // 用户自定义的灯光数量，默认为0，其他灯光的定义就从这个Count后面开始
    RtgLightItem        m_lightDirect;
    RtgVertex3          m_lightTerrainAmbient;

    gridlist_t          m_listRenderGrid;

    BOOL                m_bReflectSceneObject;  // 反射场景物件
    BOOL                m_bReflectOtherActor;   // 反射除场景物件以外的其它物件
    BOOL                m_bHaveReflect;         // 是否有反射物件
   // DWORD               m_dwRenderFlag;         // 渲染标记
    //RtsObject*          m_pRenderObjHead;       // 第一个渲染物件的头

    RtsFootPrint        m_FootPrint;
    RtsSLightSet        m_lightSet;
    RtsWayPoint        *m_pathPoint;             //tim.yang  声明路点列表类对象RtsWayPoint
    DWORD               m_dwVB, m_dwIB;

    RtsSRenderTile*     m_pRenderTile;
    int                 m_iRenderTileCount;                 // 总共分配内存的Tile个数

    RtsSceneRender      m_SceneRender;
    RtsGrassRender      m_GrassRender;
    RtsObjectRender     m_ObjectRender;

    RtsSceneTexture      m_texMaps[MAX_MAP_COUNT];             
    RtsSSceneWater       m_Waters[MAX_WATER_COUNT];    
    RtsSRenderMap        m_WaterRenderMap[MAX_WATER_COUNT]; 
    int                 m_iRenderTileCur;                   // 当前使用Tile的个数



    RtsSRenderMap*      m_pWaterRenderMapHead;
    int                 m_iWaterRenderTileCur;              // 当前Water使用Tile的个数
	
    RtsSTexUVPattern    m_TexUVPattern[32];                 // 贴图坐标的模版
    RtgVertex2*         m_pTexPatternUV;                    // 贴图模版的UV数组

	RtgMatrix16			m_camMatPrev;
    Int2                m_blockPosPrev;

    struct _TransActorHolder
    {
        CRT_ActorInstance* act;
        float fVisible;

        _TransActorHolder(CRT_ActorInstance* _act, float fv)
            : act(_act), fVisible(fv)
        {
        }
    };

    vector<_TransActorHolder> m_vecTransAct;
	
	
    // 优化选项
    BOOL                m_bOptionWaterCatoptricSky;         // [Default = TRUE ] 水面反射天空内容 (环境贴图，并非真实反射)
#if RTS_DRAW_HELPER
    BOOL                m_bDrawHelper;                      // 是否画辅助信息
    static BOOL         m_bRenderObjectBoundbox;            // 是否渲染物件的BoundBox
#endif
    static BOOL         m_bRenderScene;                     // 是否渲染场景，通常用于调试
    static BOOL         m_bRenderSceneWater;                // 是否渲染水面，通常用于调试

	protected:
		RtSceneManager*  mpkSceneManager;
		float            mfObjectVisibleDistance;
		float            mfBlendDistance;
		RtgVertex3       mkHeroPosition;
	protected:
		bool  DetectObjectVisiblity( float fDistanceToCamera , RtsModel* pObject );

};

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
