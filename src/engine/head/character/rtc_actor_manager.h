
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtc_actor_manager.h
//     Author : zxy
// CreateTime : 2010.04.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _RtcAcotrManager_H_
#define _RtcAcotrManager_H_

#include "core/rt2_core.h"
#include "graph/rtg_thread_lock.h"

class CRT_ActorInstance;
class CRT_SkinInstance;
class CRT_Effect;
class CRT_Pool;
class CRT_PoolObject;
class CRT_MaterialLib;


typedef enum _ActorObjectType
{
    Actor_Obj_Unknow = 0,
    Actor_Obj_Actor,
    Actor_Obj_Skin,
    Actor_Obj_Effect,
    Actor_Obj_Mtl
} ActorObjectType;

typedef enum _PoolFindRes
{
    PoolFind_NoExist = 0,
    PoolFind_Not_Load,
    PoolFind_Loaded

} PoolFindRes;

struct RtcAcotrObject
{
    ActorObjectType _type;
    RtObject* _obj;
    RtObject* _depend;
    string _filename;

    RtcAcotrObject()
        : _type(Actor_Obj_Unknow), _obj(NULL), _depend(NULL)
    {
    }

    RtcAcotrObject(ActorObjectType _t, RtObject* _o, const char* _name = "", RtObject* _a = NULL)
        : _type(_t), _obj(_o), _filename(_name), _depend(_a)
    {
    }

    RtcAcotrObject(const RtcAcotrObject& _ao)
        : _type(_ao._type), _obj(_ao._obj), _filename(_ao._filename), _depend(_ao._depend)
    {
    }

    RtcAcotrObject& operator = (const RtcAcotrObject& _ao)
    {
        _type = _ao._type;
        _obj = _ao._obj;
        _filename = _ao._filename;
        _depend = _ao._depend;
        return *this;
    }
};


class RtcAcotrManager
{
    typedef map<string, CRT_PoolObject*> objmap_t;
    typedef set<RtObject*> objset_t;
    typedef list<RtObject*> objlist_t;
    typedef list<CRT_ActorInstance*> actorcache_t;

    enum
    {
        Actor_Cache_Size = 32
    };


public :

    bool Init();
    void Exit();
    void FrameMove();

    CRT_ActorInstance* CreateActor(const char fileName[], bool bForceLoad = false, const ActorNotify& _Notify = ActorNotify());
    CRT_SkinInstance*  CreateSkin(const char fileName[], CRT_ActorInstance* _act, bool bForceLoad = false);
    CRT_Effect* CreateEffect(const char fileName[], CRT_ActorInstance* _act, bool bForceLoad = false);
    CRT_MaterialLib* CreateMtl(const char fileName[], CRT_SkinInstance* _skin, bool bForceLoad = false);

    void ReleaseActor(CRT_ActorInstance* _actor);
    void ReleaseSkin(CRT_SkinInstance* _skin);
    void ReleaseEffect(CRT_Effect* _effect);
    void ReleaseMtl(CRT_MaterialLib* _mtl);
    void ReleasePoolObject(CRT_PoolObject* _pobj);

    RtcAcotrManager();
    virtual ~RtcAcotrManager();

    size_t GetLoadQueueSize() const { return m_loadQueue.size(); }
    size_t GetCratQueueSize() const { return m_cratQueue.size(); }

    CRT_PoolObject* NewObject(RtRuntimeClass* _Request)
    {
        return LoadObject(_Request);
    }

    void AddSearchPath(const char _path[])
    {
        m_actorPath.push_back(_path);
    }

    void EnableMutiThreadLoad(bool bEnable = true)
    {
        m_bMutiThreadLoad = bEnable;
    }

private :
    static void Load_Thread(void* _ptMgr);

    bool LoadActorObject(const RtcAcotrObject& _actobj);
    bool CreateActorObject(const RtcAcotrObject& _actobj);
    void ReleaseActorObject(const RtcAcotrObject& _actobj);

    CRT_PoolObject* LoadObject(const char _name[], RtRuntimeClass* _Request);
    CRT_PoolObject* LoadObject(RtRuntimeClass* _Request);
    void DestroyObject(CRT_PoolObject* _obj);

    CRT_PoolObject* NewObject(const char _fullName[]);
    PoolFindRes FindObject(CRT_PoolObject** _obj, const char _name[], char _fullName[]);

    bool LoadDirect(CRT_PoolObject* _obj, const char _name[], const char _fullName[], RtRuntimeClass* _Request);
    CRT_PoolObject* LoadDirect(const char _name[], const char _fullName[], RtRuntimeClass* _Request);

private :
    list<RtcAcotrObject> m_loadQueue;
    list<RtcAcotrObject> m_cratQueue;
    objset_t m_releaseQueue;
    actorcache_t m_actorCache;
    thread_lock m_loadLock;
    thread_lock m_cratLock;
    bool m_bExit;
    bool m_bExited;
    bool m_bMutiThreadLoad;

    objmap_t m_objMap;
    vector<string> m_actorPath;
};

#define Safe_ReleaseActor(_act) do { if (_act) RtcGetActorManager()->ReleaseActor(_act); (_act) = NULL; } while(false);

#endif