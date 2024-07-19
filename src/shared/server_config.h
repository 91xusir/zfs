#ifndef __SERVER_CONFIG_H__
#define __SERVER_CONFIG_H__

// dummy class by louis
class RtOutputDevice;

inline unsigned long InvertIPOrder(unsigned long dwIP)
{
    return ((dwIP>>24) & 0xff) | ((dwIP>>8) & 0xff00) |  ((dwIP<<8) &0xff0000) | ((dwIP<<24) & 0xff000000);
}
unsigned long StrToIP(const char* szIP);
bool DecodeDatabase(const char* szCode, std::string& szUsername, std::string& szPassword, std::string& szDatabase);

struct SConfigEudemon
{
    int         enable;
    std::string ip;
    int         port;
    int         update_interval;
    SConfigEudemon()
    {
        enable = 0;
        ip = "127.0.0.1";
        port = 6690;
        update_interval = 2000;
    }
};

struct SAllowIP
{
private:
    struct SIPInt
    {
        unsigned long from;
        unsigned long to;
    };
    std::list<SIPInt>  ips;
public:
    void Clear()    { ips.clear(); }
    void Serialize(CG_CmdPacket *cmd);

    bool HaveIP()   { return (ips.size()!=0); }

    // dwTo����Ϊ0, dwFrom������Ϊ0
    void AddIP(unsigned long dwFrom, unsigned long dwTo)
    {
        ips.push_back(SIPInt());
        ips.back().from = dwFrom;
        ips.back().to = dwTo;
    }
    // szTo����ΪNULL, szFrom������ΪNULL
    void AddIP(const char* szFrom, const char* szTo)
    {
        unsigned long from = StrToIP(szFrom);
        unsigned long to = (szTo==NULL)?0:StrToIP(szTo);
        ips.push_back(SIPInt());
        ips.back().from = InvertIPOrder(from);
        ips.back().to = InvertIPOrder(to);
    }
    void AddIPFromStr(const char* szStr); // ����һ���ַ���,��ʽΪ "192.168.0.10, 192.168.0.20" �� "192.168.0.10"

    bool CheckAllow(const char* szIP, bool bDefault=false) // ���IP�Ƿ���IP�б���, ���bDefault=true�����б�Ϊ�յ�ʱ�򷵻�true
    {
        unsigned long dwIP = InvertIPOrder(StrToIP(szIP));
        return CheckAllow(dwIP, bDefault);
    }
    bool CheckAllow(unsigned long dwIP, bool bDefault=false) // ���IP�Ƿ���IP�б���
    {
        if (ips.empty()) return bDefault;
        std::list<SIPInt>::iterator it;
        for (it=ips.begin(); it!=ips.end(); it++)
        {
            if ((*it).to)
            {
                if (dwIP>=(*it).from && dwIP<=(*it).to)
                    return true;
            }else
            {
                if (dwIP==(*it).from)
                    return true;
            }
        }
        return false;
    }
};

struct SAllowUser
{
private:
    struct SUser
    {
        std::string             username;
        std::string             password;
        long                    privileges; // Ȩ��
    };
    std::list<SUser>    users;

public:
    void Clear()    { users.clear(); }
    void Serialize(CG_CmdPacket *cmd);
    void Unite(SAllowUser& vOtherUser);

    void AddUser(const char* szUsername, const char* szPassword, long lPrivileges);
    void AddUserFromStr(const char* szStr); // ����һ���ַ���,��ʽΪ "user, pwd, privileges"

    bool FindUser(const char* szUsername);
    long CheckUser(const char* szUsername, const char* szPassword); // ����û��������Ƿ���ȷ�������ȷ����Ȩ�ޣ�0��־����ȷ������Ȩ��

};


struct SConfigConnect
{
    std::string     ip;
    int             port;
    std::string     bindIP;
    int             bindPort;
};

struct SConfigRegionNet
{
    long            lRegionID;
    SConfigConnect  connectGameWorld ;
    SConfigEudemon  eudemon;

    struct SListenClient
    {
        std::string             listenIP;
        int                     listenPort;
        bool                    local_login;
        int                     capacity;
        SAllowIP                allowIP;
    } listenClient;

    void Serialize(CG_CmdPacket *cmd);
    void Dump(/*RtOutputDevice* pLogDev,*/ const char* szStartStr);

};

struct SConfigGameworldNet
{
    long           lGameworldID;

    struct SConnectDatabase
    {
        std::string			ip;
        int					port;
        std::string			username;
        std::string			password;
        std::string			database;
        int					sendQueueCapacity;
        int					recvQueueCapacity;
    };
	SConnectDatabase connectDatabase;
	SConnectDatabase connectLogDb;
	SConnectDatabase connectActDB;

    SConfigConnect connectLogin ;

	SConfigConnect connectLogServer;

    struct SListenRegion
    {
        std::string             listenIP;
        int                     listenPort;
        int                     capacity;
        SAllowIP                allowIP;
        std::set<int>           mapDungeonRsID;
    } listenRegion ;

    struct SListenClient
    {
        std::string             listenIP;
        int                     listenPort;
        int                     capacity;
        SAllowIP                allowIP;
    } listenClient ;

    struct SListenGM
    {
        std::string	            listenIP;
        int		                listenPort;
        SAllowIP                allowIP;
    } listenGM;

    struct SConfigGame
    {
        std::string             szGameworldName;
        int                     maxUser;
        int                     maxRegionUser;
        int                     listType;
        SAllowIP                listAllowIP;
    } game;

	struct SLineSYS
	{
		DWORD dwRegionCount;
	} LineSYS ;

	int LogTableLimit;

    void Serialize(CG_CmdPacket *cmd);
    void Dump(/*RtOutputDevice* pLogDev,*/ const char* szStartStr);
};

struct SConfigLoginNet
{
    // ����Account DB
    struct SAccountDB
    {
        std::string     ip;              // account db ip
        short           port;            // account db port
        std::string     database;        // account db database name
        std::string     username;        // account db username
        std::string     password;        // account db password
    } accountDB;

    // ϵͳ����
    struct SSystemCfg
    {
        int         threadNum;
        int         maxCmdNum;
        int         logCycleSecond;
        long        lDefaultStatus;
    } systemCfg;

    // �����Զ����������
    struct SGamigoAdb
    {
        std::string agentIP;
        int         agentPort;
        std::string bindIP;
        int         bindPort;
        int         timeoutSecond;
    } gamigoAdb;

    // �����Զ����������
    struct SCharge
    {
        int         bEnable;
        std::string billingServerIP;
        int         billingServerPort;
        std::string bindIP;
        int         bindPort;
        std::string strUsername;
        std::string strPassword;
        int         timeoutSecond;
    } charge;

    // ����Game World Server
    struct SListenGameWorldServer
    {
        std::string             listenIP;
        int                     listenPort;
        SAllowIP                allowIP;
    } listenGW ;

    // ���� �ͻ��˵� Guide ����
    struct SListenClientGuide
    {
        std::string             listenIP;
        int                     listenPort;
        int                     capacity;
        float                   refreshInterval;
        SAllowIP                allowIP;
    } listenClientGuide ;

    // ���� �������
    struct SListenMonitor
    {
        std::string             listenIP;
        int                     listenPort;
        int                     capacity;
		float						peopleRate;
        float                   refreshInterval;
        SAllowIP                allowIP;
        SAllowUser              allowUser;
    } listenMonitor;

    // ���� Update Server
    struct SListenUpdateServer
    {
        std::string             listenIP;
        int                     listenPort;
        int                     capacity;
        SAllowIP                allowIP;
    } listenUpdateServer ;

    // ����GM��[��Ϸ��]��½IP
    SAllowIP                    gmAllowIP;

    SConfigLoginNet();
    void Dump(/*RtOutputDevice* pLogDev,*/ const char* szStartStr);
};

#define SERVER_STAT_VERSION     3

// ������ͳ��
struct SStatServer
{
    // ��һ���汾�ģ���������Linuxϵͳ�ϵ�CPU����Ϣ����ȡ�ã����ҿ���ʹ�������ļ���������������
    float   fCPURate;       // CPUռ����
    long    lMemUsed;       // �ڴ�ʹ��
    long    lAvgNetIn;      // ����ƽ������In(ÿ��)
    long    lAvgNetOut;     // ����ƽ������Out(ÿ��)
    long    lLastAvgNetIn;  // ���һ��������ƽ������In(ÿ��)
    long    lLastAvgNetOut; // ���һ��������ƽ������Out(ÿ��)
    long    lEvaluation;    // �ܵ�����ֵ

    // �ڶ����汾
    union
    {
		//unsigned char   szMD5[16];
		CM_MEMDEF(szMD5, 16)

		// �������汾
		//unsigned char   szMD51[16];
		CM_MEMDEF(szMD51, 16)
	};
    //unsigned char   szMD52[16];
	CM_MEMDEF(szMD52, 16)

    // �յ����ݵİ汾, ��������û���������
    long    lVersion;

    SStatServer()
    {
		//CM_MEMPROTECTOR(szMD5, 16)
		CM_MEMPROTECTOR(szMD51, 16)
		CM_MEMPROTECTOR(szMD52, 16)
        Reset();
    }
	SStatServer(const SStatServer &object)
	{
		memcpy(this, &object, sizeof(SStatServer));
		CM_MEMPROTECTOR(szMD51, 16)
		CM_MEMPROTECTOR(szMD52, 16)
	}
	~SStatServer()
	{
		//CM_MEMUNPROTECTOR(szMD5)
		CM_MEMUNPROTECTOR(szMD51)
		CM_MEMUNPROTECTOR(szMD52)
	}
    void Reset()
    {
        lVersion = 0;

        fCPURate = 0.f;
        lMemUsed = 0;
        lAvgNetIn = 0;
        lAvgNetOut = 0;
        lLastAvgNetIn = 0;
        lLastAvgNetOut = 0;
        lEvaluation = 0;

        memset(szMD5, 0, sizeof(char)*16);

        memset(szMD51, 0, sizeof(char)*16);
        memset(szMD52, 0, sizeof(char)*16);
    }
    void Serialize(CG_CmdPacket *cmd, int iVersion)
    {
        lVersion = iVersion;
        if (iVersion==1)
        {
            *cmd << fCPURate;
            *cmd << lMemUsed;
            *cmd << lAvgNetIn;
            *cmd << lAvgNetOut;
            *cmd << lLastAvgNetIn;
            *cmd << lLastAvgNetOut;
            *cmd << lEvaluation;
        }else if (iVersion==2)
        {
            if (cmd->IsReading())
            {
                char *pData;
                int iLen;
                cmd->ReadBinary(&pData, &iLen);
                if (iLen==16)
                {
                    memcpy(szMD5, pData, 16);
                }else
                {
                    memset(szMD5, 0, 16);
                }
            }else
            {
                cmd->WriteBinary((char*)szMD5, 16);
            }
        }else if (iVersion==3)
        {
            if (cmd->IsReading())
            {
                char *pData;
                int iLen;
                cmd->ReadBinary(&pData, &iLen);
                if (iLen==16)   memcpy(szMD51, pData, 16);
                else            memset(szMD51, 0, 16);
                cmd->ReadBinary(&pData, &iLen);
                if (iLen==16)   memcpy(szMD52, pData, 16);
                else            memset(szMD52, 0, 16);
            }else
            {
                cmd->WriteBinary((char*)szMD51, 16);
                cmd->WriteBinary((char*)szMD52, 16);
            }
        }
    }
    void UpdateInfo(int iVersion);
    void UpdateMD5(const char* szFilename);

    void UpdateMD51(const char* szFilename);
    void UpdateMD52(const char* szFilename);
    void SetMD51(const char* strMD5);
    void SetMD52(const char* strMD5);
};

// RegionServer ͳ��
struct SStatRegionServer
{
    short           sState;
    short           sID;
    short           sPort;
    std::string     strHost;

    long            lUserCount;     // ��Ҹ���
    long            lObjectCount;   // �������
    SStatServer     sv;

	DWORD            frameNum;
	DWORD            busyFrameNum;
	DWORD            totalTime;
	DWORD            syncTime;
	DWORD            processTime;
	DWORD            timerTime;

    SStatRegionServer()
    {
        sID = 0;
        Reset();
    }
    void Reset()
    {
        strHost = "";
        sPort = 0;

        lUserCount = 0;
        lObjectCount = 0;
        sv.Reset();

        frameNum	 = 0;
        busyFrameNum = 0;
        totalTime	 = 0;
        syncTime	 = 0;
        processTime  = 0;
        timerTime    = 0;
    }
    void Serialize(CG_CmdPacket *cmd, int iVersion, float rate = 1.0)
    {
		long lUser = lUserCount * rate;
		long lObject = lObjectCount * rate;
		if (cmd->IsReading())
		{
			*cmd << lUserCount;
			*cmd << lObjectCount;
		}
		else
		{
			cmd->WriteLong(lUser);
			cmd->WriteLong(lObject);
		}
        sv.Serialize(cmd, iVersion);
		*cmd << frameNum << busyFrameNum << totalTime << syncTime << processTime << timerTime;
    }
    void SerializeAll(CG_CmdPacket *cmd, int iVersion)
    {
        *cmd << strHost;
        *cmd << sPort;
        Serialize(cmd, iVersion);
    }
};

// GameWorldServer ͳ��
struct SStatGameWorldServer
{
    long            lUserCount;     // ��Ҹ���
    SStatServer     sv;
    SStatGameWorldServer()
    {
        lUserCount = 0;
    }
    void Serialize(CG_CmdPacket *cmd, int iVersion, float rate = 1.0)
    {
		long lUser = lUserCount * rate;
		if (cmd->IsReading())
			*cmd << lUserCount;
		else
			cmd->WriteLong(lUser);
        sv.Serialize(cmd, iVersion);
    }
};

// GameMasterServer ͳ��
struct SStatGameMasterServer
{
    short           sState;
    long            lGMCount;       // ��Ҹ���
    SStatServer     sv;
    SStatGameMasterServer()
    {
        sState = 0;
        lGMCount = 0;
    }
    void Serialize(CG_CmdPacket *cmd, int iVersion, float rate = 1.0)
    {
		long lGM = lGMCount * rate;
        *cmd << sState;
		if (cmd->IsReading())
			*cmd << lGMCount;
		else
			cmd->WriteLong(lGM); 
        sv.Serialize(cmd, iVersion);
    }
};

// Login ͳ��
struct SStatLoginServer
{
    long            lUserCount;     // ��Ҹ���
    long            lState;
    SStatServer     sv;
    SStatLoginServer()
    {
        lUserCount = 0;
    }
    void Serialize(CG_CmdPacket *cmd, int iVersion)
    {
        *cmd << lUserCount;
        *cmd << lState;
        sv.Serialize(cmd, iVersion);
    }
};

// UpdateServer ͳ��
struct SStatUpdateServer
{
    long            lUserCount;     // ��Ҹ���
    SStatServer     sv;
    SStatUpdateServer()
    {
        lUserCount = 0;
    }
    void Serialize(CG_CmdPacket *cmd, int iVersion)
    {
        *cmd << lUserCount;
        sv.Serialize(cmd, iVersion);
    }
};

#endif // __SERVER_CONFIG_H__
