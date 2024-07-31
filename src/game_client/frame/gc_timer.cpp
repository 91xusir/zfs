
#include "graph/rt_graph.h"
#include "gc_include.h"

GcTimer::GcTimer()
{
    m_dwCount = 0;
    m_dwMax = 32;
    m_dwID = 0;
    m_pCode = RT_NEW SGcTimer[m_dwMax];
    memset(m_pCode, 0, sizeof(SGcTimer)*m_dwMax);
}

GcTimer::~GcTimer()
{
    DEL_ARRAY(m_pCode);
}

int GcTimer::Run(float fSecond)
{
    for (DWORD i=0; i<this->m_dwCount; i++)
    {
        if (GetDevice()->GetAppTime()>=this->m_pCode[i].fNextTime)
        {
            if (this->m_pCode[i].dwListenerType==1)
            {
                ((GcTimerListener*)(m_pCode[i].pListener))->OnTimerEvent(this->m_pCode[i].dwID);
            }else if (this->m_pCode[i].dwListenerType==2)
            {
                ((gccbTimerListener)(m_pCode[i].pListener))(this->m_pCode[i].dwID);
            }
            this->m_pCode[i].fOldTime = GetDevice()->GetAppTime();
            this->m_pCode[i].fNextTime = GetDevice()->GetAppTime()+this->m_pCode[i].fDelay;
        }
        if (this->m_pCode[i].dwSendCount>0)
        {
            if (this->m_pCode[i].dwListenerType==1)
            {
                ((GcTimerListener*)(m_pCode[i].pListener))->OnTimerEvent(this->m_pCode[i].dwID);
            }else if (this->m_pCode[i].dwListenerType==2)
            {
                ((gccbTimerListener)(m_pCode[i].pListener))(this->m_pCode[i].dwID);
            }
            this->m_pCode[i].dwSendCount--;
        }
    }
    return 0;
}


DWORD GcTimer::AddTimer(GcTimerListener *pListener, float fDelay, DWORD dwPreSend)
{
    if (this->m_dwCount>=this->m_dwMax)
    {
        this->m_dwMax += 10;
        SGcTimer* pData = RT_NEW SGcTimer[this->m_dwMax];
        memset(pData, 0, sizeof(SGcTimer)*this->m_dwMax);
        memcpy(pData, m_pCode, sizeof(SGcTimer)*this->m_dwCount);
        DEL_ARRAY(m_pCode);
        m_pCode = pData;
        LOG("Timer:Add Timer Error (Count>=32), Grow 10");
    }

    // dwID 自增
    m_dwID ++;

    // add timer to array
    this->m_pCode[this->m_dwCount].dwID = m_dwID;
    this->m_pCode[this->m_dwCount].fDelay = fDelay;
    this->m_pCode[this->m_dwCount].dwListenerType = 1;
    this->m_pCode[this->m_dwCount].pListener = pListener;
    this->m_pCode[this->m_dwCount].fOldTime = GetDevice()->GetAppTime();
    this->m_pCode[this->m_dwCount].fNextTime = GetDevice()->GetAppTime()+fDelay;
    this->m_pCode[this->m_dwCount].dwSendCount = dwPreSend;
    this->m_dwCount++;

    LOG2("Timer:Add timer %d, Count=%d", m_dwID, this->m_dwCount);
    // return timer dwID
    return m_dwID;
}

DWORD GcTimer::AddTimer(gccbTimerListener cbListener, float fDelay, DWORD dwPreSend)
{
    if (this->m_dwCount>=this->m_dwMax)
    {
        this->m_dwMax += 10;
        SGcTimer* pData = RT_NEW SGcTimer[this->m_dwMax];
        memset(pData, 0, sizeof(SGcTimer)*this->m_dwMax);
        memcpy(pData, m_pCode, sizeof(SGcTimer)*this->m_dwCount);
        DEL_ARRAY(m_pCode);
        m_pCode = pData;
        LOG("Timer:Add Timer Error (Count>=32), Grow 10");
    }

    // dwID 自增
    m_dwID ++;

    // add timer to array
    this->m_pCode[this->m_dwCount].dwID = m_dwID;
    this->m_pCode[this->m_dwCount].fDelay = fDelay;
    this->m_pCode[this->m_dwCount].dwListenerType = 2;
    this->m_pCode[this->m_dwCount].pListener = cbListener;
    this->m_pCode[this->m_dwCount].fOldTime = GetDevice()->GetAppTime();
    this->m_pCode[this->m_dwCount].fNextTime = GetDevice()->GetAppTime()+fDelay;
    this->m_pCode[this->m_dwCount].dwSendCount = dwPreSend;
    this->m_dwCount++;

    LOG2("Timer:Add timer %d, Count=%d", m_dwID, this->m_dwCount);
    // return timer dwID
    return m_dwID;
}

int GcTimer::RemoveTimer(unsigned long dwID)
{
    unsigned long i, j;
    LOG1("Timer:Remove timer %d", dwID);

    // check
    if (this->m_dwCount<=0)
    {
        LOG1("Timer:Remove Timer Error (Count=%d)", this->m_dwCount);
        return 0;
    }

    // find
    for (i=0, j=-1; i<this->m_dwCount; i++)
    {
        if (this->m_pCode[i].dwID==dwID)
        {
            j = i;
            break;
        }
    }

    // move memory
    if (j>=0)
    {
        for (i=j; i<this->m_dwCount-1; i++)
        {
            memcpy(&this->m_pCode[i], &this->m_pCode[i+1], sizeof(SGcTimer));
        }
    }else
    {
        LOG("Timer:Remove Timer Error (Can't find)");
    }

    memset(&this->m_pCode[this->m_dwCount-1], 0, sizeof(SGcTimer));
    this->m_dwCount--;

    // return timer count
    return this->m_dwCount;
}

int GcTimer::RemoveObject(GcTimerListener *pListener)
{
    int i, j;
    for (i=(int)this->m_dwCount-1; i>=0; i--)
    {
        if (m_pCode[i].dwListenerType==1 && pListener==(GcTimerListener*)m_pCode[i].pListener)    // find object
        {
            // move memory
            for (j=i; j<(int)this->m_dwCount-1; j++)
            {
                memcpy(&this->m_pCode[j], &this->m_pCode[j+1], sizeof(SGcTimer));
            }
            this->m_dwCount --;
        }
    }
    return 0;
}

int GcTimer::RemoveObject(gccbTimerListener cbListener)
{
    int i, j;
    for (i=(int)this->m_dwCount-1; i>=0; i--)
    {
        if (m_pCode[i].dwListenerType==2 && cbListener==(gccbTimerListener)m_pCode[i].pListener)    // find object
        {
            // move memory
            for (j=i; j<(int)this->m_dwCount-1; j++)
            {
                memcpy(&this->m_pCode[j], &this->m_pCode[j+1], sizeof(SGcTimer));
            }
            this->m_dwCount --;
        }
    }
    return 0;
}
