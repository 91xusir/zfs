#pragma once

//
// 与 login server 的联系通道
// 该类处理与登录服务器的 TCP 会话
//
class GWLoginSession : public CG_TCPSession  // 继承自 CG_TCPSession，表示一个 TCP 会话
{
    friend class GWServer;  // 声明 GWServer 为友元类，允许访问 GWLoginSession 的私有成员

   public:
    // 检查是否可以登录
    bool CanLogin() { return (GetState() == NET_STATE_CONNECTED && m_acceptLogin); }

    // 开始构建命令
    CG_CmdPacket& BeginCmd(short cmd) {
        m_sendCmd.BeginWrite();
        m_sendCmd.WriteShort(cmd);
        return m_sendCmd;
    }

    // 发送命令
    bool SendCmd() { return SendPacket(&m_sendCmd); }

    // 初始化会话
    bool Initalize();

    // 处理会话
    void Process();

    // 初始化通道
    void InitChannel();

    // 发送关闭确认消息
    void SendShutdownOK(bool bForce);  // 是否是强制关闭

    // 更新游戏世界状态
    void UpdateGameWorldStat();

    // 登录账号
    bool LoginAccount(int seed, const char* account, const char* pwaccount, const char* pubwinKey,
                      const char* glompwd, const char* password, const char* ip /*PZH*/,
                      bool bPowerLogin /**/);

    // 登出账号
    void LogoutAccount(DWORD accountId);

    // 更新状态
    void UpdateState(char cUpdate);

    // 许可检查
    bool LicenseCheck(int nType);

    // 读取 MD5 校验值
    bool ReadMd5();

    // 检查子服务器版本
    long CheckSubServerVersion(const char* szServerName, const char* szExeMD5,
                               const char* szDataMD5, const char* szOS);

    // 处理子服务器版本检查结果
    void OnCheckSubServerVersion(long dwCheckID, bool bSame, const char* szServerName);

   private:
    // 处理连接成功的回调
    virtual void OnConnect(bool ret);

    // 处理断开连接的回调
    virtual void OnDisconnect();

    // 处理接收到的数据包
    virtual void OnReceive(CG_CmdPacket* packet);

   public:
    // 构造函数
    GWLoginSession() : m_lastConnectTime(0) { this->SetDebugStr("GWLoginSession"); }

   private:
    CG_CmdPacket     m_recvCmd;          // 接收命令数据包
    CG_CmdPacket     m_sendCmd;          // 发送命令数据包
    GWCommandChannel m_channel;          // 命令通道
    time_t           m_lastConnectTime;  // 上一次连接时间

    // 额外功能
    std::string m_strMd5;  // MD5 校验值

   public:
    bool m_acceptLogin;  // 是否接受登录
};
