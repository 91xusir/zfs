#ifndef RTS_PATH_POINT
#define RTS_PATH_POINT
#include "rts_scene_terrain.h"
class RtsSceneDoc;
class RtSceneBlockTerrain;
class RtsWayPoint
{
private:
	RtsWayPoint();
	static RtsWayPoint *m_wayPoint;
	int m_selectPoint;
public:
	list<SPointScene> m_allPathPoint;
	~RtsWayPoint();
	void AddPathPoint(SPointScene point,RtsSceneDoc *pScene); //添加路点	 ac
	bool CheckPathPoint(RtSceneBlockTerrain::STileAttr* pTile,RtsSceneDoc *pScene);//检查一个点是否在路点列表中
	static RtsWayPoint* GetInstance()
	{
		if(NULL == m_wayPoint)
			m_wayPoint = RT_NEW RtsWayPoint; 
		return m_wayPoint;
	}
};
#endif
