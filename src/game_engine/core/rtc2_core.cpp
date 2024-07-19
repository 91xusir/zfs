/**
* @file ���̹������ǲ���ϵͳ���̣�����������ָһЩ����ͬ��ִ�еĳ���
* @author janhail
*/
#include "core/rt2_core.h"
#include "rtc2_archive_ansi.h"
#include "rtc2_memory_instance.h"
#include "rtc2_core_inter.h"


namespace rt2_core {

RtTime::RtTime(bool bNow)
{
    UpdateNow();
}

void RtTime::UpdateNow()
{
    *this = RtCPU::GetSeconds();
}

/*-----------------------------------------------------------------------------
-     rtCoreInit(...) & rtCoreExit(...)
-----------------------------------------------------------------------------*/

bool rtCoreInit(const char* szIniFile, RtRuntimeClass* pCoreFactory)
{
    RtCore::Instance(); // �����ʱ��û�д�����������������Դ���һ�δ���
    RtCoreFactory factory;
    RtCoreFactory* pFactory;
    if (pCoreFactory)
    {
        pFactory = reinterpret_cast<RtCoreFactory*>(pCoreFactory->CreateObject());
        CHECK(pFactory!=NULL);
    }else
    {
        pFactory = &factory;
    }
    RtCore::Instance().Create(pFactory);
    return RtCore::Instance().Init(szIniFile);
}

bool rtCoreExit()
{
    RtCore::Instance().Exit();
    return true;
}

/*-----------------------------------------------------------------------------
-     RtCoreFactory
-----------------------------------------------------------------------------*/

RT_IMPLEMENT_DYNCREATE(RtCoreFactory, RtObject, 0, "RtCore")

void RtCoreFactory::InitLogRoot(RtLogType rootLog)
{
    rootLog->AddOutConsole("console");
    m_pCore->SetDefaultFileLog();
}

/*-----------------------------------------------------------------------------
-     RtCore
-----------------------------------------------------------------------------*/

RtCore*      RtCore::s_pInstance = NULL;
RtResLock    RtCore::s_lockSingleton;
bool         RtCore::s_bSingletonDestroyed = false;

RtCore::RtCore()
{
    pFileManager = NULL;
    m_bIsDefaultFileLog = false;
}

RtCore::~RtCore()
{
    rootLog->RemoveAllChildren();   // ɾ�����е���Log������Ϊ�˰�ȫ���˳�
    rootLog->RemoveAllOutputs();    // ɾ�����е����������Ϊ�˰�ȫ���˳�
    rootLog = NULL;                 // ����ְ��ָ�룬�������ᴥ���Զ�ɾ��
    s_bSingletonDestroyed = true;
    s_pInstance = NULL;
}

void RtCore::Create(RtCoreFactory* pFactory)
{
    CHECK(pFactory!=NULL);
    pFactory->SetCore(this);

    // Buffer
    static char szBuffer2048[2048];
    pBuffer2048 = szBuffer2048;

    // FileManager
    pFileManager = pFactory->CreateFileManager();
    if (pFileManager==NULL)
    {
        static RtFileManagerAnsi fileManagerAnsi;
        pFileManager = &fileManagerAnsi;
    }

    // Log
    rootLog = CreateLog("root");
    pFactory->InitLogRoot(rootLog);
}

void RtCore::CreateInstance()
{
    // �ڴ���Core֮ǰ�ȼ����ڴ��������Ϊֻ�б�֤�ڴ����������˳��ģ����Ż���ȷ�ļ���ڴ�й¶���
    RtMemoryAllocator::Instance(); // �����ڴ����

    RtResLock::AutoLock lock(s_lockSingleton);
    if (s_pInstance)
        return;
    RTASSERT(s_bSingletonDestroyed==false); // ���ﲻ����CHECK����ΪCHECK������Ϣ���������Ϣ�����������Core
    static RtCore                       core;
    s_pInstance = &core;
}

bool RtCore::Init(const char* szIniFile)
{
    rtciPlatformInit();
    PlatformInitInfo();
    cpu.Init();

	if (szIniFile == NULL)
	{
		// Use default to start core
		;
	}
	else
	{
		RtIni ini;
		if (!ini.OpenFile(szIniFile))
			return false;

		int iMemBreak;
		bool bEnable;
		const char* pCoreName = ini.GetEntry("System", "Name");
		if (pCoreName)
		{
			strName = pCoreName;
		}else
		{
			strName = "core";
		}
		if (ini.GetEntry("System", "MemBreak", &iMemBreak))
		{
			if (iMemBreak)
			{
				RtCoreMem().SetBreak(iMemBreak);
			}
		}
		if (ini.GetEntry("System", "LeakBreak", &bEnable))
		{
			RtCoreMem().SetLeakBreak(bEnable);
		}
		if (ini.GetEntry("System", "DumpMemLeak", &bEnable))
		{
			RtCoreMem().SetDumpMemLeak(bEnable);
		}

		ini.CloseFile();
	}

    // ΪĬ�ϵ�Log����һ���ļ����
    if (m_bIsDefaultFileLog)
    {
        if (!strName.IsEmpty())
        {
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
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%d%d%d%d%d%d",iYear,iMonth,iDay,iHour,iMin,iSec);
			RtString stime = buf;
			rootLog->AddOutFile("log", RtString("log")+PATH_SEPARATOR_S + strName+ "_log" + stime+".log");
        }
    }

    // �����ʼ��Log
    rootLog->Info("Core initialized.\n");
    return true;
}

void RtCore::Tick(int delta)
{
}

void RtCore::Exit()
{
    rtciPlatformClear();
    rootLog->Info("Core exit.\n");
}

bool RtCore::DoCommand(const char *cmd, const char *param)
{
    return true;
}

const char* RtCore::GetName()
{
    return strName.c_str();
}

RtLogType RtCore::CreateLog(const char* szName)
{
    RtLogType newLog( RT_NEW RtLog(szName));
    return newLog;
}

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/