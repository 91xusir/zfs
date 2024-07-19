
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
#define MAX_STOE_DISTACNE 100 //�����˾���ʹ��·��
#define MAX_WP_DISTANCE 100 //��·���������
//end

enum TPathResult        // ·����������ֵ
{
	PR_OK        = 0,   // ·��������ȷ
	PR_CANCEL       ,   // ��ȡ��
	PR_CTERRAIN     ,   // ��Terrain�赲
	PR_CPLAYER      ,   // ��Player�赲
	PR_NONE         ,   // ʲôҲû���ҵ�
};

enum TPathType          // ·����������ֵ
{
	PT_OBJ      = 0x01,
	PT_PATH     = 0x02,
	PT_ALL      = 0xFF,
};

enum TBlockType
{
	PB_NONE		= 0,	//Ĭ�� (���赲Ҳ�����赲)
	PB_UNBLOCK	= 1,	//���赲 (���赲���ᱻ�赲)
	PB_BLOCK	= 2,	//�赲	(�赲�һᱻ�赲)
	PB_CHECK	= 3,	//ͨ��������� (����赲�һᱻ�赲)
};

enum
{
	PF_MAX_DEPTH        = 3000,           // ·��������������  tim.yang automove  change 300 to 3000
	PF_FLAG_COLLISION   = 0xFFFFFFFF,   // �ر����ײ
};

/*----------------------------------------------------------------------------
-   RtsPathPath ·�������ڵ�
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
		bool isWalk;//0--δѰ·  1--��Ѱ·
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

	PPos        curPos;               // ��ǰλ��

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
	PPos* GetStep(int iStep); // �������ֻ���ṩ����ʹ�ã��������ã�����NULL��ʾ�Ѿ�û��·����
	int PathCount();//���ⷵ������·��
	PPos* GetAllPath(int& iLast);

protected:
	RtsPathPos::PPos    m_Path[PF_MAX_DEPTH+1]; // ��������·��
	int                 m_iFirst;               // ����ͷ
	int                 m_iLast;                // ����β��(ָ��β������һ����ַ�����Ե�ͷ������β����ʱ������ǿյ�)

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
-   RtsPathFind ·��������
----------------------------------------------------------------------------*/
struct SFindNode
{
	WORD        wDepth;    // �������
	WORD        wWeight;   // NodeȨ��
	int         x, y;       // Nodeλ��
	SFindNode*  pParent;    // ���׽ڵ� (��Nodeû���õ���ʱ�򻹱�ʾ����)
	SFindNode*  pNextBest;  // ��һ�������ȼ��� (���Ѿ��������ʱ�򻹱�ʾ����)
};
class RtsPathFind
{
public:
	RtsPathFind();
	virtual ~RtsPathFind();

	void InitOnce();        // ֻ��ʼ��һ�εĳ�ʼ��
	void ClearOnce();       // ֻ���һ�ε����

	void SetBindTerrain(RtsSceneDoc* pTerrain);          // ���³�����Ϣ������
	//void OnLoadTerrainBlock(RtSceneBlockTerrain* pBlock);
	//void OnUnloadTerrainBlock(RtSceneBlockTerrain* pBlock);

	bool FindSpace(DWORD dwActorId,short sActorType,int iTerrainX, int iTerrainY, int& iSpaceX, int& iSpaceY, bool bOnlyOne=false);

	// Actor Operator
	RtsPathPos* AddActor(DWORD dwActorId,short sActorType,int iTerrainX, int iTerrainY,bool bPath, char cBlock,void* pChkFunc=NULL);  // ��ӽ�ɫ��ָ�����񣬷���PR_OK��ʾ��ɣ�����������赲
	RtsPathPos* AddActor(DWORD dwActorId,short sActorType,int iMinTerrainX, int iMinTerrainY,int iMaxTerrainX, int iMaxTerrainY,bool bPath, char cBlock,void* pChkFunc=NULL);

	bool CanAdd(DWORD dwActorId,short sActorType,int iTerrainX, int iTerrainY, char cBlock=PB_BLOCK, bool bOnlyOne=false);
	bool CanAdd(DWORD dwActorId,short sActorType,int iMinTerrainX, int iMinTerrainY,int iMaxTerrainX, int iMaxTerrainY, char cBlock=PB_BLOCK, bool bOnlyOne=false);
	bool IsWalkMask(int iTerrainX, int iTerrainY);
	bool RemoveActor(RtsPathPath*& pActor);                                     // ɾ����ɫ�������������ɫ��ʲô�ط�������PR_OK��ʾ���
	bool RemoveActor(RtsPathPos*& pActor);                                      // ɾ����ɫ�������������ɫ��ʲô�ط�������PR_OK��ʾ���
	bool MoveTo(RtsPathPos*& pActor, int iX, int iY ,char cBlock);
	bool MoveTo(RtsPathPath*& pActor, int iX, int iY ,char cBlock);
	bool MoveTo(RtsPathPos*& pActor,int iMinTerrainX, int iMinTerrainY,int iMaxTerrainX, int iMaxTerrainY ,char cBlock);
	bool MoveTo(RtsPathPath*& pActor,int iMinTerrainX, int iMinTerrainY,int iMaxTerrainX, int iMaxTerrainY ,char cBlock);



	// ����·����������������·��
	//     pNode Ϊ��Ҫ����·��������Ľṹ�壬������������ĵ�ǰλ�ú���Ҫ����Ľ��
	//     wTargetX, wTargetY ����Ҫ���ҵ�Ŀ�ĵ��λ��
	//     ����ֵ PR_OK ��ʾ��ȷ��������ο�TPathResult�ľ�������
	bool FindPath(RtsPathPath* pNode, int iTargetX, int iTargetY);
	bool SmoothPath(RtsPathPath* pNode,bool bIgnoreActor=false);
	bool RtsPathFind::_check_path(RtsPathPath* pNode);
	bool FindPathEx(RtsPathPath* pNode, int iTargetX, int iTargetY, bool bIgnoreActor, bool bIgnoreTarget,int actorType = 0);//0--monster  1--player

	char GetSearchType()            { return m_cSearchType; }
	void SetSearchType(char cType)  { m_cSearchType = cType; }

	// �ڿ���̨����ʾ��(wSX, wSY)��Χ(wSizeX, wSizeY)��С�ĵر�
	void OutputTerrain(int iSX, int iSY, int iDX, int iDY, int iSizeX, int iSizeY);

protected:
	bool WalkAble(DWORD dwActorId,short sActorType,int vStartX,int vStartY,int vEndX,int vEndY,bool vIgnoreActor);//����������
	void ClearIncreaseFlag();
	bool HaveBlockoffAround(int iX,int iY);//Ѱ����Χ4���Ƿ����赲

	//struct SFindNode
	//{
	//	WORD        wDepth;    // �������
	//	WORD        wWeight;   // NodeȨ��
	//	int         x, y;       // Nodeλ��
	//	SFindNode*  pParent;    // ���׽ڵ� (��Nodeû���õ���ʱ�򻹱�ʾ����)
	//	SFindNode*  pNextBest;  // ��һ�������ȼ��� (���Ѿ��������ʱ�򻹱�ʾ����)
	//};
	inline WORD NodeJudge(int x, int y) {int i = (m_iCurTargetX-x)*(m_iCurTargetX-x)+(m_iCurTargetY-y)*(m_iCurTargetY-y); if (i<0) i=0x7FFFFFFF; return i;}
	int NextFindNode(char cBlock,int x, int y, SFindNode* pNode,int actorType); // 0 ��ȷ, 1 ��������, ���� ����
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
	RtsSceneDoc*    m_pTerrain; // �󶨵ĳ����ر�
	int             m_iXWidth;  // �����X���
	int             m_iYWidth;  // �����Y���

	WORD            m_wIncreaseFlag;        // �������ʱ��
	bool            m_bIgnoreActor;         // �Ƿ���Ե�ͼ�ϵ���(������Ծ�ֻ���赲���������)
	bool            m_bIgnoreTarget;        // �Ƿ����Ŀ���(������ԣ��ͻ�����Ŀ���һ��ĵط�ͣ��)
	char            m_cSearchType;          // 0 4��������(Default), 1 8��������

	bool            m_bErrExit;             // �Ƿ��Ǵ����˳�
	//char            m_szPathFindMsg[100];   // �����˳�ʱ��Ĵ�����Ϣ
	CM_MEMDEF(m_szPathFindMsg, 100)

	WORD            m_wMaxDepth;            // ���ʵ�������ȣ����������Ⱦͷ���
	WORD            m_wCurMaxDepth;         // ��ǰ���·��������������
	int             m_iCurTargetX;          // ���·��������Ŀ��λ�� (x,y)
	int             m_iCurTargetY;

	int             m_iAddActorCount;

	int             m_iMaxNode;             // ���Ķ�ջ
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

	//jin.wang ·����Ϣ
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
	double GetRadians(rtPOINT p1, rtPOINT p2, rtPOINT p3);//ȡ3��нǻ���
	long FindWPByPoint(rtPOINT p);
	SFindNode* FindPathByTwoPos(char cBlock, SPointScene spSrc, 
		SPointScene spTarget, bool bIgnoreActor, bool bIgnoreTarget, int actorType);
	bool FindPathFromWP(RtsPathPath* pNode,int startX, int startY, int endX, int endY, 
		bool bIgnoreActor, bool bIgnoreTarget,int actorType = 0);
	bool FindPathNormal(RtsPathPath* pNode,int startX, int startY, int endX, int endY, 
		bool bIgnoreActor, bool bIgnoreTarget,int actorType = 0, bool bAppend = false);
	//int GetDistanceFromWP(std::vector<rtPOINT> wplist);//��ӵ�һ��·�㵽�յ����е������ӵ��ܺ͡�
	bool PushFrontInActorPath(RtsPathPath* path,SFindNode* curNode);//����ͨ����֮���·����Ϣ��ǰ����뵽������·��������
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
		way_point* parent; // ��¼·��������
	};

	std::map<long, way_point*> m_wayPointInfo;
	//char m_szfilename[256];
	CM_MEMDEF(m_szfilename, 256)
};

#endif//GC_PATH_FIND_H
/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
