#include "gc_include.h"
#include "gc_bat.h"


CGcBat g_GcBat;

CGcBat::CGcBat()
{
}

CGcBat::~CGcBat()
{
}

void CGcBat::Run()
{
    guard;

    static DWORD dwLastRunTick = 0;
    DWORD dwNowTick = rtGetMilliseconds();
    if (dwNowTick - dwLastRunTick < 2000)
        return;
    dwLastRunTick = dwNowTick;

    int LinesTotal = 0;
    int LinesRun = 0;
    //static char buffer[1024];
	S_MEMDEF(buffer, 1024)
	S_MEMPROTECTOR(buffer, 1024, bDefined)

    std::string FileName = "bat.txt";
    FILE* pFile = fopen(FileName.c_str(), "r");
    if (!pFile)
    {
        return;
    }

    int counterForSafe = 50;
    while ( (fgets(buffer, 1024, pFile)) != NULL)
    {
        if (buffer[0] == '\0' || buffer[0] == ';' || buffer[0] == '\n')
            continue;

        LinesTotal ++;
        LinesRun ++;

        CG_CmdPacket* pPacket = NetBeginWrite();
        pPacket->WriteShort(c2r_chat);
        pPacket->WriteByte(CHAT_CHANNEL_NORMAL);
        pPacket->WriteString(buffer);
        NetSend(pPacket);

        Sleep(50);

        if (counterForSafe-- < 0)
            break;
    }
 
    fclose(pFile);

    //sprintf(buffer, "共有%d条语句，%d条被执行", LinesTotal, LinesRun);//by fox for string
	rt2_sprintf(buffer, R(MSG_BAT_RUN_INFO), LinesTotal, LinesRun);
    ShowSystemMessage(buffer);

    unguard;
}
