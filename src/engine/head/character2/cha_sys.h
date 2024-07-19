#ifndef __CHA_SYS_H__
#define __CHA_SYS_H__

enum EActorResType
{
	ACTOR_RES_TYPE_ACT    = 0,
	ACTOR_RES_TYPE_SKIN   = 1,
	ACTOR_RES_TYPE_MTLLIB = 2,
	ACTOR_RES_TYPE_EFFECT = 3,
	ACTOR_RES_TYPE_MTL    = 4,
};

enum ThreadCommand
{
	T_CREATE,
	T_DELETE,
};
const int MAX_ACTOR_RES_TYPE = 4;

class CRT_ObjectInst : public RtObject	
{
	RT_DECLARE_DYNAMIC(CRT_ObjectInst, RtObject, NULL, "")
	CRT_ObjectInst() {} 
	virtual ~CRT_ObjectInst() {}

	void SetData(CRT_ObjectData *data) { m_data = data; }
	CRT_ObjectData *GetData() { return m_data; }
	virtual void InstanceInit() { m_data->m_instList.push_back(this); }
	virtual void InstanceExit() { m_data->m_instList.remove(this); }
	virtual void OnDataChange(DWORD p1, DWORD p2) {}

	CRT_ObjectData		*m_data;
};

struct SActorPerformance
{
	DWORD boneTimer, skinTimer, phyTimer, renderTimer;
	DWORD coreRenderTimer, shadowTimer, effectRenderTimer;
	DWORD actorNum, vaVtxNum, skelBoneNum, skelVtxNum;
	DWORD actorRenderTri, skinRenderTri, effectRenderTri;
	DWORD particleRenderTri, projectRenderTri, ribbonRenderTri;
};

struct SJob
{
	ThreadCommand  cmd;
	CRT_ObjectData* pObject;
	CRT_ObjectInst* pIn;
	CRT_ObjectInst* pOut;
	string filename;
	EActorResType type;
	RtgDevice* pDevice;
};
//class RtLoadThread : public RtThread
//{
//protected:
//	OnRun()
//	{
//
//	}
class RtEngineSystemChar : public RtEngineSystem //,public RtThread
{
public:
	RT_DECLARE_DYNCREATE(RtEngineSystemChar, RtEngineSystem, NULL, "")
	RtEngineSystemChar();
	virtual ~RtEngineSystemChar();

	//
	// from RtEngineSystem
	//
	virtual bool Init(RtIni &cfg);
	virtual void Tick(DWORD delta);
	virtual void Exit();
	virtual bool ExecCmd(const char *cmd, const char *param);
	virtual const char* GetName();

	//
	// RtEngineSystemChar private
	//
	CRT_ObjectInst *CreateInstance(EActorResType type, const char *name,CRT_ObjectInst* pIn = NULL,RtgDevice* pDevice = NULL , bool bReload = false );
	CRT_ObjectInst *CreateInstance(EActorResType type, CRT_ObjectData *data);
	void DeleteInstance(CRT_ObjectInst *inst);


	CRT_ObjectData *Precache(const char *name, EActorResType type);
	void Uncache(CRT_ObjectData *obj);
	void Unload(CRT_ObjectData *obj);
	CRT_ObjectData *CopyFrom(CRT_ObjectData *data);

	void RegisterPath(const char *path, EActorResType type);
	void Output();

	// option
	bool m_bRenderActor;
	bool m_bRenderShadow;
	bool m_bRenderProjector;
	bool m_bRenderHelpSkin;
	bool m_bUpdateBoundingBox;
    
	// load object by name with .ext
	CRT_ObjectData *Load(const char *fullname, EActorResType type,CRT_ObjectInst* pIn = NULL,RtgDevice* pDevice = NULL, bool bReload = false );
	CRT_ObjectData *LoadFromFile(const char *file, EActorResType type);
	const char *GetFileExtByType(EActorResType type);
	bool GetFinishedJob(SJob &job);
	void UpdateThreadInstance();
protected:
	int OnRun();
private:
	std::queue<SJob> JobQueue;
	std::queue<SJob> JobFinishedQueue;

	RtThreadLock m_JQlock;
	RtThreadLock m_JFQlock;

	CRT_ObjectData *Find(const char *fullname);

	EXT_SPACE::unordered_map<string,CRT_ObjectData*> m_obMap;
	vector<string> m_regPath[MAX_ACTOR_RES_TYPE];
	string m_defaultPath[MAX_ACTOR_RES_TYPE];
};

//
// global interface
//
extern SActorPerformance g_actorPerformance;
inline SActorPerformance *PFM() 
{
	return &g_actorPerformance;
}

void CreateEngineSystemChar();
void DestroyEngineSystemChar();
extern RtEngineSystemChar *g_engineSystemChar;
inline RtEngineSystemChar *ACTOR()
{
	return g_engineSystemChar; 
}
#endif
