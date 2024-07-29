#include "fpsLimit.h"

CLockFrame::CLockFrame() // 默认构造函数定义
    : mLockTime(1000 / 60) // 设定一个默认的锁定时间，例如 1000 / 60 毫秒
{
    mLastTime = GetTickCount(); // 获取当前时间（以毫秒为单位）
}

CLockFrame::CLockFrame(DWORD vLockTime) // 带参数的构造函数定义
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
