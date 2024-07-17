// meshlod.cpp: implementation of the CMeshLod class.
#include "meshlod.h"
#define IS_DUP(x,y) (fabs(x-y)<0.001)

CMeshLod::CMeshLod()
{

}

CMeshLod::~CMeshLod()
{

}

void CMeshLod::CalculateLod()
{
	CalculateVertexNormal();
	CalculateAllQuadrics();
	FreezeBorders();

	int i;
	for(i = 0; i < m_vtx.size(); i++)
	{
		CalculateVertexCollapse(i);
	}

	m_nextVertexLodId = m_vtx.size() - 1;
	m_nextFaceLodId = m_face.size() - 1;
	while(true)
	{
		int vId = GetNextCollapsingVertexId();
		if(vId == -1) break;
		// replace vertex
		CollapseVertex(vId);
	}

	m_nextVertexLodId = 0;
	m_nextFaceLodId = 0;

	// set all lod id
	for(i = 0; i < m_vtx.size(); i++)
	{
		if(m_vtx[i].lodId == -1)
		{
			m_vtx[i].lodId = m_nextVertexLodId++;
			m_vtx[i].nextId = -1;
		}
	}

	// transform the id order to the lod id
	for(i=0;i<m_vtx.size();i++)
	{
		if(m_vtx[i].nextId != -1)
		{
			m_vtx[i].nextId = m_vtx[m_vtx[i].nextId].lodId;
		}
	}

	for(i=0;i<m_face.size();i++)
	{
		if(m_face[i].lodId == -1) m_face[i].lodId = m_nextFaceLodId++;
	}
	m_lodNum = m_vtx.size() - m_nextVertexLodId;
}

void CMeshLod::AddVertex(float x,float y,float z,float nx,float ny,float nz)
{
	SVertex vertex;
	
	// copy data
	vertex.x = x;
	vertex.y = y;
	vertex.z = z;
	vertex.nx = nx;
	vertex.ny = ny;
	vertex.nz = nz;

	// set default value
	vertex.lodId = -1;
	vertex.nextId = -1;
	vertex.faceReduceNum = 0;
	vertex.error = FLT_MAX;
	vertex.bCollapsed = false;
	vertex.bFreezed = false;

	// get source id,only base on postion,ignore texture uv
	vertex.srcId = -1;
	vertex.idx = m_vtx.size();

	/*
	for(int i=0;i<m_vtx.size();i++)
	{
		if(IS_DUP(m_vtx[i].x,x) && IS_DUP(m_vtx[i].y,y) &&
		   IS_DUP(m_vtx[i].z,z))
		{
			vertex.srcId = i;
			m_vtx[i].bFreezed = true;
			vertex.bFreezed = true;
			break;
		}
	}
	*/

    // push it to list
	m_vtx.push_back(vertex);
}

void CMeshLod::AddFace(int v1,int v2,int v3)
{
	SFace face;
	
	// get the next face id
	int faceId = m_face.size();
	
	// set default value
	face.lodId = -1;
	
	// copy data
	face.vId[0] = v1;
	face.vId[1] = v2;
	face.vId[2] = v3;
	
	// compute face normal
	GetFaceNormal(&m_vtx[v1],&m_vtx[v2],&m_vtx[v3],&face.normal);
	m_face.push_back(face);
		
	// associate face id with all the face vertices
	m_vtx[v1].setFaceId.insert(faceId);
	m_vtx[v2].setFaceId.insert(faceId);
	m_vtx[v3].setFaceId.insert(faceId);

	int src;
	src = v1;
	while(m_vtx[src].srcId != -1)
		src = m_vtx[src].srcId;
	m_vtx[src].setFaceNormal.insert(faceId);

	src = v2;
	while(m_vtx[src].srcId != -1)
		src = m_vtx[src].srcId;
	m_vtx[src].setFaceNormal.insert(faceId);

	src = v3;
	while(m_vtx[src].srcId != -1)
		src = m_vtx[src].srcId;
	m_vtx[src].setFaceNormal.insert(faceId);

	// associate neighbour vertices
	m_vtx[v1].setNeighbourId.insert(v2);
	m_vtx[v1].setNeighbourId.insert(v3);
	m_vtx[v2].setNeighbourId.insert(v1);
	m_vtx[v2].setNeighbourId.insert(v3);
	m_vtx[v3].setNeighbourId.insert(v1);
	m_vtx[v3].setNeighbourId.insert(v2);
}

int CMeshLod::GetVertexLodId(int vId)
{
	return m_vtx[vId].lodId;
}

int CMeshLod::GetFaceLodId(int fId)
{
	return m_face[fId].lodId;
}

int CMeshLod::GetLodCount()
{
	return m_lodNum;
}

int CMeshLod::GetVertexReplacerId(int vId)
{
	return m_vtx[vId].nextId;
}

int CMeshLod::GetVertexFaceCollapseNum(int vId)
{
	return m_vtx[vId].faceReduceNum;
}

// calculate the normal of the face
void CMeshLod::GetFaceNormal(SVertex *v1,SVertex *v2,SVertex *v3,SVertex *normal)
{
	float x1, y1, z1;
	x1 = v2->x - v1->x;
	y1 = v2->y - v1->y;
	z1 = v2->z - v1->z;

	float x2, y2, z2;
	x2 = v3->x - v1->x;
	y2 = v3->y - v1->y;
	z2 = v3->z - v1->z;

	float nx, ny, nz;
	nx = y1 * z2 - y2 * z1;
	ny = z1 * x2 - z2 * x1;
	nz = x1 * y2 - x2 * y1;


	float length;
	length = sqrt(nx * nx + ny * ny + nz * nz);
	if(fabs(length) > 0.001)
	{
		nx /= length;
		ny /= length;
		nz /= length;
	}
	normal->x = nx;
	normal->y = ny;
	normal->z = nz;
}

// calculate the area of the face
float CMeshLod::GetFaceArea(SVertex *v1,SVertex *v2,SVertex *v3)
{
	float x1, y1, z1;
	x1 = v2->x - v1->x;
	y1 = v2->y - v1->y;
	z1 = v2->z - v1->z;

	float x2, y2, z2;
	x2 = v3->x - v1->x;
	y2 = v3->y - v1->y;
	z2 = v3->z - v1->z;

	float nx, ny, nz;
	nx = y1 * z2 - y2 * z1;
	ny = z1 * x2 - z2 * x1;
	nz = x1 * y2 - x2 * y1;

	// get the length of the normal
	float length;
	length = sqrt(nx * nx + ny * ny + nz * nz);

	// calculate the face area
	float area;
	area = 0.5f * length;
	return area;
}

void CMeshLod::CalculateAllQuadrics()
{
	int i;
	SFace *face;
	SVertex normal;
	float area;
	float nx,ny,nz;
	
	for(i = 0; i < m_face.size(); i++)
	{
		face = &m_face[i];
		area = GetFaceArea(&m_vtx[face->vId[0]],&m_vtx[face->vId[1]],&m_vtx[face->vId[2]]);
		GetFaceNormal(&m_vtx[face->vId[0]],&m_vtx[face->vId[1]],&m_vtx[face->vId[2]],&normal);
		
		nx = normal.x;
		ny = normal.y;
		nz = normal.z;

		// check cos value -90 -> 90
		if(nx*m_vtx[face->vId[0]].nx + ny*m_vtx[face->vId[1]].ny + nz*m_vtx[face->vId[2]].nz < 0.0f)
		{
			nx = -nx;
			ny = -ny;
			nz = -nz;
		}
		
		CQuadric q(nx, ny, nz, -(m_vtx[face->vId[0]].x * nx + 
			m_vtx[face->vId[0]].y * ny + m_vtx[face->vId[0]].z * nz), area);
		
		q.Scale(area);
		m_vtx[face->vId[0]].quadric.Add(q);
		m_vtx[face->vId[1]].quadric.Add(q);
		m_vtx[face->vId[2]].quadric.Add(q);
	}
}

void CMeshLod::CalculateVertexCollapse(int vId)
{
	SVertex *v = &m_vtx[vId];

	v->nextId  = -1;
	v->error   = FLT_MAX;

	if(v->setNeighbourId.size() == 0)
	{
		// eliminated it 
		v->error = -1.0f;
		return;
	}

	float error;
	set<int>::iterator it;
	for(it = v->setNeighbourId.begin(); it != v->setNeighbourId.end(); it++)
	{
		error = CalculateCollapseError(vId, *it);
		// get the best replacer id
		if(error < v->error)
		{
			v->error = error;
			v->nextId = *it;
		}
	}
}

float CMeshLod::CalculateCollapseError(int sId, int tId)
{
	SVertex *sv = &m_vtx[sId];
	SVertex *tv = &m_vtx[tId];
	
	/*
	// get the target's source vertex
	if(m_vtx[tId].srcId == -1)
	{
		tv = &m_vtx[m_vtx[tId].srcId];
	}
	*/

	float error = 0,area1 = 0,area2 = 0;
	SVertex n1,n2;
	SFace *face;
	set<int>::iterator it;
	for(it = sv->setFaceId.begin(); it != sv->setFaceId.end(); it++)
	{
		face = &m_face[*it];
		
		if(((face->vId[0] == tId) || (face->vId[1] == tId) || (face->vId[2] == tId))
			&& (face->vId[0] != sId) && (face->vId[1] != sId) && (face->vId[2] != sId))
		{
			int v1, v2, v3;
			v1 = face->vId[0];
			v2 = face->vId[1];
			v3 = face->vId[2];
			
			GetFaceNormal(&m_vtx[v1],&m_vtx[v2],&m_vtx[v3],&n1);
			area1 += GetFaceArea(&m_vtx[v1],&m_vtx[v2],&m_vtx[v3]);
		
			// replace old vertex
			if(v1 == sId) v1 = tId;
			if(v2 == sId) v2 = tId;
			if(v3 == sId) v3 = tId;

			GetFaceNormal(&m_vtx[v1],&m_vtx[v2],&m_vtx[v3],&n2);
			area2 = GetFaceArea(&m_vtx[v1],&m_vtx[v2],&m_vtx[v3]);
			
			// check if face flip
			if(n1.x * n2.x + n1.y * n2.y + n1.z * n2.z < 0.0f) return FLT_MAX / 2  ;
			
			error += (1 - n1.x * n2.x + n1.y * n2.y + n1.z * n2.z); 
		}
	}

	// quadric error computation
	CQuadric q(sv->quadric);
	q.Add(tv->quadric);
	return q.Evaluate(tv->x, tv->y, tv->z) / q.GetArea();
}

void CMeshLod::CollapseVertex(int vId)
{
	SVertex *v = &m_vtx[vId];
	
	v->lodId = m_nextVertexLodId--;
	v->bCollapsed = true;

	/*
	// record replace histroy
 	if(v->nextId != -1)
	{
		if(v->srcId != -1)
		{
			m_vtx[v->nextId].srcId = v->srcId;
		}
		else
		{
			m_vtx[v->nextId].srcId = vId;
		}
	}
	*/
	
	set<int> setModifiedVertexId;
	if(v->nextId != -1)
		v->quadric.Add(m_vtx[v->nextId].quadric);

	SFace *face;
	// delete all faces with the collapsing edge
	set<int>::iterator it;
	for(it = v->setFaceId.begin(); it != v->setFaceId.end(); )
	{
		face = &m_face[*it];

		// check if the face contains the collapsing edge
		if((face->vId[0]==v->nextId)||(face->vId[1]==v->nextId)||(face->vId[2]==v->nextId))
		{
			int faceId;
			faceId = *it;

			// erase the face from all the face vertices
			m_vtx[face->vId[0]].setFaceId.erase(faceId);
			setModifiedVertexId.insert(face->vId[0]);

			m_vtx[face->vId[1]].setFaceId.erase(faceId);
			setModifiedVertexId.insert(face->vId[1]);

			m_vtx[face->vId[2]].setFaceId.erase(faceId);
			setModifiedVertexId.insert(face->vId[2]);

			// set the face lod id
			m_face[faceId].lodId = m_nextFaceLodId--;
			v->faceReduceNum++;
			
			// start checking from the beginning
			it = v->setFaceId.begin();
		}
		else
		{
			 it++;
		}
	}

	// update all remaining faces with the new collapsed vertex
	for(it = v->setFaceId.begin(); it != v->setFaceId.end(); it++)
	{
		face = &m_face[*it];
		if(face->vId[0] == vId) face->vId[0] = v->nextId;
		if(face->vId[1] == vId) face->vId[1] = v->nextId;
		if(face->vId[2] == vId) face->vId[2] = v->nextId;
		m_vtx[v->nextId].setFaceId.insert(*it);
	}
	v->setFaceId.clear();

	// remove the vertex from all its neighbours
	for(it = v->setNeighbourId.begin(); it != v->setNeighbourId.end(); it++)
	{
		if(m_vtx[*it].setNeighbourId.erase(vId) > 0)
		{
			// insert the next vertex into the neighbour set
			if(*it != v->nextId)
			{
				m_vtx[*it].setNeighbourId.insert(v->nextId);
				m_vtx[v->nextId].setNeighbourId.insert(*it);
				setModifiedVertexId.insert(*it);
			}
		}
	}
	v->setNeighbourId.clear();

	// do not recalculate edge collapse on collapsed vertex 
	setModifiedVertexId.erase(vId);

	// do recalculate edge collapse on the new collapsed vertex 
	if(v->nextId != -1) setModifiedVertexId.insert(v->nextId);

	// recalculate the best edge collapse for all the modified vertices
	for(it = setModifiedVertexId.begin(); it != setModifiedVertexId.end(); it++)
	{
		// calculate the new vertex collapse
		CalculateVertexCollapse(*it);
	}
}

void CMeshLod::FreezeBorders()
{
	bool flip = true;

	int i,faceNum,cnt=0;
	SVertex *v,*vn;
	SFace *face;
	int total = m_vtx.size(),nCnt,fCnt;
	for(i = 0; i < total ; i++)
	{
		v = &m_vtx[i];
		
		// if(v->setNeighbourId.size() < 2) continue;
		
		set<int>::iterator it;
		nCnt = v->setNeighbourId.size();
		for(it = v->setNeighbourId.begin(); it != v->setNeighbourId.end(); it++)
		{
			vn = &m_vtx[*it];
			faceNum = 0;
			
			// loop through all the vertex faces
			set<int>::iterator fit;
			fCnt = v->setFaceId.size();
			for(fit = v->setFaceId.begin(); fit != v->setFaceId.end(); fit++)
			{
				face = &m_face[*fit];
				if((face->vId[0]==*it)||(face->vId[1]==*it)||(face->vId[2]==*it)) faceNum++;
			}

			// border,freeze it 
			if(faceNum < 2)
			{
				cnt++;
				v->bFreezed = flip;
				// flip = !flip;
				break;
			}
		}
	}
	return;
}

int CMeshLod::GetNextCollapsingVertexId()
{
	int nextId = -1;
	float error = FLT_MAX;

	int i;
	for(i = 0; i < m_vtx.size(); i++)
	{
		if(!m_vtx[i].bCollapsed && !m_vtx[i].bFreezed && (m_vtx[i].error < error))
		{
			nextId = i;
			error = m_vtx[i].error;
		}
	}
	return nextId;
}

void CMeshLod::CalculateVertexNormal()
{
	SFace *face;

	for(int i=0;i<m_vtx.size();i++)
	{
		SVertex *v;
		m_vtx[i].nx = 0;
		m_vtx[i].ny = 0;
		m_vtx[i].nz = 0;
		
		v = &m_vtx[i];
		while(v->srcId != -1)
			v = &m_vtx[v->srcId];

		set<int>::iterator it;
		// int tmp = v->setFaceNormal.size();
		for(it = v->setFaceNormal.begin(); it != v->setFaceNormal.end();it++ )
		{
			face = &m_face[*it];
			m_vtx[i].nx += face->normal.x;
			m_vtx[i].ny += face->normal.y;
			m_vtx[i].nz += face->normal.z;
		}
		
		float length;
		length = sqrt(m_vtx[i].nx * m_vtx[i].nx + m_vtx[i].ny * m_vtx[i].ny
			+ m_vtx[i].nz * m_vtx[i].nz);
			
		if(fabs(length) < 0.001) 
			continue;
			
		m_vtx[i].nx /= length;
		m_vtx[i].ny /= length;
		m_vtx[i].nz /= length;
	}
}

CMeshLod::SVertex *CMeshLod::GetVertexInfo(int vId)
{
	return &m_vtx[vId];
}

void CMeshLod::Reserve(int vCnt,int fCnt)
{
	m_vtx.reserve(vCnt);
	m_duplicate.reserve(vCnt);
	m_face.reserve(fCnt);
}

bool comp(const CMeshLod::SVertex &v1,const CMeshLod::SVertex &v2)
{
	return (v1.x < v2.x) ? true : false;
}

void CMeshLod::MarkDuplicate()
{
	if(m_vtx.size() <= 1) return;
	m_duplicate = m_vtx;
	stable_sort(m_duplicate.begin(),m_duplicate.end(),comp);
	vector<SVertex>::iterator it,itbeg,itprev;

	int cnt=0;
	itbeg  = m_duplicate.begin();
	itprev = m_duplicate.begin();
	for(it=m_duplicate.begin()+1;it!=m_duplicate.end();it++)
	{
		if(IS_DUP((*it).x,(*itprev).x))
		{
			cnt++;
		}
		else
		{
			if(cnt > 0)
			{
				RealMarkDuplicate(itbeg,it);
				itbeg = it;
			}
			cnt = 0;
		}
		itprev = it;
	}
	if(cnt > 0)
		RealMarkDuplicate(itbeg,it);
}

void CMeshLod::RealMarkDuplicate(vector<SVertex>::iterator beg,vector<SVertex>::iterator end)
{
	vector<SVertex>::iterator it,it1;
	for(it=beg;it!=end;it++)
	{
		for(it1=beg;it1!=it;it1++)
		{
			if(IS_DUP((*it).x,(*it1).x) && IS_DUP((*it).y,(*it1).y) && 
			   IS_DUP((*it).z,(*it1).z) && ((*it1).idx != (*it).idx))
			{
				m_vtx[(*it).idx].srcId = (*it1).idx;
				m_vtx[(*it).idx].bFreezed = true;
				m_vtx[(*it1).idx].bFreezed = true;
				break;
			}
		}
	}
}
