
typedef union _objectId
{
    struct 
    {
        unsigned int data0;
        unsigned int data1;
        unsigned int data2;
        unsigned int data3;
    };

    byte data[16];

    bool operator < (const _objectId& _id) const
    {
        return memcmp(data, _id.data, 16) < 0;
    }

    bool operator == (const _objectId& _id) const
    {
        return memcmp(data, _id.data, 16) == 0;
    }

    _objectId()
        : data0(0), data1(0), data2(0), data3(0)
    {
    }

    string to_string() const
    {
        string str;
        char sz[32];
        sprintf(sz, "%u", data0);
        str += sz;
        sprintf(sz, "%u", data1);
        str += sz;
        sprintf(sz, "%u", data2);
        str += sz;
        sprintf(sz, "%u", data3);
        str += sz;
        return str;
    }

} objectId;

enum RtsEPick
{
    PICK_TERRAIN   = 0x01, // ���ߵ�Terrain (���ɼ�)
    PICK_OBJECT    = 0x02, // ���еĳ������ (�ɼ�ģ��)
    PICK_NPC       = 0x04, // ���е�����
    PICK_VTERRAIN  = 0x08, // ��ʾ��Terrain (�ɼ�) 
    PICK_TRIANGLE  = 0x10, // ��ײ�����ʱ��ȷΪ��ײ�����Σ�û�����ѡ���ֻ��ײBoundBox
    PICK_HELPER    = 0x20, // ���еĳ������ɼ���� (�磺�������ƹ��)
    PICK_DEFAULT   = PICK_TERRAIN | PICK_OBJECT | PICK_NPC, // Ĭ�ϵ����
};

// ����� (���ڿ��ټ���)
struct RtsSGrid
{
    RtSceneBlock*   pBlock;
    int             x, y;               // ����ʼ������(��ʾ����)
    RtgVertex3      vMin;               // �����BoundBox��Min
    RtgVertex3      vMax;               // �����BoundBox��Max
    farray<objectId>  aryObject;
    list<RtsDynamicLight*>  dLight; // ��̬�ƹ�
    bool            bObjLoad;
};

typedef std::list<RtsSGrid*> gridlist_t;


// ���㣬��������λ�ã�����������ɫ
struct RtsSVertexValue : public RtgVertexVNC
{
    float fTerrainHeight;   // ��������߶�
    float fWaterHeight;     // ˮ��߶�
    BYTE  cHeightMap;       // �߶�ͼ
    BYTE  cTerrainHMap;     // ���ߵĸ߶�ͼ������������֮��Ķ�����
    BYTE  cWaterHMap;       // ˮ�ĸ߶�ͼ��0��ʾû��ˮ
    BYTE  cWaterAlphaMap;   // ˮ�Ķ���͸���� (0-255) (ȫ͸��-��͸��)
    DWORD dwTerrainColor;   // ����Terrain����ɫ
};

// �ر���
struct RtsSVertex
{
    int                 iIndex; // ������VB�е�����
    RtsSVertex*         pNext;  // ��һ�������ָ��
    RtsSVertexValue*    pPos;
    RtgVertex2*         pUV[4];
    RtgVertex2          uvSM;
};

// �ر�����
struct RtsSIndex
{
    WORD        wIdx;
    WORD        wRenderIdx;
    RtsSIndex*  pNext;
};

// �ر���Ⱦ��������
struct RtsSRenderVertex
{
    RtsSVertex*     pHead;
    int             iStart;
    int             iCount;
    int             iStride;
    RTGVertexFormat eFormat;
};

struct RtsSRenderTile
{
    RtSceneBlock*   pBlock;
    WORD            iIdx;   // ���������
    WORD            iUV;
    int             iNext;  // ��һ������
};
struct RtsSRenderMap
{
    int             iStart;
    int             iCount;
    int             iCost;  // ��Ҫ�ȣ����С�� MIN_INDEX_COST �Ϳ���ɾ��
    bool            bLoaded;
    bool            bUse;
    RtsSRenderMap*  pNext;
    int             iTile;
    RtgShader       Shader;
    DWORD           dwColor;
    float           fColorRate;

    RtsSRenderMap()
    {
        bUse = false;
        bLoaded = false;
        iStart = 0;
        iCount = 0;
        iCost = 0;  // ��Ҫ�ȣ����С�� MIN_INDEX_COST �Ϳ���ɾ��
        dwColor = 0;
        fColorRate = 0.f;
        pNext = NULL;
    }
};

typedef struct _RtgVertexGrass
{
    RtgVertex3 v;
    DWORD c;
    RtgVertex2 t;
    enum { VF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1};

} RtgVertexGrass;

/*
struct GrassInfo
{
    GrassInfo()
    {
    }

    RtgVertex3 pos;
    RtgVertex3 dir;
    float      szh;
    float      szw;
    WORD       tId;
    float      rgt;
};
*/

typedef struct _RtsRenderTile2
{
    RtSceneBlock* block;
    WORD tile;
    BYTE tmap[4]; 

    bool operator < (const _RtsRenderTile2& rt) const
    {
        if ((int)(DWORD_PTR)block < (int)(DWORD_PTR)rt.block)
            return true;
        if ((int)(DWORD_PTR)block == (int)(DWORD_PTR)rt.block)
            return (*(DWORD*)tmap) < (*(DWORD*)rt.tmap);
        return false;
    }

    bool operator != (const _RtsRenderTile2& rt) const
    {
        return ((int)(DWORD_PTR)block != (int)(DWORD_PTR)rt.block) ||
               (*(DWORD*)tmap) != (*(DWORD*)rt.tmap);
    }

} RtsRenderTile2;

// ������ͼ
struct RtsSTileMap
{
    DWORD dwSortMapIdx;   
    BYTE  mapCnt;          
    BYTE  cWaterType;     

    // mtl info
    union {
        WORD uv;
        struct {
            WORD uv1:4;
            WORD uv2:4;
            WORD uv3:4;
            WORD uv4:4;
        };
    };
    BYTE mtl[4];

    // grass info
    RtgVertexGrass* grassVertex;
    WORD  grassId;
    WORD  grassDensity;
    float minH;
    float maxH;
    float aspectW;

    WORD wNext;
};


// ��ͼģʽ
struct RtsSTexUVPattern // ������±����ģʽ
{
    int         iCnt;       // ����ģʽ����ͼ�ж�������ͼ����
    RtgVertex2* pUV;        // ����ģʽ����ͼ���������
};

struct RtsSLightSetEnum
{
    int        iHour;
    RtgVertex3 vSunColor;
    RtgVertex3 vSunSpecular;
    RtgVertex3 vAmbientColor;
    RtgVertex3 vTerrainAmbientColor;
};

struct RtsSLightSet
{
    RtString            szName;
    int                 iEnumCnt;
    RtsSLightSetEnum    pEnums[10];
    RtsSLightSet*       pNext;
};

namespace rt_graph {
    struct _RtgTextItem;
    typedef _RtgTextItem RtgTextItem;
}

typedef struct _RtsSceneTexture
{
    RtString        szName; 
    RtString        szFile; 
    int             iType;  
    RtgTextItem*    texItem;

} RtsSceneTexture;

struct RtsSSceneWater
{
    RtString        szName;
    RtString        szFile;
    int             iAlpha;
    int             iDepth;
    DWORD           dwColor;
    float           fColorRate;
};

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
