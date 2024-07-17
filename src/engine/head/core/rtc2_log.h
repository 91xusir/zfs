/**
* @file rtc2_log.h ����log.
* @author janhail
*/

namespace rt2_core {

/** Log����豸.
*      ��Ҫ����ɿ��Դ�RtLog������ݣ��ͱ������������
*/
class RtLogOutput
{
public:
    enum Priority
    {
        PR_FATAL  = 0,     ///< ������
        PR_ALERT  ,        ///< ����
        PR_CRIT   ,        ///< 
        PR_ERROR  ,        ///< ����
        PR_WARN   ,        ///< ����
        PR_NOTICE ,        ///< ֪ͨ
        PR_INFO   ,        ///< ��Ϣ
        PR_DEBUG  ,        ///< ����
        RT_ERROR_RES_MISSING, 
        RT_PERF_TICK,
        RT_TEXT,
        PR_TSET              ///< ����

    };
    static const char* GetPriorityName(Priority e)
    {
        const char* szPName[] = {
            "FATAL",
            "ALERT",
            "CRIT",
            "ERROR",
            "WARN",
            "NOTICE",
            "INFO",
            "DEBUG",
            "RES_MISSING",
            "PERF",
            "TEXT",
            "TSET"

        };
        return szPName[e];
    };
    /** Log�¼�.
    */
    struct Event
    {
        /** ����һ��Log�¼�.
        * @param szCategory �¼��������
        * @param szMessage  ��Ϣ
        * @param szFile     �����ļ���
        * @param iLine      �����ļ���������
        * @param ePriority  ����¼������ȼ�
        */

        Event(const RtString& vCategory, const RtString& vMessage, const RtString& vFile, int vLine, Priority vPriority);
#ifndef _DEBUG
        inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
        inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
        inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
        inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
#endif
        RtString        szCategory; ///< �������
        RtString        szMessage;  ///< ��Ϣ
        RtString        szFile;     ///< �����ļ���
        int             iLine;      ///< �����ļ���������
        Priority        ePriority;  ///< ���ȼ�
        DWORD           timeStamp;  ///< ʱ���
    };
    /** �¼�������.
    */
    class Filter
    {
    public:
        enum EDecision
        {   DENY    = -1, ///< �ܾ�
            NEUTRAL = 0,  ///< û�о���
            ACCEPT  = 1   ///< ����
        };

        virtual ~Filter(){}
        virtual EDecision Decide(const Event& event) = 0;
        SmartPtr<RtProcess> pNext;
    };
    /** ��Ϣ�������.
    */
    class Layout
    {
    public:
        virtual ~Layout(){}
        int Format(const Event& event, char* szBuf, int iBufSize)
        {
            if (this==0)
            {
                return BaseFormat(event, szBuf, iBufSize);
            }
            return OnFormat(event, szBuf, iBufSize);
        }
        virtual int OnFormat(const Event& event, char* szBuf, int iBufSize) = 0;
        static int BaseFormat(const Event& event, char* szBuf, int iBufSize);
#ifndef _DEBUG
        inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
        inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
        inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
        inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
#endif
    };
    /** �����������.
    */
    class LayoutBase : public Layout
    {
    public:
        virtual ~LayoutBase(){}
        virtual int OnFormat(const Event& event, char* szBuf, int iBufSize);
    };
    /** ģ���������.
    * - %d ����    ����ʽΪ 20060331
    * - %t ʱ��    ����ʽΪ 09:38:24
    * - %T ����ʱ�䣬��ʽΪ 20060331 09:38:24
    * - %s ��      ����ʽΪ 260842
    * - %m ��Ϣ    ����ʽΪ message
    * - %c ���    ����ʽΪ category
    * - %f λ��    ����ʽΪ rtc2_log.cpp:256
    * - %p ���ȼ�  ����ʽΪ ERROR
    */
    class LayoutPattern : public Layout
    {
    public:
        LayoutPattern(const char* szPattern);
        virtual ~LayoutPattern(){}
        virtual int OnFormat(const Event& event, char* szBuf, int iBufSize);
    public:
        struct Component
        {
            virtual ~Component(){}
            virtual const char* Get(const Event& event) = 0;
#ifndef _DEBUG
            inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
            inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
            inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
            inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
#endif
        };
        struct ComponentString : public Component
        {
            ComponentString(const char* p, int iLen) : szStr((char*)p,iLen)    {}
            virtual ~ComponentString(){}
            virtual const char* Get(const Event& event) { return szStr.c_str(); }
            RtString szStr;
        };
        struct ComponentDate : public Component
        {
            virtual ~ComponentDate(){}
            virtual const char* Get(const Event& event);
        };
        struct ComponentTime : public Component
        {
            virtual ~ComponentTime(){}
            virtual const char* Get(const Event& event);
        };
        struct ComponentDateTime : public Component
        {
            virtual ~ComponentDateTime(){}
            virtual const char* Get(const Event& event);
        };
        struct ComponentSecond : public Component
        {
            virtual ~ComponentSecond(){}
            virtual const char* Get(const Event& event);
        };
        struct ComponentMsg : public Component
        {
            virtual ~ComponentMsg(){}
            virtual const char* Get(const Event& event) { return event.szMessage.c_str(); }
        };
        struct ComponentCategory : public Component
        {
            virtual ~ComponentCategory(){}
            virtual const char* Get(const Event& event) { return event.szCategory.c_str(); }
        };
        struct ComponentPosition : public Component
        {
            virtual ~ComponentPosition(){}
            virtual const char* Get(const Event& event);
        };
        struct ComponentPriority : public Component
        {
            virtual ~ComponentPriority(){}
            virtual const char* Get(const Event& event) { return GetPriorityName(event.ePriority); }
        };
    protected:
        std::list< SmartPtr<Component> > m_listComponent;
    };
protected:
    RtLogOutput(const RtString& szName);

public:
    virtual ~RtLogOutput();
    
    virtual void DoLog(const Event& event)
    {
        if ((m_eThreshold==PR_TSET) || (m_eThreshold>event.ePriority))
        {
            if (!m_pFilter || (m_pFilter->Decide(event) != Filter::DENY))
            {
                Log(event);
            }
        }
    }

    const RtString& GetName()   { return m_szName; }

    /** ������͵���������ȼ�.
    *   ����������ȼ��Ķ����ᱻLog
    *   @param ���õ����ȼ�.
    */
    virtual void SetThreshold(Priority vPriority);

    /** ȡ����͵�������ȼ�.
    * @return ������͵�������ȼ�
    **/
    virtual Priority GetThreshold();

    /** �����¼�������.
    */
    virtual void SetFilter(SmartPtr<Filter> pFilter);

    /** ȡ���¼�������.
    * @return ���ع�����������NULL��ʾ�����ڹ�����
    */
    virtual SmartPtr<Filter> GetFilter();

    /** �������ģ��.
    */
    virtual void SetLayout(SmartPtr<Layout> pLayout);
    virtual void SetBaseLayout();
    virtual void SetPatternLayout(const char* szPattern);

    /** ȡ�����ģ��.
    * @return �������ģ�棬����NULL��ʾ���������ģ�棨����Ĭ�������
    */
    virtual SmartPtr<Layout> GetLayout();
#ifndef _DEBUG
    inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
    inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
    inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
    inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
#endif
protected:
    virtual void Log(const Event& event) = 0;
    
protected:
    RtString            m_szName;
    Priority            m_eThreshold;
    SmartPtr<Filter>    m_pFilter;
    SmartPtr<Layout>    m_pLayout;
};

class RtLog;
typedef SmartPtr<RtLog>         RtLogType;
typedef SmartPtr<RtLogOutput>   RtLogOutputType;
/*
[Log/Sub1]
Output = Console
Output = File, "log/log_file.txt", true
Output = RollingFile, "log/log_file.txt", 10, 1, true
layout = BasicLayout

<log>
    <logger name="sub1" layout="BasicLayout">
        <console/>
        <file name="log/log_file.txt" mode=true/>
        <rollingfile name="log/log_file.txt" size=10 backup=1 append=true/>
        <syslog name="log" host="192.168.0.1" facility=-1 portNumber=-1/>
        <idsa name="log"/>
        <win32debug/>
        <NTEventLog source="test" threshold=""/>
    <logger/>
<log/>
*/

/** ��������ַ�����һЩ�豸��.
*  - RtLog ʹ�÷���.
*      @code
*        RtLog myLog(new RtLogOutFile("mylog.txt", new RtLogOutConsole(new RtLogOutNetware)));
*        myLog.Logf("Time = %s", rtTimestamp());
*      @endcode
*  - ����Core��ʼ����ʱ����Զ���ʼ��һ��Log���ú���RtCoreLog()����
*  - ��Ҫע����ǣ�ʹ��RtLog��GetChildren����������������ֵ���Log�����ھͻᴴ��һ��
*  - ������һЩ���ӵĴ��룬��һЩ����������
*      @code
*        printf("\n�����Զ������ģ��1, ���Ӧ��Ϊ '1msg2:(3'\n");
*        RtCoreLog().GetOutput("console")->SetPatternLayout("1%m2%a3\n");
*        RtCoreLog().Info("msg");
*
*        printf("\n�����Զ������ģ��2, ���Ӧ��Ϊ '1msg:(3'\n");
*        RtCoreLog().GetOutput("console")->SetPatternLayout("1%m%a3\n");
*        RtCoreLog().Info("msg");
*
*        printf("\n����������Log: Sub1��Sub2\n");
*        RtLogType sub1 = RtCoreLog().GetChildren("Sub1");
*        RtLogType sub2 = RtCoreLog().GetChildren("Sub2");
*        RtCoreLog().GetOutput("console")->SetPatternLayout("[root]%c: %m\n"); // �������ģ��Ϊ "���: ��Ϣ"
*
*        printf("\n������Log���ݻ��������Log�У����Ӧ��Ϊ '[root]sub1: msg' �� '[root]sub2: msg'\n");
*        sub1->Info("msg");
*        sub2->Info("msg");
*
*        printf("\n���Դ�����Log�Լ�����������Ӧ��Ϊ '[out1]sub1: msg'��'[root]sub1: msg'\n");
*        sub1->AddOutConsole("console");
*        sub1->GetOutput("console")->SetPatternLayout("[out1]%c: %m\n");
*        sub1->Info("msg");
*
*        printf("\n���Թر���Log���ݻ��������Log�У����Ӧ��Ϊ '[out1]sub1: msg'��'[root]sub2: msg'\n");
*        sub1->SetLogToParent(false);
*        sub1->Info("msg");
*        sub2->Info("msg");
*      @endcode
*/
class RtLog
{
#ifdef ENABLE_LOG
private:
    /// Ĭ�Ϲ��캯��.
    RtLog(const char* szName);
public:
    friend class RtCore;
    /// ��������.
    ~RtLog();

    const RtString& GetName();

    void Fatal(const char* pFmt, ...);
    void Alert(const char* pFmt, ...);
    void Error(const char* pFmt, ...);
    void Warn(const char* pFmt, ...);
    void Notice(const char* pFmt, ...);
    void Info(const char* pFmt, ...);
    void Debug(const char* pFmt, ...);
    void Error_ResMissing(const char* pFmt, ...);
    void Perf(const char* pFmt, ...);
    void Text(int colorIndex, const char* pFmt, ...);

    /** ���һ���ַ���.
    */
    void Log(RtLogOutput::Priority ePriority, const char* pStr);

    /** ���һ��RtString�ַ���.
    */
    void Log(RtLogOutput::Priority ePriority, const RtString& szStr);

    /** ���ո�ʽ���һ���ַ�������ʽ��ο�C��׼���� printf.
    */
    void Logf(RtLogOutput::Priority ePriority, const char* pFmt, ...);

    /** ��pBuffer��������ݰ���ʮ�������������ʽΪ "FD 08 A5 C6 ...".
    */
    void Dump(RtLogOutput::Priority ePriority, const void* pBuffer, size_t iBufSize);

    /** �������ȼ������ڴ����ȼ�����־������¼.
    */
    void SetPriority(RtLogOutput::Priority ePriority);

    RtLogOutputType GetOutput(const RtString& szName);

    /** ���һ������ߣ������������.
    */
    bool AddOutput(RtLogOutputType pOut);

    /** ɾ��һ��������.
    */
    bool RemoveOutput(const RtString& szName);
    void RemoveAllOutputs();

    /** ���һ������ΪszFile������ļ�������������Ե��ö����ͬʱ���������ļ���.
    *      ע�⣺���������������ļ�szFile�����е�����ȫ�����
    */
    bool AddOutFile(const char* szName, const char* szFile, bool bAppend=false);

    /** ���������̨��.
    *      ֻ�ܵ���һ�Σ���ε��ý���Ч
    */
    bool AddOutConsole(const char* szName, int iType=0);

    /** ����������У�ÿ�ε���������ڱ��ش�sPort�˿ڼ���TCP���ӣ�����������������ͻ��������ݷ��͵���������.
    *      ֻ�ܵ���һ�Σ���ε��ý���Ч
    */
    bool AddOutLogServer(const char* szName, int iPortNumber=6900);
    bool AddOutAssert(const char* szName){return false;}
    // iMaxFileSize�ĵ�λΪM.
    bool AddOutRollingFile(const char* szName, const char* szFile, size_t iMaxFileSize=10, unsigned int iMaxBackupIndex=1, bool bAppend=true){return false;}
    bool AddOutNTEventLog(const char* szName, const char* szSourceName){return false;}
    bool AddOutUnixSyslog(const char* szName, const char* szSyslogName, int iFacility=0){return false;} // iFacility=LOG_USER
    bool AddOutRemoteSyslog(const char* szName, const char* szSyslogName, const char* szRelayerHost, int iFacility=0, int iPortNumber=514){return false;}

    bool HasChildren(const RtString& szName);

    /** ע�⣺���������ֵ�Log�����ڣ������Get�����ᴴ��һ��.
    */
    RtLogType GetChildren(const RtString& szName);
    bool AddChildren(RtLogType pChildren);
    bool RemoveChildren(const RtString& szName);
    void RemoveAllChildren();

    void SetLogToParent(bool bLogToParent)  { m_bLogToParent = bLogToParent; }
    bool GetLogToParent()                   { return m_bLogToParent; }

private:
    void _DoLog(RtLogOutput::Priority ePriority, char* pMsg, const char* szFile, int iLine);
    void _DoLogEvent(RtLogOutput::Event event);
    void _Logfv(RtLogOutput::Priority ePriority, const char* pFmt, va_list arg);
    void _Logfv(RtLogOutput::Priority ePriority, int _colorIndex, const char* pFmt, va_list arg);

    RtLogOutput::Priority GetRootPriority();
private:
    RtResLock               m_lock;
    RtString                m_szName;
    RtLogOutput::Priority   m_ePriority;
    bool                    m_bLogToParent;

    RtLog*                              m_pParent;
    std::list< RtLogOutputType >        m_listOutputs;
    std::list< RtLogType >              m_listChildren;

#else
public:
    /** �������һ���ٵ�RtLog��.
    *    ���ر�ENABLE_LOG��ʱ�򣬻���뵽����࣬�����������Ա���ǿպ��������Ա��������Ż���������
    */
    RtLog(){}
    RtLog(RtLogOutput* pOut){}
    ~RtLog(){}
    void Log (const char* pStr)                         {}
    void Log (const RtString& szStr)                    {}
    void Logf(const char* pFmt, ...)                    {}
    void Dump(const void* pBuffer, size_t iBufSize)     {}
    bool AddOutFile(const char* szFile)                 {}
    bool AddOutConsole()                                {}
    bool AddOutNetwork(int sPort=6900)                  {}
    void AddOutput(RtLogOutput* pOut)                   {}
    void AddSingleRecever(RtArchive* pOut, bool bADel)  {}
    void RemoveOutput(RtArchive* pOut)                  {}
#endif
};


} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
