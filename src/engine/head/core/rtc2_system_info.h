/**
* @file rtc2_system_info.h ϵͳ��Ϣ��ѯ
* @author janhail
*/

namespace rt2_core {

/** CPU���.
*   ��CPU��صĺ���������CPU���ں�CPU����
*/
class RT2_CORE_API RtCPU
{
public:
    RtCPU();
    void Init();

    /// ȡ�õ�ǰ��CPU����ֵ��ֻ����64λ�еĵ�λ
    static inline DWORD GetCycles();
    /// ȡ�õ�ǰ��CPU����ֵ�����浽����DWORD��
    static inline void GetCyclesEx(DWORD& L, DWORD& H);
    /// ȡ�õ�ǰ������
    static inline RtTime GetSeconds();

public:
    // ������Щ��������֧��WindowsӦ��
    BOOL    bIsMMX;
    BOOL    bIsPentiumPro;
    BOOL    bIsKatmai;
    BOOL    bIsK6;
    BOOL    bIs3DNow;
    BOOL    bIsSSE;
    BOOL    bIsSSE2;
    DWORD   dwCPUSpeed;

    // ��̬����
    static BOOL   bTimestamp;
    static double fSecondsPerCycle;
};

/** ��ʼ����CPU����.
*      - Timer��һ��DWORDֵ�ı���
*      - ��ʼ�ͽ����Ĺ�����һ�����صĹ��̣����Զ�ο�ʼ������Timer���汣������ܺͣ�
*      - Ҫ���¼����������㣬���ߵ���rtClockClear
*/
#define rtClock(Timer)          {Timer -= RtCPU::GetCycles();}

/** ��������CPU����.
*      - Timer��һ��DWORDֵ�ı���
*/
#define rtUnclock(Timer)        {Timer += RtCPU::GetCycles()-34;}

/** ���CPU������.
*      - Timer��һ��DWORDֵ�ı���
*/
#define rtClockClear(Timer)     {Timer = 0; }

/** ���CPU������.
*      - Timer��һ��DWORDֵ�ı���
*/
#define rtClockSeconds(Timer)   ((float)(Timer * RtCPU::fSecondsPerCycle))

/** ȡ��CPU���ڣ�����windows����Ч.
*   - ����CPU����ˮ����ҵ��Ϊ�˱�֤���Ե�׼ȷ�ȣ�Inter��������
*       @code
*       #define rdtsc _asm _emit 0x0f _asm _emit 0x31
*       #define cpuid _asm _emit 0x0f _asm _emit 0xa2
*       @endcode
*   - ��ִ��cpuid�Ὺʼһ���µĴ�������
*/


#if (defined(_MSC_VER) || defined(__ICL))
inline DWORD RtCPU::GetCycles()
{
    if( bTimestamp ) __asm
    {
        xor   eax,eax             // Required so that VC++ realizes EAX is modified.
        _emit 0x0F                // RDTSC  -  Pentium+ time stamp register to EDX:EAX.
        _emit 0x31                // Use only 32 bits in EAX - even a Ghz cpu would have a 4+ sec period.
        xor   edx,edx             // Required so that VC++ realizes EDX is modified.
    }else
    {
        return 0;
    }
}

inline void RtCPU::GetCyclesEx(DWORD& L, DWORD& H)
{
    if( bTimestamp )
    {
        __asm
        {
            xor   eax,eax   // Required so that VC++ realizes EAX is modified.
            xor   edx,edx   // Required so that VC++ realizes EDX is modified.
            _emit 0x0F      // RDTSC  -  Pentium+ time stamp register to EDX:EAX.
            _emit 0x31      // Use only 32 bits in EAX - even a Ghz cpu would have a 4+ sec period.
            mov   [L],eax   // Save low value.
            mov   [H],edx   // Save high value.
        }
    }else
    {
        L = 0;
        H = 0;
    }
}

inline RtTime RtCPU::GetSeconds()
{
    if( bTimestamp )
    {
        DWORD L,H;
        __asm
        {
            xor   eax,eax   // Required so that VC++ realizes EAX is modified.
            xor   edx,edx   // Required so that VC++ realizes EDX is modified.
            _emit 0x0F      // RDTSC  -  Pentium+ time stamp register to EDX:EAX.
            _emit 0x31      // Use only 32 bits in EAX - even a Ghz cpu would have a 4+ sec period.
            mov   [L],eax   // Save low value.
            mov   [H],edx   // Save high value.
        }
        return ((double)L +  4294967296.0 * (double)H) * fSecondsPerCycle;
    }
    else return RtTime((float)rtSecond());
}
#elif defined(__GNUC__)
inline DWORD RtCPU::GetCycles()
{
        return 1;
}

inline void RtCPU::GetCyclesEx(DWORD& L, DWORD& H)
{
}

inline RtTime RtCPU::GetSeconds()
{
	 return RtTime((float)rtSecond());
}
#endif



} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
