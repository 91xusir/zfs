
namespace rt_graph {

class RtgFrustum
{
public:
    RtgFrustum();
    ~RtgFrustum();

public:
	bool PointTest(const RtgVertex3& vPos);
	bool SphereTest(const RtgVertex3& vPos, float radius);
	bool CubeTest(const RtgVertex3& vPos, float size);
    bool AABBTest(const RtgVertex3& vMin, const RtgVertex3& vMax);
    //bool BoundTest(ELBound* pBound);

public:
	void ExtractFrustum(RtgCamera* pCamera);
    RtgFrustum CutByPlane(RtgPlane& plane);

protected:
    RtgVertex4  m_Planes[6];

};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
