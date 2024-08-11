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
    void SetCustomWndHandle(HWND hWnd);  // 如果用户自己要创建窗口，就需要设置这个属性
    void         Close();
    virtual int  RunEngine();
    virtual void RenderScene();
    virtual bool OnErrorMessage(const char* szMsg, int iType);  // iType 0 错误, 1 必须退出的错误
    virtual void OnWindowSizeChanged() = 0;

    virtual ID3DXEffect* GetEffect(RtgEffectType _Type) = 0;
    RtgDeviceEvent*      GetEvent();

    virtual void SetTextureCoord(int texId, int coordId) = 0;
    void         BindViewWindow(RTGViewWindowMode eMode, RTHWND hWnd, bool bResize = true);
    void SetViewWindowInfo(RTGViewWindowMode eMode, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth,
                           DWORD dwRefreshRate);
    bool SetViewMode(RTGViewWindowMode eMode, bool bStype = true);  // 设置显示模式

    static void ResumeDefaultDisplayMode();

   public:
    virtual long DeviceMsgProc(RTHWND hWnd, UINT uMsg, UINT wParam, UINT lParam) = 0;

    virtual BOOL BeginScene() = 0;
    virtual void EndScene() = 0;
    virtual void EndPostProcess() = 0;

    virtual BOOL  SetRenderState(RTGRenderState eState, DWORD dwValue) = 0;
    virtual DWORD GetRenderState(RTGRenderState eState) = 0;

    // SetRenderTarget和SetDefaultRenderTarget必须成对使用，
    //      也就是说调用SetRenderTarget之后一定要调用SetDefaultRenderTarget
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
    BOOL         SetHighlightPower(float fPower = 0.f);  // 设置高光的强度

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

   public:  // 以下函数只提供给类 RtgTexture 使用，要使用类似功能，请调用 m_Texture.function()
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
    virtual void SetFogEnable(BOOL bEnable) = 0;  // 雾的开关
    virtual void SetFogParameter(DWORD dwColor, float fNear,
                                 float fFar) = 0;  // 设置雾的颜色，最近最远值
    virtual void SetFogColor(DWORD dwColor) = 0;   // 设置雾的颜色
    virtual void SetFogNear(float fNear) = 0;      // 设置雾的最近值
    virtual void SetFogFar(float fFar) = 0;        // 设置雾的最远值

    virtual BOOL GetFogEnable() { return m_bFogEnable; }  // 取得雾的开关

    virtual DWORD GetFogColor() { return m_dwFogColor; }  // 取得雾的颜色

    virtual float GetFogNear() { return m_fFogStart; }  // 取得雾的最近值

    virtual float GetFogFar() { return m_fFogEnd; }  // 取得雾的最远值

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
                                        bool        bJPG = true);  // bJPG=false就存成BMP
    virtual BOOL SaveRenderTargetToArchive(RtArchive* pArc,
                                           bool       bJPG = true);  // bJPG=false就存成BMP

    virtual BOOL IsWindowed() = 0;
    virtual BOOL HasFocus() = 0;
    virtual BOOL IsActive() = 0;

    RTGViewWindowMode GetViewWindowMode() { return m_eCurrentViewWindowMode; }

    const RtgViewWindowInfo& GetViewWindowInfo(RTGViewWindowMode eMode) { return m_ViewWnd[eMode]; }

    RTHINSTANCE GetAppInstance() { return m_hAppInstance; }

    RTHWND GetWndHandle(RTGViewWindowMode eMode = RTGVWM_WINDOWED);

    virtual void UpdateAppTime() = 0;
    virtual float GetElapsedTime() = 0;  // 返回离上一次调用Render和FrameMove的时间差(秒)
    virtual float GetAppTime() = 0;      // 返回程序执行的时间(秒)

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

    // 取得显示模式, 返回总共可以取得的数量
    //     如果pModeBuf传入指针为空，就可以通过返回值取得Buf的大小,
    //     iBufSize限制Buffer的大小，如果为0表示不限制大小
    virtual DWORD GetDisplayMode(RtgDisplayMode* pModeBuf, DWORD dwBufSize = 0) = 0;
    // 是否能有窗口模式
    virtual bool GetCanDoWindowed() = 0;
    // 设备描述
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
    RTGDeviceType m_eDeviceType;   // 设备类型
    BOOL          m_bDeviceReady;  // 当前驱动是否已经准备好

    RtgCamera* m_pCamera;          // 系统默认的Camera
                                   // RtgTexture          m_Texture;              // 贴图管理
    RtgDeviceAbility   m_Ability;  // 能力
    RtgGeometryFactory m_GeometryFactory;  // 几何图形生成器

    RtgRenderStreamManager* m_pVB;  // Vertex Buffer 管理
    //RtgRenderStreamManager* m_pIB;              // Index Buffer 管理

    // Display mode
    int              m_iWndWidth;          // 屏幕宽度
    int              m_iWndHeight;         // 屏幕高度
    int              m_iWndColorBits;      // 颜色深度
    RTGTextureFormat m_eBackbufferFormat;  // Backbuffer Format
    DWORD            m_dwRefreshRate;      // 屏幕刷新率

    // cull face
    BOOL  m_bLeftHanded;    // 是否是左手坐标系
    BOOL  m_bCullFace;      // 是否使用可视面检查
    DWORD m_dwCullDefault;  // 默认的面检查

    // render device
    //RtgVertexSpec       m_VertexSpec;         // 最后的顶点属性
    RTGVertexFormat m_eVertexFormat;      // 最后的顶点属性
    DWORD           m_dwVertexFVF;        // 最后一次使用的FVF
    DWORD           m_dwVertexStride;     // 最后一次使用的顶点格式的大小
    BOOL            m_bVertexProcessing;  // 是否用软件处理顶点 (VS)

    // lighting
    BOOL m_bLighting;  // 是否使用灯光
    BOOL m_bLight[8];  // 使用的灯光

    // alpha state
    BOOL         m_bAlphaTest;  // 是否使用AlphaTest
    int          m_iAlphaRef;   // AlphaTest参数
    RTGAlphaFunc m_eAlphaFunc;  // AlphaTest函数

    // blend state
    BOOL         m_bBlend;          // 是否使用混合
    RTGBlendFunc m_eBlendSrcFunc;   // 原
    RTGBlendFunc m_eBlendDestFunc;  // 目的
    DWORD        m_dwBlendFactor;
    DWORD        m_dwTextureFactor;  // Texture Factor

    // Shader
    RtgShader* m_pLastShader;  // 最后一个Shader
    RtgShader  m_nullShader;   // 空的Shader

    // depth
    BOOL         m_bZTest;  // 是否使用深度检查
    BOOL         m_bZWrite;
    RTGDepthFunc m_eZFunc;

    // Material
    RtgVertex4 m_vMaterialDiffuse;
    RtgVertex4 m_vMaterialAmbient;
    RtgVertex4 m_vMaterialSpecular;
    RtgVertex4 m_vMaterialEmissive;
    float      m_fMaterialPower;

    // texture state
    BOOL             m_bTexture[8];            // 是否使用贴图
    BOOL             m_bUseTextureMatrix[8];   // 是否使用贴图Matrix
    long             m_dwTexTransformFlag[8];  // 贴图矩阵类型
    RtgMatrix16      m_mTextureDefaultMatrix;  // 贴图的默认矩阵
    RTGTextureFilter m_eTextureFilterMip;      // MipMap层之间的过滤
    RTGTextureFilter m_eTextureFilterMin;      // 贴图缩小的过滤
    RTGTextureFilter m_eTextureFilterMag;      // 贴图放大的过滤
    DWORD m_dwTextureBorderColor;  // 贴图边缘颜色，只有在贴图寻址方式为RTGTADD_BORDER时有效
    int                 m_iTextureCurrentID;       // 当前使用的贴图ID
    RTGTextureAddress   m_eTextureAddress;         // 贴图寻址方式
    int                 m_iTextureCurPass;         // 当前的贴图通道
    int                 m_iTextureRequiredPasses;  // 需要Draw的次数 (1为只有1次)
    RTGTextureBlendMode m_eTextureBlendMode[RTG_MAX_MULTITEXTURES];
    RTGTextureCoordMode m_eTextureCoordMode[RTG_MAX_MULTITEXTURES];

    // polygon mode
    RTGFillMode m_eFillMode;         // 填充模式 点、线框、贴图
    BOOL        m_bEnableViewCheck;  // 是否进行可见检查
    BOOL m_bClearZRender;  // 是否清除Z缓冲区后调用OnClearZRender函数(通常用在编辑器)

    // matrix
    RTGMatrix    m_eMatrixMode;
    RtgMatrix16  m_WorldMatrix;
    RtgMatrix16  m_ProjectionMatrix;
    RtgMatrix16  m_ProjectionCullingMatrix;
    RtgMatrix16  m_TextureMatrix[8];
    RtgMatrix16* m_pCurrentMatrix;

    // Color
    unsigned long m_dwClearColor;    // 清除背景的颜色
    unsigned long m_dwCurrentColor;  // 当前保存的颜色

    int   m_iPostProcessEffectType;       // 0 默认, 1 黑白
    float m_fPostProcessEffectContrast;   // 后期处理对比度
    float m_fPostProcessEffectIntensity;  // 后期处理强度

    // spec light pass
    float  m_fPostProcessObjectHLScale;
    DWORD  m_dwPostProcessObjectHLColorEmi;
    DWORD  m_dwPostProcessObjectHLColorDif;
    DWORD  m_dwPostProcessObjectHLColorSpe;
    double m_fPostProcessObjectHLColorSpePower;

   protected:
    BOOL         m_bFogEnable;  // 是否使用雾
    unsigned int m_dwFogColor;
    float        m_fFogStart;
    float        m_fFogEnd;
    DWORD        m_dwVertexBufferStartOffset;
    DWORD        m_dwIndexBufferStartOffset;

    bool m_bUsePostProcessObjectHighLight;
    bool m_bUsePostProcessEffectOnoff;           // 开关
    bool m_bUsePostProcessObjectHighLightOnoff;  // 开关
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
        bool  bFullscreen;     // 全屏模式   关
        bool  bFSAA;           // 全屏抗锯齿 关
        bool  bEdgeAntialias;  // 边缘抗锯齿 关
        bool  bEnableConsole;
        long  lWndWidth;
        long  lWndHeight;
        long  lWndColorDepth;
        bool  bPosteffect;  // 后期处理   关
        bool  bObjEffect;   // 后期加强   关
        DWORD dwMaxMultiSampleTypeWindowed;
        DWORD dwMaxMultiSampleTypeFullscreen;
        long  lMillisecondPerFrame;
        //lyymark 垂直同步
        bool bvsync;

        // 质量
        long lTextureQuality;  // 贴图质量   高 (贴图质量等级 0 最高, 1 其次, 2 再次)
        long lGeometryQuality;  // 几何质量   高 (几何图形质量等级 0 最高, 1 其次, 2 再次)
    };

    struct SConfig {
        // Window
        int iDefaultStartX;  // 开始窗口的X位置，默认为 -1
        int iDefaultStartY;  // 开始窗口的Y位置，默认为 -1
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

    // 贴图(Texture)
    struct TextureFormat {
        RTGTextureFormat m_eFormat;
        int              m_bSupported;
        int              m_iBits;
        int              m_iAlphaBits, m_iRBits, m_iGBits, m_iBBits;
        void (*fill)(int width, int height, void* dest, int destpitch, void* src, int srcpitch);
    };

    enum { MAX_TEXTUREFORMATS = 32 };

    int            m_iTexNumBits;  // 贴图的图形格式
    int            m_iTexNumFormats;
    TextureFormat  m_TexFormat[MAX_TEXTUREFORMATS];
    TextureFormat* m_pTexAlphaFormat;  // 包含颜色和Alpha的格式
    TextureFormat* m_pTexColorFormat;  // 只包含颜色没有Alpha的格式
    TextureFormat* m_pTexKeyFormat;    // 包含颜色和1位Alpha的格式

   public:
    // 时间(Time)
    float m_fDesireFPS;  // 希望得到的帧数，如果得到了就会Sleep

   public:
    // 统计(STAT.)
    int m_iLastRenderNumFaces;     // 最后一次渲染的面数
    int m_iLastRenderNumClusters;  // 最后一次渲染的次数
    int m_iLastRenderUseTexture;
    int m_iLastRenderDrawPrimitive;
    int m_iLastRenderDrawPrimitiveVB;
    int m_iLastRenderDrawIndexPrimitive;
    int m_iLastRenderDrawIndexPrimitiveVB;

   protected:
    RtgDeviceEvent* m_pEvent;  // 事件接收器
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

    // 时间为相距上一次调用本函数的时间(秒)，要取得应用程序执行时间请调用函数GetElapsedTime
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
