
class CEditorFrame : public CRtgAppFrame
{
public:
	RT_DECLARE_DYNCREATE(CEditorFrame, CRtgAppFrame, NULL, "")
    CEditorFrame();
    virtual ~CEditorFrame();

    virtual bool OnFrameInit();
    virtual void OnFrameClose();
    virtual void OnRender();
    virtual void OnFrameMove(float fDifTime);
    virtual bool OnErrorMessage(const char* szMsg, int iType); // iType 0 错误, 1 必须退出的错误

    virtual void OnMouseLDown(int iButton, int x, int y);
    virtual void OnMouseLDrag(int iButton, int x, int y, int increaseX, int increaseY);
    virtual void OnMouseLUp(int iButton, int x, int y);

    virtual void OnMouseRDown(int iButton, int x, int y);
    virtual void OnMouseRDrag(int iButton, int x, int y, int increaseX, int increaseY);
    virtual void OnMouseRUp(int iButton, int x, int y);

    virtual void OnMouseMDown(int iButton, int x, int y);
    virtual void OnMouseMDrag(int iButton, int x, int y, int increaseX, int increaseY);
    virtual void OnMouseMUp(int iButton, int x, int y);
	virtual void OnMouseMove(int iButton, int x, int y, int increaseX, int increaseY);
	virtual void OnMouseWheel(int iButton, long vDelta, int x, int y);

	virtual void OnKeyDown(int iChar, bool bAltDown);
	// virtual void OnKeyUp(int iChar, bool bAltDown);
	// virtual void OnKeyChar(int iChar, bool bAltDown);
	void OnSelectFrame(int frame);
	void SetDirectLight(float r,float g,float b);
	void ChangeFillMode();
	
protected:
    void OnBrushMove(int iButton, int x, int y);

public:
 	RtgGeometry *m_boundingBox;
    // RtgShader   m_ShaderFloor;
	RtgShader   m_shaderBox;
    RtgShader   m_ShaderNull;
	RtgVertex3  m_actorScale;

    RtgLight    m_Light;
    int         m_iCameraMode;
    int         m_iFillTerrain;
    RtgVertex2  m_iCameraPos;

    // Option
	bool        m_bShowBoundBox;
	RtgVertex3  m_clearColor;
    // DWORD       m_dwEnvColor;
    // BOOL        m_bWireframe;
    // BOOL        m_bDrawNormal;
    bool        m_bFog;
    RtgVertex3  m_fogColor;
	RtgVertex3  m_dirLight;
    long        m_fogNear, m_fogFar;
	RtgVertex3  m_emi;

	int         m_curFrame;
    void*       m_pView;			// Ctool_characterView的指针，要强制转换
	CRT_ActorInstance *m_floor;
	bool        m_bCameraLight;
};

extern bool g_bRenderShadow;
extern bool g_bRenderFrame;
extern bool g_bRenderCoor;
extern bool g_bRenderFloor;
extern bool g_bRenderInfo;
extern bool g_bRenderWire;
