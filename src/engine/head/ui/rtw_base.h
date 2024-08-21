#ifndef _RTUI_BASE_H
#define _RTUI_BASE_H
//********************************************************************
//	created:	2010.04.06 17:00
//	filename: 	rtw_base.h.
//	author:		ldr123
//	purpose:	��Ϸuiϵͳ��Ҫ�õ���һЩ������������
//  other:		�ֲ��ع����ע��
//********************************************************************
extern unsigned long dwCurrentTime;
#define BEGINTICK() dwCurrentTime = GetTickCount();
#define SHOWTICK()                                                                                 \
    { RtCoreLog().Info("line:%d tick:%d\n", __LINE__, GetTickCount() - dwCurrentTime); }

namespace ui {

#ifndef foreach
#define foreach(type, iter, var)                                                                   \
    for (type::iterator iter = (var).begin(); (iter) != (var).end(); ++(iter))
#endif  // foreach �������

#ifndef foreach_rev
#define foreach_rev(type, iter, var)                                                               \
    for (type::reverse_iterator iter = (var).rbegin(); (iter) != (var).rend(); ++(iter))
#endif  // foreach_rev �������

#ifndef Real
#define Real double
#endif

constexpr Real UI_REAL_PRECISION = 0.000001;

#ifndef REAL_EQUAL
#define REAL_EQUAL(a, b)                                                                           \
    (((a) - (b) > -UI_REAL_PRECISION) && ((a) - (b) < UI_REAL_PRECISION)) /*�Ƿ����*/
#endif

constexpr Real UI_PI = 3.1415926535;  //�е�ֵ

//������ɫ��alphaֵ
#define SETCOLORALPHA(obj, tra) (((RtwPixel)obj).SetAlpha(tra))

//�ؼ���ʾ�رշ�ʽ
enum EWidgetShowType {
    esNormal = 1 << 0,  //����ģʽ
    esTrans  = 1 << 1,  //͸��ģʽ
    esZoom   = 1 << 2   //����ģʽ
};

enum EWidgetFlag {
    wfVisible = 1 << 0,  // �Ƿ�ɼ�
    wfGrabMouse =
        1 << 1,  // �Ƿ���Ӧ����¼�(MouseDown,MouseUp,MouseClick,MouseEnter,MouseLeave,MouseWheel)
    wfMouseMove     = 1 << 2,  // �Ƿ���Ӧ���Move�¼�
    wfDClick        = 1 << 3,  // ���˫���¼�
    wfKeyboardEvent = 1 << 4,  // �Ƿ���Ӧ�����¼�
    //wfMove			= 1 << 5,   // �Ƿ�����ƶ�
    //wfResize			= 1 << 6,   // �Ƿ��������
    wfEnable     = 1 << 7,   // �Ƿ���Ч
    wfFocus      = 1 << 8,   // �Ƿ��ܱ�����
    wfHint       = 1 << 9,   // �Ƿ���ʾhint
    wfDragOut    = 1 << 10,  // �Ƿ���Ӧ����г�
    wfDragIn     = 1 << 11,  // �Ƿ���Ӧ�������
    wfPick       = 1 << 12,  // �Ƿ��赲���
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
    wfPenetrate = 1 << 30  //��͸
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

//���뷽ʽ
enum EAlignment {
    alignNear,
    alignCenter,
    alignFar,
    alignUnKnown,
};

//���ֶ���
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

enum EWidgetType  //�ؼ�����
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

extern std::string g_WidgetTypeShortName[];  //�ؼ�������д
extern std::string g_WidgetTypeName[];       //�ؼ�����ȫ��

/*
	ʵ�ֻ��������ü������ܣ����������õ������Ϳ��Լ̳д˽ӿ�
	�̳д˽ӿڵĶ���Ҫͨ��new���ɣ����������ÿ����RtwRefObject�̳е��඼Ҫ������
	ֻʹ��grab()��drop()����ȷά����������ü�������Ҫʹ��deleteɾ������
	���ز����û����OnBeforeDestroy()���ڴ˺�����������Ա���ͷţ�������������������
	ԭ���ϣ�ÿ���ƺ�ɾ��һ��RtwRefObject�̳е�ָ�룬��Ӧ����ɾ���ü�����
	��������ǣ�getXX,operator[]��ʽ�ĺ�����Ӱ�����ü���
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
            CHECK(0);  // û�е���OnBeforeDestroy
            // ��������Ϊ������new�����ģ����Բ���ͨ�����ü������ٵ�
            // ���߼̳е���û�е���RtwRefObject::OnBeforeDestroy
        }
    }

    virtual void OnBeforeDestroy() { m_bCallDestroyed = true; }

    void grab() {  //������һ
        if (!m_bIsDestroying)
            m_RefCounter++;
    }

    void drop();

    int getRefCount() {  //������õĴ���
        return m_RefCounter;
    }

   private:
    int  m_RefCounter;
    bool m_bCallDestroyed;
    bool m_bIsDestroying;

   protected:
    std::string m_base_name;
};

//��������(����ʹ�����)
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

//��ɫֵ(��alphaͨ��)
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

    //���ַ���ת������ɫ
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

    //������ɫ��alphaֵ
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

//��С(�ؼ������)
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

//����
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

//��Ե
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

//����
struct RtwRect {
    enum ERectMouseHover  //���ھ����еķ���
    {
        rmhLeft        = 1 << 0,                //���Ե
        rmhTop         = 1 << 1,                //����Ե
        rmhRight       = 1 << 2,                //�ұ�Ե
        rmhBottom      = 1 << 3,                //�ױ�Ե
        rmhLeftTop     = rmhLeft | rmhTop,      //���Ͻ�
        rmhLeftBottom  = rmhLeft | rmhBottom,   //���½�
        rmhRightTop    = rmhRight | rmhTop,     //���Ͻ�
        rmhRightBottom = rmhRight | rmhBottom,  //���½�
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

    //�Ƿ�����Ч�ؼ�
    bool isValid() const { return (right >= left && bottom >= top); }

    //��ȡ���
    int getWidth() const { return (right - left); }

    //��ȡ�߶�
    int getHeight() const { return (bottom - top); }

    //��ȡ�м��
    SPoint getCenter() const { return SPoint((left + right) >> 1, (top + bottom) >> 1); }

    //��ȡ���ϵ�
    SPoint getLeftTop() const { return SPoint(left, top); }

    //��ȡ���µ�
    SPoint getLeftBottom() const { return SPoint(left, bottom); }

    //��ȡ���ϵ�
    SPoint getRightTop() const { return SPoint(right, top); }

    //��ȡ���µ�
    SPoint getBottomRight() const { return SPoint(bottom, right); }

    SPoint getRightBottom() const { return SPoint(right, bottom); }

    //ĳ���Ƿ��ھ���ı�Ե(��������������)
    ERectMouseHover IsInBorderline(int nWidth, int x, int y) {
        ERectMouseHover eResult = rmhUnknow;
        if (!IsContained(x, y))  //xy�����Ƿ��ھ����ڲ�,������ھ����ڲ�̸����ק����?
        {
            return eResult;
        }

        if (getWidth() <= nWidth || getHeight() <= nWidth)  //����������Ȳ�Ӧ���ھ��εĳ����
        {
            return eResult;
        }

        RtwRect inSide(left + nWidth, top + nWidth, right - nWidth, bottom - nWidth);
        if (!inSide.isValid())  //�ڲ����β�����
        {
            return eResult;
        }

        if (inSide.IsContained(x, y))  //xy�������ڲ������ڲ�,˵��������ק��Ч����
        {
            return eResult;
        }

        bool bLeft, bTop, bRight, bBottom;
        bLeft = bTop = bRight = bBottom = false;

        if (x < inSide.left)  //��������
        {
            bLeft = true;
        }

        if (x > inSide.right)  //������Ҳ�
        {
            bRight = true;
        }

        if (y < inSide.top)  //����ڶ���
        {
            bTop = true;
        }

        if (y > inSide.bottom)  //����ڵײ�
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

    //ĳ���Ƿ��ھ�����
    bool IsContained(int x, int y) const {
        return ((x >= left) && (x <= right) && (y >= top) && (y <= bottom));
    }

    //ĳ���Ƿ��ھ�����
    bool IsContained(const SPoint& point) const { return IsContained(point.x, point.y); }

    //ĳ�����Ƿ��ھ�����
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

    //��С
    void Deflate(int l, int t, int r, int b) {
        left += l;
        top += t;
        right -= r;
        bottom -= b;
    }

    void Deflate(const SMargin& margin) {
        Deflate(margin.left, margin.top, margin.right, margin.bottom);
    }

    //����
    void Inflate(int _left, int _top, int _right, int _bottom) {
        left -= _left;
        top -= _top;
        right += _right;
        bottom += _bottom;
    }

    void Inflate(int val) { Inflate(val, val, val, val); }

    void Inflate(int cx, int cy) { Inflate(cx, cy, cx, cy); }

    void Inflate(const SSize& size) { Inflate(size.width, size.height, size.width, size.height); }

    //�ƶ�
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

// ���뷽ʽ(ê��)
enum EAnchorPoint {
    anchorLeft,
    anchorRight,
    anchorTop,
    anchorBottom,
    anchorCenterV,
    anchorCenterH
};

//ê��(���ڿؼ��Ķ���)
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

// ������
enum EHyperLinkType {
    hlinkInvalid,  // ��Ч
    hlinkFile,     // ���ӵ��ļ�
    hlinkWidget,   // ��ʾ������ָ��һ��UI Widget����������Ԫ��
    hlinkString,   // ���ӵ�һ���ַ���
    hlinkClient,   // ui_layer_main�д���������
    hlinkRServer,  //������ָ��������ĳ�����
    hlinkURL,      // ��URL
    hlinkNpc       //������ָ����Ϸ�е�NPC
};

enum EChildHyperLinkType {
    hlinkChildInvalid,
    hlinkTask,  //��������
    hlinkItem   //���߳�����
};

//����
struct SHyperLink {
    EHyperLinkType      Type;
    EChildHyperLinkType childType;
    std::string         Text;

    RtwPixel normalColor;  //����
    RtwPixel hilghtColor;  //����
    RtwPixel pushedColor;  //����

    std::vector<DWORD> vecParam;

    // Ĭ�Ϲ��캯�������� Reset() ��ʼ����Ա
    SHyperLink() { Reset(); }

    // ���������캯��
    SHyperLink(EHyperLinkType _Type, const std::string& _Text)
        : Type(_Type), Text(_Text), childType(hlinkChildInvalid) {
        // ��ʼ��������Ա
        normalColor = hilghtColor = pushedColor = RtwPixel();  // ʹ��Ĭ�Ϲ��캯��
    }

    // �������캯��
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
        if (this != &other) {  // ��ֹ�Ը�ֵ
            Type        = other.Type;
            childType   = other.childType;
            Text        = other.Text;
            normalColor = other.normalColor;
            hilghtColor = other.hilghtColor;
            pushedColor = other.pushedColor;
            vecParam    = other.vecParam;  // ֱ�Ӹ�ֵ�������ظ�����
        }
        return *this;
    }

    // ��������
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
