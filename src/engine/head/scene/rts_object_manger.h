
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rts_object_manager.h
//     Author : zxy
// CreateTime : 2010.05.01
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _RtsObjectManager_H_
#define _RtsObjectManager_H_

class RtsObjectManager
{
    typedef map<objectId, RtsObject*> objmap_t;
public :

    RtsObjectManager()
    {
    }

    virtual ~RtsObjectManager()
    {
    }

    void Clear()
    {
        for (objmap_t::iterator _it = m_mapObject.begin();
             _it != m_mapObject.end(); ++_it)
            DEL_ONE(_it->second);
        m_mapObject.clear();
    }

    objectId NewObject(RtRuntimeClass* _rclass);
    void DelObject(objectId _objId);

    bool SetObject(RtsObject* _obj)
    {
        if (!_obj)
            return false;

        objmap_t::iterator _it = m_mapObject.find(_obj->Id());
        if (_it != m_mapObject.end())
            return false;
        m_mapObject[_obj->Id()] = _obj;

        return true;
    }

    RtsObject* GetObject(objectId _objId)
    {
        objmap_t::iterator _it = m_mapObject.find(_objId);
        if (_it == m_mapObject.end())
            return NULL;
        return _it->second;
    }

    size_t ObjectNum() const
    {
        return m_mapObject.size();
    }

    RtsObject* RecomputeObjectId(objectId _objId);
    void Clearup(set<size_t, size_t>& remapTab);

private :
    objmap_t m_mapObject;
};

extern RtsObjectManager g_ObjectManager;
inline RtsObjectManager* RtObjectManager() { return &g_ObjectManager; }

#endif