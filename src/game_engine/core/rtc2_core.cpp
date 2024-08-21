/**
* @file 进程管理（非操作系统进程），在这里是指一些可以同步执行的程序
* @author janhail
*/
#include "core/rt2_core.h"
#include "rtc2_archive_ansi.h"
#include "rtc2_memory_instance.h"
#include "rtc2_core_inter.h"

namespace rt2_core {

RtTime::RtTime(bool bNow) {
    UpdateNow();
}

void RtTime::UpdateNow() {
    *this = RtCPU::GetSeconds();
}

/*-----------------------------------------------------------------------------
	-     rtCoreInit(...) & rtCoreExit(...)
	-----------------------------------------------------------------------------*/

bool rtCoreInit(const char* szIniFile, RtRuntimeClass* pCoreFactory) {
    RtCore::Instance();  // 如果这时候没有创建，用这个方法可以触发一次创建
    RtCoreFactory  factory;
    RtCoreFactory* pFactory;
    if (pCoreFactory) {
        pFactory = reinterpret_cast<RtCoreFactory*>(pCoreFactory->CreateObject());
        CHECK(pFactory != NULL);
    } else {
        pFactory = &factory;
    }
    RtCore::Instance().Create(pFactory);
    return RtCore::Instance().Init(szIniFile);
}

bool rtCoreExit() {
    RtCore::Instance().Exit();
    return true;
}

/*-----------------------------------------------------------------------------
	-     RtCoreFactory
	-----------------------------------------------------------------------------*/

RT_IMPLEMENT_DYNCREATE(RtCoreFactory, RtObject, 0, "RtCore")

void RtCoreFactory::InitLogRoot(RtLogType rootLog) {
    rootLog->AddOutConsole("console");
    m_pCore->SetDefaultFileLog();
}

/*-----------------------------------------------------------------------------
	-     RtCore
	-----------------------------------------------------------------------------*/

RtCore*   RtCore::s_pInstance = NULL;
RtResLock RtCore::s_lockSingleton;
bool      RtCore::s_bSingletonDestroyed = false;

RtCore::RtCore() {
    pFileManager = NULL;
    m_bIsDefaultFileLog = false;
}

RtCore::~RtCore() {
    rootLog->RemoveAllChildren();  // 删除所有的子Log，这是为了安全的退出
    rootLog->RemoveAllOutputs();   // 删除所有的输出，这是为了安全的退出
    rootLog = NULL;                // 这是职能指针，所以这句会触发自动删除
    s_bSingletonDestroyed = true;
    s_pInstance = NULL;
}

void RtCore::Create(RtCoreFactory* pFactory) {
    CHECK(pFactory != NULL);
    pFactory->SetCore(this);

    // Buffer
    static char szBuffer2048[2048];
    pBuffer2048 = szBuffer2048;

    // FileManager
    pFileManager = pFactory->CreateFileManager();
    if (pFileManager == NULL) {
        static RtFileManagerAnsi fileManagerAnsi;
        pFileManager = &fileManagerAnsi;
    }

    // Log
    rootLog = CreateLog("root");
    pFactory->InitLogRoot(rootLog);
}

void RtCore::CreateInstance() {
    // 在创建Core之前先激活内存管理，因为只有保证内存管理是最后退出的，他才会正确的检查内存泄露情况
    RtMemoryAllocator::Instance();  // 激活内存管理

    RtResLock::AutoLock lock(s_lockSingleton);
    if (s_pInstance)
        return;
    RTASSERT(s_bSingletonDestroyed ==
             false);  // 这里不能用CHECK，因为CHECK会有信息输出，而信息输出又依赖于Core
    static RtCore core;
    s_pInstance = &core;
}

bool RtCore::Init(const char* szIniFile) {
    rtciPlatformInit();
    PlatformInitInfo();
    cpu.Init();
    RtIni ini;
    if (szIniFile != NULL && ini.OpenFile(szIniFile)) {
        int         iMemBreak;
        bool        bEnable;
        const char* pCoreName = ini.GetEntry("System", "Name");
        if (pCoreName) {
            strName = pCoreName;
        } else {
            strName = "core";
        }
        if (ini.GetEntry("System", "MemBreak", &iMemBreak)) {
            if (iMemBreak) {
                RtCoreMem().SetBreak(iMemBreak);
            }
        }
        if (ini.GetEntry("System", "LeakBreak", &bEnable)) {
            RtCoreMem().SetLeakBreak(bEnable);
        }
        if (ini.GetEntry("System", "DumpMemLeak", &bEnable)) {
            RtCoreMem().SetDumpMemLeak(bEnable);
        }

        ini.CloseFile();
    } else {
        strName = "zfs";
        RtCoreMem().SetBreak(0);
        RtCoreMem().SetLeakBreak(0);
        RtCoreMem().SetDumpMemLeak(0);
    }

    // 为默认的Log设置一个文件输出
    if (m_bIsDefaultFileLog) {
        if (!strName.IsEmpty()) {
            int iYear = 0;
            int iMonth = 0;
            int iDayOfWeek = 0;
            int iDay = 0;
            int iHour = 0;
            int iMin = 0;
            int iSec = 0;
            int iMSec = 0;
            rtSystemTime(iYear, iMonth, iDayOfWeek, iDay, iHour, iMin, iSec, iMSec);
            char buf[40];
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%d%d%d%d%d%d", iYear, iMonth, iDay, iHour, iMin, iSec);
            RtString stime = buf;
            rootLog->AddOutFile("log", RtString("log") + PATH_SEPARATOR_S + strName + "_log" +
                                           stime + ".log");
        }
    }

    // 输出初始化Log
    rootLog->Info("Core initialized.\n");
    return true;
}

void RtCore::Tick(int delta) {}

void RtCore::Exit() {
    rtciPlatformClear();
    rootLog->Info("Core exit.\n");
}

bool RtCore::DoCommand(const char* cmd, const char* param) {
    return true;
}

const char* RtCore::GetName() {
    return strName.c_str();
}

RtLogType RtCore::CreateLog(const char* szName) {
    RtLogType newLog(RT_NEW RtLog(szName));
    return newLog;
}

}  // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
