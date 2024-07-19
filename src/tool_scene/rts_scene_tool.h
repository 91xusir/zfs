
#include "scene/rts_scene_area.h"
class CBrushTool;

class RtSceneTool : public RtScene
{
public:
    RtSceneTool();
    virtual ~RtSceneTool();

    bool ToolRender(RtgCamera& inCamera, RtgDevice& inDevice);
    bool ToolNewScene(int iTileSizeX, int iTileSizeY);
    bool ToolLoadScene(const char* szFilename);
	bool ToolLoadScene(const char* szFilename,int flag);
    bool ToolSaveScene(const char* szFilename);
    bool ToolSave_CurrBlock();

    void GenerateMiniMap(int x, int y);

    virtual void OnBlockChanged(RtsSceneBlockMap* pNewBlock, RtsSceneBlockMap* pOldBlock);

    void GridItemChanged();

    void AddAreaFromScreen();
    void DeleteArea(RtsAreaHelper* pArea);
    void SelectArea(RtsAreaHelper* pArea); // 选中该区域
    void MoveToArea(RtsAreaHelper* pArea); // 屏幕移动到区域块位置

    RtsAreaHelper* AreaInPoint(int iMouseX, int iMouseY, int iOrder);

private:
    void DrawArea(long lMinX, long lMinY, long lMaxX, long lMaxY, DWORD dwColor);

public:
    BOOL            m_bRender;

    BOOL            m_bDrawGridBox;         // Grid 的 Box
    BOOL            m_bDrawWireframe;       // 画线框模式
    BOOL            m_bDrawNormal;          // 是否画出法向量
    BOOL            m_bDrawAttrWalk;        // 是否画出行走阻挡
    BOOL            m_bDrawAttrFly;         // 是否画出飞行阻挡
    BOOL            m_bDrawAttrDig;         // 是否画出钻地阻挡
    BOOL            m_bDrawMinimap;         // 是否画小地图

    BOOL            m_bAreaTool;            // 是否是区域工具
	BOOL            m_bPathPointTool;       // tim.yang   是否是路点工具
    RtsSceneArea    m_Areas;
    RtsAreaHelper*  m_pSelectedArea;
    float           m_fScale;
    float           m_fViewPosX;
    float           m_fViewPosY;
    BOOL            m_bIsPaint;
    int             m_iHittestOrder;
    int             m_iPaintbrushMinX;
    int             m_iPaintbrushMinY;
    int             m_iPaintbrushMaxX;
    int             m_iPaintbrushMaxY;
    float           m_fPervX;
    float           m_fPervY;


    int m_iCurrentBlockX, m_iCurrentBlockY;
    //std::list<RtsSceneBlockMap::SArea*> m_listArea;
    virtual void OnPositionChanged(float fX, float fY);

private:
    BOOL                        m_bUpdateViewAreas;
    std::list<RtsAreaHelper*>   m_listViewAreas;
	RtsSceneBlockMap*			m_pBlockMap;
};




/*------------------------------------------------------------------------
  The End.
------------------------------------------------------------------------*/
