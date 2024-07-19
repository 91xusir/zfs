
#ifndef GC_PATH_FIND_H
#define GC_PATH_FIND_H
#include "rts_scene_terrain.h"
#include "core/RtPool.h"
class RtsSceneDoc;
class RtsPathFind;
class RtsPathPath;
class RtSceneBlockTerrain;

//tim.yang automove
const int DEPTH = 10000;
const int MAXNODE = 30000;
#define PATH_INFINITY 9999999
#define MAX_STOE_DISTACNE 100 //超过此距离使用路点
#define MAX_WP_DISTANCE 100 //两路点间最大距离
//end

enum TPathResult        // 路径搜索返回值
{
	PR_OK        = 0,   // 路径搜索正确
	PR_CANCEL       ,   // 被取消
	PR_CTERRAIN     ,   // 被Terrain阻挡
	PR_CPLAYER      ,   // 被Player阻挡
	PR_NONE         ,   // 什么也没有找到
};

enum TPathType          // 路径搜索返回值
{
	PT_OBJ      = 0x01,
	PT_PATH     = 0x02,
	PT_ALL      = 0xFF,
};

enum TBlockType
{
	PB_NONE		= 0,	//默认 (不阻挡也不被阻挡)
	PB_UNBLOCK	= 1,	//不阻挡 (不阻挡但会被阻挡)
	PB_BLOCK	= 2,	//阻挡	(阻挡且会被阻挡)
	PB_CHECK	= 3,	//通过函数检测 (检测阻挡且会被阻挡)
};

enum
{
	PF_MAX_DEPTH        = 3000,           // 路径搜索的最大深度  tim.yang automove  change 300 to 3000
	PF_FLAG_COLLISION   = 0xFFFFFFFF,   // 地表的碰撞
};

/*----------------------------------------------------------------------------
-   RtsPathPath 路径搜索节点
----------------------------------------------------------------------------*/

class RtsPathPos
{
public:
	struct PPos
	{
		int MinX;
		int MinY;
		int MaxX;
		int MaxY;
		bool isWalk;//0--未寻路  1--已寻路
		PPos(){isWalk = 0;}
	};

	RtsPathPos():dwActorId(0),sActorType(0),m_pChkFunc(NULL)	{SetPos(0,0,0,0);pNext=NULL;cType=PT_OBJ;cBlock=PB_NONE;}
	void SetPos(int x,int y){curPos.MinX=x; curPos.MinY=y;curPos.MaxX=x; curPos.MaxY=y;}
	void SetPos(int iMinX,int iMinY,int iMaxX,int iMaxY){curPos.MinX=iMinX; curPos.MinY=iMinY;curPos.MaxX=iMaxX; curPos.MaxY=iMaxY;}
	void SetMin(int iMinX, int iMinY){curPos.MinX=iMinX; curPos.MinY=iMinY;}
	void SetMax(int iMaxX, int iMaxY){curPos.MaxX=iMaxX; curPos.MaxY=iMaxY;}
	int CurMinX(){return curPos.MinX;}
	int CurMinY(){return curPos.MinY;}
	int CurMaxX(){return curPos.MaxX;}
	int CurMaxY(){return curPos.MaxY;}
	void OnCreate(){}
	void OnDestroy(){}

	bool (*m_pChkFunc)(short,DWORD,short,DWORD) ;

	PPos        curPos;               // 当前位置

	RtsPathPos* pNext;
	char        cBlock;
	char        cType;
	DWORD		dwActorId;
	short		sActorType;
};

class RtsPathPath : public RtsPathPos
{
public:
	friend class RtsPathFind;
public:
	RtsPathPath(){ClearPath();cType=PT_PATH;memset(m_Path,0,sizeof(RtsPathPos::PPos)*(PF_MAX_DEPTH+1));}
	void ClearPath()                
	{
		if (m_iFirst>=m_iLast)		
			m_iFirst=m_iLast=0;		
	}
	void ClearPathEx()
	{
		m_iFirst=m_iLast=0;
	}
	bool Next(int& x, int& y,RtsPathFind* pPath ,bool bIgnoreActor = 0, bool bIgnoreTarget = 0,int actorType = 0);
	bool Next(int& x,int& y);
	PPos* GetStep(int iStep); // 这个函数只是提供遍历使用，其他慎用，返回NULL表示已经没有路径了
	int PathCount();//向外返回所有路径
	PPos* GetAllPath(int& iLast);

protected:
	RtsPathPos::PPos    m_Path[PF_MAX_DEPTH+1]; // 搜索到的路径
	int                 m_iFirst;               // 队列头
	int                 m_iLast;                // 队列尾部(指向尾部的下一个地址，所以当头部等于尾部的时候队列是空的)

};

class RtsPathContainer
{
public:
	int             iBlock;
	int				iCheck;
	RtsPathPos*     pNodes;

	void AddNode(RtsPathPos* pNode);
	bool RemoveNode(RtsPathPos* pNode);
	RtsPathPos* FindActor(DWORD dwActorId,short sActorType);
	RtsPathPos* GetBlockNode();
	bool IsCheck()  { return iCheck!=0; }
	bool IsBlock()  { return iBlock!=0; }
	void OnCreate(){}
	void OnDestroy()
	{
		CHECK (pNodes==NULL);
		CHECK (iBlock==0);
		CHECK (iCheck==0);
	}

	RtsPathContainer():iBlock(0),iCheck(0),pNodes(NULL){};
};

/*----------------------------------------------------------------------------
-   RtsPathFind 路径搜索类
----------------------------------------------------------------------------*/
struct SFindNode
{
	WORD        wDepth;    // 搜索深度
	WORD        wWeight;   // Node权重
	int         x, y;       // Node位置
	SFindNode*  pParent;    // 父亲节点 (当Node没有用到的时候还表示链接)
	SFindNode*  pNextBest;  // 下一个高优先级的 (当已经处理过的时候还表示链接)
};
class RtsPathFind
{
public:
	RtsPathFind();
	virtual ~RtsPathFind();

	void InitOnce();        // 只初始化一次的初始化
	void ClearOnce();       // 只清除一次的清除

	void SetBindTerrain(RtsSceneDoc* pTerrain);          // 更新场景信息到这里
	//void OnLoadTerrainBlock(RtSceneBlockTerrain* pBlock);
	//void OnUnloadTerrainBlock(RtSceneBlockTerrain* pBlock);

	bool FindSpace(DWORD dwActorId,short sActorType,int iTerrainX, int iTerrainY, int& iSpaceX, int& iSpaceY, bool bOnlyOne=false);

	// Actor Operator
	RtsPathPos* AddActor(DWORD dwActorId,short sActorType,int iTerrainX, int iTerrainY,bool bPath, char cBlock,void* pChkFunc=NULL);  // 添加角色到指定网格，返回PR_OK表示完成，否则就是有阻挡
	RtsPathPos* AddActor(DWORD dwActorId,short sActorType,int iMinTerrainX, int iMinTerrainY,int iMaxTerrainX, int iMaxTerrainY,bool bPath, char cBlock,void* pChkFunc=NULL);

	bool CanAdd(DWORD dwActorId,short sActorType,int iTerrainX, int iTerrainY, char cBlock=PB_BLOCK, bool bOnlyOne=false);
	bool CanAdd(DWORD dwActorId,short sActorType,int iMinTerrainX, int iMinTerrainY,int iMaxTerrainX, int iMaxTerrainY, char cBlock=PB_BLOCK, bool bOnlyOne=false);
	bool IsWalkMask(int iTerrainX, int iTerrainY);
	bool RemoveActor(RtsPathPath*& pActor);                                     // 删除角色，而不管这个角色在什么地方，返回PR_OK表示完成
	bool RemoveActor(RtsPathPos*& pActor);                                      // 删除角色，而不管这个角色在什么地方，返回PR_OK表示完成
	bool MoveTo(RtsPathPos*& pActor, int iX, int iY ,char cBlock);
	bool MoveTo(RtsPathPath*& pActor, int iX, int iY ,char cBlock);
	bool MoveTo(RtsPathPos*& pActor,int iMinTerrainX, int iMinTerrainY,int iMaxTerrainX, int iMaxTerrainY ,char cBlock);
	bool MoveTo(RtsPathPath*& pActor,int iMinTerrainX, int iMinTerrainY,int iMaxTerrainX, int iMaxTerrainY ,char cBlock);



	// 查找路径，这个查找是最短路径
	//     pNode 为需要查找路径的物体的结构体，里面包含了他的当前位置和需要保存的结果
	//     wTargetX, wTargetY 是需要查找到目的点的位置
	//     返回值 PR_OK 表示正确，其他请参考TPathResult的具体意义
	bool FindPath(RtsPathPath* pNode, int iTargetX, int iTargetY);
	bool SmoothPath(RtsPathPath* pNode,bool bIgnoreActor=false);
	bool RtsPathFind::_check_path(RtsPathPath* pNode);
	bool FindPathEx(RtsPathPath* pNode, int iTargetX, int iTargetY, bool bIgnoreActor, bool bIgnoreTarget,int actorType = 0);//0--monster  1--player

	char GetSearchType()            { return m_cSearchType; }
	void SetSearchType(char cType)  { m_cSearchType = cType; }

	// 在控制台中显示点(wSX, wSY)周围(wSizeX, wSizeY)大小的地表
	void OutputTerrain(int iSX, int iSY, int iDX, int iDY, int iSizeX, int iSizeY);

protected:
	bool WalkAble(DWORD dwActorId,short sActorType,int vStartX,int vStartY,int vEndX,int vEndY,bool vIgnoreActor);//不含出发点
	void ClearIncreaseFlag();
	bool HaveBlockoffAround(int iX,int iY);//寻找周围4格是否有阻挡

	//struct SFindNode
	//{
	//	WORD        wDepth;    // 搜索深度
	//	WORD        wWeight;   // Node权重
	//	int         x, y;       // Node位置
	//	SFindNode*  pParent;    // 父亲节点 (当Node没有用到的时候还表示链接)
	//	SFindNode*  pNextBest;  // 下一个高优先级的 (当已经处理过的时候还表示链接)
	//};
	inline WORD NodeJudge(int x, int y) {int i = (m_iCurTargetX-x)*(m_iCurTargetX-x)+(m_iCurTargetY-y)*(m_iCurTargetY-y); if (i<0) i=0x7FFFFFFF; return i;}
	int NextFindNode(char cBlock,int x, int y, SFindNode* pNode,int actorType); // 0 正确, 1 不能行走, 其他 错误
	void IncreaseFlag()
	{
		m_wIncreaseFlag++;
		if (m_wIncreaseFlag==32767)
		{
			ClearIncreaseFlag();
		}
	}

public:
	RtPool<RtsPathContainer, 100>   m_poolContainer;
	RtPool<RtsPathPos, 100>         m_poolPos;
	RtPool<RtsPathPath, 100>        m_poolPath;

protected:
	TPathResult     m_lastResult;
	RtsSceneDoc*    m_pTerrain; // 绑定的场景地表
	int             m_iXWidth;  // 网格的X宽度
	int             m_iYWidth;  // 网格的Y宽度

	WORD            m_wIncreaseFlag;        // 递增访问标记
	bool            m_bIgnoreActor;         // 是否忽略地图上的人(如果忽略就只有阻挡能阻拦玩家)
	bool            m_bIgnoreTarget;        // 是否忽略目标点(如果忽略，就会再离目标点一点的地方停下)
	char            m_cSearchType;          // 0 4方向搜索(Default), 1 8方向搜索

	bool            m_bErrExit;             // 是否是错误退出
	//char            m_szPathFindMsg[100];   // 错误退出时候的错误信息
	CM_MEMDEF(m_szPathFindMsg, 100)

	WORD            m_wMaxDepth;            // 访问的最深深度，高于这个深度就放弃
	WORD            m_wCurMaxDepth;         // 当前这次路径搜索的最高深度
	int             m_iCurTargetX;          // 这次路径搜索的目标位置 (x,y)
	int             m_iCurTargetY;

	int             m_iAddActorCount;

	int             m_iMaxNode;             // 最大的堆栈
	SFindNode*      m_pNodes;
	SFindNode*      m_pPathRoot;
	SFindNode*      m_pWeightRoot;
	SFindNode*      m_pCloseRoot;
	SFindNode*      m_pUnuseRoot;

//tim.yang automove
	//int m_g;
	int m_h;
	//int m_gRef;
	//inline int GetGValued() {return m_g;}
	inline int GetHValued() {return m_h;}
public:
	inline void InitAStar() {/* m_g = 0; */m_h = 0/*;m_gRef = 0*/;}
//end

	//jin.wang 路点信息
	//jin.wang
public:
	struct  rtPOINT
	{
		long x;
		long y;
	};
	void SaveWayPointInfo(list<rtPOINT>& PathPoint);
	void LoadWayPointInfo(const char* pstrfilename);
	bool DijkstraFindPath(long startwp, long endwp, std::vector<rtPOINT>& v);
	long Distance(long sx, long sy, long tx, long ty);
	bool Distance2(int sx, int sy, int tx, int ty);
	long GetNearPathPoint(long sx, long sy);
	long GetBestStartPathPoint(RtsPathPath* pActor, long sx, long sy, long ex, long ey);
	int GetNeartwoPathPoint(RtsPathPath* pActor, long sx, long sy, deque<long>& WPqueue);
	double GetRadians(rtPOINT p1, rtPOINT p2, rtPOINT p3);//取3点夹角弧度
	long FindWPByPoint(rtPOINT p);
	SFindNode* FindPathByTwoPos(char cBlock, SPointScene spSrc, 
		SPointScene spTarget, bool bIgnoreActor, bool bIgnoreTarget, int actorType);
	bool FindPathFromWP(RtsPathPath* pNode,int startX, int startY, int endX, int endY, 
		bool bIgnoreActor, bool bIgnoreTarget,int actorType = 0);
	bool FindPathNormal(RtsPathPath* pNode,int startX, int startY, int endX, int endY, 
		bool bIgnoreActor, bool bIgnoreTarget,int actorType = 0, bool bAppend = false);
	//int GetDistanceFromWP(std::vector<rtPOINT> wplist);//求从第一个路点到终点所有点距离相加的总和。
	bool PushFrontInActorPath(RtsPathPath* path,SFindNode* curNode);//将相通两点之间的路点信息从前面插入到人身上路点数组中
	struct way_edge
	{
		rtPOINT point;
		long weight;
		way_edge* next_edge;
	};

	struct way_point
	{
		long x;
		long y;
		bool visited;
		long dist;
		way_edge* first_edge;
		way_point* parent; // 记录路径回溯用
	};

	std::map<long, way_point*> m_wayPointInfo;
	//char m_szfilename[256];
	CM_MEMDEF(m_szfilename, 256)
};

#endif//GC_PATH_FIND_H
/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
