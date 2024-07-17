/**
* @file rtc2_math.h 数学相关.
* @author janhail
*/
#include <math.h>

namespace rt2_core {

struct float3
{
    union {
        struct {
            float x;
            float y;
            float z;
        };

        float v[3];
    };

    float3(float _x = 0.f, float _y = 0.f, float _z = 0.f)
        : x(_x), y(_y), z(_z)
    {
    }

     operator float* ()
    {
        return v;
    }
};

#ifndef fsin
#define fsin(x) (float)sin((x))
#endif

#ifndef fcos
#define fcos(x) (float)cos((x))
#endif

#ifndef ftan
#define ftan(x) (float)tan((x))
#endif

#ifndef fsqrt
#define fsqrt(x) (float)sqrt((x))
#endif

#ifndef fasin
#define fasin(x) (float)asin((x))
#endif 

#ifndef facos
#define facos(x) (float)acos((x))
#endif

#ifndef fatan
#define fatan(x) (float)atan((x))
#endif

#ifndef fatan2
#define fatan2(y, x) (float)atan2((y), (x))
#endif

#ifndef PI
const float PI    = 3.14159265f;
#endif
const float PIx2  = PI*2;
const float DELTA = 0.00001;

/// 转换浮点数到整数(四舍五入)
inline int rtRound( float F )
{
    return (int)(F + 0.5f);
}

/// 转换浮点数到整数(四舍五入)
union rt_core_num_Cast { double l_d; long l_l; };
inline int rtRound( DOUBLE F )
{
#ifdef WIN32
	return (int)ceil(F);
#else
    volatile union rt_core_num_Cast u;
    u.l_d = F + 6755399441055744.0;
    return u.l_l;
#endif
}

/// 转换浮点数到整数(小数点丢弃)
inline int rtFloor( float F )
{
   return (int)F;
}

class RT2_CORE_API RtGlobalMath
{
public:
	// Constants.
	enum {ANGLE_SHIFT 	= 2};		// Bits to right-shift to get lookup value.
	enum {ANGLE_BITS	= 14};		// Number of valid bits in angles.
	enum {NUM_ANGLES 	= 16384}; 	// Number of angles that are in lookup table.
	enum {NUM_SQRTS		= 16384};	// Number of square roots in lookup table.
	enum {ANGLE_MASK    =  (((1<<ANGLE_BITS)-1)<<(16-ANGLE_BITS))};

    // Basic math functions.
	float Sqrt( int i )
	{
		return SqrtFLOAT[i]; 
	}
	float SinTab( int i )
	{
		return TrigFLOAT[((i>>ANGLE_SHIFT)&(NUM_ANGLES-1))];
	}
	float CosTab( int i )
	{
		return TrigFLOAT[(((i+16384)>>ANGLE_SHIFT)&(NUM_ANGLES-1))];
	}
	float SinFloat( float F )
	{
		return SinTab(rtRound((F*65536.f)/(2.f*PI)));
	}
	float CosFloat( float F )
	{
		return CosTab(rtRound((F*65536.f)/(2.f*PI)));
	}

	// Constructor.
	RtGlobalMath();

private:
	// Tables.
	float  TrigFLOAT		[NUM_ANGLES];
	float  SqrtFLOAT		[NUM_SQRTS];
	float  LightSqrtFLOAT	[NUM_SQRTS];
};

/// 取绝对值
template< class T > inline T Abs( const T A )
{
	return (A>=(T)0) ? A : -A;
}
/// 取得符号
template< class T > inline T Sgn( const T A )
{
	return (A>0) ? 1 : ((A<0) ? -1 : 0);
}
/// 取得大值
template< class T > inline T Max( const T A, const T B )
{
	return (A>=B) ? A : B;
}
/// 取得小值
template< class T > inline T Min( const T A, const T B )
{
	return (A<=B) ? A : B;
}
/// 取得平方
template< class T > inline T Square( const T A )
{
	return A*A;
}
/// 取得范围内的值
template< class T > inline T Clamp( const T X, const T Min, const T Max )
{
	return X<Min ? Min : X<Max ? X : Max;
}
/// 指针对齐
template< class T > inline T Align( const T Ptr, int Alignment )
{
	return (T)(((DWORD)Ptr + Alignment - 1) & ~(Alignment-1));
}
/// 交换
template< class T > inline void Exchange( T& A, T& B )
{
	const T Temp = A;
	A = B;
	B = Temp;
}
/// 内插值，求得A到B之间走了Alpha值的值
template< class T > T Lerp( T& A, T& B, float Alpha )
{
	return A + Alpha * (B-A);
}

/// 是否同号
template< class T > T SameSign( T& A, T& B )
{
	return ( (A>=0 && B>=0) || (A<0 && B<0) );
}

/// 是否约等于0
inline BOOL IsZero( float X )
{
	return (Abs(X)<DELTA);
}

/// 弧度 -> 角度
inline float RadianToDegree( float X )
{
	return (180.0f/PI*(X));
}

/// 角度 -> 弧度
inline float DegreeToRadian( float X )
{
	return (PI/180.0f*(X));
}

/*------------------------------------------------------------------------
-   Random.
------------------------------------------------------------------------*/

RT2_CORE_API void rtRandomSeed(unsigned int seed);   ///< 随机种子
RT2_CORE_API int rtRandom();                         ///< return 0 through 32767.
RT2_CORE_API int rtRandom(int iMax, int iMin);       ///< 范围取值, 可能等于iMin，永远小于iMax (除了iMin==iMax)

/// 随机函数类
//    用固定的随机种子就可以产生一个固定的随机序列
class RT2_CORE_API RtRandom
{
public:
    RtRandom();

    /// 构造函数，可以传入一个随机种子
    RtRandom(unsigned int iSeed);

    /// 设置随即种子
    void SetSeed(unsigned int iSeed);

    /// 取得一个0到32768之间的随机数(不包括32768)
    int Get();
    /// 取得一个0到iMax之间的随机数(不包括iMax)
    int Get(int iMax);
    /// 取得一个iMin到iMax之间的随机数(不包括iMax)
    int Get(int iMin, int iMax);

private:
    long m_lSeed;
};

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
