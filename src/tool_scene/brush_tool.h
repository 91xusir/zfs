
class CDrawerBrush
{
public:
    enum EBrushType
    {
        DB_RECTANGLE,
        DB_ROUND,
        DB_BOX,
    };
    enum {DB_MAX_RECTANGLE_QUALITY = 19*4};
    enum {DB_MAX_ROUND_QUALITY = 20};
    enum {DB_MAX_SIZE = 10};
public:
    CDrawerBrush();
    ~CDrawerBrush();

    void SetType(EBrushType eType, bool bDoubleDeck=FALSE);
    void SetDoubleDeck(bool bDoubleDeck);
    void SetShow(bool bShow)                                    { m_bShow = bShow; }
    void SetSize(int iMax, int iMin);
    void SetSizeType(int iType);
    void SetSize1(int iSize);
    void SetSize2(int iMax, int iMin);
    void Draw(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY);

public:
    EBrushType          m_eType;        // ���Ļ���Բ��
    bool                m_bShow;
    bool                m_bDoubleDeck;  // �Ƿ���˫���
    int                 m_iSizeMax, m_iSizeMin;
    RtgVertex2          m_VertexBuffer[DB_MAX_ROUND_QUALITY*2];
    RtgVertexVC         m_LineBuffer[2][DB_MAX_RECTANGLE_QUALITY*2];
    CBrushTool*         m_pBrushTool;

    float               m_fMask[DB_MAX_SIZE*2+1][DB_MAX_SIZE*2+1];  // Vertex Mask, ���ˢ�Ӱ����Ķ����Mask
    float               m_fBuffer[DB_MAX_SIZE*2+1][DB_MAX_SIZE*2+1];

private:
    int                 m_iSizeType;
    int                 m_iSize1;
    int                 m_iSize2Max, m_iSize2Min;
};

class CBrushTool;

class CBrusher
{
public:
    enum EAdsorbType
    {
        AT_NONE  = 0,   // û����������
        AT_VERTEX   ,   // ��������
        AT_TILE     ,   // ������ʾ����
        AT_TERRAIN  ,   // ������������
        AT_ADSORB_MAX
    };

public:
    CBrusher(CBrushTool* pBrushTool)    {m_pBrushTool=pBrushTool;m_eAdsorbType=AT_NONE;}
    virtual ~CBrusher()                 {}
    virtual BOOL OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY);
    virtual BOOL OnBrushMove(int iKey, int iMouseX, int iMouseY){ return TRUE;}
    virtual void OnEndBrush(int iKey, int iMouseX, int iMouseY){}
    virtual void SetBrushData(long lParam1, long lParam2, long lParam3){}

    virtual void OnActive(){}
    virtual void OnExit(){}
    virtual void OnCommand(int iCmdID, long lParam1, long lParam2) {}

    virtual void OnFrameMove(float fDiffTime) {}
    virtual void OnBeginRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY){}
    virtual void OnRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY){}
    virtual void OnEndRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY){}

    virtual void OnDrawText(RtgDevice* pDevice) {}
    virtual bool IsEditTerrain()    { return false; }

    void DrawLineBrush(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY);

	//jiang add
	//std::string  AlterString(CString p);

public:
    CBrushTool* m_pBrushTool;
    EAdsorbType m_eAdsorbType;
	//jiang add
	RtsObject*      m_pBrushObject;
};

class CBrushTool
{
public:
    enum EBrushType
    {
        BT_NONE = 0,
	    BT_VERTEX,      // ����
        BT_POING,       // ���� (ֻ�༭һ����)
        BT_TILE_VERTEX, // ���� (һ������Ķ���)
	    BT_TILE,        // �ر���ͼ
	    BT_COLOR,       // ������ɫ
	    BT_OBJECT,      // �������
        BT_WATER,       // ˮ��
        BT_ATTR_VERTEX, // ���߶���
        BT_ATTR_WALK,
        BT_ATTR_FLY,
        BT_ATTR_DIG,
        BT_OBJECT_MOVE,   // ��������ƶ�
        BT_OBJECT_ADJUST, // �������΢��
        BT_GRASS,
        BT_BRUSH_MAX
    };
public:
    CBrushTool();
    virtual ~CBrushTool();

    void SetBrushType(EBrushType eType);
    EBrushType GetBrushType()               { return m_eBrushType;}

    void SetBrushData(long lParam1, long lParam2=0, long lParam3=0);
    void Command(int iCmdID, long lParam1=0, long lParam2=0);

    void OnFrameMove(float fDiffTime);

    void OnBeginRender(RtgDevice* pDevice, RtSceneTool& vScene);
    void OnRender(RtgDevice* pDevice, RtSceneTool& vScene);
    void OnEndRender(RtgDevice* pDevice, RtSceneTool& vScene);

    BOOL OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY); // 1 left, 2 right, 4 middle
    void OnBrushMove(int iKey, int iMouseX, int iMouseY);
    void OnEndBrush(int iKey, int iMouseX, int iMouseY);

    void DrawBrushString(RtgDevice* pDevice) {if (m_pBrusher) m_pBrusher->OnDrawText(pDevice);}

    int CallbackTouch(int (__cdecl *cbTouch)(int iLevel, int x, int y, float fDist, long p1, long p2), long p1=0, long p2=0);
    int CallbackSmallTouch(int (__cdecl *cbTouch)(int x, int y, int iIdx, float fDist, long p1, long p2), long p1=0, long p2=0);

    void GetTerrainPos(int iMouseX, int iMouseY, int& iOutX, int& iOutY, bool bViewTerrain=true, bool bGetVertex=false);

    void OriginallyObjectSize();

public:
    CDrawerBrush    m_Drawer;
    CBrusher*       m_pBrusher;
    RtgVertex3      m_vLastCursorPos;
    BOOL            m_bHitTest;
    int             m_iLastSelX;
    int             m_iLastSelY;
    int             m_iLastTileX;
    int             m_iLastTileY;

    int             m_iLastMoveTerrainX;
    int             m_iLastMoveTerrainY;

protected:
    EBrushType      m_eBrushType;

public:
    CBrusher*       m_pBrusherList[BT_BRUSH_MAX];


};

/*----------------------------------------------------------------------------
    The End.
----------------------------------------------------------------------------*/
