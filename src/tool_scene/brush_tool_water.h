
/*----------------------------------------------------------------------------
    CBrusherWater.
----------------------------------------------------------------------------*/

class CBrusherWater : public CBrusher
{
public:
    CBrusherWater(CBrushTool* pBrushTool);
    virtual ~CBrusherWater();

    virtual void OnCommand(int iCmdID, long lParam1, long lParam2);

    virtual BOOL OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY);
    virtual BOOL OnBrushMove(int iKey, int iMouseX, int iMouseY);
    virtual void OnEndBrush(int iKey, int iMouseX, int iMouseY);
    virtual void SetBrushData(long lParam1, long lParam2, long lParam3);
    virtual void OnActive();
    virtual void OnDrawText(RtgDevice* pDevice);
    virtual void OnRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY);

    void OnFillWater(int iTileX, int iTileY, BYTE cHeight, BYTE cWaterType);
    bool FillLine(int iLine, BYTE cHeight, BYTE cWaterType);
    void ClearWater(int iTileX, int iTileY, BYTE cHeight);
    bool ClearLine(int iLine, BYTE cHeight);

    void UpdateVertexPositionByWaterHeightmap(int iMinX, int iMinY, int iMaxX, int iMaxY, BYTE cHeight);

protected:
    int     m_iMinX, m_iMinY, m_iAbMinX, m_iAbMinY;
    int     m_iMaxX, m_iMaxY, m_iAbMaxX, m_iAbMaxY;

    BYTE    m_cLastSelectWaterType;
    BYTE    m_cWaterHeight;

};

/*----------------------------------------------------------------------------
    The End.
----------------------------------------------------------------------------*/
