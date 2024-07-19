
#ifdef USE_D3D8

namespace rt_graph {

class RtgDeviceD3D8 : public RtgDevice, public CD3DApplication
{
public:
    RtgDeviceD3D8();
    virtual ~RtgDeviceD3D8();

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

public:
	/*virtual LPDIRECT3DDEVICE8 GetD3DDevice8();*/

/*----------------------------------------------------------------------
    Message Proc
----------------------------------------------------------------------*/
    virtual long DeviceMsgProc( RTHWND hWnd, UINT uMsg, UINT wParam, UINT lParam );

/*----------------------------------------------------------------------
    Render
----------------------------------------------------------------------*/
    virtual BOOL BeginScene();
    virtual void EndScene();

    virtual void EndPostProcess();

    virtual BOOL SetRenderState(RTGRenderState eState, DWORD dwValue);
    virtual DWORD GetRenderState(RTGRenderState eState);

    virtual BOOL SetRenderTarget(_TexItem* pRenderTarget, _TexItem* pZStencil);
    virtual BOOL ResetDefaultRenderTarget();
    virtual BOOL ClearRenderTarget(bool bStencil, bool bTarget, bool bZBuffer, DWORD dwColor, float fNewZ, DWORD dwStencil);
    virtual void LockFps(DWORD dwMillisecondPerFrame);

	virtual BOOL GetBackBufferRenderTarget(_TexItem* pRenderTarget,_TexItem* pZStencil=0){return false;};

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

    void DrawString(float fX, float fY, DWORD dwColor, const char* pString, DWORD dwFlags);

    virtual int GetAvailableTextureMem();
    virtual BOOL SaveRenderTargetToFile(const char* szFileName, bool bJPG=true); // bJPG=false就存成BMP
    virtual BOOL SaveRenderTargetToArchive(RtArchive* pArc, bool bJPG=true); // bJPG=false就存成BMP

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

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice(D3DCAPS8*,DWORD,D3DFORMAT);
    virtual HRESULT AdjustWindowForChange();
    virtual bool OnD3D8ErrorMessage(const char* szMsg, int iType);

protected:
    void RestoreTextureCaps();
    void UseTextureStage(int iLevel, RtgTexStage* pStage);

protected:
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

    IDirect3DSurface8*  m_pDefaultRenderTarget;
    DWORD               m_texPostProcessID;
    DWORD               m_texPostProcessRenderTarget;
    IDirect3DSurface8*  m_pDefaultDepthStencilSurface;

    RTGVertexFormat     m_eLastVertexFormat;

    //RtgDisplayModeEnum  m_enumDisplayMode;

};

} // namespace rt_graph

#endif

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
