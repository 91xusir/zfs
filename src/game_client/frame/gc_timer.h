
typedef unsigned long       DWORD;      


typedef void (*gccbTimerListener)(DWORD dwID);

class GcTimerListener
{
public:
    GcTimerListener(){}
    virtual ~GcTimerListener(){}

    virtual void OnTimerEvent(DWORD dwID){}

};

class GcTimer
{
    struct SGcTimer
    {
        DWORD           dwID;
        float           fDelay;
        float           fOldTime;
        float           fNextTime;
        DWORD           dwSendCount;
        DWORD           dwListenerType; // 接收者类型, 1 GcTimerListener, 2 Callback
        void*           pListener;
    };

public:
    GcTimer();
    ~GcTimer();

    int Run(float fSecond);

    // operate

    // 添加一个时间控制器,最多为32个对象
    //   pListener 为需要接收消息的对象
    //   lDelay 为需要延迟的时间,以秒为单位
    //   dwPreSend 在添家时间控制器後发送消息的个数
    //   返回0表示失败,否则返回控制的ID值
    DWORD AddTimer(GcTimerListener *pListener, float fDelay, DWORD dwPreSend=0);
    DWORD AddTimer(gccbTimerListener cbListener, float fDelay, DWORD dwPreSend=0);

    // 移除一个时间控制器,返回当前的时间控制器数,返回0表示没有找到ID或者没有时间控制器了
    int RemoveTimer(DWORD dwID);

    // 移除某一个对象的全部时间控制器对象,返回0表示没有需要删除的，否则返回删除个数
    int RemoveObject(GcTimerListener *pListener);
    int RemoveObject(gccbTimerListener cbListener);

    // attribute

    // 取得时间控制器的总数,最多为32个对象
    DWORD GetTotal()
    {
        return m_dwCount;
    }

protected:
    DWORD       m_dwCount;     // 总的时间控制器数
    DWORD       m_dwMax;       // 最大的时间控制器数目
    DWORD       m_dwID;        // ID
    SGcTimer*   m_pCode;
};
