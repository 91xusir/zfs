
#pragma once
namespace rt_graph {


 
int rtgRand(int minValue, int maxValue, int mValue, int mPercent = 60, int mRangeDivisor = 5);

///////////////////////////////////////////////////////////////////////////////////////
// inline 函数

inline float rtg2DDistanceSqr( const float p1[2], const float p2[2] );
inline float rtg2DDistance( const float p1[2], const float p2[2] );
inline float rtgDistanceSqr( const float p1[3], const float p2[3] );
inline float rtgDistance( const float p1[3], const float p2[3] );

inline BYTE rtgEncodeUprightDirection(const RtgMatrix16& m16);
inline void rtgDecodeUprightDirection(RtgMatrix16& m16, BYTE cDir);

inline void TransformCoordArray(RtgVertex3 _dst[], RtgVertex3 _src[], size_t _Num, const RtgMatrix16& _m16)
{
    for (size_t i = 0; i < _Num; ++i)
    {
        float _rhw = 1.f / (_src[i].x * _m16._03 + _src[i].y * _m16._13 + _src[i].z * _m16._23 + _m16._33);
        _dst[i].x = (_src[i].x * _m16._00 + _src[i].y * _m16._10 + _src[i].z * _m16._20 + _m16._30) * _rhw;
        _dst[i].y = (_src[i].x * _m16._01 + _src[i].y * _m16._11 + _src[i].z * _m16._21 + _m16._31) * _rhw;
        _dst[i].z = (_src[i].x * _m16._02 + _src[i].y * _m16._12 + _src[i].z * _m16._22 + _m16._32) * _rhw;
    }
}

// AABB[6] --> x0, y0, z0, x1, y1, z1, 
inline BOOL rtgIsVertexIntersectAABB( const RtgVertex3 &v, const RtgVertex3 &inMin, const RtgVertex3 &inMax )
{
    if ( v.x <  inMin.x ) return FALSE;
    if ( v.x >= inMax.x ) return FALSE;
    if ( v.y <  inMin.y ) return FALSE;
    if ( v.y >= inMax.y ) return FALSE;
    if ( v.z <  inMin.z ) return FALSE;
    if ( v.z >= inMax.z ) return FALSE;
    return TRUE;
}

inline BOOL rtgVertexInAABB(RtgVertex3& v, RtgVertex3& vMin, RtgVertex3& vMax)
{
    if ( v.x <  vMin.x ) return FALSE;
    if ( v.x >= vMax.x ) return FALSE;
    if ( v.y <  vMin.y ) return FALSE;
    if ( v.y >= vMax.y ) return FALSE;
    if ( v.z <  vMin.z ) return FALSE;
    if ( v.z >= vMax.z ) return FALSE;
    return TRUE;
}

class RtgRangeAABB
{
public:
    RtgVertex3  vMin;
    RtgVertex3  vMax;
    RtgRangeAABB()  { vMin.x=vMin.y=vMin.z=vMax.x=vMax.y=vMax.z=0.f;}
    inline void Extend(const RtgVertex3& v)
    {
        if (v.x<vMin.x) vMin.x = v.x;
        else if (v.x>vMax.x) vMax.x = v.x;
        if (v.y<vMin.y) vMin.y = v.y;
        else if (v.y>vMax.y) vMax.y = v.y;
        if (v.z<vMin.z) vMin.z = v.z;
        else if (v.z>vMax.z) vMax.z = v.z;
    }
    inline void Extend(const RtgRangeAABB& aabb)
    {
        if (aabb.vMin.x<vMin.x) vMin.x = aabb.vMin.x;
        if (aabb.vMin.y<vMin.y) vMin.y = aabb.vMin.y;
        if (aabb.vMin.z<vMin.z) vMin.z = aabb.vMin.z;
        if (aabb.vMax.x>vMax.x) vMax.x = aabb.vMax.x;
        if (aabb.vMax.y>vMax.y) vMax.y = aabb.vMax.y;
        if (aabb.vMax.z>vMax.z) vMax.z = aabb.vMax.z;
    }
    inline RtgVertex3 Pos()
    {
        return RtgVertex3((vMax.x+vMin.x)/2.f, (vMax.y+vMin.y)/2.f, (vMax.z+vMin.z)/2.f);
    }
    inline RtgVertex3 Ext()
    {
        return RtgVertex3((vMax.x-vMin.x)/2.f, (vMax.y-vMin.y)/2.f, (vMax.z-vMin.z)/2.f);
    }
};

class RtgAABB
{
public:
    RtgVertex3  vExt;
    RtgVertex3  vPos;
    RtgAABB()                                       { vExt.x=vExt.y=vExt.z=vPos.x=vPos.y=vPos.z=0.f;}
    RtgAABB(const RtgVertex3& _pos, const RtgVertex3& _ext)
        : vPos(_pos),
          vExt(_ext)
    {
    }
    inline void Zero()                              { vPos.Zero(); vExt.Zero(); }
    inline void CopyAABB(RtgAABB& aabb)             { vExt = aabb.vExt; vPos = aabb.vPos; }
    // inline RtgAABB & operator= ( RtgAABB& aabb )    { CopyAABB(aabb); return *this; }
    inline void FromRangeAABB(RtgRangeAABB& aabb)   { vPos = aabb.Pos(); vExt = aabb.Ext(); }
    inline void ToRangeAABB(RtgRangeAABB& aabb)     { aabb.vMin = vPos-vExt; aabb.vMax = vPos+vExt; }
    inline RtgVertex3 Min() const                        { return vPos-vExt; }
    inline RtgVertex3 Max() const                         { return vPos+vExt; }
    inline BOOL IntersectAABB(const RtgAABB& aabb)
    {
        return ( Abs(vPos[0] - aabb.vPos[0]) < (vExt[0] + aabb.vExt[0]) &&
            Abs(vPos[1] - aabb.vPos[1]) < (vExt[1] + aabb.vExt[1]) &&
            Abs(vPos[2] - aabb.vPos[2]) < (vExt[2] + aabb.vExt[2]) );
    }
    inline BOOL IsVertexIntersect(const RtgVertex3 &v)  { return rtgIsVertexIntersectAABB(v, Min(), Max());}
	friend RtArchive& operator<<(RtArchive &Ar, RtgAABB &ab) 
	{
		return Ar << ab.vExt << ab.vPos;
	}

    void GetMin(RtgVertex3* _min) const
    {
        (*_min) = vPos-vExt;
    }


    void GetMax(RtgVertex3* _max) const
    {
        (*_max) = vPos+vExt;
    }
};


///////////////////////////////////////////////////////////////////////////////////////
// Area 面积计算

float rtg2DTriangleArea   ( const float v0[2], const float v1[2], const float v2[2] );
float rtg2DTriangleAreaX2 ( const float v0[2], const float v1[2], const float v2[2] );
float rtgTriangleArea     ( const float v0[3], const float v1[3], const float v2[3] );
float rtgTriangleTiltAngle( const float v0[3], const float v1[3], const float v2[3] );

// 由一个由位置和边长组成的BoundBox生成这个BoundBox的8个顶点
void rtgGetBox(RtgVertex3 &vHalfExt, RtgMatrix16 &m16, RtgVertex3 p[8]);





///////////////////////////////////////////////////////////////////////////////////////
// inline 实现

// Calculate arc tangent in degree
inline float rtg2DDistanceSqr( const float p1[2], const float p2[2] )
{
    float xSq = p1[0] - p2[0];
    float ySq = p1[1] - p2[1];
    return xSq*xSq + ySq*ySq;
}

// Calculate arc tangent in degree
inline float rtg2DDistance( const float p1[2], const float p2[2] )
{
    float xSq = p1[0] - p2[0];
    float ySq = p1[1] - p2[1];
    return fsqrt( xSq*xSq + ySq*ySq );
}

// Calculate arc tangent in degree
inline float rtgDistanceSqr( const float p1[3], const float p2[3] )
{
    float xSq = p1[0] - p2[0];
    float ySq = p1[1] - p2[1];
    float zSq = p1[2] - p2[2];
    return xSq*xSq + ySq*ySq + zSq*zSq;
}

// Calculate arc tangent in degree
inline float rtgDistance( const float p1[3], const float p2[3] )
{
    float xSq = p1[0] - p2[0];
    float ySq = p1[1] - p2[1];
    float zSq = p1[2] - p2[2];
    return fsqrt(xSq*xSq + ySq*ySq + zSq*zSq);
}

// 把仅仅Z轴旋转的矩阵旋转压缩成一个BYTE, 通常用于人物的方向，rtgDecodeUprightDirection用于还原
inline BYTE rtgEncodeUprightDirection(const RtgMatrix16& m16)
{
    BYTE cDir = 0;
    if (m16._00<0.f) cDir |= 0x80;
    if (m16._01<0.f) cDir |= 0x40;
    cDir |= Clamp((int)(fabs(m16._00)*0x3F), 0, 0x3F);
    return cDir;
}

inline void rtgDecodeUprightDirection(RtgMatrix16& m16, BYTE cDir)
{
    m16._00 = ((float)(cDir&0x3F))/0x3F;
    if (cDir&0x80)  m16._00 = -m16._00;
    m16._01 = sqrt(1-m16._00*m16._00);
    if (cDir&0x40)  m16._01 = -m16._01;
    m16._10 = - m16._01;
    m16._11 =   m16._00;
    m16._02 = m16._12 = m16._20 = m16._21 = 0.f;
    m16._22 = 1.f;
    m16.NormalizeMatrix();
}


//
// add by zxy
//

inline bool rtgRayTriIntersect(const RtgVertex3& ray0, 
                               const RtgVertex3& ray1,
                               const RtgVertex3& tri0,
                               const RtgVertex3& tri1,
                               const RtgVertex3& tri2,
                               RtgVertex3* vInter)
{
    RtgVertex3 u = tri1 - tri0;
    RtgVertex3 v = tri2 - tri0;
    RtgVertex3 n = u.Cross(v);
    RtgVertex3 d = ray1 - ray0;
    RtgVertex3 w = ray0 - tri0;

    float a = -n.Dot(w);
    float b = n.Dot(d);

    if (fabs(b) < 0.00000001f)
        return false;

    float r = a / b;

    if (r < 0.f)
        return false;

    (*vInter) = ray0 + r * d; 

    float uu = u.Dot(u);
    float uv = u.Dot(v);
    float vv = v.Dot(v);
    RtgVertex3 t = (*vInter) - tri0;
    float tu = t.Dot(u);
    float tv = t.Dot(v);
    float y = uv * uv - uu * vv;
    float s = (uv * tv - vv * tu) / y;

    if (s < 0.f || s > 1.f)       
        return false;

    float q = (uv * tu - uu * tv) / y;

    if (q < 0.f || (s + q) > 1.0f)
        return false;

    return true;
}

//
// add by zxy
//


inline bool rtgRegTriIntersect(const RtgVertex3& reg0, 
                               const RtgVertex3& reg1,
                               const RtgVertex3& tri0,
                               const RtgVertex3& tri1,
                               const RtgVertex3& tri2,
                               RtgVertex3* vInter)
{
    RtgVertex3 u = tri1 - tri0;
    RtgVertex3 v = tri2 - tri0;
    RtgVertex3 n = u.Cross(v);
    RtgVertex3 d = reg1 - reg0;
    RtgVertex3 w = reg0 - tri0;

    float a = -n.Dot(w);
    float b = n.Dot(d);

    if (fabs(b) < 0.00000001f)
        return false;

    float r = a / b;

    if (r < 0.f)
        return false;

    (*vInter) = reg0 + r * d; 

    float uu = u.Dot(u);
    float uv = u.Dot(v);
    float vv = v.Dot(v);
    RtgVertex3 t = (*vInter) - tri0;
    float tu = t.Dot(u);
    float tv = t.Dot(v);
    float y = uv * uv - uu * vv;
    float s = (uv * tv - vv * tu) / y;

    if (s < 0.f || s > 1.f)       
        return false;

    float q = (uv * tu - uu * tv) / y;

    if (q < 0.f || (s + q) > 1.0f)
        return false;

    u = (*vInter) - reg0;
    v = (*vInter) - reg1;

    if (u.Dot(v) > 0.f)
        return false;

    return true;
}

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
