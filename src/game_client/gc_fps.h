#ifndef CLOCKFRAME_H
#define CLOCKFRAME_H

#include "core/rt2_core.h" // For Sleep and GetTickCount

class CLockFrame
{
public:
    // 默认构造函数
    CLockFrame(); // 默认构造函数声明

    // 带参数的构造函数
    CLockFrame(DWORD vLockTime);

    // 析构函数
    ~CLockFrame();

private:
    DWORD mLastTime;   // 上次记录的时间
    DWORD mLockTime;   // 每帧应锁定的时间（单位：毫秒）
};

#endif // CLOCKFRAME_H
