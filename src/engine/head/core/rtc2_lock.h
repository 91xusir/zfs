/**
* @file rtc2_lock.h ͬ����.
* @author janhail
*/

namespace rt2_core {

/** ������.
*   ���������ĵ���Դ���
*/
class RtProcessLock
{
public:
    /// Lock
    void                        Lock()              { RtProcessMutexLock(&m_handle); }
    void                        Enter()              { RtProcessMutexLock(&m_handle); }

    /// Unlock
    void                        Unlock()            { RtProcessMutexUnlock(&m_handle); }
    void                        Leave()            { RtProcessMutexUnlock(&m_handle); }

    /// ȡ�õ�ǰ���
    const RtProcessMutexType&    GetHandle() const  { return m_handle; }
    RtProcessMutexType&          GetHandle()        { return m_handle; }
    const RtProcessMutexType*    operator&() const  { return &m_handle; }
    RtProcessMutexType*          operator&()        { return &m_handle; }

public:
    /// ���캯��, ����������
    RtProcessLock()                                 { RtProcessMutexInit(&m_handle); }

    /// ��������������������
    ~RtProcessLock()                                { RtProcessMutexDestroy(&m_handle); }

public:
    /** �����Զ���.
    *       ԭ�������þֲ������������򣬳���������ᱻ�Զ�����
    */  
    class AutoLock
    {
    public:
        AutoLock(RtProcessLock & lock):m_lock(lock)  { lock.Lock(); }
        ~AutoLock()                                  { m_lock.Unlock(); }
    private:
        RtProcessLock &        m_lock;
    };

private:
    RtProcessMutexType       m_handle;
};

/** �߳���.
*   ���������ĵ���Դ�Ƚϸ�
*/
class RtThreadLock
{
public:
    /// Lock
    void                        Lock()              { RtThreadMutexLock(&m_handle); }
    void                        Enter()              { RtThreadMutexLock(&m_handle); }
    /// Unlock
    void                        Unlock()            { RtThreadMutexUnlock(&m_handle); }
    void                        Leave()            { RtThreadMutexUnlock(&m_handle); }
    /// ȡ�õ�ǰ���
    const RtThreadMutexType&    GetHandle() const   { return m_handle; }
    RtThreadMutexType&          GetHandle()         { return m_handle; }
    const RtThreadMutexType*    operator&() const   { return &m_handle; }
    RtThreadMutexType*          operator&()         { return &m_handle; }

public:
    /// ���캯��, ����������
    RtThreadLock()                                  { RtThreadMutexInit(&m_handle); }

    /// ��������������������
    ~RtThreadLock()                                 { RtThreadMutexDestroy(&m_handle); }

public:
    /** �߳��Զ���.
    *       ԭ�������þֲ������������򣬳���������ᱻ�Զ�����
    */  
    class AutoLock
    {
    public:
        AutoLock(RtThreadLock & lock):m_lock(lock)  { lock.Lock(); }
        ~AutoLock()                                 { m_lock.Unlock(); }
    private:
        RtThreadLock &        m_lock;
    };

private:
    RtThreadMutexType       m_handle;
};

/** ��Դ��.
*   ���������ĵ���Դ�Ƚϵ�
*/
class RtResLock
{
public:
    /// Lock
    bool                        TryLock()           { return RtResLockTryLock(&m_handle); }

    /// Lock
    void                        Lock()              { RtResLockLock(&m_handle); }

    /// Unlock
    void                        Unlock()            { RtResLockUnlock(&m_handle); }

    /// ȡ�õ�ǰ���
    const RtResLockType&    GetHandle() const       { return m_handle; }
    RtResLockType&          GetHandle()             { return m_handle; }
    const RtResLockType*    operator&() const       { return &m_handle; }
    RtResLockType*          operator&()             { return &m_handle; }

public:
    /// ���캯��, ����������
    RtResLock()                                     { RtResLockInit(&m_handle); }

    /// ��������������������
    ~RtResLock()                                    { RtResLockDestroy(&m_handle); }

public:
    /** ��Դ�Զ���.
    *       ԭ�������þֲ������������򣬳���������ᱻ�Զ�����
    */  
    class AutoLock
    {
    public:
        AutoLock(RtResLock & lock):m_lock(lock)     { lock.Lock(); }
        ~AutoLock()                                 { m_lock.Unlock(); }
    private:
        RtResLock &        m_lock;
    };

private:
    RtResLockType       m_handle;
};

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
