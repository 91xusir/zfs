#pragma once

//
// �� login server ����ϵͨ��
// ���ദ�����¼�������� TCP �Ự
//
class GWLoginSession : public CG_TCPSession  // �̳��� CG_TCPSession����ʾһ�� TCP �Ự
{
    friend class GWServer;  // ���� GWServer Ϊ��Ԫ�࣬������� GWLoginSession ��˽�г�Ա

   public:
    // ����Ƿ���Ե�¼
    bool CanLogin() { return (GetState() == NET_STATE_CONNECTED && m_acceptLogin); }

    // ��ʼ��������
    CG_CmdPacket& BeginCmd(short cmd) {
        m_sendCmd.BeginWrite();
        m_sendCmd.WriteShort(cmd);
        return m_sendCmd;
    }

    // ��������
    bool SendCmd() { return SendPacket(&m_sendCmd); }

    // ��ʼ���Ự
    bool Initalize();

    // ����Ự
    void Process();

    // ��ʼ��ͨ��
    void InitChannel();

    // ���͹ر�ȷ����Ϣ
    void SendShutdownOK(bool bForce);  // �Ƿ���ǿ�ƹر�

    // ������Ϸ����״̬
    void UpdateGameWorldStat();

    // ��¼�˺�
    bool LoginAccount(int seed, const char* account, const char* pwaccount, const char* pubwinKey,
                      const char* glompwd, const char* password, const char* ip /*PZH*/,
                      bool bPowerLogin /**/);

    // �ǳ��˺�
    void LogoutAccount(DWORD accountId);

    // ����״̬
    void UpdateState(char cUpdate);

    // ��ɼ��
    bool LicenseCheck(int nType);

    // ��ȡ MD5 У��ֵ
    bool ReadMd5();

    // ����ӷ������汾
    long CheckSubServerVersion(const char* szServerName, const char* szExeMD5,
                               const char* szDataMD5, const char* szOS);

    // �����ӷ������汾�����
    void OnCheckSubServerVersion(long dwCheckID, bool bSame, const char* szServerName);

   private:
    // �������ӳɹ��Ļص�
    virtual void OnConnect(bool ret);

    // ����Ͽ����ӵĻص�
    virtual void OnDisconnect();

    // ������յ������ݰ�
    virtual void OnReceive(CG_CmdPacket* packet);

   public:
    // ���캯��
    GWLoginSession() : m_lastConnectTime(0) { this->SetDebugStr("GWLoginSession"); }

   private:
    CG_CmdPacket     m_recvCmd;          // �����������ݰ�
    CG_CmdPacket     m_sendCmd;          // �����������ݰ�
    GWCommandChannel m_channel;          // ����ͨ��
    time_t           m_lastConnectTime;  // ��һ������ʱ��

    // ���⹦��
    std::string m_strMd5;  // MD5 У��ֵ

   public:
    bool m_acceptLogin;  // �Ƿ���ܵ�¼
};
