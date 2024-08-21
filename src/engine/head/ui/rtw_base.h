#ifndef _RTUI_BASE_H
#define _RTUI_BASE_H
//********************************************************************
//	created:	2010.04.06 17:00
//	filename: 	rtw_base.h.
//	author:		ldr123
//	purpose:	游戏ui系统中要用到的一些公共函数和类
//  other:		局部重构添加注释
//********************************************************************
extern unsigned long dwCurrentTime;
#define BEGINTICK() dwCurrentTime = GetTickCount();
#define SHOWTICK()                                                                                 \
    { RtCoreLog().Info("line:%d tick:%d\n", __LINE__, GetTickCount() - dwCurrentTime); }

namespace ui {

#ifndef foreach
#define foreach(type, iter, var)                                                                   \
    for (type::iterator iter = (var).begin(); (iter) != (var).end(); ++(iter))
#endif  // foreach 正向遍历

#ifndef foreach_rev
#define foreach_rev(type, iter, var)                                                               \
    for (type::reverse_iterator iter = (var).rbegin(); (iter) != (var).rend(); ++(iter))
#endif  // foreach_rev 反向遍历

#ifndef Real
#define Real double
#endif

constexpr Real UI_REAL_PRECISION = 0.000001;

#ifndef REAL_EQUAL
#define REAL_EQUAL(a, b)                                                                           \
    (((a) - (b) > -UI_REAL_PRECISION) && ((a) - (b) < UI_REAL_PRECISION)) /*是否相等*/
#endif

constexpr Real UI_PI = 3.1415926535;  //π的值

//设置颜色的alpha值
#define SETCOLORALPHA(obj, tra) (((RtwPixel)obj).SetAlpha(tra))

//控件显示关闭方式
enum EWidgetShowType {
    esNormal = 1 << 0,  //正常模式
    esTrans  = 1 << 1,  //透明模式
    esZoom   = 1 << 2   //缩放模式
};

enum EWidgetFlag {
    wfVisible = 1 << 0,  // 是否可见
    wfGrabMouse =
        1 << 1,  // 是否相应鼠标事件(MouseDown,MouseUp,MouseClick,MouseEnter,MouseLeave,MouseWheel)
    wfMouseMove     = 1 << 2,  // 是否相应鼠标Move事件
    wfDClick        = 1 << 3,  // 鼠标双击事件
    wfKeyboardEvent = 1 << 4,  // 是否相应键盘事件
    //wfMove			= 1 << 5,   // 是否可以移动
    //wfResize			= 1 << 6,   // 是否可以缩放
    wfEnable     = 1 << 7,   // 是否有效
    wfFocus      = 1 << 8,   // 是否能被激活
    wfHint       = 1 << 9,   // 是否显示hint
    wfDragOut    = 1 << 10,  // 是否响应鼠标托出
    wfDragIn     = 1 << 11,  // 是否响应鼠标托入
    wfPick       = 1 << 12,  // 是否阻挡鼠标
    wfDragServer = 1 << 13,  // change [3/17/2009 tooth.shi]
    wfDragClient = 1 << 14,
    //wfActivateParent	= 1 << 7,
    //wfClip			= 1 << 9,   //
    //wfNoClip			= 1 << 10,
    //wfTopMost			= 1 << 11,
    //wfBottomMost		= 1 << 12,
    wfClick = 1 << 14,
    //wfFadeIn			= 1 << 16,
    //wfFadeOut			= 1 << 17,
    //wfGrabMouseMove	= 1 << 18,
    //wfNoParentMouseClip	= 1 << 19,
    //wfNoMouseClip		= 1 << 20,
    //wfFlushString		= 1 << 22,
    //wfAutoSize		= 1 << 23,
    wfInput = wfGrabMouse | wfKeyboardEvent,
    //wfFade		= wfFadeIn | wfFadeOut,
    wfPenetrate = 1 << 30  //穿透
};

//enum EDockStyle
//{
//	dockNone,
//	dockLeft,
//	dockTop,
//	dockRight,
//	dockBottom,
//	dockFill
//};

//enum EAnchorStyle
//{
//	anchorNone		= 0,
//	anchorLeft		= 1,
//	anchorTop		= 2,
//	anchorRight		= 4,
//	anchorBottom	= 8,
//	anchorAll = anchorLeft | anchorTop | anchorRight | anchorBottom
//};

//对齐方式
enum EAlignment {
    alignNear,
    alignCenter,
    alignFar,
    alignUnKnown,
};

//文字对齐
struct SUiTextAlignment {
    EAlignment align : 16;
    EAlignment lineAlign : 16;

    SUiTextAlignment() {
        align     = alignNear;
        lineAlign = alignCenter;
    }

    SUiTextAlignment(EAlignment _align, EAlignment _lineAlign) {
        align     = _align;
        lineAlign = _lineAlign;
    }
};

enum EWidgetType  //控件类型
{
    wtUnknown      = 1,
    wtWidget       = 2,   // wgt
    wtLayer        = 3,   // lyr
    wtLabel        = 4,   // lab
    wtButton       = 5,   // btn
    wtForm         = 6,   // frm
    wtEditBox      = 7,   // edt
    wtScrollBarV   = 8,   // sbv
    wtScrollBarH   = 9,   // sbh
    wtListBox      = 10,  // lst
    wtProgressBar  = 11,  // pgb
    wtComboBox     = 12,  // cmb
    wtTabCtrl      = 13,  // tab
    wtCheckButton  = 14,  // chk
    wtTree         = 15,  // tre
    wtPopupMenu    = 16,  // mnu
    wtMediaFrame   = 17,  // mfm
    wtAliasButton  = 18,  // ali
    wtHtmlView     = 19,  // hvw
    wtChatInputBox = 20,  // chi
    wtChatBox      = 21,  // chb
    wtHud          = 22,  // hud
    wt3dview       = 23,  // 3dview
    wtCount
};

extern std::string g_WidgetTypeShortName[];  //控件类型缩写
extern std::string g_WidgetTypeName[];       //控件类型全名

/*
	实现基本的引用计数功能，如果对象会用到共享，就可以继承此接口
	继承此接口的对象都要通过new生成，不允许包含每个从RtwRefObject继承的类都要做到：
	只使用grab()和drop()来正确维护对象的引用计数，不要使用delete删除对象；
	重载并调用基类的OnBeforeDestroy()，在此函数里完成类成员的释放，而不是在析沟函数里
	原则上，每复制和删除一个RtwRefObject继承的指针，都应该增删引用计数；
	特殊情况是：getXX,operator[]形式的函数不影响引用计数
	*/
class RtwRefObject : public RtObject {
    RT_DECLARE_DYNCREATE(RtwRefObject, RtObject, 0, "ui")

   public:
    RtwRefObject() {
        m_RefCounter     = 0;
        m_bCallDestroyed = false;
        m_bIsDestroying  = false;
        m_base_name="";
    }

    virtual ~RtwRefObject() {
        if (!m_bCallDestroyed) {
            CHECK(0);  // 没有调用OnBeforeDestroy
            // 可能是因为对象不是new出来的，所以不是通过引用计数销毁的
            // 或者继承的类没有调用RtwRefObject::OnBeforeDestroy
        }
    }

    virtual void OnBeforeDestroy() { m_bCallDestroyed = true; }

    void grab() {  //计数加一
        if (!m_bIsDestroying)
            m_RefCounter++;
    }

    void drop();

    int getRefCount() {  //获得引用的次数
        return m_RefCounter;
    }

   private:
    int  m_RefCounter;
    bool m_bCallDestroyed;
    bool m_bIsDestroying;

   protected:
    std::string m_base_name;
};

//丢弃对象(对象使用完毕)
#define DROP_RTUI_OBJECT(ptr)                                                                      \
    {                                                                                              \
        if (ptr) {                                                                                 \
            (ptr)->drop();                                                                         \
            (ptr) = NULL;                                                                          \
        }                                                                                          \
    }

#define UI_ENSURE(clause)                                                                          \
    {                                                                                              \
        if (!(clause))                                                                             \
            return;                                                                                \
    }
#define UI_ENSURE_B(clause)                                                                        \
    {                                                                                              \
        if (!(clause))                                                                             \
            return false;                                                                          \
    }
#define UI_ENSURE_P(clause)                                                                        \
    {                                                                                              \
        if (!(clause))                                                                             \
            return NULL;                                                                           \
    }

//颜色值(带alpha通道)
struct RtwPixel {
    unsigned char r, g, b, a;
    unsigned char rBack, gBack, bBack, aBack;

    RtwPixel() { *(DWORD*)this = 0xffffffff; }

    RtwPixel(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) {
        r     = _r;
        g     = _g;
        b     = _b;
        a     = _a;
        rBack = r;
        gBack = g;
        bBack = b;
        aBack = a;
    }

    RtwPixel(DWORD color) {
        *(DWORD*)this = color;
        rBack         = r;
        gBack         = g;
        bBack         = b;
        aBack         = a;
    }

    //从字符串转换到颜色
    RtwPixel(const char* str) {
        *(DWORD*)this = strtoul(str, NULL, 16);
        rBack         = r;
        gBack         = g;
        bBack         = b;
        aBack         = a;
    }

    operator const DWORD&() const { return *(DWORD*)this; }

    operator DWORD&() { return *(DWORD*)this; }

    RtwPixel& operator+=(const RtwPixel& other) {
        r += other.r;
        g += other.g;
        b += other.b;
        a += other.a;
        rBack = r;
        gBack = g;
        bBack = b;
        aBack = a;
        return *this;
    }

    RtwPixel& operator*=(Real scale) {
        r *= scale;
        g *= scale;
        b *= scale;
        a *= scale;
        rBack = r;
        gBack = g;
        bBack = b;
        aBack = a;
        return *this;
    }

    //设置颜色的alpha值
    RtwPixel SetAlpha(float fAlpha) {
        if (fAlpha > 1.0) {
            fAlpha = 1.0;
        } else if (fAlpha < 0.0) {
            fAlpha = 0.0;
        }

        r = rBack * fAlpha;
        g = gBack * fAlpha;
        b = bBack * fAlpha;
        a = aBack * fAlpha;
        return *this;
    }
};

//大小(控件长宽等)
struct SSize {
    int width, height;

    void Reset() {
        width  = 0;
        height = 0;
    }

    SSize() { Reset(); }

    SSize(int _width, int _height) {
        width  = _width;
        height = _height;
    }

    SSize& operator+=(const SSize& _Other) {
        width += _Other.width;
        height += _Other.height;
        return *this;
    }

    operator bool() const { return width != 0 || height != 0; }

    bool        FromString(const std::string& str);
    std::string ToString();
};

//坐标
struct SPoint {
    int x, y;

    SPoint() {
        x = 0;
        y = 0;
    }

    SPoint(int _x, int _y) {
        x = _x;
        y = _y;
    }

    SPoint& operator+=(const SPoint& _Other) {
        x += _Other.x;
        y += _Other.y;
        return *this;
    }

    bool operator==(const SPoint& _Other) const { return x == _Other.x && y == _Other.y; }

    bool operator!=(const SPoint& _Other) const { return !(*this == _Other); }

    SPoint& operator+=(const SSize& _Size) {
        x += _Size.width;
        y += _Size.height;
        return *this;
    }

    SPoint& operator-() {
        x = -x;
        y = -y;
        return *this;
    }

    bool        FromString(const std::string& str);
    std::string ToString();
};

//边缘
struct SMargin {
    int left, top, right, bottom;

    void Set(int l, int t, int r, int b) {
        left   = l;
        top    = t;
        right  = r;
        bottom = b;
    }

    bool IsEmpty() const { return !left && !top && !right && !bottom; }

    int Width() const { return left + right; }

    int Height() const { return top + bottom; }

    SMargin() {}

    SMargin(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}

    SMargin(const char* str);
    std::string ToString();
};

//矩形
struct RtwRect {
    enum ERectMouseHover  //点在矩形中的方向
    {
        rmhLeft        = 1 << 0,                //左边缘
        rmhTop         = 1 << 1,                //顶边缘
        rmhRight       = 1 << 2,                //右边缘
        rmhBottom      = 1 << 3,                //底边缘
        rmhLeftTop     = rmhLeft | rmhTop,      //左上角
        rmhLeftBottom  = rmhLeft | rmhBottom,   //左下角
        rmhRightTop    = rmhRight | rmhTop,     //右上角
        rmhRightBottom = rmhRight | rmhBottom,  //右下角
        rmhUnknow      = 0
    };

    int left, top, right, bottom;

    RtwRect() { left = top = right = bottom = 0; }

    RtwRect(int _left, int _top, int _right, int _bottom) {
        left   = _left;
        top    = _top;
        right  = _right;
        bottom = _bottom;
    }

    //是否是有效控件
    bool isValid() const { return (right >= left && bottom >= top); }

    //获取宽度
    int getWidth() const { return (right - left); }

    //获取高度
    int getHeight() const { return (bottom - top); }

    //获取中间点
    SPoint getCenter() const { return SPoint((left + right) >> 1, (top + bottom) >> 1); }

    //获取左上点
    SPoint getLeftTop() const { return SPoint(left, top); }

    //获取左下点
    SPoint getLeftBottom() const { return SPoint(left, bottom); }

    //获取右上点
    SPoint getRightTop() const { return SPoint(right, top); }

    //获取右下点
    SPoint getBottomRight() const { return SPoint(bottom, right); }

    SPoint getRightBottom() const { return SPoint(right, bottom); }

    //某点是否在矩阵的边缘(用于鼠标拖拉变大)
    ERectMouseHover IsInBorderline(int nWidth, int x, int y) {
        ERectMouseHover eResult = rmhUnknow;
        if (!IsContained(x, y))  //xy坐标是否在矩形内部,如果不在矩形内部谈何拖拽缩放?
        {
            return eResult;
        }

        if (getWidth() <= nWidth || getHeight() <= nWidth)  //给出的鼠标宽度不应大于矩形的长或宽
        {
            return eResult;
        }

        RtwRect inSide(left + nWidth, top + nWidth, right - nWidth, bottom - nWidth);
        if (!inSide.isValid())  //内部矩形不存在
        {
            return eResult;
        }

        if (inSide.IsContained(x, y))  //xy坐标在内部矩形内部,说明不在拖拽有效区域
        {
            return eResult;
        }

        bool bLeft, bTop, bRight, bBottom;
        bLeft = bTop = bRight = bBottom = false;

        if (x < inSide.left)  //鼠标在左侧
        {
            bLeft = true;
        }

        if (x > inSide.right)  //鼠标在右侧
        {
            bRight = true;
        }

        if (y < inSide.top)  //鼠标在顶部
        {
            bTop = true;
        }

        if (y > inSide.bottom)  //鼠标在底部
        {
            bBottom = true;
        }

        if (bLeft) {
            if (bTop) {
                return rmhLeftTop;
            }

            if (bBottom) {
                return rmhLeftBottom;
            }

            return rmhLeft;
        }

        if (bRight) {
            if (bTop) {
                return rmhRightTop;
            }

            if (bBottom) {
                return rmhRightBottom;
            }

            return rmhRight;
        }

        if (bTop) {
            return rmhTop;
        }

        if (bBottom) {
            return rmhBottom;
        }
        return eResult;
    }

    //某点是否在矩阵内
    bool IsContained(int x, int y) const {
        return ((x >= left) && (x <= right) && (y >= top) && (y <= bottom));
    }

    //某点是否在矩阵内
    bool IsContained(const SPoint& point) const { return IsContained(point.x, point.y); }

    //某矩阵是否在矩阵内
    bool IsContained(const RtwRect& rc) const {
        return IsContained(rc.getLeftTop()) && IsContained(rc.getBottomRight());
    }

    bool IsContain(int x, int y) const {
        return ((x >= left) && (x <= right) && (y >= top) && (y <= bottom));
    }

    bool IsContain(const SPoint& point) const { return IsContain(point.x, point.y); }

    bool IsContain(const RtwRect& rc) const {
        return IsContain(rc.getLeftTop()) && IsContain(rc.getRightBottom());
    }

    bool IsIntersect(const RtwRect& rc) const;

    void Deflate(int val) { Deflate(val, val, val, val); }

    void Deflate(int cx, int cy) { Deflate(cx, cy, cx, cy); }

    //缩小
    void Deflate(int l, int t, int r, int b) {
        left += l;
        top += t;
        right -= r;
        bottom -= b;
    }

    void Deflate(const SMargin& margin) {
        Deflate(margin.left, margin.top, margin.right, margin.bottom);
    }

    //增大
    void Inflate(int _left, int _top, int _right, int _bottom) {
        left -= _left;
        top -= _top;
        right += _right;
        bottom += _bottom;
    }

    void Inflate(int val) { Inflate(val, val, val, val); }

    void Inflate(int cx, int cy) { Inflate(cx, cy, cx, cy); }

    void Inflate(const SSize& size) { Inflate(size.width, size.height, size.width, size.height); }

    //移动
    void Offset(int cx, int cy) {
        left += cx;
        right += cx;
        top += cy;
        bottom += cy;
    }

    void Offset(const SSize& size) { Offset(size.width, size.height); }

    void Offset(const SPoint& point) { Offset(point.x, point.y); }

    void ReSize(const SSize& size);
    void ClipBy(const RtwRect& rc);
    void Align(const RtwRect& TargetRect, EAlignment XAlign, EAlignment YAlign);
    void Expand(const RtwRect& rcOther);

    int Left() { return left; }

    int Top() { return top; }

    bool        FromString(const std::string& str);
    std::string ToString();

    inline RtwRect& operator=(const RtwRect& t) {
        left   = t.left;
        right  = t.right;
        top    = t.top;
        bottom = t.bottom;
        return *this;
    }
};

struct SRect3D {
    RtwRect s;
    float   z;

    SRect3D() noexcept {}
};

inline bool operator==(const RtwRect& rc1, const RtwRect& rc2) noexcept {
    return (rc1.left == rc2.left && rc1.right == rc2.right && rc1.top == rc2.top &&
            rc1.bottom == rc2.bottom);
}

inline bool operator!=(const RtwRect& rc1, const RtwRect& rc2) {
    return !(rc1 == rc2);
}

// 对齐方式(锚点)
enum EAnchorPoint {
    anchorLeft,
    anchorRight,
    anchorTop,
    anchorBottom,
    anchorCenterV,
    anchorCenterH
};

//锚点(用于控件的对齐)
struct SUiAnchor {
    EAnchorPoint Point;
    std::string  RelativeWidgetName;
    EAnchorPoint RelativePoint;
    SSize        Offset;

    void Reset() noexcept {
        Point = anchorLeft;
        RelativeWidgetName.clear();
        RelativePoint = anchorLeft;
        Offset.Reset();
    }

    SUiAnchor() { Reset(); }

    SUiAnchor(EAnchorPoint _Point, const std::string& _RelativeWidgetName,
              EAnchorPoint _RelativePoint, const SSize& _Offset) {
        Point              = _Point;
        RelativeWidgetName = _RelativeWidgetName;
        RelativePoint      = _RelativePoint;
        Offset             = _Offset;
    }
};

// 超链接
enum EHyperLinkType {
    hlinkInvalid,  // 无效
    hlinkFile,     // 链接到文件
    hlinkWidget,   // 表示超链接指向一个UI Widget及其所有子元素
    hlinkString,   // 链接到一个字符串
    hlinkClient,   // ui_layer_main中处理超链接用
    hlinkRServer,  //可能是指向服务器的超链接
    hlinkURL,      // 打开URL
    hlinkNpc       //可能是指向游戏中的NPC
};

enum EChildHyperLinkType {
    hlinkChildInvalid,
    hlinkTask,  //任务超链接
    hlinkItem   //道具超链接
};

//超链
struct SHyperLink {
    EHyperLinkType      Type;
    EChildHyperLinkType childType;
    std::string         Text;

    RtwPixel normalColor;  //正常
    RtwPixel hilghtColor;  //高亮
    RtwPixel pushedColor;  //按下

    std::vector<DWORD> vecParam;

    // 默认构造函数，调用 Reset() 初始化成员
    SHyperLink() { Reset(); }

    // 带参数构造函数
    SHyperLink(EHyperLinkType _Type, const std::string& _Text)
        : Type(_Type), Text(_Text), childType(hlinkChildInvalid) {
        // 初始化其他成员
        normalColor = hilghtColor = pushedColor = RtwPixel();  // 使用默认构造函数
    }

    // 拷贝构造函数
    SHyperLink(const SHyperLink& other)
        : Type(other.Type), childType(other.childType), Text(other.Text),
          normalColor(other.normalColor), hilghtColor(other.hilghtColor),
          pushedColor(other.pushedColor), vecParam(other.vecParam) {}

    void Reset() noexcept {
        Type        = hlinkInvalid;
        childType   = hlinkChildInvalid;
        Text        = "";
        normalColor = hilghtColor = pushedColor = -1;
        vecParam.clear();
    }

    SHyperLink& operator=(const SHyperLink& other) {
        if (this != &other) {  // 防止自赋值
            Type        = other.Type;
            childType   = other.childType;
            Text        = other.Text;
            normalColor = other.normalColor;
            hilghtColor = other.hilghtColor;
            pushedColor = other.pushedColor;
            vecParam    = other.vecParam;  // 直接赋值，避免重复拷贝
        }
        return *this;
    }

    // 析构函数
    ~SHyperLink() {}

    bool        FromString(const std::string& str);
    std::string ToString();
};

struct SHyperLink_DefaultStyle {
    RtwPixel NormalColor;
    RtwPixel HighlightColor;
    bool     bUnderLine;
};
}  // namespace ui
#endif
