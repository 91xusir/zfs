
///////////////////////////////////////////////////////////////////////////////////////
// ��ײ���
//      AABB ( Axis-Aligned Bounding Box ) ��������ƽ�е�Bounding Box
//      AABB[6] -> x0, y0, z0, x1, y1, z1
//      OBB ( Oriented Bounding Box )
//      basis  : u, v, w axes �ֱ���x,y,z������
//      extents: extents ( half-length) in u, v, w
//      pos    : center position �м�λ��
//  2D
//      �����Ƿ��ھ����ڲ�  rtg2DIsVertexIntersectRectangle
//      �����߶��Ƿ��ཻ    rtg2DIsLineIntersectLine
//      �߶��Ƿ�������ཻ  rtg2DIsLineIntersectRectangle
//      �㵽ֱ�ߵľ���      rtg2DPoint2LineDistance         ���ݲ�������ѡ���ǵ��߶λ���ֱ�ߵľ���
//  3D
//      �����Ƿ���AABB�ڲ�  rtgIsVertexIntersectAABB
//      �߶��Ƿ���AABB�ཻ  rtgIsLineIntersectAABB          ���Է��ؽ����λ��(0.0 - 1.0)
//      �߶��Ƿ�������ཻ  rtgIsLineIntersectSquareXY      �þ����� Z �ᴹֱ
//                          rtgIsLineIntersectSquareXZ      �þ����� Y �ᴹֱ
//                          rtgIsLineIntersectSquareYZ      �þ����� Z �ᴹֱ
//      AABB�Ƿ���AABB�ڲ�  rtgIsAABBInAABB
//      �߶����������ཻ    rtgIsLineIntersectTriangle
//      �������������ཻ    rtgIsRayHitTriangle             ���Է��ؽ���ľ���
//      �߶��Ƿ���AABB�ཻ  rtgIsLineIntersectHollowAABB    �߶β��ܰ�����AABB��,���Է��ؽ����λ��(0.0 - 1.0)
//      AABB�Ƿ��������    rtgIsAABBIntersectVertex
//      AABB���߶��ཻ      rtgIsAABBIntersectLine
//      AABB���������ཻ    rtgIsAABBIntersectTriangle
//      AABB�����ཻ        rtgIsAABBIntersectSphere
//      �߶������ཻ        rtgIsLineIntersectSphere
//      AABB��AABB�ཻ      rtgIsAABBIntersectAABB
//      OBB��OBB�ཻ        rtgIsOBBIntersectOBB
//      OBB���������ཻ     rtgIsOBBIntersectTriangle
//      ���������������ཻ  rtgIsTriangleIntersectTriangle
//
//      ��AABB�ü�ֱ��      rtgAABBCutLine                  ���زü��������߶�

namespace rt_graph {

BOOL rtg2DIsVertexIntersectRectangle( const float v[2], const float rect[4] );
BOOL rtg2DIsLineIntersectLine( const float v0[2], const float v1[2], const float u0[2], const float u1[2] );
BOOL rtg2DIsLineIntersectRectangle( const float v0[2], const float v1[2], const float rect[4] );
BOOL rtg2DRectangleCutLine(const float* v0, const float* v1, const float* inMin, const float* inMax, float* outR0, float* outR1);
// ��v��ֱ��(l1, l2)����̾���, bLineΪtrue��ʾֻҪ����㵽ֱ�ߵľ��룬Ϊfalse��ʾ�㵽�߶εľ���
float rtg2DPoint2LineDistance(RtgVertex2* l1, RtgVertex2* l2, RtgVertex2* v, bool bLine);

BOOL rtgIsAABBInAABB(const float AABB0[6], const float AABB1[6]);

inline BOOL rtgIsVertexIntersectAABB( const RtgVertex3 &v, const RtgVertex3 &inMin, const RtgVertex3 &inMax );

BOOL rtgIsLineIntersectSphere  (const float fLine0[3], const float fLine1[3], const float fPos[3], float r_square);
BOOL rtgIsLineIntersectAABB( const float v0[3], const float v1[3], float AABB[6] );
BOOL rtgIsLineIntersectAABB( const RtgVertex3 &v0, const RtgVertex3 &v1, const RtgVertex3 &inMin, const RtgVertex3 &inMax );
BOOL rtgIsLineIntersectAABB( const RtgVertex3 &v0, const RtgVertex3 &v1, const RtgVertex3 &inMin, const RtgVertex3 &inMax, float *r1, float *r2 );
BOOL rtgIsLineIntersectSquareXY( const float v0[3], const float v1[3], const float square[6] );
BOOL rtgIsLineIntersectSquareXZ( const float v0[3], const float v1[3], const float square[6] );
BOOL rtgIsLineIntersectSquareYZ( const float v0[3], const float v1[3], const float square[6] );
BOOL rtgIsLineIntersectTriangle( const float v0[3], const float v1[3], const float t1[3], const float t2[3], const float t3[3]);
inline BOOL rtgIsLineIntersectTriangle( const float v0[3], const float v1[3], const float triangle[9] )     { return rtgIsLineIntersectTriangle(v0, v1, triangle, triangle+3, triangle+6);}
BOOL rtgIsRayHitTriangle( const float p[3], const float dir[3], const float v1[3], const float v2[3], const float v3[3], float *t );
inline BOOL rtgIsRayHitTriangle( const float p[3], const float dir[3], const float triangle[9], float *t )  { return rtgIsRayHitTriangle(p, dir, triangle, triangle+3, triangle+6, t);}
BOOL rtgIsLineIntersectHollowAABB( const RtgVertex3 &v0, const RtgVertex3 &v1, const RtgVertex3 &inMin, const RtgVertex3 &inMax, float *r1=NULL, float *r2=NULL );
BOOL rtgAABBCutLine( const RtgVertex3 &v0, const RtgVertex3 &v1, const RtgVertex3 &inMin, const RtgVertex3 &inMax, RtgVertex3 &outR0, RtgVertex3 &outR1 );

// AABB ( Axis-Aligned Bounding Box )
// Ref: Real-Time Rendering, Tomas Moller, Eric Hanies
//      A K Peters
//      Page 292
BOOL rtgIsAABBIntersectVertex  ( const float AABB[6], const float v[3] );
BOOL rtgIsAABBIntersectLine    ( const float AABB[6], const float v0[3], const float v1[3] );
BOOL rtgIsAABBIntersectTriangle( const float AABB[6], const float v1[3], const float v2[3], const float v3[3] );
BOOL rtgIsAABBIntersectSphere  ( const float AABB[6], const float pos[3], float r_square );
inline BOOL rtgIsAABBIntersectTriangle( const float AABB[6], const float triangle[9] )  { return rtgIsAABBIntersectTriangle(AABB, triangle, triangle+3, triangle+6);}

BOOL rtgIsAABBIntersectVertex  ( const float ext[3], const float pos[3], const float v[3] );
BOOL rtgIsAABBIntersectLine    ( const float ext[3], const float pos[3], const float v0[3], const float v1[3] );
BOOL rtgIsAABBIntersectTriangle( const float ext[3], const float pos[3], const float v1[3], const float v2[3], const float v3[3] );
BOOL rtgIsAABBIntersectSphere  ( const float ext[3], const float aabbPos[3], const float pos[3], float r_square );
inline BOOL rtgIsAABBIntersectTriangle( const float ext[3], const float pos[3], const float triangle[9] )  { return rtgIsAABBIntersectTriangle(ext, pos, triangle, triangle+3, triangle+6);}

// AABB ( Axis-Aligned Bounding Box)
// extents: extents ( half-length) in u, v, w
// pos    : center position of OBB
BOOL rtgIsAABBIntersectAABB( const RtgVertex3 &extentsA, const RtgVertex3 &posA,
                                        const RtgVertex3 &extentsB, const RtgVertex3 &posB );

BOOL rtgIsOBBIntersectOBB( const RtgVertex3 &extentsA, const RtgMatrix16 &mA, const RtgVertex3 &extentsB, const RtgMatrix16 &mB );

// OBB ( Oriented Bounding Box )
// extents: extents ( half-length) in u, v, w
// pos    : center position of OBB
// basis  : u, v, w axes of OBB
BOOL rtgIsOBBIntersectOBB( const RtgVertex3 &extentsA, const RtgVertex3 &posA, const RtgVertex3 basisA[3], 
                                        const RtgVertex3 &extentsB, const RtgVertex3 &posB, const RtgVertex3 basisB[3] );
BOOL rtgIsTriangleIntersectTriangle(const float v1[3], const float v2[3], const float v3[3],
                                        const float u1[3], const float u2[3], const float u3[3]);
BOOL rtgIsOBBIntersectTriangle(const RtgVertex3 &extents, const RtgVertex3 &pos, const RtgVertex3 basis[3],
                                        const float v1[3], const float v2[3], const float v3[3]);
inline BOOL rtgIsOBBIntersectTriangle(const RtgVertex3 &extents, const RtgVertex3 &pos, const RtgVertex3 basis[3],
                                        const float triangle[9]);

// �����Ľ���, ����ķ���Ϊ ax+by+cz+d=0
// [IN]  p0, p1, p2 3����ķ��̲���
// [OUT] pIntersection ���������
// ����: TRUE�н���, FALSEû�н���
BOOL rtgGetPlaneIntersect(float p0[4], float p1[4], float p2[4], float pIntersection[3]);
// ����ߵĽ���, ���ߵķ���Ϊ ax+by+c=0
// [IN]  p0, p1 2���ߵķ��̲���
// [OUT] pIntersection ���������
// ����: TRUE�н���, FALSEû�н���
BOOL rtgGet2dLineIntersect(float p0[3], float p1[3], float pIntersection[2]);









//////////////////////////////////////////////////////////////////////////
// ʵ��

inline BOOL rtgIsOBBIntersectTriangle(const RtgVertex3 &extents, const RtgVertex3 &pos, const RtgVertex3 basis[3], 
                                         const float triangle[9])
{
    return rtgIsOBBIntersectTriangle(extents, pos, basis, triangle, triangle+3, triangle+6);
}

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
