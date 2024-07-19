#include "rtg_graph_inter.h"

namespace rt_graph {

int rtgRand(int minValue, int maxValue, int mValue, int mPercent, int mRangeDivisor) // tDAN 1211, 2000
{
    if(maxValue < minValue) return minValue;

    int dis = maxValue - minValue + 1;

    if(mValue < minValue || mValue > maxValue)
    {
        return rand() % dis + minValue;
    }
    else
    {
        if(rand() % 100 < mPercent)
        {
            CHECK( mRangeDivisor!=0 );

            int mDis = dis / mRangeDivisor;
            if(mDis == 0) return rand() % dis + minValue;
            else return rand() % mDis + mValue - mDis / 2;
        }
        else
        {
            return rand() % dis + minValue;
        }
    }
}


//
// Area
//

/*-----------------------------------
    perform the following calculation
    | x0 y0 |
    a =   | x1 y1 | / 2
    | x2 y2 |
    | x0 y0 |
-----------------------------------*/
float rtg2DTriangleArea( const float v0[2], const float v1[2], const float v2[2] )
{
    return (v0[0]*(v1[1]-v2[1])+v1[0]*(v2[1]-v0[1])+v2[0]*(v0[1]-v1[1])) / 2.0f;
}

/*-----------------------------------
    perform the following calculation
    | x0 y0 |
    a =   | x1 y1 |
    | x2 y2 |
    | x0 y0 |
-----------------------------------*/
float rtg2DTriangleAreaX2( const float v0[2], const float v1[2], const float v2[2] )
{
    return (v0[0]*(v1[1]-v2[1])+v1[0]*(v2[1]-v0[1])+v2[0]*(v0[1]-v1[1]));
}

float rtgTriangleArea( float *v0, float *v1, float *v2 )
{
    float e1[3];
    float e2[3];

    e1[0] = v1[0]-v0[0];
    e1[1] = v1[1]-v0[1];
    e1[2] = v1[2]-v0[2];

    e2[0] = v2[0]-v0[0];
    e2[1] = v2[1]-v0[1];
    e2[2] = v2[2]-v0[2];

    return (rtgVectorDot( e1, e2 )/2.0f);
}


float rtgTriangleTiltAngle( const float *v0, const float *v1, const float *v2 )
{
    float v[9];

    v[0] = v1[0]-v0[0];   v[1] = v1[1]-v0[1];   v[2] = v1[2]-v0[2];
    v[3] = v2[0]-v0[0];   v[4] = v2[1]-v0[1];   v[5] = v2[2]-v0[2];

    rtgVectorCross( v+6, v, v+3 );

    v[0] = fsqrt(v[6]*v[6]+v[7]*v[7]+v[8]*v[8]); // Length of normal vector

    if( v[0] < DELTA ) return 0;

    return (float) ( acos( v[8]/v[0] ) * 57.29578 ); // *180/pi
}

void rtgGetBox(RtgVertex3 &vHalfExt, RtgMatrix16 &m16, RtgVertex3 p[8])
{
    RtgVertex3 vec0, vec1, vec2;

    vec0.Set(2.0f * vHalfExt.x, 0.0f, 0.0f);
    vec1.Set(0.0f, 2.0f * vHalfExt.y, 0.0f);
    vec2.Set(0.0f, 0.0f, 2.0f * vHalfExt.z);

    p[0] = vHalfExt;
    p[0].Negative();
    p[1] = p[0] + vec0;
    p[2] = p[1] + vec2;
    p[3] = p[0] + vec2;

    p[4] = p[0] + vec1;
    p[5] = p[1] + vec1;
    p[6] = p[2] + vec1;
    p[7] = p[3] + vec1;

    for ( int i=0; i<8; i++ )
    {
        p[i] = p[i] * m16;
    }
}



//
// Implementation of RtgTriangle
//
RtgTriangle::RtgTriangle( const float triangle[9] )
{
    m_v0 = triangle;
    m_v1 = triangle+3;
    m_v2 = triangle+6;
}

RtgTriangle::RtgTriangle( const RtgVertex3 &v0, 
                         const RtgVertex3 &v1, 
                         const RtgVertex3 &v2 ):
m_v0(v0), m_v1(v1), m_v2(v2)
{
}

/*-----------------------------------
    perform the following calculation
    | x1 y1 |
    a =   | x2 y2 | / 2
    | x3 y3 |
    | x1 y1 |
-----------------------------------*/
float RtgTriangle::Area2D( void ) const
{
    return ( m_v0[0]*(m_v1[1]-m_v2[1]) +
        m_v1[0]*(m_v2[1]-m_v0[1]) + 
        m_v2[0]*(m_v0[1]-m_v1[1])   ) / 2.0f;
}

/*-----------------------------------
    perform the following calculation
    | x1 y1 |
    a =   | x2 y2 | / 2
    | x3 y3 |
    | x1 y1 |
-----------------------------------*/
float RtgTriangle::Area2DX2( void ) const
{
    return ( m_v0[0]*(m_v1[1]-m_v2[1]) +
        m_v1[0]*(m_v2[1]-m_v0[1]) + 
        m_v2[0]*(m_v0[1]-m_v1[1])   );
}

float RtgTriangle::Area( void ) const
{
    return (m_v1-m_v0).Dot(m_v2-m_v0) / 2.0f;
}

RtgVertex3 RtgTriangle::Normal( void )
{
    return (m_v1-m_v0).Cross(m_v2-m_v0);
}

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
