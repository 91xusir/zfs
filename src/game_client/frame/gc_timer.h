
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
        DWORD           dwListenerType; // ����������, 1 GcTimerListener, 2 Callback
        void*           pListener;
    };

public:
    GcTimer();
    ~GcTimer();

    int Run(float fSecond);

    // operate

    // ���һ��ʱ�������,���Ϊ32������
    //   pListener Ϊ��Ҫ������Ϣ�Ķ���
    //   lDelay Ϊ��Ҫ�ӳٵ�ʱ��,����Ϊ��λ
    //   dwPreSend �����ʱ��������ᷢ����Ϣ�ĸ���
    //   ����0��ʾʧ��,���򷵻ؿ��Ƶ�IDֵ
    DWORD AddTimer(GcTimerListener *pListener, float fDelay, DWORD dwPreSend=0);
    DWORD AddTimer(gccbTimerListener cbListener, float fDelay, DWORD dwPreSend=0);

    // �Ƴ�һ��ʱ�������,���ص�ǰ��ʱ���������,����0��ʾû���ҵ�ID����û��ʱ���������
    int RemoveTimer(DWORD dwID);

    // �Ƴ�ĳһ�������ȫ��ʱ�����������,����0��ʾû����Ҫɾ���ģ����򷵻�ɾ������
    int RemoveObject(GcTimerListener *pListener);
    int RemoveObject(gccbTimerListener cbListener);

    // attribute

    // ȡ��ʱ�������������,���Ϊ32������
    DWORD GetTotal()
    {
        return m_dwCount;
    }

protected:
    DWORD       m_dwCount;     // �ܵ�ʱ���������
    DWORD       m_dwMax;       // ����ʱ���������Ŀ
    DWORD       m_dwID;        // ID
    SGcTimer*   m_pCode;
};
