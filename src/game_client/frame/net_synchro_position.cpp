
#include "gc_include.h"
#include "region_client.h"
#include "ui_form_msg.h"
#include <mmsystem.h>
#include <preConsole.h>

/*------------------------------------------------------------------------
-   Class NetSynchroPosition.
------------------------------------------------------------------------*/

NetSynchroPosition::NetSynchroPosition() {
    m_pWorld       = NULL;
    m_sSyncCycle   = 0;  // 服务器行走同步周期(ms)，即r2c_snapshot的周期
    m_sUpdateCycle = 0;  // 客户端更新位置周期(ms)，即c2r_update_position的周期
    m_sMaxPileCount =
        0;  // 在开始运行前需要积累的数据包个数，积累的时候m_bPileSnapshot=0，积累完成则=1
    m_sLimitPileCount = 0;  // 积累数据报的界限，超过这个界限就认为客户端太慢了，需要等待
    m_dwLastServerTime = 0;  // 最后一次收到的Server时间(ms)
    m_dwPrevServerTime = 0;  // 倒数第二次收到的Server时间(ms)
    m_dwClientTime     = 0;  // 客户端的时间
    m_dwAbsClientTime  = 0;
    m_dwDiffUpdateTime = 0;  // 距离最后一次更新位置的时间
    m_dwSnapshotCount  = 0;  // 更新的次数
    m_bPileSnapshot    = 0;  // 是否还在积累数据

    m_fixClientTime = 0;
}

NetSynchroPosition::~NetSynchroPosition() {}

void NetSynchroPosition::DownloadInitPacket(CG_CmdPacket* pPacket) {
    long sRandSeed;
    READ_short(m_sSyncCycle);
    READ_short(m_sUpdateCycle);
    READ_short(m_sMaxPileCount);
    READ_short(m_sLimitPileCount);
    READ_short(g_bUnWg);

    m_dwFirstServerTime = 0;
    m_dwAbsClientTime   = 0;

    if (g_bUnWg) {
        unsigned char buf[1024 * 20];
        char*         pBuf;
        int           len = 1024 * 20;
        pPacket->ReadBinary(&pBuf, &len);
        memcpy(buf, pBuf, len);

        if (NES_Initialize_C("", 111, buf, len)) {
            UIFormMsg::ShowStatic(R(MSG_CLIENT_INITIALIZEFAIL), UIFormMsg::TYPE_OK_CANCEL);
        }
    }

    char cUse;
    READ_char(cUse);
    if (cUse) {
        GetWorld()->m_cUserJhWg = cUse;
        READ_long(sRandSeed);
        Lua()->CallLFunc("setrs", "i:", sRandSeed);
    }
}

static int s_iCntNetTimeout = 0;
static int s_iMaxNetTimeout = 2;

void NetSynchroPosition::DownloadNetSnapshot(CG_CmdPacket* pPacket, bool event) {
    CG_CmdPacket* p;
    p = RT_NEW    CG_CmdPacket;
    if (!p->CloneFrom(pPacket)) {
        CHECKEX("错误: Clone Packet Error.");
        DEL_ONE(p);
    } else {
        m_SnapshotCmdList.push_back(p);
    }

    if (m_bPileSnapshot == 0 && m_SnapshotCmdList.size() >= m_sMaxPileCount) {
        m_bPileSnapshot = 1;
    }

    if (!event) {
        // snapshot with a server sec,event don't
        if (!pPacket->ReadLong((long*)&m_dwServerSec))
            return;
    }

    long curServerTime;
    if (!pPacket->ReadLong(&curServerTime))
        return;

    if (m_dwFirstServerTime == 0) {
        m_dwFirstServerTime = curServerTime;
        m_dwFirstClientTime = rtGetMilliseconds();
    } else {
        if ((curServerTime - m_dwFirstServerTime) > 30 * 1000)  // 30秒
        {
            // 如果误差超过9秒，就认为是使用加速器
            long lDiffTime = (long)(rtGetMilliseconds() - m_dwFirstClientTime) -
                             (long)(curServerTime - m_dwFirstServerTime);
            if (lDiffTime > 9000) {
                if (s_iCntNetTimeout >= s_iMaxNetTimeout) {
                    if (s_isHacker == 0)
                        s_isHacker = 1;
                }
                s_iCntNetTimeout++;
#ifdef _DEBUG
                float fRate = ((float)(rtGetMilliseconds() - m_dwFirstClientTime)) /
                              ((float)(curServerTime - m_dwFirstServerTime));
                ERR4("Net Accelerated, Time=%dms Rate=%.3f, Count=%d/%d\n", lDiffTime, fRate,
                     s_iCntNetTimeout, s_iMaxNetTimeout);
#endif
            }
            m_dwFirstServerTime = 0;
            m_dwFirstClientTime = 0;
        }
    }

    if (m_SnapshotCmdList.size() > m_sLimitPileCount) {
        if (curServerTime > m_dwClientTime + 500) {
            m_fixClientTime = m_dwClientTime + 500;
        } else {
            m_fixClientTime = curServerTime;
        }

        if (m_SnapshotCmdList.size() >= 3) {

            P_LOGWARN("缓冲的数据太多了 " + std::to_string(m_SnapshotCmdList.size()) +
                      "个包，客户端发生严重错误，或者太慢");

            ERR2("%d: 缓冲的数据太多了(%d个包)，客户端发生严重错误，或者太慢。\n", (int)time(NULL),
                 (int)m_SnapshotCmdList.size());
        }
    }
}

void NetSynchroPosition::DownloadNetEvent(CG_CmdPacket* pPacket) {
    DownloadNetSnapshot(pPacket, true);
}

void NetSynchroPosition::Render(DWORD dwDifMillisecond) {
    if (m_bPileSnapshot == 0) {
        return;
    }

    // 更新数据必须超过2次(客户端时间才能开始启动)
    if (m_dwSnapshotCount < 2) {
        RenderNetSnapshot();  // 更新服务器数据到客户端
        return;
    }

    m_dwClientTime += dwDifMillisecond;  // 累加客户端时间

    while (m_dwClientTime > m_dwLastServerTime) {
        if (!RenderNetSnapshot())
            break;
    }

    while (m_fixClientTime > m_dwClientTime) {
        if (!RenderNetSnapshot())
            break;
        m_dwClientTime = m_dwLastServerTime;
    }

    /*
    while (m_dwClientTime>m_dwLastServerTime) //if (m_dwClientTime<m_dwLastServerTime+(m_sSyncCycle>>1))
    {   // 如果客户端时间太靠前(在服务器时间的前面)周期的1半时间，时间就停在这里，不再前进
        if (!RenderNetSnapshot())// 更新服务器数据到客户端
        {
            break;
        }
		// ERR2("%d: 缓冲数据过多，快速前进。\n",time(NULL),m_SnapshotCmdList.size());
        // LOG(".");
    }
	*/

    // 如果客户端时间还是远远的超过服务器时间，则重新设置客户端时间
    DWORD dwOffset = m_dwLastServerTime + 5000;
    if (m_dwClientTime > dwOffset) {
        LOG4("网络: STOP Client time (%d). %u, [%u, %u].\n", m_dwClientTime - m_dwLastServerTime,
             m_dwClientTime, m_dwPrevServerTime, m_dwLastServerTime);
        m_dwClientTime = dwOffset;
    }

    // 最后一次更新的时间
    m_dwDiffUpdateTime += dwDifMillisecond;
    if (m_dwDiffUpdateTime >= m_sUpdateCycle) {
        // 如果到了更新周期，就把自己的信息更新到服务器中
        m_dwDiffUpdateTime = 0;
        OnUploadPosition(m_dwClientTime);
    }

    // 倒数第二次时间到最后一次时间之间的进度 (百分比)
    float fProgress;
    if (m_dwLastServerTime == m_dwPrevServerTime)
        fProgress = 1.f;
    else
        fProgress =
            ((m_dwClientTime - m_dwPrevServerTime) / (m_dwLastServerTime - m_dwPrevServerTime));

    if (fProgress > 1)
        fProgress = 1;
    m_pWorld->OnNetCurrentProgress(fProgress);
}

bool NetSynchroPosition::RenderNetSnapshot() {
    if (m_SnapshotCmdList.size() == 0) {
        // LOG("错误: 缓冲区中没有数据可读.\n");
        return false;
    }

    short         cmdID;
    CG_CmdPacket* pPacket = *(m_SnapshotCmdList.begin());
    m_SnapshotCmdList.pop_front();
    pPacket->BeginRead();
    pPacket->ReadShort(&cmdID);
    if (cmdID == r2c_snapshot) {
        m_dwSnapshotCount++;

        // 备份时间
        m_dwPrevServerTime = m_dwLastServerTime;

        DWORD sec;
        READb_ulong(sec);

        // 取得服务器时间，同时计算客户端和这个时间的偏移时间
        READb_ulong(m_dwLastServerTime);

        // 第一次读取的时间就是客户端时间
        if (m_dwSnapshotCount == 1) {
            m_dwClientTime = m_dwLastServerTime;
        } else {
            // m_dwClientTime = m_dwPrevServerTime;
        }
        /*
		if(pPacket->GetDataSize() > 10000)
		{
			int t;
			t = 2;
		}
		*/
        OnDownloadSnapshot(pPacket, m_dwLastServerTime);

        GetWorld()->m_pPlayer->_auto_battle();
        GetWorld()->m_pPlayer->_auto_pick_item();
        GetWorld()->m_pPlayer->_auto_sell_item();
    } else if (cmdID == r2c_event) {
        OnDownloadEvent(pPacket);
    }
    // m_dwClientTime = m_dwLastServerTime;
    DEL_ONE(pPacket);

    return true;
}

void NetSynchroPosition::OnDownloadSnapshot(CG_CmdPacket* pPacket, DWORD dwTime) {
    m_pWorld->OnNetDownloadSnapshot(pPacket, dwTime, m_dwPrevServerTime);
}

void NetSynchroPosition::OnDownloadEvent(CG_CmdPacket* pPacket) {
    m_pWorld->OnNetDownloadEvent(pPacket, m_dwClientTime);
}

void NetSynchroPosition::OnUploadPosition(DWORD dwClientTime) {
    if (m_dwAbsClientTime) {
        m_pWorld->OnNetUploadPosition(timeGetTime() - m_dwAbsClientTime);
    } else {
        m_pWorld->OnNetUploadPosition(0);
        m_dwAbsClientTime = timeGetTime();
    }
}

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
