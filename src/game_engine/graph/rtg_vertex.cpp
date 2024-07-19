#include "rtg_graph_inter.h"

namespace rt_graph {

void RotationToQuaternion( RtgVertex4 &in, RtgVertex4 &out )
{
	float s = sinf( in.w/2.0f );

    out.x = in.x * s;
    out.y = in.y * s;
    out.z = in.z * s;
    out.w = cosf( in.w/2.0f );
	//out.Normalize();
}

void QuaternionToRotation( RtgVertex4 &in, RtgVertex4 &out )
{
    out.w = acosf(in.w) * 2.0f;
	float s = sinf(out.w/2.0f);
	if ( s==0.0f )
	{
		out.x = 1.0f;
		out.y = out.z = 0.0f;
		out.w = 0.0f;
	}else
	{
		out.x    = in.x / s;
		out.y    = in.y / s;
		out.z    = in.z / s;
	}
	//out.Normalize();
}

// ÔöÖµ
#define DELTA 0.0001f

RtgVertex4 Slerp(RtgVertex4 &from, RtgVertex4 &to, float t)
{
	RtgVertex4 res;
    float  to1[4];
	float  omega, cosom, sinom, scale0, scale1;

	// calc cosine
    cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w;

    // adjust signs (if necessary)
	if ( cosom <0.0 )
	{ 
		cosom = -cosom; to1[0] = - to.x;
		to1[1] = - to.y;
		to1[2] = - to.z;
		to1[3] = - to.w;
    } 
	else
	{
		to1[0] = to.x;
		to1[1] = to.y;
		to1[2] = to.z;
		to1[3] = to.w;
    }

    // calculate coefficients
    if ( (1.0f - cosom) > DELTA ) 
	{
		// standard case (slerp)
        omega = acosf(cosom);
        sinom = sinf(omega);
        scale0 = sinf((1.0f - t) * omega) / sinom;
        scale1 = sinf(t * omega) / sinom;
    } 
	else 
	{        
		// "from" and "to" quaternions are very close 
	    //  ... so we can do a linear interpolation
        scale0 = 1.0f - t;
        scale1 = t;
    }
	// calculate final values
	res.x = scale0 * from.x + scale1 * to1[0];
	res.y = scale0 * from.y + scale1 * to1[1];
	res.z = scale0 * from.z + scale1 * to1[2];
	res.w = scale0 * from.w + scale1 * to1[3];

	return res;
}

RtgVertex3 GetUpVecFromFront(RtgVertex3 front)
{
	float ax = (float)Abs(front.x);
	float ay = (float)Abs(front.y);
	float az = (float)Abs(front.z);
	RtgVertex3 up;

	if ( ax > ay && ax > az )
	// vec0 most
	{
		up.x = 0.0f;
		if ( front.z!=0.0f )
		{
			up.y = front.y > 0 ? 1.0f:-1.0f;
			up.z = -front.y/front.z * up.y;
		}
		else
		{
			up.z = front.z > 0 ? 1.0f:-1.0f;
			up.y = -front.z/front.y * up.z;
		}
	}
	else if ( ay > ax && ay > az )
	// vec1 most
	{
		up.y = 0.0f;
		if ( front.z!=0.0f )
		{
			up.x = front.x > 0 ? 1.0f:-1.0f;
			up.z = -front.x/front.z * up.x;
		}
		else
		{
			up.z = front.z > 0 ? 1.0f:-1.0f;
			up.x = -front.z/front.x * up.z;
		}
	}	
	else
	// vec2 most
	{
		up.z = 0.0f;
		if ( front.y!=0.0f )
		{
			up.x = front.x > 0 ? 1.0f:-1.0f;
			up.y = -front.x/front.y * up.x;
		}
		else
		{
			up.y = front.y > 0 ? 1.0f:-1.0f;
			up.x = -front.y/front.x * up.y;
		}
	}
	up.Normalize();
	return up;
}

//
// Vector
//

float rtgVectorAngle( const float v1[3], const float v2[3] )
{
    float f = rtgVectorLength(v1)*rtgVectorLength(v2);

    CHECK( f!=0.0f );

    f = facos( rtgVectorDot(v1,v2) / f );

    return ( f>=0.0f ? f : (f+180.0f) );
}

float rtgVectorSAngle( const float v1[3], const float v2[3], const float refereeV[3])
{
    float f = (rtgVectorLength(v1)*rtgVectorLength(v2));

    CHECK( f!=0.0f );

    f = facos( rtgVectorDot(v1,v2) / f );
    f = (f>=0.0f)? f : (f+180.0f);

    float v3[3];
    rtgVectorCross(v3, v1, v2);

    if(rtgVectorDot(v3, refereeV) < 0.0f) f = -f;

    return f;
}

float rtgVector2DAngle( const float v1[2], const float v2[2] )
{
    double v1angle,v2angle,t;

    v1angle=atan2(v1[1],v1[0]);

    v2angle=atan2(v2[1],v2[0]);

    t=v1angle-v2angle;
    if(t> PI) t-=PIx2;
    if(t<-PI) t+=PIx2;

    return Abs((float)(t*180.0/PI));
}

float rtgVector2DSAngle( const float v1[2], const float v2[2] )
{
    double v1angle, v2angle, t;

    v1angle = atan2( v1[1], v1[0] );

    v2angle = atan2( v2[1], v2[0] );

    t = v2angle - v1angle;
    if( t >  PI ) t -= PIx2;
    if( t < -PI ) t += PIx2;

    return (float)( t * 180.0 / PI );
}

void rtgVectorRotate( float dest[3], const float vect[3], const float ref[3], float degree )
{
    CHECK( vect[0] != 0.0f || vect[1] != 0.0f || vect[2] != 0.0f );
    CHECK( vect[0] != ref[0] || vect[1] != ref[1] || vect[2] != ref[2] );
    CHECK( ref[0] != 0.0f || ref[1] != 0.0f || ref[2] != 0.0f );

    float face[3], right[3];
    rtgVectorCross( right, vect, ref );

    if( right[0] != 0.0f || right[1] != 0.0f || right[2] != 0.0f )
    {
        rtgVectorCross( face, ref, right );

        float offsetAngle = rtgVectorSAngle( face, vect, right );

        RtgMatrix12 m12( face, ref, right );
        m12.RotateLZ( degree );
        m12.RotateLX( offsetAngle );

        const float *v = m12.V();
        dest[0] = v[0];
        dest[1] = v[1];
        dest[2] = v[2];
    }
}

void rtgVectorRotateXAxis( float dest[3], const float vect[3], float degree )
{
    CHECK( vect[0] != 0.0f || vect[1] != 0.0f || vect[2] != 0.0f );

    RtgMatrix12 m12;
    m12.RotateX( degree );

    rtgV3MultiplyM12( dest, vect, (float *)m12 );
    rtgVectorNormalize( dest );
}

void rtgVectorRotateYAxis( float dest[3], const float vect[3], float degree )
{
    CHECK( vect[0] != 0.0f || vect[1] != 0.0f || vect[2] != 0.0f );

    RtgMatrix12 m12;
    m12.RotateY( degree );

    rtgV3MultiplyM12( dest, vect, (float *)m12 );
    rtgVectorNormalize( dest );
}

void rtgVectorRotateZAxis( float dest[3], const float vect[3], float degree )
{
    CHECK( vect[0] != 0.0f || vect[1] != 0.0f || vect[2] != 0.0f );

    RtgMatrix12 m12;
    m12.RotateZ( degree );

    rtgV3MultiplyM12( dest, vect, (float *)m12 );
    rtgVectorNormalize( dest );
}


void rtgNormalVectorOfTriangle( float n[3], float triangle[9] )
{
    float e[6];

    rtgVectorSub( e,   triangle+3, triangle );
    rtgVectorSub( e+3, triangle+6, triangle );
    rtgVectorCross( n, e, e+3 );
}

//------------------------------
//  convert a quaternion to a M9
// -------------------------------
void rtgQuatToM9( float m[9], const float vt[3], const float w )
{
    float xs = vt[0] * 2.0f;
    float ys = vt[1] * 2.0f;
    float zs = vt[2] * 2.0f;
    float wx = w * xs;
    float wy = w * ys;
    float wz = w * zs;
    float xx = vt[0] * xs;
    float xy = vt[0] * ys;
    float xz = vt[0] * zs;
    float yy = vt[1] * ys;
    float yz = vt[1] * zs;
    float zz = vt[2] * zs;

    m[0] = 1.0f - ( yy + zz );
    m[1] = xy + wz;
    m[2] = xz - wy;

    m[3] = xy - wz;
    m[4] = 1.0f - ( xx + zz );
    m[5] = yz + wx;

    m[6] = xz + wy;
    m[7] = yz - wx;
    m[8] = 1.0f - ( xx + yy );
}

void rtgQuatNormalize( float vt[3], float &w )
{
    float	fDist;
    float fSquare = vt[0]*vt[0] + vt[1]*vt[1] + vt[2]*vt[2] + w * w;

    CHECK( fSquare!=0.0f );

    fDist = 1.0f / fsqrt(fSquare);

    vt[0] *= fDist;
    vt[1] *= fDist;
    vt[2] *= fDist;
    w     *= fDist;
}


//
// V3
//
void rtgV3MultiplyM12( float *dest, const float *v3, const float *m12 )
{
    float *dest1 = dest;
    float fTmp[3];
    if( dest==v3 ) dest1 = fTmp;

    dest1[0] = v3[0]*m12[0] + v3[1]*m12[3] + v3[2]*m12[6] + m12[9];
    dest1[1] = v3[0]*m12[1] + v3[1]*m12[4] + v3[2]*m12[7] + m12[10];
    dest1[2] = v3[0]*m12[2] + v3[1]*m12[5] + v3[2]*m12[8] + m12[11];

    if( dest1==dest ) return;

    dest[0] = dest1[0];
    dest[1] = dest1[1];
    dest[2] = dest1[2];
}

void rtgV3MultiplyM9( float *dest, const float *v3, const float *m9 )
{
    float *dest1 = dest;
    float fTmp[3];
    if( dest==v3 ) dest1 = fTmp;

    dest1[0] = v3[0]*m9[0] + v3[1]*m9[3] + v3[2]*m9[6];
    dest1[1] = v3[0]*m9[1] + v3[1]*m9[4] + v3[2]*m9[7];
    dest1[2] = v3[0]*m9[2] + v3[1]*m9[5] + v3[2]*m9[8];

    if( dest1==dest ) return;

    dest[0] = dest1[0];
    dest[1] = dest1[1];
    dest[2] = dest1[2];
}


//
// V4
//

void rtgV4MultiplyM12( float dest[4], const float v4[4], float m12[12] )
{
    float *dest1 = dest;
    float fTmp[4];

    if( dest==v4 ) dest1 = fTmp;

    dest1[0] = v4[0]*m12[0] + v4[1]*m12[3] + v4[2]*m12[6] + v4[3]*m12[9];
    dest1[1] = v4[0]*m12[1] + v4[1]*m12[4] + v4[2]*m12[7] + v4[3]*m12[10];
    dest1[2] = v4[0]*m12[2] + v4[1]*m12[5] + v4[2]*m12[8] + v4[3]*m12[11];
    dest1[3] = v4[3];

    if( dest1==dest ) return;

    dest[0] = dest1[0];
    dest[1] = dest1[1];
    dest[2] = dest1[2];
    dest[3] = dest1[3];
}

/*
RtgVertex3 operator*( const RtgVertex3 &v, const AzmM16 &m )
{
    return v.V4Multiply(m);
}
*/

void RtgVertex2::Output()
{
    RtCoreLog().Info("(%7.4f, %7.4f)", x, y);
}

void RtgVertex3::Output()
{
	RtCoreLog().Info("(%7.4f, %7.4f, %7.4f)", x, y, z);
}

void RtgVertex4::Output()
{
	RtCoreLog().Info("(%7.4f, %7.4f, %7.4f, %7.4f)", x, y, z, w);
}

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
