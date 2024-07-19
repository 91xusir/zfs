#ifndef __GAME_WORLD_STUB_H__
#define __GAME_WORLD_STUB_H__

class CGameWorldStub
{
public:
    enum EStubState 
    {
        STUB_STATE_NONE,

        STUB_STATE_ALIVE,
        // ���ں�gws��������

        STUB_STATE_NET_DEAD,
        // gws�����Ѿ��Ͽ�

        STUB_STATE_SERVER_SHUTDOWNING,
        // �ر���(�����ر�������ǻ�û�н��ܵ�gws�Ѿ��ر�����)
        //     ��ʱֻ����ǳ���������������ʧ��

        STUB_STATE_SERVER_SHUTDOWNED,
        // �Ѿ��ر� (�ܵ�gws�Ѿ��ر�����)
        //     ���ڲ�Ӧ�����κ����������
        //     �ȴ��ر�����
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
	

    // ���������
    std::string         m_szName;           // ����������
    std::string         m_szListenClientIP; // ����IP
    long                m_lListenClientPort;
    std::string         m_szGameworldIP;    // ����IP
    long                m_lGameworldPort;
    bool                m_bRegistered;      // �Ƿ��Ѿ�ע��
    char                m_gwState;          // ������״̬

    // ����������
    char                m_evaluation;       // ������������������ͻ��˵�½����
    int                 m_userCount;        // �������

    // �б�����
    long                m_lListType;    // �б�����,����������ҿ������б�
                                        //     0x0001 �Զ�����, �����GameWorld�Ͽ����ӵ�ʱ���Զ����б����Ƴ�
                                        //     0x0002 IP����, ���ѡ������IP�б����������ҿ������б�
    SAllowIP            m_listAllowIP;  // �б��IP���ƣ�ֻ�����IP�ڵ��˲��ܿ������GameWorld

    // �汾���
    bool                m_bVersionChecked;  // �Ƿ��Ѿ������汾
    std::string         m_strExeMD5;
    std::string         m_strDataMD5;
    std::string         m_strOsType;

    // ����
    char                m_cTestState;    // 0 û�в���, 1 ���ڲ���, 2 ���Է���
    std::string         m_strTestResult;

    SStatGameWorldServer            m_gwStat;
    SStatGameMasterServer           m_gmStat;
    std::vector<SStatRegionServer>  m_rsStat;

};

#endif
