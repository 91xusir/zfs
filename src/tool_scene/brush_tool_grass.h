
class CBrushGrassForm;

class CBrusherGrass : public CBrusher
{
public:
    CBrusherGrass(CBrushTool* pBrushTool);
    virtual ~CBrusherGrass();

    virtual void OnActive();
    virtual void OnRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY)
    {
        DrawLineBrush(pDevice, vScene, vPos, iGX, iGY);
    }

    void SetBrushData(long lParam1, long lParam2, long lParam3)
    {
        m_pForm = (CBrushGrassForm*)lParam1;
    }

    BOOL OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY)
    {
        //guard;
        if (iMouseBtn==1)   // 只有按下鼠标左键的时候才能开始刷
            return TRUE;
        return FALSE;
        //unguard;
    }

    BOOL OnBrushMove(int iKey, int iMouseX, int iMouseY);
    void OnBrushTouch(int iKey, int iLevel, float fDist, int iTileX, int iTileY);

private :
    CBrushGrassForm* m_pForm;
};