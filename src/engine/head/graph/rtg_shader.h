
#include "graph/rtg_texture_manager.h"

namespace rt_graph {


struct _TexItem;
struct _TexIFL;

struct _RtgTextItem;
typedef _RtgTextItem RtgTextItem;

struct RtgTexStage
{
private :
    RtgTextItem*       pTextItem;

#ifdef _DEBUG
    union
    {
        _TexItem*       pTex;
        _TexIFL*        pIfl;
    };
#endif

public:

    RTGTextureOp        eColorOp;           // D3DTSS_COLOROP
    RTGTexArg           eColorArg1;         // D3DTSS_COLORARG1
    RTGTexArg           eColorArg2;         // D3DTSS_COLORARG2

    RTGTextureOp        eAlphaOp;           // D3DTSS_ALPHAOP
    RTGTexArg           eAlphaArg1;         // D3DTSS_ALPHAARG1
    RTGTexArg           eAlphaArg2;         // D3DTSS_ALPHAARG2

    RTGTexArg           eColorArg0;         // D3DTSS_COLORARG0
    RTGTexArg           eAlphaArg0;         // D3DTSS_ALPHAARG0
    RTGTexArg           eResultArg;         // D3DTSS_RESULTARG

    RTGTexCoor          eTexCoordIndex;     // D3DTSS_TEXCOORDINDEX

    BOOL                bUseTextureMatrix;  // 是否使用贴图矩阵(默认不使用)
    RtgMatrix16         mMatrix16;          // 贴图的矩阵
    long                dwTexTransformFlag; // D3DTSS_TEXTURETRANSFORMFLAGS = RTGTTF_COUNT2，可以用或（如：RTGTTF_COUNT3|RTGTTF_PROJECTED）

    /*
    float               fBumpEnvmat00;      // D3DTSS_BUMPENVMAT00
    float               fBumpEnvmat01;      // D3DTSS_BUMPENVMAT01
    float               fBumpEnvmat10;      // D3DTSS_BUMPENVMAT10
    float               fBumpEnvmat11;      // D3DTSS_BUMPENVMAT11
    float               fBumpEnvlScale;     // D3DTSS_BUMPENVLSCALE
    float               fBumpEnvlOffset;    // D3DTSS_BUMPENVLOFFSET

    DWORD               dwConstant;         // D3DTSS_CONSTANT
    */

    /// 设置TextureStage的贴图.
    // 如果原来有设置贴图，这个函数会先调用DeleteTexture删除贴图，然后调用CreateTexture创建贴图。
    // @param inDevice 传入的Device参数
    // @param szFilename 贴图的文件名
    // @param eTexBitType 贴图的颜色数据格式
    //      - RTGTBT_AUTO     自动选择
    //      - RTGTBT_COLOR    只有颜色数据(适用于BMP)
    //      - RTGTBT_ALPHA    有Alpha数据(适用于TGA)
    //      - RTGTBT_COLORKEY Alpha层仅仅表示是否要使用前面的颜色(适用于TGA)
    //      - RTGTBT_DXT_AUTO DXT格式
    // @param bCreateNow 是否马上就创建贴图，如果设置为false，就会在使用到这个ID的时候进行创建，否则调用这个函数就进行创建
    // @param iMipmap 有几层的Mipmap
    // @param cFormat 贴图在内存中的格式 0 默认, 1 A8R8G8B8, 2 A4R4G4B4, 3 A1R5G5B5
    // @param bQualityManage 是否接受质量管理，如果接受就有可能被调低质量，所有的UI应该都不接受以保证UI的清晰度
    //void SetTexture(RtgDevice& inDevice, const char *szFilename, RTGTexBitType eTexBitType=RTGTBT_AUTO, bool bCreateNow=false, int iMipmap=1, BYTE cFormat=0, bool bQualityManage=true);
    //void SetTexture(unsigned int dwTexID)
    //unsigned int GetTexture()               { return dwTextureID; }
    
    
    void SetTexture(RtgTextItem* texItem) { pTextItem = texItem; }
    RtgTextItem* GetTexture() const { return pTextItem; }
    
    RtgTexStage(void)
    {
        pTextItem = NULL;
        //dwTextureID = 0;
#ifdef _DEBUG
        pTex = NULL;
#endif

        eColorOp   = RTGTOP_MODULATE;
        eColorArg1 = RTGTA_TEXTURE;
        eColorArg2 = RTGTA_DIFFUSE;

        eAlphaOp   = RTGTOP_SELECTARG1;
        eAlphaArg1 = RTGTA_TEXTURE;
        eAlphaArg2 = RTGTA_DIFFUSE;

        eColorArg0 = RTGTA_DIFFUSE;
        eAlphaArg0 = RTGTA_DIFFUSE;
        eResultArg = RTGTA_CURRENT;

        eTexCoordIndex = RTGTC_PASSTHRU;

        bUseTextureMatrix = FALSE;
        dwTexTransformFlag = RTGTTF_COUNT2;
    }
};

struct RtgShader
{
    bool                bColorKey;
    bool                bTwoSide;
    bool                bEnableBlendFunc;
    bool                bEnableAlphaTest;
    RTGBlendFunc        eBlendSrcFunc;
    RTGBlendFunc        eBlendDstFunc;
    DWORD               dwTextureFactor;
    int                 iMapChannel;
    int                 iFilter;
    DWORD               alphaRef;
    DWORD               dwBlendFactor;


    int iNumStage;
    RtgTexStage  Shaders[RTG_MAXSHADERS];
    
    RtgShader()
    {
        iNumStage = 0;
        bColorKey = false;
        bTwoSide = false;
        bEnableBlendFunc = false;
        bEnableAlphaTest = false;
        eBlendSrcFunc = RTGBLEND_SRCALPHA;
        eBlendDstFunc = RTGBLEND_INVSRCALPHA;
        dwTextureFactor = 0xFFFFFFFF;
        alphaRef = 0x32;
    }

    void Clone(RtgShader* _sh) const
    {
        _sh->bColorKey = bColorKey;
        _sh->bTwoSide = bTwoSide;
        _sh->bEnableBlendFunc = bEnableBlendFunc;
        _sh->bEnableAlphaTest = bEnableAlphaTest;
        _sh->eBlendSrcFunc = eBlendSrcFunc;
        _sh->eBlendDstFunc = eBlendDstFunc;
        _sh->dwTextureFactor = dwTextureFactor;
        _sh->iMapChannel = iMapChannel;
        _sh->iFilter = iFilter;
        _sh->alphaRef = alphaRef;
        _sh->dwBlendFactor = dwBlendFactor;
        _sh->iNumStage = iNumStage;

        for (int i = 0; i < iNumStage; ++i)
        {
            _sh->Shaders[i].SetTexture(Shaders[i].GetTexture());
            if (_sh->Shaders[i].GetTexture())
                _sh->Shaders[i].GetTexture()->Ref++;
        }
    }

    inline bool ResourceReady() const
    {
        for (int i = 0; i < iNumStage; ++i)
            if (Shaders[i].GetTexture() && Shaders[i].GetTexture()->state != Object_State_Ready)
                return false;
        return true;
    }

    RtgShader& operator = (const RtgShader& sh)
    {
        RTASSERT(Shaders[0].GetTexture() == NULL);
        return *this;
    }

#ifdef _DEBUG
    ~RtgShader();
#endif
    RtgTexStage *GetShaderItem(int index)
    {
        if ( index<0 || index>RTG_MAXSHADERS-1 )
            return NULL;
        return &Shaders[index];
    }
};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
