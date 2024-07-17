
/*----------------------------------------------------------------------------
-   Class CBrusherVertex.
----------------------------------------------------------------------------*/

class CBrusherVertex : public CBrusher
{
public:
    CBrusherVertex(CBrushTool* pBrushTool);
    virtual ~CBrusherVertex();

    virtual BOOL OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY);
    virtual BOOL OnBrushMove(int iKey, int iMouseX, int iMouseY);
    virtual void OnEndBrush(int iKey, int iMouseX, int iMouseY);
    virtual void OnActive();
    virtual void SetBrushData(long lParam1, long lParam2, long lParam3);
    virtual void OnRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY)
    {
        DrawLineBrush(pDevice, vScene, vPos, iGX, iGY);
    }
    virtual void OnDrawText(RtgDevice* pDevice);
    virtual bool IsEditTerrain();

    BOOL OnBrushVertexHeight(int iKey, int iMouseX, int iMouseY, int iTileX, int iTileY);
    BOOL OnBrushVertexDrag(int iKey, int iMouseX, int iMouseY, int iTileX, int iTileY);
    BOOL OnBrushVertexNoise(int iKey, int iMouseX, int iMouseY, int iTileX, int iTileY);
    BOOL OnBrushVertexSmooth(int iKey, int iMouseX, int iMouseY, int iTileX, int iTileY);

public:
    void ReComputeNormal();

    BYTE GetHeight(int iPosX, int iPosY);
    void SetHeight(int iPosX, int iPosY, BYTE cHeight);

protected:
    int     m_iBrushType;
    int     m_bBrushBegin;
    bool    m_bEditTile;
    bool    m_bEditTerrain;
    float   m_fBrushHeight;
    int     m_iMinX, m_iMinY, m_iMaxX, m_iMaxY;
    int     m_iDragX, m_iDragY, m_iDragMouseY, m_iLastMouseY;
    BYTE    m_cLastHeight;

};

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
