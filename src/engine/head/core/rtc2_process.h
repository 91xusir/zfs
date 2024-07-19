/**
* @file rtc2_process.h ���̹����ǲ���ϵͳ���̣�����������ָһЩ����ͬ��ִ�еĳ���
* @author janhail
*/

namespace rt2_core {

/*-----------------------------------------------------------------------------
-   RtProcess
-----------------------------------------------------------------------------*/

/** ������.
*      ������Ҫ����ʱ����صĽ��̶��̳��������
*    ʹ�÷���
*       RtProcessItem test(new RtProcess("Test", 0));
*       processManager.Attach(test);
*       ����ֱ��new��Ӧ��ʹ������ָ�룬�������ܱ�֤������
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
    /// �쿴�Ƿ��Ѿ�Ҫ
    virtual bool        IsDead() const              { return m_bKill; }
    /** ��ǽ���Ϊ��Ҫɾ����ע�⣺��Ҫ����ִ��delete.
    *    ���Kill�Ķ���m_pNext�ж��󣬲���m_pNext�Ķ���û�б����������ã������ᱻһ��ɾ��
    *    ��ɾ������������ָ��ָ��Ķ���û�����ö�����ģ�����Ҫ��Ϊ��Ԥ
    */
    virtual void        Kill();

    /// ȡ������
    virtual const char* GetType() const             { return m_szType.c_str(); }
    /// ��������
    virtual void        SetType(const char* szType) { m_szType = szType; }

    /// �Ƿ��Ǽ���״̬
    virtual bool        IsActive() const            { return m_bActive; }
    /// ���ü���״̬
    virtual void        SetActive(const bool b)     { m_bActive = b; }
    /// ������̸����ڹ���������
    virtual bool        IsAttached() const;
    /// �������Ϊ���ӵģ�ֻ�ܱ�RtProcessManager����
    virtual void        SetAttached(const bool wantAttached);

    /// �Ƿ�����ͣ״̬
    virtual bool        IsPaused() const            { return m_bPaused; }
    /// �л���ͣ״̬
    virtual void        TogglePause()               { m_bPaused = !m_bPaused; }

    /// �Ƿ��ǵ�һ��Update
    bool                IsInitialized() const       { return !m_bInitialUpdate; }

    /// ȡ����һ����ؽ���
    SmartPtr<RtProcess> const GetNext() const       { return m_pNext; }
    /** ���ý��������
    *   A->SetNext(B) ��ʾB��ȴ�A���(A��ɾ����B���п��ܱ�ɾ��)
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

/** ���̹�����.
*/
class RtProcessManager
{
public:
    /// �������еĽ���
    void UpdateProcesses(int deltaMilliseconds);
    /// �������еĽ���
    void DeleteProcessList();
    /// �Ƿ��Ǽ���Ľ���
    bool IsProcessActive(const char* szType);
    /// �󶨽���
    void Attach(RtProcessItem pProcess);
    /// �Ƿ��н���
    bool HasProcesses();

private:
    /// �������
    void Detach(SmartPtr<RtProcess> pProcess);

protected:
    typedef std::list< SmartPtr<RtProcess> > ProcessList;
    ProcessList m_ProcessList;
};

/*-----------------------------------------------------------------------------
-   RtProcessTimer
-----------------------------------------------------------------------------*/
/** ����������.
*       - ʹ�÷���
*           @code
*           RtProcessItem wait(new RtProcessTimer(1000, 5));
*           processManager.Attach(wait);
*           @endcode
*         ǰ����δ������˼�ǣ�����һ��ÿ1000������Ӧһ�Σ��ܹ�5�εļ�ʱ��
*/
class RtProcessTimer : private RtProcess
{
public:
    /// ��ʼ����ʱ����dwHeartbeatMillisecondsΪ�������ʱ�䣬iCountΪ������-1��ʾ���޴�
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
    virtual void OnTimer(int iTick) { } ///< ����ʱ��ʱ�䵽��ʱ�򴥷����������iTick��ʾ�ǵڼ��δ�������1��iCount
    virtual void OnTimerStart()     { } ///< ����ʱ����ʼ������ʱ�򴥷��������
    virtual void OnTimerEnd()       { } ///< ����ʱ��������ʱ�򴥷��������
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

/** �ȴ�����.
*       - ʹ�÷���
*           @code
*           RtProcessItem wait(new RtProcessWait(3000));
*           processManager.Attach(wait);
*           RtProcessItem sing(new RtProcessPlayPose(pCharactor, "sing"));
*           wait->SetNext(sing);
*           @endcode
*         ǰ����δ������˼�ǣ��ȴ�3���Ͳ���һ�������Pose
*/
class RtProcessWait : private RtProcess
{
public:
    /// ��ʼ���ȴ����̣�dwNumMillisecondsΪ�ȴ���ʱ��
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
