
class CBrusherMultObject : public CBrusher
{
public:
    CBrusherMultObject(CBrushTool* pBrushTool);
    virtual ~CBrusherMultObject();

    virtual BOOL OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY);
    virtual BOOL OnBrushMove(int iKey, int iMouseX, int iMouseY);
    virtual void OnBrushTouch(int iKey, int iLevel, float fDist, int iTileX, int iTileY);
    virtual void OnEndBrush(int iKey, int iMouseX, int iMouseY);
    virtual void OnActive();
    virtual void OnRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY)
    {
        DrawLineBrush(pDevice, vScene, vPos, iGX, iGY);
    }

protected:

};

/*----------------------------------------------------------------------------
    The End.
----------------------------------------------------------------------------*/
