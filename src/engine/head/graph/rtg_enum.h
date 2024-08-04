
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

enum RTGDeviceType {
    RTG_DEVICE_D3D7,
    RTG_DEVICE_D3D8,
    RTG_DEVICE_D3D9,
    RTG_DEVICE_OPENGL,
};

enum RTGViewWindowMode {
    RTGVWM_WINDOWED = 0,  // 窗口模式
    RTGVWM_TOPWINDOW,  // 顶层窗口模式(假全屏, 会修改分辨率使自己看起来像全屏幕)
    RTGVWM_FULLSCREEN,  // 全屏幕模式
    RTGVWM_MAX
};

enum RTGMemoryType { RTG_SYSTEM_MEMORY, RTG_VIDEO_MEMORY };

enum RTG_SHADER_ENUM {
    RTG_MAX_MULTITEXTURES = 8,
    RTG_MAXSHADERS = 8,
};

enum RTGBufferType { RTGBT_VERTEX_BUFFER, RTGBT_INDEX_BUFFER };

enum RTGVertexFormat {
    RTGVF_NULL = 0,
    RTGVF_V = 0x0001,
    RTGVF_TV = 0x0002,
    RTGVF_N = 0x0004,
    RTGVF_C = 0x0008,
    RTGVF_T1 = 0x0010,
    RTGVF_T2 = 0x0020,
    RTGVF_T3 = 0x0040,
    RTGVF_T4 = 0x0080,
    RTGVF_C2 = 0x0100,

    RTG_VNCT = RTGVF_V | RTGVF_N | RTGVF_C | RTGVF_T1,
    RTG_VNC2T = RTGVF_V | RTGVF_N | RTGVF_C | RTGVF_T1 | RTGVF_C2,
    RTG_VNCT2 = RTGVF_V | RTGVF_N | RTGVF_C | RTGVF_T2,
    RTG_VNCT3 = RTGVF_V | RTGVF_N | RTGVF_C | RTGVF_T3,
    RTG_VNCT4 = RTGVF_V | RTGVF_N | RTGVF_C | RTGVF_T4,
    RTG_VNT = RTGVF_V | RTGVF_N | 0 | RTGVF_T1,
    RTG_VNT2 = RTGVF_V | RTGVF_N | 0 | RTGVF_T2,
    RTG_VNT3 = RTGVF_V | RTGVF_N | 0 | RTGVF_T3,
    RTG_VNT4 = RTGVF_V | RTGVF_N | 0 | RTGVF_T4,
    RTG_VCT = RTGVF_V | 0 | RTGVF_C | RTGVF_T1,
    RTG_VCT2 = RTGVF_V | 0 | RTGVF_C | RTGVF_T2,
    RTG_VCT3 = RTGVF_V | 0 | RTGVF_C | RTGVF_T3,
    RTG_VCT4 = RTGVF_V | 0 | RTGVF_C | RTGVF_T4,
    RTG_VN = RTGVF_V | RTGVF_N | 0 | 0,
    RTG_VC = RTGVF_V | 0 | RTGVF_C | 0,
    RTG_VT = RTGVF_V | 0 | 0 | RTGVF_T1,
    RTG_VT2 = RTGVF_V | 0 | 0 | RTGVF_T2,
    RTG_TRANSFORMED_VT = RTGVF_TV | 0 | 0 | RTGVF_T1,
    RTG_TRANSFORMED_VC = RTGVF_TV | 0 | RTGVF_C | 0,
    RTG_TRANSFORMED_VT2 = RTGVF_TV | 0 | 0 | RTGVF_T2,
    RTG_TRANSFORMED_VCT = RTGVF_TV | 0 | RTGVF_C | RTGVF_T1,
    RTG_TRANSFORMED_VCT2 = RTGVF_TV | 0 | RTGVF_C | RTGVF_T2,
    RTG_VERTEX_SHADER = 0x8000,
};

enum RTGLightMode {
    RTG_LIGHT_POINT,
    RTG_LIGHT_SPOT,
    RTG_LIGHT_DIRECTION,
    RTG_LIGHT_PARALLELPOINT,
    RTG_LIGHT_MAX
};

enum RTGPrimitive {
    RTG_TRIANGLES = 0,
    RTG_TRIANGLE_FAN,
    RTG_TRIANGLE_STRIP,
    RTG_LINES,
    RTG_LINE_STRIP,
    RTG_POINTS
};

enum RTGFillMode {
    RTG_FILLPOINT = 1,
    RTG_FILLWIREFRAME = 2,
    RTG_FILLSOLID = 3,
};

enum RTGLock {
    RTGLK_NONE = 0,
    RTGLK_READONLY,
    RTGLK_DISCARD,
    RTGLK_NOOVERWRITE,
    RTGLK_NOSYSLOCK,
    RTGLK_MAX
};

enum RTGMatrix {
    RTGTS_WORLD,  // 世界矩阵，通常用于对象在世界坐标系中的位置和变换
    RTGTS_VIEW,  // 视图矩阵，定义了相机的位置和朝向，用于将世界坐标系的物体转换到相机坐标系
    RTGTS_PROJECTION,  // 投影矩阵，定义了视锥体，用于将三维场景映射到二维视口
    RTGTS_TEXTURE,  // 纹理矩阵，通常用于纹理坐标的变换。RTGTS_TEXTURE + iStage 表示具体的纹理阶段（stage）
    RTGTS_MAX,  // 矩阵类型的最大值，用于表示枚举的结束，通常用于遍历和验证
};

enum RTGCmpFunc {
    RTGCMP_NEVER = 1,     // !
    RTGCMP_LESS,          // <
    RTGCMP_EQUAL,         // ==
    RTGCMP_LESSEQUAL,     // <=
    RTGCMP_GREATER,       // >
    RTGCMP_NOTEQUAL,      // !=
    RTGCMP_GREATEREQUAL,  // >=
    RTGCMP_ALWAYS,        // =
    RTGCMP_MAX,
};

enum RTGTextureFormat {
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

enum RTGTextureBlendMode {
    RTG_BLENDCUSTOM,
    RTG_REPLACE,
    RTG_ADD,
    RTG_SUB,
    RTG_MODULATE,
    RTG_DOT3BUMPMAP
};

enum RTGTextureCoordMode {
    RTG_TEXCOORD_SPECIFIC = 0,
    RTG_TEXCOORD_OBJECTXYZ,
    RTG_TEXCOORD_WORLDXYZ,
    RTG_TEXCOORD_EYESPACE,
    RTG_TEXCOORD_SPHEREMAP
};

enum RTGTextureFilter {
    RTGTEXF_NONE = 0,     // filtering disabled (valid for mip filter only)
    RTGTEXF_POINT,        // nearest
    RTGTEXF_LINEAR,       // linear interpolation
    RTGTEXF_ANISOTROPIC,  // anisotropic
    //RTGTEXF_FLATCUBIC        ,    // cubic
    //RTGTEXF_GAUSSIANCUBIC    ,    // different cubic kernel
    RTGTEXF_MAX,
};

enum RTGTextureAddress {
    RTGTADD_WRAP = 0,
    RTGTADD_MIRROR,
    RTGTADD_CLAMP,
    RTGTADD_BORDER,
    RTGTADD_MIRRORONCE,
    RTGTADD_MAX,
};

enum RTGCull {
    RTGCULL_NONE = 1,  // 不检测
    RTGCULL_FACE = 2,  // 正面
    RTGCULL_CON = 3,   // 反面
    RTGCULL_MAX = 3,
};

typedef unsigned short RTGIndexType;
typedef RTGCmpFunc     RTGAlphaFunc;
typedef RTGCmpFunc     RTGDepthFunc;
typedef unsigned short RTGIndex;

// 定义了一个枚举类型 RTGRenderState，用于表示不同的渲染状态
enum RTGRenderState {
    RTGRS_Z_TEST = 0,  // 深度测试开关，默认开启。用于确定像素的深度是否通过测试。
    RTGRS_Z_WRITE,  // 深度写入开关，默认开启。用于控制深度缓冲区是否可以写入。
    RTGRS_Z_FUNC,  // 深度比较函数，默认使用 RTGCMP_LESSEQUAL（小于或等于）。用于比较当前像素的深度值。
    RTGRS_COLOR_WRITE,  // 颜色写入开关，默认开启。用于控制颜色缓冲区是否可以写入。
    RTGRS_ALPHA_TEST,  // Alpha 测试开关，默认关闭。用于根据 alpha 值进行测试。
    RTGRS_ALPHA_REF,   // Alpha 测试的参考值。用于与像素的 alpha 值进行比较。
    RTGRS_ALPHA_FUNC,  // Alpha 测试函数，默认使用 RTGCMP_ALWAYS（总是通过）。用于决定 alpha 测试是否通过。
    RTGRS_CULLMODE,  // 剔除模式，用于控制哪些面会被剔除（例如背面剔除）。
    RTGRS_DITHERENABLE,  // 抖动启用开关，用于控制是否使用抖动技术改善色彩表现。
    RTGRS_LIGHTING,              // 光照开关，控制是否启用光照效果。
    RTGRS_ALPHA_BLEND,           // Alpha 混合开关，控制是否启用 alpha 混合。
    RTGRS_BLEND_SRC,             // 混合源因子，定义如何计算混合源的颜色。
    RTGRS_BLEND_DEST,            // 混合目标因子，定义如何计算混合目标的颜色。
    RTGRS_ENVIRONMENT_LIGHTING,  // 环境光照开关，用于启用或禁用环境光照效果。
    RTGRS_FILL_MODE,  // 填充模式，用于定义几何体的填充方式（如实心或线框）。
    RTGRS_DIFFUSEMATERIALSOURCE,  // 漫反射材质源，默认使用 RTGMCS_COLOR1（颜色1）。用于定义漫反射光照的材质来源。
    RTGRS_SPECULARMATERIALSOURCE,  // 镜面材质源，默认使用 RTGMCS_COLOR2（颜色2）。用于定义镜面光照的材质来源。
    RTGRS_AMBIENTMATERIALSOURCE,  // 环境材质源，默认使用 RTGMCS_COLOR2（颜色2）。用于定义环境光照的材质来源。
    RTGRS_EMISSIVEMATERIALSOURCE,  // 自发光材质源，默认使用 RTGMCS_MATERIAL（材质）。用于定义自发光效果的材质来源。
    RTGRS_HEIGHTLIGHT,  // 高光开关，默认关闭。用于启用或禁用高光效果。
    RTGRS_BLENDOP,  // 混合操作，默认使用 RTGBOP_ADD（加法）。用于定义颜色混合的操作方式。
    RTGRS_FULLSCRENE_ANTIALIASING,  // 全屏抗锯齿开关，默认开启。用于启用全屏抗锯齿技术。
    RTGRS_NORMALIZENORMALS,  // 法线归一化开关，默认关闭。用于控制是否对法线进行归一化处理。
    RTGRS_EDGE_ANTIALIAS,  // 边缘抗锯齿开关，默认关闭。用于启用或禁用边缘抗锯齿效果。
    RTGRS_MAX  // 渲染状态的最大值，用于遍历和验证。
};

// 定义了一个枚举类型 RTGBlendOP，用于表示不同的混合操作类型
enum RTGBlendOP {
    RTGBOP_ADD = 1,  // Result = Source + Destination  // 加法混合操作：结果等于源颜色加上目标颜色
    RTGBOP_SUBTRACT,  // Result = Source - Destination  // 减法混合操作：结果等于源颜色减去目标颜色
    RTGBOP_REVSUBTRACT,  // Result = Destination - Source // 反向减法混合操作：结果等于目标颜色减去源颜色
    RTGBOP_MIN,  // Result = MIN(Source, Destination) // 最小值混合操作：结果等于源颜色和目标颜色中的最小值
    RTGBOP_MAX,  // Result = MAX(Source, Destination) // 最大值混合操作：结果等于源颜色和目标颜色中的最大值
    RTGBOP_MAX_ENUM  // 枚举的最大值，通常用于遍历和验证
};

// 定义了一个枚举类型 RTGMaterialColorSource，用于表示不同的材质颜色来源
enum RTGMaterialColorSource {
    RTGMCS_MATERIAL = 0,  // Color from material is used // 使用材质颜色
    RTGMCS_COLOR1,        // Diffuse vertex color is used // 使用漫反射顶点颜色
    RTGMCS_COLOR2,        // Specular vertex color is used // 使用镜面反射顶点颜色
    RTGMCS_MAX            // 枚举的最大值，通常用于遍历和验证
};

// 定义了一个枚举类型 RTGBlendFunc，用于表示不同的混合函数
enum RTGBlendFunc {
    RTGBLEND_ZERO = 0,      // 0  // 混合因子为 0
    RTGBLEND_ONE,           // 1  // 混合因子为 1
    RTGBLEND_SRCCOLOR,      // s  // 源颜色作为混合因子
    RTGBLEND_INVSRCCOLOR,   // 1-s  // 源颜色的反值作为混合因子
    RTGBLEND_SRCALPHA,      // sa  // 源 alpha 值作为混合因子
    RTGBLEND_INVSRCALPHA,   // 1-sa  // 源 alpha 值的反值作为混合因子
    RTGBLEND_DESTALPHA,     // da  // 目标 alpha 值作为混合因子
    RTGBLEND_INVDESTALPHA,  // 1-da  // 目标 alpha 值的反值作为混合因子
    RTGBLEND_DESTCOLOR,     // d  // 目标颜色作为混合因子
    RTGBLEND_INVDESTCOLOR,  // 1-d  // 目标颜色的反值作为混合因子
    RTGBLEND_SRCALPHASAT,   // sa_sat // 源 alpha 值饱和度作为混合因子
    RTGBLEND_BOTHSRCALPHA,  // sa + da // 源 alpha 值和目标 alpha 值的和作为混合因子
    RTGBLEND_BOTHINVSRCALPHA,  // (1-sa) + (1-da) // 源 alpha 值和目标 alpha 值的反值的和作为混合因子
    RTGBLEND_BLENDFACTOR,  // blend factor // 混合因子
    RTGBLEND_MAX           // 枚举的最大值，通常用于遍历和验证
};

// 定义了一个枚举类型 RTGUsage，用于表示资源的使用方式
enum RTGUsage {
    RTGU_RENDERTARGET = 0x00000001L,  // 渲染目标 // 表示资源用作渲染目标
    RTGU_DEPTHSTENCIL = 0x00000002L,  // 深度模板 // 表示资源用作深度或模板缓冲区
    RTGU_WRITEONLY = 0x00000008L,     // 只写 // 表示资源仅用于写入，不读取
    RTGU_SOFTWAREPROCESSING = 0x00000010L,  // 软件处理 // 表示资源用于软件处理
    RTGU_DONOTCLIP = 0x00000020L,           // 不剪裁 // 表示资源在渲染时不进行剪裁
    RTGU_POINTS = 0x00000040L,              // 点 // 表示资源用于点渲染
    RTGU_RTPATCHES = 0x00000080L,           // RTPatches // 表示资源用于 RTPatches
    RTGU_NPATCHES = 0x00000100L,            // NPatches // 表示资源用于 NPatches
    RTGU_DYNAMIC = 0x00000200L,             // 动态 // 表示资源用于动态数据
};


enum RTGTextureOp {
    // Control
    RTGTOP_DISABLE = 0,  // disables stage
    RTGTOP_SELECTARG1,   // the default
    RTGTOP_SELECTARG2,

    // Modulate
    RTGTOP_MODULATE,    // multiply args together
    RTGTOP_MODULATE2X,  // multiply and  1 bit
    RTGTOP_MODULATE4X,  // multiply and  2 bits

    // Add
    RTGTOP_ADD,          // add arguments together
    RTGTOP_ADDSIGNED,    // add with -0.5 bias
    RTGTOP_ADDSIGNED2X,  // as above but left  1 bit
    RTGTOP_SUBTRACT,     // Arg1 - Arg2, with no saturation
    RTGTOP_ADDSMOOTH,    // add 2 args, subtract product
    // Arg1 + Arg2 - Arg1*Arg2
    // = Arg1 + (1-Arg1)*Arg2

    // Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
    RTGTOP_BLENDDIFFUSEALPHA,  // iterated alpha
    RTGTOP_BLENDTEXTUREALPHA,  // texture alpha
    RTGTOP_BLENDFACTORALPHA,   // alpha from D3DRS_TEXTUREFACTOR

    // Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
    RTGTOP_BLENDTEXTUREALPHAPM,  // texture alpha
    RTGTOP_BLENDCURRENTALPHA,    // by alpha of current color

    // Specular mapping
    RTGTOP_PREMODULATE,                // modulate with next texture before use
    RTGTOP_MODULATEALPHA_ADDCOLOR,     // Arg1.RGB + Arg1.A*Arg2.RGB
    RTGTOP_MODULATECOLOR_ADDALPHA,     // Arg1.RGB*Arg2.RGB + Arg1.A
    RTGTOP_MODULATEINVALPHA_ADDCOLOR,  // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
    RTGTOP_MODULATEINVCOLOR_ADDALPHA,  // (1-Arg1.RGB)*Arg2.RGB + Arg1.A

    // Bump mapping
    RTGTOP_BUMPENVMAP,           // per pixel env map perturbation
    RTGTOP_BUMPENVMAPLUMINANCE,  // with luminance channel

    // Dot Product
    RTGTOP_DOTPRODUCT3,

    // Triadic ops
    RTGTOP_MULTIPLYADD,  // Arg0 + Arg1*Arg2
    RTGTOP_LERP,         // (Arg0)*Arg1 + (1-Arg0)*Arg2
    RTGTOP_MAX
};

enum RTGTexArg {
    RTGTA_SELECTMASK = 0,  // mask for arg selector
    RTGTA_DIFFUSE,         // select diffuse color (read only)
    RTGTA_CURRENT,         // select stage destination register (read/write)
    RTGTA_TEXTURE,         // select texture color (read only)
    RTGTA_TFACTOR,         // select D3DRS_TEXTUREFACTOR (read only)
    RTGTA_SPECULAR,        // select specular color (read only)
    RTGTA_TEMP,            // select temporary register color (read/write)
    RTGTA_MAX
};

enum RTGTexCoor {
    RTGTC_PASSTHRU = 0,
    RTGTC_PASSTHRU0,
    RTGTC_PASSTHRU1,
    RTGTC_PASSTHRU2,
    RTGTC_PASSTHRU3,
    RTGTC_PASSTHRU4,
    RTGTC_PASSTHRU5,
    RTGTC_PASSTHRU6,
    RTGTC_PASSTHRU7,
    RTGTC_CAMERASPACENORMAL,
    RTGTC_CAMERASPACEPOSITION,
    RTGTC_CAMERASPACEREFLECTIONVECTOR,
    RTGTC_MAX
};

enum RTGTexTransformFlag {
    RTGTTF_DISABLE = 0,
    RTGTTF_COUNT1 = 1,
    RTGTTF_COUNT2 = 2,
    RTGTTF_COUNT3 = 3,
    RTGTTF_COUNT4 = 4,
    RTGTTF_PROJECTED = 256,
    RTGTTF_MAX
};

// 用于 RtgGeometryNode 的几何图形类型
enum RTGGeometryType {
    RTGGT_RECTANGLE = 0,  // 矩形，一个面片
    RTGGT_TRAPEZIA,       // 梯形
    RTGGT_BOX,            // Box
    RTGGT_CIRCLE,         // 圆形
    RTGGT_CONE,           // 锥形
    RTGGT_CYLINDER,       // 圆柱体
    RTGGT_SPHERE,         // 球体
    RTGGT_DONUT,          // 圆环圈
    RTGGT_MAX
};

enum RTGRenderMask {
    RTGRM_BLENDOBJ_ONLY = 1,    // 只渲染带Alpha的物件
    RTGRM_NOBLENDOBJ_ONLY = 2,  // 只渲染不带Alpha的物件
    RTGRM_ALL = 3,              // 渲染所有的物件
};

enum RTGTexBitType {
    RTGTBT_AUTO = 0,      // 自动选择
    RTGTBT_COLOR = 1,     // 只有颜色数据
    RTGTBT_ALPHA = 2,     // 有Alpha数据
    RTGTBT_COLORKEY = 3,  // 有ColorKey数据(1位alpha)
    RTGTBT_DXT_AUTO = 4,  // 是DXT文件，由文件自己决定
    RTGTBT_MAX
};

}  // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
