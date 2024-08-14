#pragma once

// CGameWorldStub 类用于表示游戏世界的一个实例
class CGameWorldStub {
   public:
    // 游戏世界的状态枚举
    enum EStubState {
        STUB_STATE_NONE,  // 无状态，未初始化或无效

        STUB_STATE_ALIVE,  // 仍在与游戏世界服务保持连接

        STUB_STATE_NET_DEAD,  // 网络连接已经断开

        STUB_STATE_SERVER_SHUTDOWNING,  // 服务器正在关闭中（已发出关闭命令，但尚未收到游戏世界关闭命令）此时只处理登出命令，其他命令返回失败

        STUB_STATE_SERVER_SHUTDOWNED,  // 服务器已关闭（收到游戏世界关闭命令）现在不应有任何命令进来，等待关闭连接
    };

    // 默认构造函数
    CGameWorldStub();

    // 虚析构函数
    virtual ~CGameWorldStub();

    // 设置网络连接
    void SetNetLink(CG_TCPSession* link);

    // 解析命令
    bool ParseCmd(CG_CmdPacket* cmd);

    // 发送命令
    void SendCmd(CG_CmdPacket* cmd);

    // 更新状态，通常用于定时调用以处理周期性任务
    bool Tick(long delta);

    // 处理网络断开的情况
    void OnNetDead();

    // 发送接受登录命令
    void SendAcceptLoginCmd(bool bAccept);

    // 发送关闭服务器命令
    void SendShutdownCmd(long lDelayTime);

    // 发送踢出所有用户命令
    void SendKickAllUser(long lDelayTime);

    // 发送消息到指定用户
    void SendMessageToUser(DWORD dwAccountID, char cMsgType, const char* szMessage);

    // 测试功能
    void Test();

    // 获取测试结果
    bool GetTestResult(std::string& resultStr);

    // 用户计数相关
    void OnAddUser() {
        m_userCount++;
        OnUserCountChanged();
    }

    void OnRemoveUser() {
        m_userCount--;
        OnUserCountChanged();
    }

    // 用户数量变化时的处理
    void OnUserCountChanged() {
        m_evaluation = m_userCount / 400;
        if (m_evaluation > 10)
            m_evaluation = 10;  // 最大负载评估值为10
        else if (m_evaluation < 2)
            m_evaluation = 2;  // 最小负载评估值为2
    }

   public:
    long           m_lStubIdx;      // 游戏世界实例的索引
    EStubState     m_stubState;     // 当前状态
    long           m_lGameworldID;  // 游戏世界 ID
    CG_TCPSession* m_netLink;       // 网络连接
    long           m_netDeadTime;   // 网络断开时间
    CG_CmdPacket*  m_packet;        // 命令包

    // 服务器情况
    std::string m_szName;             // 服务器名称
    std::string m_szListenClientIP;   // 侦听客户端的 IP
    long        m_lListenClientPort;  // 侦听客户端的端口
    std::string m_szGameworldIP;      // 游戏世界 IP
    long        m_lGameworldPort;     // 游戏世界端口
    bool        m_bRegistered;        // 是否已注册
    char        m_gwState;            // 服务器状态

    // 服务器负载
    char m_evaluation;  // 负载情况，用于传递给客户端登录界面
    int  m_userCount;   // 用户数量

    // 列表类型
    long m_lListType;  // 列表类型，用于限制玩家看到的列表
        // 0x0001 自动销毁，当此游戏世界断开连接时自动从列表中移除
        // 0x0002 IP 限制，根据 IP 列表限制玩家看到的列表
    SAllowIP m_listAllowIP;  // 列表的 IP 限制，只有在这个 IP 列表中的用户才能看到这个游戏世界

    // 版本检查
    bool        m_bVersionChecked;  // 是否已检查版本
    std::string m_strExeMD5;        // 可执行文件的 MD5
    std::string m_strDataMD5;       // 数据文件的 MD5
    std::string m_strOsType;        // 操作系统类型

    // 测试
    char m_cTestState;  // 测试状态，0 表示未测试，1 表示正在测试，2 表示测试返回
    std::string m_strTestResult;  // 测试结果

    SStatGameWorldServer           m_gwStat;  // 游戏世界服务器的统计信息
    SStatGameMasterServer          m_gmStat;  // 游戏主服务器的统计信息
    std::vector<SStatRegionServer> m_rsStat;  // 区域服务器的统计信息
};
