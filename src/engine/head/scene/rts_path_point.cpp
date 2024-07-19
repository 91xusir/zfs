
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
//���·��
void RtsWayPoint::AddPathPoint(SPointScene point,RtsSceneDoc *pScene)		//������������	ac
{
	list<SPointScene>::iterator it = m_allPathPoint.begin();
	for(;it!=m_allPathPoint.end();)
	{
         if((*it).x == point.x && (*it).y == point.y)
		 {
			 pScene->RtsSceneDoc::RemoveTerrainAttr(it->x,it->y,0x80);		//�����õ��bWayPoint = 1;��˵������Ѱ·����    ac
			 m_allPathPoint.erase(it++);
			 return;
		 }
		 else
			 ++it;
	}
	m_allPathPoint.push_back(point);
}
////���һ�����Ƿ���·���б���							//������µ�check���� ��bWayPoint�� �������ɾ��     ac
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

