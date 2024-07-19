#ifndef __ENUM_PROC_H__
#define __ENUM_PROC_H__

#include <vector>
#include <max.h>
#include <modstack.h>
#include <iparamb2.h>
#include <iskin.h>
#include <stdmat.h>
using namespace std;

#define MAX_STR_LEN		512
#define NODETYPE_DUMMY	BYTE(0x01)				// Node is a dummy
#define NODETYPE_BONE	BYTE(0x02)				// Node is a bone

inline void FilterSpace(char* str)
{
	char *p = str;
	while(*p)
	{
		if(*p == ' ') *p = '_';
		p++;
	}
}

inline bool IsBone(INode *node)
{
	char *p = node->GetName();
	return ((p[0] == 'b') || (p[0] == 'B'));
}

inline bool IsDummy(INode *node)
{
	char *p = node->GetName();
	return ((p[0] == 'd') || (p[0] == 'D'));
}

struct TNode
{
	TNode() { ZeroMemory(this, sizeof(TNode)); }
	Point3	Pivot;									// pivot
	char	szName[MAX_STR_LEN];					// name
	char	szParent[MAX_STR_LEN];					// parent name
	int		nChildCount;							// num of children
	BYTE	btType;									// node type
};

typedef std::vector<TNode>	TNodeArray;
inline int GetNodeIndex(char name[], const TNodeArray& NodeArray)
{
	int i;
	for(i=0; i<NodeArray.size(); i++)
	{
		if(strcmp(NodeArray[i].szName, name) == 0)
			return i;
	}
	return -1;
}

struct TFrameData
{
	float	data[12];								// data
};

typedef std::vector<TFrameData>		TFrame;
// dump bone animation
class DumpFrame : public ITreeEnumProc
{
public:
	void			SetTimeToDump(TimeValue t);
	const TFrame&	GetBoneFrame(){ return m_tBoneFrame; }
	const TFrame&	GetDummyFrame(){ return m_tDummyFrame; }
	const float *	GetBounds(){ return m_bounds;}
	DumpFrame(bool bSel = false);
	~DumpFrame();

private:
	virtual int				callback(INode *node);
	TimeValue				m_tTime;
	TFrame					m_tBoneFrame;
	TFrame					m_tDummyFrame;
	float					m_bounds[6];	// min and max xyz
	bool					m_bDumpSel;
};

class DumpNodes : public ITreeEnumProc
{
public:
	DumpNodes(bool bSel = false);
	void init(FILE* pFile);
	virtual ~DumpNodes();
	const TNodeArray& GetBones(){ return m_bones;}
	const TNodeArray& GetDummies(){ return m_dummies;}
private:
	TNodeArray				m_bones;
	TNodeArray				m_dummies;
	virtual int				callback(INode *node);
	FILE					*m_pFile;
	bool					m_bDumpSelected;
};

struct TWeightInfo
{
	char	bone[MAX_STR_LEN];
	float	fWeight;
	bool operator < (const TWeightInfo& info) const
	{
		return fWeight < info.fWeight ? true : false;
	}
};

struct TLodInfo
{
	int lodId;
	int collapseId;
	int faceCollapseCnt;
};

struct TVertex
{
	TVertex() { ZeroMemory(this, sizeof(TVertex)); }
	Point3						vertex;
	Point3						normal;
	UVVert						uv;
	std::vector<TWeightInfo>	tWeightArray;
	TLodInfo					lodInfo;

	void PushWeight(TWeightInfo wi)
	{
		if(wi.fWeight <= 0.001f)
			return;
		for(int i =0; i < tWeightArray.size(); i++)
		{
			if(strcmp(wi.bone, tWeightArray[i].bone) == 0)
			{
				tWeightArray[i].fWeight += wi.fWeight;
				return;
			}
		}
		tWeightArray.push_back(wi);
	}
};

struct TMaterial
{
	int		MatIndex;
	TMaterial() { ZeroMemory(this, sizeof(TMaterial)); }
	char	TexName[MAX_STR_LEN];
	BMM_Color_64   ColorKey;
	Color	Ambient;
	Color	Diffuse;
	Color	Specular;
	Color	Emissive;
	float	Opacity;
	char	Option[MAX_STR_LEN];
	BOOL	TwoSide;
	BOOL	bClrKey;
	float   Shinese;
	float   ShineStr;
};

struct TFace
{
	bool operator < (const TFace& face) const
	{
		return (m < face.m) ? true : false;

	}
	int	m;						// Material id
	int a, b, c;				// Vertice  index
	TLodInfo	lodInfo;		// LOD information
};

typedef std::vector<TFace>		TFaceArray;
typedef std::vector<TMaterial>	TMaterialArray;
typedef std::vector<TVertex>	TVertexArray;

struct TSkin
{
	char			name[MAX_STR_LEN];
	TVertexArray	tVertexArray;
	TMaterialArray	tMaterialArray;
	TFaceArray		tFaceArray;
};

typedef std::vector<TSkin>		TSkinArray;
class DumpSkin : public ITreeEnumProc
{
public:
	void SortForLod();
	DumpSkin(bool bSel = false,int time = 0);
	~DumpSkin();
	const TSkinArray& GetSkinArray(){ return m_SkinArray; }

private:
	void GetTexFilename(TMaterial& m, StdMat* pMtl);
	void GetMtl(Mtl* pMtl, TSkin& skin);
	void GetMtlInfo(StdMat2* src,TMaterial *tar);
	int PushVertex(const TVertex& ver, TSkin& skin);
	Modifier* FindPhysiqueModifier(INode* node);
	void AddBoneWeight(TVertex& vert, DWORD iVert, INode* node);
	virtual int				callback(INode *node);
	Modifier*				m_pModifier;
	TSkinArray				m_SkinArray;
	bool					m_bDumpSelected;
	int						m_time;
};

#endif 
