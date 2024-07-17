
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rts_scene_render.h
//     Author : zxy
// CreateTime : 2009.12.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _RtsSceneRender_H_
#define _RtsSceneRender_H_

#define _ErrorTileNode 0xffff
#define _ErrorTileId   0xffff
#define _MaxTileMapNum 5

struct RtsTileNode
{
    RtsTileNode()
        : m_wChild(_ErrorTileNode), 
          m_wNext(_ErrorTileNode), 
          m_wTileNum(0),
          m_texItem(NULL), 
          m_wTile(_ErrorTileId)
    {
    }

    void Reset()
    {
        m_wChild = _ErrorTileNode;
        m_wNext  = _ErrorTileNode;
        m_wTileNum = 0;
        m_texItem  = NULL;
        m_wTile  = _ErrorTileId;

    }

    bool Leaf()
    {
        return m_wChild == _ErrorTileNode;
    }

    WORD m_wChild;
    WORD m_wNext;
    WORD m_wTileNum;
    RtgTextItem* m_texItem;
    WORD m_wTile;
};

class RtsSceneRender
{
    enum
    {
        Init_ib_Size = 1200
    };

    typedef struct _RtgTileVertexVNC
    {
        RtgVertex3 p;
        RtgVertex3 n;
        DWORD      c;
    } RtgTileVertexVNC;

public :
    RtsSceneRender();
    virtual ~RtsSceneRender();

    bool Init();
    void Insert(RtSceneBlock* pBlock, int tileId, RtgTextItem* texMap[]);
    void Reset();
    WORD RequireTileNode();
    void Render(RtScene* pScene);
    void RenderNormal(RtScene* pScene);

protected :
    void Render_FFP(RtScene* pScene);
    void Render_PP(RtScene* pScene);
    void RenderNode_FFP(RtScene* scene, RtSceneBlock* block, RtsTileNode* node, int level, RtgShader* sh);
    void RenderNode_PP(RtScene* scene, RtSceneBlock* block, RtsTileNode* node, int level, ID3DXEffect* eff);
    void Prepare_ib(size_t _Size);

public :

    RtsTileNode* GetTileNode(WORD nodeId)
    {
        if ((size_t)nodeId >= m_tnPoolSize)
            return NULL;
        return &m_tileNodePool[nodeId];
    }

public :
    typedef map<RtSceneBlock*, WORD> RenderBlockMap;
    RenderBlockMap      m_mapRenderBlock;
    RtsTileNode*        m_tileNodePool;
    size_t              m_tnPoolSize;
    WORD                m_freeNodeNext;
    int                 m_tileNumOfNodeMax;
    bool                m_bUseSM;
    int                 m_nProcessLevel;
    RtgBufferItem*      m_ib;
};

#endif