
#include "rtg_graph_inter.h"

namespace rt_graph {

#ifdef _DEBUG
static int g_iCntAABBIntersectTriangle = 0;
static int g_iCntLineIntersectTriangle = 0;
static int g_iCntOBBIntersectOBB = 0;
static int g_iCntRayHitTriangle = 0;
#endif

//
// Hit Test
//

BOOL rtg2DIsVertexIntersectRectangle( const float v[2], const float rect[4] )
{
    if( v[0] < rect[0] ) return FALSE;
    if( v[0] > rect[2] ) return FALSE;
    if( v[1] < rect[1] ) return FALSE;
    if( v[1] > rect[3] ) return FALSE;
    return TRUE;
}

const int CLOCKWISE = -1;
const int COUNTER_CLOCKWISE = 1;
const int LINE = 0;

int check_tri_clock_dir(const float* pt1, const float* pt2, const float* pt3)
{
    float test;
    test = (((pt2[0] - pt1[0])*(pt3[1] - pt1[1])) - ((pt3[0] - pt1[0])*(pt2[1] - pt1[1]))); 
    if (test > 0) return COUNTER_CLOCKWISE;
    else if(test < 0) return CLOCKWISE;
    else return LINE;
}

BOOL rtg2DIsLineIntersectLine( const float *v0, const float *v1, const float *u0, const float *u1 )
{
    int test1_a, test1_b, test2_a, test2_b;

    test1_a = check_tri_clock_dir(v0, v1, u0);
    test1_b = check_tri_clock_dir(v0, v1, u1);
    if (test1_a != test1_b)
    {
        test2_a = check_tri_clock_dir(u0, u1, v0);
        test2_b = check_tri_clock_dir(u0, u1, v1);
        if (test2_a != test2_b)
        {
            return TRUE;
        }
    }
    return FALSE;	
}
                
BOOL rtg2DIsLineIntersectRectangle( const float *v0, const float *v1, const float *rect )
{
    if( rtg2DIsVertexIntersectRectangle( v0, rect ) ) return TRUE;
    if( rtg2DIsVertexIntersectRectangle( v1, rect ) ) return TRUE;

    if( rtg2DIsLineIntersectLine( v0, v1, rect, rect+2 ) ) return TRUE;
    float f[4];
    f[0] = rect[0];
    f[1] = rect[3];
    f[2] = rect[2];
    f[3] = rect[1];
    if( rtg2DIsLineIntersectLine( v0, v1, f, f+2 ) ) return TRUE;

    return FALSE;
}

BOOL rtg2DRectangleCutLine(const float* v0, const float* v1, const float* inMin, const float* inMax, float* outR0, float* outR1)
{
    if (   (v0[0]>inMin[0]) && (v0[0]<inMax[0]) && (v0[1]>inMin[1]) && (v0[1]<inMax[1])
        && (v1[0]>inMin[0]) && (v1[0]<inMax[0]) && (v1[1]>inMin[1]) && (v1[1]<inMax[1]))
    {
        outR0[0] = v0[0]; outR0[1] = v0[1]; outR1[0] = v1[0]; outR1[1] = v1[1];
        return TRUE;
    }

    if ((v0[0]<inMin[0]) && (v1[0]<inMin[0])) return FALSE;
    if ((v0[0]>inMax[0]) && (v1[0]>inMax[0])) return FALSE;
    if ((v0[1]<inMin[1]) && (v1[1]<inMin[1])) return FALSE;
    if ((v0[1]>inMax[1]) && (v1[1]>inMax[1])) return FALSE;

    float m, n;
    outR0[0] = v0[0];
    outR0[1] = v0[1];
    outR1[0] = v1[0];
    outR1[1] = v1[1];

    // x
    m = outR1[0]-outR0[0]; // 长度
    if (!IsZero(m))
    {
        n = (inMin[0] - outR0[0]) / m;
        if(n>=0.0f && n<=1.0f)
        {
            if (outR0[0]<inMin[0])
            {
                outR0[0] = inMin[0];
                outR0[1] = outR0[1]+(outR1[1]-outR0[1])*n;
            }else
            {
                outR1[0] = inMin[0];
                outR1[1] = outR0[1]+(outR1[1]-outR0[1])*n;
            }
        }
    }

    m = outR1[0]-outR0[0]; // 长度
    if (!IsZero(m))
    {
        n = (inMax[0] - outR0[0]) / m;
        if(n>=0.0f && n<=1.0f)
        {
            if (outR0[0]>=inMax[0])
            {
                outR0[0] = inMax[0];
                outR0[1] = outR0[1]+(outR1[1]-outR0[1])*n;
            }else
            {
                outR1[0] = inMax[0];
                outR1[1] = outR0[1]+(outR1[1]-outR0[1])*n;
            }
        }
    }

    if ((v0[1]<inMin[1]) && (v1[1]<inMin[1])) return FALSE;
    if ((v0[1]>inMax[1]) && (v1[1]>inMax[1])) return FALSE;

    // y
    m = outR1[1] - outR0[1];
    if (!IsZero(m))
    {
        n = (inMin[1] - outR0[1]) / m;
        if(n>=0.0f && n<=1.0f)
        {
            if (outR0[1]<inMin[1])
            {
                outR0[0] = outR0[0]+(outR1[0]-outR0[0])*n;
                outR0[1] = inMin[1];
            }else
            {
                outR1[0] = outR0[0]+(outR1[0]-outR0[0])*n;
                outR1[1] = inMin[1];
            }
        }
    }

    m = outR1[1] - outR0[1];
    if (!IsZero(m))
    {
        n = (inMax[1] - outR0[1]) / m;
        if(n>=0.0f && n<=1.0f)
        {
            if (outR0[1]>=inMax[1])
            {
                outR0[0] = outR0[0]+(outR1[0]-outR0[0])*n;
                outR0[1] = inMax[1];
            }else
            {
                outR1[0] = outR0[0]+(outR1[0]-outR0[0])*n;
                outR1[1] = inMax[1];
            }
        }
    }
    return TRUE;
}

// ax + by + c = 0;
float rtg2DPoint2LineDistance(RtgVertex2* l1, RtgVertex2* l2, RtgVertex2* v, bool bLine)
{
    if (!bLine)
    {
        RtgVertex3 t1, t2;
        t1.Set(l1->x - v->x, l1->y - v->y, 0.f);
        t2.Set(l1->x - l2->x, l1->y - l2->y, 0.f);
        if (t1.Dot(t2)<0) return t1.Distance(t2);
        t1.Set(l2->x - v->x, l2->y - v->y, 0.f);
        t2.Set(l2->x - l1->x, l2->y - l1->y, 0.f);
        if (t1.Dot(t2)<0) return t1.Distance(t2);
    }
    float a, b, c, d;
    if (l1->y == l2->y)
    {
        a = 0;
        b = -1;
        c = l1->y;
    }else
    {
        a = (l1->y - l2->y)/(l1->x - l2->x);
        b = -1;
        c = l1->y - a * l1->x;
    }
    d = fabs(a*v->x+b*v->y+c) / fsqrt(a*a + b*b);
    return d;
}

BOOL rtgIsAABBInAABB(const float AABB0[6], const float AABB1[6])
{
    if( AABB0[0] < AABB1[0] ) return FALSE;
    if( AABB0[0] > AABB1[3] ) return FALSE;
    if( AABB0[1] < AABB1[1] ) return FALSE;
    if( AABB0[1] > AABB1[4] ) return FALSE;
    if( AABB0[2] < AABB1[2] ) return FALSE;
    if( AABB0[2] > AABB1[5] ) return FALSE;

    if( AABB0[3] < AABB1[0] ) return FALSE;
    if( AABB0[3] > AABB1[3] ) return FALSE;
    if( AABB0[4] < AABB1[1] ) return FALSE;
    if( AABB0[4] > AABB1[4] ) return FALSE;
    if( AABB0[5] < AABB1[2] ) return FALSE;
    if( AABB0[5] > AABB1[5] ) return FALSE;
    return TRUE;
}

BOOL rtgIsLineIntersectSquareXY( const float *v0, const float *v1, const float *square )
{
    float dz = v1[2]-v0[2];

    if( IsZero(dz) ) // Line and plane are in parallel
    {
        dz = v0[2]-square[2];
        if( IsZero(dz) ) // line in in the plane
        {
            float rectangle[4];
            rectangle[0] = square[0];
            rectangle[1] = square[1];
            rectangle[2] = square[3];
            rectangle[3] = square[4];
            if( rtg2DIsLineIntersectRectangle( v0, v1, rectangle ) )
            {
                return TRUE;
            }
        }
        return FALSE;
    }

    // Check if v0 & v1 are at oppsite side of the plane
    if( v0[2]<square[2] )
    {
        if( v1[2]<square[2] ) return FALSE;
    }
    else if( v0[2]>square[2] )
    {
        if( v1[2]>square[2] ) return FALSE;
    }

    // Check if intersect point inside Square
    float t = (square[2]-v0[2])/dz;
    float x = v0[0] + (v1[0]-v0[0])*t;
    float y = v0[1] + (v1[1]-v0[1])*t;

    if( x<square[0] || x>square[3] ) return FALSE;
    if( y<square[1] || y>square[4] ) return FALSE;

    return TRUE;
}

BOOL rtgIsLineIntersectSquareXZ( const float *v0, const float *v1, const float *square )
{
    float dy = v1[1]-v0[1];

    if( IsZero(dy) ) // Line and plane are in parallel
    {
        dy = v0[1]-square[1];
        if( IsZero(dy) ) // line in in the plane
        {
            float v[4];
            v[0] = v0[0];
            v[1] = v0[2];
            v[2] = v1[0];
            v[3] = v1[2];

            float rectangle[4];
            rectangle[0] = square[0];
            rectangle[1] = square[2];
            rectangle[2] = square[3];
            rectangle[3] = square[5];
            if( rtg2DIsLineIntersectRectangle( v, v+2, rectangle ) )
            {
                return TRUE;
            }
        }
        return FALSE;
    }

    // Check if v0 & v1 are at oppsite side of the plane
    if( v0[1]<square[1] )
    {
        if( v1[1]<square[1] ) return FALSE;
    }
    else if( v0[1]>square[1] )
    {
        if( v1[1]>square[1] ) return FALSE;
    }

    // Check if intersect point inside Square
    float t = (square[1]-v0[1])/dy;
    float x = v0[0] + (v1[0]-v0[0])*t;
    float z = v0[2] + (v1[2]-v0[2])*t;

    if( x<square[0] || x>square[3] ) return FALSE;
    if( z<square[2] || z>square[5] ) return FALSE;

    return TRUE;
}

BOOL rtgIsLineIntersectSquareYZ( const float *v0, const float *v1, const float *square )
{
    float dx = v1[0]-v0[0];

    if( IsZero(dx) ) // Line and plane are in parallel
    {
        dx = v0[0]-square[0];
        if( IsZero(dx) ) // line in in the plane
        {
            float v[4];
            v[0] = v0[1];
            v[1] = v0[2];
            v[2] = v1[1];
            v[3] = v1[2];

            float rectangle[4];
            rectangle[0] = square[1];
            rectangle[1] = square[2];
            rectangle[2] = square[4];
            rectangle[3] = square[5];
            if( rtg2DIsLineIntersectRectangle( v, v+2, rectangle ) )
            {
                return TRUE;
            }
        }
        return FALSE;
    }

    // Check if v0 & v1 are at oppsite side of the plane
    if( v0[0]<square[0] )
    {
        if( v1[0]<square[0] ) return FALSE;
    }
    else if( v0[0]>square[0] )
    {
        if( v1[0]>square[0] ) return FALSE;
    }

    // Check if intersect point inside Square
    float t = (square[0]-v0[0])/dx;
    float y = v0[1] + (v1[1]-v0[1])*t;
    float z = v0[2] + (v1[2]-v0[2])*t;

    if( y<square[1] || y>square[4] ) return FALSE;
    if( z<square[2] || z>square[5] ) return FALSE;

    return TRUE;
}

// fomular: 
// x = x0 + l*t; y = y0 + m*t; z = z0 + n*t;
BOOL rtgIsLineIntersectAABB( const float v0[3], const float v1[3], float AABB[6] )
{
#if DO_CHECK
    if (!(AABB[0]<=AABB[3] && AABB[1]<=AABB[4] && AABB[2]<=AABB[5]))
    {
        RtCoreLog().Warn("rtgIsLineIntersectAABB 的AABB Boundbox为空\n");
    }
#endif
    return rt_graph::rtgIsAABBIntersectLine( AABB, v0, v1 );
}

BOOL rtgIsLineIntersectAABB( const RtgVertex3 &v0, const RtgVertex3 &v1, const RtgVertex3 &inMin, const RtgVertex3 &inMax )
{
    float AABB[6];
    AABB[0] = inMin.x;
    AABB[1] = inMin.y;
    AABB[2] = inMin.z;
    AABB[3] = inMax.x;
    AABB[4] = inMax.y;
    AABB[5] = inMax.z;
    return rt_graph::rtgIsAABBIntersectLine( AABB, v0, v1 );
}

// fomular: 
// x = x0 + l*t; y = y0 + m*t; z = z0 + n*t;
BOOL rtgIsLineIntersectAABB( const RtgVertex3 &v0, const RtgVertex3 &v1, const RtgVertex3 &inMin, const RtgVertex3 &inMax, float *r1, float *r2 )
{
#if DO_CHECK
    if (!(inMin.x<=inMax.x && inMin.y<=inMax.y && inMin.z<=inMax.z ))
    {
        RtCoreLog().Warn("rtgIsLineIntersectAABB 的AABB Boundbox为空\n");
    }
#endif
    if( rtgIsVertexIntersectAABB( v0, inMin, inMax ) ) { if( r1 ) *r1 = 0.0f; return TRUE; }
    if( rtgIsVertexIntersectAABB( v1, inMin, inMax ) ) { if( r2 ) *r2 = 1.0f; return TRUE; }
    return rtgIsLineIntersectHollowAABB( v0, v1, inMin, inMax, r1, r2 );
}

// 用AABB裁剪直线 (v0,v1)
// V0   线段的开始(float[3])
// V1   线段的结束(float[3])
// AABB 和坐标轴平行的BoundingBox(float[6])
// rt0  裁剪出来的线段的开始(float[3])
// rt1  裁剪出来的线段的结束(float[3])
// 返回值, 如果有裁剪到线段就TRUE, 否则FALSE
BOOL rtgAABBCutLine(const RtgVertex3 &v0, const RtgVertex3 &v1, const RtgVertex3 &inMin, const RtgVertex3 &inMax, RtgVertex3 &outR0, RtgVertex3 &outR1)
{
#if DO_CHECK
    if (!(inMin.x<=inMax.x && inMin.y<=inMax.y && inMin.z<=inMax.z))
    {
        RtCoreLog().Warn("rtgAABBCutLine 的AABB Boundbox为空\n");
    }
#endif

    if (   (v0.x>inMin.x) && (v0.x<inMax.x)
        && (v0.y>inMin.y) && (v0.y<inMax.y)
        && (v0.z>inMin.z) && (v0.z<inMax.z)
        && (v1.x>inMin.x) && (v1.x<inMax.x)
        && (v1.y>inMin.y) && (v1.y<inMax.y)
        && (v1.z>inMin.z) && (v1.z<inMax.z))
    {
        outR0.x = v0.x;
        outR0.y = v0.y;
        outR0.z = v0.z;
        outR1.x = v1.x;
        outR1.y = v1.y;
        outR1.z = v1.z;
        return TRUE;
    }

    if ((v0.x<inMin.x) && (v1.x<inMin.x)) return FALSE;
    if ((v0.x>inMax.x) && (v1.x>inMax.x)) return FALSE;
    if ((v0.y<inMin.y) && (v1.y<inMin.y)) return FALSE;
    if ((v0.y>inMax.y) && (v1.y>inMax.y)) return FALSE;
    if ((v0.z<inMin.z) && (v1.z<inMin.z)) return FALSE;
    if ((v0.z>inMax.z) && (v1.z>inMax.z)) return FALSE;

    float m, n;
    outR0.x = v0.x;
    outR0.y = v0.y;
    outR0.z = v0.z;
    outR1.x = v1.x;
    outR1.y = v1.y;
    outR1.z = v1.z;

    // x
    m = outR1.x-outR0.x; // 长度
    if (!IsZero(m))
    {
        n = (inMin.x - outR0.x) / m;
        if(n>=0.0f && n<=1.0f)
        {
            if (outR0.x<inMin.x)
            {
                outR0.x = inMin.x;
                outR0.y = outR0.y+(outR1.y-outR0.y)*n;
                outR0.z = outR0.z+(outR1.z-outR0.z)*n;
            }else
            {
                outR1.x = inMin.x;
                outR1.y = outR0.y+(outR1.y-outR0.y)*n;
                outR1.z = outR0.z+(outR1.z-outR0.z)*n;
            }
        }
    }

    m = outR1.x-outR0.x; // 长度
    if (!IsZero(m))
    {
        n = (inMax.x - outR0.x) / m;
        if(n>=0.0f && n<=1.0f)
        {
            if (outR0.x>=inMax.x)
            {
                outR0.x = inMax.x;
                outR0.y = outR0.y+(outR1.y-outR0.y)*n;
                outR0.z = outR0.z+(outR1.z-outR0.z)*n;
            }else
            {
                outR1.x = inMax.x;
                outR1.y = outR0.y+(outR1.y-outR0.y)*n;
                outR1.z = outR0.z+(outR1.z-outR0.z)*n;
            }
        }
    }

    if ((v0.y<inMin.y) && (v1.y<inMin.y)) return FALSE;
    if ((v0.y>inMax.y) && (v1.y>inMax.y)) return FALSE;
    if ((v0.z<inMin.z) && (v1.z<inMin.z)) return FALSE;
    if ((v0.z>inMax.z) && (v1.z>inMax.z)) return FALSE;

    // y
    m = outR1.y - outR0.y;
    if (!IsZero(m))
    {
        n = (inMin.y - outR0.y) / m;
        if(n>=0.0f && n<=1.0f)
        {
            if (outR0.y<inMin.y)
            {
                outR0.x = outR0.x+(outR1.x-outR0.x)*n;
                outR0.y = inMin.y;
                outR0.z = outR0.z+(outR1.z-outR0.z)*n;
            }else
            {
                outR1.x = outR0.x+(outR1.x-outR0.x)*n;
                outR1.y = inMin.y;
                outR1.z = outR0.z+(outR1.z-outR0.z)*n;
            }
        }
    }

    m = outR1.y - outR0.y;
    if (!IsZero(m))
    {
        n = (inMax.y - outR0.y) / m;
        if(n>=0.0f && n<=1.0f)
        {
            if (outR0.y>=inMax.y)
            {
                outR0.x = outR0.x+(outR1.x-outR0.x)*n;
                outR0.y = inMax.y;
                outR0.z = outR0.z+(outR1.z-outR0.z)*n;
            }else
            {
                outR1.x = outR0.x+(outR1.x-outR0.x)*n;
                outR1.y = inMax.y;
                outR1.z = outR0.z+(outR1.z-outR0.z)*n;
            }
        }
    }

    if ((v0.z<inMin.z) && (v1.z<inMin.z)) return FALSE;
    if ((v0.z>inMax.z) && (v1.z>inMax.z)) return FALSE;

    // z
    m = outR1.z - outR0.z;
    if (!IsZero(m))
    {
        n = (inMin.z - outR0.z) / m;
        if(n>=0.0f && n<=1.0f)
        {
            if (outR0.z<inMin.z)
            {
                outR0.x = outR0.x+(outR1.x-outR0.x)*n;
                outR0.y = outR0.y+(outR1.y-outR0.y)*n;
                outR0.z = inMin.z;
            }else
            {
                outR1.x = outR0.x+(outR1.x-outR0.x)*n;
                outR1.y = outR0.y+(outR1.y-outR0.y)*n;
                outR1.z = inMin.z;
            }
        }
    }

    m = outR1.z - outR0.z;
    if (!IsZero(m))
    {
        n = (inMax.z - outR0.z) / m;
        if(n>=0.0f && n<=1.0f)
        {
            if (outR0.z>=inMax.z)
            {
                outR0.x = outR0.x+(outR1.x-outR0.x)*n;
                outR0.y = outR0.y+(outR1.y-outR0.y)*n;
                outR0.z = inMax.z;
            }else
            {
                outR1.x = outR0.x+(outR1.x-outR0.x)*n;
                outR1.y = outR0.y+(outR1.y-outR0.y)*n;
                outR1.z = inMax.z;
            }
        }
    }
    return TRUE;
}

// fomular: 
// x = x0 + l*t; y = y0 + m*t; z = z0 + n*t;
BOOL rtgIsLineIntersectHollowAABB( const RtgVertex3 &v0, const RtgVertex3 &v1, const RtgVertex3 &inMin, const RtgVertex3 &inMax, float *r1, float *r2 )
{
#if DO_CHECK
    if (!(inMin.x<=inMax.x && inMin.y<=inMax.y && inMin.z<=inMax.z))
    {
        RtCoreLog().Warn("rtgIsLineIntersectHollowAABB 的AABB Boundbox为空\n");
    }
#endif
    float l, m, n, t;

    l = v1.x - v0.x;
    m = v1.y - v0.y;
    n = v1.z - v0.z;

    if(l != 0.0f)
    {
        t = (inMin.x - v0.x) / l;
        if(t >= 0.0f && t <= 1.0f) 
        {
            float y = v0.y + m*t;
            if(y >= inMin.y && y <= inMax.y)
            {
                float z = v0.z + n*t;
                if(z >= inMin.z && z <= inMax.z) { if(r1) *r1 = t; return TRUE; }
            }
        }
        t = (inMax.x - v0.x) / l;
        if(t >= 0.0f && t <= 1.0f)
        {
            float y = v0.y + m*t;
            if(y >= inMin.y && y <= inMax.y)
            {
                float z = v0.z + n*t;
                if(z >= inMin.z && z <= inMax.z) { if(r1) *r1 = t; return TRUE; }
            }
        }
    }
    if(m != 0.0f)
    {
        t = (inMin.y - v0.y) / m;
        if(t >= 0.0f && t <= 1.0f)
        {
            float x = v0.x + l*t;
            if(x >= inMin.x && x <= inMax.x)
            {
                float z = v0.z + n*t;
                if(z >= inMin.z && z <= inMax.z) { if(r1) *r1 = t; return TRUE; }
            }
        }
        t = (inMax.y - v0.y) / m;
        if(t >= 0.0f && t <= 1.0f)
        {
            float x = v0.x + l*t;
            if(x >= inMin.x && x <= inMax.x)
            {
                float z = v0.z + n*t;
                if(z >= inMin.z && z <= inMax.z) { if(r1) *r1 = t; return TRUE; }
            }
        }
    }
    if(n != 0.0f)
    {
        t = (inMin.z - v0.z) / n;
        if(t >= 0.0f && t <= 1.0f)
        {
            float x = v0.x + l*t;
            if(x >= inMin.x && x <= inMax.x)
            {
                float y = v0.y + m*t;
                if(y >= inMin.y && y <= inMax.y) { if(r1) *r1 = t; return TRUE; }
            }
        }
        t = (inMax.z - v0.z) / n;
        if(t >= 0.0f && t <= 1.0f)
        {
            float x = v0.x + l*t;
            if(x >= inMin.x && x <= inMax.x)
            {
                float y = v0.y + m*t;
                if(y >= inMin.y && y <= inMax.y) { if(r1) *r1 = t; return TRUE; }
            }
        }
    }

    return FALSE;
}

BOOL rtgIsLineIntersectTriangle( const float v0[3], const float v1[3], const float t1[3], const float t2[3], const float t3[3])
{
#ifdef _DEBUG
    g_iCntLineIntersectTriangle ++;
#endif
    float e1[3], e2[3];
    float n[3];
    float v[3];
	float temp[3];

    rtgVectorSub( v,  v1, v0 );
    rtgVectorSub( e1, t2, t1 );
    rtgVectorSub( e2, t3, t1 );
    rtgVectorSub( temp,t1,v0 );

    rtgVectorCross( n, e1, e2 );

    
    float t = rtgVectorDot( n, v );
    if( IsZero(t) ) return FALSE;
    
	t = rtgVectorDot(n,temp)/t;
	
    if( t<0.0f || t>1.0f ) return FALSE;

    float g[3]; // Intersect point
    g[0] = v0[0]+v[0]*t;
    g[1] = v0[1]+v[1]*t;
    g[2] = v0[2]+v[2]*t;

    // Check triangle area
    float u1[9], u2[9];
    float dot[3];

    u1[0] = t1[0]-g[0];
    u1[1] = t1[1]-g[1];
    u1[2] = t1[2]-g[2];

    u1[3] = t2[0]-g[0];
    u1[4] = t2[1]-g[1];
    u1[5] = t2[2]-g[2];

    u1[6] = t3[0]-g[0];
    u1[7] = t3[1]-g[1];
    u1[8] = t3[2]-g[2];

    rtgVectorCross( u2,   u1,   u1+3 );
    rtgVectorCross( u2+3, u1+3, u1+6 );
    rtgVectorCross( u2+6, u1+6, u1   );

    dot[0] = rtgVectorDot( u2,   u2+3 );
    if( dot[0]<0.0f ) return FALSE;
    dot[1] = rtgVectorDot( u2+3, u2+6 );
    if( dot[1]<0.0f ) return FALSE;
    dot[2] = rtgVectorDot( u2+6, u2   );
    if( dot[2]<0.0f ) return FALSE;

    return TRUE;
}
//#include <D3DX9.h>

// Original from http://www.acm.org/jgt/papers/MollerTrumbore97
BOOL rtgIsRayHitTriangle( const float p[3], const float dir[3], const float v1[3], const float v2[3], const float v3[3], float *t )
{
#ifdef _DEBUG
    g_iCntRayHitTriangle ++;
#endif
    float e1[3], e2[3];
    float tv[3], pv[3], qv[3];
    float det, inv_det;

    // Find vectors for two edges sharing v[0]
    rtgVectorSub( e1, v2, v1 );
    rtgVectorSub( e2, v3, v1 );

    // 方向判断
    {
        rtgVectorCross( pv, e1, e2 );
        if (rtgVectorDot( pv, dir )>0.f) return FALSE;
    }

    // Begin calculating determinant - also used to calculate U parameter
    rtgVectorCross( pv, dir, e2 );

    // If determinant is near zero, ray lies in plane of triangle
    det = rtgVectorDot( e1, pv );

    if( IsZero(det) ) return FALSE;

    inv_det = 1.0f/det;

    // Calcuate distance from v0 to ray origin
    rtgVectorSub( tv, p, v1 );

    // Calculate U parameter and test boudns
    float u = rtgVectorDot( tv, pv ) * inv_det;
    if( u < 0.0f || u > 1.0f ) return FALSE;

    // Prepare to test V parameter
    rtgVectorCross( qv, tv, e1 );

    // Calcuate V parameter and test boudns
    float v = rtgVectorDot( dir, qv ) * inv_det;
    if( v < 0.0f || u + v > 1.0f ) return FALSE;

    if( t ) {
        // Calcuate t, ray intersects triangle
        *t = rtgVectorDot( e2, qv ) * inv_det;
        //*t /= rtgVectorLength(dir);
	    //if( ( *t < DELTA ) || ( *t > (1.0f-DELTA) ) )
        //    return FALSE;
    }

    return TRUE;
}

BOOL rtgIsAABBIntersectVertex( const float AABB[6], const float v[3] )
{
    if( v[0] < AABB[0] ) return FALSE;
    if( v[0] > AABB[3] ) return FALSE;
    if( v[1] < AABB[1] ) return FALSE;
    if( v[1] > AABB[4] ) return FALSE;
    if( v[2] < AABB[2] ) return FALSE;
    if( v[2] > AABB[5] ) return FALSE;
    return TRUE;
}

BOOL rtgIsAABBIntersectVertex( const float ext[3], const float pos[3], const float v[3] )
{
    if (abs(pos[0]-v[0])>ext[0]) return FALSE;
    if (abs(pos[1]-v[1])>ext[1]) return FALSE;
    if (abs(pos[2]-v[2])>ext[2]) return FALSE;
    return TRUE;
}

BOOL rtgIsAABBIntersectLine( const float AABB[6], const float *v0, const float *v1 )
{
    float square[6];

    // XY Plane
    square[0] = AABB[0];
    square[1] = AABB[1];
    square[2] = AABB[2];
    square[3] = AABB[3];
    square[4] = AABB[4];
    square[5] = AABB[2];  
    if( rtgIsLineIntersectSquareXY( v0, v1, square ) ) return TRUE;
    square[2] = square[5] = AABB[5];
    if( rtgIsLineIntersectSquareXY( v0, v1, square ) ) return TRUE;

    // YZ Plane
    //square[0] = AABB[0];
    //square[1] = AABB[1];
    square[2] = AABB[2];
    square[3] = AABB[0];
    //square[4] = AABB[4];
    //square[5] = AABB[5];
    if( rtgIsLineIntersectSquareYZ( v0, v1, square ) ) return TRUE;
    square[0] = square[3] = AABB[3];
    if( rtgIsLineIntersectSquareYZ( v0, v1, square ) ) return TRUE;

    // XZ Plane
    square[0] = AABB[0];
    //square[1] = AABB[1];
    //square[2] = AABB[2];
    //square[3] = AABB[3];
    square[4] = AABB[1];
    //square[5] = AABB[5];
    if( rtgIsLineIntersectSquareXZ( v0, v1, square ) ) return TRUE;
    square[1] = square[4] = AABB[4];
    if( rtgIsLineIntersectSquareXZ( v0, v1, square ) ) return TRUE;

    return FALSE;
}

BOOL rtgIsAABBIntersectLine( const float ext[3], const float pos[3], const float v0[3], const float v1[3] )
{
    float square[6];

    // XY Plane
    square[0] = pos[0]-ext[0];
    square[1] = pos[1]-ext[1];
    square[2] = pos[2]-ext[2];
    square[3] = pos[0]+ext[0];
    square[4] = pos[1]+ext[1];
    square[5] = pos[2]-ext[2];  
    if( rtgIsLineIntersectSquareXY( v0, v1, square ) ) return TRUE;
    square[2] = square[5] = pos[2]+ext[2];
    if( rtgIsLineIntersectSquareXY( v0, v1, square ) ) return TRUE;

    // YZ Plane
    //square[0] = pos[0]-ext[0];
    //square[1] = pos[1]-ext[1];
    square[2] = pos[2]-ext[2];
    square[3] = pos[0]-ext[0];
    //square[4] = pos[1]+ext[1];
    //square[5] = pos[2]+ext[2];
    if( rtgIsLineIntersectSquareYZ( v0, v1, square ) ) return TRUE;
    square[0] = square[3] = pos[0]+ext[0];
    if( rtgIsLineIntersectSquareYZ( v0, v1, square ) ) return TRUE;

    // XZ Plane
    square[0] = pos[0]-ext[0];
    //square[1] = pos[1]-ext[1];
    //square[2] = pos[2]-ext[2];
    //square[3] = pos[0]+ext[0];
    square[4] = pos[1]-ext[1];
    //square[5] = pos[2]+ext[2];
    if( rtgIsLineIntersectSquareXZ( v0, v1, square ) ) return TRUE;
    square[1] = square[4] = pos[1]+ext[1];
    if( rtgIsLineIntersectSquareXZ( v0, v1, square ) ) return TRUE;

    return FALSE;
}

BOOL rtgIsAABBIntersectTriangle( const float AABB[6], const float v1[3], const float v2[3], const float v3[3] )
{
#ifdef _DEBUG
    g_iCntAABBIntersectTriangle ++;
#endif
    // Check if any vertex of triangle intersect cube
    if( rtgIsAABBIntersectVertex( AABB, v1 ) ) return TRUE;
    if( rtgIsAABBIntersectVertex( AABB, v2 ) ) return TRUE;
    if( rtgIsAABBIntersectVertex( AABB, v3 ) ) return TRUE;

    // Check if any edge of triangle interserct cube
    if( rtgIsAABBIntersectLine( AABB, v1, v2 ) ) return TRUE;
    if( rtgIsAABBIntersectLine( AABB, v1, v3 ) ) return TRUE;
    if( rtgIsAABBIntersectLine( AABB, v2, v3 ) ) return TRUE;

    // Check if any diagonal of cube intersect triangle
    if( rtgIsLineIntersectTriangle( AABB, AABB+3, v1, v2, v3 ) ) return TRUE;

    float v[6];
    v[0] = AABB[0]; v[1] = AABB[1]; v[2] = AABB[5];
    v[3] = AABB[3]; v[4] = AABB[4]; v[5] = AABB[2];
    if( rtgIsLineIntersectTriangle( v, v+3, v1, v2, v3 ) ) return TRUE;

    v[0] = AABB[3]; v[1] = AABB[1]; v[2] = AABB[2];
    v[3] = AABB[0]; v[4] = AABB[4]; v[5] = AABB[5];
    if( rtgIsLineIntersectTriangle( v, v+3, v1, v2, v3 ) ) return TRUE;

    v[0] = AABB[0]; v[1] = AABB[4]; v[2] = AABB[2];
    v[3] = AABB[3]; v[4] = AABB[1]; v[5] = AABB[5];
    if( rtgIsLineIntersectTriangle( v, v+3, v1, v2, v3 ) ) return TRUE;

    return FALSE;
}

BOOL rtgIsAABBIntersectTriangle( const float ext[3], const float pos[3], const float v1[3], const float v2[3], const float v3[3] )
{
#ifdef _DEBUG
    g_iCntAABBIntersectTriangle ++;
#endif
    // Check if any vertex of triangle intersect cube
    if( rtgIsAABBIntersectVertex( ext, pos, v1 ) ) return TRUE;
    if( rtgIsAABBIntersectVertex( ext, pos, v2 ) ) return TRUE;
    if( rtgIsAABBIntersectVertex( ext, pos, v3 ) ) return TRUE;

    // Check if any edge of triangle interserct cube
    if( rtgIsAABBIntersectLine( ext, pos, v1, v2 ) ) return TRUE;
    if( rtgIsAABBIntersectLine( ext, pos, v1, v3 ) ) return TRUE;
    if( rtgIsAABBIntersectLine( ext, pos, v2, v3 ) ) return TRUE;

    float v[6];
    v[0] = ext[0]-pos[0];
    v[1] = ext[1]-pos[1];
    v[2] = ext[2]-pos[2];
    v[3] = ext[0]+pos[0];
    v[4] = ext[1]+pos[1];
    v[5] = ext[2]+pos[2];
    // Check if any diagonal of cube intersect triangle
    if( rtgIsLineIntersectTriangle( v, v+3, v1, v2, v3 ) ) return TRUE;

    //v[0] = ext[0]-pos[0];
    //v[1] = ext[1]-pos[1];
    v[2] = ext[2]+pos[2];
    //v[3] = ext[0]+pos[0];
    //v[4] = ext[1]+pos[1];
    v[5] = ext[2]-pos[2];
    if( rtgIsLineIntersectTriangle( v, v+3, v1, v2, v3 ) ) return TRUE;

    v[0] = ext[0]+pos[0];
    //v[1] = ext[1]-pos[1];
    v[2] = ext[2]-pos[2];
    v[3] = ext[0]-pos[0];
    //v[4] = ext[1]+pos[1];
    v[5] = ext[2]+pos[2];
    if( rtgIsLineIntersectTriangle( v, v+3, v1, v2, v3 ) ) return TRUE;

    v[0] = ext[0]-pos[0];
    v[1] = ext[1]+pos[1];
    //v[2] = ext[2]-pos[2];
    v[3] = ext[0]+pos[0];
    v[4] = ext[1]-pos[1];
    //v[5] = ext[2]+pos[2];
    if( rtgIsLineIntersectTriangle( v, v+3, v1, v2, v3 ) ) return TRUE;

    return FALSE;
}

// Ref: Real-Time Rendering, Tomas Moller, Eric Hanies
//      A K Peters
//      Page 323
BOOL rtgIsAABBIntersectSphere( const float AABB[6], const float pos[3], float r_square )
{
    float f;
    float d=0.0f;

    for( int i=0; i<3; i++ ) {
        if( pos[i] < AABB[i] ) {
            f = pos[i]-AABB[i];
            d += f*f;
        }
        else if( pos[i] > AABB[i+3] ) {
            f = pos[i]-AABB[i+3];
            d += f*f;
        }
        if( d > r_square ) return FALSE;
    }

    return TRUE;
}

BOOL rtgIsAABBIntersectSphere( const float ext[3], const float aabbPos[3], const float pos[3], float r_square )
{
    float v[6];
    v[0] = ext[0]-aabbPos[0];
    v[1] = ext[1]-aabbPos[1];
    v[2] = ext[2]-aabbPos[2];
    v[3] = ext[0]+aabbPos[0];
    v[4] = ext[1]+aabbPos[1];
    v[5] = ext[2]+aabbPos[2];
    return rtgIsAABBIntersectSphere(v, pos, r_square);
}

BOOL rtgIsLineIntersectSphere(const float fLine0[3], const float fLine1[3], const float fPos[3], float r_square)
{
    float xx1  = fPos[0]-fLine0[0];
    float yy1  = fPos[1]-fLine0[1];
    float zz1  = fPos[2]-fLine0[2];
    float x2x1 = fLine1[0]-fLine0[0];
    float y2y1 = fLine1[1]-fLine0[1];
    float z2z1 = fLine1[2]-fLine0[2];
    float t = x2x1*xx1 + y2y1*yy1 + z2z1*zz1;
    float fDistance = xx1*xx1 + yy1*yy1 + zz1*zz1 - t*t/(x2x1*x2x1 + y2y1*y2y1 + z2z1*z2z1);
    if (fDistance>r_square*r_square) return FALSE;
    return TRUE;
}

BOOL rtgIsAABBIntersectAABB( const RtgVertex3 &extentsA, const RtgVertex3 &posA,
                      const RtgVertex3 &extentsB, const RtgVertex3 &posB )
{
    if( Abs(posA[0] - posB[0]) < (extentsA[0] + extentsB[0]) &&
        Abs(posA[1] - posB[1]) < (extentsA[1] + extentsB[1]) &&
        Abs(posA[2] - posB[2]) < (extentsA[2] + extentsB[2]) ) 
        return TRUE;
    else
        return FALSE;
}

BOOL rtgIsOBBIntersectOBB( const RtgVertex3 &extentsA, const RtgMatrix16 &mA, const RtgVertex3 &extentsB, const RtgMatrix16 &mB )
{
    RtgVertex3 posA, posB;
    RtgVertex3 ar[3], br[3];
    mA.GetRow(3, posA);
    mB.GetRow(3, posB);
    for ( int i=0; i<3; i++ )
    {
        mA.GetRow(i, ar[i]);
        ar[i].Normalize();
        mB.GetRow(i, br[i]);
        br[i].Normalize();
    }
    return rtgIsOBBIntersectOBB(extentsA, posA, ar, extentsB, posB, br);
}

BOOL rtgIsOBBIntersectOBB( const RtgVertex3 &extentsA, const RtgVertex3 &posA, const RtgVertex3 basisA[3], 
                    const RtgVertex3 &extentsB, const RtgVertex3 &posB, const RtgVertex3 basisB[3] )
{
#ifdef _DEBUG
    g_iCntOBBIntersectOBB ++;
#endif
    RtgVertex3 dir;			// vector between centers
    float c[3][3];		// matrix c = basisA.Transpose() * basisB
    float ac[3][3];		// absolute values of c
    float axisdir[3];	// basisA[i] * dir
    float d, e0, e1;	// distance between centers and projected extentsA

    dir = posB - posA;

    // basisA C0 + t * A0
    c[0][0] = basisA[0] * basisB[0];
    c[0][1] = basisA[0] * basisB[1];
    c[0][2] = basisA[0] * basisB[2];
    axisdir[0] = basisA[0] * dir;
    ac[0][0] = fabs( c[0][0] );
    ac[0][1] = fabs( c[0][1] );
    ac[0][2] = fabs( c[0][2] );

    d = fabs( axisdir[0] );
    e0 = extentsA[0];
    e1 = extentsB[0] * ac[0][0] + extentsB[1] * ac[0][1] + extentsB[2] * ac[0][2];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * A1
    c[1][0] = basisA[1] * basisB[0];
    c[1][1] = basisA[1] * basisB[1];
    c[1][2] = basisA[1] * basisB[2];
    axisdir[1] = basisA[1] * dir;
    ac[1][0] = fabs( c[1][0] );
    ac[1][1] = fabs( c[1][1] );
    ac[1][2] = fabs( c[1][2] );

    d = fabs( axisdir[1] );
    e0 = extentsA[1];
    e1 = extentsB[0] * ac[1][0] + extentsB[1] * ac[1][1] + extentsB[2] * ac[1][2];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * A2
    c[2][0] = basisA[2] * basisB[0];
    c[2][1] = basisA[2] * basisB[1];
    c[2][2] = basisA[2] * basisB[2];
    axisdir[2] = basisA[2] * dir;
    ac[2][0] = fabs( c[2][0] );
    ac[2][1] = fabs( c[2][1] );
    ac[2][2] = fabs( c[2][2] );

    d = fabs( axisdir[2] );
    e0 = extentsA[2];
    e1 = extentsB[0] * ac[2][0] + extentsB[1] * ac[2][1] + extentsB[2] * ac[2][2];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * B0
    d = fabs( basisB[0] * dir );
    e0 = extentsA[0] * ac[0][0] + extentsA[1] * ac[1][0] + extentsA[2] * ac[2][0];
    e1 = extentsB[0];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * B1
    d = fabs( basisB[1] * dir );
    e0 = extentsA[0] * ac[0][1] + extentsA[1] * ac[1][1] + extentsA[2] * ac[2][1];
    e1 = extentsB[1];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * B2
    d = fabs( basisB[2] * dir );
    e0 = extentsA[0] * ac[0][2] + extentsA[1] * ac[1][2] + extentsA[2] * ac[2][2];
    e1 = extentsB[2];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * A0xB0
    d = fabs( axisdir[2] * c[1][0] - axisdir[1] * c[2][0] );
    e0 = extentsA[1] * ac[2][0] + extentsA[2] * ac[1][0];
    e1 = extentsB[1] * ac[0][2] + extentsB[2] * ac[0][1];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * A0xB1
    d = fabs( axisdir[2] * c[1][1] - axisdir[1] * c[2][1] );
    e0 = extentsA[1] * ac[2][1] + extentsA[2] * ac[1][1];
    e1 = extentsB[0] * ac[0][2] + extentsB[2] * ac[0][0];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * A0xB2
    d = fabs( axisdir[2] * c[1][2] - axisdir[1] * c[2][2] );
    e0 = extentsA[1] * ac[2][2] + extentsA[2] * ac[1][2];
    e1 = extentsB[0] * ac[0][1] + extentsB[1] * ac[0][0];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * A1xB0
    d = fabs( axisdir[0] * c[2][0] - axisdir[2] * c[0][0] );
    e0 = extentsA[0] * ac[2][0] + extentsA[2] * ac[0][0];
    e1 = extentsB[1] * ac[1][2] + extentsB[2] * ac[1][1];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * A1xB1
    d = fabs( axisdir[0] * c[2][1] - axisdir[2] * c[0][1] );
    e0 = extentsA[0] * ac[2][1] + extentsA[2] * ac[0][1];
    e1 = extentsB[0] * ac[1][2] + extentsB[2] * ac[1][0];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * A1xB2
    d = fabs( axisdir[0] * c[2][2] - axisdir[2] * c[0][2] );
    e0 = extentsA[0] * ac[2][2] + extentsA[2] * ac[0][2];
    e1 = extentsB[0] * ac[1][1] + extentsB[1] * ac[1][0];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * A2xB0
    d = fabs( axisdir[1] * c[0][0] - axisdir[0] * c[1][0] );
    e0 = extentsA[0] * ac[1][0] + extentsA[1] * ac[0][0];
    e1 = extentsB[1] * ac[2][2] + extentsB[2] * ac[2][1];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * A2xB1
    d = fabs( axisdir[1] * c[0][1] - axisdir[0] * c[1][1] );
    e0 = extentsA[0] * ac[1][1] + extentsA[1] * ac[0][1];
    e1 = extentsB[0] * ac[2][2] + extentsB[2] * ac[2][0];
    if ( d > e0 + e1 ) {
        return FALSE;
    }

    // basisA C0 + t * A2xB2
    d = fabs( axisdir[1] * c[0][2] - axisdir[0] * c[1][2] );
    e0 = extentsA[0] * ac[1][2] + extentsA[1] * ac[0][2];
    e1 = extentsB[0] * ac[2][1] + extentsB[1] * ac[2][0];
    if ( d > e0 + e1 ) {
        return FALSE;
    }
    return TRUE;

    /*

    // Translation, in parent frame
    RtgVertex3 V = posB - posA;

    // Translation, in A's frame
    RtgVertex3 T;
    T[0] = V.Dot( basisA[0] );
    T[1] = V.Dot( basisA[1] );
    T[2] = V.Dot( basisA[2] );

    // B's basis with respect to A's local frame
    float R[3][3];
    float ra, rb, t;
    int   i, j;

    // Calculate rotation matrix
    for( i=0; i<3; i++ ) {
        for( j=0; j<3; j++ ) {
            R[i][j] = basisA[i].Dot( basisB[j] );
        }
    }

    // ALGORITHM: Use the separating axis test for all 15 potential separating
    // axes. If a separating axes could not be found, the two boxes overlap.

    // A's basis vectors
    for( i=0; i<3; i++ ) {
        ra = extentsA[i];
        rb = extentsB[0] * Abs(R[i][0]) + 
            extentsB[1] * Abs(R[i][1]) + 
            extentsB[2] * Abs(R[i][2]);

        if( Abs(T[i]) > ra+rb ) return FALSE;
    }

    // B's basis vectors
    for( j=0; j<3; j++ ) {
        ra = extentsA[0] * Abs(R[0][j]) + 
            extentsA[1] * Abs(R[1][j]) + 
            extentsA[2] * Abs(R[2][j]);
        rb = extentsB[j];
        t  = T[0]*R[0][j] + T[1]*R[1][j] + T[2]*R[2][j];
        if( Abs(t) > ra+rb ) return FALSE;
    }

    // 9 cross products
    // L = A0 x B0
    ra = extentsA[1] * Abs(R[2][0]) + extentsA[2] * Abs(R[1][0]);
    rb = extentsB[1] * Abs(R[0][2]) + extentsB[2] * Abs(R[0][1]);
    t  = Abs( T[2]*R[1][0] - T[1]*R[2][0] );
    if( t > ra+rb ) return FALSE;

    // L = A0 x B1
    ra = extentsA[1] * Abs(R[2][1]) + extentsA[2] * Abs(R[1][1]);
    rb = extentsB[0] * Abs(R[0][2]) + extentsB[2] * Abs(R[0][0]);
    t  = Abs( T[2]*R[1][1] - T[1]*R[2][1] );
    if( t > ra+rb ) return FALSE;

    // L = A0 x B2
    ra = extentsA[1] * Abs(R[2][2]) + extentsA[2] * Abs(R[1][2]);
    rb = extentsB[0] * Abs(R[0][1]) + extentsB[1] * Abs(R[0][0]);
    t  = Abs( T[2]*R[1][2] - T[1]*R[2][2] );
    if( t > ra+rb ) return FALSE;

    // L = A1 x B0
    ra = extentsA[0] * Abs(R[2][0]) + extentsA[2] * Abs(R[0][0]);
    rb = extentsB[1] * Abs(R[1][2]) + extentsB[2] * Abs(R[1][1]);
    t  = Abs( T[0]*R[2][0] - T[2]*R[0][0] );
    if( t > ra+rb ) return FALSE;

    // L = A1 x B1
    ra = extentsA[0] * Abs(R[2][1]) + extentsA[2] * Abs(R[0][1]);
    rb = extentsB[0] * Abs(R[1][2]) + extentsB[2] * Abs(R[1][0]);
    t  = Abs( T[0]*R[2][1] - T[2]*R[0][1] );
    if( t > ra+rb ) return FALSE;

    // L = A1 x B2
    ra = extentsA[0] * Abs(R[2][2]) + extentsA[2] * Abs(R[0][2]);
    rb = extentsB[0] * Abs(R[1][1]) + extentsB[1] * Abs(R[1][0]);
    t  = Abs( T[0]*R[2][2] - T[2]*R[0][2] );
    if( t > ra+rb ) return FALSE;

    // L = A2 x B0
    ra = extentsA[0] * Abs(R[1][0]) + extentsA[1] * Abs(R[0][0]);
    rb = extentsB[1] * Abs(R[2][2]) + extentsB[2] * Abs(R[2][1]);
    t  = Abs( T[1]*R[0][0] - T[0]*R[1][0] );
    if( t > ra+rb ) return FALSE;

    // L = A2 x B1
    ra = extentsA[0] * Abs(R[1][1]) + extentsA[1] * Abs(R[0][1]);
    rb = extentsB[0] * Abs(R[2][2]) + extentsB[2] * Abs(R[2][0]);
    t  = Abs( T[1]*R[0][1] - T[0]*R[1][1] );
    if( t > ra+rb ) return FALSE;

    // L = A2 x B2
    ra = extentsA[0] * Abs(R[1][2]) + extentsA[1] * Abs(R[0][2]);
    rb = extentsB[0] * Abs(R[2][1]) + extentsB[1] * Abs(R[2][0]);
    t  = Abs( T[1]*R[0][2] - T[0]*R[1][2] );
    if( t > ra+rb ) return FALSE;

    // No separating axis found, the two boxes overlap

    return TRUE;
    */
}

BOOL rtgIsTriangleIntersectTriangle(const float v1[3], const float v2[3], const float v3[3], const float u1[3], const float u2[3], const float u3[3])
{
    if (rtgIsLineIntersectTriangle(v1, v2, u1, u2, u3)) return TRUE;
    if (rtgIsLineIntersectTriangle(v2, v3, u1, u2, u3)) return TRUE;
    if (rtgIsLineIntersectTriangle(v3, v1, u1, u2, u3)) return TRUE;

    if (rtgIsLineIntersectTriangle(u1, u2, v1, v2, v3)) return TRUE;
    if (rtgIsLineIntersectTriangle(u2, u3, v1, v2, v3)) return TRUE;
    if (rtgIsLineIntersectTriangle(u3, u1, v1, v2, v3)) return TRUE;
    return FALSE;
}

BOOL rtgIsOBBIntersectTriangle(const RtgVertex3 &extents, const RtgVertex3 &pos, const RtgVertex3 basis[3], const float v1[3], const float v2[3], const float v3[3])
{
    CHECKEX("没有实现的函数");
    return FALSE;
}

BOOL rtgGetPlaneIntersect(float p0[4], float p1[4], float p2[4], float pIntersection[3])
{
    // 这个函数是有问题的，只解了全部参数不为0的方程
    CHECK(p0!=0 && p1!=0 && p2!=0 && pIntersection!=0);
    float i, j, k, m, n, o, p;
    i = p0[1]/p0[0];
    j = p0[2]/p0[0];
    k = p0[3]/p0[0];
    n = p1[1]-p1[0]*i;
    o = p1[2]-p1[0]*j;
    p = p1[3]-p1[0]*k;
    m = (p2[1]-p2[0]*i)/n;
    pIntersection[2] = (p*m-(p2[3]-p2[0]*k))/((p2[2]-p2[0]*j)-o*m);
    pIntersection[1] = -(pIntersection[2]*o+p)/n;
    pIntersection[0] = -pIntersection[1]*i - pIntersection[2]*j - k;
    return TRUE;
}

BOOL rtgGet2dLineIntersect(float p0[3], float p1[3], float pIntersection[2])
{
    // 这个函数是有问题的，只解了全部参数不为0的方程
    CHECK(p0!=0 && p1!=0 && pIntersection!=0);
    float i, j;
    i = p0[2]/p0[0];
    j = p0[1]/p0[0];
    pIntersection[1] = (i*p1[0]-p1[2])/(p1[1]-j*p1[0]);
    pIntersection[0] = -i-j*pIntersection[1];
    return TRUE;
}

#ifdef _DEBUG
void rtguClearCnt()
{
    g_iCntAABBIntersectTriangle = 0;
    g_iCntLineIntersectTriangle = 0;
    g_iCntOBBIntersectOBB = 0;
    g_iCntRayHitTriangle = 0;
}

int rtguGetCntAABBIntersectTriangle()
{
    return g_iCntAABBIntersectTriangle;
}

int rtguGetCntLineIntersectTriangle()
{
    return g_iCntLineIntersectTriangle;
}

int rtguGetCntOBBIntersectOBB()
{
    return g_iCntOBBIntersectOBB;
}

int rtguGetCntRayHitTriangle()
{
    return g_iCntRayHitTriangle;
}
#endif
} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
