//#include "login.h"
#include "net/rt_net.h"
#include "rt1tort2.h"
#include "server_config.h"

unsigned long StrToIP(const char* szIP)
{
    return inet_addr(szIP);
}

void SAllowIP::Serialize(CG_CmdPacket *cmd)
{
    long lAllowCnt, lFrom, lTo;
    if (cmd->IsReading())
    {
        *cmd << lAllowCnt;
        while (lAllowCnt)
        {
            *cmd << lFrom;
            *cmd << lTo;
            this->AddIP(lFrom, lTo);
            lAllowCnt --;
        }
    }else
    {
        lAllowCnt = (long)ips.size();
        *cmd << lAllowCnt;
        std::list<SIPInt>::iterator it;
        for (it=ips.begin(); it!=ips.end(); it++)
        {
            *cmd << (*it).from;
            *cmd << (*it).to;
        }
    }
}

void SAllowIP::AddIPFromStr(const char* szStr)
{
    const char seps[] = ", ";
    rt2_strcpy(g_strStaticBuffer, szStr);
    char* pIP1 = strtok(g_strStaticBuffer, seps);
    char* pIP2 = NULL;
    if (pIP1)
    {
        pIP2 = strtok(NULL, seps);
    }
    AddIP(pIP1, pIP2);
}

//////////////////////////////////////////////////////////////////////////

bool DecodeDatabase(const char* szCode, std::string& szUsername, std::string& szPassword, std::string& szDatabase)
{
    if (strlen(szCode)!=100) return false;
    unsigned short sKey;
    unsigned char pUsername[16], pPassword[16], pDatabase[16];
    unsigned char szSource[50];
    unsigned char szKey[50];
    unsigned char szTarget[50];
    unsigned char cUsername, cPassword, cDatabase;
    int i;
    for (i=0; i<50; i++)
    {
        szTarget[i] = (((szCode[i*2+0]>='A')?(szCode[i*2+0]-'A'+10):(szCode[i*2+0]-'0'))&0x0F)
            | ((((szCode[i*2+1]>='A')?(szCode[i*2+1]-'A'+10):(szCode[i*2+1]-'0'))&0x0F)<<4);
    }
    memcpy(&sKey, szTarget, 2);

    rtRandomSeed(sKey);
    for (i=0; i<50; i++)
    {
        szKey[i] = rtRandom()%0xFF;
    }

    for (i=2; i<50; i++)
    {
        szSource[i] = szKey[i]^szTarget[i];
    }

    cUsername = (szSource[2]&0xF0)>>4;
    cPassword = (szSource[2]&0x0F);
    cDatabase = (szSource[3]&0xF0)>>4;

    memcpy(pUsername, szSource+ 5, cUsername); pUsername[cUsername] = 0;
    memcpy(pPassword, szSource+20, cPassword); pPassword[cPassword] = 0;
    memcpy(pDatabase, szSource+35, cDatabase); pDatabase[cDatabase] = 0;

    szUsername = (char*)pUsername;
    szPassword = (char*)pPassword;
    szDatabase = (char*)pDatabase;

    return true;
}

//////////////////////////////////////////////////////////////////////////

void SAllowUser::Serialize(CG_CmdPacket *cmd)
{
    long lCnt;
    if (cmd->IsReading())
    {
        *cmd << lCnt;
        while (lCnt)
        {
            users.push_back(SUser());
            *cmd << users.back().username;
            *cmd << users.back().password;
            *cmd << users.back().privileges;
            lCnt --;
        }
    }else
    {
        lCnt = (long)users.size();
        *cmd << lCnt;
        std::list<SUser>::iterator it;
        for (it=users.begin(); it!=users.end(); it++)
        {
            *cmd << (*it).username;
            *cmd << (*it).password;
            *cmd << (*it).privileges;
        }
    }
}

void SAllowUser::Unite(SAllowUser& vOtherUser)
{
    users.insert(users.end(), vOtherUser.users.begin(), vOtherUser.users.end());
}

void SAllowUser::AddUser(const char* szUsername, const char* szPassword, long lPrivileges)
{
    users.push_back(SUser());
    users.back().username = szUsername;
    users.back().password = szPassword;
    users.back().privileges = lPrivileges;
}

void SAllowUser::AddUserFromStr(const char* szStr)
{
    const char seps[] = ", ";
    rt2_strcpy(g_strStaticBuffer, szStr);
    char const* pUser = strtok(g_strStaticBuffer, seps);
    char const* pPwd  = NULL;
    char const* pPriv = NULL;
    if (pUser)
        pPwd = strtok(NULL, seps);
    if (pPwd)
        pPriv = strtok(NULL, seps);
    if (pPriv==NULL)
        pPriv = "0";
    AddUser(pUser, pPwd, atol(pPriv));
}

bool SAllowUser::FindUser(const char* szUsername)
{
    stlforeach(std::list<SUser>, allow, users)
    {
        if (stricmp(allow->username.c_str(),szUsername)==0)
        {
            return true;
        }
    }
    return false;
}

long SAllowUser::CheckUser(const char* szUsername, const char* szPassword)
{
//    char szMD5Pwd[100];
//    rtMD5StringString((char*)szPassword, szMD5Pwd);
//    szMD5Pwd[99] = 0;
    SUser* pUser = NULL;
    stlforeach(std::list<SUser>, allow, users)
    {
        pUser = &(*allow);
        if (stricmp(pUser->username.c_str(),szUsername)==0 && stricmp(pUser->password.c_str(),szPassword)==0)
        {
            break;
        }
    }
    if (allow == users.end())
    {
        return 0;
    }
    return pUser->privileges;
}

//////////////////////////////////////////////////////////////////////////

SConfigLoginNet::SConfigLoginNet()
{
    accountDB.port              = 3306;

    systemCfg.threadNum         = 3;
    systemCfg.maxCmdNum         = 10000;
    systemCfg.logCycleSecond    = 600; // 默认，每10分钟log一次时间
    systemCfg.lDefaultStatus    = 0xFFFF;

    gamigoAdb.agentPort         = 6656;
    gamigoAdb.bindPort          = 0;
    gamigoAdb.timeoutSecond     = 5; // 默认，5秒时间还没有收到回应，就回复用户登录失败

    charge.bEnable              = false;
    charge.billingServerPort    = 6657;
    charge.bindPort             = 0;
    charge.timeoutSecond        = 5;

    listenGW.listenPort         = 6650;

    listenClientGuide.listenPort        = 6620;
    listenClientGuide.capacity          = 100;
    listenClientGuide.refreshInterval   = 2.f;

    listenMonitor.listenPort        = 6622;
    listenMonitor.capacity          = 10;

    listenUpdateServer.listenPort   = 6621;
    listenUpdateServer.capacity     = 100;
}

void SConfigRegionNet::Serialize(CG_CmdPacket *cmd)
{
    /*
    *cmd << sRegionID;

    *cmd << connectGameWorld.ip;
    *cmd << connectGameWorld.port;
    *cmd << connectGameWorld.bindIP;
    *cmd << connectGameWorld.bindPort;

    *cmd << listenClient.listenIP;
    *cmd << listenClient.listenPort;
    *cmd << listenClient.local_login;
    */
}

void SConfigRegionNet::Dump(/*RtOutputDevice* pLogDev,*/ const char* szStartStr)
{
    /*
    pLogDev->Logf("%s[Region server]\n", szStartStr);
    pLogDev->Logf("%s    RegionID     : %d\n", szStartStr, sRegionID);
    pLogDev->Logf("%s    [connect to GameWorld]\n", szStartStr);
    pLogDev->Logf("%s        IP       : %s\n", szStartStr, connectGameWorld.ip.c_str());
    pLogDev->Logf("%s        Port     : %d\n", szStartStr, connectGameWorld.port);
    pLogDev->Logf("%s        Bind IP  : %s\n", szStartStr, connectGameWorld.bindIP.c_str());
    pLogDev->Logf("%s        Bind Port: %d\n", szStartStr, connectGameWorld.bindPort);
    pLogDev->Logf("%s    [listen Client]\n", szStartStr);
    pLogDev->Logf("%s        IP             : %s\n", szStartStr, listenClient.listenIP.c_str());
    pLogDev->Logf("%s        Port           : %d\n", szStartStr, listenClient.listenPort);
    pLogDev->Logf("%s        local_login    : %d\n", szStartStr, listenClient.local_login);
    pLogDev->Info("\n");
    */
}

void SConfigGameworldNet::Serialize(CG_CmdPacket *cmd)
{
    /*
    *cmd << id;
    *cmd << name;

    *cmd << connectDatabase.ip;
    *cmd << connectDatabase.port;
    *cmd << connectDatabase.username;
    *cmd << connectDatabase.password;
    *cmd << connectDatabase.database;
    *cmd << connectDatabase.sendQueueCapacity;
    *cmd << connectDatabase.recvQueueCapacity;

    *cmd << connectLogin.ip;
    *cmd << connectLogin.port;
    *cmd << connectLogin.bindIP;
    *cmd << connectLogin.bindPort;

    *cmd << listenRegion.listenIP;
    *cmd << listenRegion.listenPort;

    int iCount;
    if (cmd->IsReading())
    {
        *cmd << iCount;
        SConfigRegionNet* pCRS;
        for (int i=0; i<iCount; i++)
        {
            pCRS = RT_NEW SConfigRegionNet;
            pCRS->Serialize(cmd);
            rsList.push_back(pCRS);
        }
    }else
    {
        iCount = (int)rsList.size();
        *cmd << iCount;
        std::list<SConfigRegionNet*>::iterator it;
        for (it=rsList.begin(); it!=rsList.end(); it++)
        {
            (*it)->Serialize(cmd);
        }
    }

    *cmd << listenClient.listenIP;
    *cmd << listenClient.listenPort;
    *cmd << listenClient.capacity;

    *cmd << listenGM.listenIP;
    *cmd << listenGM.listenPort;
    */
}

void SConfigGameworldNet::Dump(/*RtOutputDevice* pLogDev,*/ const char* szStartStr)
{
    /*
    pLogDev->Logf("%s[GameWorld server]\n", szStartStr);
    pLogDev->Logf("%s    ID  : %d\n", szStartStr, id);
    pLogDev->Logf("%s    Name: %s\n", szStartStr, name.c_str());
    pLogDev->Logf("%s    [connect to database]\n", szStartStr);
    pLogDev->Logf("%s        IP               : %s\n", szStartStr, connectDatabase.ip.c_str());
    pLogDev->Logf("%s        Port             : %d\n", szStartStr, connectDatabase.port);
    pLogDev->Logf("%s        username         : %s\n", szStartStr, "[hidden]");//connectDatabase.username.c_str());
    pLogDev->Logf("%s        password         : %s\n", szStartStr, "[hidden]");//connectDatabase.password.c_str());
    pLogDev->Logf("%s        database         : %s\n", szStartStr, connectDatabase.database.c_str());
    pLogDev->Logf("%s        sendQueueCapacity: %d\n", szStartStr, connectDatabase.sendQueueCapacity);
    pLogDev->Logf("%s        recvQueueCapacity: %d\n", szStartStr, connectDatabase.recvQueueCapacity);
    pLogDev->Logf("%s    [connect to login]\n", szStartStr);
    pLogDev->Logf("%s        IP             : %s\n", szStartStr, connectLogin.ip.c_str());
    pLogDev->Logf("%s        Port           : %d\n", szStartStr, connectLogin.port);
    pLogDev->Logf("%s        Bind IP        : %s\n", szStartStr, connectLogin.bindIP.c_str());
    pLogDev->Logf("%s        Bind Port      : %d\n", szStartStr, connectLogin.bindPort);
    pLogDev->Logf("%s    [listen region]\n", szStartStr);
    pLogDev->Logf("%s        IP             : %s\n", szStartStr, listenRegion.listenIP.c_str());
    pLogDev->Logf("%s        Port           : %d\n", szStartStr, listenRegion.listenPort);
    pLogDev->Logf("%s    [listen client]\n", szStartStr);
    pLogDev->Logf("%s        IP             : %s\n", szStartStr, listenClient.listenIP.c_str());
    pLogDev->Logf("%s        Port           : %d\n", szStartStr, listenClient.listenPort);
    pLogDev->Logf("%s        Capacity       : %d\n", szStartStr, listenClient.capacity);
    pLogDev->Logf("%s    [listen gm server]\n", szStartStr);
    pLogDev->Logf("%s        IP             : %s\n", szStartStr, listenGM.listenIP.c_str());
    pLogDev->Logf("%s        Port           : %d\n", szStartStr, listenGM.listenPort);
    pLogDev->Logf("%s    [region server(s)] [total: %d]\n", szStartStr, (int)rsList.size());

    std::string strStartStr(szStartStr);
    strStartStr += "        ";
    std::list<SConfigRegionNet*>::iterator it;
    for (it=rsList.begin(); it!=rsList.end(); it++)
    {
        (*it)->Dump(pLogDev, strStartStr.c_str());
    }
    pLogDev->Info("\n");
    */
}

void SConfigLoginNet::Dump(/*RtOutputDevice* pLogDev,*/ const char* szStartStr)
{
    /*
    pLogDev->Logf("%s[Login server]\n", szStartStr);
    pLogDev->Logf("%s    [account database]\n", szStartStr);
    pLogDev->Logf("%s        IP               : %s\n", szStartStr, accountDB.ip.c_str());
    pLogDev->Logf("%s        Port             : %d\n", szStartStr, accountDB.port);
    pLogDev->Logf("%s        username         : %s\n", szStartStr, "[hidden]");//accountDB.username.c_str());
    pLogDev->Logf("%s        password         : %s\n", szStartStr, "[hidden]");//accountDB.password.c_str());
    pLogDev->Logf("%s        database         : %s\n", szStartStr, accountDB.database.c_str());
    pLogDev->Logf("%s    [system config]\n", szStartStr);
    pLogDev->Logf("%s        threadNum        : %d\n", szStartStr, systemCfg.threadNum);
    pLogDev->Logf("%s        maxCmdNum        : %d\n", szStartStr, systemCfg.maxCmdNum);
    pLogDev->Logf("%s        logCycleSecond   : %d (second)\n", szStartStr, systemCfg.logCycleSecond);
    pLogDev->Logf("%s        defaultstatus    : %x\n", szStartStr, systemCfg.lDefaultStatus);
    pLogDev->Logf("%s    [gamigo accountDB agent]\n", szStartStr);
    pLogDev->Logf("%s        agentIP          : %s\n", szStartStr, gamigoAdb.agentIP.c_str());
    pLogDev->Logf("%s        agentPort        : %d\n", szStartStr, gamigoAdb.agentPort);
    pLogDev->Logf("%s        bindIP           : %s\n", szStartStr, gamigoAdb.bindIP.c_str());
    pLogDev->Logf("%s        bindPort         : %d\n", szStartStr, gamigoAdb.bindPort);
    pLogDev->Logf("%s        timeoutSecond    : %d (second)\n", szStartStr, gamigoAdb.timeoutSecond);
    pLogDev->Logf("%s    [listen client guide]\n", szStartStr);
    pLogDev->Logf("%s        IP             : %s\n", szStartStr, listenClientGuide.listenIP.c_str());
    pLogDev->Logf("%s        Port           : %d\n", szStartStr, listenClientGuide.listenPort);
    pLogDev->Logf("%s        Capacity       : %d\n", szStartStr, listenClientGuide.capacity);
    pLogDev->Logf("%s        RefreshInterval: %.2f (second)\n", szStartStr, listenClientGuide.refreshInterval);
    pLogDev->Logf("%s    [listen monitor client]\n", szStartStr);
    pLogDev->Logf("%s        IP             : %s\n", szStartStr, listenMonitor.listenIP.c_str());
    pLogDev->Logf("%s        Port           : %d\n", szStartStr, listenMonitor.listenPort);
    pLogDev->Logf("%s        Capacity       : %d\n", szStartStr, listenMonitor.capacity);
    pLogDev->Logf("%s    [listen update server]\n", szStartStr);
    pLogDev->Logf("%s        IP             : %s\n", szStartStr, listenUpdateServer.listenIP.c_str());
    pLogDev->Logf("%s        Port           : %d\n", szStartStr, listenUpdateServer.listenPort);
    pLogDev->Logf("%s        Capacity       : %d\n", szStartStr, listenUpdateServer.capacity);
    pLogDev->Logf("%s    [listen game world server]\n", szStartStr);
    pLogDev->Logf("%s        IP             : %s\n", szStartStr, listenGW.listenIP.c_str());
    pLogDev->Logf("%s        Port           : %d\n", szStartStr, listenGW.listenPort);
    pLogDev->Logf("%s    [game world server(s)] [total: %d]\n", szStartStr, (int)gwsList.size());

    std::string strStartStr(szStartStr);
    strStartStr += "        ";
    std::list<SConfigGameworldNet*>::iterator it;
    for (it=gwsList.begin(); it!=gwsList.end(); it++)
    {
        (*it)->Dump(pLogDev, strStartStr.c_str());
    }
    pLogDev->Info("\n");
    */
}

void SStatServer::UpdateInfo(int iVersion)
{
    if (iVersion==1)
    {
        //lAvgNetIn      = CG_NetSocket::m_globalNetInfo.GetAvgFluxIn();
        //lAvgNetOut     = CG_NetSocket::m_globalNetInfo.GetAvgFluxOut();
        //lLastAvgNetIn  = CG_NetSocket::m_globalNetInfo.GetLastSecAvgFluxIn();
        //lLastAvgNetOut = CG_NetSocket::m_globalNetInfo.GetLastSecAvgFluxOut();
    }
}

void SStatServer::UpdateMD5(const char* szFilename)
{
//    rtMD5File((char*)szFilename, (unsigned char*)szMD5);
}

void SStatServer::UpdateMD51(const char* szFilename)
{
   // rtMD5File((char*)szFilename, (unsigned char*)szMD51);
}

void SStatServer::UpdateMD52(const char* szFilename)
{
    //rtMD5File((char*)szFilename, (unsigned char*)szMD52);
}

void SetStrMD5ToMemMD5(const char* strMD5, unsigned char* memMD5)
{
    unsigned char* pMD5 = memMD5;
    const char* p = strMD5;
    char c1, c2, i=0;
    while (*p && i<16)
    {
        c1 = *p;
        if (*p==0) break;
        p ++;
        c2 = *p;
        p ++;

        if (c1>='0'&&c1<='9') c1 -= '0';
        else if (c1>='a'&&c1<='z') c1 = c1-'a'+10;
        else if (c1>='A'&&c1<='Z') c1 = c1-'A'+10;

        if (c2>='0'&&c2<='9') c2 -= '0';
        else if (c2>='a'&&c1<='z') c2 = c2-'a'+10;
        else if (c2>='A'&&c1<='Z') c2 = c2-'A'+10;

        *pMD5 = ((c1&0x0F)<<4)|(c2&0x0F);
        pMD5 ++;
        i++;
    }
}

void SStatServer::SetMD51(const char* strMD5)
{
    SetStrMD5ToMemMD5(strMD5, (unsigned char*)szMD51);
}

void SStatServer::SetMD52(const char* strMD5)
{
    SetStrMD5ToMemMD5(strMD5, (unsigned char*)szMD52);
}
