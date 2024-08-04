
// �����Ǳ��ļ����������͵�˵��
//
// enum RTGDeviceType           3D�豸���ͣ�����ΪD3D7, 8, 9 ���� OpenGL
// enum RTGMemoryType           �������ڴ����ͣ�����Ϊ�ڴ���Դ�
// enum RTG_SHADER_ENUM         Shader��һЩ��������
//
// enum RTGVertexFormat         ��������
// enum RTGLightMode            ����ģʽ��������Դ��ƽ�й�
// enum RTGBufferType           ���������ͣ�����ΪVB��IB
// enum RTGPrimitive
// enum RTGFillMode             �����ε����ģʽ��������������
// enum RTGLock                 Lock��ģʽ
// enum RTGCull                 ����ж�ģʽ
//
// enum RTGCmpFunc              �ȽϺ���������Alpha��Z�ıȽ�
//
// enum RTGBlendOP              ��Ⱦ���ģʽ�������ɫ����ɫ��
// enum RTGBlendFunc            ��ͼ��Ϻ���
//
// enum RTGRenderState          ��Ⱦ״̬������SetRenderState
// enum RTGUsage
// enum RTGMatrix               ���þ�������SetMatrix
//
// enum RTGTextureFormat        ��ͼ���ݵĸ�ʽ������565��8888�ȵ�
// enum RTGTextureBlendMode     ��ͼ���ģʽ�������ɫ����ɫ��
// enum RTGTextureCoordMode     ��ͼ����ģʽ��
// enum RTGTextureFilter
// enum RTGTextureAddress       ��ͼ��Ѱַ��ʽ
// enum RTGTextureOp            ��ͼ��ϲ���
// enum RTGTexArg               ��ͼ��ϲ���
// enum RTGTexCoor              ��ͼ����
// enum RTGTexTransformFlags    ��ͼת�����
// enum RTGMaterialColorSource  ������ɫ��Դ����������ͼ���߶�����ɫ
//
// enum RTGGeometryType         ����ͼ������
// enum RTGRenderMask           ��Ⱦ���������

namespace rt_graph {

enum RTGDeviceType {
    RTG_DEVICE_D3D7,
    RTG_DEVICE_D3D8,
    RTG_DEVICE_D3D9,
    RTG_DEVICE_OPENGL,
};

enum RTGViewWindowMode {
    RTGVWM_WINDOWED = 0,  // ����ģʽ
    RTGVWM_TOPWINDOW,  // ���㴰��ģʽ(��ȫ��, ���޸ķֱ���ʹ�Լ���������ȫ��Ļ)
    RTGVWM_FULLSCREEN,  // ȫ��Ļģʽ
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
    RTGTS_WORLD,  // �������ͨ�����ڶ�������������ϵ�е�λ�úͱ任
    RTGTS_VIEW,  // ��ͼ���󣬶����������λ�úͳ������ڽ���������ϵ������ת�����������ϵ
    RTGTS_PROJECTION,  // ͶӰ���󣬶�������׶�壬���ڽ���ά����ӳ�䵽��ά�ӿ�
    RTGTS_TEXTURE,  // �������ͨ��������������ı任��RTGTS_TEXTURE + iStage ��ʾ���������׶Σ�stage��
    RTGTS_MAX,  // �������͵����ֵ�����ڱ�ʾö�ٵĽ�����ͨ�����ڱ�������֤
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
    RTGCULL_NONE = 1,  // �����
    RTGCULL_FACE = 2,  // ����
    RTGCULL_CON = 3,   // ����
    RTGCULL_MAX = 3,
};

typedef unsigned short RTGIndexType;
typedef RTGCmpFunc     RTGAlphaFunc;
typedef RTGCmpFunc     RTGDepthFunc;
typedef unsigned short RTGIndex;

// ������һ��ö������ RTGRenderState�����ڱ�ʾ��ͬ����Ⱦ״̬
enum RTGRenderState {
    RTGRS_Z_TEST = 0,  // ��Ȳ��Կ��أ�Ĭ�Ͽ���������ȷ�����ص�����Ƿ�ͨ�����ԡ�
    RTGRS_Z_WRITE,  // ���д�뿪�أ�Ĭ�Ͽ��������ڿ�����Ȼ������Ƿ����д�롣
    RTGRS_Z_FUNC,  // ��ȱȽϺ�����Ĭ��ʹ�� RTGCMP_LESSEQUAL��С�ڻ���ڣ������ڱȽϵ�ǰ���ص����ֵ��
    RTGRS_COLOR_WRITE,  // ��ɫд�뿪�أ�Ĭ�Ͽ��������ڿ�����ɫ�������Ƿ����д�롣
    RTGRS_ALPHA_TEST,  // Alpha ���Կ��أ�Ĭ�Ϲرա����ڸ��� alpha ֵ���в��ԡ�
    RTGRS_ALPHA_REF,   // Alpha ���ԵĲο�ֵ�����������ص� alpha ֵ���бȽϡ�
    RTGRS_ALPHA_FUNC,  // Alpha ���Ժ�����Ĭ��ʹ�� RTGCMP_ALWAYS������ͨ���������ھ��� alpha �����Ƿ�ͨ����
    RTGRS_CULLMODE,  // �޳�ģʽ�����ڿ�����Щ��ᱻ�޳������米���޳�����
    RTGRS_DITHERENABLE,  // �������ÿ��أ����ڿ����Ƿ�ʹ�ö�����������ɫ�ʱ��֡�
    RTGRS_LIGHTING,              // ���տ��أ������Ƿ����ù���Ч����
    RTGRS_ALPHA_BLEND,           // Alpha ��Ͽ��أ������Ƿ����� alpha ��ϡ�
    RTGRS_BLEND_SRC,             // ���Դ���ӣ�������μ�����Դ����ɫ��
    RTGRS_BLEND_DEST,            // ���Ŀ�����ӣ�������μ�����Ŀ�����ɫ��
    RTGRS_ENVIRONMENT_LIGHTING,  // �������տ��أ��������û���û�������Ч����
    RTGRS_FILL_MODE,  // ���ģʽ�����ڶ��弸�������䷽ʽ����ʵ�Ļ��߿򣩡�
    RTGRS_DIFFUSEMATERIALSOURCE,  // ���������Դ��Ĭ��ʹ�� RTGMCS_COLOR1����ɫ1�������ڶ�����������յĲ�����Դ��
    RTGRS_SPECULARMATERIALSOURCE,  // �������Դ��Ĭ��ʹ�� RTGMCS_COLOR2����ɫ2�������ڶ��徵����յĲ�����Դ��
    RTGRS_AMBIENTMATERIALSOURCE,  // ��������Դ��Ĭ��ʹ�� RTGMCS_COLOR2����ɫ2�������ڶ��廷�����յĲ�����Դ��
    RTGRS_EMISSIVEMATERIALSOURCE,  // �Է������Դ��Ĭ��ʹ�� RTGMCS_MATERIAL�����ʣ������ڶ����Է���Ч���Ĳ�����Դ��
    RTGRS_HEIGHTLIGHT,  // �߹⿪�أ�Ĭ�Ϲرա��������û���ø߹�Ч����
    RTGRS_BLENDOP,  // ��ϲ�����Ĭ��ʹ�� RTGBOP_ADD���ӷ��������ڶ�����ɫ��ϵĲ�����ʽ��
    RTGRS_FULLSCRENE_ANTIALIASING,  // ȫ������ݿ��أ�Ĭ�Ͽ�������������ȫ������ݼ�����
    RTGRS_NORMALIZENORMALS,  // ���߹�һ�����أ�Ĭ�Ϲرա����ڿ����Ƿ�Է��߽��й�һ������
    RTGRS_EDGE_ANTIALIAS,  // ��Ե����ݿ��أ�Ĭ�Ϲرա��������û���ñ�Ե�����Ч����
    RTGRS_MAX  // ��Ⱦ״̬�����ֵ�����ڱ�������֤��
};

// ������һ��ö������ RTGBlendOP�����ڱ�ʾ��ͬ�Ļ�ϲ�������
enum RTGBlendOP {
    RTGBOP_ADD = 1,  // Result = Source + Destination  // �ӷ���ϲ������������Դ��ɫ����Ŀ����ɫ
    RTGBOP_SUBTRACT,  // Result = Source - Destination  // ������ϲ������������Դ��ɫ��ȥĿ����ɫ
    RTGBOP_REVSUBTRACT,  // Result = Destination - Source // ���������ϲ������������Ŀ����ɫ��ȥԴ��ɫ
    RTGBOP_MIN,  // Result = MIN(Source, Destination) // ��Сֵ��ϲ������������Դ��ɫ��Ŀ����ɫ�е���Сֵ
    RTGBOP_MAX,  // Result = MAX(Source, Destination) // ���ֵ��ϲ������������Դ��ɫ��Ŀ����ɫ�е����ֵ
    RTGBOP_MAX_ENUM  // ö�ٵ����ֵ��ͨ�����ڱ�������֤
};

// ������һ��ö������ RTGMaterialColorSource�����ڱ�ʾ��ͬ�Ĳ�����ɫ��Դ
enum RTGMaterialColorSource {
    RTGMCS_MATERIAL = 0,  // Color from material is used // ʹ�ò�����ɫ
    RTGMCS_COLOR1,        // Diffuse vertex color is used // ʹ�������䶥����ɫ
    RTGMCS_COLOR2,        // Specular vertex color is used // ʹ�þ��淴�䶥����ɫ
    RTGMCS_MAX            // ö�ٵ����ֵ��ͨ�����ڱ�������֤
};

// ������һ��ö������ RTGBlendFunc�����ڱ�ʾ��ͬ�Ļ�Ϻ���
enum RTGBlendFunc {
    RTGBLEND_ZERO = 0,      // 0  // �������Ϊ 0
    RTGBLEND_ONE,           // 1  // �������Ϊ 1
    RTGBLEND_SRCCOLOR,      // s  // Դ��ɫ��Ϊ�������
    RTGBLEND_INVSRCCOLOR,   // 1-s  // Դ��ɫ�ķ�ֵ��Ϊ�������
    RTGBLEND_SRCALPHA,      // sa  // Դ alpha ֵ��Ϊ�������
    RTGBLEND_INVSRCALPHA,   // 1-sa  // Դ alpha ֵ�ķ�ֵ��Ϊ�������
    RTGBLEND_DESTALPHA,     // da  // Ŀ�� alpha ֵ��Ϊ�������
    RTGBLEND_INVDESTALPHA,  // 1-da  // Ŀ�� alpha ֵ�ķ�ֵ��Ϊ�������
    RTGBLEND_DESTCOLOR,     // d  // Ŀ����ɫ��Ϊ�������
    RTGBLEND_INVDESTCOLOR,  // 1-d  // Ŀ����ɫ�ķ�ֵ��Ϊ�������
    RTGBLEND_SRCALPHASAT,   // sa_sat // Դ alpha ֵ���Ͷ���Ϊ�������
    RTGBLEND_BOTHSRCALPHA,  // sa + da // Դ alpha ֵ��Ŀ�� alpha ֵ�ĺ���Ϊ�������
    RTGBLEND_BOTHINVSRCALPHA,  // (1-sa) + (1-da) // Դ alpha ֵ��Ŀ�� alpha ֵ�ķ�ֵ�ĺ���Ϊ�������
    RTGBLEND_BLENDFACTOR,  // blend factor // �������
    RTGBLEND_MAX           // ö�ٵ����ֵ��ͨ�����ڱ�������֤
};

// ������һ��ö������ RTGUsage�����ڱ�ʾ��Դ��ʹ�÷�ʽ
enum RTGUsage {
    RTGU_RENDERTARGET = 0x00000001L,  // ��ȾĿ�� // ��ʾ��Դ������ȾĿ��
    RTGU_DEPTHSTENCIL = 0x00000002L,  // ���ģ�� // ��ʾ��Դ������Ȼ�ģ�建����
    RTGU_WRITEONLY = 0x00000008L,     // ֻд // ��ʾ��Դ������д�룬����ȡ
    RTGU_SOFTWAREPROCESSING = 0x00000010L,  // ������� // ��ʾ��Դ�����������
    RTGU_DONOTCLIP = 0x00000020L,           // ������ // ��ʾ��Դ����Ⱦʱ�����м���
    RTGU_POINTS = 0x00000040L,              // �� // ��ʾ��Դ���ڵ���Ⱦ
    RTGU_RTPATCHES = 0x00000080L,           // RTPatches // ��ʾ��Դ���� RTPatches
    RTGU_NPATCHES = 0x00000100L,            // NPatches // ��ʾ��Դ���� NPatches
    RTGU_DYNAMIC = 0x00000200L,             // ��̬ // ��ʾ��Դ���ڶ�̬����
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

// ���� RtgGeometryNode �ļ���ͼ������
enum RTGGeometryType {
    RTGGT_RECTANGLE = 0,  // ���Σ�һ����Ƭ
    RTGGT_TRAPEZIA,       // ����
    RTGGT_BOX,            // Box
    RTGGT_CIRCLE,         // Բ��
    RTGGT_CONE,           // ׶��
    RTGGT_CYLINDER,       // Բ����
    RTGGT_SPHERE,         // ����
    RTGGT_DONUT,          // Բ��Ȧ
    RTGGT_MAX
};

enum RTGRenderMask {
    RTGRM_BLENDOBJ_ONLY = 1,    // ֻ��Ⱦ��Alpha�����
    RTGRM_NOBLENDOBJ_ONLY = 2,  // ֻ��Ⱦ����Alpha�����
    RTGRM_ALL = 3,              // ��Ⱦ���е����
};

enum RTGTexBitType {
    RTGTBT_AUTO = 0,      // �Զ�ѡ��
    RTGTBT_COLOR = 1,     // ֻ����ɫ����
    RTGTBT_ALPHA = 2,     // ��Alpha����
    RTGTBT_COLORKEY = 3,  // ��ColorKey����(1λalpha)
    RTGTBT_DXT_AUTO = 4,  // ��DXT�ļ������ļ��Լ�����
    RTGTBT_MAX
};

}  // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
