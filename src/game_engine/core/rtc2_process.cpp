#include "core/rt2_core.h"

namespace rt2_core {

/*-----------------------------------------------------------------------------
-   RtProcess
-----------------------------------------------------------------------------*/

RtProcess::RtProcess()
{
}

RtProcess::RtProcess(const RtProcess& rhs)
{
}

RtProcess::RtProcess(const char* szType, unsigned int uOrder) :
    m_szType(szType),
    m_bKill(false),
    m_bActive(true),
    m_uProcessFlags(0),
    m_pNext(NULL),
    m_bPaused(false),
    m_bInitialUpdate(true)
{
}

// 注意：只能使用Kill，不要调用Delete，否则会产生错误
RtProcess::~RtProcess()
{
    if (m_pNext)
    {
        m_pNext = NULL;
    }
}

void RtProcess::Kill()
{
    m_bKill = true;
}

// 设定进程相关性
// A->SetNext(B) 表示B会等待A完成后才完成
void RtProcess::SetNext(SmartPtr<RtProcess> nNext)
{
    if (m_pNext)
    {
        m_pNext = NULL;
    }
    m_pNext = nNext;
}

// 这个进程是否附件在管理器上
// 如果标记为附加，则只被管理器调用
bool RtProcess::IsAttached() const
{
    return (m_uProcessFlags & PROCESS_FLAG_ATTACHED)? true : false;
}

void RtProcess::SetAttached(const bool wantAttached)
{
    if (wantAttached)
    {
        m_uProcessFlags |= PROCESS_FLAG_ATTACHED;
    }else
    {
        m_uProcessFlags &= ~PROCESS_FLAG_ATTACHED;
    }
}

/*-----------------------------------------------------------------------------
-   RtProcessManager
-----------------------------------------------------------------------------*/

bool RtProcessManager::HasProcesses()
{
    return !m_ProcessList.empty();
}

void RtProcessManager::Attach(SmartPtr<RtProcess> pProcess)
{
    m_ProcessList.push_back(pProcess);
    pProcess->SetAttached(true);
}

void RtProcessManager::Detach(SmartPtr<RtProcess> pProcess)
{
    m_ProcessList.remove(pProcess);
    pProcess->SetAttached(false);
}

bool RtProcessManager::IsProcessActive(const char* szType)
{
    for (ProcessList::iterator i=m_ProcessList.begin(); i!=m_ProcessList.end(); i++)
    {
        if ((strcmp((*i)->GetType(), szType)==0) && ((*i)->IsDead()==false || (*i)->m_pNext))
        {
            return true;
        }
    }
    return false;
}

void RtProcessManager::DeleteProcessList()
{
    for (ProcessList::iterator i=m_ProcessList.begin(); i!=m_ProcessList.end(); i++)
    {
        Detach(*(i++));
    }
}

void RtProcessManager::UpdateProcesses(int deltaMilliseconds)
{
    ProcessList::iterator i=m_ProcessList.begin();
    SmartPtr<RtProcess> pNext(NULL);

    while (i!=m_ProcessList.end())
    {
        SmartPtr<RtProcess> p = *i;
        ++ i;

        if (p->IsDead())
        {
            // 检查是否有子进程，如果有就添加到进程管理中（也就是激活该进程）
            pNext = p->GetNext();
            if (pNext)
            {
                p->SetNext(SmartPtr<RtProcess>(NULL));
                Attach(pNext);
            }
            Detach(p);
        }else if (p->IsActive() && !p->IsPaused())
        {
            p->OnUpdate(deltaMilliseconds);
        }
    }
}

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
