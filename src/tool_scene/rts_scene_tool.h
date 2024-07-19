
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
    void SelectArea(RtsAreaHelper* pArea); // ѡ�и�����
    void MoveToArea(RtsAreaHelper* pArea); // ��Ļ�ƶ��������λ��

    RtsAreaHelper* AreaInPoint(int iMouseX, int iMouseY, int iOrder);

private:
    void DrawArea(long lMinX, long lMinY, long lMaxX, long lMaxY, DWORD dwColor);

public:
    BOOL            m_bRender;

    BOOL            m_bDrawGridBox;         // Grid �� Box
    BOOL            m_bDrawWireframe;       // ���߿�ģʽ
    BOOL            m_bDrawNormal;          // �Ƿ񻭳�������
    BOOL            m_bDrawAttrWalk;        // �Ƿ񻭳������赲
    BOOL            m_bDrawAttrFly;         // �Ƿ񻭳������赲
    BOOL            m_bDrawAttrDig;         // �Ƿ񻭳�����赲
    BOOL            m_bDrawMinimap;         // �Ƿ�С��ͼ

    BOOL            m_bAreaTool;            // �Ƿ������򹤾�
	BOOL            m_bPathPointTool;       // tim.yang   �Ƿ���·�㹤��
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
