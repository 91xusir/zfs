
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

    bool OnceInit(RtgDevice* pDevice);  // һЩ�����ĳ�ʼ������Щ��ʼ��ֻ��һ��
    bool OnceClear();

    bool Init();
    void Clear();

    void OnBlockLoadFinish(RtsSceneBlockMap* _blockMap);

    void BuildShadowMap();
    void BuildShadowMap_ForCurrBlock();
    void BuildShadowMap(const Int2& _blockIndex);
    void ProcessSM(RtgTextItem* texSrc, int sizSrc, RtgTextItem* texDst, int sizDst);
	void ViewCenter();

    // ��Ⱦ
    //    inCamera ����Ⱦ��Camera
    //    pDevice ����Ⱦ������
    bool Render(RtgCamera& inCamera, RTGRenderMask eMask);
    void DrawTerrain(RtgCamera& inCamera, BYTE cAttrMask, DWORD dwNormalColor=0xFF88FF88, DWORD dwHitColor=0xFFFF0000, float fOffset=0.5f);
    void DrawMiniMap(int iScreenX, int iScreenY, int iScreenWidth, int iScreenHeight, float fSceneWidth, float fCenterX, float fCenterY);
    void RenderSceneObjectHighLight(RtgCamera& inCamera);
	void SetRenderMode(BOOL bMiniMap);

    void ResetRenderEntity();

    // �߼�һ֡�Ĵ���
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

    // ���µƹ�, �����������ʱ����µ�ǰ����ɫ�������������ƽ�й����ɫ�Լ�����
    //    iX, iY ����ҵ�ǰ���ڵ�λ��
    //    iMinute �ǵ�ǰ��ʱ��(����, ��0�㿪ʼ����ķ���)
    void UpdateSkyLight(int iX, int iY, int iMinute);

    // �������ڵ�λ��
    void UpdateAudioListener(const RtgVertex3& vPos);

    bool SaveEnvironmentInfo(const char* szFilename);
    bool LoadEnvironmentInfo(const char* szFilename);

    void RecomputeAllLight();
    void RecomputeObjectBelong();
    int CheckSameObject();       // ��鳡������ȫ��ͬ������(λ�ú��������ͬ)
    int DeleteSameObject();      // ɾ����������ȫ��ͬ������(λ�ú��������ͬ)

    bool DeleteBlockMap(int x, int y);
  //  void GetRenderObject(RtsObject**& plistObj, int& nNum);



public: // Util
   // virtual RtgLight* CreateDynamicLight();
  //  virtual void DestroyDynamicLight(RtgLight* pLight);
  //  virtual void UpdateDynamicLight(RtgLight* pLight);

public:
   // void IncreaseRenderFlag();
    void GetSortTileMapIdx(RtsSTileMap& tileMapIdx);
    RtSceneBlock* GetTerrainByPos(float fPosX, float fPosY, int& iTx, int& iTy); // x, y λ��
    RtSceneBlock* GetTileByPos(float fPosX, float fPosY, int& iTx, int& iTy); // x, y λ��
    RtSceneBlock* GetTileByPos(RtgVertex3& vPos, int& iTx, int& iTy); // x, y λ��
    void GetPosByTile(int iTileX, int iTileY, RtgVertex3& vPos);
    void GetPosByTerrain(int iTerrainX, int iTerrainY, RtgVertex3& vPos);
    RtSceneBlock* GetBlockByPos(float fPosX, float fPosY);  // x, y Ϊλ��
    RtSceneBlock* GetBlockByPos(const RtgVertex3& vPos);    // vPos Ϊλ��
    RtSceneBlock* GetBlockByTile(int iX, int iY, int* pBx=0, int* pBy=0); // x, y Ϊ���������
    RtSceneBlock* GetBlock(int iX, int iY);         // x, y Ϊ�������
    RtSceneBlock* GetFirstBlock();
    RtSceneBlock* GetNextBlock(RtSceneBlock* pCur);
    void SetTileMapMtl(int iX, int iY, BYTE cMtl, DWORD dwFlag);    // x, y Ϊ���������
    void SetTileMapGrass(int iX, int iY, int Density, WORD tid, RtgVertex2 szh, float AspectW);
    void SetTileWaterType(int iX, int iY, BYTE cWaterType);         // x, y Ϊ���������
    RtsSVertexValue* GetVertexValue(int x, int y, bool bEdit);  // ȡ�ö�����������, x, y Ϊ���������
    BYTE GetVertexHeightLevel(int x, int y, bool bTile);                        // x, y Ϊ���������
    void SetVertexHeightLevel(int x, int y, bool bTerrain, bool bTile, BYTE cHeightLevel);     // x, y Ϊ���������

    //BYTE GetOpFlag(int x, int y);                   // x, y Ϊ���������

    int GetTerrainMapIdx(float fX, float fY);           // �������ڵ����ͼ��ţ�-1��ʾû���ҵ�, x, y Ϊλ��
    int GetTerrainMapType(float fX, float fY);          // x, y Ϊλ��
    const char* GetTerrainMapFile(float fX, float fY);  // x, y Ϊλ��
    const char* GetTerrainMapName(float fX, float fY);  // x, y Ϊλ��
    DWORD GetTerrainColor(float fX, float fY);          // �������ڵ�Ķ�����ɫֵ
    void GetTerrainColor(float fX, float fY, RtgVertex3& vColor);   // x, y Ϊλ��

    // ���ص�X��Y�������������XY, fOutDist������ײ��ľ���, ��������ֵ����ײ����ģ�ͣ����û��ģ�Ͷ��ǵر����-1
    RtsObject* PickLine(const RtgVertex3 &v0, const RtgVertex3 &v1, float& fOutDist, int& fOutX, int& fOutY, DWORD dwType);
    RtsObject* PickRay(const RtgVertex3 &v, const RtgVertex3 &dir, float& fOutDist, int& fOutX, int& fOutY, DWORD dwType);
    bool GetHeight(const RtgVertex3 &v, float& fOutDist, int& fOutX, int& fOutY, DWORD dwType);
    bool GetTerrainHeight(float fX, float fY, float& outZ); // x, y Ϊλ��

    // ����һ�������ڵ�����
    //    iMinX, iMinY, iMaxX, iMaxY �����������, ����Min������Max
    void UpdateGrid(int iMinX, int iMinY, int iMaxX, int iMaxY);
    void UpdateVertexPositionByHeightmap(int iMinX, int iMinY, int iMaxX, int iMaxY);
    void UpdateVertexPositionByTerrainHeightmap(int iMinX, int iMinY, int iMaxX, int iMaxY);

    void ComputeVertexNormal(int iX, int iY);   // x, y Ϊ�����������һ����Ķ�����Ϊ(g_iSceneTileCntX+1)*(g_iSceneTileCntY+1)
    void ComputeVertexColor(int iX, int iY);    // x, y Ϊ�����������һ����Ķ�����Ϊ(g_iSceneTileCntX+1)*(g_iSceneTileCntY+1)

    bool AddObject(objectId _objId);
    bool RemoveObject(objectId _objId);

	//���ƽ������
	//BOOL AddObjToQueue(RtsObject* pObject);
	//void LoadObjInQueue();

    BOOL AddDynamicLightToScene(RtsDynamicLight* pLight);
    BOOL RemoveDynamicLightByScene(RtsDynamicLight* pLight);

    // ȡ��Ӱ��ָ��λ�õĵƹ��б�, cFlag & 0x01��̬�ƹ�, 0x02��̬�ƹ�
    RtsLight* GetRangeLights(RtgVertex3& vPos, float fRadius=200.f, BYTE cFlag=0x03);

    // ���һ����Ⱦ���嵽�����У����������Ҫ��Ⱦ����ô��ÿ����Ⱦ��ʱ����Ҫ���
    //     ���еĳ�ʼ�����ڵ��ó�����PreRender��ʱ��
    //bool AddRenderObject(const RtgVertex3& vPos, RtsObject* pObject);

    // ȡ��ָ����Χ����������Ϣ
    //     inAABB    Ϊָ���ķ�Χ
    //     pVertices ���ڱ��涥���ָ��
    //     iVertexNum �������Ĵ�С(���Ա����RtgVertex3����)��ִ������������洢���涥��ĸ���
    //     pIndices  ���ڱ���������ָ��
    //     iIndexNum  �������Ĵ�С(���Ա����short����)��ִ������������洢���������ĸ���
    //  ����: true��ʾ�ɹ�, false��ʾʧ��
    //     ���ܳɹ�����ʧ�ܶ���ı� iVertexNum �� iIndexNum ��ֵ
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

    // �ƹ�
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

    // �Ż�ѡ��
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

    RtsSTileMap* GetTileMap(int iX, int iY);                        // x, y Ϊ���������
    RtsSVertex* GetBlockVertex(int x, int y);       // ȡ�ö��������,  x, y Ϊ���������
    RtsSGrid* GetGridByPos(float fPosX, float fPosY);       // x, y Ϊλ��
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
    float               m_fPreLoadBlockDistance;    // Ԥ���볡����ľ���
    float               m_fLoadBlockDistance;       // Ԥ���볡������ľ���
    float               m_fPreLoadObjectDistance;   // ���볡����ľ���
    float               m_fLoadObjectDistance;      // ���볡������ľ���
    long                m_iMaxBufferBlockCount;

    // OnMoveTo��Ϣ
    float               m_fLastOnFrameMoveTime;
    //int                 m_iCurrentBlockX;
    //int                 m_iCurrentBlockY;
    //std::list<RtsSceneBlockMap::SArea*> m_listArea;

    // �ƹ�
    DWORD               m_lightSkyColor;
    BOOL                m_bUseDynamicLight;
    RtsDynamicLight*    m_pFirstDynamicLight;

    BOOL                m_bShowBlockFlag;                   // [Default = TRUE ] �ڽ��볡����ʱ���Ƿ���ʾ�������
    float               m_fShowBlockFlagTime;               // ��ʾ��ʱ�䣬�������0��ʾ������ʾ
    RtgShader           m_shadowBlockFlag;
    RtString            m_strShowBlockFlagTextureName;      // ��ͼ�ļ���

    BOOL                m_bRenderMiniMapMode;               // ��ȾС��ͼģʽ(����ͶӰ)
    //char                m_pCameraSaveBuffer[64];            // ��������Camera�Ļ�����
	CM_MEMDEF(m_pCameraSaveBuffer, 64)

    float               m_fMinimapWidth, m_fMinimapHeight;  // С��ͼһ����ͼ����ڵ�ͼ�Ĵ�С
    //char                m_szMinimapFile[256];
	CM_MEMDEF(m_szMinimapFile, 256)
    RtgShader           m_ShaderMinimap;
    RtString            m_strMinimapMaskTexture;            // С��ͼ���������ͼ������ʹС��ͼ��ʾΪԲ�Ļ���������״
    float               m_fMinimapMaskLeft;
    float               m_fMinimapMaskTop;
    float               m_fMinimapMaskRight;
    float               m_fMinimapMaskBottom;
    float               m_fMinimapRotate;
	bool                m_bEnableFadeInOut;
	bool                m_bSceneModify;

	multimap<short,RtsModel* , std::less<short> >  m_mapSceneObj;                     // ��Ҫ���������������ȼ�����
	
public:
	BOOL                m_bAsynchronism;                    //�Ƿ��첽�������

    // �ƹ�
    int                 m_iCustomLightCount;                // �û��Զ���ĵƹ�������Ĭ��Ϊ0�������ƹ�Ķ���ʹ����Count���濪ʼ
    RtgLightItem        m_lightDirect;
    RtgVertex3          m_lightTerrainAmbient;

    gridlist_t          m_listRenderGrid;

    BOOL                m_bReflectSceneObject;  // ���䳡�����
    BOOL                m_bReflectOtherActor;   // ������������������������
    BOOL                m_bHaveReflect;         // �Ƿ��з������
   // DWORD               m_dwRenderFlag;         // ��Ⱦ���
    //RtsObject*          m_pRenderObjHead;       // ��һ����Ⱦ�����ͷ

    RtsFootPrint        m_FootPrint;
    RtsSLightSet        m_lightSet;
    RtsWayPoint        *m_pathPoint;             //tim.yang  ����·���б������RtsWayPoint
    DWORD               m_dwVB, m_dwIB;

    RtsSRenderTile*     m_pRenderTile;
    int                 m_iRenderTileCount;                 // �ܹ������ڴ��Tile����

    RtsSceneRender      m_SceneRender;
    RtsGrassRender      m_GrassRender;
    RtsObjectRender     m_ObjectRender;

    RtsSceneTexture      m_texMaps[MAX_MAP_COUNT];             
    RtsSSceneWater       m_Waters[MAX_WATER_COUNT];    
    RtsSRenderMap        m_WaterRenderMap[MAX_WATER_COUNT]; 
    int                 m_iRenderTileCur;                   // ��ǰʹ��Tile�ĸ���



    RtsSRenderMap*      m_pWaterRenderMapHead;
    int                 m_iWaterRenderTileCur;              // ��ǰWaterʹ��Tile�ĸ���
	
    RtsSTexUVPattern    m_TexUVPattern[32];                 // ��ͼ�����ģ��
    RtgVertex2*         m_pTexPatternUV;                    // ��ͼģ���UV����

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
	
	
    // �Ż�ѡ��
    BOOL                m_bOptionWaterCatoptricSky;         // [Default = TRUE ] ˮ�淴��������� (������ͼ��������ʵ����)
#if RTS_DRAW_HELPER
    BOOL                m_bDrawHelper;                      // �Ƿ񻭸�����Ϣ
    static BOOL         m_bRenderObjectBoundbox;            // �Ƿ���Ⱦ�����BoundBox
#endif
    static BOOL         m_bRenderScene;                     // �Ƿ���Ⱦ������ͨ�����ڵ���
    static BOOL         m_bRenderSceneWater;                // �Ƿ���Ⱦˮ�棬ͨ�����ڵ���

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
