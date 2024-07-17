
//////////////////////////////////////////////////////////////////////////
//
//  FileName     : ThreadLock.h
//  Author       : zxy
//  CreateTime   : 2009-05-18
//  Desc         : Resource Lock, use to thread Synchronization 
//
//////////////////////////////////////////////////////////////////////////

#ifndef _ThreadLock_H_
#define _ThreadLock_H_

class thread_lock
{
public :
    thread_lock()
    {
        InitializeCriticalSection(&m_CriticlSection);
    };

    ~thread_lock()
    {
        DeleteCriticalSection(&m_CriticlSection);
    }

    void lock()
    {
        EnterCriticalSection(&m_CriticlSection);
    }

    void unlock()
    {
        LeaveCriticalSection(&m_CriticlSection);
    }

private :
    CRITICAL_SECTION m_CriticlSection;
};

#endif