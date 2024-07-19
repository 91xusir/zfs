namespace rt_graph {

/*----------------------------------------------------------------------------
-   class D3DVertexStreamManager.
----------------------------------------------------------------------------*/

class GRAPH_API D3DVertexStreamManager : public RtgRenderStreamManager
{
public:

    D3DVertexStreamManager(RtgDevice* pDevice) : m_pVertexBuffer(NULL),RtgRenderStreamManager(pDevice) {}
    ~D3DVertexStreamManager();

    virtual BOOL Bind(DWORD dwChannel, DWORD dwResID);

    virtual BOOL OnCreateStreamManager();    
    virtual BOOL OnReleaseStreamManager();
    virtual BOOL OnResizeStream(DWORD dwResID, DWORD dwSize);

protected:

    virtual void* LockStream(DWORD dwBufferID, DWORD dwOffset, DWORD dwSize, DWORD bDiscard);
    virtual BOOL  UnlockStream(DWORD dwBufferID);

protected:

    void**  m_pVertexBuffer;    
};

/*----------------------------------------------------------------------------
-   class D3DIndexStreamManager.
----------------------------------------------------------------------------*/

class GRAPH_API D3DIndexStreamManager : public RtgRenderStreamManager
{
public:

    D3DIndexStreamManager(RtgDevice* pDevice) : m_pIndexBuffer(NULL),RtgRenderStreamManager(pDevice) {}
    ~D3DIndexStreamManager();

    virtual BOOL Bind(DWORD dwVBResID, DWORD dwIBResID);

    virtual BOOL OnCreateStreamManager();
    virtual BOOL OnReleaseStreamManager();
    virtual BOOL OnResizeStream(DWORD dwResID, DWORD dwSize);

protected:

    virtual void* LockStream(DWORD dwBufferID, DWORD dwOffset, DWORD dwSize, DWORD bDiscard);
    virtual BOOL  UnlockStream(DWORD dwBufferID);

protected:

    void**      m_pIndexBuffer;
};

} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
