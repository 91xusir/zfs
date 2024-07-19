
#define UPDATE_GEOMETRY_VERTEX		1
#define UPDATE_GEOMETRY_COLOR		2
#define UPDATE_GEOMETRY_TEXCOORD	4

namespace rt_graph {

/*----------------------------------------------------------------------------
-   Class RtgGeometryNode.
----------------------------------------------------------------------------*/

// 保存各种几何图形信息的节点
struct RtgGeometryNode
{
    friend class RtgGeometryFactory;
    RTGGeometryType eType;
    RTGVertexFormat eFormat;
    BYTE*           pVertexBuffer;
    BYTE*           pIndexBuffer;
    int             iVertexCnt;
    int             iIndexCnt;

    bool CopyBufTo(RTGVertexFormat einFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize);
    bool CopyVertexBufTo(RTGVertexFormat einFormat, BYTE* pVertexBuf, int iVBufSize);
    bool CopyIndexBufTo(BYTE* pIndexBuf, int iIBufSize);

    void SetVertex(int iIdx, RtgVertex3 &v3);
    void SetVertex(int iIdx, float x, float y, float z);
    void GetVertex(int iIdx, RtgVertex3 &v3);
    RtgVertex3 *GetVertex(int iIdx);

    void SetNormal(int iIdx, RtgVertex3 &v3);
    void SetNormal(int iIdx, float x, float y, float z);
    void GetNormal(int iIdx, RtgVertex3 &v3);
    RtgVertex3 *GetNormal(int iIdx);

    void SetColor(int iIdx, DWORD dwColor);
    void SetColor(int iIdx, float r, float g, float b, float a=1.0f);
    DWORD* GetColor(int iIdx);

    void SetTexCoord(int iIdx, RtgVertex2 &v2, int iChannel = 0);
    void SetTexCoord(int iIdx, float u, float v, int iChannel = 0);
    void GetTexCoord(int iIdx, float &u, float &v, int iChannel = 0);
    void GetTexCoord(int iIdx, RtgVertex2 &v2, int iChannel = 0);
    float *GetTexCoord(int iIdx, int iChannel = 0);

    void SetIndex(int iIdx, short sValue);
    void GetIndex(int iIdx, short &sValue);

	void Draw(RtgDevice* pDevice);

protected:
    RtgGeometryNode(){};
};

/*----------------------------------------------------------------------------
-   Class RtgGeometryFactory.
----------------------------------------------------------------------------*/
class RtgGeometryFactory
{
public:
    enum ETexCoordMode  // 贴图模式
    {
        TCM_DEFAULT,    // 默认
        TCM_BOX,        // Box
        TCM_PLANE,      // Plane
        TCM_COLUMN,     // 圆柱
        TCM_SPHERE,     // 球
    };
public:
    RtgGeometryFactory();
    ~RtgGeometryFactory();

    void SetGeometryQuality(int iQuality);

    RtgGeometryNode* CreateRect(RTGVertexFormat eFormat, float w, float h, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    RtgGeometryNode* CreateTrape(RTGVertexFormat eFormat, float t, float b, float h, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    RtgGeometryNode* CreateBox(RTGVertexFormat eFormat, float w, float h, float d, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    RtgGeometryNode* CreateCircle(RTGVertexFormat eFormat, float r, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    RtgGeometryNode* CreateCone(RTGVertexFormat eFormat, float w, float h, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    RtgGeometryNode* CreateCylinder(RTGVertexFormat eFormat, float r1, float r2, float h, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    RtgGeometryNode* CreateSphere(RTGVertexFormat eFormat, float fRadius, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    RtgGeometryNode* CreateDonut(RTGVertexFormat eFormat, float or, float ir, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    RtgGeometryNode* CreateArrowhead(RTGVertexFormat eFormat, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    RtgGeometryNode* CreateArrowhead2(RTGVertexFormat eFormat, DWORD dwColorH=0xFFFFFFFF, DWORD dwColorF=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);

    void DestroyGeometry(RtgGeometryNode* pGNode);

    /// 矩形面片
    bool CreateRect(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float w, float h, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    /// 梯形面片
    bool CreateTrape(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float t, float b, float h, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    /// 盒子
    bool CreateBox(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float w, float h, float d, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    /// 圆形面片
    bool CreateCircle(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float fRadius, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    /// 扇形面片
    bool CreateCircleEx(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float fRadius, float rs, float es, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    /// 圆锥体(无底的)
    bool CreateCone(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float w, float h, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    /// 圆台体(无底的)
    bool CreateCylinder(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float r1, float r2, float h, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    /// 圆台体(无底的, 截面是扇形的)
    bool CreateCylinderEx(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float ts, float te, float radius1, float radius2, float height, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    /// 球体
    bool CreateSphere(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float r, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    /// 球体
    bool CreateSphereEx(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float fRadius, float ts, float te, float ps, float pe, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    /// 圆环体
    bool CreateDonut(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float or, float ir, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    /// 扇环体
    bool CreateDonutEx(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float InnerRadius, float OuterRadius, float ts, float te, float ps, float pe, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    /// 箭头体(由两个圆锥体和两个个圆形的面组成)
    bool CreateArrowhead(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);
    /// 箭头体(由一个圆锥体和一个圆形的面组成)
    bool CreateArrowhead2(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, DWORD dwColorH=0xFFFFFFFF, DWORD dwColorF=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);

protected:
    void CreateOneStack(RtgGeometryNode& node, int stack, float InnerRadius, float OuterRadius, float ts, float te, float ps, float pe, int vbase, int ibase, DWORD dwColor=0xFFFFFFFF, ETexCoordMode eMode=TCM_DEFAULT);

    int     m_iPieces;
    int     m_iStacks;
};

/*----------------------------------------------------------------------------
-   Class RtgGeometry.
----------------------------------------------------------------------------*/

class RtgGeometry
{
public:
	static int m_iPieces;
	static int m_iStacks;
	static void SetGeometryQuality(int quality);

public:
	RtgGeometry();
	RtgGeometry(RtgDevice* pDevice);
	~RtgGeometry();

	RtgBuffer *GetVertices(void) { return &m_Vertices; }
	RtgBuffer *GetIndices(void) { return &m_Indices; }

	void ReleaseResource(void);
	void SetResourceType(RTGMemoryType mtype);
	void OffsetVertices(float x, float y, float z);

	void SetVertexFormat(RTGVertexFormat fmt);
	//int  SetVertexFormat(int vsize, int normal, int color, int numtex, int *texlist = NULL);
	//void SetVertexFormat(RtgVertexSpec fmt);

	int CreateResource(int vertices, int faces, int indices);
	int CreateRect(float w, float h);
    int CreateRect(float w, float h, int iSegmentX, int iSegmentY);
	int CreateTrape(float t, float b, float h);
	int CreateBox(float w, float h, float d);
	int CreateParticalCircle(float r, float rs, float es);
	int CreateCircle(float r);
	int CreateCone(float w, float h);
	int CreateParticalCylinder(float ts, float te, float radius1, float radius2, float height);
	int CreateCylinder(float r1, float r2, float h);
	int CreateParticalSphere(float radius, float ts, float te, float ps, float pe);
	int CreateSphere(float r);
	int CreateParticalDonut(float InnerRadius, float OuterRadius, float ts, float te, float ps, float pe);
	int CreateDonut(float or, float ir);
	int CreateSegment(int grids, float xext, float yext);
	void ChangeVertexColor(float r, float g, float b, float a);
	void ChangeVertexColor(int r, int g, int b, int a);
	int GetTangentSpace(int iIdx, RtgVertex3 vec[3]);

	RtgVertexSpec GetVertexFormat(void) { return m_VertexSpec; }

    void SetFVF();
	void SetBumpmapLight(RtgVertex3 &light, RTGLightMode mode = RTG_LIGHT_POINT );
	void FadeSegment(float r, float g, float b);
	void FadeSegment(int r, int g, int b);
	void ChangeTexcoord(float s, float t, int index = 0);
	void ChangeVertexPosition(RtgMatrix16 matrix);
	void Render(RTGPrimitive primitive = RTG_TRIANGLES);
    // 这个函数之前要调用 SetFVF()
	void DrawIndexPrimitive(RTGPrimitive primitive, int vstart, int vcount, int istart, int icount, int pcount);
	void UpdateNode(int node, int mask);
	void Copy(RtgGeometry &src);

private:
	RTGVertexFormat m_eVertexFormat;
    RtgDevice*      m_pDevice;

public:
	int             m_iNumVertices;
	int             m_iNumFaces;
	int             m_iNumIndices;

	RtgBuffer       m_Vertices;
	RtgBuffer       m_Indices;
	RtgVertexSpec   m_VertexSpec;
	RTGMemoryType   m_eMType;

	void CreateOneStack(int stack, float InnerRadius, float OuterRadius, float ts, float te, 
					float ps, float pe, int vbase, int ibase);
};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
