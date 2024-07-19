
/*------------------------------------------------------------------------
-   Class NetSynchroPosition.
------------------------------------------------------------------------*/
#ifndef NET_SYNCHRO_POSITION_H
#define NET_SYNCHRO_POSITION_H

class NetSynchroPosition
{
public:
    NetSynchroPosition();
    virtual ~NetSynchroPosition();

    void InitWorld(GcWorld* pWorld)                     {m_pWorld=pWorld;}

    void DownloadInitPacket(CG_CmdPacket *pPacket);
    void DownloadNetSnapshot(CG_CmdPacket *pPacket,bool event=false);
    void DownloadNetEvent(CG_CmdPacket *pPacket);
    void Render(DWORD dwDifMillisecond);
    bool RenderNetSnapshot();

    void OnDownloadSnapshot(CG_CmdPacket *pPacket, DWORD dwTime);
    void OnDownloadEvent(CG_CmdPacket *pPacket);
    void OnUploadPosition(DWORD dwClientTime);
	
	DWORD GetServerSec() { return m_dwServerSec; }

protected:
    GcWorld*                m_pWorld;
    short                   m_sSyncCycle;       // ����������ͬ������(ms)����r2c_snapshot������
    short                   m_sUpdateCycle;     // �ͻ��˸���λ������(ms)����c2r_update_position������
    short                   m_sMaxPileCount;    // �ڿ�ʼ����ǰ��Ҫ���۵����ݰ����������۵�ʱ��m_bPileSnapshot=0�����������=1
    short                   m_sLimitPileCount;  // �������ݱ��Ľ��ޣ�����������޾���Ϊ�ͻ���̫���ˣ���Ҫ�ȴ�

	DWORD                   m_dwServerSec;      // �������ϵ�������������õ�
    DWORD                   m_dwLastServerTime; // ���һ���յ���Serverʱ��(ms)
    DWORD                   m_dwPrevServerTime; // �����ڶ����յ���Serverʱ��(ms)
    DWORD                   m_dwClientTime;     // �ͻ��˵�ʱ��
    DWORD                   m_dwAbsClientTime;  // ���Կͻ��˵�ʱ��, �������κ�У����
    DWORD                   m_dwDiffUpdateTime; // �������һ�θ���λ�õ�ʱ��
    DWORD                   m_dwSnapshotCount;  // ���µĴ���
    int                     m_bPileSnapshot;    // �Ƿ��ڻ�������
    list<CG_CmdPacket*>     m_SnapshotCmdList;  // �����б�

    DWORD                   m_dwFirstClientTime;
    DWORD                   m_dwFirstServerTime;

	DWORD					m_fixClientTime;

};
#endif//NET_SYNCHRO_POSITION_H
/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
