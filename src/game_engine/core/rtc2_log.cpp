
#include "core/rt2_core.h"

namespace rt2_core {

/*-----------------------------------------------------------------------------
-     RtLogOutput
-----------------------------------------------------------------------------*/
RtLogOutput::RtLogOutput(const RtString& szName)
{
    m_szName = szName;
    m_eThreshold = PR_TSET;
    m_pFilter = NULL;
	SetBaseLayout();
}

RtLogOutput::~RtLogOutput()
{
    m_pFilter = NULL;
}

void RtLogOutput::SetThreshold(Priority vPriority)
{
    m_eThreshold = vPriority;
}

RtLogOutput::Priority RtLogOutput::GetThreshold()
{
    return m_eThreshold;
}

void RtLogOutput::SetFilter(SmartPtr<Filter> pFilter)
{
    m_pFilter = pFilter;
}

SmartPtr<RtLogOutput::Filter> RtLogOutput::GetFilter()
{
    return m_pFilter;
}

void RtLogOutput::SetLayout(SmartPtr<Layout> pLayout)
{
    m_pLayout = pLayout;
}

void RtLogOutput::SetBaseLayout()
{
    m_pLayout = RT_NEW LayoutBase;
}

void RtLogOutput::SetPatternLayout(const char* szPattern)
{
    m_pLayout = RT_NEW LayoutPattern(szPattern);
}

SmartPtr<RtLogOutput::Layout> RtLogOutput::GetLayout()
{
    return m_pLayout;
}

/*-----------------------------------------------------------------------------
-     RtLogOutput::Layout::BaseFormat
-----------------------------------------------------------------------------*/

int RtLogOutput::Layout::BaseFormat(const Event& event, char* szBuf, int iBufSize)
{
    RtLocateTime t(event.timeStamp);
	int iBufferSize = rt2_snprintf(szBuf, iBufSize,
        "[%04d%02d%02d %02d:%02d:%02d %s %s %s:%d] %s",
        t.iYear, t.iMonth, t.iDay, t.iHour, t.iMin, t.iSec,
        RtLogOutput::GetPriorityName(event.ePriority),
        event.szCategory.c_str(),
        event.szFile.c_str(),
        event.iLine,
        event.szMessage.c_str()
        );
    szBuf[iBufSize-1] = 0;
    if (iBufferSize<0) iBufferSize = iBufSize-1;
    return iBufferSize;
}

/*-----------------------------------------------------------------------------
-     RtLogOutput::LayoutBase
-----------------------------------------------------------------------------*/

int RtLogOutput::LayoutBase::OnFormat(const Event& event, char* szBuf, int iBufSize)
{
    RtLocateTime t(event.timeStamp);
	int iBufferSize = rt2_snprintf(szBuf, iBufSize,
        "[%04d%02d%02d-%02d:%02d:%02d %s] %s",
        t.iYear, t.iMonth, t.iDay, t.iHour, t.iMin, t.iSec,
        RtLogOutput::GetPriorityName(event.ePriority),
        //event.szCategory.c_str(),
        event.szMessage.c_str()
        );
    szBuf[iBufSize-1] = 0;
    if (iBufferSize<0) iBufferSize = iBufSize-1;
    return iBufferSize;
}

/*-----------------------------------------------------------------------------
-     RtLogOutput::LayoutPattern
-----------------------------------------------------------------------------*/

RtLogOutput::LayoutPattern::LayoutPattern(const char* szPattern)
{
    int i, iS=0, iLen = (int)strlen(szPattern);
    for (i=0; i<iLen; i++)
    {
        if (szPattern[i]=='%')
        {
            if (i+2<iLen)
            {
                SmartPtr<Component> ptr;
                switch (szPattern[i+1])
                {
                case 'd': ptr = RT_NEW ComponentDate; break;
                case 't': ptr = RT_NEW ComponentTime; break;
                case 'T': ptr = RT_NEW ComponentDateTime; break;
                case 's': ptr = RT_NEW ComponentSecond; break;
                case 'm': ptr = RT_NEW ComponentMsg; break;
                case 'c': ptr = RT_NEW ComponentCategory; break;
                case 'f': ptr = RT_NEW ComponentPosition; break;
                case 'p': ptr = RT_NEW ComponentPriority; break;
                default:  ptr = RT_NEW ComponentString(":(", 2); break;
                }
                if (i>iS)
                {
                    SmartPtr<Component> str( RT_NEW ComponentString(szPattern+iS,i-iS));
                    m_listComponent.push_back(str);
                }
                m_listComponent.push_back(ptr);
                i ++;
                iS = i+1;
            }
        }
    }
    if (i>iS)
    {
        SmartPtr<Component> str( RT_NEW ComponentString(szPattern+iS,i-iS));
        m_listComponent.push_back(str);
    }
}

int RtLogOutput::LayoutPattern::OnFormat(const Event& event, char* szBuf, int iBufSize)
{
    char* p = szBuf;
    int l, i = iBufSize;
    std::list< SmartPtr<Component> >::iterator it = m_listComponent.begin();
    for (; it!=m_listComponent.end(); it++)
    {
 /*       strncpy(p, (*it)->Get(event), i);
        p[i-1] = 0;*/
		rt2_strncpy(p, (*it)->Get(event), i);
        l = (int)strlen(p);
        i -= l;
        p += l;
    }
    return iBufSize-i;
}

const char* RtLogOutput::LayoutPattern::ComponentDate::Get(const Event& event)
{
    static char szBuf[40];
    RtLocateTime t(event.timeStamp);
    rt2_snprintf(szBuf, 40, "%04d%02d%02d", t.iYear, t.iMonth, t.iDay);
    szBuf[39] = 0;
    return szBuf;
}

const char* RtLogOutput::LayoutPattern::ComponentTime::Get(const Event& event)
{
    static char szBuf[40];
    RtLocateTime t(event.timeStamp);
    rt2_snprintf(szBuf, 40, "%02d:%02d:%02d", t.iHour, t.iMin, t.iSec);
    szBuf[39] = 0;
    return szBuf;
}

const char* RtLogOutput::LayoutPattern::ComponentDateTime::Get(const Event& event)
{
    static char szBuf[40];
    RtLocateTime t(event.timeStamp);
    rt2_snprintf(szBuf, 40, "%04d%02d%02d %02d:%02d:%02d", t.iYear, t.iMonth, t.iDay, t.iHour, t.iMin, t.iSec);
    szBuf[39] = 0;
    return szBuf;
}

const char* RtLogOutput::LayoutPattern::ComponentSecond::Get(const Event& event)
{
    static char szBuf[20];
    rt2_snprintf(szBuf, 20, "%d", event.timeStamp);
    szBuf[19] = 0;
    return szBuf;
}

const char* RtLogOutput::LayoutPattern::ComponentPosition::Get(const Event& event)
{
    static char szBuf[120];
    rt2_snprintf(szBuf, 120, "%s:%d", event.szFile.c_str(), event.iLine);
    szBuf[119] = 0;
    return szBuf;
}

/*-----------------------------------------------------------------------------
-     RtLogOutput::Event
-----------------------------------------------------------------------------*/

RtLogOutput::Event::Event(const RtString& vCategory, const RtString& vMessage, const RtString& vFile, int vLine, Priority vPriority) :
    szCategory(vCategory),
    szMessage(vMessage),
    szFile(vFile),
    iLine(vLine),
    ePriority(vPriority)
{
    timeStamp  = rtTimestamp();
}

/*-----------------------------------------------------------------------------
-     RtLogOutputConsole
-----------------------------------------------------------------------------*/

class RtLogOutputConsole : public RtLogOutput
{
public:
    RtLogOutputConsole(const RtString& szName, WORD wColor) : RtLogOutput(szName)
    {
        m_pAr = NULL;
        m_wColorAttr = wColor;
#ifdef WIN32
        AllocConsole();
        SetConsoleTitle("Debug Window");
        m_hWin32StdOutConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD co = {800 , 5000};
        SetConsoleScreenBufferSize(m_hWin32StdOutConsole, co);
        freopen("CONOUT$","w+t",stdout);
	    freopen("CONOUT$","w+t",stderr);
        freopen("CONIN$","r+t",stdin);

        typedef BOOL (WINAPI *SetConsoleFont_t)(HANDLE, DWORD);
        SetConsoleFont_t _SetConsoleFont = NULL;
        _SetConsoleFont = (SetConsoleFont_t)::GetProcAddress(::GetModuleHandle(("kernel32")), "SetConsoleFont");
     
        if (_SetConsoleFont)
            _SetConsoleFont(m_hWin32StdOutConsole, 0xA);


#endif
        m_pAr = RtCoreFile().CreateFileWriter(NULL, "stdout", RtFileManager::FW_Text);
    }
    virtual ~RtLogOutputConsole()
    {
        CHECK(m_pAr!=NULL);
        if (m_pAr)
        {
            RtCoreFile().CloseFile(m_pAr);
            m_pAr = NULL;
        }
    }

protected:
    RtArchive*  m_pAr;
    HANDLE      m_hWin32StdOutConsole;
    WORD        m_wColorAttr;

public:
#ifdef WIN32
    virtual void Log(const Event& event)
    {
        const size_t buf_Size = 1024;
        char szBuf[buf_Size];
        
        int iRet = 0;

        if (event.ePriority == RT_TEXT)
        {
            iRet = (int)strlen(event.szMessage) + 1;
            if (iRet > buf_Size)
                iRet = buf_Size;
            //strncpy(szBuf, event.szMessage.c_str(), iRet);
			rt2_strncpy(szBuf, event.szMessage.c_str(), buf_Size);
        }
        else
            iRet = m_pLayout->Format(event, szBuf, buf_Size);

        static WORD lastColor = 15;
        WORD wColor = 8;

        switch (event.ePriority)
        {
        case PR_FATAL :
        case PR_ERROR :
            wColor = 4;
            break;
        case RT_ERROR_RES_MISSING :
            wColor = 6;
            break;
        case PR_ALERT :
        case PR_WARN :
            wColor = 5;
            break;
		case PR_NOTICE :
			wColor = 2;
			break;
        case PR_INFO :
            wColor = 8;
            break;
        case RT_TEXT :
            wColor = event.iLine;
            break;
        case RT_PERF_TICK :
        case PR_DEBUG :
        case PR_TSET :
            wColor = 3;
            break;
        default :
            break;
        }
        
        if (lastColor != wColor)
            SetConsoleTextAttribute(m_hWin32StdOutConsole, lastColor = wColor);
        m_pAr->Serialize(szBuf, iRet);
    }
#else
    virtual void Log(const Event& event)
    {
        char szBuf[1224];
        int iRet = m_pLayout->Format(event, szBuf, 1224);
        m_pAr->Serialize(szBuf, iRet);
        m_pAr->Flush();
    }
#endif
};

/*-----------------------------------------------------------------------------
-     RtLogOutputFile
-----------------------------------------------------------------------------*/

class RtLogOutputFile : public RtLogOutput
{
public:
    RtLogOutputFile(const RtString& szName, const char* szFileName, bool bAppend = true) : RtLogOutput(szName)
    {
        m_szFilename = szFileName;
        m_bAppend = bAppend;

        int iAttr = RtFileManager::FW_Text;
        if (m_bAppend) iAttr |= RtFileManager::FW_Append;
        m_pAr = RtCoreFile().CreateFileWriter(NULL, m_szFilename, iAttr);
    }
    virtual ~RtLogOutputFile()
    {
        CHECK(m_pAr!=NULL);
        if (m_pAr)
        {
            RtCoreFile().CloseFile(m_pAr);
            m_pAr = NULL;
        }
    }
    virtual void Log(const Event& event)
    {
        char szBuf[1224];
        int iRet = m_pLayout->Format(event, szBuf, 1224);
        m_pAr->Serialize(szBuf, iRet);
        m_pAr->Flush();
    }

protected:
    RtArchive*  m_pAr;
    RtString    m_szFilename;
    bool        m_bAppend;

};

/*-----------------------------------------------------------------------------
-     RtLog
-----------------------------------------------------------------------------*/

#ifdef ENABLE_LOG

RtLog::RtLog(const char* szName)
{
    m_pParent = 0;
    m_szName = szName;
    m_ePriority = RtLogOutput::PR_TSET;
    m_bLogToParent = true;
}

RtLog::~RtLog()
{
    m_pParent = 0;
    m_listOutputs.clear();
    m_listChildren.clear();
}

const RtString& RtLog::GetName()
{
    return m_szName;
}

void RtLog::_DoLogEvent(RtLogOutput::Event event)
{
    if (!m_listOutputs.empty())
    {
        std::list< RtLogOutputType >::iterator it;
        for(it = m_listOutputs.begin(); it != m_listOutputs.end(); it++)
        {
            (*it)->DoLog(event);
        }
    }
    if (m_bLogToParent && m_pParent)
    {
        m_pParent->_DoLogEvent(event);
    }
}

void RtLog::_DoLog(RtLogOutput::Priority ePriority, char* pMsg, const char* szFile, int iLine)
{
    if (m_lock.TryLock())
    {
        RtLogOutput::Event event(GetName(), pMsg, szFile, iLine, ePriority);
        _DoLogEvent(event);
        m_lock.Unlock();
    }
}

static const int s_iLogBufferSize = 1024;

void RtLog::Fatal(const char* pFmt, ...)
{
    if ((GetRootPriority() >= RtLogOutput::PR_FATAL))
    {
	    va_list argList; va_start(argList, pFmt);
        _Logfv(RtLogOutput::PR_FATAL, pFmt, argList);
	    va_end(argList);
    }
}

void RtLog::Alert(const char* pFmt, ...)
{
    if ((GetRootPriority() >= RtLogOutput::PR_ALERT))
    {
	    va_list argList; va_start(argList, pFmt);
        _Logfv(RtLogOutput::PR_ALERT, pFmt, argList);
	    va_end(argList);
    }
}

void RtLog::Error(const char* pFmt, ...)
{
    if ((GetRootPriority() >= RtLogOutput::PR_ERROR))
    {
	    va_list argList; va_start(argList, pFmt);
        _Logfv(RtLogOutput::PR_ERROR, pFmt, argList);
	    va_end(argList);
    }
}

void RtLog::Warn(const char* pFmt, ...)
{
    if ((GetRootPriority() >= RtLogOutput::PR_WARN))
    {
	    va_list argList; va_start(argList, pFmt);
        _Logfv(RtLogOutput::PR_WARN, pFmt, argList);
	    va_end(argList);
    }
}

void RtLog::Notice(const char* pFmt, ...)
{
    if ((GetRootPriority() >= RtLogOutput::PR_NOTICE))
    {
	    va_list argList; va_start(argList, pFmt);
        _Logfv(RtLogOutput::PR_NOTICE, pFmt, argList);
	    va_end(argList);
    }
}

void RtLog::Info(const char* pFmt, ...)
{
    if ((GetRootPriority() >= RtLogOutput::PR_INFO))
    {
	    va_list argList; va_start(argList, pFmt);
        _Logfv(RtLogOutput::PR_INFO, pFmt, argList);
	    va_end(argList);
    }
}

void RtLog::Debug(const char* pFmt, ...)
{
    if ((GetRootPriority() >= RtLogOutput::PR_DEBUG))
    {
	    va_list argList; va_start(argList, pFmt);
        _Logfv(RtLogOutput::PR_DEBUG, pFmt, argList);
	    va_end(argList);
    }
}

void RtLog::Error_ResMissing(const char* pFmt, ...)
{
    if ((GetRootPriority() >= RtLogOutput::RT_ERROR_RES_MISSING))
    {
        va_list argList; va_start(argList, pFmt);
        _Logfv(RtLogOutput::RT_ERROR_RES_MISSING, pFmt, argList);
        va_end(argList);
    }
}

void RtLog::Perf(const char* pFmt, ...)
{
    if ((GetRootPriority() >= RtLogOutput::RT_PERF_TICK))
    {
        va_list argList; va_start(argList, pFmt);
        _Logfv(RtLogOutput::RT_PERF_TICK, pFmt, argList);
        va_end(argList);
    }
}

void RtLog::Text(int colorIndex, const char* pFmt, ...)
{
    if ((GetRootPriority() >= RtLogOutput::RT_TEXT))
    {
        va_list argList; va_start(argList, pFmt);
        _Logfv(RtLogOutput::RT_TEXT, colorIndex, pFmt, argList);
        va_end(argList);
    }
}

void RtLog::Log(RtLogOutput::Priority ePriority, const char* pStr)
{
    if ((GetRootPriority() >= RtLogOutput::PR_ERROR))
    {
        size_t len = strlen(pStr)+1;
        char szLogBuffer[s_iLogBufferSize];
        if (len>(s_iLogBufferSize))
            len = s_iLogBufferSize;
        memcpy(szLogBuffer, pStr, len);
        szLogBuffer[len-1] = 0;
        _DoLog(ePriority, szLogBuffer, "test", 0);
    }
}

void RtLog::Log(RtLogOutput::Priority ePriority, const RtString& szStr)
{
    if ((GetRootPriority() >= RtLogOutput::PR_ERROR))
    {
        size_t len = szStr.GetLength()+1;
        char szLogBuffer[s_iLogBufferSize];
        if (len>(s_iLogBufferSize))
            len = s_iLogBufferSize;
        memcpy(szLogBuffer, szStr.c_str(), len);
        szLogBuffer[len-1] = 0;
        _DoLog(ePriority, szLogBuffer, "test", 0);
    }
}

void RtLog::Logf(RtLogOutput::Priority ePriority, const char* pFmt, ...)
{
    if ((GetRootPriority() >= RtLogOutput::PR_ERROR))
    {
	    va_list argList;
	    va_start(argList, pFmt);
        _Logfv(ePriority, pFmt, argList);
	    va_end(argList);
    }
}

void RtLog::_Logfv(RtLogOutput::Priority ePriority, int _colorIndex, const char* pFmt, va_list arg)
{
    char szLogBuffer[s_iLogBufferSize];
    int iBufferSize = vsnprintf(szLogBuffer, s_iLogBufferSize, pFmt, arg);
    if (iBufferSize<0)  iBufferSize = s_iLogBufferSize;
    szLogBuffer[s_iLogBufferSize-1] = 0;
    _DoLog(ePriority, szLogBuffer, "test", _colorIndex);
}


void RtLog::_Logfv(RtLogOutput::Priority ePriority, const char* pFmt, va_list arg)
{
    char szLogBuffer[s_iLogBufferSize];
	int iBufferSize = vsnprintf(szLogBuffer, s_iLogBufferSize, pFmt, arg);
    if (iBufferSize<0)  iBufferSize = s_iLogBufferSize;
    szLogBuffer[s_iLogBufferSize-1] = 0;
    _DoLog(ePriority, szLogBuffer, "test", 0);
}

void RtLog::Dump(RtLogOutput::Priority ePriority, const void* pBuffer, size_t iBufSize)
{
    if ((GetRootPriority() >= RtLogOutput::PR_ERROR))
    {
        if (iBufSize>((s_iLogBufferSize)/3-1))
        {
            iBufSize = (s_iLogBufferSize)/3-1;
        }

        char szLogBuffer[s_iLogBufferSize];
        const char* szBuf = (const char*)pBuffer;
        char* pBuf = szLogBuffer;
        size_t i;
        for (i=0; i<iBufSize; i++)
        {
            sprintf(pBuf, "%02X ", *szBuf);
            szBuf ++;
            pBuf += 3;
        }
        pBuf[0] = 0;
        _DoLog(ePriority, szLogBuffer, "test", 0);
    }
}


bool RtLog::AddOutput(RtLogOutputType pOut)
{
    RtResLock::AutoLock lock(m_lock);
    RtLogOutputType f = GetOutput(pOut->GetName());
    if (!f)
    {
        m_listOutputs.push_back(pOut);
        return true;
    }
    return false;
}

RtLogOutputType RtLog::GetOutput(const RtString& szName)
{
    std::list< RtLogOutputType >::iterator it;
    for (it=m_listOutputs.begin(); it!=m_listOutputs.end(); it++)
    {
        if ((*it)->GetName()==szName)
        {
            return *it;
        }
    }
    return NULL;
}

bool RtLog::RemoveOutput(const RtString& szName)
{
    RtResLock::AutoLock lock(m_lock);
    std::list< RtLogOutputType >::iterator it;
    for (it=m_listOutputs.begin(); it!=m_listOutputs.end(); it++)
    {
        if ((*it)->GetName()==szName)
        {
            m_listOutputs.erase(it);
            return true;
        }
    }
    return false;
}

void RtLog::RemoveAllOutputs()
{
    RtResLock::AutoLock lock(m_lock);
    m_listOutputs.clear();
}

bool RtLog::AddOutFile(const char* szName, const char* szFile, bool bAppend)
{
    RtLogOutputType outFile( RT_NEW RtLogOutputFile(szName, szFile, bAppend));
    AddOutput(outFile);
    return true;
}
//lyymark [core-log]
bool RtLog::AddOutConsole(const char* szName, int iType)
{
    RtLogOutputType outConsole( RT_NEW RtLogOutputConsole(szName, iType));
    AddOutput(outConsole);
    return true;
}

bool RtLog::AddOutLogServer(const char* szName, int iPortNumber)
{
    return false;
}

bool RtLog::HasChildren(const RtString& szName)
{
    std::list< RtLogType >::iterator it;
    for (it=m_listChildren.begin(); it!=m_listChildren.end(); it++)
    {
        if ((*it)->GetName()==szName)
        {
            return true;
        }
    }
    return false;
}

RtLogType RtLog::GetChildren(const RtString& szName)
{
    std::list< RtLogType >::iterator it;
    for (it=m_listChildren.begin(); it!=m_listChildren.end(); it++)
    {
        if ((*it)->GetName()==szName)
        {
            return *it;
        }
    }
    RtLogType newLog = RT_NEW RtLog(szName);
    AddChildren(newLog);
    return newLog;
}

bool RtLog::AddChildren(RtLogType pChildren)
{
    if (!HasChildren(pChildren->GetName()))
    {
        pChildren->m_pParent = this;
        m_listChildren.push_back(pChildren);
        return true;
    }
    return false;
}

bool RtLog::RemoveChildren(const RtString& szName)
{
    std::list< RtLogType >::iterator it;
    for (it=m_listChildren.begin(); it!=m_listChildren.end(); it++)
    {
        if ((*it)->GetName()==szName)
        {
            (*it)->m_pParent = NULL;
            m_listChildren.erase(it);
            return true;
        }
    }
    return false;
}

void RtLog::RemoveAllChildren()
{
    std::list< RtLogType >::iterator it;
    for (it=m_listChildren.begin(); it!=m_listChildren.end(); it++)
    {
        (*it)->m_pParent = NULL;
    }
    m_listChildren.clear();
}

RtLogOutput::Priority RtLog::GetRootPriority()
{
    if (m_pParent)
    {
        return m_pParent->GetRootPriority();
    }
    return m_ePriority;
}

#endif // ENABLE_LOG
} // namespace rt2_core

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
