// meshlod.h: interface for the CMeshLod class.

#ifndef __C_MESH_LOD_H__
#define __C_MESH_LOD_H__

#include <math.h>
#include <float.h>

#include <set>
#include <vector>
#include <algorithm>
using namespace std;
#include "Quadric.h"

class CMeshLod  
{
public:
	struct SVertex
	{
		CQuadric quadric;
		int idx;
		int lodId;
		int nextId;				// the replacer id
		int faceReduceNum;
		int srcId;				// source id
		bool bCollapsed;
		bool bFreezed;
		
		float error;
		float x,y,z;			// position
		float nx,ny,nz;			// normal
		set<int> setNeighbourId;
		set<int> setFaceId;
		set<int> setFaceNormal;
	};

	struct SFace
	{
		int lodId;
		int vId[3];
		SVertex normal;
	};

public:
	void Reserve(int vCnt,int fCnt);
	SVertex * GetVertexInfo(int vId);
	void CalculateVertexNormal();
	
	CMeshLod();
	virtual ~CMeshLod();

	// generate lod data
	void CalculateLod();

	// add vertex and face info
	void  AddVertex(float x,float y,float z,float nx,float ny,float nz);
	void  AddFace(int v1,int v2,int v3);
	void  MarkDuplicate();

	// get result
	int GetVertexLodId(int vId);
	int GetFaceLodId(int faceId);
	int GetLodCount();
	int GetVertexReplacerId(int vId);
	int GetVertexFaceCollapseNum(int vId);
	
private:
	vector<SVertex> m_vtx;
	vector<SFace>   m_face;
	vector<SVertex> m_duplicate;
	
	int m_nextVertexLodId;
	int m_nextFaceLodId;
	int m_lodNum;

	float GetFaceArea(SVertex *v1,SVertex *v2,SVertex *v3);
	void  GetFaceNormal(SVertex *v1,SVertex *v2,SVertex *v3,SVertex *normal);
	void  RealMarkDuplicate(vector<SVertex>::iterator beg,vector<SVertex>::iterator end);
	int   GetNextCollapsingVertexId();
	void  CalculateAllQuadrics();
	void  CalculateVertexCollapse(int vertexId);
	void  CollapseVertex(int vertexId);
	void  FreezeBorders();
	float CalculateCollapseError(int collapsingVertexId, int targetVertexId);
};

#endif 