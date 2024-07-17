
#ifndef _CBrusherObject_H_
#define _CBrusherObject_H_


/*----------------------------------------------------------------------------
-   CBrusherObject.
----------------------------------------------------------------------------*/

class CBrusherObject : public CBrusher
{
public:
    enum EObjOpType{ OOT_APPEND, OOT_MOVE, OOT_ADJUST};
    CBrusherObject(CBrushTool* pBrushTool, EObjOpType eOp);
    virtual ~CBrusherObject();

    virtual void OnActive();
    virtual void OnExit();
    virtual void OnCommand(int iCmdID, long lParam1, long lParam2);

    virtual BOOL OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY);
    virtual BOOL OnBrushMove(int iKey, int iMouseX, int iMouseY);
    virtual void OnEndBrush(int iKey, int iMouseX, int iMouseY);
    virtual void SetBrushData(long lParam1, long lParam2, long lParam3);
    virtual void OnBeginRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY);
    virtual void OnFrameMove(float fDiffTime);
    virtual void OnRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY);
    virtual void OnEndRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY);
    virtual void OnDrawText(RtgDevice* pDevice);

public:
    RtsObject* GetBrushObject()     { return m_pBrushObject;}
    bool CreateObject(long lType, const char* szFile);
    bool DeleteObject();

    BOOL AddObjectToScene(int iMouseX, int iMouseY);
    BOOL RemoveObjectFromScene(RtsObject* pObject);

    void OriginallyObjectSize();

    bool ReComputeLight(RtgVertex3& vPos, float fRadius);
    bool ReComputeObjectLight(RtsModel* pObject);

public:
    EObjOpType      m_eOpType;
    RtsObject*      m_pBrushObject;
    RtsObject*      m_pLastSelectObject;
    RtRuntimeClass* m_pRuntimeClass;
    long            m_lObjectType;
    RtString        m_strObjectFile;
    bool            m_bActive;
    bool            m_bFirstMove;
    bool            m_bLightObject; // 是否是灯光物件
    RtgMatrix16     m_OffsetMatrix; // 物件的偏移

    float           m_fObjectScale;
    RtgMatrix16     m_OffsetObject;
    RtgMatrix16     m_OffsetLight;
    RtgMatrix16     m_OffsetAudio;

private:
    bool            m_bInPopupMenu;
};

#endif

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
