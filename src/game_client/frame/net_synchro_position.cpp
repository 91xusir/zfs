
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
    m_sSyncCycle   = 0;  // ����������ͬ������(ms)����r2c_snapshot������
    m_sUpdateCycle = 0;  // �ͻ��˸���λ������(ms)����c2r_update_position������
    m_sMaxPileCount =
        0;  // �ڿ�ʼ����ǰ��Ҫ���۵����ݰ����������۵�ʱ��m_bPileSnapshot=0�����������=1
    m_sLimitPileCount = 0;  // �������ݱ��Ľ��ޣ�����������޾���Ϊ�ͻ���̫���ˣ���Ҫ�ȴ�
    m_dwLastServerTime = 0;  // ���һ���յ���Serverʱ��(ms)
    m_dwPrevServerTime = 0;  // �����ڶ����յ���Serverʱ��(ms)
    m_dwClientTime     = 0;  // �ͻ��˵�ʱ��
    m_dwAbsClientTime  = 0;
    m_dwDiffUpdateTime = 0;  // �������һ�θ���λ�õ�ʱ��
    m_dwSnapshotCount  = 0;  // ���µĴ���
    m_bPileSnapshot    = 0;  // �Ƿ��ڻ�������

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
        CHECKEX("����: Clone Packet Error.");
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
        if ((curServerTime - m_dwFirstServerTime) > 30 * 1000)  // 30��
        {
            // �������9�룬����Ϊ��ʹ�ü�����
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

            P_LOGWARN("���������̫���� " + std::to_string(m_SnapshotCmdList.size()) +
                      "�������ͻ��˷������ش��󣬻���̫��");

            ERR2("%d: ���������̫����(%d����)���ͻ��˷������ش��󣬻���̫����\n", (int)time(NULL),
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

    // �������ݱ��볬��2��(�ͻ���ʱ����ܿ�ʼ����)
    if (m_dwSnapshotCount < 2) {
        RenderNetSnapshot();  // ���·��������ݵ��ͻ���
        return;
    }

    m_dwClientTime += dwDifMillisecond;  // �ۼӿͻ���ʱ��

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
    {   // ����ͻ���ʱ��̫��ǰ(�ڷ�����ʱ���ǰ��)���ڵ�1��ʱ�䣬ʱ���ͣ���������ǰ��
        if (!RenderNetSnapshot())// ���·��������ݵ��ͻ���
        {
            break;
        }
		// ERR2("%d: �������ݹ��࣬����ǰ����\n",time(NULL),m_SnapshotCmdList.size());
        // LOG(".");
    }
	*/

    // ����ͻ���ʱ�仹��ԶԶ�ĳ���������ʱ�䣬���������ÿͻ���ʱ��
    DWORD dwOffset = m_dwLastServerTime + 5000;
    if (m_dwClientTime > dwOffset) {
        LOG4("����: STOP Client time (%d). %u, [%u, %u].\n", m_dwClientTime - m_dwLastServerTime,
             m_dwClientTime, m_dwPrevServerTime, m_dwLastServerTime);
        m_dwClientTime = dwOffset;
    }

    // ���һ�θ��µ�ʱ��
    m_dwDiffUpdateTime += dwDifMillisecond;
    if (m_dwDiffUpdateTime >= m_sUpdateCycle) {
        // ������˸������ڣ��Ͱ��Լ�����Ϣ���µ���������
        m_dwDiffUpdateTime = 0;
        OnUploadPosition(m_dwClientTime);
    }

    // �����ڶ���ʱ�䵽���һ��ʱ��֮��Ľ��� (�ٷֱ�)
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
        // LOG("����: ��������û�����ݿɶ�.\n");
        return false;
    }

    short         cmdID;
    CG_CmdPacket* pPacket = *(m_SnapshotCmdList.begin());
    m_SnapshotCmdList.pop_front();
    pPacket->BeginRead();
    pPacket->ReadShort(&cmdID);
    if (cmdID == r2c_snapshot) {
        m_dwSnapshotCount++;

        // ����ʱ��
        m_dwPrevServerTime = m_dwLastServerTime;

        DWORD sec;
        READb_ulong(sec);

        // ȡ�÷�����ʱ�䣬ͬʱ����ͻ��˺����ʱ���ƫ��ʱ��
        READb_ulong(m_dwLastServerTime);

        // ��һ�ζ�ȡ��ʱ����ǿͻ���ʱ��
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
