
/*----------------------------------------------------------------------------
    CBrusherTile.
----------------------------------------------------------------------------*/

class CBrusherTile : public CBrusher
{
public:
    CBrusherTile(CBrushTool* pBrushTool);
    virtual ~CBrusherTile();

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

protected:
    int m_iFillTerrain;

};

/*----------------------------------------------------------------------------
    CBrusherAttribute.
----------------------------------------------------------------------------*/

class CBrusherAttribute : public CBrusher
{
public:
    CBrusherAttribute(CBrushTool* pBrushTool, BYTE cAttrMask);
    virtual ~CBrusherAttribute();

    virtual BOOL OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY);
    virtual BOOL OnBrushMove(int iKey, int iMouseX, int iMouseY);
    virtual void OnBrushTouch(int iKey, int iLevel, float fDist, int iTileX, int iTileY);
    virtual void OnEndBrush(int iKey, int iMouseX, int iMouseY);
    virtual void OnActive();
    virtual void OnRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY);
    virtual void OnDrawText(RtgDevice* pDevice);
    virtual bool IsEditTerrain();

protected:
    BYTE m_cAttrMask;

};

/*----------------------------------------------------------------------------
    The End.
----------------------------------------------------------------------------*/
