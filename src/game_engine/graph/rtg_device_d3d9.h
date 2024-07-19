
#ifdef USE_D3D9

class RtgEffectManager;

namespace rt_graph {

class RtgDeviceD3D9 : public RtgDevice, public CD3DApplication
{
public:
    RtgDeviceD3D9();
    virtual ~RtgDeviceD3D9();

protected:
    virtual bool OnPreCreate3DDevice();
    virtual bool OnCreate3DDevice();
    virtual bool OnAfterCreate3DDevice();
    virtual void OnGetDeviceCaps();
    virtual bool OnInit();
    virtual void OnClose();
    virtual int  OnRun();
    virtual void RenderScene();
    virtual bool FullscreenToggle();
    virtual void OnWindowSizeChanged();
    virtual bool SetFullScreenInfo(DWORD dwWidth, DWORD dwHeight, DWORD dwDepth);
    virtual bool ResetDevice(BOOL bFullscreen, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth);
	virtual void UpdateAppTime();
    virtual float GetElapsedTime();
    virtual float GetAppTime();
    virtual ID3DXEffect* GetEffect(RtgEffectType _Type);

    //
    // Device Buffer
    //
    virtual RtgBufferId CreateVB(UINT _Size)
    {
        return m_BufferAllocator.CreateVertexBuffer(_Size);
    }

    virtual void ReleaseVB(RtgBufferId _Id)
    {
        m_BufferAllocator.ReleaseVertexBuffer((IDirect3DVertexBuffer9*)_Id);
    }

    virtual void* LockVB(RtgBufferId _Id)
    {
        if (_Id == ErrorBufferId)
            return NULL;
        IDirect3DVertexBuffer9* vb = (IDirect3DVertexBuffer9*)_Id;
        void* v = NULL;
        vb->Lock(0, 0, &v, 0);
        return v;
    }

    virtual void UnlockVB(RtgBufferId _Id)
    {
        if (_Id == ErrorBufferId)
            return;
        IDirect3DVertexBuffer9* vb = (IDirect3DVertexBuffer9*)_Id;
        vb->Unlock();
    }

    virtual RtgBufferId CreateIB(UINT _Size)
    {
        return m_BufferAllocator.CreateIndexBuffer(_Size);
    }

    virtual void  ReleaseIB(RtgBufferId _Id)
    {
        m_BufferAllocator.ReleaseIndexBuffer((IDirect3DIndexBuffer9*)_Id);
    }

    virtual void* LockIB(RtgBufferId _Id) 
    {
        if (_Id == ErrorBufferId)
            return NULL;
        IDirect3DIndexBuffer9* ib = (IDirect3DIndexBuffer9*)_Id;
        void* i = NULL;
        ib->Lock(0, 0, &i, 0);
        return i;
    }

    virtual void UnlockIB(RtgBufferId _Id) 
    {
        if (_Id == ErrorBufferId)
            return;
        IDirect3DIndexBuffer9* ib = (IDirect3DIndexBuffer9*)_Id;
        ib->Unlock();
    }

public:
	virtual bool  StoreDefaultZBuffer( )  ;
	virtual bool  ResetDefaultZBuffer( )  ;
public:
/*----------------------------------------------------------------------
    Message Proc
----------------------------------------------------------------------*/
    virtual long DeviceMsgProc( RTHWND hWnd, UINT uMsg, UINT wParam, UINT lParam );

/*----------------------------------------------------------------------
    Render
----------------------------------------------------------------------*/
    virtual BOOL BeginScene();
    virtual void EndScene();

    virtual void EndPostProcess(){}

    virtual BOOL SetRenderState(RTGRenderState eState, DWORD dwValue);
    virtual DWORD GetRenderState(RTGRenderState eState);

    virtual BOOL GetBackBufferRenderTarget(_TexItem* pRenderTarget, _TexItem* pZStencil);
	        BOOL SetBackBufferRenderTarget();
    virtual BOOL SetRenderTarget(_TexItem* pRenderTarget, _TexItem* pZStencil);
    virtual BOOL ResetDefaultRenderTarget();
    virtual BOOL ClearRenderTarget(bool bStencil, bool bTarget, bool bZBuffer, DWORD dwColor, float fNewZ, DWORD dwStencil);
    virtual void LockFps(DWORD dwMillisecondPerFrame);

/*----------------------------------------------------------------------
    Light
----------------------------------------------------------------------*/
    virtual BOOL SetLight(int iIdx, RtgLight* pLight);

/*----------------------------------------------------------------------
    Flexible Vertex Format
----------------------------------------------------------------------*/
    virtual BOOL OnSetFVF();

/*----------------------------------------------------------------------
    Martex
----------------------------------------------------------------------*/
    virtual void SetMatrix(RTGMatrix eMode, RtgMatrix16 *pM16);
    virtual void GetMatrix(RTGMatrix eMode, RtgMatrix16 *pM16);

/*----------------------------------------------------------------------
    Other
----------------------------------------------------------------------*/

    virtual BOOL DrawPrimitive(RTGPrimitive eType, void *pVertices, UINT dwVerticesCount, UINT dwPrimitiveCount=-1);
    virtual BOOL DrawPrimitiveVB(RTGPrimitive eType, void *pVertices, UINT dwVertexStart, UINT dwVerticesCount, UINT dwPrimitiveCount);
    virtual BOOL DrawPrimitiveVB(RTGPrimitive eType, UINT dwVertexStart, UINT dwVerticesCount, UINT dwPrimitiveCount); // for vb mgr
    virtual BOOL DrawIndexPrimitive(RTGPrimitive eType, void *pVertices, UINT dwVerticesCount, void *pIndices, UINT dwIndicesCount, UINT dwPrimitiveCount=-1);
    virtual BOOL DrawIndexPrimitiveVB(RTGPrimitive eType, void *pVertices, UINT dwVerticesCount, void *pIndices, UINT dwIndicesCount, UINT dwPrimitiveCount);
    virtual BOOL DrawIndexPrimitiveVB(RTGPrimitive eType, void *pVertices, UINT dwVertexStart, UINT dwVerticesCount, void *pIndices, UINT dwIndexStart, UINT dwIndicesCount, UINT dwPrimitiveCount);
    virtual BOOL DrawIndexPrimitiveVB(RTGPrimitive eType, UINT dwVertexStart, UINT dwVerticesCount, UINT dwIndexStart, UINT dwPrimitiveCount); // for vb & ib mgr

    virtual BOOL SetStreamSource(UINT dwStreamNumber, void* pStreamData, UINT dwStride);
    virtual BOOL SetIndices(void* pIndexData);

    virtual BOOL CreateVertexBuffer(UINT dwLength, void** ppVertexBuffer, DWORD dwUsage);
    virtual BOOL LockVertexBuffer(void* pVertexBuffer, void** ppData, DWORD dwOffset, DWORD dwSize, RTGLock eLockFlag);
    virtual BOOL UnLockVertexBuffer(void* pVertexBuffer);
    virtual BOOL ReleaseVertexBuffer(void* pVertexBuffer);

    virtual BOOL CreateIndexBuffer(UINT dwLength, void** ppIndexBuffer, DWORD dwUsage);
    virtual BOOL LockIndexBuffer(void* pIndexBuffer, void** ppData, DWORD dwOffset, DWORD dwSize, RTGLock eLockFlag);
    virtual BOOL UnLockIndexBuffer(void* pIndexBuffer);
    virtual BOOL ReleaseIndexBuffer(void* pIndexBuffer);

    virtual bool SaveTexture(void* p, const char* fileName);
    virtual BOOL SetMaterial(RtgVertex4& vDiffuse, RtgVertex4& vAmbient, RtgVertex4& vSpecular, RtgVertex4& vEmissive, float fPower);
    virtual BOOL SelectTextureFormat(int iBits);
    virtual void SetTextureFilterMip(RTGTextureFilter eType);
    virtual void SetTextureFilterMin(RTGTextureFilter eType);
    virtual void SetTextureFilterMag(RTGTextureFilter eType);
    virtual void SetTextureAddress(RTGTextureAddress eType);
    virtual void SetTextureBorderColor(DWORD dwColor);
    virtual void SetShader(RtgShader* pShader);
    virtual void RestoreShader(RtgShader* pShader);
    virtual void DeleteShader(RtgShader* pShader);
    virtual BOOL CreateTexture(_TexItem* pItem, BOOL bDxManaged);
    virtual BOOL CreateTextureDynamic(_TexItem* pItem);
    virtual BOOL CreateTextureRenderTarget(_TexItem* pItem);
    virtual BOOL CreateTextureDepthStencil(_TexItem* pItem);
    virtual BOOL CreateTextureByFile(_TexItem* pItem, const char* szTexFileName, int iMipMap);
    virtual BOOL CreateTextureByImage(_TexItem* pItem, RtgImage& img);
    virtual BOOL CreateTextureByMemoryCpy(_TexItem* pItem);
    virtual BOOL DeleteTexture(_TexItem* pItem);
    virtual BOOL UseTexture(int iTextureStage, _TexItem* pItem);
    virtual BOOL UpdateTexture(_TexItem* pItem, void *pBuffer);
    virtual BOOL UpdateTextureDynamic(_TexItem* pItem, void *pBuffer);
    virtual void SetTextureMatrix(int iIdx, RtgMatrix16 *pM16);
    virtual void GetTextureMatrix(int iIdx, RtgMatrix16 *pM16);
    virtual BOOL TextureLock(_TexItem* pItem, RtgTexture::TexLock& texLock);
    virtual void TextureUnlock(_TexItem* pItem, RtgTexture::TexLock& texLock);
	virtual void ResetTextMat();

    void DrawString(float fX, float fY, DWORD dwColor, const char* pString, DWORD dwFlags = 0L);

    virtual int GetAvailableTextureMem();
    virtual BOOL SaveRenderTargetToFile(const char* szFileName,bool bJPG = true);

    virtual void SetFogEnable(BOOL bEnable);
    virtual void SetFogParameter(DWORD dwColor, float fNear, float fFar);
    virtual void SetFogColor(DWORD dwColor);
    virtual void SetFogNear(float fNear);
    virtual void SetFogFar(float fFar);

    virtual bool CreateUIStateBlock(DWORD* dwToken);
    virtual bool BeginRecordStateBlock();
    virtual bool EndRecordStateBlock(DWORD* dwToken);
    virtual bool DeleteStateBlock(DWORD dwToken);
    virtual bool ApplyStateBlock(DWORD dwToken);
    virtual bool CaptureStateBlock(DWORD dwToken);

    virtual BOOL IsWindowed();
    virtual BOOL HasFocus();
    virtual BOOL IsActive();

    virtual DWORD GetDisplayMode(RtgDisplayMode* pModeBuf, DWORD dwBufSize);
    virtual bool GetCanDoWindowed();
    virtual const char* GetDeviceDesc();
    virtual void SetTextureCoord(int texId, int coordId);

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice(D3DCAPS9*,DWORD,D3DFORMAT,D3DFORMAT);
    virtual HRESULT AdjustWindowForChange();
    virtual bool OnD3D9ErrorMessage(const char* szMsg, int iType);

protected:
    void UseTextureStage(int iLevel, RtgTexStage* pStage);

protected:
    RtgEffectManager* m_pEffectManager;

    struct RtgRenderStateList
    {
        BOOL            bChanged;
        DWORD           dwValue;
    };
    CD3DFont*           m_pFont; // Font for drawing text
    RtgRenderStateList  m_listRenderState[RTGRS_MAX];
    HWND                m_hRenderWnd;
    HWND                m_hRenderFullWnd;
    bool                m_bCustomToggleFullscreen;
	bool                m_bCurFrameUsePostProcessEffect;

    IDirect3DSurface9*  m_pDefaultRenderTarget;
    DWORD               m_texPostProcessID;
    DWORD               m_texPostProcessRenderTarget;
    //IDirect3DSurface9*  m_pDefaultDepthStencilSurface;


	IDirect3DSurface9*  m_pDefaultDepthStencilSurface;
	IDirect3DSurface9*  m_pRenderTargetDepthStencilSurface;
	int                 m_iCurrentZBufferState;

};

} // namespace rt_graph

#endif

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
