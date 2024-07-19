
static D3DRENDERSTATETYPE enumRTGRenderState[RTGRS_MAX] =
{
    D3DRS_ZENABLE                   ,
    D3DRS_ZWRITEENABLE              ,
    D3DRS_ZFUNC                     ,
    D3DRS_COLORWRITEENABLE          ,
    D3DRS_ALPHATESTENABLE           ,
    D3DRS_ALPHAREF                  ,
    D3DRS_ALPHAFUNC                 ,
    D3DRS_CULLMODE                  ,
    D3DRS_DITHERENABLE              ,
    D3DRS_LIGHTING                  ,
    D3DRS_ALPHABLENDENABLE          ,
    D3DRS_SRCBLEND                  ,
    D3DRS_DESTBLEND                 ,
    D3DRS_AMBIENT                   ,
    D3DRS_FILLMODE                  ,
    D3DRS_DIFFUSEMATERIALSOURCE     ,
    D3DRS_SPECULARMATERIALSOURCE    ,
    D3DRS_AMBIENTMATERIALSOURCE     ,
    D3DRS_EMISSIVEMATERIALSOURCE    ,
    D3DRS_SPECULARENABLE            ,
    D3DRS_BLENDOP                   ,
    D3DRS_MULTISAMPLEANTIALIAS      ,
    D3DRS_NORMALIZENORMALS          ,
    //D3DRS_EDGEANTIALIAS             ,
};

static D3DBLENDOP enumRTGBlendOP[RTGBOP_MAX_ENUM] =
{
    D3DBLENDOP_ADD              ,
    D3DBLENDOP_SUBTRACT         ,
    D3DBLENDOP_REVSUBTRACT      ,
    D3DBLENDOP_MIN              ,
    D3DBLENDOP_MAX              ,
};

static D3DMATERIALCOLORSOURCE enumRTGMaterialColorSource[RTGMCS_MAX] =
{
    D3DMCS_MATERIAL        ,            // Color from material is used
    D3DMCS_COLOR1          ,            // Diffuse vertex color is used
    D3DMCS_COLOR2          ,            // Specular vertex color is used
};

static D3DBLEND enumRTGBlendFunc[RTGBLEND_MAX] = 
{
    D3DBLEND_ZERO           , // 0
    D3DBLEND_ONE            , // 1
    D3DBLEND_SRCCOLOR       , // s
    D3DBLEND_INVSRCCOLOR    , // 1-s
    D3DBLEND_SRCALPHA       , // sa
    D3DBLEND_INVSRCALPHA    , // 1-sa
    D3DBLEND_DESTALPHA      , // da
    D3DBLEND_INVDESTALPHA   , // 1-da
    D3DBLEND_DESTCOLOR      , // d
    D3DBLEND_INVDESTCOLOR   , // 1-d
    D3DBLEND_SRCALPHASAT    ,
    D3DBLEND_BOTHSRCALPHA   ,
    D3DBLEND_BOTHINVSRCALPHA,
    D3DBLEND_BLENDFACTOR 
};

static D3DTEXTUREOP enumRTGTextureOp[RTGTOP_MAX] = 
{
    // Control
    D3DTOP_DISABLE,       // disables stage
    D3DTOP_SELECTARG1,    // the default
    D3DTOP_SELECTARG2,
    
    // Modulate
    D3DTOP_MODULATE,      // multiply args together
    D3DTOP_MODULATE2X,    // multiply and  1 bit
    D3DTOP_MODULATE4X,    // multiply and  2 bits

    // Add
    D3DTOP_ADD,           // add arguments together
    D3DTOP_ADDSIGNED,     // add with -0.5 bias
    D3DTOP_ADDSIGNED2X,   // as above but left  1 bit
    D3DTOP_SUBTRACT,      // Arg1 - Arg2, with no saturation
    D3DTOP_ADDSMOOTH,     // add 2 args, subtract product
    
    // Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
    D3DTOP_BLENDDIFFUSEALPHA,     // iterated alpha
    D3DTOP_BLENDTEXTUREALPHA,     // texture alpha
    D3DTOP_BLENDFACTORALPHA,      // alpha from D3DRS_TEXTUREFACTOR

    // Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
    D3DTOP_BLENDTEXTUREALPHAPM,   // texture alpha
    D3DTOP_BLENDCURRENTALPHA,     // by alpha of current color
    
    // Specular mapping
    D3DTOP_PREMODULATE,               // modulate with next texture before use
    D3DTOP_MODULATEALPHA_ADDCOLOR,    // Arg1.RGB + Arg1.A*Arg2.RGB
    D3DTOP_MODULATECOLOR_ADDALPHA,    // Arg1.RGB*Arg2.RGB + Arg1.A
    D3DTOP_MODULATEINVALPHA_ADDCOLOR, // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
    D3DTOP_MODULATEINVCOLOR_ADDALPHA, // (1-Arg1.RGB)*Arg2.RGB + Arg1.A

    // Bump mapping
    D3DTOP_BUMPENVMAP,                // per pixel env map perturbation
    D3DTOP_BUMPENVMAPLUMINANCE,       // with luminance channel

    // Dot Product
    D3DTOP_DOTPRODUCT3,

    // Triadic ops
    D3DTOP_MULTIPLYADD,   // Arg0 + Arg1*Arg2
    D3DTOP_LERP,          // (Arg0)*Arg1 + (1-Arg0)*Arg2
};

static DWORD enumRTGTexArg[RTGTA_MAX] = 
{
    D3DTA_SELECTMASK,  // mask for arg selector
    D3DTA_DIFFUSE   ,  // select diffuse color (read only)
    D3DTA_CURRENT   ,  // select stage destination register (read/write)
    D3DTA_TEXTURE   ,  // select texture color (read only)
    D3DTA_TFACTOR   ,  // select D3DRS_TEXTUREFACTOR (read only)
    D3DTA_SPECULAR  ,  // select specular color (read only)
    D3DTA_TEMP      ,  // select temporary register color (read/write)
};

static DWORD enumRTGTexCoor[RTGTC_MAX] = 
{
    D3DTSS_TCI_PASSTHRU                   ,
    D3DTSS_TCI_PASSTHRU + 0               ,
    D3DTSS_TCI_PASSTHRU + 1               ,
    D3DTSS_TCI_PASSTHRU + 2               ,
    D3DTSS_TCI_PASSTHRU + 3               ,
    D3DTSS_TCI_PASSTHRU + 4               ,
    D3DTSS_TCI_PASSTHRU + 5               ,
    D3DTSS_TCI_PASSTHRU + 6               ,
    D3DTSS_TCI_PASSTHRU + 7               ,
    D3DTSS_TCI_CAMERASPACENORMAL          ,
    D3DTSS_TCI_CAMERASPACEPOSITION        ,
    D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR,
};

static D3DTEXTURETRANSFORMFLAGS enumRTGTexTransformFlags[RTGTTF_MAX] = 
{
    D3DTTFF_DISABLE,
    D3DTTFF_COUNT1,
    D3DTTFF_COUNT2,
    D3DTTFF_COUNT3,
    D3DTTFF_COUNT4,
    D3DTTFF_PROJECTED,
};

static D3DPRIMITIVETYPE enumRTGPrimitive[] =
{
	D3DPT_TRIANGLELIST  , // RTG_TRIANGLES
	D3DPT_TRIANGLEFAN   , // RTG_TRIANGLE_FAN
	D3DPT_TRIANGLESTRIP , // RTG_TRIANGLE_STRIP
	D3DPT_LINELIST      , // RTG_LINES
	D3DPT_LINESTRIP     , // RTG_LINE_STRIP
	D3DPT_POINTLIST     , // RTG_POINTS
};

static D3DFORMAT enumRTGTextureFormat[RTG_TEXTURE_MAX] = 
{
    D3DFMT_A8R8G8B8,
    D3DFMT_A8R8G8B8,
    D3DFMT_R8G8B8  ,
    D3DFMT_A4R4G4B4,
    D3DFMT_A1R5G5B5,
    D3DFMT_R5G6B5  ,
    D3DFMT_X1R5G5B5,
    D3DFMT_X4R4G4B4,
    D3DFMT_A8,
    D3DFMT_D24X8
};

static D3DLIGHTTYPE enumRTGLightMode[RTG_LIGHT_MAX] = 
{
    D3DLIGHT_POINT          ,
    D3DLIGHT_SPOT           ,
    D3DLIGHT_DIRECTIONAL    ,
};

static D3DTRANSFORMSTATETYPE enumRTGMatrix[RTGTS_MAX+7] =
{
    D3DTS_WORLD     ,
    D3DTS_VIEW      ,
    D3DTS_PROJECTION,
    D3DTS_TEXTURE0  ,
    D3DTS_TEXTURE1  ,
    D3DTS_TEXTURE2  ,
    D3DTS_TEXTURE3  ,
    D3DTS_TEXTURE4  ,
    D3DTS_TEXTURE5  ,
    D3DTS_TEXTURE6  ,
    D3DTS_TEXTURE7  ,
};

static D3DTEXTUREFILTERTYPE enumRTGTextureFilter[RTGTEXF_MAX] = 
{
    D3DTEXF_NONE            ,
    D3DTEXF_POINT           ,
    D3DTEXF_LINEAR          ,
    D3DTEXF_ANISOTROPIC     ,
};

static D3DTEXTUREADDRESS enumRTGTextureAddress[RTGTADD_MAX] = 
{
    D3DTADDRESS_WRAP            ,
    D3DTADDRESS_MIRROR          ,
    D3DTADDRESS_CLAMP           ,
    D3DTADDRESS_BORDER          ,
    D3DTADDRESS_MIRRORONCE      ,
};

static DWORD enumRTGLock[RTGLK_MAX] = 
{
    0                           ,
    D3DLOCK_READONLY            ,
    D3DLOCK_DISCARD             ,
    D3DLOCK_NOOVERWRITE         ,
    D3DLOCK_NOSYSLOCK           ,
};

static D3DCULL enumRTGCull[RTGCULL_MAX] =
{
    D3DCULL_NONE               ,
    D3DCULL_CW                 ,
    D3DCULL_CCW                ,
};

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
