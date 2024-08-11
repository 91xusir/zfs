#ifndef _RtgDevice_H_
#define _RtgDevice_H_

#include <D3DX9.h>
#include "rtg_buffer_allocator.h"
#include "rtg_buffer_manager_util.h"
#include "rtg_buffer_manager.h"

namespace rt_graph {

struct _RtgTextItem;
typedef _RtgTextItem RtgTextItem;

class RtgDevice {
    enum { RenderTarget_Stack_Size = 8 };

   public:
    RtgDevice();
    virtual ~RtgDevice();

   public:
    bool Init(RTHINSTANCE hInst, RtRuntimeClass* pEventClass, RtRuntimeClass* pCameraClass/*,
              const char* szGraphIni, const char* szUserIni)*/);
    bool Init(RTHINSTANCE hInst, RtRuntimeClass* pEventClass, RtRuntimeClass* pCameraClass,
              RtIni* pGraphIni, RtIni* pUserIni);
    void SetCustomWndHandle(HWND hWnd);  // ����û��Լ�Ҫ�������ڣ�����Ҫ�����������
    void         Close();
    virtual int  RunEngine();
    virtual void RenderScene();
    virtual bool OnErrorMessage(const char* szMsg, int iType);  // iType 0 ����, 1 �����˳��Ĵ���
    virtual void OnWindowSizeChanged() = 0;

    virtual ID3DXEffect* GetEffect(RtgEffectType _Type) = 0;
    RtgDeviceEvent*      GetEvent();

    virtual void SetTextureCoord(int texId, int coordId) = 0;
    void         BindViewWindow(RTGViewWindowMode eMode, RTHWND hWnd, bool bResize = true);
    void SetViewWindowInfo(RTGViewWindowMode eMode, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth,
                           DWORD dwRefreshRate);
    bool SetViewMode(RTGViewWindowMode eMode, bool bStype = true);  // ������ʾģʽ

    static void ResumeDefaultDisplayMode();

   public:
    virtual long DeviceMsgProc(RTHWND hWnd, UINT uMsg, UINT wParam, UINT lParam) = 0;

    virtual BOOL BeginScene() = 0;
    virtual void EndScene() = 0;
    virtual void EndPostProcess() = 0;

    virtual BOOL  SetRenderState(RTGRenderState eState, DWORD dwValue) = 0;
    virtual DWORD GetRenderState(RTGRenderState eState) = 0;

    // SetRenderTarget��SetDefaultRenderTarget����ɶ�ʹ�ã�
    //      Ҳ����˵����SetRenderTarget֮��һ��Ҫ����SetDefaultRenderTarget
    // BOOL GetBackBufferRenderTarget2(DWORD dwRenderTarget, DWORD dwZStencil=0);
    virtual BOOL GetBackBufferRenderTarget(_TexItem* pRenderTarget, _TexItem* pZStencil = 0) = 0;
    //BOOL SetRenderTarget2(DWORD dwRenderTarget, DWORD dwZStencil=0);
    virtual BOOL SetRenderTarget(RtgTextItem* frameTarget, RtgTextItem* depthTarget = NULL) = 0;
    virtual BOOL RestoreRenderTarget() = 0;
    //virtual BOOL ResetDefaultRenderTarget()=0;

    virtual BOOL ClearRenderTarget(bool bStencil = false, bool bTarget = true, bool bZBuffer = true,
                                   DWORD dwColor = 0x00000000, float fNewZ = 1.f,
                                   DWORD dwStencil = 0) = 0;

    void         SetMatrixFromCamera();
    virtual void LockFps(DWORD dwMillisecondPerFrame) = 0;
    virtual BOOL SetLight(int iIdx, RtgLight* pLight) = 0;

    void DrawPoint(RtgVertex3& vPos, DWORD* pColor = NULL);
    void DrawLine(RtgVertex3& vStart, RtgVertex3& vEnd, DWORD* pColor = NULL);
    void DrawAxis(RtgMatrix16& m16, float fLen);
    void DrawBox(RtgVertex3& vHalfExt, RtgMatrix16& m16, DWORD* pColor = NULL);
    void DrawBox(RtgVertex3& mMin, RtgVertex3& mMax, DWORD* pColor = NULL);
    void DrawTriangle(RtgVertex3& v1, RtgVertex3& v2, RtgVertex3& v3, DWORD* pColor = NULL);
    void DrawAABBox(const RtgAABB* _aabbox, DWORD _color);

    RTGVertexFormat GetVertexFormat();
    DWORD           GetVertexStride();
    void            SetVertexFormat(RTGVertexFormat eVF, const char* pVertexShader = NULL);
    virtual BOOL    OnSetFVF() = 0;
    virtual DWORD   GetCurrentVertexStride() const = 0;

    virtual void SetMatrix(RTGMatrix eMode, RtgMatrix16* pM16) = 0;
    virtual void GetMatrix(RTGMatrix eMode, RtgMatrix16* pM16) = 0;

    //
    // native function
    //

    virtual BOOL SetStream_vb(UINT _StreamIndex, IDirect3DVertexBuffer9* _vb, UINT _Stride,
                              UINT _Offset = 0, UINT _vSize = 0, DWORD _FVF = 0) = 0;

    virtual BOOL SetStream_ib(IDirect3DIndexBuffer9* ib, UINT _Offset = 0) = 0;

    virtual BOOL SetFVF(DWORD _FVF) = 0;

    virtual BOOL DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinIndex,
                                      UINT NumVertices, UINT StartIndex, UINT PrimitiveCount) = 0;

    virtual BOOL DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex,
                               UINT PrimitiveCount) = 0;

    virtual BOOL DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE Type, UINT MinVertexIndex,
                                        UINT NumVertices, void* _Indices, void* _Vertex,
                                        UINT _vStride, UINT _PrimitiveCount) = 0;

    virtual BOOL DrawPrimitiveUP(D3DPRIMITIVETYPE Type, void* _Vertex, UINT _vStride,
                                 UINT _PrimitiveCount) = 0;

    // ext function

    virtual BOOL SetStream_vb(UINT _StreamIndex, RtgBufferItem* _vbItem, UINT _Stride,
                              DWORD _FVF = 0) = 0;

    virtual BOOL SetStream_ib(RtgBufferItem* _ibItem) = 0;

    virtual BOOL SetStream_vb(UINT _StreamIndex, RtgShareBufferItem* _vbItem, UINT _Stride,
                              DWORD _FVF = 0) = 0;

    virtual BOOL SetStream_ib(RtgShareBufferItem* _ibItem) = 0;

    virtual BOOL DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, UINT PrimitiveCount,
                                      UINT NumVertices = 0, UINT _ibOffset = 0) = 0;

    virtual BOOL DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, RtgBufferItem* vb, UINT _vbStride,
                                      RtgBufferItem* ib, UINT _PrimitiveCount) = 0;

    virtual BOOL DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, RtgShareBufferItem* vb, UINT _vbStride,
                                      RtgShareBufferItem* ib, UINT PrimitiveCount,
                                      UINT _ibOffset = 0) = 0;

    virtual BOOL DrawPrimitive(D3DPRIMITIVETYPE Type, RtgShareBufferItem* vb, UINT _vbStride,
                               UINT PrimitiveCount) = 0;

    virtual void DisableStreamSource(UINT _StreamIndex) = 0;

    //virtual BOOL DrawPrimitive(RTGPrimitive eType, void *pVertices, UINT dwVerticesCount, UINT dwPrimitiveCount=-1)=0;
    virtual BOOL DrawPrimitiveVB(RTGPrimitive eType, void* pVertices, UINT dwVertexStart,
                                 UINT dwVerticesCount, UINT dwPrimitiveCount) = 0;
    virtual BOOL DrawPrimitiveVB(RTGPrimitive eType, UINT dwVertexStart, UINT dwVerticesCount,
                                 UINT dwPrimitiveCount) = 0;  // for vb mgr
    //virtual BOOL DrawIndexPrimitive(RTGPrimitive eType, void *pVertices, UINT dwVerticesCount, void *pIndices, UINT dwIndicesCount, UINT dwPrimitiveCount=-1)=0;
    //virtual BOOL DrawIndexPrimitiveVB(RTGPrimitive eType, void *pVertices, UINT dwVerticesCount, void *pIndices, UINT dwIndicesCount, UINT dwPrimitiveCount)=0;
    //virtual BOOL DrawIndexPrimitiveVB(RTGPrimitive eType, void *pVertices, UINT dwVertexStart, UINT dwVerticesCount, void *pIndices, UINT dwIndexStart, UINT dwIndicesCount, UINT dwPrimitiveCount)=0;
    //virtual BOOL DrawIndexPrimitiveVB(RTGPrimitive eType, UINT dwVertexStart, UINT dwVerticesCount, UINT dwIndexStart, UINT dwPrimitiveCount)=0;

    virtual BOOL SetStreamSource(UINT dwStreamNumber, void* pStreamData, UINT dwStride) = 0;
    virtual BOOL SetIndices(void* pIndexData) = 0;

    virtual BOOL CreateVertexBuffer(UINT dwLength, void** ppVertexBuffer,
                                    DWORD dwUsage = 0x08L) = 0;
    virtual BOOL LockVertexBuffer(void* pVertexBuffer, void** ppData, DWORD dwOffset = 0,
                                  DWORD dwSize = 0, RTGLock eLockFlag = RTGLK_DISCARD) = 0;
    virtual BOOL UnLockVertexBuffer(void* pVertexBuffer) = 0;
    virtual BOOL ReleaseVertexBuffer(void* pVertexBuffer) = 0;

    virtual BOOL CreateIndexBuffer(UINT dwLength, void** ppIndexBuffer, DWORD dwUsage = 0x08L) = 0;
    virtual BOOL LockIndexBuffer(void* pIndexBuffer, void** ppData, DWORD dwOffset = 0,
                                 DWORD dwSize = 0, RTGLock eLockFlag = RTGLK_DISCARD) = 0;
    virtual BOOL UnLockIndexBuffer(void* pIndexBuffer) = 0;
    virtual BOOL ReleaseIndexBuffer(void* pIndexBuffer) = 0;

    void SetVertexBufferStart(DWORD dwOffset) { m_dwVertexBufferStartOffset = dwOffset; }

    void SetIndexBufferStart(DWORD dwOffset) { m_dwIndexBufferStartOffset = dwOffset; }

    /*----------------------------------------------------------------------
    Material
----------------------------------------------------------------------*/

    virtual BOOL SetMaterial(RtgVertex4& vDiffuse, RtgVertex4& vAmbient, RtgVertex4& vSpecular,
                             RtgVertex4& vEmissive, float fPower = 0.f) = 0;
    BOOL         SetDefaultMaterial(float fR = 1.f, float fG = 1.f, float fB = 1.f, float fA = 1.f);
    BOOL         SetHighlightPower(float fPower = 0.f);  // ���ø߹��ǿ��

    /*----------------------------------------------------------------------
    Texture
----------------------------------------------------------------------*/

    virtual BOOL SelectTextureFormat(int iBits) = 0;
    virtual void SetTextureFilterMip(RTGTextureFilter eType) = 0;
    virtual void SetTextureFilterMin(RTGTextureFilter eType) = 0;
    virtual void SetTextureFilterMag(RTGTextureFilter eType) = 0;
    virtual void SetTextureAddress(RTGTextureAddress eType) = 0;
    virtual void SetTextureBorderColor(DWORD dwColor) = 0;
    virtual void SetShader(RtgShader* pShader) = 0;
    virtual void RestoreShader(RtgShader* pShader) = 0;
    virtual void DeleteShader(RtgShader* pShader) = 0;
    virtual bool SaveTexture(void* p, const char* fileName) = 0;
    virtual void ResetTextMat() = 0;

    virtual void Blur(RtgTextItem* texItem) = 0;
    virtual void Gray(RtgTextItem* texItem, bool bFirst) = 0;

   public:  // ���º���ֻ�ṩ���� RtgTexture ʹ�ã�Ҫʹ�����ƹ��ܣ������ m_Texture.function()
    virtual BOOL CreateTextureByFile(_TexItem* pItem, const char* szTexFileName,
                                     int iMipMap = 0) = 0;
    virtual BOOL CreateTextureByImage(_TexItem* pItem, RtgImage& img) = 0;
    virtual BOOL CreateTextureDynamic(_TexItem* pItem) = 0;
    virtual BOOL CreateTextureRenderTarget(_TexItem* pItem) = 0;
    virtual BOOL CreateTextureDepthStencil(_TexItem* pItem) = 0;
    virtual BOOL CreateTextureByMemoryCpy(_TexItem* pItem) = 0;
    virtual BOOL CreateTexture(_TexItem* pItem, BOOL bDxManaged) = 0;
    virtual BOOL DeleteTexture(_TexItem* pItem) = 0;
    virtual BOOL UseTexture(int stage, RtgTextItem* texItem) = 0;
    virtual BOOL UpdateTexture(_TexItem* pItem, void* pBuffer) = 0;
    virtual BOOL UpdateTextureDynamic(_TexItem* pItem, void* pBuffer) = 0;
    virtual void SetTextureMatrix(int iIdx, RtgMatrix16* pM16) = 0;
    virtual void GetTextureMatrix(int iIdx, RtgMatrix16* pM16) = 0;
    virtual BOOL TextureLock(_TexItem* pItem, RtgTexture::TexLock& texLock) = 0;
    virtual void TextureUnlock(_TexItem* pItem, RtgTexture::TexLock& texLock) = 0;

   protected:
    virtual bool FullscreenToggle() = 0;
    virtual bool SetFullScreenInfo(DWORD dwWidth, DWORD dwHeight, DWORD dwDepth) = 0;
    virtual bool ResetDevice(BOOL bFullscreen, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth) = 0;

    /*----------------------------------------------------------------------
    Fog
----------------------------------------------------------------------*/
   public:
    virtual void SetFogEnable(BOOL bEnable) = 0;  // ��Ŀ���
    virtual void SetFogParameter(DWORD dwColor, float fNear,
                                 float fFar) = 0;  // ���������ɫ�������Զֵ
    virtual void SetFogColor(DWORD dwColor) = 0;   // ���������ɫ
    virtual void SetFogNear(float fNear) = 0;      // ����������ֵ
    virtual void SetFogFar(float fFar) = 0;        // ���������Զֵ

    virtual BOOL GetFogEnable() { return m_bFogEnable; }  // ȡ����Ŀ���

    virtual DWORD GetFogColor() { return m_dwFogColor; }  // ȡ�������ɫ

    virtual float GetFogNear() { return m_fFogStart; }  // ȡ��������ֵ

    virtual float GetFogFar() { return m_fFogEnd; }  // ȡ�������Զֵ

    /*----------------------------------------------------------------------
    Device State Block
----------------------------------------------------------------------*/
    virtual bool CreateUIStateBlock(DWORD* dwToken) = 0;
    virtual bool BeginRecordStateBlock() = 0;
    virtual bool EndRecordStateBlock(DWORD* dwToken) = 0;
    virtual bool DeleteStateBlock(DWORD dwToken) = 0;
    virtual bool ApplyStateBlock(DWORD dwToken) = 0;
    virtual bool CaptureStateBlock(DWORD dwToken) = 0;

    /*----------------------------------------------------------------------
    Other
----------------------------------------------------------------------*/
    void         SetCamera(RtgCamera* pCamera);
    void         SetEvent(RtgDeviceEvent* pEvent);
    virtual void DrawString(float fX, float fY, DWORD dwColor, const char* pString,
                            DWORD dwFlags = 0) = 0;

    virtual int  GetAvailableTextureMem();
    virtual BOOL SaveRenderTargetToFile(const char* szFileName,
                                        bool        bJPG = true);  // bJPG=false�ʹ��BMP
    virtual BOOL SaveRenderTargetToArchive(RtArchive* pArc,
                                           bool       bJPG = true);  // bJPG=false�ʹ��BMP

    virtual BOOL IsWindowed() = 0;
    virtual BOOL HasFocus() = 0;
    virtual BOOL IsActive() = 0;

    RTGViewWindowMode GetViewWindowMode() { return m_eCurrentViewWindowMode; }

    const RtgViewWindowInfo& GetViewWindowInfo(RTGViewWindowMode eMode) { return m_ViewWnd[eMode]; }

    RTHINSTANCE GetAppInstance() { return m_hAppInstance; }

    RTHWND GetWndHandle(RTGViewWindowMode eMode = RTGVWM_WINDOWED);

    virtual void UpdateAppTime() = 0;
    virtual float GetElapsedTime() = 0;  // ��������һ�ε���Render��FrameMove��ʱ���(��)
    virtual float GetAppTime() = 0;      // ���س���ִ�е�ʱ��(��)

    virtual void SetPostProcessEffectOnoff(bool bOn) { m_bUsePostProcessEffectOnoff = bOn; }

    virtual bool IsUsePostProcessEffectOn() { return m_bUsePostProcessEffectOnoff; }

    virtual void Gray(bool bOn) { m_bGray = bOn; }

    virtual bool IsGray() { return m_bGray; }

    void SetPostProcessObjectHighLightOnoff(bool bOn) {
        m_bUsePostProcessObjectHighLightOnoff = bOn;
    }

    bool IsUsePostProcessObjectHighLightOn() { return m_bUsePostProcessObjectHighLightOnoff; }

    void UsePostProcessObjectHighLight(bool bUse) { m_bUsePostProcessObjectHighLight = bUse; }

    bool IsUsePostProcessObjectHighLight() { return m_bUsePostProcessObjectHighLight; }

    RTGTextureAddress GetTextureAddress() const { return m_eTextureAddress; }

    void GetScreenPos(RtgVertex3& vWorldPos, int& nOutX, int& nOutY);
    void GetScreenPos(RtgVertex3& vWorldPos, int& nOutX, int& nOutY, float& nOutZ);
    void GetPickRayFromScreen(int sx, int sy, RtgVertex3& vOrig, RtgVertex3& vDir);

    // ȡ����ʾģʽ, �����ܹ�����ȡ�õ�����
    //     ���pModeBuf����ָ��Ϊ�գ��Ϳ���ͨ������ֵȡ��Buf�Ĵ�С,
    //     iBufSize����Buffer�Ĵ�С�����Ϊ0��ʾ�����ƴ�С
    virtual DWORD GetDisplayMode(RtgDisplayMode* pModeBuf, DWORD dwBufSize = 0) = 0;
    // �Ƿ����д���ģʽ
    virtual bool GetCanDoWindowed() = 0;
    // �豸����
    virtual const char* GetDeviceDesc() = 0;

    ///!Z buffer surface switch
    //virtual bool  StoreDefaultZBuffer( )  { return false; }
    //virtual bool  ResetDefaultZBuffer( )  { return false; }
   protected:
    void ReadGraphIni(RtIni* pGraphIni = nullptr);
    void ReadUserIni(RtIni* pUserIni = nullptr);
    void ApplyConfig();

   public:
    // current render state
    RTGDeviceType m_eDeviceType;   // �豸����
    BOOL          m_bDeviceReady;  // ��ǰ�����Ƿ��Ѿ�׼����

    RtgCamera* m_pCamera;          // ϵͳĬ�ϵ�Camera
                                   // RtgTexture          m_Texture;              // ��ͼ����
    RtgDeviceAbility   m_Ability;  // ����
    RtgGeometryFactory m_GeometryFactory;  // ����ͼ��������

    RtgRenderStreamManager* m_pVB;  // Vertex Buffer ����
    //RtgRenderStreamManager* m_pIB;              // Index Buffer ����

    // Display mode
    int              m_iWndWidth;          // ��Ļ���
    int              m_iWndHeight;         // ��Ļ�߶�
    int              m_iWndColorBits;      // ��ɫ���
    RTGTextureFormat m_eBackbufferFormat;  // Backbuffer Format
    DWORD            m_dwRefreshRate;      // ��Ļˢ����

    // cull face
    BOOL  m_bLeftHanded;    // �Ƿ�����������ϵ
    BOOL  m_bCullFace;      // �Ƿ�ʹ�ÿ�������
    DWORD m_dwCullDefault;  // Ĭ�ϵ�����

    // render device
    //RtgVertexSpec       m_VertexSpec;         // ���Ķ�������
    RTGVertexFormat m_eVertexFormat;      // ���Ķ�������
    DWORD           m_dwVertexFVF;        // ���һ��ʹ�õ�FVF
    DWORD           m_dwVertexStride;     // ���һ��ʹ�õĶ����ʽ�Ĵ�С
    BOOL            m_bVertexProcessing;  // �Ƿ������������ (VS)

    // lighting
    BOOL m_bLighting;  // �Ƿ�ʹ�õƹ�
    BOOL m_bLight[8];  // ʹ�õĵƹ�

    // alpha state
    BOOL         m_bAlphaTest;  // �Ƿ�ʹ��AlphaTest
    int          m_iAlphaRef;   // AlphaTest����
    RTGAlphaFunc m_eAlphaFunc;  // AlphaTest����

    // blend state
    BOOL         m_bBlend;          // �Ƿ�ʹ�û��
    RTGBlendFunc m_eBlendSrcFunc;   // ԭ
    RTGBlendFunc m_eBlendDestFunc;  // Ŀ��
    DWORD        m_dwBlendFactor;
    DWORD        m_dwTextureFactor;  // Texture Factor

    // Shader
    RtgShader* m_pLastShader;  // ���һ��Shader
    RtgShader  m_nullShader;   // �յ�Shader

    // depth
    BOOL         m_bZTest;  // �Ƿ�ʹ����ȼ��
    BOOL         m_bZWrite;
    RTGDepthFunc m_eZFunc;

    // Material
    RtgVertex4 m_vMaterialDiffuse;
    RtgVertex4 m_vMaterialAmbient;
    RtgVertex4 m_vMaterialSpecular;
    RtgVertex4 m_vMaterialEmissive;
    float      m_fMaterialPower;

    // texture state
    BOOL             m_bTexture[8];            // �Ƿ�ʹ����ͼ
    BOOL             m_bUseTextureMatrix[8];   // �Ƿ�ʹ����ͼMatrix
    long             m_dwTexTransformFlag[8];  // ��ͼ��������
    RtgMatrix16      m_mTextureDefaultMatrix;  // ��ͼ��Ĭ�Ͼ���
    RTGTextureFilter m_eTextureFilterMip;      // MipMap��֮��Ĺ���
    RTGTextureFilter m_eTextureFilterMin;      // ��ͼ��С�Ĺ���
    RTGTextureFilter m_eTextureFilterMag;      // ��ͼ�Ŵ�Ĺ���
    DWORD m_dwTextureBorderColor;  // ��ͼ��Ե��ɫ��ֻ������ͼѰַ��ʽΪRTGTADD_BORDERʱ��Ч
    int                 m_iTextureCurrentID;       // ��ǰʹ�õ���ͼID
    RTGTextureAddress   m_eTextureAddress;         // ��ͼѰַ��ʽ
    int                 m_iTextureCurPass;         // ��ǰ����ͼͨ��
    int                 m_iTextureRequiredPasses;  // ��ҪDraw�Ĵ��� (1Ϊֻ��1��)
    RTGTextureBlendMode m_eTextureBlendMode[RTG_MAX_MULTITEXTURES];
    RTGTextureCoordMode m_eTextureCoordMode[RTG_MAX_MULTITEXTURES];

    // polygon mode
    RTGFillMode m_eFillMode;         // ���ģʽ �㡢�߿���ͼ
    BOOL        m_bEnableViewCheck;  // �Ƿ���пɼ����
    BOOL m_bClearZRender;  // �Ƿ����Z�����������OnClearZRender����(ͨ�����ڱ༭��)

    // matrix
    RTGMatrix    m_eMatrixMode;
    RtgMatrix16  m_WorldMatrix;
    RtgMatrix16  m_ProjectionMatrix;
    RtgMatrix16  m_ProjectionCullingMatrix;
    RtgMatrix16  m_TextureMatrix[8];
    RtgMatrix16* m_pCurrentMatrix;

    // Color
    unsigned long m_dwClearColor;    // �����������ɫ
    unsigned long m_dwCurrentColor;  // ��ǰ�������ɫ

    int   m_iPostProcessEffectType;       // 0 Ĭ��, 1 �ڰ�
    float m_fPostProcessEffectContrast;   // ���ڴ���Աȶ�
    float m_fPostProcessEffectIntensity;  // ���ڴ���ǿ��

    // spec light pass
    float  m_fPostProcessObjectHLScale;
    DWORD  m_dwPostProcessObjectHLColorEmi;
    DWORD  m_dwPostProcessObjectHLColorDif;
    DWORD  m_dwPostProcessObjectHLColorSpe;
    double m_fPostProcessObjectHLColorSpePower;

   protected:
    BOOL         m_bFogEnable;  // �Ƿ�ʹ����
    unsigned int m_dwFogColor;
    float        m_fFogStart;
    float        m_fFogEnd;
    DWORD        m_dwVertexBufferStartOffset;
    DWORD        m_dwIndexBufferStartOffset;

    bool m_bUsePostProcessObjectHighLight;
    bool m_bUsePostProcessEffectOnoff;           // ����
    bool m_bUsePostProcessObjectHighLightOnoff;  // ����
    bool m_bGray;

    /*  DWORD               m_RenderTargetStack[10];
	DWORD               m_ZStencilStack[10];
    int                 m_iRenderTargetStackSize;*/

    RTHINSTANCE       m_hAppInstance;
    RTGViewWindowMode m_eCurrentViewWindowMode;
    RtgViewWindowInfo m_ViewWnd[RTGVWM_MAX];

   public:
    struct SUserConfig {
        // Graph
        bool  bFullscreen;     // ȫ��ģʽ   ��
        bool  bFSAA;           // ȫ������� ��
        bool  bEdgeAntialias;  // ��Ե����� ��
        bool  bEnableConsole;
        long  lWndWidth;
        long  lWndHeight;
        long  lWndColorDepth;
        bool  bPosteffect;  // ���ڴ���   ��
        bool  bObjEffect;   // ���ڼ�ǿ   ��
        DWORD dwMaxMultiSampleTypeWindowed;
        DWORD dwMaxMultiSampleTypeFullscreen;
        long  lMillisecondPerFrame;
        //lyymark ��ֱͬ��
        bool bvsync;

        // ����
        long lTextureQuality;  // ��ͼ����   �� (��ͼ�����ȼ� 0 ���, 1 ���, 2 �ٴ�)
        long lGeometryQuality;  // ��������   �� (����ͼ�������ȼ� 0 ���, 1 ���, 2 �ٴ�)
    };

    struct SConfig {
        // Window
        int iDefaultStartX;  // ��ʼ���ڵ�Xλ�ã�Ĭ��Ϊ -1
        int iDefaultStartY;  // ��ʼ���ڵ�Yλ�ã�Ĭ��Ϊ -1
                             //char    szWindowTitle[256];
        CM_MEMDEF(szWindowTitle, 256)
        HWND hCustomWndHandle;

        // Camera
        float fCameraFOV;
        float fCameraAspect;
        float fCameraNear;
        float fCameraFar;
        bool  bEnableFog;

        SConfig(){CM_MEMPROTECTOR(szWindowTitle, 256)} SConfig(const SConfig& object) {
            CM_MEMPROTECTOR(szWindowTitle, 256)
        }

        ~SConfig() { CM_MEMUNPROTECTOR(szWindowTitle) }
    };

    SUserConfig m_userConfig;
    SConfig     m_config;

   public:
    friend RtgTexture;
    friend _TexItem;

    // ��ͼ(Texture)
    struct TextureFormat {
        RTGTextureFormat m_eFormat;
        int              m_bSupported;
        int              m_iBits;
        int              m_iAlphaBits, m_iRBits, m_iGBits, m_iBBits;
        void (*fill)(int width, int height, void* dest, int destpitch, void* src, int srcpitch);
    };

    enum { MAX_TEXTUREFORMATS = 32 };

    int            m_iTexNumBits;  // ��ͼ��ͼ�θ�ʽ
    int            m_iTexNumFormats;
    TextureFormat  m_TexFormat[MAX_TEXTUREFORMATS];
    TextureFormat* m_pTexAlphaFormat;  // ������ɫ��Alpha�ĸ�ʽ
    TextureFormat* m_pTexColorFormat;  // ֻ������ɫû��Alpha�ĸ�ʽ
    TextureFormat* m_pTexKeyFormat;    // ������ɫ��1λAlpha�ĸ�ʽ

   public:
    // ʱ��(Time)
    float m_fDesireFPS;  // ϣ���õ���֡��������õ��˾ͻ�Sleep

   public:
    // ͳ��(STAT.)
    int m_iLastRenderNumFaces;     // ���һ����Ⱦ������
    int m_iLastRenderNumClusters;  // ���һ����Ⱦ�Ĵ���
    int m_iLastRenderUseTexture;
    int m_iLastRenderDrawPrimitive;
    int m_iLastRenderDrawPrimitiveVB;
    int m_iLastRenderDrawIndexPrimitive;
    int m_iLastRenderDrawIndexPrimitiveVB;

   protected:
    RtgDeviceEvent* m_pEvent;  // �¼�������
    virtual bool    OnPreCreate3DDevice();
    virtual bool    OnAfterCreate3DDevice();
    virtual bool    OnInit();
    virtual void    OnClose();
    virtual bool    OnRestoreDevice();
    virtual bool    OnInvalidateDevice();
    virtual void    OnBeginRender();
    virtual void    OnRender();
    virtual void    OnRender2D();
    virtual void    OnEndRender();
    virtual void    OnClearZRender();

    // ʱ��Ϊ�����һ�ε��ñ�������ʱ��(��)��Ҫȡ��Ӧ�ó���ִ��ʱ������ú���GetElapsedTime
    virtual void OnFrameMove(float fDifTime);

    virtual bool OnCreate3DDevice() = 0;
    virtual int  OnRun() = 0;
};

class RtgDeviceD3D9;

inline RtPerformance* RtGetPref() {
    extern RtPerformance g_GraphPref;
    return &g_GraphPref;
}

inline RtgRenderOption* RtgGetRenderOption() {
    extern RtgRenderOption g_RenderOption;
    return &g_RenderOption;
}

}  // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
#endif
