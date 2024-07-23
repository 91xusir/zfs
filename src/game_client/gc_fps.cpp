#include "gc_fps.h"

CLockFrame::CLockFrame(DWORD vLockTime) // 单位：毫秒
    : mLockTime(vLockTime)
{
    mLastTime = GetTickCount(); // 获取当前时间（以毫秒为单位）
}

CLockFrame::~CLockFrame()
{
    DWORD now = GetTickCount();
    DWORD passTime = now - mLastTime;

    if (passTime < mLockTime)
    {
        // 计算需要睡眠的时间（单位：毫秒）
        DWORD sleepTime = mLockTime - passTime;
        Sleep(sleepTime); // 以毫秒为单位
    }

    // 更新 mLastTime 为当前时间，以准备下一帧
    mLastTime = GetTickCount();
}