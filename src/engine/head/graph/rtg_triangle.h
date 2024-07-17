
namespace rt_graph {

///////////////////////////////////////////////////////////////////////////////////////
// Triangle º¯Êý

class RtgTriangle
{
protected:
    RtgVertex3 m_v0, m_v1, m_v2;

public:
    RtgTriangle() {}
    RtgTriangle( const float triangle[9] );
    RtgTriangle( const RtgVertex3 &v0, const RtgVertex3 &v1, const RtgVertex3 &v2 );
    ~RtgTriangle() {}

    inline RtgVertex3 &V1() { return m_v0; }
    inline RtgVertex3 &V2() { return m_v1; }
    inline RtgVertex3 &V3() { return m_v2; }
    inline void V1( const RtgVertex3 &v ) { m_v0=v; }
    inline void V2( const RtgVertex3 &v ) { m_v1=v; }
    inline void V3( const RtgVertex3 &v ) { m_v2=v; }

    float Area2D( void ) const;
    float Area2DX2( void ) const;
    float Area( void ) const;

    RtgVertex3 Normal( void );
};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
