#include <max.h>
#include "meshlod.h"
#include "EnumProc.h"
#include <phyexp.h>
#include <bmmlib.h>

DumpNodes::DumpNodes(bool bSel)
{
	m_bDumpSelected = bSel;
}

void DumpNodes::init(FILE* pFile)
{
	m_pFile = pFile;
}

DumpNodes::~DumpNodes()
{

}

int DumpNodes::callback(INode *node)
{
	Matrix3 parentTM, nodeTM, localTM;
	if(m_bDumpSelected)
		if(!node->Selected())
			return TREE_CONTINUE;
	TNode	bone;
	if(node->IsNodeHidden()) return TREE_CONTINUE;
	INode *parent = node->GetParentNode();
 	if( IsBone(node))
		bone.btType = NODETYPE_BONE;
	else if(IsDummy(node))
		bone.btType = NODETYPE_DUMMY;
	else
		return TREE_CONTINUE;

	strcpy(bone.szName, node->GetName());
	FilterSpace(bone.szName);
	if(parent != NULL) 
	{
		if (!strcmp(parent->GetName(), "Scene Root")) 
		{
			strcpy(bone.szParent,"NULL");
		}
		else 
		{
			strcpy(bone.szParent, parent->GetName());
		}
		nodeTM = node->GetNodeTM(0);
		parentTM = parent->GetNodeTM(0);
		localTM = nodeTM * Inverse(parentTM);
	}
	else 
	{
		strcpy(bone.szParent,"NULL");
		nodeTM = node->GetNodeTM(0);
		localTM = node->GetNodeTM(0);
	}

	FilterSpace(bone.szParent);
	Point3 row = localTM.GetRow(3);
	bone.Pivot.x = row.x;
	bone.Pivot.y = row.y;
	bone.Pivot.z = row.z;
	if(bone.btType == NODETYPE_DUMMY)
	{
		m_dummies.push_back(bone);
	}
	else if(bone.btType == NODETYPE_BONE)
	{
		int nIndex = GetNodeIndex(bone.szParent, m_bones);
		if(nIndex >=0)
			m_bones[nIndex].nChildCount ++;
		m_bones.push_back(bone);
	}
	else
		return TREE_CONTINUE;

	return TREE_CONTINUE;
}

int DumpFrame::callback(INode *node)
{
	Matrix3 parentTM, nodeTM, localTM;
	if(m_bDumpSel)
		if(!node->Selected())
			return TREE_CONTINUE;
	if(!IsBone(node) && !IsDummy(node)) return TREE_CONTINUE;
	TNode	bone;
	if(node->IsNodeHidden()) return TREE_CONTINUE;
	INode *parent = node->GetParentNode();
	TFrameData frame;

	// Get Node TM
	if(parent != NULL) 
	{
		nodeTM   = node->GetNodeTM(m_tTime);
		parentTM = parent->GetNodeTM(m_tTime);
		localTM  = nodeTM * Inverse(parentTM);
	}
	else 
	{
		nodeTM  = node->GetNodeTM(m_tTime);
		localTM = node->GetNodeTM(m_tTime);
	}
	// get bounds
	Point3 bound = nodeTM.GetRow(3);
	if(m_bounds[0] > bound.x) m_bounds[0] = bound.x; 
	if(m_bounds[3] < bound.x) m_bounds[3] = bound.x; 
	if(m_bounds[1] > bound.y) m_bounds[1] = bound.y; 
	if(m_bounds[4] < bound.y) m_bounds[4] = bound.y; 
	if(m_bounds[2] > bound.z) m_bounds[2] = bound.z; 
	if(m_bounds[5] < bound.z) m_bounds[5] = bound.z; 

	Quat quat(localTM);
	Point3 row = localTM.GetRow(3);

	if(IsDummy(node))
	{
		frame.data[0] = row.x;
		frame.data[1] = row.y;
		frame.data[2] = row.z;
		frame.data[3] = -quat.w;
		frame.data[4] = quat.x;
		frame.data[5] = quat.y;
		frame.data[6] = quat.z;
		m_tDummyFrame.push_back(frame);
	}
	else
	{
		if(parent == NULL)
		{
			frame.data[0] = row.x;
			frame.data[1] = row.y;
			frame.data[2] = row.z;
			frame.data[3] = -quat.w;
			frame.data[4] = quat.x;
			frame.data[5] = quat.y;
			frame.data[6] = quat.z;
		}
		else
		{
			if (!strcmp(parent->GetName(), "Scene Root"))
			{
				frame.data[0] = row.x;
				frame.data[1] = row.y;
				frame.data[2] = row.z;
				frame.data[3] = -quat.w;
				frame.data[4] = quat.x;
				frame.data[5] = quat.y;
				frame.data[6] = quat.z;
			}
			else
			{
				frame.data[0] = row.x;
				frame.data[1] = row.y;
				frame.data[2] = row.z;
				frame.data[3] = -quat.w;
				frame.data[4] = quat.x;
				frame.data[5] = quat.y;
				frame.data[6] = quat.z;
			}
		}
		m_tBoneFrame.push_back(frame);
	}
	return TREE_CONTINUE;
}

DumpFrame::DumpFrame(bool bSel)
{
	m_tTime = 0;
	m_bDumpSel = bSel;
	for(int i=0;i<6;i++)
		m_bounds[i] = 0.0f;
}

DumpFrame::~DumpFrame()
{

}

void DumpFrame::SetTimeToDump(TimeValue t)
{
	m_tTime = t;
}

DumpSkin::DumpSkin(bool bSel,int time)
{
	m_bDumpSelected = bSel;
	m_time = time;
}

DumpSkin::~DumpSkin()
{

}

int DumpSkin::callback(INode *node)
{
	Matrix3 nodeTM;
	if(m_bDumpSelected)
		if(!node->Selected())
			return TREE_CONTINUE;
	TNode	bone;
	if(node->IsNodeHidden()) return TREE_CONTINUE;

	if(IsBone(node)) return TREE_CONTINUE;
	if(IsDummy(node)) return TREE_CONTINUE;

	INode *parent = node->GetParentNode();

	ObjectState os = node->EvalWorldState(m_time);

	nodeTM = node->GetObjectTM(m_time);
	Matrix3 normalTM = nodeTM;
	normalTM.SetRow(3, Point3(0,0,0));

	TSkin	skin;
	strcpy(skin.name, node->GetName());
	Object*	pobj = os.obj;
	TriObject *ptriobj;
	BOOL fConvertedToTriObject = 
		pobj->CanConvertToType(triObjectClassID) &&
		(ptriobj = (TriObject*)pobj->ConvertToType(0, triObjectClassID)) != NULL;

	if (!fConvertedToTriObject)
		return TREE_CONTINUE;

	Mesh *pmesh = &ptriobj->mesh;
	// pmesh->buildNormals();
	pmesh->buildRenderNormals();

	Mtl *mtl = node->GetMtl();
	GetMtl(mtl,skin);
	m_pModifier = FindPhysiqueModifier(node);
	
	int cFaces = pmesh->getNumFaces();
	DWORD iVert;
	DWORD iTVert;
	for (int iFace = 0; iFace < cFaces; iFace++)
	{
		Face*	pface		= &pmesh->faces[iFace];
		TVFace*	ptvface = 0;
		if(pmesh->getNumTVerts() != 0)
			ptvface		= &pmesh->tvFace[iFace];
		
		TFace	face;
		int FaceVertexIndex[3];
		for(int iv = 0; iv < 3; ++iv)
		{
			TVertex	vertex;
			iVert	= pface->getVert(iv);
			vertex.vertex	= pmesh->getVert(iVert) * nodeTM;
			vertex.normal	= pmesh->getNormal(iVert) * normalTM;
			vertex.uv.x = vertex.uv.y = 0.0f;
			if(mtl)
			{
				if(ptvface)
				{
					iTVert	= ptvface->getTVert(iv);
					if(iTVert >=0)
						vertex.uv = pmesh->getTVert(iTVert);
				}
			}
			AddBoneWeight(vertex, iVert, node);
			FaceVertexIndex [iv] = PushVertex(vertex, skin);
		}
		face.a = FaceVertexIndex[0];
		face.b = FaceVertexIndex[1];
		face.c = FaceVertexIndex[2];
		if (mtl && mtl->NumSubMtls()) 
		{
			face.m = pmesh->getFaceMtlIndex(iFace);
			if (face.m>= mtl->NumSubMtls())
				face.m = 0;
		}
		else
		{
			face.m = 0;
		}
		skin.tFaceArray.push_back(face);
	}	
	std::sort(skin.tFaceArray.begin(), skin.tFaceArray.end());
	m_SkinArray.push_back(skin);
	return TREE_CONTINUE;
}

Modifier* DumpSkin::FindPhysiqueModifier(INode *node)
{
	Object *pObject;
	pObject = node->GetObjectRef();
	if(pObject == 0) return 0;
	while(pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject *pDerivedObject;
		pDerivedObject = static_cast<IDerivedObject *>(pObject);

		int stackId;
		for(stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++)
		{
			Modifier *pModifier;
			pModifier = pDerivedObject->GetModifier(stackId);
			if(pModifier->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
				return pModifier;
		}
		pObject = pDerivedObject->GetObjRef();
	}
	return 0;
}

void DumpSkin::AddBoneWeight(TVertex& vert, DWORD iVert, INode* node)
{
	if(m_pModifier == 0)
		return;
	IPhysiqueExport *pPhysiqueExport;
	pPhysiqueExport = (IPhysiqueExport *)m_pModifier->GetInterface(I_PHYINTERFACE);
	if(pPhysiqueExport == 0)
		return;		
	
	IPhyContextExport *pContextExport;
	pContextExport = (IPhyContextExport *)pPhysiqueExport->GetContextInterface(node);
	if(pContextExport == 0)
		return;	
	pContextExport->ConvertToRigid(TRUE);
	pContextExport->AllowBlending(TRUE);
	
	IPhyVertexExport *pVertexExport;
	pVertexExport = (IPhyVertexExport *)pContextExport->GetVertexInterface(iVert);
	if(pVertexExport == 0)
		return;
	
	int vertexType = pVertexExport->GetVertexType();
	if(vertexType == RIGID_TYPE)
	{
		TWeightInfo info;
		IPhyRigidVertex *pTypeVertex;
		pTypeVertex = (IPhyRigidVertex *)pVertexExport;

		strcpy(info.bone, (pTypeVertex->GetNode())->GetName());
		info.fWeight = 1.0f;
		FilterSpace(info.bone);
		vert.PushWeight(info);
		
	}
	else if(vertexType == RIGID_BLENDED_TYPE)
	{
		TWeightInfo info;
		IPhyBlendedRigidVertex *pTypeVertex;
		pTypeVertex = (IPhyBlendedRigidVertex *)pVertexExport;
		
		int nodeId;
		int nNodes = pTypeVertex->GetNumberNodes(); 
		for(nodeId = 0; nodeId < nNodes; nodeId++)
		{
			strcpy(info.bone, (pTypeVertex->GetNode(nodeId))->GetName());
			info.fWeight = pTypeVertex->GetWeight(nodeId);
			FilterSpace(info.bone);
			vert.PushWeight(info);
		}
	}
	std::sort(vert.tWeightArray.begin(),vert.tWeightArray.end());
	std::reverse(vert.tWeightArray.begin(),vert.tWeightArray.end()); 
}

int DumpSkin::PushVertex(const TVertex& ver, TSkin& skin)
{
	for(int i =0; i < skin.tVertexArray.size(); ++i)
	{
		if(ver.vertex == skin.tVertexArray[i].vertex && 
			ver.uv == skin.tVertexArray[i].uv)
			return i;
	}
	skin.tVertexArray.push_back(ver);
	return i;
}

void DumpSkin::GetMtlInfo(StdMat2* src,TMaterial *tar)
{
	GetTexFilename(*tar,src);
	tar->Ambient	= src->GetAmbient(0);
	tar->Diffuse	= src->GetDiffuse(0);
	tar->Specular	= src->GetSpecular(0);
	tar->TwoSide	= src->GetTwoSided();
	tar->Opacity	= src->GetOpacity(0);
	tar->Emissive	= src->GetSelfIllumColor(0);
	tar->Shinese	= src->GetShininess(0);
	tar->ShineStr	= src->GetShinStr(0);
			
	int tran = src->GetTransparencyType();
	switch(tran)
	{
		case 0:	
			strcpy(tar->Option,"SUB");
			break;

		case 1:
			strcpy(tar->Option,"ADD");
			break;

		case 2:
			strcpy(tar->Option,"ALPHA");
			break;
				
		default:
			strcpy(tar->Option,"NONE");
			break;
	}
}
void DumpSkin::GetMtl(Mtl *pMtl, TSkin& skin)
{
	TMaterial material;
	material.TexName[0] = 0;
	material.Ambient =Color(1.0f,1.0f,1.0f);
	material.Diffuse =Color(1.0f,1.0f,1.0f);
	material.Specular = Color(0.0f,0.0f,0.0f);

	if (pMtl == NULL) 
		return;
	if (pMtl->NumSubMtls()) 
	{
		StdMat2 *sub;
		for (int i = 0; i < pMtl->NumSubMtls(); i++) 
		{
			sub = (StdMat2*)pMtl->GetSubMtl(i);
			if(sub == 0)
				continue;
			GetMtlInfo(sub,&material);
			skin.tMaterialArray.push_back(material);
		}
	}
	else
	{
		StdMat2* stdMtl = (StdMat2*)pMtl;
		GetMtlInfo(stdMtl,&material);
		skin.tMaterialArray.push_back(material);
	}
	return;
}

void DumpSkin::GetTexFilename(TMaterial &m, StdMat* pMtl)
{
	if(pMtl == 0)
		return;
	for(int it = 0; it < pMtl->NumSubTexmaps(); ++it)
	{
		Texmap *tex = pMtl->GetSubTexmap(it);
		if (tex && pMtl->MapEnabled(it)) 
		{
			if(tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
			{
				BitmapTex *pbtex = (BitmapTex*)tex;
				char buf[MAX_STR_LEN], name[MAX_STR_LEN];
				sprintf(buf, pbtex->GetMapName());
				_splitpath(buf, NULL, NULL, name, NULL);
				strcpy(m.TexName, name);

				BMM_Color_64 color;
				m.bClrKey = (name[0] == 'k');
				Bitmap* bm = pbtex->GetBitmap(0);
				
				if(!bm)
				{
				}
				else
				{
					bm->GetPixels(0,0,1,&color);
					m.ColorKey = color;
				}
				break;
			}
		}
	}
}

void DumpSkin::SortForLod()
{
	for(int iskin = 0; iskin < m_SkinArray.size(); ++iskin)
	{
		TSkin				Skin = m_SkinArray[iskin];
		TVertexArray		TmpVArray = Skin.tVertexArray;
		TFaceArray			TmpFArray = Skin.tFaceArray;

		int i = 0;
		// create lodder 
		CMeshLod lodder;
		
		// add vertices 
		for(i = 0;i < TmpVArray.size();i++)
		{
			lodder.AddVertex(TmpVArray[i].vertex.x,
				TmpVArray[i].vertex.y,
				TmpVArray[i].vertex.z,
				TmpVArray[i].normal.x,
				TmpVArray[i].normal.y,
				TmpVArray[i].normal.z);
		}
		lodder.MarkDuplicate();
		// add faces 
		for(i = 0; i < TmpFArray.size(); i++)
		{
			lodder.AddFace(TmpFArray[i].a,TmpFArray[i].b,TmpFArray[i].c);
		}

		// calculate LOD levels 
		lodder.CalculateLod();
		// lodder.CalculateVertexNormal();

		// set the LOD id of all vertices
		for(i = 0; i < TmpVArray.size();i++)	
		{
			int lodId = lodder.GetVertexLodId(i);
			
			// set the LOD id of the vertex candidates
			Skin.tVertexArray[lodId].lodInfo.lodId = lodId;
			// set the collapse id of the vertex candidates
			Skin.tVertexArray[lodId].lodInfo.collapseId = lodder.GetVertexReplacerId(i);
			// set the face collapse count of the vertex candidates
			Skin.tVertexArray[lodId].lodInfo.faceCollapseCnt = lodder.GetVertexFaceCollapseNum(i);
			
			Skin.tVertexArray[lodId].vertex = TmpVArray[i].vertex;
			CMeshLod::SVertex *v = lodder.GetVertexInfo(i);
			Skin.tVertexArray[lodId].normal = Point3(v->nx, v->ny, v->nz);
			// Skin.tVertexArray[lodId].normal = TmpVArray[i].normal;
			Skin.tVertexArray[lodId].tWeightArray = TmpVArray[i].tWeightArray;
			Skin.tVertexArray[lodId].uv = TmpVArray[i].uv;

			/*
			CMeshLod::SVertex *v = lodder.GetVertexInfo(i);
			Skin.tVertexArray[i].normal = Point3(v->nx, v->ny, v->nz);
			*/
		}
		
		// set the LOD id of all face vertices
		for(i = 0 ; i < TmpFArray.size(); i++)
		{
			int lodId = lodder.GetFaceLodId(i);
			// get the face
			TFace *face = &TmpFArray[i];
			face->lodInfo.lodId = lodId;
			
			// set the LOD id of the face vertices
			face->a = lodder.GetVertexLodId(face->a);
			face->b = lodder.GetVertexLodId(face->b);
			face->c = lodder.GetVertexLodId(face->c);

			Skin.tFaceArray[lodId] = *face; 
		}
		m_SkinArray[iskin] = Skin;
	}
}