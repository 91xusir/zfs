
#ifndef RT_SCENE_ONLY_LOAD_TERRAIN
#include "scene/rt_scene.h"
#else
#include "core/rt2_core.h"
#include "scene/rts_path_point.h"
#include "scene/rts_scene_doc.h"
#endif
RtsWayPoint* RtsWayPoint::m_wayPoint = NULL;

RtsWayPoint::RtsWayPoint()
{

}

RtsWayPoint::~RtsWayPoint()
{
}
//添加路点
void RtsWayPoint::AddPathPoint(SPointScene point,RtsSceneDoc *pScene)		//添加了输入参数	ac
{
	list<SPointScene>::iterator it = m_allPathPoint.begin();
	for(;it!=m_allPathPoint.end();)
	{
         if((*it).x == point.x && (*it).y == point.y)
		 {
			 pScene->RtsSceneDoc::RemoveTerrainAttr(it->x,it->y,0x80);		//修正该点的bWayPoint = 1;（说明属于寻路链表）    ac
			 m_allPathPoint.erase(it++);
			 return;
		 }
		 else
			 ++it;
	}
	m_allPathPoint.push_back(point);
}
////检查一个点是否在路点列表中							//添加了新的check方法 （bWayPoint） 这个方法删除     ac
//bool RtsWayPoint::CheckPathPoint(RtSceneBlockTerrain::STileAttr* pTile,RtsSceneDoc *pScene)	
//{
//	list<SPointScene>::iterator it = m_allPathPoint.begin();
//	for(; it!=m_allPathPoint.end(); ++it)
//	{
//		//jin.wang
//		//if (it->x == x && it->y == y)
//		//{
//		//	return true;
//
//		//}
//		RtSceneBlockTerrain::STileAttr* pAttr = pScene->GetTerrainPathAttr(it->x, it->y);
//		
//		if(pTile == pAttr)
//		{
//			return true;
//		}
//	}
//
//}

