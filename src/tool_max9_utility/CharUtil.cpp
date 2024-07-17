//
// FILE: CharUtil.cpp
//

#include "CharUtil.h"
// #define DO_MEMCHECK 0
#include <max9/modstack.h>
#include <max9/iparamb2.h>
#include <max9/iskin.h>
#include <max9/stdmat.h>
#include <max9/Notetrck.h>
#include <max9/tvnode.h>
#include "MsgWnd.h"

#include "character/actor.h"
#include "rt1tort2.h"
#include <max9/CS/phyexp.h>
#include <max9/CS/bipexp.h>
#include <string>
using namespace std;

#define CHARUTIL_CLASS_ID	Class_ID(0x50a00423, 0x3568b2d7)
#define MAX_STR_LEN		512
inline bool IsBone(INode *node)
{
	char *p = node->GetName();
	return ((p[0] == 'b') || (p[0] == 'B'));
}

inline bool IsSkin(INode *node)
{
	char *p = node->GetName();
	return ((p[0] == 's') || (p[0] == 'S'));
}

void ScaleBiped(INode* node, int scale)
{
	if (node->IsRootNode()) return;

	// Use the class ID to check to see if we have a biped node
	Control* c = node->GetTMController();
	if ((c->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) ||
		(c->ClassID() == BIPBODY_CONTROL_CLASS_ID) ||
		(c->ClassID() == FOOTPRINT_CLASS_ID))
	{

		// Get the Biped Export Interface from the controller 
		IBipedExport *BipIface = (IBipedExport *) c->GetInterface(I_BIPINTERFACE);

		// Either remove the non-uniform scale from the biped, 
		// or add it back in depending on the boolean scale value
		BipIface->RemoveNonUniformScale(scale);
		Control* iMaster = (Control *) c->GetInterface(I_MASTER);
		iMaster->NotifyDependents(FOREVER, PART_TM, REFMSG_CHANGE);

		// Release the interfaces
		c->ReleaseInterface(I_MASTER,iMaster);
		c->ReleaseInterface(I_BIPINTERFACE,BipIface);
	}
}

class CharUtil : public UtilityObj 
{
public:
	void SelectAllFrame();
	void SelectOneFrame();
	bool GetFrameInfo();
    bool GetStandMtl(CRT_MtlStandard *stand,StdMat2* pMtl);
	bool GetTexture(string &texture,string &fullpath,string &fullname,StdMat* pMtl);
	bool ExportMaterialLib(CRT_MaterialLib &lib,INode *node);

	// export va skin
	bool ExportVaSkin(CRT_VaSkin &skin,INode *node);
	bool GetVaSkin(INode *node,CRT_VaSkin &va,vector<int> &idx,TimeValue time);
	bool GetVaFrame(INode *node,CRT_VaSkin &va,vector<int> &idx,TimeValue time);

	// export skeletal skin
	// bool ExportSkelSkin();
	bool ExportSkelSkin(CRT_Actor &actor,CRT_SkelSkin &skin,INode *node);
	bool GetSkelSkinNode(CRT_Actor &actor,CRT_SkelSkin &skin,INode *node);

	// export actor
	bool EnumBone(INode *node,CRT_Actor &actor,bool abs,bool translate);
	bool GetBone(INode *node,SRT_Bone &bone,bool abs,bool translate);
	bool EnumBoundingBox(INode *node,TimeValue time,RtgRangeAABB &box);
	bool GetBoundingBox(INode *node,TimeValue time,RtgRangeAABB &box);

	// export actor and skin together
	bool ExportAll(bool skel);
	void EnumAllSkin(bool skel,CRT_Actor *actor,INode *node);
    
	Modifier *FindPhysiqueModifier(INode *node);
	void AddBoneWeight(TimeValue time,CRT_Actor &actor,SRT_DynamicVtx &vert, DWORD iVert, INode* node,Modifier *mod);

	bool GetFileName(string &name,const char *filter);
	HWND			hPanel;
	IUtil			*iu;
	Interface		*ip;
	
	void BeginEditParams(Interface *ip,IUtil *iu);
	void EndEditParams(Interface *ip,IUtil *iu);
	void Init(HWND hWnd);
	void Destroy(HWND hWnd);
	void DeleteThis() { }		

	//Constructor/Destructor
	CharUtil();
	virtual ~CharUtil();
	
private:
	HWND m_hWnd;
	int  m_startFrm;
	int  m_endFrm;
	string m_exportPath;
};

static CharUtil theCharUtil;

class CharUtilClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return &theCharUtil; }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
	Class_ID		ClassID() { return CHARUTIL_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("CharUtil"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle
};

static CharUtilClassDesc CharUtilDesc;
ClassDesc2* GetCharUtilDesc() { return &CharUtilDesc; }

static BOOL CALLBACK CharUtilDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			theCharUtil.Init(hWnd);
            break;

		case WM_DESTROY:
			theCharUtil.Destroy(hWnd);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) 
			{
				case IDC_EXPORT_ALL_VA:
					if(theCharUtil.GetFrameInfo()) 
					{
						try {
							if(!theCharUtil.ExportAll(false))
								MessageBox(hWnd,"输出失败！","Error",MB_OK);
						}
						catch (...) {
						 	MessageBox(hWnd,"发生未知错误！","Error",MB_OK);
						}
					}
					break;

				case IDC_EXPORT_ALL_SKEL:
					if(theCharUtil.GetFrameInfo()) 
					{
						try {
							if(!theCharUtil.ExportAll(true))
								MessageBox(hWnd,"输出失败！","Error",MB_OK);
						}
						catch (...) {
							MessageBox(hWnd,"发生未知错误！","Error",MB_OK);
						}
					}
					break;
				/*
				case IDC_EXPORT_VASKIN:
					if(theCharUtil.GetFrameInfo()) 
					{
						try {
							// theCharUtil.ExportVaSkin();
						}
						catch (...) {
							MessageBox(hWnd,"发生未知错误！","Error",MB_OK);
						}
					}
					break;
				
				case IDC_EXPORT_SKELSKIN:
					if(theCharUtil.GetFrameInfo()) 
					{
						try {
							if(!theCharUtil.ExportSkelSkin())
								MessageBox(hWnd,"输出失败！","Error",MB_OK);
						}
						catch (...) {
							MessageBox(hWnd,"发生未知错误！","Error",MB_OK);
						}
					}
					break;
				*/
				case IDC_ALL_FRAME:
					theCharUtil.SelectAllFrame();
					break;

				case IDC_ONE_FRAME:
					theCharUtil.SelectOneFrame();
					break;

				case IDC_START_FRAME:
				case IDC_END_FRAME:
					switch (HIWORD(wParam)) 
					{
						case EN_SETFOCUS:
							DisableAccelerators();
							break;
						case EN_KILLFOCUS:
							EnableAccelerators();
							break;
					}
					break;
				/*
				case IDC_LOG:
					int ret;
					ret = GetCheckBox(hWnd,IDC_LOG);
					switch(ret) {
						case BST_CHECKED:
							MessageBox(hWnd,"BST_CHECKED!","test",MB_OK);
							break;
						case BST_INDETERMINATE: 
							MessageBox(hWnd,"BST_INDETERMINATE!","test",MB_OK);
							break;
						case BST_UNCHECKED: 
							MessageBox(hWnd,"BST_UNCHECKED!","test",MB_OK);
							break;
					}
					break;
				*/
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			theCharUtil.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return FALSE;
	}
	return TRUE;
}

CharUtil::CharUtil()
{
	iu = NULL;
	ip = NULL;	
	hPanel = NULL;
}

CharUtil::~CharUtil()
{

}

void CharUtil::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = iu;
	this->ip = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		CharUtilDlgProc,
		GetString(IDS_PARAMS),
		0);
}
	
void CharUtil::EndEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = NULL;
	this->ip = NULL;
	ip->DeleteRollupPage(hPanel);
	hPanel = NULL;
}

void CharUtil::Init(HWND hWnd)
{
	m_hWnd = hWnd;
	SetCheckBox(hWnd,IDC_LOG,TRUE);

	// init core lib
	// R [11/26/2008 Louis.Huang]
	//rtCoreInit(NULL,NULL,NULL,NULL,NULL);
	rtCoreInit("clt_engine.ini");
	ActorInit();
}

void CharUtil::Destroy(HWND hWnd)
{

}

bool CharUtil::GetFileName(string &name,const char *filter)
{
	OPENFILENAME ofn;				// common dialog box structure
	char szFile[MAX_STR_LEN];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ZeroMemory(szFile, sizeof(szFile));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);

	// ofn.lpstrFilter = "All(*.*)\0*.*\0Text(*.txt)\0*.txt\0";
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST;

	// Display the Open dialog box. 
	if(GetOpenFileName(&ofn)==TRUE)
	{
		name = ofn.lpstrFile;
		return true;
	}
	else
	{
		return false;
	}
	/*
		hf = CreateFile(ofn.lpstrFile, GENERIC_READ,
		0, (LPSECURITY_ATTRIBUTES) NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		(HANDLE) NULL);
	*/
}

/*
bool CharUtil::ExportSkelSkin()
{
	string name;
	if(!GetFileName(name,"ski(*.ski)\0*.ski\0"))
	{
		MessageBox(m_hWnd,"Get File Name Failed !","Error",MB_OK);
		return false;
	}

	if( name[name.length()-1] != 'i' ||
		name[name.length()-2] != 'k' ||
		name[name.length()-3] != 's' ||
		name[name.length()-4] != '.' )
	{
		name += ".ski";
	}

	int num = ip->GetSelNodeCount();
	if(num <= 0)
	{
		MessageBox(m_hWnd,"Must Select One Skin!","Error",MB_OK);
		return false;
	}
	else if(num != 1)
	{
		MessageBox(m_hWnd,"Only Can Select One Skin!","Error",MB_OK);
		return false; 
	}

	INode *node = ip->GetSelNode(0);
	CRT_SkelSkin skin;
	if(ExportSkelSkin(skin,node))
	{
		RtArchive *ar = g_pFileManager->CreateFileWriter(name.c_str());
		if(!ar) return false;
		long time = timeGetTime();
		if(!ar->WriteObject(&skin)) return false;
		delete ar;
		// skin.Output(*g_pLog);
		return true;
	}
	else
	{
		return false;
	}
}
*/

bool CharUtil::ExportVaSkin(CRT_VaSkin &skin,INode *node)
{
	vector<int> idx;
	if(node->IsRootNode() || node->IsNodeHidden()) 
		return false;
	// get option from user prop
	// skin.m_bNor = (GetCheckBox(m_hWnd,IDC_EXPORT_NOR) == BST_CHECKED);
	// skin.m_bUV  = (GetCheckBox(m_hWnd,IDC_EXPORT_UV) == BST_CHECKED);
	// skin.m_bBoard = (GetCheckBox(m_hWnd,IDC_BILLBOARD) == BST_CHECKED);
	
	BOOL flag;
	skin.m_bNor = false;
	skin.m_bUV  = false;
	skin.m_bBoard = false;
	const char *name = node->GetName();
	if(node->GetUserPropBool("board",flag))
	{
		if(flag) skin.m_bBoard = true;
	}
	if(node->GetUserPropBool("normal",flag))
	{
		if(flag) skin.m_bNor = true;
	}
	if(node->GetUserPropBool("uv",flag))
	{
		if(flag) skin.m_bUV = true;
	}
	if(!GetVaSkin(node,skin,idx,m_startFrm*GetTicksPerFrame())) return false;

	char buf[MAX_STR_LEN];
	// Interval range = ip->GetAnimRange();
	int numFrame = m_endFrm - m_startFrm;
    if(numFrame > 0)
	{
		for(int nIndex = m_startFrm; nIndex <= m_endFrm; nIndex += 1)
		{		
			TimeValue time = nIndex * GetTicksPerFrame();
			GetVaFrame(node,skin,idx,time);
			// msg.SetCurProgress(nIndex*100/numFrame);
		}
	}
	else
	{
		skin.m_bStatic = true;
	}
	return true;
}

//
// export all to a single file !!!
//
bool CharUtil::ExportAll(bool skel)
{
	guard
	string name;

	if(!GetFileName(name,"act(*.act)\0*.act\0"))
	{
		MessageBox(m_hWnd,"Get File Name Failed !","Error",MB_OK);
		return false;
	}

	if( name[name.length()-1] != 't' ||
		name[name.length()-2] != 'c' ||
		name[name.length()-3] != 'a' ||
		name[name.length()-4] != '.' )
	{
		name += ".act";
	}

	char tmp[MAX_STR_LEN];
	strcpy(tmp,name.c_str());

	// get path
	tmp[name.length()-4] = '\0';
	int len = name.length();

	// remove file name
	while(len>0 && tmp[len]!='\\' && tmp[len]!='/')
		len--;
	len--;
	// remove path1
	while(len>0 && tmp[len]!='\\' && tmp[len]!='/')
		len--;
	len--;

	tmp[len+1] = '\0';
	m_exportPath = tmp;

	// export actor
	CMsgWnd *msg = new CMsgWnd;
	msg->Create(hInstance,IDD_MSG);
	msg->SetMsg("Export Bone...");
	msg->SetTotalProgress(0);
	msg->SetCurProgress(0);
	msg->Refresh();

	/*
	DefNoteTrack *t = (DefNoteTrack*)ip->GetRootNode()->GetNoteTrack(0);
	if(t)
	{
		for(int k=0; k<t->keys.Count(); k++)
		{
			NoteKey *notekey = *(t->keys.Addr(k));
			int j;
			j = 1;
		}
	}
	*/

	CRT_Actor *actor = new CRT_Actor;
	actor->m_frameNum = m_endFrm - m_startFrm + 1; 
	if(skel)
        actor->m_animType = ACTOR_ANIM_SKEL;
	else if(actor->m_frameNum == 1)
		actor->m_animType = ACTOR_ANIM_STATIC;
	else 
		actor->m_animType = ACTOR_ANIM_VA;
	EnumBone(ip->GetRootNode(),*actor,true,true);
	msg->SetTotalProgress(30);

	// get bounding box
	msg->SetMsg("Export Boundingbox...");
	msg->SetCurProgress(0);
	msg->Refresh();
	RtgRangeAABB box;
	for(int nIndex=m_startFrm; nIndex<=m_endFrm; nIndex+=1)
	{	
		msg->SetCurProgress(nIndex*100/(m_endFrm-m_startFrm+1));
		box.vMin.Zero();
		box.vMax.Zero();
		TimeValue time = nIndex * GetTicksPerFrame();
		EnumBoundingBox(ip->GetRootNode(),time,box);
		RtgAABB ret;
		ret.FromRangeAABB(box);
		actor->m_boundBoxList.push_back(ret);
	}
	msg->SetTotalProgress(60);

	// export all skin
	msg->SetMsg("Export Skin...");
	msg->SetCurProgress(0);
	msg->Refresh();
	int skinIdx = 0;
	actor->RebuildBoneMap();
	EnumAllSkin(skel,actor,ip->GetRootNode());
	msg->SetTotalProgress(100);
	msg->Refresh();
	delete msg;

	// write file,if file already exist,combin it
	RtArchive *old = RtCoreFile().CreateFileReader(name.c_str());
	if(old)
	{
		RtObject *ob = old->ReadObject(NULL);
		if(!ob) return false;
		if(!ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Actor))) return false;
		CRT_Actor *oldActor = (CRT_Actor*)ob;

		// copy pose info
		actor->m_poseMap = oldActor->m_poseMap;

		// copy effect
		for(int i=0; i<oldActor->m_eftList.size(); i++)
		{
			actor->m_eftList.push_back((CRT_Effect*)oldActor->m_eftList[i]->NewObject());
		}

		// copy material
		for(int i=0; i<actor->m_skinList.size(); i++)
		{
			for(int j=0; j<oldActor->m_skinList.size(); j++)
				if(actor->m_skinList[i]->m_name == oldActor->m_skinList[j]->m_name)
				{
					actor->m_skinList[i]->m_mtlLib = (CRT_MaterialLib*)oldActor->m_skinList[j]->m_mtlLib->NewObject();
					break;
				}
		}
		delete old;
		// delete ob;
	}

	RtArchive *ar = RtCoreFile().CreateFileWriter(NULL,name.c_str());
	if(!ar) return false;
	long time = timeGetTime();
	if(!ar->WriteObject(actor)) return false;
	LOG("save actor archive time used = %d\n",timeGetTime() - time);
	delete ar;
	// C [11/26/2008 Louis.Huang]
	//actor->Output(*g_pLog);

	// LOG("write file end\n");
	return true;

//	guard_end_ex
}

bool CharUtil::ExportSkelSkin(CRT_Actor &actor,CRT_SkelSkin &skin,INode *node)
{
	if(node->IsRootNode() || node->IsNodeHidden()) 
		return false;

	if(!GetSkelSkinNode(actor,skin,node))
		return false;

	skin.m_bStatic = false;
	// Normalize bone weight
	for(int i=0; i<skin.m_verVertex.size(); i++)
	{
		skin.m_verVertex[i].Normalize();
	}
	return true;
}

void CharUtil::EnumAllSkin(bool skel,CRT_Actor *actor,INode *node)
{
	char tmp[MAX_STR_LEN];
	int cnt=0, max=node->NumberOfChildren();
	INode *child;
	if(IsSkin(node))
	{
		bool ret;
		if(skel)
		{
			CRT_SkelSkin *skin = new CRT_SkelSkin;
			if(ExportSkelSkin(*actor,*skin,node))
				actor->m_skinList.push_back(skin);
			else
				delete skin;
		}
		else
		{
			CRT_VaSkin *skin = new CRT_VaSkin;
			if(ExportVaSkin(*skin,node))
				actor->m_skinList.push_back(skin);
			else
				delete skin;
		}
	}

	// get bounding box
	for(cnt=0; cnt<max; cnt++)
	{
		child = node->GetChildNode(cnt);
		EnumAllSkin(skel,actor,child);
	}
}

bool CharUtil::EnumBone(INode *node,CRT_Actor &actor,bool abs,bool translate)
{
	char *name = node->GetName();
    node->EvalWorldState(0); 

	int cnt=0, max=node->NumberOfChildren();
	SRT_Bone bone;
	if(GetBone(node,bone,abs,translate))
		actor.m_bones.push_back(bone);
	
	INode *child;
	for(cnt=0; cnt<max; cnt++)
	{
		child = node->GetChildNode(cnt);
		EnumBone(child,actor,abs,translate);
	}
	return true;
}

bool CharUtil::GetBone(INode *node,SRT_Bone &bone,bool abs,bool translate)
{
	if(node->IsRootNode())
	{
		bone.Name = "root";
		bone.ParentName = "NULL";
		return true;
	}
	else
	{
		if(node->IsRootNode() || node->IsNodeHidden()) return false;
		if(!IsBone(node)) return false;
			
		bone.ExportTranslate = translate;
		bone.AbsMode = abs;
		bone.Name = node->GetName();
		if(node->GetParentNode()->IsRootNode())
		{
			bone.ParentName = "root";
			bone.ExportTranslate = true;
		}
		else
		{
			bone.ParentName = node->GetParentNode()->GetName();
		}
		
		// ScaleBiped(node,1);
		// Interval range = ip->GetAnimRange();
		// int numFrame = m_endFrm - m_startFrm;
		for(int nIndex=m_startFrm; nIndex<=m_endFrm; nIndex+=1)
		{		
			TimeValue time = nIndex * GetTicksPerFrame();
			Matrix3 parentTM, nodeTM, localTM;
			INode *parent = node->GetParentNode();

			if(!parent->IsRootNode()) 
			{
				nodeTM   = node->GetNodeTM(time);
				parentTM = parent->GetNodeTM(time);
				localTM  = nodeTM * Inverse(parentTM);
			}
			else
			{
				nodeTM  = node->GetNodeTM(time);
				localTM = node->GetNodeTM(time);
			}
			
			Quat quat;
			Point3 row;
			if(bone.AbsMode)
			{
				quat = nodeTM;
				row = nodeTM.GetRow(3);
			}
			else
			{
				quat = localTM;
				row = localTM.GetRow(3);
			}
		
			RtgVertex4 v4;
			// careful,use -w
			v4.x = quat.x; v4.y = quat.y; v4.z = quat.z; v4.w = -quat.w;
			bone.aryRotas.push_back(v4);

			if(nIndex == m_startFrm)
			{
				RtgVertex3 v3 = row;
				bone.aryTrans.push_back(v3);  
			}
			else if(!bone.AbsMode && !bone.ExportTranslate)
			{
				RtgVertex3 vv = bone.aryTrans[0];
				if( Abs(row.x-vv.x)>0.001||
					Abs(row.y-vv.y)>0.001||
					Abs(row.z-vv.z)>0.001)
				{
					int t;
					t = 1;
				}
			}
			else
			{
				RtgVertex3 v3 = row;
				bone.aryTrans.push_back(v3);  
			}
		}

		BOOL flag;
		int  cnt;
		bone.maxRibbonDis = 10;
		
		TimeValue time;
		Matrix3 nodeTM;
		SRT_VaVtxPos vaPos;
		Point3 row,row1;

		// export robbin info
		if(node->GetUserPropBool("ribbon",flag))
		{
			if(node->GetUserPropInt("ribbon_cnt",cnt))
			{
				if(cnt <= 0) cnt = 10;
				// bone.maxRibbonDis = cnt;
			}
			for(int nIndex=m_startFrm; nIndex<=m_endFrm; nIndex+=1)
			{	
                SRibbonFrame rbFrame;
				time = nIndex * GetTicksPerFrame();
				nodeTM = node->GetNodeTM(time);
				row = nodeTM.GetRow(3);

				vaPos.SetValue(RtgVertex3(row.x,row.y,row.z));
				rbFrame.pos.push_back(vaPos);
				
				if(nIndex == m_endFrm)
				{
					rbFrame.segNum = 0;
					bone.ribbonFrame.push_back(rbFrame);
					break;
				}

				time = (nIndex+1) * GetTicksPerFrame();
				nodeTM = node->GetNodeTM(time);
				row1 = nodeTM.GetRow(3);

				float d = sqrt( (row.x-row1.x)*(row.x-row1.x)+(row.y-row1.y)*(row.y-row1.y)+
					(row.z-row1.z)*(row.z-row1.z) );
				int pieceNum = d / bone.maxRibbonDis;

				pieceNum = cnt;
				if(pieceNum < 1) pieceNum = 1;
				
				rbFrame.segNum = pieceNum;
				for(int j=1; j<=pieceNum; j++)
				{
					time = nIndex * GetTicksPerFrame() + j*GetTicksPerFrame()/(pieceNum+1);
					nodeTM = node->GetNodeTM(time);
					row = nodeTM.GetRow(3);
					vaPos.SetValue(RtgVertex3(row.x,row.y,row.z));
					rbFrame.pos.push_back(vaPos);
				}
				bone.ribbonFrame.push_back(rbFrame);
			}
		}
		// ScaleBiped(node,0);
	}

	LOG2("bone [%s] ribbon frame = %d\n",bone.Name.c_str(), bone.ribbonFrame.size());
	return true;
}

bool CharUtil::EnumBoundingBox(INode *node,TimeValue time,RtgRangeAABB &box)
{
	if(IsSkin(node))
	{
		GetBoundingBox(node,time,box);		
	}
	for(int i=0; i<node->NumberOfChildren(); i++)
	{
		GetBoundingBox(node->GetChildNode(i),time,box);
	}
	return true;
}

bool CharUtil::GetBoundingBox(INode *node,TimeValue time,RtgRangeAABB &box)
{
	if(node->IsRootNode() || node->IsNodeHidden()) 
		return false;
	ObjectState os = node->EvalWorldState(time);
	Object*	pobj = os.obj;
	TriObject *ptriobj;
	BOOL fConvertedToTriObject = 
		pobj->CanConvertToType(triObjectClassID) &&
		(ptriobj = (TriObject*)pobj->ConvertToType(0, triObjectClassID)) != NULL;

	if(!fConvertedToTriObject) return false;

	Mesh *pMesh = &ptriobj->mesh;
	Matrix3 nodeTM,normalTM;
	nodeTM = node->GetObjectTM(time);
	for(int i=0; i<pMesh->getNumVerts(); i++)
	{
        RtgVertex3 v = pMesh->getVert(i) * nodeTM;
		box.Extend(v);
	}
	return true;
}

bool CharUtil::GetVaFrame(INode *node,CRT_VaSkin &va,vector<int> &idx,TimeValue time)
{
	if(node->IsRootNode() || node->IsNodeHidden()) 
		return false;
	ObjectState os = node->EvalWorldState(time);
	Object*	pobj = os.obj;
	TriObject *ptriobj;
	BOOL fConvertedToTriObject = 
		pobj->CanConvertToType(triObjectClassID) &&
		(ptriobj = (TriObject*)pobj->ConvertToType(0, triObjectClassID)) != NULL;

	if(!fConvertedToTriObject) return false;

	Mesh *pMesh = &ptriobj->mesh;
	pMesh->buildRenderNormals();

	RtcSkinSubset rs;
	int i;
	Mtl *pMtl = node->GetMtl();

	// if no material,return it
	if (pMtl == NULL) 
		return false;

	Matrix3 nodeTM,normalTM;
	nodeTM = node->GetObjectTM(time);
	normalTM = nodeTM;
	normalTM.SetRow(3, Point3(0,0,0));

	SRT_VaFrame frame;
	SRT_VaVtxPos ver;
	SRT_VaVtxNor nor;	
	SRT_VaVtxUV  uv;

	RtgVertex3 boardPos;
	frame.visible = node->GetVisibility(time);
	if(va.m_bBoard)
	{
        boardPos = nodeTM.GetRow(3);
		va.m_boardPosList.push_back(boardPos);
	}
	int maxVer = idx.size();
	for( i=0; i<maxVer; i++)
	{
		RtgVertex3 v;
		if(va.m_bBoard)
		{
			// if billboard only get position and scale info
			Control *c;
			c = node->GetTMController()->GetScaleController();
			ScaleValue scale;
			Interval valid;
			c->GetValue(time,&scale,valid,CTRL_ABSOLUTE);
			v = pMesh->getVert(idx[i]);
			v.x *= scale.s.x;
			v.y *= scale.s.y;
  			v.z  = 0;
		}
		else
		{
			v = pMesh->getVert(idx[i]) * nodeTM;
		}
		ver.SetValue(v);
		frame.verList.push_back(ver);
		
		v = pMesh->getNormal(idx[i]) * normalTM;
		v.Normalize();

		RtgVertex2 u;
		u.x = 0;
		u.y = 0;
		UVVert ut = pMesh->getTVert(idx[i]);
		u.x = ut.x;
		u.y = ut.y;

		uv.SetValue(u);
		nor.SetValue(v);
		if(va.m_bNor)
			frame.norList.push_back(nor);
		if(va.m_bUV)
			frame.uvList.push_back(uv);
	}

	va.m_frmList.push_back(frame);
	return true;
}

bool CharUtil::GetVaSkin(INode *node,CRT_VaSkin &va,vector<int> &idx,TimeValue time)
{
	const char *name = node->GetName();
	if(node->IsRootNode() || node->IsNodeHidden())
	{
		return false;
	}
	ObjectState os = node->EvalWorldState(time);
	Object*	pobj = os.obj;
	TriObject *ptriobj;
	BOOL fConvertedToTriObject = 
		pobj->CanConvertToType(triObjectClassID) &&
		(ptriobj = (TriObject*)pobj->ConvertToType(0, triObjectClassID)) != NULL;

	if(!fConvertedToTriObject)
	{
		ERR1("skin[%s] can't covert to tri object\n",name);
		return false;
	}

	Mesh *pMesh = &ptriobj->mesh;
	pMesh->buildRenderNormals();
	va.m_name = node->GetName();

	RtcSkinSubset rs;
	unsigned int i,j;
	Mtl *pMtl = node->GetMtl();
	
	// if no material,return it
	if (pMtl == NULL) 
	{
		ERR1("skin[%s] no material\n",name);
		return false;
	}
	CRT_MaterialLib *lib = new CRT_MaterialLib;
	va.m_mtlLib = lib;
	if(!ExportMaterialLib(*va.m_mtlLib,node))
	{
		ERR1("skin[%s] export material lib failed\n",name);
		return false;
	}

	Matrix3 nodeTM,normalTM;
	nodeTM = node->GetObjectTM(time);
	normalTM = nodeTM;
	normalTM.SetRow(3, Point3(0,0,0));

	// mulit material
	string fullname,fullpath;
	if(pMtl->NumSubMtls()) 
	{
		StdMat2 *sub;
		for(int i=0; i<pMtl->NumSubMtls(); i++) 
		{
			sub = (StdMat2*)pMtl->GetSubMtl(i);
			GetTexture(rs.Texture,fullpath,fullname,sub);
			rs.Material = lib->GetMaterial(i)->GetName();
			va.m_rsList.push_back(rs);
		}
	}
	// only one material
	else
	{
		StdMat2* stdMtl = (StdMat2*)pMtl;
		GetTexture(rs.Texture,fullpath,fullname,stdMtl);
		rs.Material = lib->GetMaterial((size_t)0)->GetName();
		va.m_rsList.push_back(rs);
	}

	RtgVertex3 boardPos;
	if(va.m_bBoard)
	{
		boardPos = nodeTM.GetRow(3);
		va.m_boardPos = boardPos;
	}

	// get triangle
	int maxFace = pMesh->getNumFaces();
	for( i=0; i<va.m_rsList.size(); i++)
	{
		for( j=0; j<maxFace; j++)
		{
			int mtlIdx = pMesh->getFaceMtlIndex(j);
			if(mtlIdx > pMtl->NumSubMtls()) 
				mtlIdx = 0;
			if(mtlIdx != i) continue;

			Face* pFace = &pMesh->faces[j];
			TVFace*	ptvFace = &pMesh->tvFace[j]; 
			short tri[3];
			for(int iv=0; iv<3; iv++)
			{
				SRT_StaticVtx sv;
				int iVert = pFace->getVert(iv);
				if(va.m_bBoard)
				{	
					// if billboard only get position and scale info
					Control *c;
					c = node->GetTMController()->GetScaleController();
					ScaleValue scale;
					Interval valid;
					c->GetValue(time,&scale,valid,CTRL_ABSOLUTE);
					sv.pos = pMesh->getVert(iVert);
					sv.pos.x *= scale.s.x;
					sv.pos.y *= scale.s.y;
					sv.pos.z  = 0;
				}
				else
				{
					sv.pos = pMesh->getVert(iVert) * nodeTM;
				}
				sv.nor = pMesh->getNormal(iVert) * normalTM;
				sv.nor.Normalize();
				sv.uv.x = sv.uv.y = 0.0f;
								
				// if(pMtl && ptvFace)
				// {
                    int iTVert = ptvFace->getTVert(iv);
					UVVert uv = pMesh->getTVert(iTVert);
					sv.uv.x = uv.x;
					sv.uv.y = 1.f - uv.y;
				// }
				tri[iv] = va.PushVertex(sv);

				// save vertex index in max
				if(tri[iv] >= idx.size())
                    idx.push_back(iVert);
			}
			va.m_rsList[i].faceList.push_back(tri[0]);
			va.m_rsList[i].faceList.push_back(tri[1]);
			va.m_rsList[i].faceList.push_back(tri[2]);
		}
	}
	return true;
}

bool CharUtil::GetStandMtl(CRT_MtlStandard *stand,StdMat2* pMtl)
{
	/*
	Color tmp;
    tmp	= pMtl->GetAmbient(0);
	m.amb.x = tmp.r; m.amb.y = tmp.g; m.amb.z = tmp.b;

	tmp = pMtl->GetDiffuse(0);
	m.dif.x = tmp.r; m.dif.y = tmp.g; m.dif.z = tmp.b;

	tmp = pMtl->GetSpecular(0);
	m.spe.x = tmp.r; m.spe.y = tmp.g; m.spe.z = tmp.b;

	tmp	= pMtl->GetSelfIllumColor(0);
	m.emi.x = tmp.r; m.emi.y = tmp.g; m.emi.z = tmp.b;
	
	m.spePower = pMtl->GetShininess(0);
	m.speStr   = pMtl->GetShinStr(0);
	*/
	// tar->Opacity = src->GetOpacity(0);
	stand->m_bTwoSide = pMtl->GetTwoSided();
	int tran = pMtl->GetTransparencyType();
	switch(tran)
	{
		case TRANSP_SUBTRACTIVE:	
			stand->m_bAlpha = true;
			stand->m_alphaOpt = "Sub";
			break;

		case TRANSP_ADDITIVE:
			stand->m_bAlpha = true;
			stand->m_alphaOpt = "Add";
			break;

		case TRANSP_FILTER:
			int ret;
			ret = pMtl->GetMapState(ID_OP);
			LOG1("ret = %d\n",ret);
			if(ret == 2)
			{
				// Modified by Wayne Wong 2010-11-27
				// 本应设置如下两个值（m_bAlpha=true, m_alphaOpt="Blend"），
				// 但是游戏中总是使用m_bAlpha=false，m_bMask=true来达到此效果，
				// 故在此强行设置一下
				//stand->m_bAlpha = true;
				//stand->m_alphaOpt = "Blend";

				stand->m_bAlpha = false;
				stand->m_bMask = true;
			}
			else
			{
				stand->m_bAlpha = false;
				stand->m_alphaOpt = "";
			}
			break;

		default: // none
			stand->m_bAlpha = false;
			stand->m_alphaOpt = "";
			break;
	}
	LOG1("alpha = %d\n",stand->m_bAlpha);
	return true;
}

bool CharUtil::GetTexture(string &texture,string &fullpath,string &fullname,StdMat* pMtl)
{
	for(int i=0; i<pMtl->NumSubTexmaps(); i++)
	{
		Texmap *tex = pMtl->GetSubTexmap(i);
		if(tex && pMtl->MapEnabled(i)) 
		{
			if(tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
			{
				BitmapTex *pbtex = (BitmapTex*)tex;
				char buf[MAX_STR_LEN], name[MAX_STR_LEN], ext[MAX_STR_LEN];
				sprintf(buf, pbtex->GetMapName());
				fullpath = buf;
				_splitpath(buf, NULL, NULL, name, ext);
				texture = name;
				fullname = name;
				fullname += ext;
				return true;
			}
		}
	}
	return false;
}

bool CharUtil::GetFrameInfo()
{
	int startFrm,endFrm;
    BOOL valid;
	startFrm = GetDlgItemFloat(m_hWnd,IDC_START_FRAME,&valid);
	if(!valid) return false;

	endFrm = GetDlgItemFloat(m_hWnd,IDC_END_FRAME,&valid);
	if(!valid) return false;
	
	Interval range = ip->GetAnimRange();
	if((!(range.Start() <= startFrm * GetTicksPerFrame()
			&& range.End() >= endFrm * GetTicksPerFrame())) || endFrm < startFrm)
	{
		MessageBox(m_hWnd,"Get Frame Info Failed","Error",MB_OK);
		return false;
	}
	
	m_startFrm = startFrm;
	m_endFrm = endFrm;
	LOG("start frm = %d,end frm = %d\n",startFrm,endFrm);
	return true;
}

void CharUtil::SelectAllFrame()
{
	Interval range = ip->GetAnimRange();
	SetDlgItemFloat(m_hWnd,IDC_START_FRAME,	range.Start()/GetTicksPerFrame());
	SetDlgItemFloat(m_hWnd,IDC_END_FRAME, range.End()/GetTicksPerFrame());
}

void CharUtil::SelectOneFrame()
{
	SetDlgItemFloat(m_hWnd,IDC_START_FRAME,0);
	SetDlgItemFloat(m_hWnd,IDC_END_FRAME,0);
}

bool CharUtil::ExportMaterialLib(CRT_MaterialLib &lib,INode *node)
{
	Mtl *pMtl = node->GetMtl();
	// if no material,return it
	if (pMtl == NULL) 
		return false;

	string name,fullname,fullpath,dest;
	CRT_MtlStandard *stand;
	// mulit material
	if(pMtl->NumSubMtls()) 
	{
		StdMat2 *sub;
		for(int i=0; i<pMtl->NumSubMtls(); i++) 
		{
			sub = (StdMat2*)pMtl->GetSubMtl(i);
			if(!GetTexture(name,fullpath,fullname,sub)) return false;
			stand = new CRT_MtlStandard;
			stand->SetName(name);
			stand->m_texture = name;
			GetStandMtl(stand,sub);
			while(!lib.AddMaterial(stand))
			{
				name += "a"; 
				stand->SetName(name);
			}
			dest = m_exportPath;
			dest += "\\texture\\";
			dest += fullname;

			if(m_exportPath.size()==0 || !CopyFile(fullpath.c_str(),dest.c_str(),FALSE))
			{
				ERR("Copy copy texture failed, from: \n%s\nto\n%s\n", fullpath.c_str(), dest.c_str());
				MessageBox(m_hWnd,"copy texture failed","Error",MB_OK);
			}
		}
	}
	// only one material
	else
	{
		StdMat2* stdMtl = (StdMat2*)pMtl;
		if(!GetTexture(name,fullpath,fullname,stdMtl)) return false;
		stand = new CRT_MtlStandard;
		stand->SetName(name);
		stand->m_texture = name;
		GetStandMtl(stand,stdMtl);
		while(!lib.AddMaterial(stand))
		{
			name += "a"; 
			stand->SetName(name);
		}
		dest = m_exportPath;
		dest += "\\texture\\";
		dest += fullname;

		if(!CopyFile(fullpath.c_str(),dest.c_str(),FALSE))
		{
			ERR("Copy copy texture failed, from: \n%s\nto\n%s\n", fullpath.c_str(), dest.c_str());
			MessageBox(m_hWnd,"copy texture failed","error",MB_OK);
		}
	}
	return true;
}

bool CharUtil::GetSkelSkinNode(CRT_Actor &actor,CRT_SkelSkin &skin,INode *node)
{
	const char *name = node->GetName();
	if(node->IsRootNode() || node->IsNodeHidden())
	{
		return false;
	}
	ObjectState os = node->EvalWorldState(m_startFrm*GetTicksPerFrame());
	Object*	pobj = os.obj;
	TriObject *ptriobj;
	BOOL fConvertedToTriObject = 
		pobj->CanConvertToType(triObjectClassID) &&
		(ptriobj = (TriObject*)pobj->ConvertToType(0, triObjectClassID)) != NULL;

	if(!fConvertedToTriObject) return false;

	Mesh *pMesh = &ptriobj->mesh;
	pMesh->buildRenderNormals();
	skin.m_name = node->GetName();

	RtcSkinSubset rs;
	unsigned int i,j;
	Mtl *pMtl = node->GetMtl();

	// if no material,return it
	if (pMtl == NULL) 
		return false;

	Matrix3 nodeTM,normalTM;
	nodeTM = node->GetObjectTM(m_startFrm*GetTicksPerFrame());
	normalTM = nodeTM;
	normalTM.SetRow(3, Point3(0,0,0));

	CRT_MaterialLib *lib = new CRT_MaterialLib;
	skin.m_mtlLib = lib;
	if(!ExportMaterialLib(*skin.m_mtlLib,node))
		return false;

	// mulit material
	string fullname,fullpath;
	if(pMtl->NumSubMtls()) 
	{
		StdMat2 *sub;
		for(int i=0; i<pMtl->NumSubMtls(); i++) 
		{
			sub = (StdMat2*)pMtl->GetSubMtl(i);
			GetTexture(rs.Texture,fullpath,fullname,sub);
			rs.Material = lib->GetMaterial(i)->GetName();
			skin.m_rsList.push_back(rs);
		}
	}
	// only one material
	else
	{
		StdMat2* stdMtl = (StdMat2*)pMtl;
		GetTexture(rs.Texture,fullpath,fullname,stdMtl);
		rs.Material = lib->GetMaterial((size_t)0)->GetName();
		skin.m_rsList.push_back(rs);
	}

	Modifier *mod = FindPhysiqueModifier(node);

	// get triangle
	int maxFace = pMesh->getNumFaces();
	for( i=0; i<skin.m_rsList.size(); i++)
	{
		for( j=0; j<maxFace; j++)
		{
			int mtlIdx = pMesh->getFaceMtlIndex(j);
			if(mtlIdx > pMtl->NumSubMtls()) 
				mtlIdx = 0;
			if(mtlIdx != i) continue;

			Face* pFace = &pMesh->faces[j];
			TVFace*	ptvFace = &pMesh->tvFace[j]; 
			short tri[3];
			for(int iv=0; iv<3; iv++)
			{
				SRT_DynamicVtx sv;
				int iVert = pFace->getVert(iv);
				sv.pos = pMesh->getVert(iVert) * nodeTM;
				sv.nor = pMesh->getNormal(iVert) * normalTM;
				sv.nor.Normalize();
				sv.uv.x = sv.uv.y = 0.0f;

				// if(pMtl && ptvFace)
				// {
				int iTVert = ptvFace->getTVert(iv);
				UVVert uv = pMesh->getTVert(iTVert);
				sv.uv.x = uv.x;
				sv.uv.y = 1.f - uv.y;
				// }
				
				// add bone weight
				AddBoneWeight(m_startFrm*GetTicksPerFrame(),actor,sv,iVert,node,mod);
				tri[iv] = skin.PushVertex(sv);
			}
			skin.m_rsList[i].faceList.push_back(tri[0]);
			skin.m_rsList[i].faceList.push_back(tri[1]);
			skin.m_rsList[i].faceList.push_back(tri[2]);
		}
	}
	return true;
}

Modifier* CharUtil::FindPhysiqueModifier(INode *node)
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

void CharUtil::AddBoneWeight(TimeValue time,CRT_Actor &actor,SRT_DynamicVtx &vert, DWORD iVert, INode* node,Modifier *mod)
{
	if(mod == NULL)
		return;
	IPhysiqueExport *pPhysiqueExport;
	pPhysiqueExport = (IPhysiqueExport *)mod->GetInterface(I_PHYINTERFACE);
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
		SRT_BoneRef info;
		IPhyRigidVertex *pTypeVertex;
		pTypeVertex = (IPhyRigidVertex *)pVertexExport;

		if(0xff == (info.bondIndex = actor.GetBoneIndex((pTypeVertex->GetNode())->GetName())))
		{
			ERR1("CharUtil::AddBoneWeight: can't find bone %s\n",(pTypeVertex->GetNode())->GetName());
			return;	
		}
		info.Weight = 1.0f;
		
		// get pos and nor offset
		Point3 tmp,ret;
		Matrix3 nodeTM,normalTM;
		nodeTM = pTypeVertex->GetNode()->GetNodeTM(time);
		normalTM = nodeTM;
		normalTM.SetRow(3, Point3(0,0,0));
		
		tmp.x = vert.pos.x;
		tmp.y = vert.pos.y;
		tmp.z = vert.pos.z;
		ret = tmp * Inverse(pTypeVertex->GetNode()->GetNodeTM(time));
		info.PosOffset.x = ret.x;
		info.PosOffset.y = ret.y;
		info.PosOffset.z = ret.z;
		
		tmp.x = vert.nor.x;
		tmp.y = vert.nor.y;
		tmp.z = vert.nor.z;
		ret = tmp * Inverse(normalTM);
		info.NorOffset.x = ret.x;
		info.NorOffset.y = ret.y;
		info.NorOffset.z = ret.z;
		vert.boneList.push_back(info);
	}
	else if(vertexType == RIGID_BLENDED_TYPE)
	{
		SRT_BoneRef info;
		IPhyBlendedRigidVertex *pTypeVertex;
		pTypeVertex = (IPhyBlendedRigidVertex *)pVertexExport;

		int nodeId;
		int nNodes = pTypeVertex->GetNumberNodes(); 
		for(nodeId = 0; nodeId < nNodes; nodeId++)
		{
			if(0xff == (info.bondIndex = actor.GetBoneIndex((pTypeVertex->GetNode(nodeId))->GetName())))
			{
				ERR1("CharUtil::AddBoneWeight: can't find bone %s\n",(pTypeVertex->GetNode(nodeId))->GetName());
				continue;	
			}
			info.Weight = pTypeVertex->GetWeight(nodeId);
			
			// get pos and nor offset
			Point3 tmp,ret;
			Matrix3 nodeTM,normalTM;
			nodeTM = pTypeVertex->GetNode(nodeId)->GetNodeTM(time);
			normalTM = nodeTM;
			normalTM.SetRow(3, Point3(0,0,0));
			
			tmp.x = vert.pos.x;
			tmp.y = vert.pos.y;
			tmp.z = vert.pos.z;
			ret = tmp * Inverse(pTypeVertex->GetNode(nodeId)->GetNodeTM(time));
			info.PosOffset.x = ret.x;
			info.PosOffset.y = ret.y;
			info.PosOffset.z = ret.z;
		
			tmp.x = vert.nor.x;
			tmp.y = vert.nor.y;
			tmp.z = vert.nor.z;
			ret = tmp * Inverse(normalTM);
			info.NorOffset.x = ret.x;
			info.NorOffset.y = ret.y;
			info.NorOffset.z = ret.z;
			vert.boneList.push_back(info);
		}
	}

	/*
	IPhyFloatingVertex *tmp;
	tmp = pContextExport->GetFloatingVertexInterface(iVert);
	if(tmp)
	{
		int cnt = tmp->GetNumberNodes();
		g_pLog->Logf("float bone = %d\n",tmp);
	}
	*/
	std::sort(vert.boneList.begin(),vert.boneList.end());
	std::reverse(vert.boneList.begin(),vert.boneList.end()); 
}
