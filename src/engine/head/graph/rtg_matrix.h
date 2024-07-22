namespace rt_graph {

///////////////////////////////////////////////////////////////////////////////////////
// Matrix M12 函数

void  rtgM12MakeUnit   ( float dest[12] );
void  rtgM12Assign     ( float dest[12], float m1[12] );
void  rtgM12AssignM16  ( float dest[12], float m1[16] );
void  rtgM12Multiply   ( float dest[12], float m1[3], float m2[3] );
void  rtgM12RotateX    ( float dest[12], float degree );
void  rtgM12RotateY    ( float dest[12], float degree );
void  rtgM12RotateZ    ( float dest[12], float degree );
void  rtgM12MakeRotateX( float dest[12], float degree );
void  rtgM12MakeRotateY( float dest[12], float degree );
void  rtgM12MakeRotateZ( float dest[12], float degree );
void  rtgM12MakeInverse( float dest[12], float m[12] );
void  rtgM12MakeArbitraryRotate( float dest[12], float axis[3], float v[3], float degree );
void  rtgM12MakeTransformationYZ( float dest[12], float pos[3], float y[3], float z[3] );
void  rtgMakeMatrix(float *M,float *pos, float *fDir, float *uDir, float *scale);

class RtgMatrix12
{
public:
    union
    {
        float m_m[12];
        struct
        {
            float _00, _01, _02;
            float _10, _11, _12;
            float _20, _21, _22;
            float _30, _31, _32;
        };
    };

    static RtgMatrix12 matIdentity;

public:
    RtgMatrix12() { Unit(); }
    RtgMatrix12( const RtgMatrix12 & );
    RtgMatrix12( const float m[12] );
    RtgMatrix12(float _00, float _01, float _02,
                float _10, float _11, float _12,
                float _20, float _21, float _22,
                float _30, float _31, float _32
                );
    RtgMatrix12( const float face[3], const float up[3], const float right[3] = NULL, const float pos[3] = NULL );
    ~RtgMatrix12() {}

    void Zero( void );
    void Unit( void );
    void Dump( float m[12] ) const;
    std::string ToString() const;
    RtgMatrix12   operator+ ( const RtgMatrix12 &rhs );
    RtgMatrix12   operator- ( const RtgMatrix12 &rhs );
    RtgMatrix12 & operator+=( const RtgMatrix12 &rhs );
    RtgMatrix12 & operator-=( const RtgMatrix12 &rhs );
    RtgMatrix12 & operator= ( const RtgMatrix12 &rhs );
    RtgMatrix12 & operator= ( const RtgMatrix16 &rhs );
    RtgMatrix12   operator+ ( const float m[12] );
    RtgMatrix12   operator- ( const float m[12] );
    RtgMatrix12 & operator+=( const float m[12] );
    RtgMatrix12 & operator-=( const float m[12] );
    RtgMatrix12 & operator= ( const float m[12] );
    RtgMatrix12   operator* ( const RtgMatrix12 &rhs ) const;
    RtgMatrix12   operator* ( float f );
    RtgMatrix12   operator/ ( float f );
    RtgMatrix12   operator*=( float f );
    RtgMatrix12   operator/=( float f );
    inline float       & operator[]( int i )             { return m_m[i]; }
    inline const float & operator[]( int i ) const       { return m_m[i]; }
    inline               operator float * ()             { return m_m; }
    inline               operator const float * () const { return m_m; }

	bool operator == (const RtgMatrix12& m) const
	{
		return 0 == memcmp(this, &m, sizeof(RtgMatrix12));
	}

	bool operator != (const RtgMatrix12& m) const
	{
		return !((*this) == m);
	}

	void NormalizeMatrix(void);
    RtgMatrix12 Inverse( void ) const;

    inline const float * U(void) const { return m_m; }
    inline const float * V(void) const  { return m_m+3; }
    inline const float * W(void) const  { return m_m+6; }
    inline const float * Position(void) const  { return m_m+9; }

    void FillRotateX( float degree );
    void FillRotateY( float degree );
    void FillRotateZ( float degree );

    void RotateLX( float degree );
    void RotateLY( float degree );
    void RotateLZ( float degree );
    void RotateX( float degree );
    void RotateY( float degree );
    void RotateZ( float degree );

    void TranslateX( const float Distance );
    void TranslateY( const float Distance );
    void TranslateZ( const float Distance );
    void Translate( float x, float y, float z );

    void SetRow(int _row, const RtgVertex3& _v3)
    {
        switch(_row)
        {
        case 0: _00 = _v3.m_v[0]; _01 = _v3.m_v[1]; _02 = _v3.m_v[2]; break;
        case 1: _10 = _v3.m_v[0]; _11 = _v3.m_v[1]; _12 = _v3.m_v[2]; break;
        case 2: _20 = _v3.m_v[0]; _21 = _v3.m_v[1]; _22 = _v3.m_v[2]; break;
        case 3: _30 = _v3.m_v[0]; _31 = _v3.m_v[1]; _32 = _v3.m_v[2]; break;
        }
    }

    const RtgVertex3* Row(int _row) const
    {
        RTASSERT(_row < 4);
        switch (_row)
        {
        case 0 : return (RtgVertex3*)&_00; break;
        case 1 : return (RtgVertex3*)&_10; break;
        case 2 : return (RtgVertex3*)&_20; break;
        case 3 : return (RtgVertex3*)&_30; break;
        }
    }

    void SetCol(int _col, const RtgVertex3& _v3)
    {
        switch(_col)
        {
        case 0: _00 = _v3[0]; _10 = _v3[1]; _20 = _v3[2]; break;
        case 1: _01 = _v3[0]; _11 = _v3[1]; _21 = _v3[2]; break;
        case 2: _02 = _v3[0]; _12 = _v3[1]; _22 = _v3[2]; break;
        }
    }

	void Output();
	friend RtArchive& operator<<( RtArchive &Ar, RtgMatrix12 &mat12) 
	{
		Ar.Serialize(mat12.m_m,12*sizeof(float));
		return Ar;
	}
};

///////////////////////////////////////////////////////////////////////////////////////
// Matrix M16 函数

void  rtgM16MakeUnit   ( float dest[16] );
void  rtgM16Assign     ( float dest[16], float m1[16] );
void  rtgM16AssignM12  ( float dest[16], float m1[12] );
void  rtgM16Multiply   ( float dest[16], float m1[16], float m2[16] );
void  rtgM16MakeRotateX( float dest[16], float degree );
void  rtgM16MakeRotateY( float dest[16], float degree );
void  rtgM16MakeRotateZ( float dest[16], float degree );
void  rtgM16MakeInverse( float dest[16], float m1[16] );
void  rtgM16MakeInverseM12( float dest[16], float m1[12] );
void  rtgM16MakeArbitraryRotate( float dest[16], float axis[3], float v[3], float degree );
void  rtgM16LUDecomposition( float m[16], int *indx, float *d, int dim=4 );
void  rtgM16LUBackSubstitute( float m[16], int *indx, float *b, int dim=4 );

class RtgMatrix16
{
public:
    union
    {
        float m_m[16];
        struct
        {
            float _00, _01, _02, _03;
            float _10, _11, _12, _13;
            float _20, _21, _22, _23;
            float _30, _31, _32, _33;
        };
    };

public:
    RtgMatrix16() {}
    RtgMatrix16( const RtgMatrix16 & );
    RtgMatrix16( const float m[16] );
    RtgMatrix16( float v00, float v01, float v02, float v03,
                float v10, float v11, float v12, float v13,
                float v20, float v21, float v22, float v23,
                float v30, float v31, float v32, float v33);
    ~RtgMatrix16() {}

    void Zero( void );
    void Unit( void );
    //设置为单位矩阵
    void Identity(void) { Unit();}
    void Dump( float m[16] ) const;
    //格式化为字符串形式 方便控制台打印
    std::string ToString() const;

    RtgMatrix16   operator+ ( const RtgMatrix16 &rhs );
    RtgMatrix16   operator- ( const RtgMatrix16 &rhs );
    RtgMatrix16 & operator+=( const RtgMatrix16 &rhs );
    RtgMatrix16 & operator-=( const RtgMatrix16 &rhs );
    RtgMatrix16 & operator= ( const RtgMatrix16 &rhs );
    RtgMatrix16 & operator= ( const RtgMatrix12 &rhs );
    RtgMatrix16   operator+ ( const float m[16] );
    RtgMatrix16   operator- ( const float m[16] );
    RtgMatrix16 & operator+=( const float m[16] );
    RtgMatrix16 & operator-=( const float m[16] );
    RtgMatrix16 & operator= ( const float m[16] );
    RtgMatrix16   operator* ( const RtgMatrix16 &rhs );
    RtgMatrix16   operator* ( float f );
    RtgMatrix16   operator/ ( float f );
    RtgMatrix16   operator*=( float f );
    RtgMatrix16   operator/=( float f );
    inline float       & operator[]( int i )             { return m_m[i]; }
    inline const float & operator[]( int i ) const       { return m_m[i]; }
    inline               operator float * ()             { return m_m; }
    inline               operator const float * () const { return m_m; }

	bool operator == (const RtgMatrix16& m) const
	{
		return 0 == memcmp(this, &m, sizeof(RtgMatrix16));
	}

	bool operator != (const RtgMatrix16& m) const
	{
		return !((*this) == m);
	}

	void NormalizeMatrix(void);
    RtgMatrix16 Inverse( void ) const;
    RtgMatrix16 Adjoint(void) const;
    void Invert(void);
    void Transport(void);
    float Determinant(void) const;

    void RotateLX( const float Degrees );
    void RotateLY( const float Degrees );
    void RotateLZ( const float Degrees );
    void FillRotateX( float degree );
    void FillRotateY( float degree );
    void FillRotateZ( float degree );
    void RotateX( float degree );
    void RotateY( float degree );
    void RotateZ( float degree );
    int  Rotate( const float frad, float x, float y, float z);
    void Scale(float x, float y, float z);

    void FaceTo(RtgVertex3& vFace, RtgVertex3& vUp); // vFace是方向的矢量，vUp是向上方向的矢量，两个参数都不一定需要单位化
    void FaceToVertex(RtgVertex3& vPos, RtgVertex3& vUp); // vPos是目标点的坐标，vUp是向上方向的矢量，vUp不一定需要单位化
    void FaceToVertex(RtgVertex3& vPos); // vPos是目标点的坐标，向上方向为垂直向上

	void TranslateX( const float Distance );
	void TranslateY( const float Distance );
	void TranslateZ( const float Distance );
	void Translate( float x, float y, float z );

    void Perspective( float fov, float aspect, float zNear, float zFar );
    void Ortho(float left, float right, float bottom, float top, float nearplane, float farplane, bool righthand=true);
    void SetView( float fromX, float fromY, float fromZ, float cX, float cY, float cZ, float uX, float uY, float uZ);
    void SetView( RtgVertex3 pos, RtgVertex3 center, RtgVertex3 up );
    void Reflect( RtgVertex4& vPlane );

	int  GetRotate( float *frad, float *x, float *y, float *z ) const;
	int  GetOrientation(RtgOrientation &orient);
	void QuaternionFromMatrix(RtgVertex4 &v);
	void MatrixFromQuaternion(RtgVertex4 v);
	void Fix(void);
	RtgVertex3 MapRotation(RtgVertex3 v);
	void MatrixFromFace(RtgVertex3 &face);
	void Output();
	float operator() (int i, int j) const { return *(m_m+i*4+j); }

    void GetAxis(int axis, RtgVertex3 &vct) const;
	void SetAxis(int axis, const float *pVet);
	void GetCol(int axis, RtgVertex3 &vct) const;
	void SetCol(int axis, const float *pVet);
	void GetRow(int row, RtgVertex3 &vct) const;
	void GetRow(int row, float *pOut) const;
	void SetRow(int row, RtgVertex3 &vector);
	void SetRow(int row, const float *pVet);
	void Set4X3Matrix(const float *pMatrix);
	void Get4X3Matrix(float *pMatrix) const;
    inline const float * U(void) const { return m_m; }
    inline const float * V(void) const { return m_m+4; }
    inline const float * W(void) const { return m_m+8; }
    inline const float * Position(void) const { return m_m+12; }

	float *GetMatrixPointer(void) { return &_00; }
	void  GetMatrix(float *matrix) const;
	void  GetMatrix(double *matrix) const;
	float GetElement( const int Row, const int Column ) const;
	void  SetElement( const int Row, const int Column, const float Value );
	void  SetMatrix(const float *matrix);

	friend RtArchive& operator<<( RtArchive &Ar, RtgMatrix16 &mat16) 
	{
		Ar.Serialize(mat16.m_m,16*sizeof(float));
		return Ar;
	}
};

void VxM(RtgVertex3 &out, RtgVertex3 &v, RtgMatrix16 &m);
RtgVertex3 operator*(const RtgVertex3 &v, const RtgMatrix12 &m);
RtgVertex3 operator*(const RtgVertex3 &v, const RtgMatrix16 &m);
RtgVertex4 operator*(const RtgVertex4 &v, const RtgMatrix16 &m);
RtgMatrix16 operator*( RtgMatrix16 const &l, RtgMatrix16 const &r );
RtgMatrix16 operator*( float r, RtgMatrix16 &m);


inline void CopyMatrix(RtgMatrix16* _m16, const RtgMatrix12* _m12)
{
    RTASSERT(_m16);
    RTASSERT(_m12);

    _m16->_00 = _m12->_00; _m16->_01 = _m12->_01; _m16->_02 = _m12->_02; _m16->_03 = 0.f;
    _m16->_10 = _m12->_10; _m16->_11 = _m12->_11; _m16->_12 = _m12->_12; _m16->_13 = 0.f;
    _m16->_20 = _m12->_20; _m16->_21 = _m12->_21; _m16->_22 = _m12->_22; _m16->_23 = 0.f;
    _m16->_30 = _m12->_30; _m16->_31 = _m12->_31; _m16->_32 = _m12->_32; _m16->_33 = 1.f;

}

inline void CopyMatrix(RtgMatrix16* _m1, const RtgMatrix16* _m2)
{
    (*_m1) = (*_m2);
}

inline void CopyMatrix(RtgMatrix12* _m12, const RtgMatrix16* _m16)
{
    RTASSERT(_m16);
    RTASSERT(_m12);

    _m12->_00 = _m16->_00; _m12->_01 = _m16->_01; _m12->_02 = _m16->_02; 
    _m12->_10 = _m16->_10; _m12->_11 = _m16->_11; _m12->_12 = _m16->_12; 
    _m12->_20 = _m16->_20; _m12->_21 = _m16->_21; _m12->_22 = _m16->_22; 
    _m12->_30 = _m16->_30; _m12->_31 = _m16->_31; _m12->_32 = _m16->_32; 
}

inline void CopyMatrix(RtgMatrix12* _m1, const RtgMatrix12* _m2)
{
    (*_m1) = (*_m2);
}

///////////////////////////////////////////////////////////////////////////////////////
// Matrix 2D M9 函数
// 这是一个专门用于2D计算的Matrix

class RtgMatrix2D9
{
public:
    union
    {
        float m_m[9];
        struct
        {
            float _00, _01, _02;
            float _10, _11, _12;
            float _20, _21, _22;
        };
    };

public:
    RtgMatrix2D9() {}
    RtgMatrix2D9( const RtgMatrix16 &rhs ) { for (int i=0; i<9; i++) m_m[i]=rhs.m_m[i]; }
    RtgMatrix2D9( const float m[16] )      { for (int i=0; i<9; i++) m_m[i]=m[i]; }
    ~RtgMatrix2D9() {}

    void Zero( void )   { for (int i=0; i<9; i++) m_m[i]=0.f; }
    void Unit( void )   { Zero(); _00 = _11 = _22 = 1.f; }

    inline float       & operator[]( int i )             { return m_m[i]; }
    inline const float & operator[]( int i ) const       { return m_m[i]; }
    inline               operator float * ()             { return m_m; }
    inline               operator const float * () const { return m_m; }

    RtgMatrix2D9 operator*( const RtgMatrix2D9 &rhs )
    {
        RtgMatrix2D9 M;
        M[0]  = m_m[0]*rhs.m_m[0] + m_m[1]*rhs.m_m[3] + m_m[2]*rhs.m_m[6] ;
        M[1]  = m_m[0]*rhs.m_m[1] + m_m[1]*rhs.m_m[4] + m_m[2]*rhs.m_m[7] ;
        M[2]  = m_m[0]*rhs.m_m[2] + m_m[1]*rhs.m_m[5] + m_m[2]*rhs.m_m[8] ;
        M[3]  = m_m[3]*rhs.m_m[0] + m_m[4]*rhs.m_m[3] + m_m[5]*rhs.m_m[6] ;
        M[4]  = m_m[3]*rhs.m_m[1] + m_m[4]*rhs.m_m[4] + m_m[5]*rhs.m_m[7] ;
        M[5]  = m_m[3]*rhs.m_m[2] + m_m[4]*rhs.m_m[5] + m_m[5]*rhs.m_m[8] ;
        M[6]  = m_m[6]*rhs.m_m[0] + m_m[7]*rhs.m_m[3] + m_m[8]*rhs.m_m[6] ;
        M[7]  = m_m[6]*rhs.m_m[1] + m_m[7]*rhs.m_m[4] + m_m[8]*rhs.m_m[7] ;
        M[8]  = m_m[6]*rhs.m_m[2] + m_m[7]*rhs.m_m[5] + m_m[8]*rhs.m_m[8] ;
        return M;
    }

    void SaveTo4x4(RtgMatrix16& m16)
    {
        m16.Unit();
        m16._00 = m_m[0]; m16._01 = m_m[1]; m16._02 = m_m[2];
        m16._10 = m_m[3]; m16._11 = m_m[4]; m16._12 = m_m[5];
        m16._20 = m_m[6]; m16._21 = m_m[7]; m16._22 = m_m[8];
    }

    void LoadBy4x4(RtgMatrix16& m16)
    {
        m_m[0] = m16._00; m_m[1] = m16._01; m_m[2] = m16._02;
        m_m[3] = m16._10; m_m[4] = m16._11; m_m[5] = m16._12;
        m_m[6] = m16._20; m_m[7] = m16._21; m_m[8] = m16._22;
    }

    void RotateLZ( const float fDegree )
    {
        /*
        float Temp00, Temp01, Temp02;
        float Temp10, Temp11, Temp12;
        float fSin = fsin(fDegrees), fCos = fcos(fDegrees);
        Temp00 = _00 * fCos + _10 * fSin;
        Temp01 = _01 * fCos + _11 * fSin;
        Temp02 = _02 * fCos + _12 * fSin;
        Temp10 = _00 *-fSin + _10 * fCos;
        Temp11 = _01 *-fSin + _11 * fCos;
        Temp12 = _02 *-fSin + _12 * fCos;
        _00 = Temp00; _01 = Temp01; _02 = Temp02;
        _10 = Temp10; _11 = Temp11; _12 = Temp12;
        */
        float ca = fcos( fDegree );
        float sa = fsin( fDegree );
        RtgMatrix2D9 dr;
        dr._00 = ca;    dr._10 = -sa;   dr._20 = 0.5f*(1-ca+sa);
        dr._01 = sa;    dr._11 =  ca;   dr._21 = 0.5f*(1-ca-sa);
        dr._02 =  0;    dr._12 =   0;   dr._22 = 1.f;
        *this = dr * (*this);
    }
    void RotateZ( float fDegree )
    {
        /*
        float ca = fcos( fDegree );
        float sa = fsin( fDegree );
        m_m[0] = m_m[0]*ca - m_m[1]*sa;
        m_m[1] = m_m[0]*sa + m_m[1]*ca;
        m_m[2] = m_m[2];
        m_m[3] = m_m[3]*ca - m_m[4]*sa;
        m_m[4] = m_m[3]*sa + m_m[4]*ca;
        m_m[5] = m_m[5];
        m_m[6] = m_m[6]*ca - m_m[7]*sa;
        m_m[7] = m_m[6]*sa + m_m[7]*ca;
        m_m[8] = m_m[8];
        */
        float ca = fcos( fDegree );
        float sa = fsin( fDegree );
        RtgMatrix2D9 dr;
        dr._00 = ca;    dr._10 = -sa;   dr._20 = 0.5f*(1-ca+sa);
        dr._01 = sa;    dr._11 =  ca;   dr._21 = 0.5f*(1-ca-sa);
        dr._02 =  0;    dr._12 =   0;   dr._22 = 1.f;
        *this = dr * (*this);
    }
    void Scale(float x, float y)
    {
        /*
        RtgMatrix2D9 ma;
        ma.Unit();
        ma._00 = x;
        ma._11 = y;
        (*this) = ma * (*this);
        */
        RtgMatrix2D9 ma;
        ma.Unit();
        ma._00 = x;
        ma._11 = y;
        ma._20 = 0.5f*(1-x);
        ma._21 = 0.5f*(1-y);
        (*this) = ma * (*this);
    }
    void TranslateX( const float fDistance )
    {
        _20 += fDistance;
    }
    void TranslateY( const float fDistance )
    {
        _21 += fDistance;
    }
    void Translate( float x, float y )
    {
        _20 += x;
        _21 += y;
    }

    friend RtArchive& operator<<( RtArchive &Ar, RtgMatrix2D9 &mat9) 
    {
        Ar.Serialize(mat9.m_m,9*sizeof(float));
        return Ar;
    }
};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
