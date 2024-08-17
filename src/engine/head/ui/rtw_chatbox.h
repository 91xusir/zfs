#ifndef _RTUI_CHATBOX_H
#define _RTUI_CHATBOX_H

namespace ui {
class RtwChatBox : public RtwTextBox {
   public:
    RtwChatBox();
    RtwChatBox(const RtwChatBox& object);
    virtual ~RtwChatBox();

    enum piecetype {
        ptPrefix    = 1,  // ǰ׺
        ptText      = 2,  // �ı�
        ptImage     = 3,  // ͼ��
        ptSeparator = 4,  // �ָ���
        ptBackslash = 5,  // ��б��
        ptName      = 6,  // ����
    };

    // ������ṹ��
    struct Item {
        std::string text;    // �ı�����
        char*       sender;  // ����������
    };

    // ����Ƭ�νṹ��
    struct Piece {
        piecetype type;  // Ƭ������
    };

    Item*       GetSelectItem();           // ��ȡ��ǰѡ���������
    Piece*      GetSelectPiece();          // ��ȡ��ǰѡ�������Ƭ��
    void        ClearByTime();             // ����ʱ�������������
    void        SetFilter(int filter);     // ���ù�����
    void        SetClearTime(DWORD time);  // �������ʱ��
    std::string GetLast(int iMask);        // ��ȡָ����������һ����Ϣ
    /*����ʹ��GetSelectedText�滻GetLast����*/
    //		std::string GetSelectedText() const{return m_curItem?m_curItem->text:"";};
    int  GetScrollnum();
    int  GetScrollRange();
    int  GetScrollPosition();
    void SetScrollnum(int num);
    void SetScrollPosition(int pos);

    // �����������
    void Add(int mask, DWORD color, const char* description, char* fromname, char* message,
             long vFromRegionID = 0, bool ischange = true);  //heten  ��ӻ����ж������ID

    // ��������¼�
    void WDScroll(DWORD delta, int nStartCount, int& nResultCount);  //heten

    // ����������Ϣ
    bool InsertChatMessage(const std::string& channel, const std::string& sendname,
                           const std::string& text, const RtwPixel color = RtwPixel(0xffffffff),
                           int indexBefore = -1, bool bRefresh = true, void* param = NULL);

   public:
    RtwEventDispatcher EvLSelect;  //���ѡ��
    RtwEventDispatcher EvRSelect;  //�Ҽ�ѡ��
    RtwEventDispatcher EvScroll;

   private:
    int    m_position;
    int    m_range;
    int    m_Scrollnum;
    Item*  m_curItem;
    Piece* m_curPiece;
    DWORD  m_ClearSystime;
    DWORD  m_currentTime;
    //char   m_buf[128];
    CM_MEMDEF(m_buf, 128)
};
}  // namespace ui
#endif
