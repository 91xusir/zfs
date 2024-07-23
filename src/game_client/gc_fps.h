#ifndef CLOCKFRAME_H
#define CLOCKFRAME_H

#include "core/rt2_core.h" // For Sleep and GetTickCount

class CLockFrame
{
public:
    // Ĭ�Ϲ��캯��
    CLockFrame(); // Ĭ�Ϲ��캯������

    // �������Ĺ��캯��
    CLockFrame(DWORD vLockTime);

    // ��������
    ~CLockFrame();

private:
    DWORD mLastTime;   // �ϴμ�¼��ʱ��
    DWORD mLockTime;   // ÿ֡Ӧ������ʱ�䣨��λ�����룩
};

#endif // CLOCKFRAME_H
