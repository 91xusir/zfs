#ifndef CLOCKFRAME_H
#define CLOCKFRAME_H

#include <windows.h> // For Sleep and GetTickCount

class CLockFrame
{
public:
    // 构造函数，接受锁定时间（单位：毫秒）
    CLockFrame(DWORD vLockTime);

    // 析构函数
    ~CLockFrame();

private:
    DWORD mLastTime;   // 上次记录的时间
    DWORD mLockTime;   // 每帧应锁定的时间（单位：毫秒）
};

#endif // CLOCKFRAME_H
