
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
    PICK_TERRAIN   = 0x01, // 行走的Terrain (不可见)
    PICK_OBJECT    = 0x02, // 所有的场景物件 (可见模型)
    PICK_NPC       = 0x04, // 所有的人物
    PICK_VTERRAIN  = 0x08, // 显示的Terrain (可见) 
    PICK_TRIANGLE  = 0x10, // 碰撞物件的时候精确为碰撞三角形，没有这个选项就只碰撞BoundBox
    PICK_HELPER    = 0x20, // 所有的场景不可见物件 (如：声音，灯光等)
    PICK_DEFAULT   = PICK_TERRAIN | PICK_OBJECT | PICK_NPC, // 默认的情况
};

// 网格块 (用于快速检索)
struct RtsSGrid
{
    RtSceneBlock*   pBlock;
    int             x, y;               // 网格开始的坐标(显示网格)
    RtgVertex3      vMin;               // 网格的BoundBox的Min
    RtgVertex3      vMax;               // 网格的BoundBox的Max
    farray<objectId>  aryObject;
    list<RtsDynamicLight*>  dLight; // 动态灯光
    bool            bObjLoad;
};

typedef std::list<RtsSGrid*> gridlist_t;


// 顶点，包含顶点位置，法向量，颜色
struct RtsSVertexValue : public RtgVertexVNC
{
    float fTerrainHeight;   // 行走网格高度
    float fWaterHeight;     // 水面高度
    BYTE  cHeightMap;       // 高度图
    BYTE  cTerrainHMap;     // 行走的高度图，用来做桥梁之类的东西用
    BYTE  cWaterHMap;       // 水的高度图，0表示没有水
    BYTE  cWaterAlphaMap;   // 水的顶点透明度 (0-255) (全透明-不透明)
    DWORD dwTerrainColor;   // 行走Terrain的颜色
};

// 地表顶点
struct RtsSVertex
{
    int                 iIndex; // 顶点在VB中的索引
    RtsSVertex*         pNext;  // 下一个顶点的指针
    RtsSVertexValue*    pPos;
    RtgVertex2*         pUV[4];
    RtgVertex2          uvSM;
};

// 地表索引
struct RtsSIndex
{
    WORD        wIdx;
    WORD        wRenderIdx;
    RtsSIndex*  pNext;
};

// 地表渲染顶点属性
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
    WORD            iIdx;   // 网格的索引
    WORD            iUV;
    int             iNext;  // 下一个网格
};
struct RtsSRenderMap
{
    int             iStart;
    int             iCount;
    int             iCost;  // 重要度，如果小于 MIN_INDEX_COST 就可以删除
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
        iCost = 0;  // 重要度，如果小于 MIN_INDEX_COST 就可以删除
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

// 网格贴图
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


// 贴图模式
struct RtsSTexUVPattern // 数组的下标就是模式
{
    int         iCnt;       // 这种模式的贴图有多少种贴图坐标
    RtgVertex2* pUV;        // 这种模式的贴图坐标的数组
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
