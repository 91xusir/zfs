#ifndef _RTUI_EVENT_H
#define _RTUI_EVENT_H

namespace ui {
class RtwWidget;

enum EUiButtonBits {
    buttonLeft = 1,
    buttonRight = 2,
    buttonMiddle = 4,
};

enum EUiVirtualKey {
    vkEscape = 0x1B,
    vkBack = 8,
    vkTab = 9,
    vkEnter = 13,
    vkShift = 16,
    vkCtrl = 17,
    vkSpace = 32,
    vkEnd = 35,
    vkHome = 36,
    vkLeft = 37,
    vkUp = 38,
    vkRight = 39,
    vkDown = 40,
    vkDelete = 46,
    vkPageUp = 0x21,
    vkPageDown = 0x22
};

enum EUiEventType {
    weNone,
    etKeyDown,
    etKeyUp,
    etKeyChar,
    weMouseDown,
    etMouseUp,
    weMouseClick,
    etMouseDClick,
    etMouseMove,
    etMouseEnter,
    etMouseLeave,
    etMouseWheel,
    //etDragBegin,
    //etDragEnd,
    //etDragEnter,
    //etDragLeave,
    //etDrag,
    //etDragOut,
    etDragIn,
    etDragNull,  // Drag到一个位置
    //etDragCancel,   // 取消Drag
    etShow,
    etHide,
    weFocusIn,  // 激活
    etUnFocus,  // 反激活
    etEnable,
    etDisable,
    etMove,
    etOffset,  // 大小没有变化，仅仅位置变化
    etResize,
    etCreate,
    etClose,
    etScroll,
    etVScroll,
    etHScroll,
    etSelect,
    etDropDown,    // ComboBox 按下拉按钮
    weUpdateText,  // EditBox,ComboBox更新文本
    etHint,        // 显示hint前
    etSetCursor,
    etDraw,
    etMenuSelect,
    etHyperLink,
    etButtonPressed,  //控件被鼠标左健选中，但是没有右健释放的状态。
    etPostDraw,
    etInputChar,
    etLast
};

struct RtwEventDelegate {
    int   type;
    bool  cancle;  // 如果设置为 true，则停止事件的传播
    bool  del_me;  // 如果设置为 true，则删除这个委托
    void* param1;

    // 联合体，用于存储不同类型的事件数据

    union {
        // 鼠标事件的数据
        struct Mouse {
            int button;  // 鼠标按钮
            int x;       // 鼠标 x 坐标
            int y;       // 鼠标 y 坐标
        } mouse;

        // 鼠标滚轮事件的数据
        struct MouseWheel {
            int delta;  // 滚轮的增量
        } mouseWheel;

        // 键盘事件的数据
        struct Key {
            int code;  // 按键码
        } key;

        // 拖放事件的数据
        struct DragIn {
            RtwWidget* pDragOutWidget;  // 拖动出的部件指针
        } dragIn;

        // 菜单选择事件的数据
        struct MenuSelect {
            int index;  // 菜单项的索引
        } menuSelect;

        // 偏移事件的数据
        struct Offset {
            int width;
            int height;
        } offset;

        // 移动事件的数据
        struct Move {
            int OrigFrame_Left;
            int OrigFrame_Right;
            int OrigFrame_Top;
            int OrigFrame_Bottom;
        } move;

        // 滚动事件的数据
        struct Scroll {
            int OrigValue;  // 原始滚动值
        } scroll;

        // 超链接事件的数据
        struct HyperLink {
            SHyperLink* pHLink;  // 超链接指针
        } hyperLink;

        // 激活事件的数据
        struct Activate {
            RtwWidget* widget;  // 激活的部件指针
        } activate;

        // 动作事件的数据
        struct Action {
            const char* descriptor;  // 动作的描述
        } action;

        // 焦点事件的数据
        struct Focus {
            RtwWidget* widget;  // 获得焦点的部件指针
        } focus;
    };

    // 默认构造函数
    RtwEventDelegate() {
        type = weNone;         // 设置事件类型为无事件
        cancle = false;        // 不取消事件
        del_me = false;        // 不删除委托
        param1 = NULL;         // 参数为空
        scroll.OrigValue = 0;  // 初始化滚动值为 0
    }

    // 带参数的构造函数
    RtwEventDelegate(int t) {
        type = t;              // 设置事件类型
        cancle = false;        // 不取消事件
        del_me = false;        // 不删除委托
        scroll.OrigValue = 0;  // 初始化滚动值为 0
    }

    // 获取用户参数
    void* GetUser1() { return param1; }

    // 设置用户参数
    void SetUser1(void* v) { param1 = v; }
};

class RtwEventDispatcher {
   private:
    void Dispatch(RtwWidget* sender, RtwEventDelegate* e);

   public:
    RtwEventDispatcher::RtwEventDispatcher() { Clear(); }

    void operator()(RtwWidget* sender, const RtwEventDelegate& e) {
        if (!m_delegates.empty()) {
            Dispatch(sender, (RtwEventDelegate*)&e);
        }
    }

    operator bool() { return !m_delegates.empty(); }

    RtwEventDispatcher& operator+=(const CUiDelegate& delegate) {
        if (m_delegates.size() > 200)
            CHECK(0);  // @@@@@

        foreach (std::list<CUiDelegate>, dele, m_delegates) {
            if (delegate.IsEqual(*dele))
                return *this;
        }

        m_delegates.push_back(delegate);
        param.param1 = delegate.m_param;
        return *this;
    }

    //add = from += by tooth.shi
    RtwEventDispatcher& operator=(const CUiDelegate& delegate) {
        if (m_delegates.size() > 200)
            CHECK(0);  // @@@@@

        foreach (std::list<CUiDelegate>, dele, m_delegates) {
            if (delegate.IsEqual(*dele))
                return *this;
        }

        m_delegates.push_back(delegate);
        param.param1 = delegate.m_param;
        return *this;
    }

    RtwEventDispatcher& operator-=(const CUiDelegate& delegate) {
        std::list<CUiDelegate>::iterator dele = m_delegates.begin();
        for (; dele != m_delegates.end();) {
            if (!m_delegates.empty()) {
                if (delegate.IsEqual(*dele)) {
                    dele = m_delegates.erase(dele);
                    param.param1 = 0;
                } else
                    ++dele;
            }
        }

        return *this;
    }

    void Reset() {
        m_delegates.clear();
        param.param1 = 0;
    }

    // begin rtui_event.h [3/15/2009 tooth.shi]
    void ClearAndSetDelegate(const CUiDelegate& delegate) {
        m_delegates.clear();
        m_delegates.push_back(delegate);
    }

    // end rtui_event.h [3/15/2009 tooth.shi]

    //RtwEventDispatcher& operator = (const CUiDelegate& delegate)
    //{
    //	m_delegates.clear();
    //	m_delegates.push_back(delegate);
    //	return *this;
    //}
    // add [3/18/2009 tooth.shi]
    void Clear() {
        m_delegates.clear();
        param.param1 = 0;
    }

    void* m_param1;

    std::list<CUiDelegate> m_delegates;
    RtwEventDelegate       param;
};

}  // namespace ui

#endif  // _RTUI_EVENT_H
