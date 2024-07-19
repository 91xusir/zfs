
namespace rt_graph {

int RtgGetVertexFormatSize(RTGVertexFormat eFormat);

inline DWORD RtgGetColor(int r, int g, int b, int a)
{
    r = Clamp(r,0,255); g = Clamp(g,0,255); b = Clamp(b,0,255); a = Clamp(a,0,255);
    return a<<24 | r<<16 | g<<8 | b; // return a<<24 | b<<16 | g<<8 | r; // OpenGL
}

inline DWORD RtgVectorToColor(const RtgVertex3 &v, float fHeight)
{
    return RtgGetColor((int)( 255.0f * v.x ),(int)( 255.0f * v.y ),(int)( 255.0f * v.z ),(int)( 255.0f * fHeight ));
}

inline RtgVertex3 RtgColorToVector(DWORD c)
{
    return RtgVertex3(((c&0x00FF0000)>>16)/255.f, ((c&0x0000FF00)>>8)/255.f, (c&0x000000FF)/255.f);
}

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
