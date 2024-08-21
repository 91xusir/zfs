


#ifndef _RtgDeviceD3D9_H_
#define _RtgDeviceD3D9_H_

#include "graph\d3dapp.h"
#include "graph\d3dfont.h"

namespace rt_graph {

    class RtgEffectManager;
    class RtgTextureManager;
    class RtgRenderTargetManager;

    enum 
    {
        Max_Stream_Num = 8,
        Bloom_Kernel_Size = 13
    };

    typedef struct _Stream_vb_Desc
    {
        UINT _Offset;
        UINT _vNum;
        UINT _Stride;
        DWORD _FVF;
    } Stream_vb_Desc;

    typedef struct _Stream_ib_Desc
    {
        UINT _Offset;
    } Stream_ib_Desc;

    class RtgDeviceD3D9 : public RtgDevice, public CD3DApplication
    {
    public:
        RtgDeviceD3D9();
        virtual ~RtgDeviceD3D9();

    public :
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

        virtual unsigned long GetElapsedMS() const
        {
            return m_dwElapsedMS;
        }

        virtual float GetElapsedTime()
        {
            return m_fElapsedTime;
        }

        virtual float GetAppTime()
        {
            return m_fTime;
        }
        virtual ID3DXEffect* GetEffect(RtgEffectType _Type);

    public:
	    //virtual bool  StoreDefaultZBuffer( )  ;
	    //virtual bool  ResetDefaultZBuffer( )  ;
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
	            //BOOL SetBackBufferRenderTarget();
        virtual BOOL SetRenderTarget(RtgTextItem* frameTarget, RtgTextItem* depthTarget = NULL);
        virtual BOOL RestoreRenderTarget();
        //virtual BOOL ResetDefaultRenderTarget();
        virtual BOOL ClearRenderTarget(bool bStencil=false, bool bTarget=true, bool bZBuffer=true, DWORD dwColor=0x00000000, float fNewZ=1.f, DWORD dwStencil=0);
        virtual void LockFps(DWORD dwMillisecondPerFrame);

    /*----------------------------------------------------------------------
        Light
    ----------------------------------------------------------------------*/
        virtual BOOL SetLight(int iIdx, RtgLight* pLight);

    /*----------------------------------------------------------------------
        Flexible Vertex Format
    ----------------------------------------------------------------------*/
        virtual BOOL OnSetFVF();

        virtual DWORD GetCurrentVertexStride() const { return m_dwVertexStride; }
        virtual D3DPRIMITIVETYPE GetD3DPT(RTGPrimitive pt) const;

    /*----------------------------------------------------------------------
        Martex
    ----------------------------------------------------------------------*/
        virtual void SetMatrix(RTGMatrix eMode, RtgMatrix16 *pM16);
        virtual void GetMatrix(RTGMatrix eMode, RtgMatrix16 *pM16);

        //
        // native function
        //

        virtual BOOL SetStream_vb(
            UINT _StreamIndex,
            IDirect3DVertexBuffer9* _vb,
            UINT _Stride,
            UINT _Offset = 0,
            UINT _vNum = 0,
            DWORD _FVF = 0)
        {
            RTASSERT(_StreamIndex < Max_Stream_Num);
            
            m_vbDesc[_StreamIndex]._Stride = _Stride;
            m_vbDesc[_StreamIndex]._Offset = _Offset;

            if (_vNum)
                m_vbDesc[_StreamIndex]._vNum = _vNum;
            if (_FVF)
            {
                m_vbDesc[_StreamIndex]._FVF = _FVF;
                m_pd3dDevice->SetFVF(_FVF);
            }

            if (FAILED(m_pd3dDevice->SetStreamSource(
                _StreamIndex,
                _vb,
                _Offset,
                _Stride)))
                return FALSE;
            return TRUE;
        }

        virtual BOOL SetStream_ib(
            IDirect3DIndexBuffer9* ib,
            UINT _Offset = 0)
        {
            m_ibDesc._Offset = _Offset;
            if (FAILED(m_pd3dDevice->SetIndices(ib)))
                return FALSE;
            return TRUE;
        }

        virtual BOOL SetFVF(DWORD _FVF)
        {
            m_vbDesc[0]._FVF = _FVF;
            return SUCCEEDED(m_pd3dDevice->SetFVF(_FVF));
        }

        virtual BOOL DrawIndexedPrimitive(
            D3DPRIMITIVETYPE Type,
            INT  BaseVertexIndex,
            UINT MinIndex,
            UINT NumVertices,
            UINT StartIndex,
            UINT PrimitiveCount
            )
        {
            RtGetPref()->Proc_FaceNum += PrimitiveCount;
            ++RtGetPref()->Draw_Call;
            if (FAILED(m_pd3dDevice->DrawIndexedPrimitive(
                Type,
                BaseVertexIndex,
                MinIndex,
                NumVertices,
                StartIndex,
                PrimitiveCount))
                )
                return FALSE;
            return TRUE;
        }

        virtual BOOL DrawPrimitive(
            D3DPRIMITIVETYPE PrimitiveType,
            UINT StartVertex,
            UINT PrimitiveCount
            )
        {
            RtGetPref()->Proc_FaceNum += PrimitiveCount;
            ++RtGetPref()->Draw_Call;
            if (FAILED(m_pd3dDevice->DrawPrimitive(
                PrimitiveType,
                StartVertex,
                PrimitiveCount))
                )
                return FALSE;
            return TRUE;
        }

        virtual BOOL DrawIndexedPrimitiveUP(
            D3DPRIMITIVETYPE Type,
            UINT MinVertexIndex,
            UINT NumVertices,
            void* _Indices,
            void* _Vertex,
            UINT _vStride,
            UINT _PrimitiveCount
            )
        {
            RtGetPref()->Proc_FaceNum += _PrimitiveCount;
            ++RtGetPref()->Draw_Call;
            if (FAILED(m_pd3dDevice->DrawIndexedPrimitiveUP(
                Type,
                MinVertexIndex,
                NumVertices,
                _PrimitiveCount,
                _Indices,
                D3DFMT_INDEX16,
                _Vertex,
                _vStride)))
                return FALSE;
            return TRUE;
        }

        virtual BOOL DrawPrimitiveUP(
            D3DPRIMITIVETYPE Type,
            void* _Vertex,
            UINT _vStride,
            UINT _PrimitiveCount
            )
        {
            RtGetPref()->Proc_FaceNum += _PrimitiveCount;
            ++RtGetPref()->Draw_Call;
            if (FAILED(m_pd3dDevice->DrawPrimitiveUP(
                Type,
                _PrimitiveCount,
                _Vertex,
                _vStride))
                )
                return FALSE;
            return TRUE;
        }


        // ext function

        virtual BOOL SetStream_vb(
            UINT _StreamIndex,
            RtgBufferItem* _vbItem,
            UINT _Stride,
            UINT _Offset,
            UINT _vNum,
            DWORD _FVF = 0)
        {
            return SetStream_vb(
                _StreamIndex,
                (IDirect3DVertexBuffer9*)_vbItem->pBuffer,
                _Stride,
                _Offset,
                _vNum,
                _FVF
                );
        }

        virtual BOOL SetStream_vb(
            UINT _StreamIndex,
            RtgBufferItem* _vbItem,
            UINT _Stride,
            DWORD _FVF = 0
            )
        {
            return SetStream_vb(
                _StreamIndex,
                (IDirect3DVertexBuffer9*)_vbItem->pBuffer,
                _Stride,
                0,
                _vbItem->dwSize / _Stride,
                _FVF
                );
        }

        virtual BOOL SetStream_ib(RtgBufferItem* _ibItem)
        {
            m_ibDesc._Offset = 0;
            if (FAILED(m_pd3dDevice->SetIndices((IDirect3DIndexBuffer9*)_ibItem->pBuffer)))
                return FALSE;
            return TRUE;
        }

        virtual BOOL SetStream_vb(
            UINT _StreamIndex,
            RtgShareBufferItem* _vbItem,
            UINT _Stride,
            DWORD _FVF = 0
            )
        {
            return SetStream_vb(
                _StreamIndex,
                (IDirect3DVertexBuffer9*)_vbItem->pShareBuf->bufItem->pBuffer,
                _Stride,
                _vbItem->Offset,
                _vbItem->Size / _Stride,
                _FVF
                );
        }

        virtual BOOL SetStream_ib(RtgShareBufferItem* _ibItem)
        {
            m_ibDesc._Offset = _ibItem->Offset / sizeof(WORD);
            if (FAILED(m_pd3dDevice->SetIndices((IDirect3DIndexBuffer9*)_ibItem->pShareBuf->bufItem->pBuffer)))
                return FALSE;
            return TRUE;
        }

        virtual BOOL DrawIndexedPrimitive(
            D3DPRIMITIVETYPE Type,
            UINT PrimitiveCount,
            UINT NumVertices = 0,
            UINT _ibOffset = 0
            )
        {
            return DrawIndexedPrimitive(
                Type,
                0,
                0,
                (NumVertices == 0 ? m_vbDesc[0]._vNum : NumVertices),
                m_ibDesc._Offset + _ibOffset,
                PrimitiveCount
                );
        }

        virtual BOOL DrawIndexedPrimitive(
            D3DPRIMITIVETYPE Type, 
            RtgBufferItem* vb, 
            UINT _vbStride, 
            RtgBufferItem* ib,
            UINT _PrimitiveCount
            )
        {
            if (!SetStream_vb(0, vb, _vbStride))
                return FALSE;
            if (!SetStream_ib(ib))
                return FALSE;
            return DrawIndexedPrimitive(Type, _PrimitiveCount);
        }

        virtual BOOL DrawIndexedPrimitive(
            D3DPRIMITIVETYPE Type, 
            RtgShareBufferItem* vb, 
            UINT _vbStride, 
            RtgShareBufferItem* ib,
            UINT PrimitiveCount,
            UINT _ibOffset = 0
            )
        {
            if (!SetStream_vb(0, vb, _vbStride))
                return FALSE;
            if (!SetStream_ib(ib))
                return FALSE;
            return DrawIndexedPrimitive(Type, PrimitiveCount, 0, _ibOffset);
        }

        virtual BOOL DrawPrimitive(
            D3DPRIMITIVETYPE Type, 
            RtgShareBufferItem* vb, 
            UINT _vbStride, 
            UINT PrimitiveCount
            )
        {
            if (!SetStream_vb(0, vb, _vbStride))
                return FALSE;
            return DrawPrimitive(Type, 0, PrimitiveCount);
        }

        virtual void DisableStreamSource(UINT _StreamIndex)
        {
            m_pd3dDevice->SetStreamSource(_StreamIndex, NULL, 0, 0);
        }

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
        virtual BOOL UseTexture(int stage, RtgTextItem* texItem);
        virtual BOOL UpdateTexture(_TexItem* pItem, void *pBuffer);
        virtual BOOL UpdateTextureDynamic(_TexItem* pItem, void *pBuffer);
        virtual void SetTextureMatrix(int iIdx, RtgMatrix16 *pM16);
        virtual void GetTextureMatrix(int iIdx, RtgMatrix16 *pM16);
        virtual BOOL TextureLock(_TexItem* pItem, RtgTexture::TexLock& texLock);
        virtual void TextureUnlock(_TexItem* pItem, RtgTexture::TexLock& texLock);
	    virtual void ResetTextMat();

        virtual void Blur(RtgTextItem* texItem);
        virtual void Gray(RtgTextItem* texItem, bool bFirst);

        void DrawString(float fX, float fY, DWORD dwColor, const char* pString, DWORD dwFlags = 0L);

        virtual int GetAvailableTextureMem();
        virtual BOOL SaveRenderTargetToFile(const char* szFileName,bool bJPG = true);

        virtual void SetFogEnable(BOOL );
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

        virtual DWORD GetDisplayMode(RtgDisplayMode* pModeBuf, DWORD dwBufSize = 0);
        virtual bool GetCanDoWindowed();
        virtual const char* GetDeviceDesc();
        virtual void SetTextureCoord(int texId, int coordId);
		
		virtual RTGTextureFilter GetTextureFilterMip() { return m_eTextureFilterMip; }
        virtual RTGTextureFilter GetTextureFilterMin() { return m_eTextureFilterMin; }
        virtual RTGTextureFilter GetTextureFilterMag() { return m_eTextureFilterMag; }

        virtual bool SetVertexDecl_vnt()
        {
            return SUCCEEDED(m_pd3dDevice->SetVertexDeclaration(m_ActDecl_vnt));
        }

        virtual bool SetVertexDecl_vnct()
        {
            return SUCCEEDED(m_pd3dDevice->SetVertexDeclaration(m_ActDecl_vnct));
        }

        void EnableDrawPref(bool bEnable, float x = 5, float y = 5)
        {
            m_bDrawPerf = bEnable;
            m_fPerfx = x;
            m_fPerfy = y;
        }

        virtual void SetPostProcessEffectOnoff(bool bOn);
        virtual void Gray(bool bOn);

        bool CreatePostProcessResource();
        void DestoryPostProcessResource(bool bForce);

        bool CreateGrayResource();
        void DestoryGrayResource(bool bForce);

		virtual BOOL DrawPrimitiveVB(RTGPrimitive eType, void *pVertices, UINT dwVertexStart, UINT dwVerticesCount, UINT dwPrimitiveCount);
		virtual BOOL DrawPrimitiveVB(RTGPrimitive eType, UINT dwVertexStart, UINT dwVerticesCount, UINT dwPrimitiveCount); // for vb mgr
		
		virtual BOOL SetStreamSource(UINT dwStreamNumber, void* pStreamData, UINT dwStride);
		virtual BOOL SetIndices(void* pIndexData);

		virtual BOOL CreateVertexBuffer(UINT dwLength, void** ppVertexBuffer, DWORD dwUsage=0x08L);
		virtual BOOL LockVertexBuffer(void* pVertexBuffer, void** ppData, DWORD dwOffset=0, DWORD dwSize=0, RTGLock eLockFlag=RTGLK_DISCARD);
		virtual BOOL UnLockVertexBuffer(void* pVertexBuffer);
		virtual BOOL ReleaseVertexBuffer(void* pVertexBuffer);

		virtual BOOL CreateIndexBuffer(UINT dwLength, void** ppIndexBuffer, DWORD dwUsage=0x08L);
		virtual BOOL LockIndexBuffer(void* pIndexBuffer, void** ppData, DWORD dwOffset=0, DWORD dwSize=0, RTGLock eLockFlag=RTGLK_DISCARD);
		virtual BOOL UnLockIndexBuffer(void* pIndexBuffer);
		virtual BOOL ReleaseIndexBuffer(void* pIndexBuffer);

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
        virtual void    DrawPerfInformation();

    protected:
        void UseTextureStage(int iLevel, RtgTexStage* pStage);

    public :
        RtgTextureManager*  GetTextMgr() const { return m_pTextMamager;   }
        RtgBufferAllocator* GetBuffMgr() const { return m_pBufferManager; }
        

    protected:
        RtgEffectManager*       m_pEffectManager;
        RtgTextureManager*      m_pTextMamager;
        RtgRenderTargetManager* m_pRTManager;
        RtgBufferAllocator*     m_pBufferManager;
        bool                    m_bDrawPerf;
        float                   m_fPerfx;
        float                   m_fPerfy;
        int                     m_bloomTexW;
        int                     m_bloomTexH;
        float                   m_bloomTexOffset_h[Bloom_Kernel_Size];
        float                   m_bloomTexOffset_v[Bloom_Kernel_Size];
        DWORD                   m_dwGrayBegin;

        Stream_vb_Desc          m_vbDesc[Max_Stream_Num];
        Stream_ib_Desc          m_ibDesc;

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
        
        RtgTextItem*        m_texPostProcess;
        RtgTextItem*        m_texPostDown0;
        RtgTextItem*        m_texPostDown1;
        
        IDirect3DVertexDeclaration9* m_ActDecl_vnt;
        IDirect3DVertexDeclaration9* m_ActDecl_vnct;


	    IDirect3DSurface9*  m_pDefaultDepthStencilSurface;
	    IDirect3DSurface9*  m_pRenderTargetDepthStencilSurface;
	    int                 m_iCurrentZBufferState;

    };
        
}

#endif

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
