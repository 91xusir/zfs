
#ifndef RT_SCENE_ONLY_LOAD_TERRAIN
#include "scene/rt_scene.h"
#else
#include "core/rt2_core.h"
#include "rts_path_find.h"
#include "rts_scene_doc.h"
#include "scene/rts_path_point.h"
#endif



#ifdef _DEBUG
#   define DDD_CHECK(exp)   CHECK(exp)
#else
#   define DDD_CHECK(exp)   
#endif

//tim.yang
#define PATH_INFINITY 9999999
#define MAX_STOE_DISTACNE 100 //超过此距离使用路点
#define MAX_WP_DISTANCE 100 //两路点间最大距离

bool RtsPathPath::Next(int& x,int& y)
{
	if (m_iFirst >= PF_MAX_DEPTH && m_iLast >= PF_MAX_DEPTH)
		return false;

	if(m_iFirst!=m_iLast)
	{
		x=m_Path[m_iFirst].MinX;
		y=m_Path[m_iFirst].MinY;
		m_iFirst++;
		return true;
	}
	return false;
}

bool RtsPathPath::Next(int& x,int& y,RtsPathFind* pPath,bool bIgnoreActor, bool bIgnoreTarget,int actorType)
{
//    if (m_iFirst < 0 || m_iLast < 0 
//            || m_iFirst > PF_MAX_DEPTH || m_iLast > PF_MAX_DEPTH)
//    {
//        ClearPathEx();
//        x = 0;
//        y = 0;
//        return false;
//    }

	/*if (m_iFirst >= PF_MAX_DEPTH && m_iLast >= PF_MAX_DEPTH)
		return false;

	if(m_iFirst!=m_iLast)
	{
		x=m_Path[m_iFirst].MinX;
		y=m_Path[m_iFirst].MinY;
		m_iFirst++;
		return true;
	}
	return false;*/
	if (m_iFirst >= PF_MAX_DEPTH && m_iLast >= PF_MAX_DEPTH)
		return false;

	if(m_iFirst!=m_iLast)
	{
		SFindNode* curNode = 0;
		if (1 == actorType && !m_Path[m_iFirst].isWalk)
		{
			SPointScene start,end;
			start.x = curPos.MinX;
			start.y = curPos.MinY;
			end.x = m_Path[m_iFirst].MinX;
			end.y = m_Path[m_iFirst].MinY;
			curNode = pPath->FindPathByTwoPos(cBlock,start,end,bIgnoreActor,bIgnoreTarget,actorType);
			if (!curNode)
			{
				return false;
			}
			m_Path[m_iFirst].isWalk = 1;
			if (pPath->PushFrontInActorPath(this,curNode))
			{
				pPath->SmoothPath(this,bIgnoreActor);
				x=m_Path[m_iFirst].MinX;
				y=m_Path[m_iFirst].MinY;
				m_iFirst++;
				return pPath->_check_path(this);
			}
			return false;
		}
		else
		{
			x=m_Path[m_iFirst].MinX;
			y=m_Path[m_iFirst].MinY;
			m_iFirst++;
		}
		return true;
	}
	return false;
}

RtsPathPath::PPos* RtsPathPath::GetStep(int iStep)
{
	if (m_iFirst==m_iLast) return NULL;
	if (iStep==0) return m_Path + m_iFirst;
	int iCnt = PathCount();
	if ((iStep+1)>iCnt) return NULL;
	iCnt = m_iFirst + iStep;
	if (iCnt>=(int)PF_MAX_DEPTH) iCnt %= PF_MAX_DEPTH;
	return m_Path + iCnt;
}

RtsPathPath::PPos* RtsPathPath::GetAllPath(int& iLast)
{
	iLast = m_iLast;
	return m_Path;
}

int RtsPathPath::PathCount()
{
	return ((m_iLast+(PF_MAX_DEPTH+1))-m_iFirst)%(PF_MAX_DEPTH+1);
}

void RtsPathContainer::AddNode(RtsPathPos* pNode)
{
	RtsPathPos* p = pNodes;

	if(p)
	{
		if(p->dwActorId==pNode->dwActorId && p->sActorType==pNode->sActorType)
			return;

		while(p->pNext)
		{
			if(p->dwActorId==pNode->dwActorId && p->sActorType==pNode->sActorType)
				return;
			p = p->pNext;
		}
	}

	RtsPathPos* pAddNode=RT_NEW RtsPathPos;

	*pAddNode=*pNode;

	if (pAddNode->cBlock==PB_BLOCK)
	{
		++ iBlock;
	}
	else if (pAddNode->cBlock==PB_CHECK)
	{
		++ iCheck;
	}

	pAddNode->pNext = pNodes;
	pNodes = pAddNode;


}

bool RtsPathContainer::RemoveNode(RtsPathPos* pNode)
{
#ifdef _DEBUG
	CHECK(pNode!=NULL);
#endif

	RtsPathPos * pDelNode;

	if (pNodes->dwActorId==pNode->dwActorId && pNodes->sActorType==pNode->sActorType)
	{
		pDelNode=pNodes;

		if (pDelNode->cBlock==PB_BLOCK)
		{
			--iBlock;
		}
		else if (pDelNode->cBlock==PB_CHECK)
		{	
			--iCheck;	
		}

		pNodes = pNodes->pNext;

		DEL_ONE(pDelNode);
			pDelNode=NULL;

		return true;
	}
	else
	{
		RtsPathPos* p = pNodes;
		while (p->pNext)
		{
			if (p->pNext->dwActorId==pNode->dwActorId && p->pNext->sActorType==pNode->sActorType)
			{
				pDelNode=p->pNext;

				if (pDelNode->cBlock==PB_BLOCK)
				{
					--iBlock;
				}
				else if (pDelNode->cBlock==PB_CHECK)
				{	
					--iCheck;	
				}

				p->pNext = p->pNext->pNext;

				DEL_ONE(pDelNode);
					pDelNode=NULL;

				return true;
			}
			p = p->pNext;
		}
	}
	return false;
}

RtsPathPos* RtsPathContainer::FindActor(DWORD dwActorId,short sActorType)
{
	RtsPathPos* p = pNodes;
	while (p)
	{
		if (p->dwActorId == dwActorId && p->sActorType == sActorType)
		{
			return p;
		}
		p = p->pNext;
	}
	return NULL;
}

RtsPathPos* RtsPathContainer::GetBlockNode()
{
	RtsPathPos* p = pNodes;
	while (p)
	{
		if (p->cBlock)
		{
			return p;
		}
		p = p->pNext;
	}
	return NULL;
}

/*----------------------------------------------------------------------------
-   RtsPathFind 路径搜索类
----------------------------------------------------------------------------*/

RtsPathFind::RtsPathFind()
{
	CM_MEMPROTECTOR(m_szPathFindMsg, 100)
	CM_MEMPROTECTOR(m_szfilename, 256)

	m_pTerrain = NULL;

	m_wIncreaseFlag = 0;
	m_bIgnoreActor = false;
	m_bErrExit = false;
	m_szPathFindMsg[0] = 0;

	m_cSearchType = 0;

	m_iAddActorCount = 0;
	m_iMaxNode = 0;
	m_pNodes = NULL;
	m_pPathRoot = NULL;
	m_pWeightRoot = NULL;
	m_pCloseRoot = NULL;
	m_pUnuseRoot = NULL;
	m_wCurMaxDepth = 0;
	m_iXWidth = 20;  // 网格的X宽度
	m_iYWidth = 20;  // 网格的Y宽度

	//tim.yang automove
	//m_g = 0;
	m_h = 0;
	//m_gRef = 0;
	rt2_strcpy(m_szfilename, "scene/scene01/scene01.wp");
}

RtsPathFind::~RtsPathFind()
{
	CM_MEMUNPROTECTOR(m_szPathFindMsg)
	CM_MEMUNPROTECTOR(m_szfilename)

	if (m_poolPath.UsedCount())
	{
		RtCoreLog().Error("RtsPathFind: pool path nodes not cleared when exit: [%d]\n", m_poolPath.UsedCount());
	}
	if (m_iAddActorCount!=0)
	{
		RtCoreLog().Error("RtsPathFind:  Path find actor not cleared when exit:[%d]\n", m_iAddActorCount);
	}
}

// 只初始化一次的初始化
void RtsPathFind::InitOnce()
{
#ifdef REGION_SERVER
	m_wMaxDepth = 80;
	m_iMaxNode = 300;
#else
	m_wMaxDepth = DEPTH;
	m_iMaxNode  = MAXNODE;
#endif
	m_pNodes    = RT_NEW SFindNode[m_iMaxNode];
	//LoadSceneFile("scene01");
}

// 只清除一次的清除
void RtsPathFind::ClearOnce()
{
	DEL_ARRAY(m_pNodes);
}

void RtsPathFind::ClearIncreaseFlag()
{
	m_wIncreaseFlag = 0;
	m_pTerrain->ClearPathFindIncreaseFlag();
}

void RtsPathFind::SetBindTerrain(RtsSceneDoc* pTerrain)
{
	CHECK(pTerrain);
	m_pTerrain = pTerrain;
	m_pTerrain->ClearPathFindInfo();
	ClearIncreaseFlag();
}

/*
void RtsPathFind::OnLoadTerrainBlock(RtSceneBlockTerrain* pBlock)
{
}

void RtsPathFind::OnUnloadTerrainBlock(RtSceneBlockTerrain* pBlock)
{
}
*/

bool RtsPathFind::IsWalkMask(int iTerrainX, int iTerrainY)
{
	RtSceneBlockTerrain::STileAttr* pAttr = m_pTerrain->GetTerrainPathAttr(iTerrainX, iTerrainY);
	if (pAttr==NULL)            return true;
	else if (pAttr->bWalk)      return true;
	return false;
}

bool RtsPathFind::CanAdd(DWORD dwActorId,short sActorType,int iTerrainX, int iTerrainY, char cBlock, bool bOnlyOne)
{
	if(cBlock==PB_NONE)
		return true;

	RtSceneBlockTerrain::STileAttr* pAttr = m_pTerrain->GetTerrainPathAttr(iTerrainX, iTerrainY);
	if (pAttr==NULL)
	{
		m_lastResult = PR_CANCEL;
		return false;
	}
	if (pAttr->bWalk)
	{
		m_lastResult = PR_CTERRAIN;
		return false;
	}
	if (cBlock)
	{
		if (pAttr->pContainer)
		{
			if (bOnlyOne)
			{
				m_lastResult = PR_CPLAYER;
				return false;
			}

			if (pAttr->pContainer->iBlock)
			{
				short i=1;
				RtsPathPos* p = pAttr->pContainer->pNodes;

				if(p)
				{
					if (p->dwActorId!=dwActorId /*&& p->sActorType != sActorType*/)
					{
						m_lastResult = PR_CPLAYER;
						return false;
					}
				}

				while (p->pNext)
				{
					if (p->pNext->dwActorId!=dwActorId/* && p->pNext->sActorType != sActorType*/)
					{
						m_lastResult = PR_CPLAYER;
						return false;
					}
					++i;
					if(i==pAttr->pContainer->iBlock)
						break;
					p = p->pNext;
				}
			}
			else if(pAttr->pContainer->iCheck)
			{
				short i=1;
				RtsPathPos* p = pAttr->pContainer->pNodes;
				if(p)
				{
					if (p->dwActorId!=dwActorId && p->sActorType != sActorType)
					{
						if(!(p->m_pChkFunc && p->m_pChkFunc(sActorType,dwActorId,p->sActorType,p->dwActorId)))
						{
							m_lastResult = PR_CPLAYER;
							return false;
						}
					}
				}

				while (p->pNext)
				{
					if (p->pNext->dwActorId!=dwActorId && p->pNext->sActorType != sActorType)
					{
						if(!(p->m_pChkFunc && p->m_pChkFunc(sActorType,dwActorId,p->sActorType,p->dwActorId)))
						{
							m_lastResult = PR_CPLAYER;
							return false;
						}
					}
					++i;
					if(i==pAttr->pContainer->iBlock)
						break;
					p = p->pNext;
				}
			}
		}
	}
	m_lastResult = PR_OK;
	return true;
}

bool RtsPathFind::CanAdd(DWORD dwActorId,short sActorType,int iMinTerrainX, int iMinTerrainY,int iMaxTerrainX, int iMaxTerrainY, char cBlock, bool bOnlyOne)
{
	if(cBlock==PB_NONE)
		return true;

	for(int i=iMinTerrainX;i<=iMaxTerrainX;++i)
	{
		for(int j=iMinTerrainY;j<=iMaxTerrainY;++j)
		{	
			RtSceneBlockTerrain::STileAttr* pAttr = m_pTerrain->GetTerrainPathAttr(i, j);
			if (pAttr==NULL)
			{
				m_lastResult = PR_CANCEL;
				return false;
			}
			if (pAttr->bWalk)
			{
				m_lastResult = PR_CTERRAIN;
				return false;
			}
			if (cBlock)
			{
				if (pAttr->pContainer)
				{
					if (bOnlyOne)
					{
						m_lastResult = PR_CPLAYER;
						return false;
					}

					if (pAttr->pContainer->iBlock)
					{
						short i=1;
						RtsPathPos* p = pAttr->pContainer->pNodes;

						if(p)
						{
							if (p->dwActorId!=dwActorId && p->sActorType != sActorType)
							{
								m_lastResult = PR_CPLAYER;
								return false;
							}
						}

						while (p->pNext)
						{
							if (p->pNext->dwActorId!=dwActorId && p->pNext->sActorType != sActorType)
							{
								m_lastResult = PR_CPLAYER;
								return false;
							}
							++i;
							if(i==pAttr->pContainer->iBlock)
								break;
							p = p->pNext;
						}
					}
					else if(pAttr->pContainer->iCheck)
					{
						short i=1;
						RtsPathPos* p = pAttr->pContainer->pNodes;
						if(p)
						{
							if(p->dwActorId!=dwActorId && p->sActorType != sActorType)
							{
								if(!(p->m_pChkFunc && p->m_pChkFunc(sActorType,dwActorId,p->sActorType,p->dwActorId)))
								{
									m_lastResult = PR_CPLAYER;
									return false;
								}
							}
						}

						while (p->pNext)
						{
							if (p->pNext->dwActorId!=dwActorId && p->pNext->sActorType != sActorType)
							{
								if(!(p->m_pChkFunc && p->m_pChkFunc(sActorType,dwActorId,p->sActorType,p->dwActorId)))
								{
									m_lastResult = PR_CPLAYER;
									return false;
								}
							}
							++i;
							if(i==pAttr->pContainer->iBlock)
								break;
							p = p->pNext;
						}

					}
				}
			}
		}
	}
	m_lastResult = PR_OK;
	return true;
}

RtsPathPos* RtsPathFind::AddActor(DWORD dwActorId,short sActorType,int iTerrainX, int iTerrainY, bool bPath, char cBlock,void* pChkFunc)
{
	if(cBlock==PB_NONE)
		return NULL;

	RtSceneBlockTerrain::STileAttr* pAttr = m_pTerrain->GetTerrainPathAttr(iTerrainX, iTerrainY);
	if (pAttr==NULL)
	{
		m_lastResult = PR_CANCEL;
		return NULL;
	}

	if (pAttr->pContainer==NULL)
	{
		pAttr->pContainer = m_poolContainer.CreateObj();
	}

	RtsPathPos* pPosNode;
	if (bPath)
	{
		pPosNode = m_poolPath.CreateObj();
		((RtsPathPath*)pPosNode)->ClearPath();
	}else
	{
		pPosNode = m_poolPos.CreateObj();
	}

	pPosNode->dwActorId = dwActorId;
	pPosNode->sActorType = sActorType;
	pPosNode->cBlock = cBlock;
	pPosNode->SetPos(iTerrainX, iTerrainY);

	if(pChkFunc)
		pPosNode->m_pChkFunc=(bool(*)(short,DWORD,short,DWORD))pChkFunc;

	pAttr->pContainer->AddNode(pPosNode);
	m_lastResult = PR_OK;
	return pPosNode;
}



RtsPathPos* RtsPathFind::AddActor(DWORD dwActorId,short sActorType,int iMinTerrainX, int iMinTerrainY,int iMaxTerrainX, int iMaxTerrainY,bool bPath, char cBlock,void* pChkFunc)
{
	if(cBlock==PB_NONE)
		return NULL;

	int iTerrainX, iTerrainY;

	RtsPathPos* pPosNode;

	if (bPath)
	{
		pPosNode = m_poolPath.CreateObj();
		((RtsPathPath*)pPosNode)->ClearPath();
	}else
	{
		pPosNode = m_poolPos.CreateObj();
	}
	pPosNode->dwActorId = dwActorId;
	pPosNode->sActorType = sActorType;
	pPosNode->cBlock = cBlock;
	pPosNode->SetPos(iMinTerrainX, iMinTerrainY,iMaxTerrainX, iMaxTerrainY);

	if(pChkFunc)
		pPosNode->m_pChkFunc=(bool(*)(short,DWORD,short,DWORD))pChkFunc;

	for(int i=iMinTerrainX;i<=iMaxTerrainX;++i)
	{
		for(int j=iMinTerrainY;j<=iMaxTerrainY;++j)
		{		
			iTerrainX=i;
			iTerrainY=j;

			RtSceneBlockTerrain::STileAttr* pAttr = m_pTerrain->GetTerrainPathAttr(iTerrainX, iTerrainY);
			if (pAttr==NULL)
			{
				m_lastResult = PR_CANCEL;

				if(iMinTerrainX==iMaxTerrainX && iMinTerrainY==iMaxTerrainY)
					return NULL;			
				else
					break;
			}

			if (pAttr->pContainer==NULL)
			{
				pAttr->pContainer = m_poolContainer.CreateObj();
			}

			pAttr->pContainer->AddNode(pPosNode);
			m_lastResult = PR_OK;
		}
	}

	return pPosNode;
}


bool RtsPathFind::RemoveActor(RtsPathPath*& pActor)
{
	if (pActor==NULL)
	{
		m_lastResult = PR_OK;
		return true;
	}

	for(int i=pActor->CurMinX();i<=pActor->CurMaxX();++i)
	{
		for(int j=pActor->CurMinY();j<=pActor->CurMaxY();++j)
		{
			RtSceneBlockTerrain::STileAttr* pAttr = m_pTerrain->GetTerrainPathAttr(i,j);
			// RtSceneBlockTerrain::STileAttr* pAttr2 = m_pTerrain->GetTerrainPathAttr(548,887);

			if (pAttr==NULL)
			{
				m_lastResult = PR_CANCEL;
				if(pActor->CurMinX()==pActor->CurMaxX() && pActor->CurMinY()==pActor->CurMaxY())
					return true;
				else
					break;
			}
			if (pAttr->pContainer==NULL)
			{
				m_lastResult = PR_CANCEL;

				if(pActor->CurMinX()==pActor->CurMaxX() && pActor->CurMinY()==pActor->CurMaxY())
					return false;
				else
					break;
			}
			pAttr->pContainer->RemoveNode(pActor);

			if (pAttr->pContainer->pNodes==NULL)
			{
				m_poolContainer.DestroyObj(pAttr->pContainer);
				pAttr->pContainer = NULL;
			}

		}
	}		

	pActor->ClearPath();
	m_poolPath.DestroyObj(pActor);
	pActor = NULL;
	m_lastResult = PR_OK;

	return true;
}

bool RtsPathFind::RemoveActor(RtsPathPos*& pActor)
{
	if (pActor==NULL)
	{
		m_lastResult = PR_OK;
		return true;
	}

	for(int i=pActor->CurMinX();i<=pActor->CurMaxX();++i)
	{
		for(int j=pActor->CurMinY();j<=pActor->CurMaxY();++j)
		{
			RtSceneBlockTerrain::STileAttr* pAttr = m_pTerrain->GetTerrainPathAttr(pActor->CurMinX(), pActor->CurMinY());
			if (pAttr==NULL)
			{
				m_lastResult = PR_CANCEL;

				if(pActor->CurMinX()==pActor->CurMaxX() && pActor->CurMinY()==pActor->CurMaxY())
					return true;
				else
					break;
			}
			if (pAttr->pContainer==NULL)
			{
				m_lastResult = PR_CANCEL;
				if(pActor->CurMinX()==pActor->CurMaxX() && pActor->CurMinY()==pActor->CurMaxY())
					return false;
				else
					break;
			}
			pAttr->pContainer->RemoveNode(pActor);
		}
	}

	if (pActor->cType&PT_PATH)
	{
		((RtsPathPath*)pActor)->ClearPath();
		m_poolPath.DestroyObj((RtsPathPath*)pActor);
	}else
	{
		m_poolPos.DestroyObj(pActor);
	}
	pActor = NULL;
	m_lastResult = PR_OK;
	return true;
}

bool RtsPathFind::SmoothPath(RtsPathPath* pNode,bool vIgnoreActor)//平滑路径并抛弃出发点
{
	static	RtsPathPath::PPos    Path[PF_MAX_DEPTH+1]; // 搜索到的路径
	RtsPathPath::PPos*	NodePath=pNode->m_Path;

	int		First=pNode->m_iFirst;
	int		Last=pNode->m_iLast;
	int		Check=First;
	int		Current=First+1;

	if(First==Last || First+1==Last ||First+2==Last)
	{//在两个点以内，不需要做平滑
		//去掉出发点
		if(pNode->m_iFirst!=pNode->m_iLast)
		{
			pNode->m_iFirst++;
		}

		return false;
	}

	Path[First]=NodePath[Check];

//    for(;Current+1<Last;)
	for(;Current+1!=Last;)
	{
		if(!HaveBlockoffAround(NodePath[Check].MinX, NodePath[Check].MinY) && WalkAble(pNode->dwActorId,pNode->sActorType,NodePath[Check].MinX,NodePath[Check].MinY,NodePath[Current+1].MinX,NodePath[Current+1].MinY,vIgnoreActor))
		{
			Current++;
		}
		else
		{
			Check=Current;
			Current++;
			First++;
			Path[First]=NodePath[Check];//将各检查点记录
		}
	}

	First++;
	Path[First]=NodePath[Current];//将最后一个点记录

	//得到平滑后的路径点//去掉出发点
	for(int i=pNode->m_iFirst;i<First;i++)
	{
		pNode->m_Path[i]=Path[i+1];
	}

	pNode->m_iLast=First;
#ifndef 	REGION_SERVER
	//tim.yang  automove
    if (pNode->m_iFirst < 0 || pNode->m_iLast < 0 
            || pNode->m_iFirst > PF_MAX_DEPTH || pNode->m_iLast > PF_MAX_DEPTH)
        pNode->ClearPathEx();
#endif
	return _check_path(pNode);
}

bool RtsPathFind::WalkAble(DWORD dwActorId,short sActorType,int vStartX,int vStartY,int vEndX,int vEndY,bool vIgnoreActor)//不含出发点
{
	//将该直线光栅化//略有些不同
	int x1,x2,y1,y2,dx,dy,e;
	int MinX, MaxX, MinY, MaxY, x, y, temp, i;

	MinX=vStartX<vEndX ? vStartX:vEndX;
	MaxX=vStartX>vEndX ? vStartX:vEndX;

	MinY=vStartY<vEndY ? vStartY:vEndY;
	MaxY=vStartY>vEndY ? vStartY:vEndY;

	x1=vStartX;
	x2=vEndX;
	y1=vStartY;
	y2=vEndY;

	if(vIgnoreActor) //忽略人
	{
		if(MinX==MaxX) //该线垂直
		{
			for(i=MinY;i<=MaxY;i++)
			{
				if(IsWalkMask(x1, i)) //不行
				{
					if(!(i==vStartY && x1==vStartX)) //忽略起始点
					{
						return false;
					}
				}
			}
			return true;
		}
		if(MinY==MaxY) //该线水平
		{
			for(i=MinX;i<=MaxX;i++)
			{
				if(IsWalkMask(i, MinY)) //不行
				{
					if(!(i==vStartX && MinY==vStartY)) //忽略起始点
					{
						return false;
					}
				}
			}
			return true;
		}
	}else //不忽略人
	{
		if(MinX==MaxX) //该线垂直
		{
			for(i=MinY;i<=MaxY;i++)
			{
				if(!CanAdd(dwActorId,sActorType,x1, i))
				{//不行
					if(!(i==vStartY && x1==vStartX)) //忽略起始点
					{
						return false;
					}
				}
			}
			return true;
		}
		if(MinY==MaxY) //该线水平
		{
			for(i=MinX;i<=MaxX;i++)
			{
				if(!CanAdd(dwActorId,sActorType,i, MinY)) //不行
				{
					if(!(i==vStartX && MinY==vStartY)) //忽略起始点
					{
						return false;
					}
				}
			}
			return true;
		}
	}

	//采用Bresenham画线算法(见:计算机图形学<清华版>169页)//略有些不同
#ifdef REGION_SERVER
	if(MaxX-MinX<MaxY-MinY)
#endif
	//if(MaxX-MinX<MaxY-MinY) //y轴为自增量
	{
		if(y1 > y2)
		{
			temp=x1; x1=x2; x2=temp;
			temp=y1; y1=y2; y2=temp;
		}
		dy=MaxY-MinY;
		dx=MaxX-MinX;
		e=-dy;
		x=x1;
		y=y1;

		if(vIgnoreActor) //忽略人
		{
			if(x2>x1)
			{
				for(i=y1;;)
				{
					if(IsWalkMask(x, y)) //不行
					{
						if(!(x==vStartX && y==vStartY)) return false; //忽略起始点
					}
					i++;
					if(!(i<=y2)) break;
					y++;
					e=e+2*dx;
					if(e>=0)
					{
						if(IsWalkMask(x, y)) return false; //不行
						x++;
						e=e-2*dy;
					}
				}
			}else if(x2<x1)
			{
				for(int i=y1;;)
				{
					if(IsWalkMask(x, y))
					{//不行
						if(!(x==vStartX && y==vStartY)) return false; //忽略起始点
					}
					i++;
					if(!(i<=y2)) break;
					y++;
					e=e+2*dx;
					if(e>=0)
					{
						if(IsWalkMask(x, y)) return false; //不行
						x--;
						e=e-2*dy;
					}
				}
			}
		}else //不忽略人
		{
			if(x2>x1)
			{
				for(i=y1;;)
				{
					if(!CanAdd(dwActorId,sActorType,x, y)) //不行
					{
						if(!(x==vStartX && y==vStartY)) return false; //忽略起始点
					}
					i++;
					if(!(i<=y2)) break;
					y++;
					e=e+2*dx;
					if(e>=0)
					{
						if(!CanAdd(dwActorId,sActorType,x, y)) return false; //不行
						x++;
						e=e-2*dy;
					}
				}
			}else if(x2<x1)
			{
				for(i=y1;;)
				{
					if(!CanAdd(dwActorId,sActorType,x, y))
					{//不行
						if(!(x==vStartX && y==vStartY)) return false; //忽略起始点
					}
					i++;
					if(!(i<=y2)) break;
					y++;
					e=e+2*dx;
					if(e>=0)
					{
						if(!CanAdd(dwActorId,sActorType,x, y)) return false; //不行
						x--;
						e=e-2*dy;
					}
				}
			}
		}
	}
#ifdef REGION_SERVER
	else//x轴为自增量
#endif
	//else //x轴为自增量
	{
		if( x1 >x2 )
		{
			temp=x1; x1=x2; x2=temp;
			temp=y1; y1=y2; y2=temp;
		}
		dy=MaxY-MinY;
		dx=MaxX-MinX;
		e=-dx;
		x=x1;
		y=y1;

		if(vIgnoreActor) //忽略人
		{
			if(y2>y1)
			{
				for(i=x1;;)
				{
					if(IsWalkMask(x, y))
					{//不行
						if(!(x==vStartX && y==vStartY)) return false; //忽略起始点
					}
					i++;
					if(!(i<=x2)) break;
					x++;
					e=e+2*dy;
					if(e>=0)
					{
						if(IsWalkMask(x, y)) return false; //不行
						y++;
						e=e-2*dx;
					}
				}
			}else if(y2<y1)
			{
				for(i=x1;;)
				{
					if(IsWalkMask(x, y)) //不行
					{
						if(!(x==vStartX && y==vStartY)) return false; //忽略起始点
					}
					i++;
					if(!(i<=x2)) break;
					x++;
					e=e+2*dy;
					if(e>=0)
					{
						if(IsWalkMask(x, y)) return false; //不行
						y--;
						e=e-2*dx;
					}
				}
			}
		}else //不忽略人
		{
			if(y2>y1)
			{
				for(i=x1;;)
				{
					if(!CanAdd(dwActorId,sActorType,x, y)) //不行
					{
						if(!(x==vStartX && y==vStartY)) return false; //忽略起始点
					}
					i++;
					if(!(i<=x2)) break;
					x++;
					e=e+2*dy;
					if(e>=0)
					{
						if(!CanAdd(dwActorId,sActorType,x, y)) return false; //不行
						y++;
						e=e-2*dx;
					}
				}
			}else if(y2<y1)
			{
				for(i=x1;;)
				{
					if(!CanAdd(dwActorId,sActorType,x, y)) //不行
					{
						if(!(x==vStartX && y==vStartY)) return false; //忽略起始点
					}
					i++;
					if(!(i<=x2)) break;
					x++;
					e=e+2*dy;
					if(e>=0)
					{
						if(!CanAdd(dwActorId,sActorType,x, y)) return false; //不行
						y--;
						e=e-2*dx;
					}
				}
			}
		}
	}
	return true;
}

bool RtsPathFind::MoveTo(RtsPathPos*& pActor, int iX, int iY ,char cBlock)
{
	bool bPath;

	DWORD dwActorId = pActor->dwActorId;
	short sActorType = pActor->sActorType;

	bPath = (pActor->cType&PT_PATH)!=0;
	if (CanAdd(dwActorId,sActorType,iX, iY, cBlock))
	{
		if (!RemoveActor(pActor))
		{
			return false;
		}
		pActor = AddActor(dwActorId,sActorType, iX, iY, bPath, cBlock);
		return (pActor!=NULL);
	}
	return false;
}

bool RtsPathFind::MoveTo(RtsPathPath*& pActor, int iX, int iY ,char cBlock)
{
	bool bPath;
	DWORD dwActorId = pActor->dwActorId;
	short sActorType = pActor->sActorType;

	bPath = (pActor->cType&PT_PATH)!=0;
	if (CanAdd(dwActorId,sActorType,iX, iY, cBlock))
	{
		if (!RemoveActor(pActor))
		{
			return false;
		}

		pActor = (RtsPathPath*)AddActor(dwActorId,sActorType, iX, iY, bPath, cBlock);
		return (pActor!=NULL);
	}
	return false;
}

bool RtsPathFind::MoveTo(RtsPathPos*& pActor, int iMinTerrainX, int iMinTerrainY,int iMaxTerrainX, int iMaxTerrainY ,char cBlock)
{
	bool bPath;
	DWORD dwActorId = pActor->dwActorId;
	short sActorType = pActor->sActorType;
	bPath = (pActor->cType&PT_PATH)!=0;
	if (CanAdd(dwActorId,sActorType,iMinTerrainX, iMinTerrainY,iMaxTerrainX, iMaxTerrainY, cBlock))
	{
		if (!RemoveActor(pActor))
		{
			return false;
		}
		pActor = AddActor(dwActorId,sActorType,iMinTerrainX, iMinTerrainY,iMaxTerrainX, iMaxTerrainY, bPath, cBlock);
		return (pActor!=NULL);
	}
	return false;
}

bool RtsPathFind::MoveTo(RtsPathPath*& pActor,int iMinTerrainX, int iMinTerrainY,int iMaxTerrainX, int iMaxTerrainY ,char cBlock)
{
	bool bPath;
	DWORD dwActorId = pActor->dwActorId;
	short sActorType = pActor->sActorType;
	bPath = (pActor->cType&PT_PATH)!=0;
	if (CanAdd(dwActorId,sActorType,iMinTerrainX, iMinTerrainY,iMaxTerrainX, iMaxTerrainY, cBlock))
	{
		if (!RemoveActor(pActor))
		{
			return false;
		}

		pActor = (RtsPathPath*)AddActor(dwActorId,sActorType,iMinTerrainX, iMinTerrainY,iMaxTerrainX, iMaxTerrainY, bPath, cBlock);
		return (pActor!=NULL);
	}
	return false;
}


int RtsPathFind::NextFindNode(char cBlock,int x, int y, SFindNode* pNode,int actorType)
{
	RtSceneBlockTerrain::STileAttr* pAttr = m_pTerrain->GetTerrainPathAttr(x, y);
	if (1 == actorType && !pAttr)
	{
		RtCoreLog().Info("x, y\n",x,y);
		return 3;
	}
	if (pAttr==NULL)    return 1;
	if (pAttr->bWalk)   return 1; // 如果 (x,y) 处是障碍就直接返回

	if (!m_bIgnoreActor && cBlock && pAttr->pContainer && pAttr->pContainer->iBlock) // 人物阻挡
	{
		return 1;
	}

	SFindNode* pCur = pNode;
	while (pCur)
	{
		if (x==pCur->x && y==pCur->y) return 1;//如果 (x,y) 在当前的路径中,直接返回
		pCur = pCur->pParent;
	}

	WORD wDepth = pNode->wDepth + 1;
	if (wDepth>=m_wMaxDepth) return 1; // 如果当前的深度已经超过预先设置的深度
	if (pAttr->dis.wFlag==m_wIncreaseFlag && wDepth>=(pAttr->dis.wDepth))
		return 1;// 如果曾经有更好的方案移动到 (x,y) 失败
	pAttr->dis.wFlag = m_wIncreaseFlag;
	pAttr->dis.wDepth = wDepth;

	// 将这步方案记入待处理队列
	if (m_pUnuseRoot==NULL)
	{
		m_bErrExit = true;
		rt2_sprintf(m_szPathFindMsg, "堆栈空间不够");
		return 2;
	}
	pCur = m_pUnuseRoot;
	m_pUnuseRoot = m_pUnuseRoot->pParent;
	pCur->x = x;
	pCur->y = y;
	pCur->wDepth = wDepth;
	pCur->pParent = pNode;
	if (m_wCurMaxDepth<wDepth) m_wCurMaxDepth = wDepth;

	//tim.yang   automove
	if (0 == actorType)
	{
		pCur->wWeight = NodeJudge(x, y);
	}
	else
	{
		//m_g += m_gRef;
		m_h = abs(m_iCurTargetX - x) + abs(m_iCurTargetY - y);
		pCur->wWeight = /*m_g + */m_h;
		//m_g = 0;
	}
	//end

	SFindNode* pParent = NULL;
	SFindNode* pWeight = m_pWeightRoot;
	while (pWeight && pCur->wWeight > pWeight->wWeight)
	{
		pParent = pWeight;
		pWeight = pWeight->pNextBest;
	}
	pCur->pNextBest = pWeight;
	if (pParent) pParent->pNextBest = pCur;
	else         m_pWeightRoot = pCur;
	return 0;
}

SFindNode* RtsPathFind::FindPathByTwoPos(char cBlock, SPointScene spSrc, SPointScene spTarget, bool bIgnoreActor, bool bIgnoreTarget, int actorType)
{

	int x, y;
	m_iCurTargetX = spTarget.x;
	m_iCurTargetY = spTarget.y;

	m_bIgnoreActor = bIgnoreActor;
	m_bIgnoreTarget = bIgnoreTarget;
	IncreaseFlag();

	// 清除队列
	m_bErrExit = false;
	m_pPathRoot = NULL;
	m_pCloseRoot = NULL;
	m_pWeightRoot = NULL;
	m_pUnuseRoot = m_pNodes;

	//tim.yang   automove
	if (0 == actorType)
	{
		m_iMaxNode = 300;
		m_wMaxDepth = 80;
	}
	else if (1 == actorType)
	{
		m_iMaxNode = MAXNODE;
		m_wMaxDepth = DEPTH;
	}
	else
		return 0;
	//end

	for (x=0; x<m_iMaxNode-1; x++)
	{
		m_pNodes[x].pParent = m_pNodes+(x+1);
	}
	m_pNodes[m_iMaxNode-1].pParent = NULL;

	// 开始第一个元素(起始位置)
	SFindNode* pCurNode;
	m_pPathRoot = m_pUnuseRoot;
	m_pUnuseRoot = m_pUnuseRoot->pParent;
	m_pPathRoot->x = spSrc.x;
	m_pPathRoot->y = spSrc.y;
	m_pPathRoot->wDepth = 0;
	m_pPathRoot->pParent = NULL;

	m_pWeightRoot = m_pPathRoot;
	m_pWeightRoot->pNextBest = NULL;

	//tim.yang automove
	if (0 == actorType)
	{
		m_pWeightRoot->wWeight = NodeJudge(m_pWeightRoot->x, m_pWeightRoot->y);
	}
	else
	{
		m_pWeightRoot->wWeight = /*m_g +*/ m_h;
	}
	//end


	bool bCanMove;
	int iResult;
	while (1)
	{
		// 取出优先级最高的Node
		pCurNode = m_pWeightRoot;
		if (pCurNode==NULL) // 错误，可能是两边的路径不通导致的
		{
			m_bErrExit = true;
			rt2_sprintf(m_szPathFindMsg, "找了所有的点也没有找到路径");
			OutputTerrain(spSrc.x, spSrc.y, spTarget.x, spTarget.y, 10, 10);
			break;
		}
		m_pWeightRoot = m_pWeightRoot->pNextBest;
		// 把这个Node加入处理过的表
		pCurNode->pNextBest = m_pCloseRoot;
		m_pCloseRoot = pCurNode;

		x = pCurNode->x;
		y = pCurNode->y;
		if (m_bIgnoreTarget)
		{
			if (m_cSearchType==0)
			{
				if (x==m_iCurTargetX)
				{
					if (abs(y-m_iCurTargetY)==1) break;
				}else if (y==m_iCurTargetY)
				{
					if (abs(x-m_iCurTargetX)==1) break;
				}
			}else if (m_cSearchType==1)
			{
				if (abs(y-m_iCurTargetY)<=1 && abs(x-m_iCurTargetX)<=1)
				{
					break;
				}
			}
		}
		if (x==m_iCurTargetX && y==m_iCurTargetY) break; // 已经到达目的地


		bCanMove = false;
		//if (1 == actorType)
		//{
		//	depthNum++;
		//	m_gRef = 10 * depthNum;
		//}
		iResult = NextFindNode(cBlock,x, y-1, pCurNode,actorType);
		if (iResult==0) bCanMove = true;
		else if (iResult == 3)
		{
			bCanMove = true;
			break;
		}
		else if (iResult!=1) break;
		iResult = NextFindNode(cBlock,x, y+1, pCurNode,actorType);
		if (iResult==0) bCanMove = true;
		else if (iResult == 3)
		{
			bCanMove = true;
			break;
		}
		else if (iResult!=1) break;
		iResult = NextFindNode(cBlock,x-1, y, pCurNode,actorType);
		if (iResult==0) bCanMove = true;
		else if (iResult == 3)
		{
			bCanMove = true;
			break;
		}
		else if (iResult!=1) break;
		iResult = NextFindNode(cBlock,x+1, y, pCurNode,actorType);
		if (iResult==0) bCanMove = true;
		else if (iResult == 3)
		{
			bCanMove = true;
			break;
		}
		else if (iResult!=1) break;
		if (m_cSearchType)
		{
			//if (1 == actorType)
			//{
			//	m_gRef = 14 * depthNum;
			//}
			iResult = NextFindNode(cBlock,x-1, y-1, pCurNode,actorType);
			if (iResult==0) bCanMove = true;
			else if (iResult!=1) break;
			iResult = NextFindNode(cBlock,x+1, y+1, pCurNode,actorType);
			if (iResult==0) bCanMove = true;
			else if (iResult!=1) break;
			iResult = NextFindNode(cBlock,x-1, y+1, pCurNode,actorType);
			if (iResult==0) bCanMove = true;
			else if (iResult!=1) break;
			iResult = NextFindNode(cBlock,x+1, y-1, pCurNode,actorType);
			if (iResult==0) bCanMove = true;
			else if (iResult!=1) break;
		}
		if (bCanMove==false)
		{
			// 如果不能移动，就把最后这个节点从处理过的表中释放
			m_pCloseRoot = m_pCloseRoot->pNextBest;
		}
	}
	if (m_bErrExit)
	{
		return 0;
	}
	// 回溯树，将求出的最佳路径遍历一次
	SFindNode *p1, *p2=NULL;
	while (pCurNode)
	{
		p1 = pCurNode->pParent;
		pCurNode->pParent = p2;
		p2 = pCurNode;
		if (p1==NULL) break;
		pCurNode = p1;
	}

	return pCurNode;
}

bool RtsPathFind::FindPathEx(RtsPathPath* pActor, int iTargetX, int iTargetY, bool bIgnoreActor, bool bIgnoreTarget,int actorType)
{
	if (!pActor)
		return false;

	int startX = pActor->curPos.MinX;
	int startY = pActor->curPos.MinY;

	if (1 == actorType)
	{
		InitAStar();  
		
        pActor->ClearPathEx(); 

		if (Distance2(startX, startY, iTargetX, iTargetY))
		{
			if (FindPathNormal(pActor, startX, startY,iTargetX,iTargetY,bIgnoreActor,bIgnoreTarget,actorType))
				return true;			
		}
		else
		{
			if (FindPathFromWP(pActor,startX,startY,iTargetX,iTargetY,bIgnoreActor,bIgnoreTarget,actorType))
				return true;
		}
	}
	else
	{
		if (FindPathNormal(pActor,startX,startY,iTargetX,iTargetY,bIgnoreActor,bIgnoreTarget,actorType))
			return true;
	}
	return false;
	/*SPointScene start,end;
	start.x = pActor->curPos.MinX;
	start.y = pActor->curPos.MinY;
	end.x = iTargetX;
	end.y = iTargetY;
	SFindNode* pCurNode = FindPathByTwoPos(pActor->cBlock,start,end,bIgnoreActor,bIgnoreTarget,actorType);
	pActor->ClearPath();
	while (pCurNode)
	{
		if (pActor->m_iLast>=PF_MAX_DEPTH) break;
		pActor->m_Path[pActor->m_iLast].MinX = pCurNode->x;
		pActor->m_Path[pActor->m_iLast].MinY = pCurNode->y;
		pActor->m_Path[pActor->m_iLast].isWalk = 1;
		pActor->m_iLast ++;
		pCurNode = pCurNode->pParent;
	}
	return _check_path(pActor);*/
}

bool RtsPathFind::FindPathNormal(RtsPathPath* pActor,int startX, int startY, int endX, int endY, bool bIgnoreActor, bool bIgnoreTarget,int actorType, bool bAppend)
{
	SPointScene start,end;
	//start.x = pActor->curPos.MinX;
	//start.y = pActor->curPos.MinY;
	start.x = startX;
	start.y = startY;
	end.x = endX;
	end.y = endY;
	SFindNode* pCurNode = FindPathByTwoPos(pActor->cBlock,start,end,bIgnoreActor,bIgnoreTarget,actorType);
	if (!pCurNode)
	{
		return false;
	}
	
	if (!bAppend)//不是追加就清除
	{
		pActor->ClearPath();
	}

	while (pCurNode)
	{
		if (pActor->m_iLast>=PF_MAX_DEPTH) break;
		pActor->m_Path[pActor->m_iLast].MinX = pCurNode->x;
		pActor->m_Path[pActor->m_iLast].MinY = pCurNode->y;
		pActor->m_Path[pActor->m_iLast].isWalk = 1;
		pActor->m_iLast ++;
		pCurNode = pCurNode->pParent;
	}
	return _check_path(pActor);
}

bool RtsPathFind::_check_path(RtsPathPath* pNode)
{
	if (pNode->m_iLast>=PF_MAX_DEPTH ||
			(pNode->m_iLast - pNode->m_iFirst)>=PF_MAX_DEPTH ||
			(pNode->m_iLast - pNode->m_iFirst) < 0)
	{
		pNode->m_iLast = 0;
		pNode->m_iFirst = 0;
		return false;
	}
	else
		return true;
}

bool RtsPathFind::FindPath(RtsPathPath* pNode, int iTargetX, int iTargetY)
{
	// 如果不忽略人的时候找不到路径就忽略人再找一次
	if (FindPathEx(pNode, iTargetX, iTargetY, false, false))
	{
		m_lastResult = PR_OK;
		return true;
	}
	if (FindPathEx(pNode, iTargetX, iTargetY, true, false))
	{
		m_lastResult = PR_CPLAYER;
		return false;
	}
	m_lastResult = PR_CTERRAIN;
	return false;
}

const int CARRY_WIDTH=100;
bool RtsPathFind::FindSpace(DWORD dwActorId,short sActorType,int iTerrainX, int iTerrainY, int& iSpaceX, int& iSpaceY, bool bOnlyOne)
{
	int x, y, j;
	for (j=0; j<CARRY_WIDTH; j++)
	{
		y = -j;
		for(x=-j;x<=j;x++)
		{
			if(CanAdd(dwActorId,sActorType,iTerrainX+x,iTerrainY+y, (char)PB_BLOCK, bOnlyOne))
			{
				iSpaceX = iTerrainX+x;
				iSpaceY = iTerrainY+y;
				return true;
			}
		}
		y = j;
		for(x=-j;x<=j;x++)
		{
			if(CanAdd(dwActorId,sActorType,iTerrainX+x,iTerrainY+y, (char)PB_BLOCK, bOnlyOne))
			{
				iSpaceX = iTerrainX+x;
				iSpaceY = iTerrainY+y;
				return true;
			}
		}
		x = -j;
		for(y=-j+1;y<j;y++)
		{
			if(CanAdd(dwActorId,sActorType,iTerrainX+x,iTerrainY+y, (char)PB_BLOCK, bOnlyOne))
			{
				iSpaceX = iTerrainX+x;
				iSpaceY = iTerrainY+y;
				return true;
			}
		}
		x = j;
		for(y=-j+1;y<j;y++)
		{
			if(CanAdd(dwActorId,sActorType,iTerrainX+x,iTerrainY+y, (char)PB_BLOCK, bOnlyOne))
			{
				iSpaceX = iTerrainX+x;
				iSpaceY = iTerrainY+y;
				return true;
			}
		}
	}
	m_lastResult = PR_CTERRAIN;
	return false;
}

void RtsPathFind::OutputTerrain(int iSX, int iSY, int iDX, int iDY, int iSizeX, int iSizeY)
{
//#ifdef _DEBUG
	int iMinX, iMinY, iMaxX, iMaxY;

	iMinX = Clamp(iSX-iSizeX, 0, m_iXWidth-1);
	iMinY = Clamp(iSY-iSizeY, 0, m_iYWidth-1);
	iMaxX = Clamp(iSX+iSizeX, 0, m_iXWidth-1);
	iMaxY = Clamp(iSY+iSizeY, 0, m_iYWidth-1);

	/*
	RtCoreLog().Info("PathFindMsg: output (%d, %d)to(%d, %d) size(%d, %d)\n", iSX, iSY, iDX, iDY, iSizeX, iSizeY);
	for (y=iMinY; y<=iMaxY; y++)
	{
	MSG("  |");
	for (x=iMinX; x<=iMaxX; x++)
	{
	if (x==iSX && y==iSY)
	{
	MSG("S");
	}else if (x==iDX && y==iDY)
	{
	MSG("D");
	}else if (IsWalkMask(x, y))
	{
	MSG("O");
	}else if (!CanAdd(x, y, PB_BLOCK))
	{
	MSG(".");
	}else
	{
	MSG(" ");
	}
	}
	MSG("|\n");
	}
	*/
//#endif
}

//只在tool_scene中调用
void RtsPathFind::SaveWayPointInfo(list<rtPOINT>& PathPoint)
{
	FILE *fp = fopen(m_szfilename, "w");
	if(PathPoint.empty())
	{
		if (fp)
		{
			RtCoreFile().Delete(m_szfilename,0,1);
		}
		return;
	}
		

	list<vector<rtPOINT> > wp_list;
	//连通信息
	float num = 0;
	list<rtPOINT >::iterator it1 = PathPoint.begin();	
	RtCoreLog().Info("开始生成连通器!\n");
	for(;it1 != PathPoint.end(); ++it1)
	{	
		vector<rtPOINT> info;
		info.push_back(*it1);

		list<rtPOINT>::iterator it2 = PathPoint.begin();
		for(; it2 != PathPoint.end(); ++it2)
		{
			long distance = Distance(it1->x, it1->y, it2->x, it2->y);

			if (it1 == it2 || distance>MAX_WP_DISTANCE) //两路点距离大于100,设置为不能走通
				continue;
			//if(WalkAble(0, 0, it1->x, it1->y, it2->x, it2->y, true)) // 两点能走通
			//{
			SPointScene start,end;
			start.x = it1->x;
			start.y = it1->y;
			end.x = it2->x;
			end.y = it2->y;
			SFindNode* curNode = FindPathByTwoPos(1,start,end,true,false,1);
			if (curNode)
			{
				info.push_back(*it2); 
			}              
			//}
		}
		++num;
		int per = num / PathPoint.size() * 100;
		RtCoreLog().Info("已经完成%d%%\n",per);
		wp_list.push_back(info);
	}

	//save
	//std::string filename = filepath + "\\" +"way_point.wp";
	//FILE *fp = fopen(filename.c_str(), "w");
	char buffer[512];
	if (fp != NULL)
	{
		list<vector<rtPOINT> >::iterator list_it = wp_list.begin();
		for(; list_it != wp_list.end(); ++list_it)
		{
			vector<rtPOINT>::iterator vector_it = list_it->begin();

			rt2_sprintf(buffer,"%d,%d", vector_it->x, vector_it->y);
			int size = strlen(buffer);
			fwrite(buffer, 1, size, fp);
			++vector_it;

			for (; vector_it != list_it->end(); ++vector_it)
			{
				rt2_sprintf(buffer,",%d,%d", vector_it->x, vector_it->y);
				int size = strlen(buffer);
				fwrite(buffer, 1, size, fp);
			}

			fwrite("\r\n", 1, 2, fp);
		}
		fclose(fp);
	}

}

long RtsPathFind::Distance(long sx, long sy, long tx, long ty)
{
	return (long)sqrt((long double)(sx-tx)*(sx-tx)+ (ty-sy)*(ty-sy));  
}

bool RtsPathFind::Distance2(int sx, int sy, int tx, int ty)
{
	if ((abs(sx - tx) + abs(sy - ty)) > MAX_STOE_DISTACNE)
	{
		return 0;//超出距离，使用路点
	}
	else
		return 1;//在距离内，使用原始寻路
}

void RtsPathFind::LoadWayPointInfo(const char* pstrfilename)
{
	list<vector<rtPOINT> > wp_list;

	if (pstrfilename)
	{
		rt2_strcpy(m_szfilename, pstrfilename);
	}
	
	RtArchive* pFile = RtCoreFile().CreateFileReader(m_szfilename);


	if (pFile==NULL)	
		return;

	long len = pFile->TotalSize();
	char* buf = RT_NEW char[len+1];
	pFile->Serialize(buf, len);
	buf[len] = 0;
	std::string szPoint = buf;

	long iStart = 0;
	if (pFile != NULL)
	{
		long iPos = szPoint.find("\r\n", iStart);
		while(iPos != std::string::npos)
		{ 
			std::string line = szPoint.substr(iStart, iPos -iStart);
			iStart = iPos;
			vector<rtPOINT> vec_point;
			rtPOINT p;
			//第一个点
			char* npos = strtok((char*)line.c_str(), ",");
			if(npos == NULL)
				continue;
			p.x = atol(npos);

			npos = strtok(NULL, ",");
			if(npos == NULL)
				continue;
			p.y = atol(npos);

			vec_point.push_back(p);
			//tim.yang  draw point
			SPointScene P;
			P.x = p.x;
			P.y = p.y;
			RtsWayPoint::GetInstance()->m_allPathPoint.push_back(P);
			//end
			//其他点
			while (npos != NULL)
			{
				npos = strtok(NULL, ",");
				if(npos == NULL)
					break;
				p.x = atol(npos);

				npos = strtok(NULL, ",");
				if(npos == NULL)
					break;
				p.y = atol(npos);

				vec_point.push_back(p);	
			}

			wp_list.push_back(vec_point);	

			iPos = szPoint.find("\r\n", iStart+2);
		}


	}

	if(wp_list.empty())
		return;

	m_wayPointInfo.clear();
	list<vector<rtPOINT> >::iterator list_it = wp_list.begin();

	long i = 0;
	for(;list_it != wp_list.end(); ++list_it, ++i)
	{
		vector<rtPOINT>::iterator vector_it = list_it->begin();

		way_point* wp = RT_NEW way_point;
		wp->x = vector_it->x;
		wp->y = vector_it->y;
		wp->first_edge = NULL;
		way_edge *pCurEdge = NULL;
		++vector_it;

		for (; vector_it != list_it->end(); ++vector_it)
		{
			long distance = Distance(wp->x, wp->y, vector_it->x, vector_it->y);
			way_edge* pwe = RT_NEW way_edge;
			pwe->point = *vector_it;
			pwe->next_edge = NULL;
			pwe->weight = distance;
			if(pCurEdge == NULL && wp->first_edge == NULL)
			{
				wp->first_edge = pwe;
				pCurEdge = pwe;
			}
			else
			{
				pCurEdge->next_edge = pwe;
				pCurEdge = pwe;
			}	
		}	

		m_wayPointInfo.insert(make_pair(i, wp));
	}
}

bool RtsPathFind::DijkstraFindPath(long startwp, long endwp, std::vector<rtPOINT>& v)
{
	//if (startwp == endwp)
	//	return false;

	std::map<long, way_point*>::iterator it = m_wayPointInfo.begin();
	//初始化
	for (; it!=m_wayPointInfo.end(); ++it)
	{
		(it->second)->visited = false;
		(it->second)->dist = PATH_INFINITY;	
		(it->second)->parent = NULL;
	}
	//起点
	m_wayPointInfo[startwp]->dist = 0;
	long iCurPoint = startwp;

	while (1)
	{
		//开启列表中取出distance最小的点
		it = m_wayPointInfo.begin();
		long iMinDist = PATH_INFINITY;

		for (; it!=m_wayPointInfo.end(); ++it)
		{
			if ((it->second)->visited  == false && (it->second)->dist <iMinDist)
			{
				iMinDist = (it->second)->dist;
				iCurPoint = it->first;
			}
		}

		if (iMinDist == PATH_INFINITY || iCurPoint == endwp)
			break;
		if (m_wayPointInfo[iCurPoint]->visited == true )
			continue;

		m_wayPointInfo[iCurPoint]->visited = true;
		//换成更好的路径
		way_edge* tempEdge = m_wayPointInfo[iCurPoint]->first_edge;

		while (tempEdge != NULL)
		{
			long point = FindWPByPoint(tempEdge->point);
			if (point == -1)
				return false;
			if(m_wayPointInfo[point]->visited== false && m_wayPointInfo[point]->dist > m_wayPointInfo[iCurPoint]->dist + tempEdge->weight)
			{
				m_wayPointInfo[point]->dist = m_wayPointInfo[iCurPoint]->dist + tempEdge->weight;
				m_wayPointInfo[point]->parent = m_wayPointInfo[iCurPoint];
			}
			tempEdge = tempEdge->next_edge;
		}
	}

	//找到路径
	if (iCurPoint == endwp)
	{
		way_point* tempPoint = m_wayPointInfo[endwp];
		while (tempPoint != NULL)
		{
			rtPOINT pp;
			pp.x = tempPoint->x;
			pp.y = tempPoint->y;
			v.push_back(pp);

			tempPoint = tempPoint->parent;
		}
		if (v.empty())
			return false;

		return true;
	}  
	return false;
}

long RtsPathFind::GetNearPathPoint(long sx, long sy)
{
	std::map<long, way_point*>::iterator it = m_wayPointInfo.begin();
	long dist = PATH_INFINITY;
	long temp;
	long point = -1;
	for (; it!=m_wayPointInfo.end(); ++it)
	{
		temp = Distance((it->second)->x, (it->second)->y, sx, sy);
		if(temp < dist)
		{
			dist = temp;
			point = it->first;
		}
	}  
	if (dist > MAX_WP_DISTANCE)
		return -1;
	return point;
}

long RtsPathFind::FindWPByPoint(rtPOINT p)
{
	std::map<long, way_point*>::iterator it = m_wayPointInfo.begin();
	for(; it != m_wayPointInfo.end(); ++it)
	{
		if (it->second->x == p.x && it->second->y == p.y)
		{
			return it->first;
		}
	}
	return -1;
}

bool RtsPathFind::FindPathFromWP(RtsPathPath* pActor,int startX, int startY, int endX, int endY,bool bIgnoreActor, bool bIgnoreTarget,int actorType)
{
	long start = GetBestStartPathPoint(pActor, startX, startY, endX, endY);
	long end = GetNearPathPoint(endX,endY);

	std::vector<rtPOINT> pointlist;
	if (-1 == start || -1 == end/* || start == end*/)
	{
		return false;
	}
	rtPOINT final, pstart;
	final.x = endX;
	final.y = endY;
	pstart.x = startX;
	pstart.y = startY;
//	pointlist.push_back(final);//将目的地点压入，在后面的寻路过程中就会包括该点
	if (DijkstraFindPath(start,end,pointlist))
	{
		if (pointlist.empty())
		{
			return false;
		}

//		pointlist.push_back(pstart);//将出发地点压入，在后面的平滑过程中会删除该点，这样就不会被平滑函数删除第一个路点
		/*
		从dijkstra得出的路点列表中删除一些点，目的不走回头路
		方法：dijkstra得出的路点列表是反向保存的，第一个路点是目的地点。
		从目的地点开始遍历，直到找到一个能与起始点相通而且距离不大于两个路点之间距离的2倍(该数值可自定)的路点结束，
		将该路点以后的路点全部删除，那么该路点就作为从起始点开始寻路的第一个路点
		*/
//		SFindNode* pCurNode = 0;
		//std::vector<rtPOINT>::iterator it = pointlist.begin();
		//RtSceneBlockTerrain::STileAttr* pAttr = 0;
		//for (;it != pointlist.end(); ++it)
		//{
		//	pAttr = m_pTerrain->GetTerrainPathAttr(it->x, it->y);
		//	SPointScene s,e;
		//	s.x = startX;
		//	s.y = startY;
		//	e.x = it->x;
		//	e.y = it->y;
		//	if (pAttr && Distance(s.x,s.y,e.x,e.y) <= MAX_WP_DISTANCE * 2)
		//	{
		//		pCurNode = FindPathByTwoPos(pActor->cBlock,s,e,bIgnoreActor,bIgnoreTarget,actorType);
		//		if (pCurNode)
		//		{
		//			++it;
		//			pointlist.erase(it,pointlist.end());
		//			break;
		//		}
		//	}
		//}
		//SPointScene s,e;
		//s.x = pActor->curPos.MinX;
		//s.y = pActor->curPos.MinY;
		//e.x = pointlist[pointlist.size() - 1].x;
		//e.y = pointlist[pointlist.size() - 1].y;
		//SFindNode* pCurNode = 0;
		//pCurNode = FindPathByTwoPos(pActor->cBlock,s,e,bIgnoreActor,bIgnoreTarget,actorType);
		//if (!pCurNode)
		//{
		//	return false;
		//}
		//pActor->ClearPath();//寻路之前清除人身上的路点信息
		//保存起始点与第一个路点之间的寻路信息，这些点都是已经寻路过的
		//while (pCurNode)
		//{
		//	if (pActor->m_iLast>=PF_MAX_DEPTH) 
		//		return false;
		//	pActor->m_Path[pActor->m_iLast].MinX = pCurNode->x;
		//	pActor->m_Path[pActor->m_iLast].MinY = pCurNode->y;
		//	pActor->m_Path[pActor->m_iLast].isWalk = 1;//表示已经寻路
		//	++pActor->m_iLast;
		//	pCurNode = pCurNode->pParent;
		//}

		//进行初始点到一个路点A*寻路
		FindPathNormal(pActor, startX, startY, m_wayPointInfo[start]->x, m_wayPointInfo[start]->y, bIgnoreActor, bIgnoreTarget, actorType);
		--pActor->m_iLast;//回退一个路点避免重复

		std::vector<rtPOINT>::reverse_iterator itr = pointlist.rbegin();
		//++itr;//将第一个路点删除
		//保存第一个路点以后的路点
		for (;itr != pointlist.rend(); ++itr)
		{
			if (pActor->m_iLast>=PF_MAX_DEPTH) 
				return false;
			pActor->m_Path[pActor->m_iLast].MinX = itr->x;
			pActor->m_Path[pActor->m_iLast].MinY = itr->y;
			pActor->m_Path[pActor->m_iLast].isWalk = 1;//表示已经寻路
			++pActor->m_iLast;
		}

		--pActor->m_iLast;//回退一个路点避免重复

		//进行最后一个路点到目的A*寻路
		FindPathNormal(pActor, m_wayPointInfo[end]->x, m_wayPointInfo[end]->y, endX, endY, bIgnoreActor, bIgnoreTarget, actorType, true);

		return _check_path(pActor);
	}
	else
	{
		return false;
	}
}

//int RtsPathFind::GetDistanceFromWP(std::vector<rtPOINT> wplist)
//{
//	int totalWPDis = 0;
//	if (wplist.empty())
//	{
//		return totalWPDis;
//	}
//	std::vector<rtPOINT>::iterator it =wplist.begin();
//	SPointScene start,end;
//	start.x = wplist[0].x;
//	start.y = wplist[0].y;
//	for (;it != wplist.end(); ++it)
//	{
//		end.x = it->x;
//		end.y = it->y;
//		totalWPDis += Distance(start.x,start.y,end.x,end.y); 
//		start.x = it->x;
//		start.y = it->y;
//	}
//	return totalWPDis;
//}

bool RtsPathFind::PushFrontInActorPath(RtsPathPath* path,SFindNode* curNode)
{
	if (!path || !curNode)
	{
		return false;
	}
	int i = 0;
	std::vector<RtsPathPos::PPos> temp;
	while ((path->m_iFirst) + i <= (path->m_iLast))
	{
		RtsPathPos::PPos oneWP;
		oneWP.MinX = path->m_Path[path->m_iFirst+i].MinX;
		oneWP.MinY = path->m_Path[path->m_iFirst+i].MinY;
		temp.push_back(oneWP);
		++i;
	}
	path->m_iLast = path->m_iFirst;//重新开始赋值
	while (curNode)
	{
		if (path->m_iLast>=PF_MAX_DEPTH) break;
		path->m_Path[path->m_iLast].MinX = curNode->x;
		path->m_Path[path->m_iLast].MinY = curNode->y;
		path->m_Path[path->m_iLast].isWalk = 1;
		path->m_iLast ++;
		curNode = curNode->pParent;
	}
	std::vector<RtsPathPos::PPos>::iterator it = temp.begin();
	for (;it!=temp.end();++it)
	{
		if (path->m_iLast>=PF_MAX_DEPTH) 
			return false;
		path->m_Path[path->m_iLast].MinX = it->MinX;
		path->m_Path[path->m_iLast].MinY = it->MinY;
		path->m_iLast ++;
	}
	return _check_path(path);
}

bool RtsPathFind::HaveBlockoffAround(int iX,int iY)
{
	RtSceneBlockTerrain::STileAttr* pAttr = m_pTerrain->GetTerrainPathAttr(iX+1, iY);

	if (pAttr==NULL || pAttr->bWalk)    return true; // 如果此处是障碍就直接返回


	pAttr = m_pTerrain->GetTerrainPathAttr(iX-1, iY);

	if (pAttr==NULL || pAttr->bWalk)    return true; // 如果此处是障碍就直接返回
	

	pAttr = m_pTerrain->GetTerrainPathAttr(iX, iY+1);

	if (pAttr==NULL || pAttr->bWalk)    return true; // 如果此处是障碍就直接返回
	
	
	pAttr = m_pTerrain->GetTerrainPathAttr(iX, iY-1);

	if (pAttr==NULL || pAttr->bWalk)    return true; // 如果此处是障碍就直接返回

	return false;
}

long RtsPathFind::GetBestStartPathPoint(RtsPathPath* pActor, long sx, long sy, long ex, long ey)
{
	std::deque<long> WPqueue;
	WPqueue.clear();

	int iWPCount = GetNeartwoPathPoint(pActor, sx, sy, WPqueue);

	if (iWPCount == -1)
	{
		return -1;
	}else if (iWPCount == 1)
	{
		return WPqueue.back();
	}

	double MinRadians = -1;
	double TempRadians = -1;

	long point = -1;
	long Minpoint = -1;

	rtPOINT pStart, pEnd, pWP;

	way_point* pWPTemp = NULL;

	pStart.x = sx; 
	pStart.y = sy;

	pEnd.x	= ex;
	pEnd.y	= ey;

	for (int i =0; i < iWPCount; i++)
	{
		point = WPqueue.back();
		pWPTemp = m_wayPointInfo[point];
		
		if (!pWPTemp)
		{
			return -1;
		}

		pWP.x = pWPTemp->x;
		pWP.y = pWPTemp->y;

		TempRadians = GetRadians(pStart, pEnd, pWP);

		if (MinRadians == -1)
		{
			MinRadians = TempRadians;
			Minpoint = point;
		}
		else if (TempRadians>= 0 && TempRadians < 10 && TempRadians < MinRadians)
		{
			MinRadians = TempRadians;
			Minpoint = point;
		}

		WPqueue.pop_back();

	}

	return Minpoint;
}

int RtsPathFind::GetNeartwoPathPoint(RtsPathPath* pActor, long sx, long sy, deque<long>& WPqueue)
{
	std::map<long, way_point*>::iterator it = m_wayPointInfo.begin();
	long dist = PATH_INFINITY;
	long temp;
	long point = -1;
	for (; it!=m_wayPointInfo.end(); ++it)
	{
		temp = Distance((it->second)->x, (it->second)->y, sx, sy);
		if(temp < dist)
		{
			dist = temp;
			point = it->first;
			
			if (dist <= MAX_WP_DISTANCE && WalkAble(pActor->dwActorId, pActor->cType, sx, sy, (it->second)->x, (it->second)->y, true))
			{
				WPqueue.push_back(point);
			}
			
			if (WPqueue.size() > 2)
			{
				WPqueue.pop_front();
			}
		}
	}  
	if (dist > MAX_WP_DISTANCE)
		return -1;

	return WPqueue.size();
}
double RtsPathFind::GetRadians(rtPOINT p1, rtPOINT p2, rtPOINT p3)
{
	double x3, x4, y3, y4, x5;

	x3 = p2.x - p1.x;
	y3 = p2.y - p1.y;
	x4 = p3.x - p1.x;
	y4 = p3.y - p1.y;
	//单位化
	x5 = sqrt(x3*x3 + y3*y3);
	x3 /= x5;
	y3 /= x5;

	x5 = sqrt(x4*x4 + y4*y4);
	x4 /= x5;
	y4 /= x5;

	double fainl = (x3 * x4) + (y3 * y4);

	return acos(fainl);
}
/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
