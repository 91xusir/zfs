/**
* @file rtc2_math.h ��ѧ���.
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

/// ת��������������(��������)
inline int rtRound( float F )
{
    return (int)(F + 0.5f);
}

/// ת��������������(��������)
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

/// ת��������������(С���㶪��)
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

/// ȡ����ֵ
template< class T > inline T Abs( const T A )
{
	return (A>=(T)0) ? A : -A;
}
/// ȡ�÷���
template< class T > inline T Sgn( const T A )
{
	return (A>0) ? 1 : ((A<0) ? -1 : 0);
}
/// ȡ�ô�ֵ
template< class T > inline T Max( const T A, const T B )
{
	return (A>=B) ? A : B;
}
/// ȡ��Сֵ
template< class T > inline T Min( const T A, const T B )
{
	return (A<=B) ? A : B;
}
/// ȡ��ƽ��
template< class T > inline T Square( const T A )
{
	return A*A;
}
/// ȡ�÷�Χ�ڵ�ֵ
template< class T > inline T Clamp( const T X, const T Min, const T Max )
{
	return X<Min ? Min : X<Max ? X : Max;
}
/// ָ�����
template< class T > inline T Align( const T Ptr, int Alignment )
{
	return (T)(((DWORD)Ptr + Alignment - 1) & ~(Alignment-1));
}
/// ����
template< class T > inline void Exchange( T& A, T& B )
{
	const T Temp = A;
	A = B;
	B = Temp;
}
/// �ڲ�ֵ�����A��B֮������Alphaֵ��ֵ
template< class T > T Lerp( T& A, T& B, float Alpha )
{
	return A + Alpha * (B-A);
}

/// �Ƿ�ͬ��
template< class T > T SameSign( T& A, T& B )
{
	return ( (A>=0 && B>=0) || (A<0 && B<0) );
}

/// �Ƿ�Լ����0
inline BOOL IsZero( float X )
{
	return (Abs(X)<DELTA);
}

/// ���� -> �Ƕ�
inline float RadianToDegree( float X )
{
	return (180.0f/PI*(X));
}

/// �Ƕ� -> ����
inline float DegreeToRadian( float X )
{
	return (PI/180.0f*(X));
}

/*------------------------------------------------------------------------
-   Random.
------------------------------------------------------------------------*/

RT2_CORE_API void rtRandomSeed(unsigned int seed);   ///< �������
RT2_CORE_API int rtRandom();                         ///< return 0 through 32767.
RT2_CORE_API int rtRandom(int iMax, int iMin);       ///< ��Χȡֵ, ���ܵ���iMin����ԶС��iMax (����iMin==iMax)

/// ���������
//    �ù̶���������ӾͿ��Բ���һ���̶����������
class RT2_CORE_API RtRandom
{
public:
    RtRandom();

    /// ���캯�������Դ���һ���������
    RtRandom(unsigned int iSeed);

    /// �����漴����
    void SetSeed(unsigned int iSeed);

    /// ȡ��һ��0��32768֮��������(������32768)
    int Get();
    /// ȡ��һ��0��iMax֮��������(������iMax)
    int Get(int iMax);
    /// ȡ��һ��iMin��iMax֮��������(������iMax)
    int Get(int iMin, int iMax);

private:
    long m_lSeed;
};

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
