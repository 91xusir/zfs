#include "character2/actor.h"

// --------------------------------------------------------------------------------------------
// actor\\ act+skin
// effect\\ eff		
// material\\ mtllib
// texture\\ tex
// --------------------------------------------------------------------------------------------
SActorPerformance g_actorPerformance;
RtEngineSystemChar *g_engineSystemChar = NULL;

/*
void CreateEngineSystem(const char *name)
{
	if(strcmp(name,"actor") == 0)
	{
		RT_NEW 
	}
}

void DestroyEngineSystem(RtEngineSystem *system)
{
	delete system;
}
*/

void CreateEngineSystemChar()
{
	if( g_engineSystemChar == NULL )
	{
		g_engineSystemChar = RT_NEW RtEngineSystemChar;
	}
}

void DestroyEngineSystemChar()
{
	DEL_ONE( g_engineSystemChar );	
}

RT_IMPLEMENT_DYNAMIC(CRT_ObjectInst, RtObject, NULL, "")
RT_IMPLEMENT_DYNAMIC(RtEngineSystem, RtObject, NULL, "")
RT_IMPLEMENT_DYNCREATE(RtEngineSystemChar, RtEngineSystem, NULL, "")
// --------------------------------------------------------------------------------------------
// engine sub system : RtEngineSystemChar 
// --------------------------------------------------------------------------------------------
RtEngineSystemChar::RtEngineSystemChar()
{
	m_defaultPath[ACTOR_RES_TYPE_ACT]	 = "actor";
	m_defaultPath[ACTOR_RES_TYPE_SKIN]   = "actor";
	m_defaultPath[ACTOR_RES_TYPE_MTLLIB] = "material";
	m_defaultPath[ACTOR_RES_TYPE_EFFECT] = "effect";

	m_bRenderActor = true;
	m_bRenderShadow = true;
	m_bRenderHelpSkin = false;
	m_bUpdateBoundingBox = true;
	m_bRenderProjector = false;

	JobQueue.empty();
	JobFinishedQueue.empty();
}

RtEngineSystemChar::~RtEngineSystemChar()
{
	if(m_obMap.size() != 0)
	{
		RtCoreLog().Info("object pool leak:");
		Output();
	}

	vector<CRT_ObjectData*> obVec;
	EXT_SPACE::unordered_map<string,CRT_ObjectData*>::iterator it;
	for(it=m_obMap.begin(); it!=m_obMap.end(); it++)
	{
		obVec.push_back(it->second);
	}

	for(int i=0; i<obVec.size(); i++)
	{
		Unload(obVec[i]);
	}
}

bool RtEngineSystemChar::Init(RtIni &cfg)
{
	/*
	RT_STATIC_REGISTRATION_CLASS(RtEngineSystemChar);
	
	// data class
	RT_STATIC_REGISTRATION_CLASS(CRT_ObjectData);
	RT_STATIC_REGISTRATION_CLASS(CRT_ObjectInst);

	RT_STATIC_REGISTRATION_CLASS(CRT_Skin);
	RT_STATIC_REGISTRATION_CLASS(CRT_VaSkin);
	RT_STATIC_REGISTRATION_CLASS(CRT_SkelSkin);
	RT_STATIC_REGISTRATION_CLASS(CRT_Actor);
	RT_STATIC_REGISTRATION_CLASS(CRT_MaterialLib);

	RT_STATIC_REGISTRATION_CLASS(CRT_Material);
	RT_STATIC_REGISTRATION_CLASS(CRT_MtlStandard);
	RT_STATIC_REGISTRATION_CLASS(CRT_MtlBlend);

	RT_STATIC_REGISTRATION_CLASS(CRT_Effect);
	RT_STATIC_REGISTRATION_CLASS(CRT_EffectEmitter);
	RT_STATIC_REGISTRATION_CLASS(CRT_EffectRibbon);
	RT_STATIC_REGISTRATION_CLASS(CRT_EffectBlur);
	*/

	// instance class
	
#ifndef MAX_UTILITY
	/*
	RT_STATIC_REGISTRATION_CLASS(CRT_ActorInstance);
	RT_STATIC_REGISTRATION_CLASS(CRT_SkinInstance);
	RT_STATIC_REGISTRATION_CLASS(CRT_MtlInstStandard);
	RT_STATIC_REGISTRATION_CLASS(CRT_MtlInstBlend);
	RT_STATIC_REGISTRATION_CLASS(CRT_MtlLibInstance);
	RT_STATIC_REGISTRATION_CLASS(CRT_EffectInstance);
	RT_STATIC_REGISTRATION_CLASS(CRT_EftInstEmitter);
	RT_STATIC_REGISTRATION_CLASS(CRT_EftInstRibbon);
	RT_STATIC_REGISTRATION_CLASS(CRT_EftInstBlur);
	*/

	CRT_ActorInstance::m_rootValid   = RT_NEW CRT_ActorInstance;
	CRT_ActorInstance::m_rootUnvalid = RT_NEW CRT_ActorInstance;
#endif

	/*
	RT_STATIC_REGISTRATION_CLASS(CRT_EffectGlow);
	RT_STATIC_REGISTRATION_CLASS(CRT_EffectLight);
	*/
#ifdef MULTITHREAD
    Start();
#endif

	RtCoreLog().Info("%s: init ok\n", GetName());
	return true;
}

void RtEngineSystemChar::Tick(DWORD delta)
{
	// reset PFM
	memset(PFM(), 0, sizeof(SActorPerformance));
#ifndef MAX_UTILITY
	CRT_ActorInstance::Update(delta);
#endif
}

void RtEngineSystemChar::Exit()
{
#ifndef MAX_UTILITY	
	DEL_ONE( CRT_ActorInstance::m_rootValid );
	DEL_ONE( CRT_ActorInstance::m_rootUnvalid );
#endif
#ifdef MULTITHREAD
	Terminate(0);
#endif
	RtCoreLog().Info("%s: exit...\n", GetName());
	Output();	
}

bool RtEngineSystemChar::ExecCmd(const char *cmd, const char *param)
{
	return false;
}

const char* RtEngineSystemChar::GetName()
{
	return "character system";
}

void RtEngineSystemChar::Unload(CRT_ObjectData *obj)
{
	if(!obj) return;

	EXT_SPACE::unordered_map<string,CRT_ObjectData*>::iterator it;
	if(obj->m_poType == POOL_TYPE_OBJ_MAIN)
	{
       // m_lock3.Lock();
		it = m_obMap.find(obj->m_poTag);
		if(it != m_obMap.end())
		{
			obj->m_poRef--;
			if(obj->m_poRef == 0) 
			{
				RtCoreLog().Info("unload obj [%s] ref = 0\n", obj->m_poTag.c_str());
				m_obMap.erase(it);				
				DEL_ONE( obj );
			}
		}
		else
		{
			RtCoreLog().Info("unload object %s failed", obj->m_diskFile.c_str());
		}
		//m_lock3.Unlock();
		return;
	}
	else if(obj->m_poType == POOL_TYPE_OBJ_SUB)
	{
		return;
	}
	else if(obj->m_poType == POOL_TYPE_OBJ_COPY)
	{
		return;
	}
    RtCoreLog().Info("unload object type error [%d]\n", obj->m_poType);
}

void RtEngineSystemChar::Output()
{
	EXT_SPACE::unordered_map<string,CRT_ObjectData*>::iterator it;
	RtCoreLog().Info("char pool size = %d:\n",m_obMap.size());
	for(it=m_obMap.begin(); it!=m_obMap.end(); it++)
	{
		CRT_ObjectData *ob = (*it).second;
		RtCoreLog().Info("\tname = %s, ref = %d\n", ob->m_diskFile.c_str(), ob->m_poRef);
	}
}

CRT_ObjectData *RtEngineSystemChar::CopyFrom(CRT_ObjectData *data)
{
	CRT_ObjectData *copy = (CRT_ObjectData*)data->GetRuntimeClass()->CreateObject();
	if(copy->CopyFrom(data))
		return copy;
	else
		return NULL;
}

void RtEngineSystemChar::RegisterPath(const char *path, EActorResType type)
{
	int idx = type;
	m_regPath[idx].push_back(path);
}

CRT_ObjectInst *RtEngineSystemChar::CreateInstance(EActorResType type, const char *name,CRT_ObjectInst *pIn/* = NULL*/,RtgDevice* pDevice/*=NULL*/, bool bReload /*= false */)
{
	string fullname;
	fullname = name;
	if( fullname.size() <= 0 ) return NULL;
	fullname += GetFileExtByType(type);
	CRT_ObjectData *data = Load(fullname.c_str(), type, pIn, pDevice , bReload );
	if(!data)
		return NULL;

	// create inst
	// RtRuntimeClass *cl = GetInstClassByType(type);
	// CRT_ObjectInst *inst = (CRT_ObjectInst*)cl->CreateObject();
	CRT_ObjectInst *inst = data->CreateInstance();
	inst->SetData(data);
	inst->InstanceInit();

	return inst;
}

CRT_ObjectInst *RtEngineSystemChar::CreateInstance(EActorResType type, CRT_ObjectData *data)
{
	CRT_ObjectInst *inst = data->CreateInstance();
	inst->SetData(data);
	inst->InstanceInit();

	return inst;
}

//CRT_ObjectInst *RtEngineSystemChar::CreateInstance(RtRuntimeClass *type, CRT_ObjectData *data)
//{
//	RtRuntimeClass *cl = type;
//	CRT_ObjectInst *inst = (CRT_ObjectInst*)cl->CreateObject();
//	inst->SetData(data);
//	inst->InstanceInit();
//
//	return inst;
//}

void RtEngineSystemChar::DeleteInstance(CRT_ObjectInst *inst)
{
#ifdef MULTITHREAD
	LockThread();
#endif
	inst->InstanceExit();
	Unload(inst->GetData());
	DEL_ONE(inst);
#ifdef MULTITHREAD
	UnlockThread();
#endif
	
}


CRT_ObjectData *RtEngineSystemChar::Precache(const char *name, EActorResType type)
{
	CRT_ObjectData *obj;
	obj = Load(name, type);
	obj->Precache();
	return obj;
}

void RtEngineSystemChar::Uncache(CRT_ObjectData *obj)
{
	obj->Uncache();
	Unload(obj);
}

CRT_ObjectData *RtEngineSystemChar::Find(const char *fullname)
{
	EXT_SPACE::unordered_map<string,CRT_ObjectData*>::iterator it;
	it = m_obMap.find(fullname);
	if(it != m_obMap.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}

CRT_ObjectData *RtEngineSystemChar::Load(const char *fullname, EActorResType type,CRT_ObjectInst* pIn,RtgDevice* pDevice, bool bReload /*= false */)
{
	CRT_ObjectData *obj;
    
#ifdef MULTITHREAD
	LockThread();
	obj = Find(fullname);
	UnlockThread();
#else
	obj = Find(fullname);
#endif
	if( bReload )
	{
		obj = NULL;
	}
	if(obj)
	{
		obj->m_poRef++;
		return obj;
	}
	else
	{
#ifdef MULTITHREAD
		SJob job;
		job.cmd = T_CREATE;
		job.pIn = pIn;
		job.filename = fullname;
		job.type = type;
		job.pDevice = pDevice;
		m_JQlock.Lock();
		JobQueue.push(job);
		m_JQlock.Unlock();
		RtCoreLog().Info("pool: multithread load %s enter, ref = %d\n", fullname, 0);
		return NULL;
#else
		string path;
		int max = m_regPath[type].size();
		for(int i=0; i<max; i++)
		{
			path = m_regPath[type][i];
			path = path + fullname;
			obj = LoadFromFile(path.c_str(), type);
			if(!obj) continue;

			obj->m_poType = POOL_TYPE_OBJ_MAIN;
			obj->m_diskFile = path;
			obj->m_poTag = fullname;
			obj->m_poRef = 0;
			if( false == bReload )
			{
				m_obMap[obj->m_poTag] = obj;
			}
			obj->m_poRef++;
			RtCoreLog().Info("pool: load %s ok, ref = %d\n", obj->m_poTag.c_str(), obj->m_poRef);
			return obj;
		}
	}
	RtCoreLog().Info("%s: Load Failed, Can't find file [%s].\n", __FUNCTION__, fullname);
	return NULL;
#endif
}

CRT_ObjectData *RtEngineSystemChar::LoadFromFile(const char *file, EActorResType type)
{
	int beg = rtMilliseconds();

	// check if object already in pool
	RtArchive *ar = RtCoreFile().CreateFileReader(file);
	if(!ar) return false;

	RtObject *tmp = ar->ReadObject(NULL);	
	DEL_ONE( ar );
	
	if(!tmp)
	{
		// read file error
		DEL_ONE( tmp );
		return NULL;
	}

	/*
	if(!tmp->IsKindOf(GetClassByType(type)))
	{
		// not pool object
		RtCoreLog().Info("%s: Load Failed,type mismatch.\n", __FUNCTION__);
		delete tmp;
		return NULL;
	}
	*/
	
	RtCoreLog().Info("%s: load %s from disk,time used = %d\n", __FUNCTION__, file, rtMilliseconds()-beg);
	return (CRT_ObjectData*)tmp;
}

const char *RtEngineSystemChar::GetFileExtByType(EActorResType type)
{
	string fullname,ext;
	switch(type)
	{
		case ACTOR_RES_TYPE_ACT:	return ".act";
			break;
		case ACTOR_RES_TYPE_SKIN:	return ".ski";
			break;
		case ACTOR_RES_TYPE_MTLLIB: return ".mtl";
			break;
		case ACTOR_RES_TYPE_EFFECT: return ".eft";
			break;
	}
	return "";
}

int RtEngineSystemChar::OnRun()
{
#ifdef MULTITHREAD
	while(1)
	{
		m_JQlock.Lock();
		if(JobQueue.size()!=0)
		{
			SJob job = JobQueue.front();
			JobQueue.pop();
			m_JQlock.Unlock();
			
            switch(job.cmd) 
			{
            case T_CREATE:

				string path;
				int max = m_regPath[job.type].size();
				for(int i=0; i<max; i++)
				{
					path = m_regPath[job.type][i];
					path = path + job.filename.c_str();//fullname;
					obj = LoadFromFile(path.c_str(), job.type);
					if(!obj) continue;

					obj->m_poType = POOL_TYPE_OBJ_MAIN;
					obj->m_diskFile = path;
					obj->m_poTag = job.filename.c_str();//fullname;
					obj->m_poRef = 0;

                    LockThread();
					m_obMap[obj->m_poTag] = obj;
					UnlockThread();
					obj->m_poRef++;
					job.pObject = obj;
					break;
				}

				m_JFQlock.Lock();
				JobFinishedQueue.push(job);
				m_JFQlock.Unlock();
				break;
					//RtCoreLog().Info("pool: load %s ok, ref = %d\n", obj->m_poTag.c_str(), obj->m_poRef);					
            default:
            }

		}
		else
		{
			m_JQlock.Unlock();
		}	
		Sleep(0.01);
	}
#endif
	return 0;
}

bool RtEngineSystemChar::GetFinishedJob(SJob &job)
{
   
	m_JFQlock.Lock();
	if (JobFinishedQueue.size() > 0)
	{
		job = JobFinishedQueue.front();
		JobFinishedQueue.pop();
		m_JFQlock.Unlock();
		return true;
	}
	else
		m_JFQlock.Unlock();
		return false;

}

void RtEngineSystemChar::UpdateThreadInstance()
{
#ifdef MULTITHREAD
	SJob job;
	while (GetFinishedJob(job))
	{
		job.pIn->OnMLoad(job.pObject,job.type);
	}
#endif
}
