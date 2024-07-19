
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
    short                   m_sSyncCycle;       // 服务器行走同步周期(ms)，即r2c_snapshot的周期
    short                   m_sUpdateCycle;     // 客户端更新位置周期(ms)，即c2r_update_position的周期
    short                   m_sMaxPileCount;    // 在开始运行前需要积累的数据包个数，积累的时候m_bPileSnapshot=0，积累完成则=1
    short                   m_sLimitPileCount;  // 积累数据报的界限，超过这个界限就认为客户端太慢了，需要等待

	DWORD                   m_dwServerSec;      // 服务器上的秒数，任务会用到
    DWORD                   m_dwLastServerTime; // 最后一次收到的Server时间(ms)
    DWORD                   m_dwPrevServerTime; // 倒数第二次收到的Server时间(ms)
    DWORD                   m_dwClientTime;     // 客户端的时间
    DWORD                   m_dwAbsClientTime;  // 绝对客户端的时间, 不进行任何校正的
    DWORD                   m_dwDiffUpdateTime; // 距离最后一次更新位置的时间
    DWORD                   m_dwSnapshotCount;  // 更新的次数
    int                     m_bPileSnapshot;    // 是否还在积累数据
    list<CG_CmdPacket*>     m_SnapshotCmdList;  // 命令列表

    DWORD                   m_dwFirstClientTime;
    DWORD                   m_dwFirstServerTime;

	DWORD					m_fixClientTime;

};
#endif//NET_SYNCHRO_POSITION_H
/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
