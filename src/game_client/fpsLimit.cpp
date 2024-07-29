#include "fpsLimit.h"

CLockFrame::CLockFrame() // Ĭ�Ϲ��캯������
    : mLockTime(1000 / 60) // �趨һ��Ĭ�ϵ�����ʱ�䣬���� 1000 / 60 ����
{
    mLastTime = GetTickCount(); // ��ȡ��ǰʱ�䣨�Ժ���Ϊ��λ��
}

CLockFrame::CLockFrame(DWORD vLockTime) // �������Ĺ��캯������
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
