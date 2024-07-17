#ifndef __GAME_WORLD_STUB_H__
#define __GAME_WORLD_STUB_H__

class CGameWorldStub
{
public:
    enum EStubState 
    {
        STUB_STATE_NONE,

        STUB_STATE_ALIVE,
        // 还在和gws保持连接

        STUB_STATE_NET_DEAD,
        // gws连接已经断开

        STUB_STATE_SERVER_SHUTDOWNING,
        // 关闭中(发出关闭命令，但是还没有接受到gws已经关闭命令)
        //     这时只处理登出命令，其他命令都返回失败

        STUB_STATE_SERVER_SHUTDOWNED,
        // 已经关闭 (受到gws已经关闭命令)
        //     现在不应该有任何命令进来了
        //     等待关闭连接
    };

	CGameWorldStub();
	virtual ~CGameWorldStub();

	void SetNetLink(CG_TCPSession *link);
	bool ParseCmd(CG_CmdPacket *cmd);
	void SendCmd(CG_CmdPacket *cmd);
	bool Tick(long delta);
	void OnNetDead();

    void SendAcceptLoginCmd(bool bAccept);
    void SendShutdownCmd(long lDelayTime);
    void SendKickAllUser(long lDelayTime);
    void SendMessageToUser(DWORD dwAccountID, char cMsgType, const char* szMessage);

    void Test();
    bool GetTestResult(std::string& resultStr);

    void OnAddUser()                    { m_userCount++; OnUserCountChanged();}
    void OnRemoveUser()                 { m_userCount--; OnUserCountChanged();}
    void OnUserCountChanged()
    {
        m_evaluation = m_userCount/400;
        if (m_evaluation>10)     m_evaluation = 10;
        else if (m_evaluation<2) m_evaluation = 2;
    }

public:

    long                m_lStubIdx;
	EStubState		    m_stubState;
    long                m_lGameworldID;
	CG_TCPSession      *m_netLink;
	long			    m_netDeadTime;
	CG_CmdPacket       *m_packet;
	

    // 服务器情况
    std::string         m_szName;           // 服务器名字
    std::string         m_szListenClientIP; // 侦听IP
    long                m_lListenClientPort;
    std::string         m_szGameworldIP;    // 侦听IP
    long                m_lGameworldPort;
    bool                m_bRegistered;      // 是否已经注册
    char                m_gwState;          // 服务器状态

    // 服务器负载
    char                m_evaluation;       // 负载情况，用来传给客户端登陆界面
    int                 m_userCount;        // 玩家人数

    // 列表类型
    long                m_lListType;    // 列表类型,用于限制玩家看到的列表
                                        //     0x0001 自动销毁, 当这个GameWorld断开连接的时候自动从列表中移除
                                        //     0x0002 IP限制, 这个选项会根据IP列表进行限制玩家看到的列表
    SAllowIP            m_listAllowIP;  // 列表的IP限制，只有这个IP内的人才能看到这个GameWorld

    // 版本检查
    bool                m_bVersionChecked;  // 是否已经检查过版本
    std::string         m_strExeMD5;
    std::string         m_strDataMD5;
    std::string         m_strOsType;

    // 测试
    char                m_cTestState;    // 0 没有测试, 1 正在测试, 2 测试返回
    std::string         m_strTestResult;

    SStatGameWorldServer            m_gwStat;
    SStatGameMasterServer           m_gmStat;
    std::vector<SStatRegionServer>  m_rsStat;

};

#endif
