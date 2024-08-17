#ifndef RTS_SCENE_TERRAIN_H
#define RTS_SCENE_TERRAIN_H

enum {RTS_BLOCK_GRID_SIZE = 5};

const DWORD RT_SCENE_BLOCK_HEADER = 0x0ADFC3;// ��������ͷ�ĳ���ֵ�����ڱ�ʶ��������
const float g_fSceneTileWidth = 40.f;   // ÿһ����ʾ����Ŀ��
const int   g_iSceneTileRate = 2;// ��������ű��������ڼ�����������Ŀ��
const float g_fSceneTerrainTileWidth = g_fSceneTileWidth / g_iSceneTileRate; // ��������Ŀ�� = ��ʾ������ / ���ű���
const float g_fSceneHeightStep = 4.f;  // ÿ���Ӧ�ĸ߶ȣ����ڵ��θ߶ȼ��㣩��������ΧΪ0-255����ƽ�߸߶�Ϊ128
const int   g_iSceneTileCntX = 100;     // ������ (����ĸ���)
const int   g_iSceneTileCntY = 100;     // ����ĸ߶ȣ�����ĸ�����
const int   g_iSceneTerrainCntX = g_iSceneTileCntX * g_iSceneTileRate;      // ��������Ŀ�� = ��ʾ������ * ���ű���
const int   g_iSceneTerrainCntY = g_iSceneTileCntY * g_iSceneTileRate;      // ��������ĸ߶� = ��ʾ����߶� * ���ű���
const int   g_iSceneBlockFaceSize = g_iSceneTileCntX * g_iSceneTileCntY * 2; // ��ʾ���� = ������*2 (����������m_pIndices�Ĵ�С)
const int   g_iSceneGridCntX = g_iSceneTileCntX / RTS_BLOCK_GRID_SIZE;       // ���ӿ��� (�����Ż���ײ����ʾ������)
const int   g_iSceneGridCntY = g_iSceneTileCntY / RTS_BLOCK_GRID_SIZE;
const float g_fSceneBlockWidth  = g_fSceneTileWidth * g_iSceneTileCntX;// ��������Ŀ�� = ��ʾ������ * �������� 4000
const float g_fSceneBlockHeight = g_fSceneTileWidth * g_iSceneTileCntY;// ��������ĸ߶� = ��ʾ������ * �������� 4000

// �ڴ�ͳ��
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

    // ��������
    struct STileAttr
    {
        union
        {
            BYTE    cAttr;  // ��������
            struct
            {
                BYTE    bWalk:1;    // �Ƿ��������
                BYTE    bFly:1;     // �Ƿ���Է���
                BYTE    bDig:1;     // �Ƿ�����궴
				BYTE	bWayPoint:1;  //�Ƿ���waypoint  ac
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
    inline bool CanWalk(int iTerrainX, int iTerrainY) // ���������Ƿ����赲
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
    // ������Ϣ
    DWORD           m_dwVersion;
    int             m_iBlockPosX;       // ���������ڵ�ƫ����
    int             m_iBlockPosY;

    // ��������������Ϣ
    STileAttr       m_pTileAttr[g_iSceneTerrainCntY*g_iSceneTerrainCntX];   // ��ͼ������(��������)

public:
    // For path find.
    void ClearPathFindInfo(bool bDelete=true);
    void ClearPathFindIncreaseFlag();

};
#endif

/*------------------------------------------------------------------------
  The End.
------------------------------------------------------------------------*/
