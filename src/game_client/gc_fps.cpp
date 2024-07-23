#include "gc_fps.h"

CLockFrame::CLockFrame(DWORD vLockTime) // ��λ������
    : mLockTime(vLockTime)
{
    mLastTime = GetTickCount(); // ��ȡ��ǰʱ�䣨�Ժ���Ϊ��λ��
}

CLockFrame::~CLockFrame()
{
    DWORD now = GetTickCount();
    DWORD passTime = now - mLastTime;

    if (passTime < mLockTime)
    {
        // ������Ҫ˯�ߵ�ʱ�䣨��λ�����룩
        DWORD sleepTime = mLockTime - passTime;
        Sleep(sleepTime); // �Ժ���Ϊ��λ
    }

    // ���� mLastTime Ϊ��ǰʱ�䣬��׼����һ֡
    mLastTime = GetTickCount();
}