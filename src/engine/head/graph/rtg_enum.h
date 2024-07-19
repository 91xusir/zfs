
// 下面是被文件中所有类型的说明
//
// enum RTGDeviceType           3D设备类型，区分为D3D7, 8, 9 或者 OpenGL
// enum RTGMemoryType           缓冲区内存类型，可能为内存和显存
// enum RTG_SHADER_ENUM         Shader的一些常数设置
//
// enum RTGVertexFormat         顶点类型
// enum RTGLightMode            光照模式，比如点光源和平行光
// enum RTGBufferType           缓冲区类型，可能为VB和IB
// enum RTGPrimitive            
// enum RTGFillMode             三角形的填充模式，比如网格和填充
// enum RTGLock                 Lock的模式
// enum RTGCull                 面的判断模式
//
// enum RTGCmpFunc              比较函数，用于Alpha和Z的比较
//
// enum RTGBlendOP              渲染混合模式，比如加色、减色等
// enum RTGBlendFunc            贴图混合函数
//
// enum RTGRenderState          渲染状态，用于SetRenderState
// enum RTGUsage                
// enum RTGMatrix               设置矩阵，用于SetMatrix
//
// enum RTGTextureFormat        贴图数据的格式，比如565和8888等等
// enum RTGTextureBlendMode     贴图混合模式，比如加色、减色等
// enum RTGTextureCoordMode     贴图坐标模式，
// enum RTGTextureFilter
// enum RTGTextureAddress       贴图的寻址方式
// enum RTGTextureOp            贴图混合操作
// enum RTGTexArg               贴图混合参数
// enum RTGTexCoor              贴图坐标
// enum RTGTexTransformFlags    贴图转换标记
// enum RTGMaterialColorSource  材质颜色来源，可能是贴图或者顶点颜色
//
// enum RTGGeometryType         几何图形类型
// enum RTGRenderMask           渲染物件的类型


namespace rt_graph {

enum RTGDeviceType
{
    RTG_DEVICE_D3D7,
    RTG_DEVICE_D3D8,
    RTG_DEVICE_D3D9,
    RTG_DEVICE_OPENGL,
};

enum RTGViewWindowMode
{
    RTGVWM_WINDOWED = 0,    // 窗口模式
    RTGVWM_TOPWINDOW,       // 顶层窗口模式(假全屏, 会修改分辨率使自己看起来像全屏幕)
    RTGVWM_FULLSCREEN,      // 全屏幕模式
    RTGVWM_MAX
};

enum RTGMemoryType
{
    RTG_SYSTEM_MEMORY,
    RTG_VIDEO_MEMORY
};

enum RTG_SHADER_ENUM
{
    RTG_MAX_MULTITEXTURES = 8,
    RTG_MAXSHADERS        = 8,
};

enum RTGBufferType
{
    RTGBT_VERTEX_BUFFER,
    RTGBT_INDEX_BUFFER
};

enum RTGVertexFormat
{
    RTGVF_NULL              = 0,
    RTGVF_V                 = 0x0001,
    RTGVF_TV                = 0x0002,
    RTGVF_N                 = 0x0004,
    RTGVF_C                 = 0x0008,
    RTGVF_T1                = 0x0010,
    RTGVF_T2                = 0x0020,
    RTGVF_T3                = 0x0040,
    RTGVF_T4                = 0x0080,
    RTGVF_C2                = 0x0100,

    RTG_VNCT                = RTGVF_V  | RTGVF_N | RTGVF_C | RTGVF_T1,
    RTG_VNC2T               = RTGVF_V  | RTGVF_N | RTGVF_C | RTGVF_T1 | RTGVF_C2,
    RTG_VNCT2               = RTGVF_V  | RTGVF_N | RTGVF_C | RTGVF_T2,
    RTG_VNCT3               = RTGVF_V  | RTGVF_N | RTGVF_C | RTGVF_T3,
    RTG_VNCT4               = RTGVF_V  | RTGVF_N | RTGVF_C | RTGVF_T4,
    RTG_VNT                 = RTGVF_V  | RTGVF_N |       0 | RTGVF_T1,
    RTG_VNT2                = RTGVF_V  | RTGVF_N |       0 | RTGVF_T2,
    RTG_VNT3                = RTGVF_V  | RTGVF_N |       0 | RTGVF_T3,
    RTG_VNT4                = RTGVF_V  | RTGVF_N |       0 | RTGVF_T4,
    RTG_VCT                 = RTGVF_V  |       0 | RTGVF_C | RTGVF_T1,
    RTG_VCT2                = RTGVF_V  |       0 | RTGVF_C | RTGVF_T2,
    RTG_VCT3                = RTGVF_V  |       0 | RTGVF_C | RTGVF_T3,
    RTG_VCT4                = RTGVF_V  |       0 | RTGVF_C | RTGVF_T4,
    RTG_VN                  = RTGVF_V  | RTGVF_N |       0 |        0,
    RTG_VC                  = RTGVF_V  |       0 | RTGVF_C |        0,
    RTG_VT                  = RTGVF_V  |       0 |       0 | RTGVF_T1,
    RTG_VT2                 = RTGVF_V  |       0 |       0 | RTGVF_T2,
    RTG_TRANSFORMED_VT      = RTGVF_TV |       0 |       0 | RTGVF_T1,
    RTG_TRANSFORMED_VC      = RTGVF_TV |       0 | RTGVF_C |        0,
    RTG_TRANSFORMED_VT2     = RTGVF_TV |       0 |       0 | RTGVF_T2,
    RTG_TRANSFORMED_VCT     = RTGVF_TV |       0 | RTGVF_C | RTGVF_T1,
    RTG_TRANSFORMED_VCT2    = RTGVF_TV |       0 | RTGVF_C | RTGVF_T2,
    RTG_VERTEX_SHADER       = 0x8000,
};

enum RTGLightMode
{
    RTG_LIGHT_POINT,
    RTG_LIGHT_SPOT,
    RTG_LIGHT_DIRECTION,
    RTG_LIGHT_PARALLELPOINT,
    RTG_LIGHT_MAX
};

enum RTGPrimitive
{
    RTG_TRIANGLES = 0,
    RTG_TRIANGLE_FAN,
    RTG_TRIANGLE_STRIP,
    RTG_LINES,
    RTG_LINE_STRIP,
    RTG_POINTS
};

enum RTGFillMode
{
    RTG_FILLPOINT       = 1,
    RTG_FILLWIREFRAME   = 2,
    RTG_FILLSOLID       = 3,
};

enum RTGLock
{
    RTGLK_NONE           = 0,
    RTGLK_READONLY          ,
    RTGLK_DISCARD           ,
    RTGLK_NOOVERWRITE       ,
    RTGLK_NOSYSLOCK         ,
    RTGLK_MAX
};

enum RTGMatrix
{
    RTGTS_WORLD,
    RTGTS_VIEW,
    RTGTS_PROJECTION,
    RTGTS_TEXTURE,      // 使用 RTGTS_TEXTURE + iStage等于要设置的Stage
    RTGTS_MAX,
};

enum RTGCmpFunc
{
    RTGCMP_NEVER            = 1 ,  // !
    RTGCMP_LESS                 ,  // <
    RTGCMP_EQUAL                ,  // ==
    RTGCMP_LESSEQUAL            ,  // <=
    RTGCMP_GREATER              ,  // >
    RTGCMP_NOTEQUAL             ,  // !=
    RTGCMP_GREATEREQUAL         ,  // >=
    RTGCMP_ALWAYS               ,  // =
    RTGCMP_MAX                  ,
};

enum RTGTextureFormat
{
    RTG_TEXTURE_A8R8G8B8,
    RTG_TEXTURE_X8R8G8B8,
    RTG_TEXTURE_R8G8B8,
    RTG_TEXTURE_A4R4G4B4,
    RTG_TEXTURE_A1R5G5B5,
    RTG_TEXTURE_R5G6B5,
    RTG_TEXTURE_X1R5G5B5,
    RTG_TEXTURE_X4R4G4B4,
    RTG_TEXTURE_A8,
    RTG_TEXTURE_D24,
    RTG_TEXTURE_MAX
};

enum RTGTextureBlendMode
{
    RTG_BLENDCUSTOM,
    RTG_REPLACE,
    RTG_ADD,
    RTG_SUB,
    RTG_MODULATE,
    RTG_DOT3BUMPMAP
};

enum RTGTextureCoordMode
{
    RTG_TEXCOORD_SPECIFIC = 0,
    RTG_TEXCOORD_OBJECTXYZ,
    RTG_TEXCOORD_WORLDXYZ,
    RTG_TEXCOORD_EYESPACE,
    RTG_TEXCOORD_SPHEREMAP
};

enum RTGTextureFilter
{
    RTGTEXF_NONE            = 0,    // filtering disabled (valid for mip filter only)
    RTGTEXF_POINT              ,    // nearest
    RTGTEXF_LINEAR             ,    // linear interpolation
    RTGTEXF_ANISOTROPIC        ,    // anisotropic
    //RTGTEXF_FLATCUBIC        ,    // cubic
    //RTGTEXF_GAUSSIANCUBIC    ,    // different cubic kernel
    RTGTEXF_MAX                ,
};

enum RTGTextureAddress
{
    RTGTADD_WRAP         = 0,
    RTGTADD_MIRROR          ,
    RTGTADD_CLAMP          ,
    RTGTADD_BORDER          ,
    RTGTADD_MIRRORONCE      ,
    RTGTADD_MAX             ,
};

enum RTGCull
{
    RTGCULL_NONE         = 1,  // 不检测
    RTGCULL_FACE         = 2,  // 正面
    RTGCULL_CON          = 3,  // 反面
    RTGCULL_MAX          = 3,
};

typedef unsigned short  RTGIndexType;
typedef RTGCmpFunc      RTGAlphaFunc;
typedef RTGCmpFunc      RTGDepthFunc;
typedef unsigned short  RTGIndex;

enum RTGRenderState
{
    RTGRS_Z_TEST                 = 0, // [Default] = TRUE
    RTGRS_Z_WRITE                   , // [Default] = TRUE
    RTGRS_Z_FUNC                    , // [Default] = RTGCMP_LESSEQUAL  <=
    RTGRS_COLOR_WRITE               , // [Default] = TRUE
    RTGRS_ALPHA_TEST                , // [Default] = FALSE
    RTGRS_ALPHA_REF                 ,
    RTGRS_ALPHA_FUNC                , // [Default] = RTGCMP_ALWAYS
    RTGRS_CULLMODE                  , 
    RTGRS_DITHERENABLE              ,
    RTGRS_LIGHTING                  ,
    RTGRS_ALPHA_BLEND               ,
    RTGRS_BLEND_SRC                 ,
    RTGRS_BLEND_DEST                ,
    RTGRS_ENVIRONMENT_LIGHTING      ,
    RTGRS_FILL_MODE                 ,
    RTGRS_DIFFUSEMATERIALSOURCE     , // [Default] = RTGMCS_COLOR1
    RTGRS_SPECULARMATERIALSOURCE    , // [Default] = RTGMCS_COLOR2
    RTGRS_AMBIENTMATERIALSOURCE     , // [Default] = RTGMCS_COLOR2
    RTGRS_EMISSIVEMATERIALSOURCE    , // [Default] = RTGMCS_MATERIAL
    RTGRS_HEIGHTLIGHT               , // [Default] = FALSE              高光
    RTGRS_BLENDOP                   , // [Default] = RTGBOP_ADD         混合参数
    RTGRS_FULLSCRENE_ANTIALIASING   , // [Default] = TRUE               全屏反锯齿
    RTGRS_NORMALIZENORMALS          , // [Default] = FALSE
    RTGRS_EDGE_ANTIALIAS            , // [Default] = FALSE              边缘反锯齿
    RTGRS_MAX
};

enum RTGBlendOP
{
    RTGBOP_ADD              = 1,    // Result = Source + Destination
    RTGBOP_SUBTRACT            ,    // Result = Source - Destination 
    RTGBOP_REVSUBTRACT         ,    // Result = Destination - Source 
    RTGBOP_MIN                 ,    // Result = MIN(Source, Destination) 
    RTGBOP_MAX                 ,    // Result = MAX(Source, Destination) 
    RTGBOP_MAX_ENUM
};

enum RTGMaterialColorSource
{
    RTGMCS_MATERIAL         = 0,            // Color from material is used
    RTGMCS_COLOR1              ,            // Diffuse vertex color is used
    RTGMCS_COLOR2              ,            // Specular vertex color is used
    RTGMCS_MAX
};

enum RTGBlendFunc
{
    RTGBLEND_ZERO            = 0, // 0
    RTGBLEND_ONE                , // 1
    RTGBLEND_SRCCOLOR           , // s
    RTGBLEND_INVSRCCOLOR        , // 1-s
    RTGBLEND_SRCALPHA           , // sa
    RTGBLEND_INVSRCALPHA        , // 1-sa
    RTGBLEND_DESTALPHA          , // da
    RTGBLEND_INVDESTALPHA       , // 1-da
    RTGBLEND_DESTCOLOR          , // d
    RTGBLEND_INVDESTCOLOR       , // 1-d
    RTGBLEND_SRCALPHASAT        ,
    RTGBLEND_BOTHSRCALPHA       ,
    RTGBLEND_BOTHINVSRCALPHA    ,
    RTGBLEND_BLENDFACTOR,
    RTGBLEND_MAX
};

enum RTGUsage
{
    RTGU_RENDERTARGET       = 0x00000001L,
    RTGU_DEPTHSTENCIL       = 0x00000002L,
    RTGU_WRITEONLY          = 0x00000008L,
    RTGU_SOFTWAREPROCESSING = 0x00000010L,
    RTGU_DONOTCLIP          = 0x00000020L,
    RTGU_POINTS             = 0x00000040L,
    RTGU_RTPATCHES          = 0x00000080L,
    RTGU_NPATCHES           = 0x00000100L,
    RTGU_DYNAMIC            = 0x00000200L,
};

enum RTGTextureOp
{
    // Control
    RTGTOP_DISABLE                   = 0, // disables stage
    RTGTOP_SELECTARG1                , // the default
    RTGTOP_SELECTARG2                ,

    // Modulate
    RTGTOP_MODULATE                  , // multiply args together
    RTGTOP_MODULATE2X                , // multiply and  1 bit
    RTGTOP_MODULATE4X                , // multiply and  2 bits

    // Add
    RTGTOP_ADD                       , // add arguments together
    RTGTOP_ADDSIGNED                 , // add with -0.5 bias
    RTGTOP_ADDSIGNED2X               , // as above but left  1 bit
    RTGTOP_SUBTRACT                  , // Arg1 - Arg2, with no saturation
    RTGTOP_ADDSMOOTH                 , // add 2 args, subtract product
    // Arg1 + Arg2 - Arg1*Arg2
    // = Arg1 + (1-Arg1)*Arg2

    // Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
    RTGTOP_BLENDDIFFUSEALPHA         , // iterated alpha
    RTGTOP_BLENDTEXTUREALPHA         , // texture alpha
    RTGTOP_BLENDFACTORALPHA          , // alpha from D3DRS_TEXTUREFACTOR

    // Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
    RTGTOP_BLENDTEXTUREALPHAPM       , // texture alpha
    RTGTOP_BLENDCURRENTALPHA         , // by alpha of current color

    // Specular mapping
    RTGTOP_PREMODULATE               , // modulate with next texture before use
    RTGTOP_MODULATEALPHA_ADDCOLOR    , // Arg1.RGB + Arg1.A*Arg2.RGB
    RTGTOP_MODULATECOLOR_ADDALPHA    , // Arg1.RGB*Arg2.RGB + Arg1.A
    RTGTOP_MODULATEINVALPHA_ADDCOLOR , // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
    RTGTOP_MODULATEINVCOLOR_ADDALPHA , // (1-Arg1.RGB)*Arg2.RGB + Arg1.A

    // Bump mapping
    RTGTOP_BUMPENVMAP                , // per pixel env map perturbation
    RTGTOP_BUMPENVMAPLUMINANCE       , // with luminance channel

    // Dot Product
    RTGTOP_DOTPRODUCT3               ,

    // Triadic ops
    RTGTOP_MULTIPLYADD               , // Arg0 + Arg1*Arg2
    RTGTOP_LERP                      , // (Arg0)*Arg1 + (1-Arg0)*Arg2
    RTGTOP_MAX
};

enum RTGTexArg
{
    RTGTA_SELECTMASK = 0,  // mask for arg selector
    RTGTA_DIFFUSE       ,  // select diffuse color (read only)
    RTGTA_CURRENT       ,  // select stage destination register (read/write)
    RTGTA_TEXTURE       ,  // select texture color (read only)
    RTGTA_TFACTOR       ,  // select D3DRS_TEXTUREFACTOR (read only)
    RTGTA_SPECULAR      ,  // select specular color (read only)
    RTGTA_TEMP          ,  // select temporary register color (read/write)
    RTGTA_MAX
};

enum RTGTexCoor
{
    RTGTC_PASSTHRU                   = 0,
    RTGTC_PASSTHRU0                     ,
    RTGTC_PASSTHRU1                     ,
    RTGTC_PASSTHRU2                     ,
    RTGTC_PASSTHRU3                     ,
    RTGTC_PASSTHRU4                     ,
    RTGTC_PASSTHRU5                     ,
    RTGTC_PASSTHRU6                     ,
    RTGTC_PASSTHRU7                     ,
    RTGTC_CAMERASPACENORMAL             ,
    RTGTC_CAMERASPACEPOSITION           ,
    RTGTC_CAMERASPACEREFLECTIONVECTOR   ,
    RTGTC_MAX
};

enum RTGTexTransformFlag
{
    RTGTTF_DISABLE   = 0,
    RTGTTF_COUNT1    = 1,
    RTGTTF_COUNT2    = 2,
    RTGTTF_COUNT3    = 3,
    RTGTTF_COUNT4    = 4,
    RTGTTF_PROJECTED = 256,
    RTGTTF_MAX
};

// 用于 RtgGeometryNode 的几何图形类型
enum RTGGeometryType
{
    RTGGT_RECTANGLE      = 0,   // 矩形，一个面片
    RTGGT_TRAPEZIA          ,   // 梯形
    RTGGT_BOX               ,   // Box
    RTGGT_CIRCLE            ,   // 圆形
    RTGGT_CONE              ,   // 锥形
    RTGGT_CYLINDER          ,   // 圆柱体
    RTGGT_SPHERE            ,   // 球体
    RTGGT_DONUT             ,   // 圆环圈
    RTGGT_MAX
};

enum RTGRenderMask
{
    RTGRM_BLENDOBJ_ONLY   = 1,  // 只渲染带Alpha的物件
    RTGRM_NOBLENDOBJ_ONLY = 2,  // 只渲染不带Alpha的物件
    RTGRM_ALL             = 3,  // 渲染所有的物件
};

enum RTGTexBitType
{
    RTGTBT_AUTO         = 0,    // 自动选择
    RTGTBT_COLOR        = 1,    // 只有颜色数据
    RTGTBT_ALPHA        = 2,    // 有Alpha数据
    RTGTBT_COLORKEY     = 3,    // 有ColorKey数据(1位alpha)
    RTGTBT_DXT_AUTO     = 4,    // 是DXT文件，由文件自己决定
    RTGTBT_MAX
};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
