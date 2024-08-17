#ifndef RTS_SCENE_TERRAIN_H
#define RTS_SCENE_TERRAIN_H

enum {RTS_BLOCK_GRID_SIZE = 5};

const DWORD RT_SCENE_BLOCK_HEADER = 0x0ADFC3;// 场景区块头的常量值，用于标识区块数据
const float g_fSceneTileWidth = 40.f;   // 每一个显示网格的宽度
const int   g_iSceneTileRate = 2;// 网格的缩放比例，用于计算行走网格的宽度
const float g_fSceneTerrainTileWidth = g_fSceneTileWidth / g_iSceneTileRate; // 行走网格的宽度 = 显示网格宽度 / 缩放比例
const float g_fSceneHeightStep = 4.f;  // 每格对应的高度（用于地形高度计算），格数范围为0-255，地平线高度为128
const int   g_iSceneTileCntX = 100;     // 网格宽度 (网格的个数)
const int   g_iSceneTileCntY = 100;     // 网格的高度（网格的个数）
const int   g_iSceneTerrainCntX = g_iSceneTileCntX * g_iSceneTileRate;      // 行走网格的宽度 = 显示网格宽度 * 缩放比例
const int   g_iSceneTerrainCntY = g_iSceneTileCntY * g_iSceneTileRate;      // 行走网格的高度 = 显示网格高度 * 缩放比例
const int   g_iSceneBlockFaceSize = g_iSceneTileCntX * g_iSceneTileCntY * 2; // 显示面数 = 网格数*2 (索引缓冲区m_pIndices的大小)
const int   g_iSceneGridCntX = g_iSceneTileCntX / RTS_BLOCK_GRID_SIZE;       // 格子块宽度 (用于优化碰撞和显示的网格)
const int   g_iSceneGridCntY = g_iSceneTileCntY / RTS_BLOCK_GRID_SIZE;
const float g_fSceneBlockWidth  = g_fSceneTileWidth * g_iSceneTileCntX;// 场景区块的宽度 = 显示网格宽度 * 网格数量 4000
const float g_fSceneBlockHeight = g_fSceneTileWidth * g_iSceneTileCntY;// 场景区块的高度 = 显示网格宽度 * 网格数量 4000

// 内存统计
class RtsPathContainer;
class RtsPathFind;

extern RtsPathFind*         g_pPathFind;
class RtScene;

struct RtsSceneBlockMap;
struct SceneBlockArray
{
    RtsSceneBlockMap* _blockArray;
    size_t            _size;

    SceneBlockArray(RtsSceneBlockMap*_ba = NULL, size_t _siz = 0)
        : _blockArray(_ba),
        _size(_siz)
    {
    }

    SceneBlockArray& operator = (const SceneBlockArray& _ba)
    {
        _blockArray = _ba._blockArray;
        _size = _ba._size;
        return *this;
    }

    bool operator == (const SceneBlockArray& _ba) const
    {
        return _blockArray == _ba._blockArray;
    }

    size_t size() const
    {
        return _size;
    }

    RtsSceneBlockMap& operator[] (size_t _index);
};

struct SPointScene
{
	int x, y;

	SPointScene()
	{
		x = 0; y = 0;
	}

	SPointScene(int _x, int _y)
	{
		x = _x; y = _y;
	}

	//SPoint& operator += (const SPoint& _Other)
	//{
	//	x += _Other.x;
	//	y += _Other.y;
	//	return *this;
	//}

	//SPoint& operator += (const SSize& _Size)
	//{
	//	x += _Size.width;
	//	y += _Size.height;
	//	return *this;
	//}

	//SPoint& operator- ()
	//{
	//	x = -x;
	//	y = -y;
	//	return *this;
	//}

	//bool FromString(const std::string& str);
	//std::string ToString();
};

class RtSceneBlockTerrain
{
public:
    struct SDisMap
    {
        WORD    wFlag;
        WORD    wDepth;
    };

    // 网格属性
    struct STileAttr
    {
        union
        {
            BYTE    cAttr;  // 网格属性
            struct
            {
                BYTE    bWalk:1;    // 是否可以行走
                BYTE    bFly:1;     // 是否可以飞行
                BYTE    bDig:1;     // 是否可以钻洞
				BYTE	bWayPoint:1;  //是否是waypoint  ac
            };
        };
        RtsPathContainer*   pContainer;
        SDisMap             dis;
    };

public:
    RtSceneBlockTerrain();
    virtual ~RtSceneBlockTerrain();

public:
    virtual bool LoadBlock(const char* szBlockFileName);

    inline bool GetModifyFlag()            { return m_bModifyFlag; }
    inline void SetModifyFlag(bool bFlag)  { m_bModifyFlag = bFlag; }

    inline BYTE GetTerrainAttr(int iTerrainX, int iTerrainY)
    {
        return m_pTileAttr[iTerrainY*g_iSceneTerrainCntY + iTerrainX].cAttr;
    }
	//jin.wang
	inline STileAttr* GetTerrain(int iTerrainX, int iTerrainY)
	{
		return &m_pTileAttr[iTerrainY*g_iSceneTerrainCntY + iTerrainX];
	}
    inline bool CanWalk(int iTerrainX, int iTerrainY) // 仅仅返回是否是阻挡
    {
        STileAttr* pAttr = m_pTileAttr + (iTerrainY*g_iSceneTerrainCntY + iTerrainX);
		if (!pAttr) return false;
        if (pAttr->cAttr!=0) return false;
        return true;
    }
    bool SetTerrainAttr(int iTerrainX, int iTerrainY, BYTE cAttrMask);
    bool RemoveTerrainAttr(int iTerrainX, int iTerrainY, BYTE cAttrMask);

protected:
    virtual bool OnSerializeChunk(RtArchive* pFile, DWORD dwType, DWORD dwSize);

    virtual bool OnResourceAllocate();
    virtual bool OnResourceFree();

    virtual bool OnSerializeBlockInfo(RtArchive* pFile);
    virtual bool OnSerializeBlockTerrain(RtArchive* pFile);
    virtual bool OnSerializeBlockTerrain(RtArchive* pFile, DWORD dwType);

private:
    bool            m_bModifyFlag;

public:
    // 场景信息
    DWORD           m_dwVersion;
    int             m_iBlockPosX;       // 场景块所在的偏移量
    int             m_iBlockPosY;

    // 场景行走网格信息
    STileAttr       m_pTileAttr[g_iSceneTerrainCntY*g_iSceneTerrainCntX];   // 地图块属性(行走网格)

public:
    // For path find.
    void ClearPathFindInfo(bool bDelete=true);
    void ClearPathFindIncreaseFlag();

};
#endif

/*------------------------------------------------------------------------
  The End.
------------------------------------------------------------------------*/
