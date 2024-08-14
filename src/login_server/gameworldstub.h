#pragma once

// CGameWorldStub �����ڱ�ʾ��Ϸ�����һ��ʵ��
class CGameWorldStub {
   public:
    // ��Ϸ�����״̬ö��
    enum EStubState {
        STUB_STATE_NONE,  // ��״̬��δ��ʼ������Ч

        STUB_STATE_ALIVE,  // ��������Ϸ������񱣳�����

        STUB_STATE_NET_DEAD,  // ���������Ѿ��Ͽ�

        STUB_STATE_SERVER_SHUTDOWNING,  // ���������ڹر��У��ѷ����ر��������δ�յ���Ϸ����ر������ʱֻ����ǳ�������������ʧ��

        STUB_STATE_SERVER_SHUTDOWNED,  // �������ѹرգ��յ���Ϸ����ر�������ڲ�Ӧ���κ�����������ȴ��ر�����
    };

    // Ĭ�Ϲ��캯��
    CGameWorldStub();

    // ����������
    virtual ~CGameWorldStub();

    // ������������
    void SetNetLink(CG_TCPSession* link);

    // ��������
    bool ParseCmd(CG_CmdPacket* cmd);

    // ��������
    void SendCmd(CG_CmdPacket* cmd);

    // ����״̬��ͨ�����ڶ�ʱ�����Դ�������������
    bool Tick(long delta);

    // ��������Ͽ������
    void OnNetDead();

    // ���ͽ��ܵ�¼����
    void SendAcceptLoginCmd(bool bAccept);

    // ���͹رշ���������
    void SendShutdownCmd(long lDelayTime);

    // �����߳������û�����
    void SendKickAllUser(long lDelayTime);

    // ������Ϣ��ָ���û�
    void SendMessageToUser(DWORD dwAccountID, char cMsgType, const char* szMessage);

    // ���Թ���
    void Test();

    // ��ȡ���Խ��
    bool GetTestResult(std::string& resultStr);

    // �û��������
    void OnAddUser() {
        m_userCount++;
        OnUserCountChanged();
    }

    void OnRemoveUser() {
        m_userCount--;
        OnUserCountChanged();
    }

    // �û������仯ʱ�Ĵ���
    void OnUserCountChanged() {
        m_evaluation = m_userCount / 400;
        if (m_evaluation > 10)
            m_evaluation = 10;  // ���������ֵΪ10
        else if (m_evaluation < 2)
            m_evaluation = 2;  // ��С��������ֵΪ2
    }

   public:
    long           m_lStubIdx;      // ��Ϸ����ʵ��������
    EStubState     m_stubState;     // ��ǰ״̬
    long           m_lGameworldID;  // ��Ϸ���� ID
    CG_TCPSession* m_netLink;       // ��������
    long           m_netDeadTime;   // ����Ͽ�ʱ��
    CG_CmdPacket*  m_packet;        // �����

    // ���������
    std::string m_szName;             // ����������
    std::string m_szListenClientIP;   // �����ͻ��˵� IP
    long        m_lListenClientPort;  // �����ͻ��˵Ķ˿�
    std::string m_szGameworldIP;      // ��Ϸ���� IP
    long        m_lGameworldPort;     // ��Ϸ����˿�
    bool        m_bRegistered;        // �Ƿ���ע��
    char        m_gwState;            // ������״̬

    // ����������
    char m_evaluation;  // ������������ڴ��ݸ��ͻ��˵�¼����
    int  m_userCount;   // �û�����

    // �б�����
    long m_lListType;  // �б����ͣ�����������ҿ������б�
        // 0x0001 �Զ����٣�������Ϸ����Ͽ�����ʱ�Զ����б����Ƴ�
        // 0x0002 IP ���ƣ����� IP �б�������ҿ������б�
    SAllowIP m_listAllowIP;  // �б�� IP ���ƣ�ֻ������� IP �б��е��û����ܿ��������Ϸ����

    // �汾���
    bool        m_bVersionChecked;  // �Ƿ��Ѽ��汾
    std::string m_strExeMD5;        // ��ִ���ļ��� MD5
    std::string m_strDataMD5;       // �����ļ��� MD5
    std::string m_strOsType;        // ����ϵͳ����

    // ����
    char m_cTestState;  // ����״̬��0 ��ʾδ���ԣ�1 ��ʾ���ڲ��ԣ�2 ��ʾ���Է���
    std::string m_strTestResult;  // ���Խ��

    SStatGameWorldServer           m_gwStat;  // ��Ϸ�����������ͳ����Ϣ
    SStatGameMasterServer          m_gmStat;  // ��Ϸ����������ͳ����Ϣ
    std::vector<SStatRegionServer> m_rsStat;  // �����������ͳ����Ϣ
};
