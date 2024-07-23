#ifndef CLOCKFRAME_H
#define CLOCKFRAME_H

#include <windows.h> // For Sleep and GetTickCount

class CLockFrame
{
public:
    // ���캯������������ʱ�䣨��λ�����룩
    CLockFrame(DWORD vLockTime);

    // ��������
    ~CLockFrame();

private:
    DWORD mLastTime;   // �ϴμ�¼��ʱ��
    DWORD mLockTime;   // ÿ֡Ӧ������ʱ�䣨��λ�����룩
};

#endif // CLOCKFRAME_H
