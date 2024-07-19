
/*----------------------------------------------------------------------------
-   Class CBrusherSingleVertex.
----------------------------------------------------------------------------*/

class CBrusherSingleVertex : public CBrusher
{
public:
    CBrusherSingleVertex(CBrushTool* pBrushTool);
    virtual ~CBrusherSingleVertex();

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

public:
    void ReComputeNormal();
    BYTE GetHeight(int iPosX, int iPosY);
    void SetHeight(int iPosX, int iPosY, BYTE cHeight);

protected:
    int     m_bBrushBegin;
    float   m_fBrushHeight;
    int     m_iMinX, m_iMinY, m_iMaxX, m_iMaxY;
    int     m_iDragX, m_iDragY, m_iDragMouseY, m_iLastMouseY;
    BYTE    m_cLastHeight;
    bool    m_bEditTile;
    bool    m_bEditTerrain;

};

/*----------------------------------------------------------------------------
-   Class CBrusherTileVertexVertex.
----------------------------------------------------------------------------*/

class CBrusherTileVertex : public CBrusher
{
public:
    CBrusherTileVertex(CBrushTool* pBrushTool);
    virtual ~CBrusherTileVertex();

    virtual BOOL OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY);
    virtual BOOL OnBrushMove(int iKey, int iMouseX, int iMouseY);
    virtual void OnBrushTouch(int iKey, int iLevel, float fDist, int iTileX, int iTileY);
    virtual void OnEndBrush(int iKey, int iMouseX, int iMouseY);
    virtual void SetBrushData(long lParam1, long lParam2, long lParam3);
    virtual void OnActive();
    virtual void OnRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY)
    {
        DrawLineBrush(pDevice, vScene, vPos, iGX, iGY);
    }
    virtual void OnDrawText(RtgDevice* pDevice);
    void ReComputeNormal();
    BYTE GetHeight(int iPosX, int iPosY);
    void SetHeight(int iPosX, int iPosY, BYTE cHeight);

protected:
    int     m_iFillTerrain;
    int     m_bBrushBegin;
    int     m_iMinX, m_iMinY, m_iMaxX, m_iMaxY;
    int     m_iDragX, m_iDragY, m_iDragMouseY, m_iLastMouseY, m_iOffset;
    bool    m_bEditTile;
    bool    m_bEditTerrain;

};

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
