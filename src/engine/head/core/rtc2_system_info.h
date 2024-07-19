/**
* @file rtc2_system_info.h 系统信息查询
* @author janhail
*/

namespace rt2_core {

/** CPU相关.
*   和CPU相关的函数，包括CPU周期和CPU特性
*/
class RT2_CORE_API RtCPU
{
public:
    RtCPU();
    void Init();

    /// 取得当前的CPU周期值，只保存64位中的低位
    static inline DWORD GetCycles();
    /// 取得当前的CPU周期值，保存到两个DWORD中
    static inline void GetCyclesEx(DWORD& L, DWORD& H);
    /// 取得当前的秒数
    static inline RtTime GetSeconds();

public:
    // 下面这些变量仅仅支持Windows应用
    BOOL    bIsMMX;
    BOOL    bIsPentiumPro;
    BOOL    bIsKatmai;
    BOOL    bIsK6;
    BOOL    bIs3DNow;
    BOOL    bIsSSE;
    BOOL    bIsSSE2;
    DWORD   dwCPUSpeed;

    // 静态变量
    static BOOL   bTimestamp;
    static double fSecondsPerCycle;
};

/** 开始计数CPU周期.
*      - Timer是一个DWORD值的变量
*      - 开始和结束的过程是一个开关的过程，所以多次开始结束后Timer里面保存的是总和，
*      - 要重新计数，请清零，或者调用rtClockClear
*/
#define rtClock(Timer)          {Timer -= RtCPU::GetCycles();}

/** 结束计数CPU周期.
*      - Timer是一个DWORD值的变量
*/
#define rtUnclock(Timer)        {Timer += RtCPU::GetCycles()-34;}

/** 清除CPU计数器.
*      - Timer是一个DWORD值的变量
*/
#define rtClockClear(Timer)     {Timer = 0; }

/** 清除CPU计数器.
*      - Timer是一个DWORD值的变量
*/
#define rtClockSeconds(Timer)   ((float)(Timer * RtCPU::fSecondsPerCycle))

/** 取得CPU周期，仅在windows下有效.
*   - 由于CPU的流水线作业，为了保证测试的准确度，Inter建议如下
*       @code
*       #define rdtsc _asm _emit 0x0f _asm _emit 0x31
*       #define cpuid _asm _emit 0x0f _asm _emit 0xa2
*       @endcode
*   - 先执行cpuid会开始一个新的处理周期
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
