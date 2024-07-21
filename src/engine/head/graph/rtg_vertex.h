
namespace rt_graph {

    struct Int2
    {
        Int2(int _x, int _y)
            : x(_x),
              y(_y)
        {
        }

        Int2()
        {
        }

        void set(int _x, int _y)
        {
            x = _x;
            y = _y;
        }

        bool operator < (const Int2& _int2) const
        {
            if (x < _int2.x)
                return true;
            if (x == _int2.x)
                return y < _int2.y;
            return false;
        }

        bool operator == (const Int2& _int2) const
        {
            return (x == _int2.x) && (y == _int2.y);
        }

        bool operator != (const Int2& _int2) const
        {
            return (x != _int2.x) || (y != _int2.y);
        }

        int x;
        int y;
    };

    class RtgVertex2
    {
    public:
        union
        {
            float m_v[2];
            struct{ float x, y;};
        };
        
    public:
	    RtgVertex2(void) {}
	    RtgVertex2(float x, float y) { this->x=x; this->y=y; }
	    inline void  Set(float x, float y) { this->x=x; this->y=y; }
        inline void  Set(const RtgVertex2& v) { x = v.x; y = v.y; }
	    inline void  Get(float *ptr) { ptr[0] = x; ptr[1] = y; }
	    inline void  Normalize(void);
	    inline void  Negative(void) { x=-x; y=-y; }
        inline float Length(void);
	    inline float Distance(RtgVertex2 &v);
	    inline void  Clip(void)
	    {
		    if ( x<0 ) x = x-(int)x;
		    if ( x>1.0f ) x = x - (int)x;
		    if ( y<0 ) y = y-(int)y;
		    if ( y>1.0f ) y = y - (int)y;
	    }
	    RtgVertex2 &operator+=(const RtgVertex2 &rhs)
	    {
		    x+=rhs.x; y+=rhs.y; return *this;
	    }
	    RtgVertex2 &operator-=(const RtgVertex2 &rhs)
	    {
		    x-=rhs.x; y-=rhs.y; return *this;
	    }

        void Output();
	    friend RtArchive& operator<<(RtArchive &Ar, RtgVertex2 &v) 
	    {
		    return Ar << v.x << v.y;
	    }

    };

    ///////////////////////////////////////////////////////////////////////////////////////
    // RtgVertex3

    class RtgVertex3
    {
    public:
        union
        {
            float m_v[3];
            struct{ float x, y, z;};
        };

    public:
        RtgVertex3() {}
        RtgVertex3( float v0, float v1, float v2 )   { m_v[0]=v0; m_v[1]=v1; m_v[2]=v2; }
        RtgVertex3( const float v[3] )               { m_v[0]=v[0]; m_v[1]=v[1]; m_v[2]=v[2]; }
        RtgVertex3( const RtgVertex3 &rhs )           { m_v[0]=rhs[0]; m_v[1]=rhs[1]; m_v[2]=rhs[2]; }
        RtgVertex3(const float3& _f3)
            : x(_f3.x), y(_f3.y), z(_f3.z)
        {
        }
        ~RtgVertex3() {}

        inline BOOL      IfZero   ( void )  { if( m_v[0] != 0.0f || m_v[1] != 0.0f || m_v[2] != 0.0f ) return FALSE; else return TRUE; }
        inline void      Zero     ( void );
        inline void      Normalize( void );
        //反转
	    inline void      Negative (void)    { x=-x; y=-y; z=-z;}
        inline float     Length   ( void ) const ;
        inline float     Length2  ( void );
	    inline float     Distance (RtgVertex3 &rhs);
	    inline float     Distance2(RtgVertex3 &rhs);
        inline RtgVertex3 Cross( const RtgVertex3 &rhs ) const;
        inline float     Dot  ( const RtgVertex3 &rhs ) const;
        inline void      Dump ( float v[3] );
        inline void      Set  ( float fx, float fy, float fz )  { x=fx, y=fy, z=fz;}
        inline void      Set  ( float fValue )                  { x = y = z = fValue;}
        inline void      Set  ( float* pValue )                 { x=pValue[0]; y=pValue[1]; z=pValue[2];}
	    inline void      Load ( float *pPtr )                   { pPtr[0] = x; pPtr[1] = y; pPtr[2] = z; }

        inline RtgVertex3   & operator+=( const RtgVertex3 &rhs );
        inline RtgVertex3   & operator-=( const RtgVertex3 &rhs );
        inline RtgVertex3   & operator= ( const RtgVertex3 &rhs );
        inline RtgVertex3     operator+ ( const float v[3] ) const;
        inline RtgVertex3     operator- ( const float v[3] ) const;
        inline RtgVertex3   & operator+=( const float v[3] );
        inline RtgVertex3   & operator-=( const float v[3] );
        inline RtgVertex3   & operator= ( const float v[3] );
        //inline RtgVertex3     operator/ ( float f ) const;
        inline RtgVertex3     operator*=( float f );
        inline RtgVertex3     operator/=( float f );
        inline float       & operator[]( int i )             { return m_v[i]; }
        inline const float & operator[]( int i ) const       { return m_v[i]; }
        inline               operator float * ()             { return m_v; }
        inline               operator const float * () const { return m_v; }
        inline RtgVertex3 operator - ()
        {
            return RtgVertex3(-x, -y, -z);
        }

        void Output();
	    friend RtArchive& operator<<(RtArchive &Ar, RtgVertex3 &v) 
	    {
		    return Ar << v.x << v.y << v.z;
	    }

        inline RtgVertex3& operator = (const float3& _f3)
        {
            x = _f3.x;
            y = _f3.y;
            z = _f3.z;
            return *this;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////
    // RtgVertex4

    class RtgVertex4
    {
    public:
        union
        {
            float m_v[4];
            struct{ float x, y, z, w;};
        };

    public:
	    RtgVertex4(void) {}
	    RtgVertex4(float a, float b, float c, float d) { x=a; y=b; z=c; w=d; }
        inline float Length3();
	    inline void Normalize3(void);
	    inline void Normalize4(void);
	    inline void Set(float a, float b, float c, float d) { x=a; y=b; z=c; w=d; }
	    inline void Set(float a) { x=y=z=w=a; }
	    void Output();
	    friend RtArchive& operator<<(RtArchive &Ar, RtgVertex4 &v) 
	    {
		    return Ar << v.x << v.y << v.z << v.w;
	    }

        void PlaneNormalize()
        {
            float t = fsqrt(x * x + y * y + z * z);
            x /= t;
            y /= t;
            z /= t;
            w /= t;
        }

        void MakePlane(const RtgVertex3& v0, const RtgVertex3& v1, const RtgVertex3& v2)
        {
            RtgVertex3 e0 = v1 - v0;
            RtgVertex3 e1 = v2 - v0;

            RtgVertex3 _normal = e0.Cross(e1);
            _normal.Normalize();
            x = _normal.x;
            y = _normal.y;
            z = _normal.z;
            w = -_normal.Dot(v0);
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////
    // RtgOrientation

    class RtgOrientation
    {
    public:
        union
        {
            float m_v[4];
            struct{ float x, y, z, ang;};
        };
	    RtgOrientation(void) {}
	    RtgOrientation(float x, float y, float z, float ang) { this->x = x; this->y = y; this->z = z; this->ang = ang; }
	    void Set(float x, float y, float z, float ang) { this->x = x; this->y = y; this->z = z; this->ang = ang; }
	    void Set(float value) { x = y = z = ang = value; }
	    inline float Normalize(void);
    };


    ///////////////////////////////////////////////////////////////////////////////////////
    // Vector 实现

    inline float rtgVectorLength(const float v[3])
    {
        return fsqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
    }

    inline float rtgVectorLengthSqr(const float v[3])
    {
        return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    }

    inline float rtgVectorLength(float x, float y, float z)
    {
        return fsqrt(x*x + y*y + z*z);
    }

    inline float rtgVectorLengthSqr(float x, float y, float z)
    {
        return x*x + y*y + z*z;
    }

    inline void rtgVectorNormalize(float v[3])
    {
        float fLen = rtgVectorLength( v );
        CHECK( fLen != 0.0f );
        v[0] /= fLen; v[1] /= fLen; v[2] /= fLen;
    }

    inline float rtgVectorLength2D(const float v[2])
    {
        return fsqrt(v[0]*v[0]+v[1]*v[1]);
    }

    inline float rtgVectorLength2DSqr(const float v[2])
    {
        return v[0]*v[0] + v[1]*v[1];
    }

    inline void rtgVectorNormalize2D(float v[2])
    {
        float fLen = rtgVectorLength2D( v );
        CHECK( fLen != 0.0f );
        v[0] /= fLen; v[1] /= fLen;
    }

    // v3 = v1 x v2
    inline void rtgVectorCross(float dest[3], const float v1[3], const float v2[3])
    {
        float fff[3];
        float *dest1 = dest;
        if( dest1==v1 || dest1==v2 ) dest1 = fff;
        dest1[0] = v1[1] * v2[2] - v1[2] * v2[1];
        dest1[1] = v1[2] * v2[0] - v1[0] * v2[2];
        dest1[2] = v1[0] * v2[1] - v1[1] * v2[0];
        if( dest1!=dest )
        {
            dest[0] = dest1[0];
            dest[1] = dest1[1];
            dest[2] = dest1[2];
        }
    }

    // v3 = v1 x v2, & do normalize
    inline void rtgVectorCrossN(float dest[3], const float v1[3], const float v2[3])
    {
        float fff[3];
        float *dest1 = dest;
        if( dest1==v1 || dest1==v2 ) dest1 = fff;
        dest1[0] = v1[1] * v2[2] - v1[2] * v2[1];
        dest1[1] = v1[2] * v2[0] - v1[0] * v2[2];
        dest1[2] = v1[0] * v2[1] - v1[1] * v2[0];
        rtgVectorNormalize(dest1);
        if( dest1!=dest )
        {
            dest[0] = dest1[0];
            dest[1] = dest1[1];
            dest[2] = dest1[2];
        }
    }

    inline RtgVertex3 rtgVectorCross(RtgVertex3 &a, RtgVertex3 &b)
    {
	    RtgVertex3 out;
	    rtgVectorCross(out, a, b);
	    return out;
    }

    // return v1 . v2
    inline float rtgVectorDot(const float v1[3], const float v2[3])
    {
        return (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
    }

    inline void rtgVectorZero( float dest[3] )
    {
        dest[0] = 0.0f;
        dest[1] = 0.0f;
        dest[2] = 0.0f;
    }

    inline void rtgVectorAssign( float dest[3], const float v1[3] )
    {
        dest[0] = v1[0];
        dest[1] = v1[1];
        dest[2] = v1[2];
    }

    // return v1 - v2
    inline void rtgVectorSub( float dest[3], const float v1[3], const float v2[3] )
    {
        dest[0] = v1[0]-v2[0];
        dest[1] = v1[1]-v2[1];
        dest[2] = v1[2]-v2[2];
    }

    // return v1 + v2
    inline void rtgVectorAdd( float dest[3], const float v1[3], const float v2[3] )
    {
        dest[0] = v1[0]+v2[0];
        dest[1] = v1[1]+v2[1];
        dest[2] = v1[2]+v2[2];
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    // RtgVertex2 实现

    float RtgVertex2::Length(void)
    {
	    float poweradd = x*x + y*y;
	    return (float) fsqrt( poweradd );
    }

    void RtgVertex2::Normalize()
    {
        float poweradd = x*x + y*y;
        CHECK( poweradd!=0.0f );
        if ( poweradd == 1.0f ) return;
        float len = (float) fsqrt( poweradd );
        x/=len;
        y/=len;
    }

    float RtgVertex2::Distance(RtgVertex2 &v)
    {
	    float dx = v.x - x;
	    float dy = v.y - y;
	    return (float) fsqrt( dx*dx + dy*dy );  
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    // RtgVertex3 实现

    void RtgVertex3::Zero( void )
    {
        m_v[0] = m_v[1] = m_v[2] = 0.0f;
    }

    float RtgVertex3::Length( void ) const
    {
        return fsqrt(m_v[0]*m_v[0] + m_v[1]*m_v[1] + m_v[2]*m_v[2]);
    }

    float RtgVertex3::Length2( void )
    {
        return m_v[0]*m_v[0] + m_v[1]*m_v[1] + m_v[2]*m_v[2];
    }

    float RtgVertex3::Distance(RtgVertex3 &rhs)
    {
	    float dx = rhs.x - x;
	    float dy = rhs.y - y;
	    float dz = rhs.z - z;
	    return (float) fsqrt( dx*dx + dy*dy + dz*dz );
    }

    float RtgVertex3::Distance2(RtgVertex3 &rhs)
    {
	    float dx = rhs.x - x;
	    float dy = rhs.y - y;
	    float dz = rhs.z - z;
	    return dx*dx + dy*dy + dz*dz;
    }

    void RtgVertex3::Normalize( void )
    {
        float fLen = Length();
        if( fLen > DELTA )
        {
            m_v[0] /= fLen; m_v[1] /= fLen; m_v[2] /= fLen;
        }else
        {
            m_v[0] = 1.f;
            m_v[1] = m_v[2] = 0.f;
    //#ifdef _DEBUG
    //        RtCoreLog().Warn("RtgVertex3::Normalize all items is ZERO.\n");
    //#endif
        }
    }

    RtgVertex3 RtgVertex3::Cross( const RtgVertex3 &rhs ) const
    {
        return RtgVertex3(m_v[1]*rhs[2] - m_v[2]*rhs[1], m_v[2]*rhs[0] - m_v[0]*rhs[2], m_v[0]*rhs[1] - m_v[1]*rhs[0]);
    }

    float RtgVertex3::Dot( const RtgVertex3 &rhs ) const
    {
        return (m_v[0]*rhs[0] + m_v[1]*rhs[1] + m_v[2]*rhs[2]);
    }

    void RtgVertex3::Dump( float v[3] )
    {
        v[0] = m_v[0];
        v[1] = m_v[1];
        v[2] = m_v[2];
    }

    RtgVertex3 RtgVertex3::operator+( const float v[3] ) const
    {
        return RtgVertex3( m_v[0]+v[0], m_v[1]+v[1], m_v[2]+v[2] );
    }

    RtgVertex3 RtgVertex3::operator-( const float v[3] ) const
    {
        return RtgVertex3( m_v[0]-v[0], m_v[1]-v[1], m_v[2]-v[2] );
    }

    RtgVertex3& RtgVertex3::operator+=( const RtgVertex3 &rhs )
    {
        m_v[0] += rhs[0];
        m_v[1] += rhs[1];
        m_v[2] += rhs[2];
        return *this;
    }

    RtgVertex3& RtgVertex3::operator+=( const float v[3] )
    {
        m_v[0] += v[0];
        m_v[1] += v[1];
        m_v[2] += v[2];
        return *this;
    }

    RtgVertex3& RtgVertex3::operator-=( const RtgVertex3 &rhs )
    {
        m_v[0] -= rhs[0];
        m_v[1] -= rhs[1];
        m_v[2] -= rhs[2];
        return *this;
    }

    RtgVertex3& RtgVertex3::operator-=( const float v[3] )
    {
        m_v[0] -= v[0];  m_v[1] -= v[1];  m_v[2] -= v[2];
        return *this;
    }

    RtgVertex3 RtgVertex3::operator*=( float f )
    {
        m_v[0] *= f;  m_v[1] *= f;  m_v[2] *= f;
        return *this;
    }

    //RtgVertex3 RtgVertex3::operator/( float f ) const
    //{
    //    CHECK( f!=0.0f );
    //    return RtgVertex3( m_v[0]/f, m_v[1]/f, m_v[2]/f );
    //}

    RtgVertex3 RtgVertex3::operator/=( float f )
    {
        CHECK( f!=0.0f );
        m_v[0] /= f;  m_v[1] /= f;  m_v[2] /= f;
        return *this;
    }

    RtgVertex3& RtgVertex3::operator=( const RtgVertex3 &rhs )
    {
        m_v[0] = rhs[0];
        m_v[1] = rhs[1];
        m_v[2] = rhs[2];
        return *this;
    }

    RtgVertex3& RtgVertex3::operator=( const float v[3] )
    {
        m_v[0] = v[0];
        m_v[1] = v[1];
        m_v[2] = v[2];
        return *this;
    }


    ///////////////////////////////////////////////////////////////////////////////////////
    // RtgVertex4 实现
    inline RtgVertex4 operator- (const RtgVertex4 &v1,const RtgVertex4 &v2)
    {
	    RtgVertex4 t;
	    t.x = v1.x - v2.x;
	    t.y = v1.y - v2.y;
	    t.z = v1.z - v2.z;
	    t.w = v1.w - v2.w;
	    return t;
    }

    inline RtgVertex4 operator+ (const RtgVertex4 &v1,const RtgVertex4 &v2)
    {
	    RtgVertex4 t;
	    t.x = v1.x + v2.x;
	    t.y = v1.y + v2.y;
	    t.z = v1.z + v2.z;
	    t.w = v1.w + v2.w;
	    return t;
    }

    inline RtgVertex4 operator* ( const float f,const RtgVertex4 &v)
    {
	    RtgVertex4 t;
	    t.x = f*v.x;
	    t.y = f*v.y;
	    t.z = f*v.z;
	    t.w = f*v.w;
	    return t;
    }

    float RtgVertex4::Length3()
    {
	    float poweradd = x*x + y*y + z*z;
	    if ( poweradd==0.0f ) return 0.f;
	    return (float) fsqrt( poweradd );
    }

    void RtgVertex4::Normalize3(void)
    {
	    float poweradd = x*x + y*y + z*z;
	    if ( poweradd == 1.0f || poweradd==0.0f ) return;
	    float len = (float) fsqrt( poweradd );
	    x/=len;
	    y/=len;
	    z/=len;
    }

    void RtgVertex4::Normalize4(void)
    {
	    float poweradd = x*x + y*y + z*z + w*w;
	    if ( poweradd == 1.0f || poweradd==0.0f ) return;
	    float len = (float) fsqrt( poweradd );
	    x/=len;
	    y/=len;
	    z/=len;
	    w/=len;
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    // RtgOrientation 实现

    float RtgOrientation::Normalize(void)
    {
	    float len = fsqrt(x*x + y*y + z*z);
	    if ( len==1.f || len==0.f ) return len;
	    x /= len;
	    y /= len;
	    z /= len;
	    return len;
    }


    // 其他 operator

    inline RtgOrientation operator+(const RtgOrientation &l, const RtgOrientation &r)
    {
	    RtgOrientation ans(l.x + r.x, l.y + r.y, l.z + r.z, l.ang + r.ang);
	    return ans;
    }

    inline RtgOrientation operator-(const RtgOrientation &l, const RtgOrientation &r)
    {
	    RtgOrientation ans(l.x - r.x, l.y - r.y, l.z - r.z, l.ang - r.ang);
	    return ans;
    }

    inline RtgOrientation operator*(const RtgOrientation &l, const float &r)
    {
	    RtgOrientation ans(l.x * r, l.y * r, l.z * r, l.ang * r);
	    return ans;
    }

    inline RtgOrientation operator*(const float &l, const RtgOrientation &r)
    {
	    RtgOrientation ans(l * r.x, l * r.y, l * r.z, l * r.ang);
	    return ans;
    }

    inline RtgVertex2 operator+(const RtgVertex2 &a, const RtgVertex2 &b)
    {
	    RtgVertex2 ans(a.x + b.x, a.y + b.y);
	    return ans;
    }

    inline RtgVertex2 operator-(const RtgVertex2 &a, const RtgVertex2 &b)
    {
	    RtgVertex2 ans(a.x - b.x, a.y - b.y);
	    return ans;
    }

    inline float operator*(const RtgVertex2 &a, const RtgVertex2 &b)
    {
	    return (a.x*b.x + a.y*b.y);
    }

    inline RtgVertex2 operator*(const float a, const RtgVertex2 &b)
    {
	    RtgVertex2 ans(a*b.x, a*b.y);
	    return ans;
    }

    inline RtgVertex3 operator+(const RtgVertex3 &a, const RtgVertex3 &b)
    {
	    return RtgVertex3(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    // dot production
    inline float operator*(const RtgVertex3 &a, const RtgVertex3 &b)
    {
	    return (a.x*b.x + a.y*b.y + a.z*b.z);
    }

    inline RtgVertex3 operator-(const RtgVertex3 &a, const RtgVertex3 &b)
    {
	    return RtgVertex3(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    inline RtgVertex3 operator*(const RtgVertex3 &a, const float f)
    {
	    RtgVertex3 c;
	    c.x = a.x*f;
	    c.y = a.y*f;
	    c.z = a.z*f;
	    return c;
    }

    inline RtgVertex3 operator/(const RtgVertex3 &a, const float f)
    {
	    RtgVertex3 c;
	    c.x = a.x/f;
	    c.y = a.y/f;
	    c.z = a.z/f;
	    return c;
    }

    inline RtgVertex3 operator*(const float f, const RtgVertex3 &a)
    {
	    RtgVertex3 c;
	    c.x = a.x*f;
	    c.y = a.y*f;
	    c.z = a.z*f;
	    return c;
    }

    inline bool operator==(const RtgVertex3 &a, const RtgVertex3 &b)
    {
	    if ( a.x!=b.x || a.y!=b.y || a.z!=b.z )
		    return false;
	    else
		    return true;
    }
    inline bool operator!=(const RtgVertex3 &a, const RtgVertex3 &b)
    {
	    if ( a.x==b.x && a.y==b.y && a.z==b.z )
		    return false;
	    else
		    return true;
    }
    inline bool operator==(const RtgVertex4 &a, const RtgVertex4 &b)
    {
	    if ( a.x!=b.x || a.y!=b.y || a.z!=b.z || a.w!=b.w )
		    return false;
	    else
		    return true;
    }
    inline bool operator!=(const RtgVertex4 &a, const RtgVertex4 &b)
    {
	    if ( a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w )
		    return false;
	    else
		    return true;
    }
    inline void Normalize4f(float v[4])
    {
	    float len = (float) fsqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
	    if ( len==1.0f )
		    return;
	    else if ( len==0.0f )
		    return;
	    v[0]/=len;
	    v[1]/=len;
	    v[2]/=len;
	    v[3]/=len;
    }


    ///////////////////////////////////////////////////////////////////////////////////////
    // Quat 函数

    void rtgQuatToM9(float m[9], const float vt[3], const float w);
    void rtgQuatNormalize(float vt[3], float &w);
    void rtgFaceRollToM9(float m[9], float face[3], float roll);
    void RotationToQuaternion( RtgVertex4 &in, RtgVertex4 &out );
    void QuaternionToRotation( RtgVertex4 &in, RtgVertex4 &out );

    ///////////////////////////////////////////////////////////////////////////////////////
    // Vector 函数

    inline void rtgVectorNormalize(float v[3]);
    inline void rtgVectorNormalize2D(float v[2]);
    inline void rtgVectorCross(float dest[3], const float v1[3], const float v2[3]);
    inline void rtgVectorCrossN(float dest[3], const float v1[3], const float v2[3]);
    inline RtgVertex3 rtgVectorCross(RtgVertex3 &a, RtgVertex3 &b);
    inline void rtgVectorZero( float dest[3] );
    inline void rtgVectorAssign( float dest[3], const float v1[3] );
    inline void rtgVectorSub( float dest[3], const float v1[3], const float v2[3] );
    inline void rtgVectorAdd( float dest[3], const float v1[3], const float v2[3] );
    inline float rtgVectorLength(const float v[3]);
    inline float rtgVectorLengthSqr(const float v[3]);
    inline float rtgVectorLength(float x, float y, float z);
    inline float rtgVectorLengthSqr(float x, float y, float z);
    inline float rtgVectorLength2D(const float v[2]);
    inline float rtgVectorLength2DSqr(const float v[2]);
    inline float rtgVectorDot(const float v1[3], const float v2[3]);

    float rtgVectorAngle( const float v1[3], const float v2[3] );
    float rtgVectorSAngle( const float v1[3], const float v2[3], const float refereeV[3] );
    float rtgVector2DAngle( const float v1[2], const float v2[2] );
    float rtgVector2DSAngle( const float v1[2], const float v2[2] );
    void  rtgVectorRotate( float dest[3], const float vect[3], const float ref[3], float degree );
    void  rtgVectorRotateXAxis( float dest[3], const float vect[3], float degree );
    void  rtgVectorRotateYAxis( float dest[3], const float vect[3], float degree );
    void  rtgVectorRotateZAxis( float dest[3], const float vect[3], float degree );

    void  rtgNormalVectorOfTriangle( float n[3], const float triangle[9] );

    void  rtgV3MultiplyM9 ( float dest[3], const float v3[3], const float m9[9] );
    void  rtgV3MultiplyM12( float dest[3], const float v3[3], const float m12[12] );
    void  rtgV4MultiplyM12( float dest[4], const float v4[4], float m12[12] );

    inline RtgOrientation operator+(const RtgOrientation &l, const RtgOrientation &r);
    inline RtgOrientation operator-(const RtgOrientation &l, const RtgOrientation &r);
    inline RtgOrientation operator*(const RtgOrientation &l, const float &r);
    inline RtgOrientation operator*(const float &l, const RtgOrientation &r);

    inline RtgVertex2 operator+(const RtgVertex2 &a, const RtgVertex2 &b);
    inline RtgVertex2 operator-(const RtgVertex2 &a, const RtgVertex2 &b);
    inline float     operator*(const RtgVertex2 &a, const RtgVertex2 &b);
    inline RtgVertex2 operator*(const float a, const RtgVertex2 &b);

    inline RtgVertex3 operator+(const RtgVertex3 &a, const RtgVertex3 &b);
    inline RtgVertex3 operator-(const RtgVertex3 &a, const RtgVertex3 &b);
    inline float     operator*(const RtgVertex3 &a, const RtgVertex3 &b); 
    inline RtgVertex3 operator*(const RtgVertex3 &a, const float f);
    inline RtgVertex3 operator*(const float f, const RtgVertex3 &a);
    inline RtgVertex3 operator/(const RtgVertex3 &a, const float f);
    inline bool operator==(const RtgVertex3 &a, const RtgVertex3 &b);
    inline void Normalize4f(float v[4]);

    RtgVertex4 Slerp(RtgVertex4 &from, RtgVertex4 &to, float t);
    RtgVertex3 GetUpVecFromFront(RtgVertex3 front);


} 
