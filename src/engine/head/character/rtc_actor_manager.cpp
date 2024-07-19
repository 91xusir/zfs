
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtc_actor_manager.cpp
//     Author : zxy
// CreateTime : 2010.04.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#include "actor.h"
#include "process.h"

namespace {

    const char* GetActorFileName(const char* name)
    {
		S_MEMDEF(fileNameBuf, MAX_PATH)
		S_MEMPROTECTOR(fileNameBuf, MAX_PATH, bMP)

		size_t len = strlen(name);
        if (name[len - 4] == '.' && 
            name[len - 3] == 'a' && 
            name[len - 2] == 'c' && 
            name[len - 1] == 't')
        {
            return name;
        }
        else
        {
            rt2_sprintf(fileNameBuf, "%s.act", name);
            return fileNameBuf;
        }
    }

    const char* GetSkinFileName(const char* name)
    {
		S_MEMDEF(fileNameBuf, MAX_PATH)
		S_MEMPROTECTOR(fileNameBuf, MAX_PATH, bMP)
        size_t len = strlen(name);

        if (name[len - 4] == '.' && 
            name[len - 3] == 's' && 
            name[len - 2] == 'k' && 
            name[len - 1] == 'i')
        {
            return name;
        }
        else
        {
            rt2_sprintf(fileNameBuf, "%s.ski", name);
            return fileNameBuf;
        }
    }

    const char* GetEffectFileName(const char* name)
    {
		S_MEMDEF(fileNameBuf, MAX_PATH)
		S_MEMPROTECTOR(fileNameBuf, MAX_PATH, bMP)
        size_t len = strlen(name);

        if (name[len - 4] == '.' && 
            name[len - 3] == 'e' && 
            name[len - 2] == 'f' && 
            name[len - 1] == 't')
        {
            return name;
        }
        else
        {
            rt2_sprintf(fileNameBuf, "%s.eft", name);
            return fileNameBuf;
        }
    }

    const char* GetMtlLibFileName(const char* name)
    {
		S_MEMDEF(fileNameBuf, MAX_PATH)
		S_MEMPROTECTOR(fileNameBuf, MAX_PATH, bMP)
        size_t len = strlen(name);

        if (name[len - 4] == '.' && 
            name[len - 3] == 'm' && 
            name[len - 2] == 't' && 
            name[len - 1] == 'l')
        {
            return name;
        }
        else
        {
            rt2_sprintf(fileNameBuf, "%s.mtl", name);
            return fileNameBuf;
        }
    }

}

RtcAcotrManager::RtcAcotrManager()
    : m_bExit(true),
      m_bExited(true),
      m_bMutiThreadLoad(true)
{

}

RtcAcotrManager::~RtcAcotrManager()
{
    Exit();
}

bool RtcAcotrManager::Init()
{
    AddSearchPath(".\\");
    m_bExit = false;
    uintptr_t res = _beginthread(Load_Thread, 0, (void*)this);
    RTASSERT(res != 1L);
    return true;
}

void RtcAcotrManager::Exit()
{
    m_bExit = true;
    while (!m_bExited)
        Sleep(10);

    for (actorcache_t::iterator i = m_actorCache.begin();
        i != m_actorCache.end(); ++i)
    {
        (*i)->StopPose();
    }

    for (actorcache_t::iterator i = m_actorCache.begin();
        i != m_actorCache.end(); ++i)
    {
        ReleaseActorObject(RtcAcotrObject(Actor_Obj_Actor, (*i)));
    }

    m_actorCache.clear();

    RtCoreLog().Warn("pool object leak (%u)\n", m_objMap.size());
    for (objmap_t::iterator it = m_objMap.begin(); 
        it != m_objMap.end(); ++it)
    {
        CRT_PoolObject* ob = (*it).second;
        RtCoreLog().Warn("\tname = %s,ref = %d\n",ob->m_diskFile.c_str(), ob->m_poRef);
    }
}

void RtcAcotrManager::FrameMove()
{
    begin_log_tick(0);

    RtcAcotrObject _ao;

    m_cratLock.lock();
    if (!m_cratQueue.empty())
    {
        _ao = m_cratQueue.front();
        m_cratQueue.pop_front();
    }
    m_cratLock.unlock();

    if (_ao._type != Actor_Obj_Unknow)
    {
        objset_t::iterator idel = m_releaseQueue.find(_ao._obj);

        if (idel == m_releaseQueue.end())
        {
            CreateActorObject(_ao);
            _ao._obj->SetState(Object_State_Ready);
            _ao._obj->OnCreateFinish();
        }
        else
        {
            m_releaseQueue.erase(_ao._obj);
            ReleaseActorObject(_ao);
        }
    }
}

CRT_ActorInstance* RtcAcotrManager::CreateActor(const char _fileName[], 
                                                bool bForceLoad /* = false */, 
                                                const ActorNotify& _Notify /* = ActorNotify */)
{
    CRT_ActorInstance* _actor = NULL;
    CRT_PoolObject* _obj = NULL;
    const char* _name = GetActorFileName(_fileName);

    struct _FindPred
    {
        _FindPred(const char _name[])
            : _findName(_name)
        {
        }

        bool operator() (const CRT_ActorInstance* _actor) const 
        {
            return !stricmp(_actor->m_Name.c_str(), _findName);
        }

        const char* _findName;
    };

    actorcache_t::iterator iiac = find_if(m_actorCache.begin(), m_actorCache.end(), _FindPred(_name));
    
    if (iiac != m_actorCache.end())
    {
        _actor = (*iiac);
        m_actorCache.erase(iiac);
        _actor->Register_ActorNotify(_Notify);
        _Notify.Notify(_actor, Actor_Create_Finish);
        RtGetPref()->ActorNum++;
        return _actor;
    }

    _actor = RT_NEW CRT_ActorInstance;
    char _fullName[MAX_PATH];
    _actor->m_Name = _name;
    _actor->Register_ActorNotify(_Notify);

    PoolFindRes _res = FindObject(&_obj, _name, _fullName);

    switch (_res)
    {
    case PoolFind_Not_Load :
        {
            RTASSERT(!_obj);

            if (!bForceLoad && m_bMutiThreadLoad)
            {
                _actor->SetState(Object_State_Loading);
                m_loadLock.lock();
                m_loadQueue.push_back(RtcAcotrObject(Actor_Obj_Actor, _actor, _name, _actor));
                m_loadLock.unlock();
            }
            else
            {
                RtcAcotrObject _actobj = RtcAcotrObject(Actor_Obj_Actor, _actor, _name, _actor);

                if (!LoadActorObject(_actobj))
                {
                    DEL_ONE(_actor);
                    return NULL;
                }

                CreateActorObject(_actobj);
                _actor->SetState(Object_State_Ready);
                _actor->OnCreateFinish();
            }

            RtGetPref()->ActorNum++;
            return _actor;

        }
        break;

    case PoolFind_Loaded :
        {
            RTASSERT(_obj);

            _actor->SetCoreObject((CRT_Actor*)_obj->NewObject());
            _actor->OnLoadFinish();
            CreateActorObject(RtcAcotrObject(Actor_Obj_Actor, _actor, _name, _actor));
            _actor->SetState(Object_State_Ready);
            _actor->OnCreateFinish();
            RtGetPref()->ActorNum++;
            return _actor;
        }
        break;
    default :
        return NULL;
        break;
    }

    return NULL;
}

CRT_SkinInstance* RtcAcotrManager::CreateSkin(const char _fileName[], 
                                              CRT_ActorInstance* _act, 
                                              bool bForceLoad /* = false */
                                              )
{
    CRT_SkinInstance* _skin = RT_NEW CRT_SkinInstance;
    const char* fileName = GetSkinFileName(_fileName);
    _skin->m_Name = fileName;

    if (!bForceLoad && m_bMutiThreadLoad)
    {
        _skin->SetState(Object_State_Loading);
        m_loadLock.lock();
        m_loadQueue.push_back(RtcAcotrObject(Actor_Obj_Skin, _skin, fileName, _act));
        m_loadLock.unlock();
    }
    else
    {
        RtcAcotrObject _actobj = RtcAcotrObject(Actor_Obj_Skin, _skin, fileName, _act);

        if (!LoadActorObject(_actobj))
        {
            DEL_ONE(_skin);
            return NULL;
        }

        CreateActorObject(_actobj);
        _skin->SetState(Object_State_Ready);
    }

    return _skin;
}

CRT_Effect* RtcAcotrManager::CreateEffect(const char _fileName[], 
                                          CRT_ActorInstance* _act, 
                                          bool bForceLoad /* = false */
                                          )
{
    CRT_PoolObject* _eft = NULL;
    const char* _name = GetEffectFileName(_fileName);
    char _fullName[MAX_PATH];

    PoolFindRes _res = FindObject(&_eft, _name, _fullName);

    switch (_res)
    {
    case PoolFind_Not_Load :
        {
            RTASSERT(!_eft);
            _eft = NewObject(_fullName);
            ((CRT_Effect*)_eft)->SetName2(_name);

            if (!bForceLoad && m_bMutiThreadLoad)
            {
                _eft->SetState(Object_State_Loading);
                m_loadLock.lock();
                m_loadQueue.push_back(RtcAcotrObject(Actor_Obj_Effect, _eft, _fullName, _act));
                m_loadLock.unlock();
            }
            else
            {
                RtcAcotrObject _actobj = RtcAcotrObject(Actor_Obj_Effect, _eft, _fullName, _act);

                if (!LoadActorObject(_actobj))
                {
                    DEL_ONE(_eft);
                    return NULL;
                }

                CreateActorObject(_actobj);
                _eft->SetState(Object_State_Ready);
            }

            return (CRT_Effect*)_eft;

        }
        break;

    case PoolFind_Loaded :
        {
            RTASSERT(_eft);
            CRT_Effect* _eftp = (CRT_Effect*)_eft->NewObject();
            _eftp->SetName2(_name);
            _eftp->OnLoadFinish();
            CreateActorObject(RtcAcotrObject(Actor_Obj_Effect, _eftp, _fullName, _act));
            _eft->SetState(Object_State_Ready);
            return _eftp;
        }
        break;
    default :
        return NULL;
        break;
    }

    return NULL;
}

CRT_MaterialLib* RtcAcotrManager::CreateMtl(const char _fileName[], 
                                            CRT_SkinInstance* _skin, 
                                            bool bForceLoad /* = false */)
{
    CRT_PoolObject* _mtl = NULL;
    const char* _name = GetMtlLibFileName(_fileName);
    char _fullName[MAX_PATH];

    PoolFindRes _res = FindObject(&_mtl, _name, _fullName);

    switch (_res)
    {
    case PoolFind_Not_Load :
        {
            RTASSERT(!_mtl);
            _mtl = RT_NEW CRT_MaterialLib;
            ((CRT_MaterialLib*)_mtl)->m_Name = _name;

            if (!bForceLoad && m_bMutiThreadLoad)
            {
                _mtl->SetState(Object_State_Loading);
                m_loadLock.lock();
                m_loadQueue.push_back(RtcAcotrObject(Actor_Obj_Mtl, _mtl, _fullName, _skin));
                m_loadLock.unlock();
            }
            else
            {
                RtcAcotrObject _actobj = RtcAcotrObject(Actor_Obj_Mtl, _mtl, _fullName, _skin);

                if (!LoadActorObject(_actobj))
                {
                    DEL_ONE(_mtl);
                    return NULL;
                }

                CreateActorObject(_actobj);
                _mtl->SetState(Object_State_Ready);
            }

            return (CRT_MaterialLib*)_mtl;

        }
        break;

    case PoolFind_Loaded :
        {
            RTASSERT(_mtl);
            CRT_MaterialLib* _mtlp = (CRT_MaterialLib*)_mtl->NewObject();
            _mtlp->m_Name = _name;
            _mtlp->OnLoadFinish();
            CreateActorObject(RtcAcotrObject(Actor_Obj_Mtl, _mtlp, _fullName, _skin));
            _mtlp->SetState(Object_State_Ready);
            return _mtlp;

        }
        break;
    default :
        return NULL;
        break;
    }

    return NULL;
}

void RtcAcotrManager::ReleaseActor(CRT_ActorInstance* _actor)
{
    if (!_actor)
        return;

    RTASSERT(find(m_actorCache.begin(), m_actorCache.end(), _actor) == m_actorCache.end());

    if (m_actorCache.size() > Actor_Cache_Size)
    {
        CRT_ActorInstance* _actordel = m_actorCache.front();
        m_actorCache.pop_front();

        RTASSERT(_actordel);

        if (_actordel->GetState() < Object_State_Ready)
            m_releaseQueue.insert(_actordel);
        else
            ReleaseActorObject(RtcAcotrObject(Actor_Obj_Actor, _actordel));
    }

    _actor->Reset();
    m_actorCache.push_back(_actor);

    if (RtGetPref()->ActorNum > 0)
        RtGetPref()->ActorNum--;

}

void RtcAcotrManager::ReleaseSkin(CRT_SkinInstance* _skin)
{
    if (!_skin)
        return;

    if (_skin->GetState() < Object_State_Ready)
        m_releaseQueue.insert(_skin);
    else
        ReleaseActorObject(RtcAcotrObject(Actor_Obj_Skin, _skin));
}

void RtcAcotrManager::ReleaseEffect(CRT_Effect* _eft)
{
    if (!_eft)
        return;

    if (_eft->GetState() < Object_State_Ready)
        m_releaseQueue.insert(_eft);
    else
        ReleaseActorObject(RtcAcotrObject(Actor_Obj_Effect, _eft));
}


void RtcAcotrManager::ReleaseMtl(CRT_MaterialLib* _mtl)
{
    if (!_mtl)
        return;

    if (_mtl->GetState() < Object_State_Ready)
        m_releaseQueue.insert(_mtl);
    else
        ReleaseActorObject(RtcAcotrObject(Actor_Obj_Mtl, _mtl));
}

void RtcAcotrManager::ReleaseActorObject(const RtcAcotrObject& _actobj)
{
    switch (_actobj._type)
    {
    case Actor_Obj_Actor :
        {
            CRT_ActorInstance* _actor = (CRT_ActorInstance*)_actobj._obj;
            DestroyObject(_actor->GetCore());
            DEL_ONE(((RtcAcotrObject&)_actobj)._obj);
        }
        break;
    case Actor_Obj_Skin :
        {
            CRT_SkinInstance* _skin = (CRT_SkinInstance*)_actobj._obj;
            DestroyObject(_skin->GetCore());
            DEL_ONE(((RtcAcotrObject&)_actobj)._obj);
        }
        break;
    case Actor_Obj_Effect :
        {
            CRT_Effect* _eft = (CRT_Effect*)_actobj._obj;
            DestroyObject(_eft);
        }
        break;
    case Actor_Obj_Mtl :
        {
            CRT_MaterialLib* _mtl = (CRT_MaterialLib*)_actobj._obj;
            DestroyObject(_mtl);
        }
        break;
    default :
        break;
    }
}

void RtcAcotrManager::ReleasePoolObject(CRT_PoolObject* _pobj)
{
    if (!_pobj)
        return;
    DestroyObject(_pobj);
}

void RtcAcotrManager::Load_Thread(void* _ptMgr)
{
    RtcAcotrManager* ptMgr = (RtcAcotrManager*)_ptMgr;
    ptMgr->m_bExited = false;

    while (!ptMgr->m_bExit)
    {
        if (ptMgr->m_loadQueue.empty())
        {
            Sleep(10);
            continue;
        }

        ptMgr->m_loadLock.lock();
        RtcAcotrObject _ao = ptMgr->m_loadQueue.front();
        ptMgr->m_loadQueue.pop_front();
        ptMgr->m_loadLock.unlock();

        if (ptMgr->LoadActorObject(_ao))
        {
            _ao._obj->Loack_SetState(Object_State_Creating);
            ptMgr->m_cratLock.lock();
            ptMgr->m_cratQueue.push_back(_ao);
            ptMgr->m_cratLock.unlock();
        }
    }

    ptMgr->m_bExited = true;
}

bool RtcAcotrManager::LoadActorObject(const RtcAcotrObject& _actobj)
{
    switch (_actobj._type)
    {
    case Actor_Obj_Actor :
        {
            CRT_ActorInstance* _actor = (CRT_ActorInstance*)_actobj._obj;
            RTASSERT(_actor);

            CRT_Actor* _core = (CRT_Actor*)LoadObject(_actobj._filename.c_str(), RT_RUNTIME_CLASS(CRT_Actor));

            if (!_core)
                return false;

            _actor->SetCoreObject(_core);
            _actor->OnLoadFinish();

        }
        break;
    case Actor_Obj_Skin :
        {
            CRT_SkinInstance* _skin = (CRT_SkinInstance*)_actobj._obj;
            CRT_ActorInstance* _actor = (CRT_ActorInstance*)_actobj._depend;
            RTASSERT(_skin);
            RTASSERT(_actor);

            CRT_Skin* core = (CRT_Skin*)LoadObject(_actobj._filename.c_str(), RT_RUNTIME_CLASS(CRT_Skin));

            if (!core)
                return false;

            RTASSERT(core->m_poRef > 0);
            --core->m_poRef;
            _skin->AttachActor(_actor);
            _skin->SetCoreObject(core);
            _skin->OnLoadFinish();
        }
        break;
    case Actor_Obj_Effect :
        {
            CRT_Effect* _eft = (CRT_Effect*)_actobj._obj;
            CRT_ActorInstance* _actor = (CRT_ActorInstance*)_actobj._depend;
            RTASSERT(_eft);
            RTASSERT(_actor);

            _eft->LinkActor(_actor);
            LoadDirect(_eft, _eft->m_Name.c_str(), _actobj._filename.c_str(), RT_RUNTIME_CLASS(CRT_Effect));
        }
        break;
    case Actor_Obj_Mtl :
        {
            CRT_MaterialLib* _mtl = (CRT_MaterialLib*)_actobj._obj;
            RTASSERT(_mtl);

            LoadDirect(_mtl, _mtl->m_Name.c_str(), _actobj._filename.c_str(), RT_RUNTIME_CLASS(CRT_MaterialLib));

        }
        break;
    default :
        RTASSERT(false);
        break;
    }

    return true;
}

bool RtcAcotrManager::CreateActorObject(const RtcAcotrObject& _actobj)
{
    bool res = false;

    switch (_actobj._type)
    {
    case Actor_Obj_Actor :
        {
            CRT_ActorInstance* _actor = (CRT_ActorInstance*)_actobj._obj;
            RTASSERT(_actor);
            res = _actor->Create();
        }
        break;
    case Actor_Obj_Skin :
        {
            CRT_SkinInstance* _skin = (CRT_SkinInstance*)_actobj._obj;
            CRT_ActorInstance* _actor = (CRT_ActorInstance*)_actobj._depend;
            RTASSERT(_skin);
            RTASSERT(_actor);
            _skin->GetCore()->RequestCreate(_actor->m_core);
            res = _skin->Create();
        }
        break;
    case Actor_Obj_Effect :
        {
            CRT_Effect* _eft = (CRT_Effect*)_actobj._obj;
            CRT_ActorInstance* _actor = (CRT_ActorInstance*)_actobj._depend;
            RTASSERT(_eft);
            RTASSERT(_actor);
            res = _eft->Create();
        }
        break;
    case Actor_Obj_Mtl :
        {
            CRT_MaterialLib* _mtl = (CRT_MaterialLib*)_actobj._obj;
            CRT_SkinInstance* _skin = (CRT_SkinInstance*)_actobj._depend;
            RTASSERT(_mtl);
            RTASSERT(_skin);
            res = _mtl->Create();
            _skin->OnMaterialChange();
        }
        break;
    default :
        RTASSERT(false);
        break;
    }

    return res;
}

CRT_PoolObject* RtcAcotrManager::LoadObject(RtRuntimeClass* _Request)
{
    CRT_PoolObject* _obj = (CRT_PoolObject*)_Request->CreateObject();
    _obj->m_poRef  = 1;
    return _obj;
}

CRT_PoolObject* RtcAcotrManager::LoadObject(const char _name[], RtRuntimeClass* _Request)
{
    CRT_PoolObject* _obj = NULL;
    char _fullName[MAX_PATH];

    PoolFindRes _fres = FindObject(&_obj, _name, _fullName);

    switch (_fres)
    {
    case PoolFind_Not_Load :
        {
            RTASSERT(!_obj);
            if (_obj = LoadDirect(_name, _fullName, _Request))
                return _obj;
        }
        break;
    case PoolFind_Loaded :
        {
            RTASSERT(_obj);
            return _obj->NewObject();
        }
        break;
    default :
        break;
    }

    RtCoreLog().Error("load file fail, can't find file [%s]\n", _name);

    return NULL;
}

void RtcAcotrManager::DestroyObject(CRT_PoolObject* _obj)
{
    if (!_obj) 
        return;

    switch(_obj->m_poType)
    {
    case Pool_Type_File :
        {
            objmap_t::iterator it = m_objMap.find(_obj->m_poTag);

            if (it != m_objMap.end())
            {
                if (--_obj->m_poRef <= 0) 
                {
                    m_objMap.erase(it);
                    DEL_ONE(_obj);
                }
            }
            else
            {
                RtCoreLog().Error("can not find pool object : %s in objmap\n", _obj->m_poTag.c_str());
            }
        }
        break;

    case Pool_Type_Memory :
        {
            if (--_obj->m_poRef <= 0) 
                DEL_ONE(_obj);
        }
        break;

    case Pool_Type_None :
        {
            DEL_ONE(_obj);
        }
        break;

    case Pool_Type_Dummy :
        break;

    default :
        break;
    }
}

CRT_PoolObject* RtcAcotrManager::NewObject(const char _fullName[])
{
    RTASSERT(_fullName);
    RtArchive* ar = RtCoreFile().CreateFileReader(_fullName);

    if (!ar) 
        return NULL;

    RtObject* _obj = ar->NewObject();
    DEL_ONE(ar);

    if (!_obj->IsKindOf(RT_RUNTIME_CLASS(CRT_PoolObject)))
        DEL_ONE(_obj);

    return (CRT_PoolObject*)_obj;
}

PoolFindRes RtcAcotrManager::FindObject(CRT_PoolObject** _obj, const char _name[], char _fullName[])
{
    RTASSERT(_obj);
    RTASSERT(_name);
    RTASSERT(_fullName);

    objmap_t::iterator _it = m_objMap.find(_name);

    if (_it != m_objMap.end())
    {
        rt2_strcpy(_fullName, _it->second->m_diskFile.c_str());
        (*_obj) = _it->second;
        return PoolFind_Loaded;
    }

    for (size_t i = 0; i < m_actorPath.size(); ++i)
    {
        rt2_sprintf(_fullName, "%s%s", m_actorPath[i].c_str(), _name);
        if (RtCoreFile().FileExist(_fullName))
            return PoolFind_Not_Load;
    }

    return PoolFind_NoExist;
}

bool RtcAcotrManager::LoadDirect(CRT_PoolObject* _obj, const char _name[], const char _fullName[], RtRuntimeClass* _Request)
{
    RTASSERT(_obj);
    RTASSERT(_name);
    RTASSERT(_fullName);
    RTASSERT(_Request);

    RtArchive* ar = RtCoreFile().CreateFileReader(_fullName);

    if (!ar) 
        return false;

    bool _res = ar->ReadObject(_obj, NULL);
    DEL_ONE(ar);

    if (!_res)
        return false;

    if (!_obj->IsKindOf(RT_RUNTIME_CLASS(CRT_PoolObject)) ||
        !_obj->IsKindOf(_Request))
    {
        RtCoreLog().Error("load object fiald, type unmatch\n");
        return false;
    }

    _obj->m_poType = Pool_Type_File;
    _obj->m_diskFile = _fullName;
    _obj->m_poTag = _name;
    _obj->m_poRef = 1;

    _obj->OnLoadFinish();
    m_objMap[_name] = _obj;

    return true;
}

CRT_PoolObject* RtcAcotrManager::LoadDirect(const char _name[], const char _fullName[], RtRuntimeClass* _Request)
{
    CRT_PoolObject* obj = NULL;
    RtArchive* ar = RtCoreFile().CreateFileReader(_fullName);

    if (!ar) 
        return false;

    RtObject* tmp = ar->ReadObject(NULL);
    DEL_ONE(ar);

    if (!tmp)
        return NULL;

    if (!tmp->IsKindOf(RT_RUNTIME_CLASS(CRT_PoolObject)) ||
        !tmp->IsKindOf(_Request))
    {
        RtCoreLog().Error("load object fiald, type unmatch\n");
        DEL_ONE(tmp);
        return NULL;
    }

    obj = (CRT_PoolObject*)tmp;
    obj->m_poType = Pool_Type_File;
    obj->m_diskFile = _fullName;
    obj->m_poTag = _name;
    obj->m_poRef = 1;

    obj->OnLoadFinish();
    m_objMap[_name] = obj;

    return obj;
}