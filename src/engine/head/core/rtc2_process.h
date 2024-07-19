/**
* @file rtc2_process.h 进程管理（非操作系统进程），在这里是指一些可以同步执行的程序
* @author janhail
*/

namespace rt2_core {

/*-----------------------------------------------------------------------------
-   RtProcess
-----------------------------------------------------------------------------*/

/** 进程类.
*      其他需要进行时间调控的进程都继承自这个类
*    使用方法
*       RtProcessItem test(new RtProcess("Test", 0));
*       processManager.Attach(test);
*       不能直接new，应该使用智能指针，这样才能保证被管理
*/
class RtProcess
{
public:
    enum
    {
        PROCESS_FLAG_ATTACHED = 0x00000001,
    };
    friend class RtProcessManager;
    friend class SmartPtr<RtProcess>;
    RtProcess(const char* szType, unsigned int uOrder = 0);
    virtual ~RtProcess();

public:
    /// 察看是否已经要
    virtual bool        IsDead() const              { return m_bKill; }
    /** 标记进程为将要删除，注意：不要尝试执行delete.
    *    如果Kill的对象m_pNext有对象，并且m_pNext的对象没有被其他人引用，那它会被一并删除
    *    该删除是由于智能指针指向的对象没有引用而引起的，不需要人为干预
    */
    virtual void        Kill();

    /// 取得类型
    virtual const char* GetType() const             { return m_szType.c_str(); }
    /// 设置类型
    virtual void        SetType(const char* szType) { m_szType = szType; }

    /// 是否是激活状态
    virtual bool        IsActive() const            { return m_bActive; }
    /// 设置激活状态
    virtual void        SetActive(const bool b)     { m_bActive = b; }
    /// 这个进程附加在管理器上吗？
    virtual bool        IsAttached() const;
    /// 把它标记为附加的，只能被RtProcessManager调用
    virtual void        SetAttached(const bool wantAttached);

    /// 是否是暂停状态
    virtual bool        IsPaused() const            { return m_bPaused; }
    /// 切换暂停状态
    virtual void        TogglePause()               { m_bPaused = !m_bPaused; }

    /// 是否是第一次Update
    bool                IsInitialized() const       { return !m_bInitialUpdate; }

    /// 取得下一个相关进程
    SmartPtr<RtProcess> const GetNext() const       { return m_pNext; }
    /** 设置进程相关性
    *   A->SetNext(B) 表示B会等待A完成(A被删除后B才有可能被删除)
    */
    virtual void    SetNext(SmartPtr<RtProcess> nNext);

    virtual void    OnUpdate(const int deltaMilliseconds);
    virtual void    OnInitialize()                  {}

private:
    RtProcess();
    RtProcess(const RtProcess& rhs);

protected:
    RtString            m_szType;
    bool                m_bKill;
    bool                m_bActive;
    bool                m_bPaused;
    bool                m_bInitialUpdate;
    SmartPtr<RtProcess> m_pNext;

private:
    unsigned int        m_uProcessFlags;

};

inline void RtProcess::OnUpdate(const int deltaMilliseconds)
{
    if (m_bInitialUpdate)
    {
        OnInitialize();
        m_bInitialUpdate = false;
    }
}

/*-----------------------------------------------------------------------------
-   RtProcessManager
-----------------------------------------------------------------------------*/

typedef SmartPtr<RtProcess> RtProcessItem;

/** 进程管理类.
*/
class RtProcessManager
{
public:
    /// 更新所有的进程
    void UpdateProcesses(int deltaMilliseconds);
    /// 分离所有的进程
    void DeleteProcessList();
    /// 是否是激活的进程
    bool IsProcessActive(const char* szType);
    /// 绑定进程
    void Attach(RtProcessItem pProcess);
    /// 是否有进程
    bool HasProcesses();

private:
    /// 分离进程
    void Detach(SmartPtr<RtProcess> pProcess);

protected:
    typedef std::list< SmartPtr<RtProcess> > ProcessList;
    ProcessList m_ProcessList;
};

/*-----------------------------------------------------------------------------
-   RtProcessTimer
-----------------------------------------------------------------------------*/
/** 计数器进程.
*       - 使用方法
*           @code
*           RtProcessItem wait(new RtProcessTimer(1000, 5));
*           processManager.Attach(wait);
*           @endcode
*         前面这段代码的意思是：设置一个每1000毫秒响应一次，总共5次的计时器
*/
class RtProcessTimer : private RtProcess
{
public:
    /// 初始化计时器，dwHeartbeatMilliseconds为心跳间隔时间，iCount为次数，-1表示无限次
    RtProcessTimer(unsigned int dwHeartbeatMilliseconds, int iCount=-1) :
        RtProcess("timer", 0),
        m_iTick(0),
        m_dwStart(0),
        m_dwHeartbeat(dwHeartbeatMilliseconds),
        m_dwNext(dwHeartbeatMilliseconds)
    {
        if (iCount<0) m_dwStop = 0xFFFFFFFF;
        else          m_dwStop = dwHeartbeatMilliseconds * iCount + 1;
    }
    virtual void OnTimer(int iTick) { } ///< 当计时器时间到的时候触发这个函数，iTick表示是第几次触发，从1到iCount
    virtual void OnTimerStart()     { } ///< 当计时器开始工作的时候触发这个函数
    virtual void OnTimerEnd()       { } ///< 当计时器结束的时候触发这个函数
private:
    virtual void OnUpdate(const int deltaMilliseconds)
    {
        RtProcess::OnUpdate(deltaMilliseconds);
        if (m_bActive)
        {
            m_dwStart += deltaMilliseconds;
            if (m_dwStart>=m_dwStop)
            {
                OnTimerEnd();
                Kill();
            }else if (m_dwStart>=m_dwNext)
            {
                m_iTick ++;
                m_dwNext += m_dwHeartbeat;
                OnTimer(m_iTick);
            }
        }
    }
    virtual void OnInitialize()     { OnTimerStart(); }
private:
    unsigned int    m_dwStart;
    unsigned int    m_dwHeartbeat;
    unsigned int    m_dwNext;
    unsigned int    m_dwStop;
    int             m_iTick;
};

/*-----------------------------------------------------------------------------
-   RtProcessWait
-----------------------------------------------------------------------------*/

/** 等待进程.
*       - 使用方法
*           @code
*           RtProcessItem wait(new RtProcessWait(3000));
*           processManager.Attach(wait);
*           RtProcessItem sing(new RtProcessPlayPose(pCharactor, "sing"));
*           wait->SetNext(sing);
*           @endcode
*         前面这段代码的意思是：等待3秒后就播放一个唱歌的Pose
*/
class RtProcessWait : private RtProcess
{
public:
    /// 初始化等待进程，dwNumMilliseconds为等待的时间
    RtProcessWait(unsigned int dwNumMilliseconds) :
        RtProcess("wait", 0),
        m_dwStart(0),
        m_dwStop(dwNumMilliseconds)
    {
    }
private:
    virtual void OnUpdate(const int deltaMilliseconds)
    {
        RtProcess::OnUpdate(deltaMilliseconds);
        if (m_bActive)
        {
            m_dwStart += deltaMilliseconds;
            if (m_dwStart>=m_dwStop)
            {
                Kill();
            }
        }
    }
protected:
    unsigned int    m_dwStart;
    unsigned int    m_dwStop;
};

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
