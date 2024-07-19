
namespace rt_graph {

struct RtgViewWindowInfo
{
    RTGViewWindowMode   eViewMode;      // ��ʾģʽ
    bool                bEnable;        // �������͵Ĵ����Ƿ���Ч
    bool                bResize;        // ��������ʾģʽ��ʱ���Ƿ���ݵ�ǰֵ���ô��ڴ�С
    bool                bTopwindow;
    bool                bFullscreen;
    int                 iPosX;
    int                 iPosY;
    DWORD               dwWidth;        // ���
    DWORD               dwHeight;       // �߶�
    DWORD               dwDepth;        // λ��
    DWORD               dwRefreshRate;  // ˢ����
    RTHWND              hWnd;           // ���ھ��
    DWORD               dwWndStyle;
};

struct RtgVertexSpec
{
    int iNumVertexSize;
    int iNumVertexCoords;
    int iVertexOffset;
    int iNormalOffset;
    int iColorOffset;
    int iTexOffset;
    int iNumTexCoords;
    int iTexCoordList[RTG_MAX_MULTITEXTURES];
    int TexOffset[RTG_MAX_MULTITEXTURES];
    RtgVertexSpec(void)
    {
        iNumVertexSize = sizeof(RtgVertexSpec);
        iVertexOffset = iNormalOffset = iColorOffset = iTexOffset = -1;
    }
};

struct RtgDeviceAbility
{
    // device ability
    int             bFullScreneAntialiasing;    // ȫ�������

    // Texture ability
    int             iMaxTextureWidth;           // �����ͼ���
    int             iMaxTextureHeight;          // �����ͼ�߶�
    int             iMaxMultiTextures;          // һ����Ⱦ������ͼ����
    int             iMaxTextureStages;          // һ����Ⱦ����Stage��
    bool            bSquareTexture;             // �Ƿ���Ҫ���ε���ͼ
    bool            bEnvBumpmap;                // �Ƿ�֧��Bumpmap
    bool            bDynamicTextures;           // �Ƿ�֧�ֶ�̬��ͼ
    bool            bDot3ProductBumpmap;        // �Ƿ�֧��Dot3ProductBumpmap
    bool            bTextureOpMultiplyAdd;      // 
    bool            bTextureAddressBorder;
    bool            bTextureAddressClamp;
    bool            bTextureAddressWrap;
    bool            bTextureMipmap;

    // vertex ability
    int             iMaxPrimitiveCount;         // ������������
    int             iMaxVertexIndex;            // ����������
    int             iMaxStreams;                // ����
    int             iMaxStreamStride;           // ����
    bool            bVertexBuffer;              // �Ƿ�֧��VertexBuffer
    bool            bVertexShader;              // �Ƿ�֧��VertexShader
    bool            bPixelShader;               // �Ƿ�֧��PixelShader
};

struct RtPerformance
{
    DWORD Time_Update;
    DWORD Time_Render;
    DWORD Used_BuffSize;
    DWORD Used_TextSize;
    DWORD Proc_FaceNum;
    DWORD Draw_Call;
    DWORD TerrRender;
    DWORD ActrRender;
    DWORD ActrUpdate;
    DWORD skinRender;
    DWORD skinUpdate;
    DWORD efftRender;
    DWORD efftUpdate;
    DWORD partNum;
    DWORD Ui_Update;
    DWORD Ui_Render;
    DWORD ActorNum;
    DWORD RenderObj_Num;
    DWORD RenderTile_Num;
    DWORD LogicProc;

    RtPerformance()
        : ActorNum(0),
          RenderObj_Num(0),
          RenderTile_Num(0)
    {
        Reset();
    }

    void Reset()
    {
        Time_Update = 0;
        Time_Render = 0;
        Proc_FaceNum = 0;
        Draw_Call = 0;
        TerrRender = 0;
        Ui_Update = 0;
        Ui_Render = 0;
        ActrRender = 0;
        ActrUpdate = 0;
        skinRender = 0;
        skinUpdate = 0;
        efftRender = 0;
        efftUpdate = 0;
        partNum = 0;
        LogicProc = 0;
    }

};

struct RtgRenderOption
{
    bool bTerrain;
    bool bObject;
    bool bGrass;
    bool bEffect;
    bool bTerrainNormal;
    bool bObjectNormal;

    RtgRenderOption()
        : bTerrain(true),
          bObject(true),
          bGrass(true),
          bEffect(true),
          bTerrainNormal(false),
          bObjectNormal(false)
    { 
    }

};

struct RtgFog
{
	RtgVertex3 Col;
	float Start;
	float End;
};

struct RtgLight
{
    RTGLightMode    eMode;                  // �ƹ�ģʽ (���,ƽ�й�,�۹�)
    RtgVertex3      vDiffuse;
    RtgVertex3      vSpecular;
    RtgVertex3      vAmbient;
    RtgVertex3      vPosition;
    RtgVertex3      vDirection;
    float           fRange;
    float           fFallOff;
    float           fTheta;
    float           fPhi;
    float           fConstantAttenuation;
    float           fLinearAttenuation;
    float           fQuadraticAttenuation;
    RtgLight(void)
    {
        memset(this, 0, sizeof(RtgLight));
        fConstantAttenuation = 1.0f;
    }
    void SetAttenuation(float a, float b, float c)
    {
        fConstantAttenuation = a;
        fLinearAttenuation = b;
        fQuadraticAttenuation = c;
    }
    friend RtArchive& operator<<( RtArchive &Ar, RtgLight &lt) 
    {
        Ar.Serialize(&lt,sizeof(RtgLight));
        return Ar;
    }
    long Serialize(RtArchive& ar)
    {
        ar.Serialize(this,sizeof(RtgLight));
        return TRUE;
    }
};

struct RtgVertexVC
{
    RtgVertex3 v;
    DWORD      c;
};

struct RtgVertexVN
{
    RtgVertex3 v;
    RtgVertex3 n;
};

struct RtgVertexVNC
{
    RtgVertex3 v;
    RtgVertex3 n;
    DWORD      c;
};

struct RtgVertexVT
{
    RtgVertex3 v;
    RtgVertex2 t;
};

struct RtgVertexVT2
{
    RtgVertex3 v;
    RtgVertex2 t0;
    RtgVertex2 t1;
};

struct RtgVertexVNT
{
    RtgVertex3 v;
    RtgVertex3 n;
    RtgVertex2 t;
};

struct RtgVertexVNT2
{
    RtgVertex3 v;
    RtgVertex3 n;
    RtgVertex2 t0;
    RtgVertex2 t1;
};

struct RtgVertexVCT
{
    RtgVertex3 v;
    DWORD      c;
    RtgVertex2 t;
};

struct RtgVertexVCT2
{
    RtgVertex3 v;
    DWORD      c;
    RtgVertex2 t0;
    RtgVertex2 t1;
};

struct RtgVertexVNCT
{
    RtgVertex3 v;
    RtgVertex3 n;
    DWORD      c;
    RtgVertex2 t;
};

struct RtgVertexVNCT2
{
    RtgVertex3 v;
    RtgVertex3 n;
    DWORD      c;
    RtgVertex2 t0;
    RtgVertex2 t1;
};

struct RtgDisplayMode
{
    DWORD   dwWidth;
    DWORD   dwHeight;
    DWORD   dwDepth;
    DWORD   dwRefreshRate;
};

struct RtgNVertexVC
{
    RtgVertex4 v;
    DWORD      c;
    RtgNVertexVC(){};
    RtgNVertexVC(float x, float y, DWORD dwColor) : v(x, y, 0, 1) , c(dwColor) {}
};

struct RtgNVertexVCT
{
    RtgVertex4 v;
    DWORD      c;
    RtgVertex2 t;
    RtgNVertexVCT(){};
    RtgNVertexVCT(float x, float y, DWORD dwColor, float tx, float ty) : v(x, y, 0, 1) , c(dwColor), t(tx, ty) {}
};

struct RtgNVertexVT
{
	RtgVertex4 v;
	RtgVertex2 t;
	RtgNVertexVT(){};
	RtgNVertexVT(float x, float y, float tx, float ty) : v(x, y, 0, 1) , t(tx, ty){}
};

struct RtgNVertexVCT2
{
    RtgVertex4 v;
    DWORD      c;
    RtgVertex2 t0;
    RtgVertex2 t1;
    RtgNVertexVCT2(){};
    RtgNVertexVCT2(float x, float y, DWORD dwColor, float tx1, float ty1, float tx2, float ty2) : v(x, y, 0, 1) , c(dwColor), t0(tx1, ty1), t1(tx2, ty2) {}
};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
