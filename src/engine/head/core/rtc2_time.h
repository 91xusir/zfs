/**
* @file rtc2_time.h 时间相关
* @author janhail
*/

namespace rt2_core {

/*-----------------------------------------------------------------------------
-     时间相关函数定义
-----------------------------------------------------------------------------*/

RT2_CORE_API void rtSystemTime( int& iYear, int& iMonth, int& iDayOfWeek, int& iDay, int& iHour, int& iMin, int& iSec, int& iMSec );
RT2_CORE_API DWORD rtTimestamp();
RT2_CORE_API const char* rtTimestampStr();
RT2_CORE_API DWORD rtMilliseconds();
RT2_CORE_API DWORD rtSecond();

class RT2_CORE_API RtLocateTime
{
public:
    RtLocateTime();
    RtLocateTime(DWORD dwTime);
    void Update();
    void Update(DWORD dwTime);

public:
    int iYear;
    int iMonth;
    int iDayOfWeek;
    int iDay;
    int iHour;
    int iMin;
    int iSec;
    int iMSec;
};



#ifdef WIN32


    struct time_tick
    {
        time_tick()
        {
            LARGE_INTEGER liFreq;
            QueryPerformanceFrequency(&liFreq);
            m_fFreq = (double)liFreq.QuadPart;
        }

        void begin()
        {
            QueryPerformanceCounter(&m_liBegin);
        }

        double end()
        {
            LARGE_INTEGER liEnd;
            QueryPerformanceCounter(&liEnd);
            return (double)(liEnd.QuadPart - m_liBegin.QuadPart) / m_fFreq;
        }

    private :
        LARGE_INTEGER m_liBegin;
        double        m_fFreq;
    };
#else
    struct time_tick
    {
        void begin()
        {
        }

        double end()
        {
        }
    };
#endif


struct _tick_log
{
    _tick_log(const char* block);
    ~_tick_log();
    char blockName[32];
    time_tick _tick;
};

#define _TICK_PERF_

#ifdef _TICK_PERF_
    #define begin_log_tick(_id)      time_tick tick_##_id; tick_##_id.begin();

    #define end_log_tick(_id, _name) \
        do { \
            float tick_time##_id =  tick_##_id.end() * 1000.f; \
            if (tick_time##_id < 10) { \
                RtCoreLog().Perf("%s use time : ", _name); \
                RtCoreLog().Text(8, "%f ms\n", tick_time##_id); \
            } else if (tick_time##_id < 20) { \
                RtCoreLog().Perf("%s use time : ", _name); \
                RtCoreLog().Text(6, "%f ms\n", tick_time##_id); \
            } else { \
                RtCoreLog().Perf("%s use time : ", _name); \
                RtCoreLog().Text(4, "%f ms\n", tick_time##_id); \
            } \
        } \
        while(false);

    #define end_log_tick2(_id, _name0, _name1) \
        do { \
            float tick_time##_id =  tick_##_id.end() * 1000.f; \
            if (tick_time##_id < 10) { \
                RtCoreLog().Perf("%s : %s use time : ", _name0, _name1); \
                RtCoreLog().Text(8, "%f ms\n", tick_time##_id); \
            } else if (tick_time##_id < 20) { \
                RtCoreLog().Perf("%s : %s use time : ", _name0, _name1); \
                RtCoreLog().Text(6, "%f ms\n", tick_time##_id); \
            } else { \
                RtCoreLog().Perf("%s : %s use time : ", _name0, _name1); \
                RtCoreLog().Text(4, "%f ms\n", tick_time##_id); \
            } \
        } \
        while (false)

    #define tick_begin()             time_tick tick##__FUNCTION__##__LINE__; tick##__FUNCTION__##__LINE__.begin() 
    #define tick_end()               tick##__FUNCTION__##__LINE__.end()
#else
    #define begin_log_tick(_id)      
    #define end_log_tick(_id, _name) 
    #define end_log_tick2(_id, _name0, _name1)
    #define tick_begin()             
    #define tick_end()               
#endif



/*-----------------------------------------------------------------------------
-     CPU周期相关的时间函数请到rtc2_system_info.h里面查找
-----------------------------------------------------------------------------*/


} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
