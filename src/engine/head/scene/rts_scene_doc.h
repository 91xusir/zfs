#ifndef RTS_SCENE_DOC
#define RTS_SCENE_DOC

class RtsSceneDoc;
struct RtsSceneBlockRtb;

enum 
{
    Check_Release_Limit = 16
};


struct RtsSceneBlockRtb
{

public:
    friend class RtsSceneBlockManager;

    RtSceneBlockTerrain*    m_pBlock;
    struct SArea
    {
        const char* szName;
        const char* szData;
        long        lMinX;      // ����
        long        lMinY;
        long        lMaxX;
        long        lMaxY;
		bool		bActivation;//�����Ƿ񼤻�

		SArea(void) : szName(0), szData(0), bActivation(true),
			lMinX(0), lMinY(0), lMaxX(0), lMaxY(0)
		{
		}

    };

#ifndef REGION_SERVER
private :
#else
public :
#endif
    RtsSceneBlockRtb();
    ~RtsSceneBlockRtb();

public :

    void Serialize(DWORD dwMapVersion, RtArchive* ar);
    bool Save(const char* szPath);

    void SetFileName(const char* szFileName)    { CHECK(strlen(szFileName)<20); rt2_strncpy(m_szFileName, szFileName, 19); m_szFileName[19]=0; }
    void SetBlockName(const char* szBlockName)  { CHECK(strlen(szBlockName)<20); rt2_strncpy(m_szBlockName, szBlockName, 19); m_szBlockName[19]=0; }
    void SetBackMusicFileName(const char* szStr){ CHECK(strlen(m_szBackMusic)<20); rt2_strncpy(m_szBackMusic, szStr, 19); m_szBackMusic[19]=0; }
    void SetFlagTexture(const char* szStr)      { CHECK(strlen(m_szBlockFlagTexture)<20); rt2_strncpy(m_szBlockFlagTexture, szStr, 19); m_szBlockFlagTexture[19]=0; }
	void SetRegionID(const int id) {m_region_id = id;}

    const char* GetFileName()                   { return m_szFileName; }
    const char* GetBlockName()                  { return m_szBlockName; }
    const char* GetBackMusicFileName()          { return m_szBackMusic; }
    const char* GetMiniMapFile(int iIdx)        { return m_szMiniMapFile[iIdx]; }
    const char* GetMiniMapFile(int x, int y)    { return m_szMiniMapFile[y*2+x]; }
    const char* GetFlagTexture()                { return m_szBlockFlagTexture; }
	const int   GetRegionID()                   { return m_region_id;}

    void SetLoadInfo(const char szPath[], int posx, int posy)
    {
        rt2_sprintf(m_loadFileName, "%s%s", szPath, m_szFileName);
        m_blockPosx = posx;
        m_blockPosy = posy;
    }

    bool Check_CanLoad() const
    {
        return (GetState() == Object_State_New && (*m_szFileName));
    }

    void SetSunDir(float inclination, float rotation);


#ifndef REGION_SERVER
protected :
#else
public :
#endif
    bool New(const char* szPath, int iOffsetX, int iOffsetY);
    bool Load(const char* szPath, int iOffsetX, int iOffsetY, int iCenterX, int iCenterY);
    bool Destroy();

public :
    void SetState(RtObjState _state)
    {
        m_state = _state;
    }

    RtObjState GetState() const
    {
        return m_state;
    }

    void Loack_SetState(RtObjState _state)
    {
#ifdef WIN32
        InterlockedExchange(&m_state, _state);
#else
        m_state = _state;
#endif
    }

    bool ReourceReady() const
    {
        return m_state == Object_State_Ready;
    }

	bool ActivationBlockArea(const char *sName, bool bAct);

#if RTS_DRAW_HELPER
    void CreateHelper();
#endif


public:
    //char                    m_loadFileName[_MAX_PATH];
    //char                    m_szFileName[20];           // ��������ļ���
    //char                    m_szBlockName[20];          // �����������
	CM_MEMDEF(m_loadFileName, _MAX_PATH)
	CM_MEMDEF(m_szFileName, 20)
	CM_MEMDEF(m_szBlockName, 20)
    int                     m_region_id;                // �������region�ֲ�id

    DWORD                   m_dwEnvironmentLighting;    // ��������Ϣ
    DWORD                   m_dwSunLighting;            // ƽ�й���Ϣ
    float3                  m_sunLightDir;
    float                   m_fSunInclination;
    float                   m_fSunRotation;

    // ���ڴ���ͺ��ڼ�ǿ

    char                    m_bPostProcessEffect;               // �Ƿ�򿪺��ڴ���
    char                    m_bPostProcessObjectHL;             // �Ƿ������߹�
    float                   m_fPostProcessEffectContrast;       // ���ڴ���Աȶ�
    float                   m_fPostProcessEffectIntensity;      // ���ڴ���ǿ��
    float                   m_fPostProcessObjectHLScale;        // �������
    DWORD                   m_dwPostProcessObjectHLColorEmi;    // �Է���
    DWORD                   m_dwPostProcessObjectHLColorDif;    // ������
    DWORD                   m_dwPostProcessObjectHLColorSpe;    // �߹�
    float                   m_fPostProcessObjectHLColorSpePower;// �߹�ǿ��

    // ��

    DWORD                   m_dwFogColor;               // �����ɫ
    float                   m_fFogNear;
    float                   m_fFogFar;

    //char                    m_szBlockFlagTexture[20];   // �������־��ͼ(���ڽ��볡��ʱ��ı��)
	CM_MEMDEF(m_szBlockFlagTexture, 20)

    float                   m_fBackMusicStartTime;      // �ӻ����ֵ����ŵļ��ʱ��
    float                   m_fBackMusicDifferenceTime; // �������ֲ�����ɺ����೤ʱ���ٴβ���
    //char                    m_szBackMusic[20];          // ���������ļ�
	CM_MEMDEF(m_szBackMusic, 20)

    //char                    m_szMiniMapFile[4][20];     // С��ͼ�ļ���[0���ϣ�1���ϣ�2���£�3����]
	CM_MEMDEF2(m_szMiniMapFile, 4, 20)

    SArea**                 m_ppAreas;
    int                     m_iMapAreaCount;

    long                    m_lLastUseTime;

    int                     m_blockPosx;
    int                     m_blockPosy;




    RtObjState              m_state;
};

/*------------------------------------------------------------------------
-   RtsSceneDoc.
------------------------------------------------------------------------*/
const DWORD RT_MAP_FILE_HEADER = 0x0DDFC3;

const int g_iSceneBlockOffsetX = 0;
const int g_iSceneBlockOffsetY = 0;

class RtsSceneDoc
{
public:
    RtsSceneDoc();
    virtual ~RtsSceneDoc();

public:
    void InitMap();
    void ClearMap(); // �����ͼ

   

    virtual bool NewMap(const char* szMapFileName);
    virtual bool LoadMap(const char* szMapFileName, bool bLoadAll); // �����ļ��������ͼ, bLoadAllΪtrue��ʱ��Ͱ�ȫ�������ڴ�
    virtual bool SaveMap(const char* szMapFileName);
    virtual bool SaveAllMap(const char* szMapFileName);
    bool AddMapBlock(int iPosX, int iPosY);
	bool AddMapBlock(int iPosX, int iPosY,RtsSceneBlockRtb* pMap);
    void LoadAllBlock();

#ifndef REGION_SERVER
    void RemapObject();
#endif

    // ��ͼ������Ϣ

    // ����������Ϣ������ӵ���ǰ�����У�ע�⣬ֻ����Ӳ���ɾ��������������Ϣ���ᱻ����
    bool AddAreaInfo(const char* szAreaFileName);

    // ���ָ�����Ƿ���ָ�����ֵ�������
    bool PointInArea(float fPosX, float fPosY, const char* szAreaName);

    // �ҵ�ָ����ĵ�һ��������Ϣ
    //     �Ҳ����ͷ���NULL
    RtsSceneBlockRtb::SArea* FindFirstArea(float fPosX, float fPosY);

    // �ҵ���һ��������Ϣ����͸���FindFirstArea�ṩ�ĵ��λ�ã�������ʹ���������֮ǰһ��Ҫ����FindFirstArea
    //     �Ҳ����ͷ���NULL
    RtsSceneBlockRtb::SArea* FindNextArea();

    // ������Ϣ

    const char* GetMapFileName()        { return m_szMapFileName; }

#ifndef RT_SCENE_ONLY_LOAD_TERRAIN

    inline void GetRange(int _blockx, int _blocky, RtgVertex4* _range) const
    {
        RTASSERT(_range);
        RTASSERT(_blockx >= 0 && _blocky >= 0 && _blockx < m_iBlockXCnt && _blocky < m_iBlockYCnt);
        
        _range->x = (_blockx - m_iCenterPosX) * g_fSceneBlockWidth;
        _range->y = (_blocky - m_iCenterPosY) * g_fSceneBlockHeight;
        _range->z = _range->x + g_fSceneBlockWidth;
        _range->w = _range->y + g_fSceneBlockHeight;

    }

    inline void GetBlockInfo(const RtgVertex2& _pos, 
                             int& _blockx, 
                             int& _blocky, 
                             Int2 _adjacent[], 
                             bool _adjacent_force[],
                             int& _adjnum)
    {
        RTASSERT(_adjacent);

        const float _force = 400.f;

        float _mapx = _pos.x + m_iCenterPosX * g_fSceneBlockWidth;
        float _mapy = _pos.y + m_iCenterPosY * g_fSceneBlockHeight;

        if (_mapx < 0.f) 
            _mapx = 0;
        _blockx = (int)(_mapx / g_fSceneBlockWidth);

        if (_blockx >= m_iBlockXCnt) 
            _blockx = m_iBlockXCnt - 1;
        
        if (_mapy < 0.f) 
            _mapy = 0;
        _blocky = (int)(_mapy / g_fSceneBlockHeight);

        if (_blocky >= m_iBlockYCnt) 
            _blocky = m_iBlockYCnt - 1;

        const float _hblockw = g_fSceneBlockWidth * 0.5f;
        const float _hBlockh = g_fSceneBlockHeight * 0.5f;

        RtgVertex4 _range;
        GetRange(_blockx, _blocky, &_range);

        bool top    = (_range.w - _pos.y < _hBlockh);
        bool bottom = (_pos.y - _range.y < _hBlockh);
        bool left   = (_pos.x - _range.x < _hblockw);
        bool right  = (_range.z - _pos.x < _hblockw);

        bool top_force    = (_range.w - _pos.y < _force);
        bool bottom_force = (_pos.y - _range.y < _force);
        bool left_force   = (_pos.x - _range.x < _force);
        bool right_force  = (_range.z - _pos.x < _force);

        _adjnum = 0;

        if (top)
        {
            _adjacent[_adjnum].set(_blockx, _blocky + 1);

            if (top_force)
                _adjacent_force[_adjnum] = true;
            else
                _adjacent_force[_adjnum] = false;

            ++_adjnum;
        }

        if (bottom)
        {
            _adjacent[_adjnum].set(_blockx, _blocky - 1);

            if (bottom_force)
                _adjacent_force[_adjnum] = true;
            else
                _adjacent_force[_adjnum] = false;

            ++_adjnum;
        }

        if (left)
        {
            _adjacent[_adjnum].set(_blockx - 1, _blocky);

            if (left_force)
                _adjacent_force[_adjnum] = true;
            else
                _adjacent_force[_adjnum] = false;

            ++_adjnum;
        }

        if (right)
        {
            _adjacent[_adjnum].set(_blockx + 1, _blocky);

            if (right_force)
                _adjacent_force[_adjnum] = true;
            else
                _adjacent_force[_adjnum] = false;

            ++_adjnum;
        }

        if (top && left)
        {
            _adjacent[_adjnum].set(_blockx - 1, _blocky + 1);

            if (top_force && left_force)
                _adjacent_force[_adjnum] = true;
            else
                _adjacent_force[_adjnum] = false;

            ++_adjnum;
        }

        if (top && right)
        {
            _adjacent[_adjnum].set(_blockx + 1, _blocky + 1);

            if (top_force && right_force)
                _adjacent_force[_adjnum] = true;
            else
                _adjacent_force[_adjnum] = false;

            ++_adjnum;
        }

        if (bottom && left)
        {
            _adjacent[_adjnum].set(_blockx - 1, _blocky - 1);

            if (bottom_force && left_force)
                _adjacent_force[_adjnum] = true;
            else
                _adjacent_force[_adjnum] = false;

            ++_adjnum;
        }

        if (bottom && right)
        {
            _adjacent[_adjnum].set(_blockx + 1, _blocky - 1);

            if (bottom_force && right_force)
                _adjacent_force[_adjnum] = true;
            else
                _adjacent_force[_adjnum] = false;
        }

        RTASSERT(_adjnum < 8);
    }

    RtsSceneBlockRtb* GetBlockMapbyPos(const RtgVertex2& _Pos, int& _blockx, int& _blocky)
    {
        float fPosX = _Pos.x;
        float fPosY = _Pos.y;
        OffsetToMap(fPosX, fPosY);
        if (fPosX<0.f) _blockx = 0;
        _blockx = (int)(fPosX/(g_fSceneBlockWidth));
        if (_blockx>=m_iBlockXCnt) _blockx = m_iBlockXCnt-1;
        if (fPosY<0.f) _blocky = 0;
        _blocky = (int)(fPosY/(g_fSceneBlockHeight));
        if (_blocky>=m_iBlockYCnt) _blocky = m_iBlockYCnt-1;
        return GetBlockMapbyIndex(_blockx, _blocky);
    }

#endif

    RtsSceneBlockRtb* GetBlockMapbyIndex(int _blockx, int _bloxky)
    {
        if (_blockx >= 0 && _bloxky >= 0 && _blockx < m_iBlockXCnt && _bloxky < m_iBlockYCnt)
            return &m_blockMapArray[_bloxky * m_iBlockXCnt + _blockx];
        return NULL;
    }

    RtsSceneBlockRtb* GetBlockMap(int x, int y)
    {
        int iBx = x + m_iCenterPosX;
        int iBy = y + m_iCenterPosY;
        if (!(iBx>=0 && iBy>=0 && iBx<m_iBlockXCnt && iBy<m_iBlockYCnt)) return NULL;
        return &m_blockMapArray[iBy*m_iBlockXCnt+iBx];
    }

    void GetBlockByPos(float fPosX, float fPosY, int& iBx, int& iBy)
    {
        OffsetToMap(fPosX, fPosY);
        if (fPosX<0.f) iBx = 0;
        iBx = (int)(fPosX/(g_fSceneBlockWidth));
        if (iBx>=m_iBlockXCnt) iBx = m_iBlockXCnt-1;
        if (fPosY<0.f) iBy = 0;
        iBy = (int)(fPosY/(g_fSceneBlockHeight));
        if (iBy>=m_iBlockYCnt) iBy = m_iBlockYCnt-1;
    }

    inline void OffsetToWorld(float& fPosX, float& fPosY)
    {
        fPosX -= m_iCenterPosX * g_fSceneBlockWidth;
        fPosY -= m_iCenterPosY * g_fSceneBlockHeight;
    }
    inline void OffsetToWorld(int& iPosX, int& iPosY)
    {
        iPosX -= m_iCenterPosX * g_iSceneTileCntX;
        iPosY -= m_iCenterPosY * g_iSceneTileCntY;
    }
    inline void OffsetToMap(float& fPosX, float& fPosY)
    {
        fPosX += m_iCenterPosX*g_fSceneBlockWidth;
        fPosY += m_iCenterPosY*g_fSceneBlockHeight;
    }
    inline void OffsetToMap(int& iPosX, int& iPosY)
    {
        iPosX += m_iCenterPosX*g_iSceneTileCntX;
        iPosY += m_iCenterPosY*g_iSceneTileCntY;
    }
    inline void OffsetTerrainToWorld(int& iX, int& iY)
    {
        iX -= m_iCenterPosX * g_iSceneTerrainCntX;
        iY -= m_iCenterPosY * g_iSceneTerrainCntY;
    }
    inline void OffsetTerrainToMap(int& iX, int& iY)
    {
        iX += m_iCenterPosX * g_iSceneTerrainCntX;
        iY += m_iCenterPosY * g_iSceneTerrainCntY;
    }
    void GetTileByPosFast(float fPosX, float fPosY, int& iTx, int& iTy); // Not safe
    void GetPosByTileXY(int iTileX, int iTileY, float& fX, float& fY);
    void GetPosByTerrainXY(int iTerrainX, int iTerrainY, float& fX, float& fY);
    void GetTerrainByPosFast(float fPosX, float fPosY, int& iTx, int& iTy); // x, y λ��
	void GetTerrainByPosFast(float fPos[], float fDir[],short sRad[],int& iMinX, int& iMinY, int& iMaxX, int& iMaxY);
	void GetTerrainByPosFast(long lPos[], long lDir[],short sRad[],int& iMinX, int& iMinY, int& iMaxX, int& iMaxY);
    
  

    // ���������е�ʵ��λ��ȡ���ڳ��������е�����
    inline bool GetTerrainTile(float fPosX, float fPosY, int& iOutX, int& iOutY) // 
    {
        GetTerrainByPosFast(fPosX, fPosY, iOutX, iOutY);
        return true;
    }
    inline bool GetTerrainPos(int iTileX, int iTileY, float& fOutPosX, float& fOutPosY) // iTileX,iTileY Ϊ���������
    {
        GetPosByTerrainXY(iTileX, iTileY, fOutPosX, fOutPosY);
        return true;
    }
    int GetBlockCntX()  { return m_iBlockXCnt; }
    int GetBlockCntY()  { return m_iBlockYCnt; }
    int GetCenterX()    { return m_iCenterPosX; }
    int GetCenterY()    { return m_iCenterPosY; }



    RtSceneBlockTerrain* GetNextBlockTerrain(RtSceneBlockTerrain* pCur);

	// For the region id [2/15/2007 louis]
	short GetRegionIDByBlock(int bX, int bY);

    const RtSceneBlockTerrain::STileAttr* GetTerrainAttr(int iTerrainX, int iTerrainY);// x, y ΪTerrain�������
    bool SetTerrainAttr(int iTerrainX, int iTerrainY, BYTE cAttrMask);// x, y ΪTerrain�������
	bool RemoveTerrainAttr(int iTerrainX, int iTerrainY, BYTE cAttrMask);// x, y ΪTerrain�������
	bool GetCanWalk(int iTerrainX, int iTerrainY);// x, y ΪTerrain�������

    void ClearPathFindInfo();
    void ClearPathFindIncreaseFlag();
    RtSceneBlockTerrain::STileAttr* GetTerrainPathAttr(int iTerrainX, int iTerrainY);// x, y ΪTerrain�������
 
	RtsSceneBlockRtb* FindSceneBlockMapByAreaName(const char *sAreaName);

private:
    bool OnSerializeMapInfo(RtArchive* pFile);
    bool OnSerializeBlockMapping(RtArchive* pFile);
    virtual bool OnSerializeObjects(RtArchive* pFile);
    virtual void OnLoadMap(const char* szMapFileName, bool bLoadAll){}

    void SetMapFileName(const char* szMapFileName);
    RtSceneBlockTerrain* GetTerrainBlock(int iTerrainX, int iTerrainY, int& iBx, int& iBy);// x, y ΪTerrain�������

protected:
    //char                m_szFilePath[_MAX_PATH];    // ��ͼ����·��
    //char                m_szMapFileName[_MAX_PATH]; // ��ͼ�ļ���(����·��)
    //char                m_szMapSetName[20];         // ��ͼ������ּ��� setname_000.rtb
	CM_MEMDEF(m_szFilePath, _MAX_PATH)
	CM_MEMDEF(m_szMapFileName, _MAX_PATH)
	CM_MEMDEF(m_szMapSetName, 20)
    DWORD               m_dwMapVersion;     // �����汾
    int                 m_iBlockXCnt;       // �������X����ĸ���
    int                 m_iBlockYCnt;       // �������Y����ĸ���
    int                 m_iBlockCnt;        // ������ĸ���
    SceneBlockArray     m_blockMapArray;
    int                 m_iCenterPosX;      // ���Ŀ����ڵ�λ�ã��ÿ������Ϊ(0.f, 0.f)
    int                 m_iCenterPosY;                  
    int                         m_iAreaRef;
    RtsSceneDoc*                m_pAreaDoc;
    std::list<RtsSceneDoc*>     m_listAreaUser;

#ifndef REGION_SERVER
    RtsSceneBlockManager    m_SceneBlockManager;
#endif

    RtsSceneBlockRtb::SArea*    m_pAreas;
    int                         m_iAreaCount;
    char**                      m_ppAreaStringBuffer;
    int                         m_iAreaStringBufferCount;
public:
	bool                m_bMergeBlock;      // �Ƿ�ϲ�������
    int                 m_iOldBlockXCnt;    // ������ϲ�ǰ��X����ĸ���
	int                 m_iOldBlockYCnt;    // ������ϲ�ǰ��Y����ĸ���
};

#endif
/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
