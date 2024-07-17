/**
* @file rtc2_lock.h 同步锁.
* @author janhail
*/

namespace rt2_core {

/** 进程锁.
*   这种锁消耗的资源最高
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

    /// 取得当前句柄
    const RtProcessMutexType&    GetHandle() const  { return m_handle; }
    RtProcessMutexType&          GetHandle()        { return m_handle; }
    const RtProcessMutexType*    operator&() const  { return &m_handle; }
    RtProcessMutexType*          operator&()        { return &m_handle; }

public:
    /// 构造函数, 创建锁对象
    RtProcessLock()                                 { RtProcessMutexInit(&m_handle); }

    /// 析构函数，销毁锁对象
    ~RtProcessLock()                                { RtProcessMutexDestroy(&m_handle); }

public:
    /** 进程自动锁.
    *       原理是利用局部变量的作用域，超出作用域会被自动析构
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

/** 线程锁.
*   这种锁消耗的资源比较高
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
    /// 取得当前句柄
    const RtThreadMutexType&    GetHandle() const   { return m_handle; }
    RtThreadMutexType&          GetHandle()         { return m_handle; }
    const RtThreadMutexType*    operator&() const   { return &m_handle; }
    RtThreadMutexType*          operator&()         { return &m_handle; }

public:
    /// 构造函数, 创建锁对象
    RtThreadLock()                                  { RtThreadMutexInit(&m_handle); }

    /// 析构函数，销毁锁对象
    ~RtThreadLock()                                 { RtThreadMutexDestroy(&m_handle); }

public:
    /** 线程自动锁.
    *       原理是利用局部变量的作用域，超出作用域会被自动析构
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

/** 资源锁.
*   这种锁消耗的资源比较低
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

    /// 取得当前句柄
    const RtResLockType&    GetHandle() const       { return m_handle; }
    RtResLockType&          GetHandle()             { return m_handle; }
    const RtResLockType*    operator&() const       { return &m_handle; }
    RtResLockType*          operator&()             { return &m_handle; }

public:
    /// 构造函数, 创建锁对象
    RtResLock()                                     { RtResLockInit(&m_handle); }

    /// 析构函数，销毁锁对象
    ~RtResLock()                                    { RtResLockDestroy(&m_handle); }

public:
    /** 资源自动锁.
    *       原理是利用局部变量的作用域，超出作用域会被自动析构
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
