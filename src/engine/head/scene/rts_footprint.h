
class RtsFootPrint
{
    struct SFootPrintNode
    {
        RtgMatrix16     m16;
        float           fTime;
        SFootPrintNode* pNext;
    };
    struct SFootPrintPane
    {
        RtgVertex3 v;
        //DWORD      c;
        RtgVertex2 t;
        SFootPrintPane(){};
        void Set(DWORD dwColor, float tx, float ty)
        {
            //c = dwColor;
            t.x = tx;
            t.y = ty;
        }
    };
public:
    RtsFootPrint();
    ~RtsFootPrint();

    void InitOnce(RtgDevice* pDevice, int iMax);
    void ClearOnce();

    void Add(RtgMatrix16& m16, DWORD dwSize=0, DWORD dwType=0);

    void Run(float fDifSecond);
    void Render(RtgCamera& inCamera, RTGRenderMask eMask);

protected:
    RtgDevice*  m_pDevice;
    float       m_fTime;
    float       m_fLifecycle;   // Éú´æÊ±¼ä

    BOOL        m_bInit;
    int         m_iMax;
    int         m_iCount;

    SFootPrintNode*     m_pHead;
    SFootPrintNode*     m_pUnused;
    SFootPrintNode*     m_pArray;

    RtgShader           m_Shader;
    SFootPrintPane*     m_pPanes;
    RtgVertex3          m_pPaneVertex[6];

};

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
