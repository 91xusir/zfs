
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rts_object_manager.cpp
//     Author : zxy
// CreateTime : 2010.05.01
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#include "scene/rt_scene.h"

RtsObjectManager g_ObjectManager;

objectId RtsObjectManager::NewObject(RtRuntimeClass* _rclass)
{
    RTASSERT(_rclass && _rclass->IsDerivedFrom(RT_RUNTIME_CLASS(RtsObject)));
    RtsObject* _obj = ((RtsObject*)_rclass->CreateObject());
    
    CoCreateGuid((GUID*)&_obj->m_Id);
    m_mapObject[_obj->Id()] = _obj;

    return _obj->Id();
}

void RtsObjectManager::DelObject(objectId _objId)
{
    objmap_t::iterator _it = m_mapObject.find(_objId);
   
    if (_it == m_mapObject.end())
    {
        RtCoreLog().Error("delete object(id:%lu) fail~\n", _objId);
        return;
    }

    DEL_ONE(_it->second);
    m_mapObject.erase(_it);
}

RtsObject* RtsObjectManager::RecomputeObjectId(objectId _objId)
{
    objmap_t::iterator _it = m_mapObject.find(_objId);

    if (_it == m_mapObject.end())
        return NULL;

    RtsObject* _obj = _it->second;
    m_mapObject.erase(_it);

    CoCreateGuid((GUID*)&_obj->m_Id);
    m_mapObject[_obj->Id()] = _obj;

    return _obj;
}

void RtsObjectManager::Clearup(set<size_t, size_t>& remapTab)
{

}
