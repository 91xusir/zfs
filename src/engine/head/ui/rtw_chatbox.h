#ifndef _RTUI_CHATBOX_H
#define _RTUI_CHATBOX_H

namespace ui {
class RtwChatBox : public RtwTextBox {
   public:
    RtwChatBox();
    RtwChatBox(const RtwChatBox& object);
    virtual ~RtwChatBox();

    enum piecetype {
        ptPrefix    = 1,  // 前缀
        ptText      = 2,  // 文本
        ptImage     = 3,  // 图像
        ptSeparator = 4,  // 分隔符
        ptBackslash = 5,  // 反斜杠
        ptName      = 6,  // 名称
    };

    // 聊天项结构体
    struct Item {
        std::string text;    // 文本内容
        char*       sender;  // 发送者名字
    };

    // 聊天片段结构体
    struct Piece {
        piecetype type;  // 片段类型
    };

    Item*       GetSelectItem();           // 获取当前选择的聊天项
    Piece*      GetSelectPiece();          // 获取当前选择的聊天片段
    void        ClearByTime();             // 根据时间清除聊天内容
    void        SetFilter(int filter);     // 设置过滤器
    void        SetClearTime(DWORD time);  // 设置清除时间
    std::string GetLast(int iMask);        // 获取指定掩码的最后一条消息
    /*建议使用GetSelectedText替换GetLast函数*/
    //		std::string GetSelectedText() const{return m_curItem?m_curItem->text:"";};
    int  GetScrollnum();
    int  GetScrollRange();
    int  GetScrollPosition();
    void SetScrollnum(int num);
    void SetScrollPosition(int pos);

    // 添加聊天内容
    void Add(int mask, DWORD color, const char* description, char* fromname, char* message,
             long vFromRegionID = 0, bool ischange = true);  //heten  添加换行判定与玩家ID

    // 处理滚动事件
    void WDScroll(DWORD delta, int nStartCount, int& nResultCount);  //heten

    // 插入聊天消息
    bool InsertChatMessage(const std::string& channel, const std::string& sendname,
                           const std::string& text, const RtwPixel color = RtwPixel(0xffffffff),
                           int indexBefore = -1, bool bRefresh = true, void* param = NULL);

   public:
    RtwEventDispatcher EvLSelect;  //左键选择
    RtwEventDispatcher EvRSelect;  //右键选额
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
