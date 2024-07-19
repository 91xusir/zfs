/**
* @file rtc2_log.h 负责log.
* @author janhail
*/

namespace rt2_core {

/** Log输出设备.
*      需要构造成可以从RtLog输出内容，就必须重载这个类
*/
class RtLogOutput
{
public:
    enum Priority
    {
        PR_FATAL  = 0,     ///< 致命的
        PR_ALERT  ,        ///< 警告
        PR_CRIT   ,        ///< 
        PR_ERROR  ,        ///< 错误
        PR_WARN   ,        ///< 警告
        PR_NOTICE ,        ///< 通知
        PR_INFO   ,        ///< 信息
        PR_DEBUG  ,        ///< 调试
        RT_ERROR_RES_MISSING, 
        RT_PERF_TICK,
        RT_TEXT,
        PR_TSET              ///< 测试

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
    /** Log事件.
    */
    struct Event
    {
        /** 构造一个Log事件.
        * @param szCategory 事件类别名字
        * @param szMessage  消息
        * @param szFile     调用文件名
        * @param iLine      调用文件代码行数
        * @param ePriority  这个事件的优先级
        */

        Event(const RtString& vCategory, const RtString& vMessage, const RtString& vFile, int vLine, Priority vPriority);
#ifndef _DEBUG
        inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
        inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
        inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
        inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
#endif
        RtString        szCategory; ///< 类别名字
        RtString        szMessage;  ///< 消息
        RtString        szFile;     ///< 调用文件名
        int             iLine;      ///< 调用文件代码行数
        Priority        ePriority;  ///< 优先级
        DWORD           timeStamp;  ///< 时间戳
    };
    /** 事件过滤器.
    */
    class Filter
    {
    public:
        enum EDecision
        {   DENY    = -1, ///< 拒绝
            NEUTRAL = 0,  ///< 没有决定
            ACCEPT  = 1   ///< 接受
        };

        virtual ~Filter(){}
        virtual EDecision Decide(const Event& event) = 0;
        SmartPtr<RtProcess> pNext;
    };
    /** 信息输出布局.
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
    /** 基本输出布局.
    */
    class LayoutBase : public Layout
    {
    public:
        virtual ~LayoutBase(){}
        virtual int OnFormat(const Event& event, char* szBuf, int iBufSize);
    };
    /** 模版输出布局.
    * - %d 日期    ，格式为 20060331
    * - %t 时间    ，格式为 09:38:24
    * - %T 日期时间，格式为 20060331 09:38:24
    * - %s 秒      ，格式为 260842
    * - %m 信息    ，格式为 message
    * - %c 类别    ，格式为 category
    * - %f 位置    ，格式为 rtc2_log.cpp:256
    * - %p 优先级  ，格式为 ERROR
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

    /** 设置最低的输出的优先级.
    *   低于这个优先级的都不会被Log
    *   @param 设置的优先级.
    */
    virtual void SetThreshold(Priority vPriority);

    /** 取得最低的输出优先级.
    * @return 返回最低的输出优先级
    **/
    virtual Priority GetThreshold();

    /** 设置事件过滤器.
    */
    virtual void SetFilter(SmartPtr<Filter> pFilter);

    /** 取得事件过滤器.
    * @return 返回过滤器，返回NULL表示不存在过滤器
    */
    virtual SmartPtr<Filter> GetFilter();

    /** 设置输出模版.
    */
    virtual void SetLayout(SmartPtr<Layout> pLayout);
    virtual void SetBaseLayout();
    virtual void SetPatternLayout(const char* szPattern);

    /** 取得输出模版.
    * @return 返回输出模版，返回NULL表示不存在输出模版（按照默认输出）
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

/** 用于输出字符串到一些设备中.
*  - RtLog 使用方法.
*      @code
*        RtLog myLog(new RtLogOutFile("mylog.txt", new RtLogOutConsole(new RtLogOutNetware)));
*        myLog.Logf("Time = %s", rtTimestamp());
*      @endcode
*  - 由于Core初始化的时候会自动初始化一个Log，用函数RtCoreLog()返回
*  - 需要注意的是，使用RtLog的GetChildren函数，如果输入名字的子Log不存在就会创建一个
*  - 下面有一些例子的代码，是一些基本的用例
*      @code
*        printf("\n测试自定义输出模版1, 输出应该为 '1msg2:(3'\n");
*        RtCoreLog().GetOutput("console")->SetPatternLayout("1%m2%a3\n");
*        RtCoreLog().Info("msg");
*
*        printf("\n测试自定义输出模版2, 输出应该为 '1msg:(3'\n");
*        RtCoreLog().GetOutput("console")->SetPatternLayout("1%m%a3\n");
*        RtCoreLog().Info("msg");
*
*        printf("\n创建两个子Log: Sub1和Sub2\n");
*        RtLogType sub1 = RtCoreLog().GetChildren("Sub1");
*        RtLogType sub2 = RtCoreLog().GetChildren("Sub2");
*        RtCoreLog().GetOutput("console")->SetPatternLayout("[root]%c: %m\n"); // 设置输出模版为 "类别: 信息"
*
*        printf("\n测试子Log内容会输出到父Log中，输出应该为 '[root]sub1: msg' 和 '[root]sub2: msg'\n");
*        sub1->Info("msg");
*        sub2->Info("msg");
*
*        printf("\n测试创建子Log自己的输出，输出应该为 '[out1]sub1: msg'和'[root]sub1: msg'\n");
*        sub1->AddOutConsole("console");
*        sub1->GetOutput("console")->SetPatternLayout("[out1]%c: %m\n");
*        sub1->Info("msg");
*
*        printf("\n测试关闭子Log内容会输出到父Log中，输出应该为 '[out1]sub1: msg'和'[root]sub2: msg'\n");
*        sub1->SetLogToParent(false);
*        sub1->Info("msg");
*        sub2->Info("msg");
*      @endcode
*/
class RtLog
{
#ifdef ENABLE_LOG
private:
    /// 默认构造函数.
    RtLog(const char* szName);
public:
    friend class RtCore;
    /// 析构函数.
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

    /** 输出一个字符串.
    */
    void Log(RtLogOutput::Priority ePriority, const char* pStr);

    /** 输出一个RtString字符串.
    */
    void Log(RtLogOutput::Priority ePriority, const RtString& szStr);

    /** 按照格式输出一个字符串，格式请参考C标准函数 printf.
    */
    void Logf(RtLogOutput::Priority ePriority, const char* pFmt, ...);

    /** 把pBuffer里面的数据按照十六进制输出，格式为 "FD 08 A5 C6 ...".
    */
    void Dump(RtLogOutput::Priority ePriority, const void* pBuffer, size_t iBufSize);

    /** 设置优先级，低于此优先级的日志不被记录.
    */
    void SetPriority(RtLogOutput::Priority ePriority);

    RtLogOutputType GetOutput(const RtString& szName);

    /** 添加一组接收者，根据链表添加.
    */
    bool AddOutput(RtLogOutputType pOut);

    /** 删除一个接收者.
    */
    bool RemoveOutput(const RtString& szName);
    void RemoveAllOutputs();

    /** 添加一个名字为szFile的输出文件，这个函数可以调用多次以同时输出到多个文件中.
    *      注意：调用这个函数会把文件szFile本来有的内容全部清空
    */
    bool AddOutFile(const char* szName, const char* szFile, bool bAppend=false);

    /** 输出到控制台中.
    *      只能调用一次，多次调用将无效
    */
    bool AddOutConsole(const char* szName, int iType=0);

    /** 输出到网络中，每次调用这个会在本地打开sPort端口监听TCP连接，如果有连接连上来就会把输出内容发送到该连接中.
    *      只能调用一次，多次调用将无效
    */
    bool AddOutLogServer(const char* szName, int iPortNumber=6900);
    bool AddOutAssert(const char* szName){return false;}
    // iMaxFileSize的单位为M.
    bool AddOutRollingFile(const char* szName, const char* szFile, size_t iMaxFileSize=10, unsigned int iMaxBackupIndex=1, bool bAppend=true){return false;}
    bool AddOutNTEventLog(const char* szName, const char* szSourceName){return false;}
    bool AddOutUnixSyslog(const char* szName, const char* szSyslogName, int iFacility=0){return false;} // iFacility=LOG_USER
    bool AddOutRemoteSyslog(const char* szName, const char* szSyslogName, const char* szRelayerHost, int iFacility=0, int iPortNumber=514){return false;}

    bool HasChildren(const RtString& szName);

    /** 注意：如果这个名字的Log不存在，那这个Get操作会创建一个.
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
    /** 这个类是一个假的RtLog类.
    *    当关闭ENABLE_LOG的时候，会编译到这个类，但是由于类成员都是空函数，所以编译器会优化掉本调用
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
