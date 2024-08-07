#ifndef _RTUI_WIDGET_H
#define _RTUI_WIDGET_H

//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	rtw_widget.h
//	author:		ldr123
//	purpose:	组件基类及组件管理器
//  other:		局部重构添加注释
//********************************************************************

namespace ui {
class RtwWidget;
typedef std::list<RtwWidget*> UiWidgetList;

/* 最基本的窗口部件类
	 从RtwWidget继承的类都要做到：    
	 (RtwRefObject)重载OnBeforeDestroy
	 重载并调用基类的Reset()，完成重置功能(只处理属性,不处理Widget父子关系)
	 重载并调用基类的Create()，完成创建功能
	 重载DrawForeground()和DrawForeground()完成画图功能
	 重载CalcClient()完成重算Client区域的功能
	 常量m_DefaultFlag，窗口部件创建时的默认flag
	 m_Name是一个特殊的属性，而且表示着父子关系
	 Layer也是从RtwWidget继承的，但是又有一些区别，某些情况需要区分
	 用户注册的事件处理函数一定要注意:
		最好只改动自己的数据,不要改动相关g_workspace的数据	
		注册到其他Widget的事件要调用_RegisterEvent，
		在自己销毁时需要反注册所有_UnregisterAllEvents，否则会造成指针非法
	*/
class RtwWidget : public RtwRefObject, public IUiEventReceiver {
    RT_DECLARE_DYNCREATE(RtwWidget, RtwRefObject, 0, "ui")

    friend class CUiWidgetFactory;
    friend class CUiWidgetContainer;
    friend class CUiLayer;
    friend class CUiDesktop;
    typedef RtwRefPtr<RtwWidget> Ptr;

   public:
    RtwWidget();
    virtual ~RtwWidget();

   public:
    virtual void OnBeforeDestroy();
    virtual void Reset();             //重置
    virtual bool Create(DWORD Id);    //创建控件,id为Id.
    virtual void Run(DWORD dwDelta);  //运行(渲染,以dwDelta为sleep,循环)
    virtual void Render(const RtwRect* pClipRect = NULL,
                        bool           bAlwaysRender = false);  //渲染
    virtual void DrawForeground(const RtwRect* pClipRect = NULL);
    virtual void DrawBackground(const RtwRect* pClipRect = NULL);
    virtual bool CloneFrom(const RtwWidget* pWidget);  // 克隆，只克隆构架，不克隆运行时数据
    virtual void OnRemoveChild(RtwWidget* pWidget);
    void         ModifyFlags(int add, int remove);
    void         ModifyFlag(int add, int remove);
    bool         PickWidget(const SPoint& Position, RtwWidget** ppWidget = NULL,
                            DWORD FlagMask = 0x00000000, CUiWidgetContainer* pExcludeWidgets = NULL);
    bool         AddChild(RtwWidget* pWidget);
    void         SetParent(RtwWidget* pWidget);
    bool         AddChild(CUiWidgetContainer* pWidgetContainer);
    bool         RemoveChild(DWORD WidgetID, bool bRecursive = false);
    bool         RemoveChild(const std::string& WidgetName, bool bRecursive);
    bool         FindChild(DWORD WidgetID, RtwWidget** ppWidget = NULL, bool bRecursive = false);
    bool         FindChild(const std::string& WidgetName, RtwWidget** ppWidget = NULL,
                           bool bRecursive = false);
    RtwWidget*   GetChildWidget(const std::string& WidgetName);
    bool RemoveAllChildren(CUiWidgetContainer* pExcludeWidgets = NULL);  // 不删除自动滚动条
    void Enable();
    void Disable();
    void SetName(const std::string& name, const std::string& replacedName = "");
    virtual void SetText(const std::string& text);

    void SetTextColor(RtwPixel color) { m_TextColor = color; }

    void SetTextFont(const std::string& Tfont) { m_FontType = Tfont; }

    void SetFontSize(const int& iFontsize) { m_FontSize = iFontsize; }

    void         SetTextAlignment(SUiTextAlignment& align);
    void         SetWidgetSize(SSize& size);
    void         SetCaption(const std::string& caption);
    virtual void Show();  //modifyed to virtual  by chib 2006-10-24
    virtual void Hide();  //
    void         ShowModal();
    void         ShowAndFocus();
    void         Refresh();
    void         Close();
    void Offset(const SSize& size, bool bTriggerEvent = true, bool bAutoScrollParent = true);
    void Move(const SPoint& pos, bool bTriggerEvent = true, bool bAutoScrollParent = true);
    void Move(const RtwRect& rect, bool bAbs = false, bool bTriggerEvent = true,
              bool bAutoScrollParent =
                  true);  // 如果bAbs == false，则：如果有父亲Widget，区域是相对于父亲Widget的
    void CenterScreen(int nOffsetX = 0, int nOffsetY = 0);
    void SetBackgroundImage(RtwImage* pImage);
    void SetBackgroundColor(RtwPixel color);

    void ClearBackGround() { m_pBackGround = NULL; }

    void SetImageSequence(RtwComplexImage* pImageSequence);

    RtwComplexImage* GetImageSequence() { return m_pImageSequence; }

    void SetHint(RtwWidget* pHint);
    void SetHintText(const std::string& HintText);
    void AutoHintPosition();
    void AutoHintSize();
    void AutoHtmlHintSize();
    void AutoSizeByFont();
    void AutoClampScreen();
    void SetFocus();
    void AddAnchor(SUiAnchor& Anchor, RtwWidget* pRelativeWidget);

    void SetBorderSize(int size) {
        m_BorderSize = size;
        if (m_BorderSize < 0)
            m_BorderSize = 0;
    }

    void SetPadding(SMargin padding) { m_padding = padding; }

    SMargin GetPadding() { return m_padding; }

    void SetBorderColor(RtwPixel color = RtwPixel(DWORD(0x00000000))) { m_BorderColor = color; }

    void SetSoundFile(const std::string& strFile) { m_strSoundFilename = strFile; }

    void SetAutoScrollV(bool bAutoScrollV);
    void SetAutoScrollH(bool bAutoScrollH);

    void SetJoinParentAutoScroll(bool bJoin) { m_bJoinParentAutoScroll = bJoin; }

    void SetBorderpadding(bool bBorderpadding) { m_Borderpadding = bBorderpadding; }

    void SetBorder(RtwImage* pBorder) {}

    RtwImage* GetBorder() { return m_pBackGround; }

    RtwImage* GetBackground() { return m_pBackGround ? m_pBackGround->getRenderImage() : 0; }

    std::string GetQualifiedID() { return m_Name; }

    bool IsKeyPressed(EUiVirtualKey vk) { return (GetKeyState(vk) & 0xFF00) != 0; }

    const void* GetUser1() const { return m_Param1; }

    const void* GetUser2() const { return m_Param2; }

    void BringToTop(){};

    void SetUserData(void* Param1) { m_Param1 = Param1; }

    const void* GetUserData() const { return m_Param1; }

    void MoveResize(const RtwRect& rect, bool bAbs = false);
    void MoveResize(int x, int y, int height, int bottom, bool bAbs = false);

    int GetMouseX() { return m_rcFrame.left; }

    int GetMouseY() { return m_rcFrame.top; }

    bool IsVisible() { return getVisible(); }

    void NextTabStop() {}

    void DefaultChange(ui::RtwWidget* sender, RtwEventDelegate* e) {}

    RtwRect GetClientRect() const { return m_rcClient; }

    void Move(int x, int y) { Move(SPoint(x, y)); }

    void SetParam1(void* Param1) { m_Param1 = Param1; }

    void SetParam2(void* Param2) { m_Param2 = Param2; }

    void SetUltraText1(const std::string& text = "",
                       SUiTextAlignment&  align = SUiTextAlignment(alignFar, alignFar),
                       int fontSize = 0, RtwPixel textColor = RtwPixel(0xffffffff));
    void SetUltraText2(const std::string& text = "",
                       SUiTextAlignment&  align = SUiTextAlignment(alignNear, alignNear),
                       int fontSize = 0, RtwPixel textColor = RtwPixel(0xffffffff));

    void SetMaskValue(Real MaskValue) { m_MaskValue = MaskValue; }

    void SetMaskColor(const RtwPixel& color) { m_MaskColor = color; }

    virtual RtwRect CalcChildrenRect();  // 计算子Widget的最小区域
    void            PrintToConsole(const std::string& prefix = "");
    void            CalcAutoScroll();  // 重新计算滚动条的值

    void SetHightLight(bool highlight) { mbHighLight = highlight; }

    int getId() const { return m_Id; }

    EWidgetType getWidgetType() const { return m_WidgetType; }

    const std::string& getName() const { return m_Name; }

    const std::string GetID() const;
    const std::string getShortName() const;

    int GetFlags() const { return m_Flag; }

    bool GetFlags(int flag);
    bool getFlag(int flag, bool bParentRelated = true);

    std::string getCaption() const { return m_Caption; }

    virtual std::string getText() const { return m_Text; };

    virtual std::string GetText() const { return m_Text; };

    std::string getFont() const { return m_FontType; }

    RtwPixel getTextColor() const { return m_TextColor; }

    SUiTextAlignment getTextAlignment() const { return m_TextAlign; }

    RtwRect getClientRect() const { return m_rcClient; }

    RtwRect GetFrameRect() const { return m_rcFrame; }

    RtwRect getFrameRect() const { return m_rcFrame; }

    RtwRect& getClientRectRef() { return m_rcClient; }

    RtwRect& getFrameRectRef() { return m_rcFrame; }

    SSize getOffsetParentSize() {
        return m_pParent ? SSize(m_rcFrame.left - m_pParent->GetFrameRect().left,
                                 m_rcFrame.top - m_pParent->GetFrameRect().top)
                         : SSize(0, 0);
    }

    bool getEnable();
    bool getVisible(bool bParentRelated = true);

    RtwWidget* getHint() { return m_pHint; }

    const std::string& getHintText() const { return m_HintText; }

    RtwPixel getBackgroundColor() const { return m_BkColor; }

    RtwWidget* GetParent() {
        return (m_pParent && m_pParent->getWidgetType() != wtLayer) ? m_pParent : NULL;
    }

    RtwWidget* getParent() {
        return (m_pParent && m_pParent->getWidgetType() != wtLayer) ? m_pParent : NULL;
    }

    CUiLayer*      getLayer();
    CUiMediaFrame* getMediaFrame();
    bool           isChildOf(RtwWidget* pWidget);

    const std::list<SUiAnchor>* getAnchors() { return &m_Anchors; }

    int getAnchorsCount() { return (int)m_Anchors.size(); }

    RtwVScrollBar* getAutoScrollV() { return m_pAutoScrollV; }

    RtwHScrollBar* getAutoScrollH() { return m_pAutoScrollH; }

    const RtwPixel& getBorderColor() const { return m_BorderColor; }

    const int getBorderSize() const { return m_BorderSize; }

    const SSize& getWidgetSize() const { return m_WidgetSize; }

    bool getJoinParentAutoScroll() const { return m_bJoinParentAutoScroll; }

    CUiWidgetContainer* getChildren() { return m_pChildren; }

    Real getMaskValue() { return m_MaskValue; }

    RtwPixel getMaskColor() { return m_MaskColor; }

    const std::string getSoundFile() const { return m_strSoundFilename; }

    const int getDefaultLineSpace() const { return 2; }

    const void* getParam1() const { return m_Param1; }

    const void* getParam2() const { return m_Param2; }

    bool getMlight() { return m_light; }

    void setMlight(bool mlight) { m_light = mlight; }

    void SetUseSystemFont(bool bUse) { m_bIsUseSystemFont = bUse; }

    void OnMove_AnchorWidget(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void ResetHint(bool bShow);

   protected:
    void         OnFrameSizeChanged();  // Frame的大小变化时的响应函数
    void         OnNameChanged(const std::string& OldFullName);  // 名字发生改变时的回掉
    virtual void CalcClient();  // Frame的大小变化时，要计算Client的位置和大小

    bool    m_bShowing;
    bool    m_bHiding;
    bool    m_bTrans;
    bool    m_bZoom;
    int     m_nShowTime;           //效果显示时间（ms）
    int     m_nShowEffectLst;      //
    int     m_nHideEffectLst;      //
    int     m_nShowEffectLstBack;  //
    int     m_nHideEffectLstBack;  //
    RtwRect m_backRc;

    virtual void MoveResizeByPos(float fPos);
    virtual void TransparentByPos(float fPos);
    // 事件
    virtual void OnCreate();
    virtual void OnClose();
    virtual void OnShow();
    virtual void OnHide();
    virtual void OnMove(const RtwRect& OrigFrameRect);
    virtual void OnOffset(const SSize& size);
    virtual void OnHint();
    virtual void OnEditBoxHint();

   public:
    virtual void OnMouseMove(const RtwEventDelegate& e);
    virtual void OnMouseWheel(const RtwEventDelegate& e);
    virtual void OnMouseEnter(const RtwEventDelegate& e);
    virtual void OnMouseLeave(const RtwEventDelegate& e);
    virtual void OnMouseDown(const RtwEventDelegate& e);
    virtual void OnMouseUp(const RtwEventDelegate& e);
    virtual void OnMouseClick(const RtwEventDelegate& e);
    virtual void OnMouseDClick(const RtwEventDelegate& e);
    virtual void OnDraw();
    virtual void OnKeyDown(const RtwEventDelegate& e);
    virtual void OnKeyUp(const RtwEventDelegate& e);
    virtual void OnKeyChar(const RtwEventDelegate& e);
    virtual void OnDragIn(const RtwEventDelegate& e);
    virtual void OnDragOut(const RtwEventDelegate& e);
    virtual void OnDragNull(const RtwEventDelegate& e);
    virtual void OnMouseCapture(const RtwEventDelegate& e);
    virtual void SetTransparency(const Real& Transparency);

   public:
    bool m_MouseLeave;
    bool m_bHint;  // 是否自定义hint
    int  m_nBackGroundAlpha;
    Real m_Transparency;

    virtual void SetWidgetEffect(int nShowEffect = esNormal, int nHideEffect = esNormal,
                                 int nDelay = 300);
    void         SetOwnHint(RtwWidget* pHint = NULL);  //

    void SetHoverBoderSize(SMargin BorderMargin) {
        if (!BorderMargin.IsEmpty()) {
            m_BorderMargin = BorderMargin;
        }
    }

    void SetHoverBoderImage(const string& strBoderFile, RtwRect srcRc, SMargin BorderMargin);

   protected:
    SMargin   m_BorderMargin;
    RtwImage* m_pHoverBorderImage;
    string    m_strHoverBorderFile;

    static const int m_DefaultFlag = wfVisible | wfEnable | wfPick | wfDClick;
    static const int Max_Width = 6 * 40;

    CUiWidgetContainer* m_pChildren;  // 子窗口容器
    RtwWidget* m_pParent;  // 父窗口（不改变父窗口的引用计数，由父窗口来设置这个变量）
    RtwWidget*       m_pHint;        // Hint窗口
    int              m_Id;           // 唯一ID
    EWidgetType      m_WidgetType;   // 窗口部件类型
    std::string      m_Name;         // 窗口部件名字(名字中不准有以下字符: ".$[]#")
    int              m_Flag;         // 标志
    std::string      m_Caption;      // 标题
    std::string      m_Text;         // 窗口文字
    std::string      m_FontType;     // 字体
    int              m_FontSize;     // 字体大小
    SUiTextAlignment m_TextAlign;    // 字体对其方式
    RtwImage*        m_pBackGround;  // 背景图
    RtwComplexImage* m_pImageSequence;    // 图片序列
    RtwPixel         m_TextColor;         // 文字前景颜色
    bool             m_bTextUnderline;    // 文字是否下划线
    RtwPixel         m_BkColor;           // 背景颜色
    RtwRect          m_rcClient;          // 客户区域
    RtwRect          m_rcFrame;           // 窗口区域
    std::string      m_HintText;          // Hint文本
    RtwPixel         m_BorderColor;       //
    int              m_BorderSize;        //
    SMargin          m_padding;           //border 以外的非客户区
    std::string      m_strSoundFilename;  // 点击时候播放的声音
    std::list<SUiAnchor>
        m_Anchors;  // 锚点 (注册锚点对象的EvMove,如果锚点是屏幕,注册Desktop的EvMove)
    SSize          m_WidgetSize;             // Widget大小
    bool           m_bJoinParentAutoScroll;  // 是否参与父亲Widget的自动滚动
    bool           m_bAutoScrollV;           // 自动滚动V
    bool           m_bAutoScrollH;           // 自动滚动H
    RtwVScrollBar* m_pAutoScrollV;
    RtwHScrollBar* m_pAutoScrollH;
    bool
        m_bCalcAutoScrollFlag;  // 是否计算自动滚动(有些特殊情况下不能计算自动滚动，譬如响应自动滚动消息的时候)
    bool m_Borderpadding;       //是否减去Border值
    bool m_bNeedSortForRender;  // 下次Render时是否需要排序
    void _SortChildrenForRender();
    bool m_bIsHintWidget;  // 是否为显示Hint用的Widget

    Real     m_MaskValue;  // 蒙板的值
    RtwPixel m_MaskColor;  // 蒙板的颜色

    bool m_bIsUseSystemFont;  // 是否使用系统的粗体，阴影字体

    struct SUltraText {
        std::string      Text;
        SUiTextAlignment Align;
        int              FontSize;
        RtwPixel         TextColor;
    };

    SUltraText m_UltraText1;
    SUltraText m_UltraText2;

    struct SRegisteredEvent {
        DWORD               WidgetId;
        RtwEventDispatcher* pDispatcher;
        CUiDelegate         Delegate_;
    };

    std::list<SRegisteredEvent> m_RegisteredEvents;  // 在其他地方注册过的事件，在退出后要清除掉

    virtual void _DoAutoScrollV(int OriginalValue);
    virtual void _DoAutoScrollH(int OriginalValue);
    SSize        m_AutoScrollOffset;  // 自动滚动的偏移量

    void* m_Param1;  // 用户参数
    void* m_Param2;  // 用户参数
    bool  m_light;

   public:
    enum { INVALIDE_RENDERTARGET = 0 };

    void SetWidgetRenderTarget(int nTargetIndex);

    void SetClearRenderTarget(bool bEnable, long nBackDiffuse) {
        mbClearRenderTaget = bEnable;
        mlBackBufferDiffuse = nBackDiffuse;
    }

    void SetUseParentClipRect(bool bRect) { mbNoClipRect = bRect; }

    void SetOnlyRenderToShow(bool bOnly) { mbOnlyRenderToShow = bOnly; }

   protected:
    int  mnRenderTargetIndex;
    bool mbClearRenderTaget;
    long mlBackBufferDiffuse;
    bool mbNoClipRect;
    bool mbOnlyRenderToShow;
    bool mbHighLight;

    void ResetScrollBar();

   private:
    void _ResetSelf();

    void _SetParent(RtwWidget* pParent);
    void _AddNamePrefix(const std::string& prefix);
    void _RemoveNamePrefix(const std::string& prefix);

    void _RegisterEvent(RtwWidget* pWidget, RtwEventDispatcher* _pDispatcher,
                        CUiDelegate& _Delegate);
    void _UnregisterAllEvents();

    void OnAutoScrollV(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnAutoScrollH(RtwWidget* pWidget, RtwEventDelegate* pEvent);

    // 可以注册的事件
   public:
    RtwEventDispatcher EvCreate;
    RtwEventDispatcher EvClose;
    RtwEventDispatcher EvShow;
    RtwEventDispatcher EvHide;
    RtwEventDispatcher EvMouseLDown;
    RtwEventDispatcher EvMouseLUp;
    RtwEventDispatcher EvMouseRDown;
    RtwEventDispatcher EvMouseRUp;
    RtwEventDispatcher EvLClick;
    RtwEventDispatcher EvRClick;
    RtwEventDispatcher EvLDClick;
    RtwEventDispatcher EvMouseMove;
    RtwEventDispatcher EvMouseWheel;
    RtwEventDispatcher EvKeyDown;
    RtwEventDispatcher EvKey;
    RtwEventDispatcher EvKeyUp;
    RtwEventDispatcher EvKeyChar;
    RtwEventDispatcher EvActivate;
    RtwEventDispatcher EvDragIn;
    RtwEventDispatcher EvDragOut;
    RtwEventDispatcher EvDragNull;
    RtwEventDispatcher EvDrag;
    RtwEventDispatcher EvDragEnd;
    RtwEventDispatcher EvHint;
    RtwEventDispatcher EvDraw;
    RtwEventDispatcher EvFocus;
    RtwEventDispatcher EvUnFocus;
    RtwEventDispatcher EvMove;
    RtwEventDispatcher EvOffset;  // 大小没有变化，仅仅位置变化

    RtwEventDispatcher EvPostDraw;

    RtwEventDispatcher EvMouseCapture;  // 控件被鼠标左健选中，但是没有右健释放的状态

    static RtwEventDispatcher EvClass_Hint;

   public:
    static void
    ChangeDrawUIName();  //开启绘制UI名字 鼠标悬浮显示 add by lyy 2024.8.7
   private:
    static bool isDrawUIName;
};

class CUiWidgetFactory {
   public:
    CUiWidgetFactory();
    ~CUiWidgetFactory();

   public:
    RtwWidget* createWidget(EWidgetType type, const std::string& name = "");
    DWORD      grabNextId();
    bool       FindWidget(DWORD Id, RtwWidget** ppWidget = NULL);

    DWORD getNextId() const { return m_NextId; }

    int getLiveWidgetCount() const { return (int)m_LiveWidgets.size(); }

   private:
    DWORD m_NextId;
    // 以下为了统计生产的Widget和销毁的Widget
   public:
    EXT_SPACE::unordered_map<DWORD, RtwWidget*> m_LiveWidgets;
    void                                        OnDeleteWidget(RtwWidget* pWidget);
};

class CUiWidgetContainer : public RtwRefObject {
   public:
    CUiWidgetContainer();
    virtual ~CUiWidgetContainer();

    virtual void OnBeforeDestroy();

    void ClearWidgets();

    bool AddWidget(RtwWidget* pWidget);
    bool InsertTopWidget(RtwWidget* pWidget);
    bool AddWidget(CUiWidgetContainer* pWidgetContainer);
    bool RemoveWidget(DWORD WidgetID, bool bRecursive = false);
    bool RemoveWidget(const std::string& WidgetName, bool bRecursive = false);
    bool FindWidget(DWORD WidgetID, RtwWidget** ppWidget = NULL, bool bRecursive = false);
    bool FindWidget(DWORD WidgetID, UiWidgetList::iterator& iter);
    bool FindWidget(const std::string& WidgetName, RtwWidget** ppWidget = NULL,
                    bool bRecursive = false);
    bool InsertWidget(RtwWidget* pWidget, int indexBefore);
    bool BringWidgetToTop(DWORD WidgetID);
    bool BringWidgetToBottom(DWORD WidgetID);

    UiWidgetList::iterator         Begin();
    UiWidgetList::iterator         End();
    UiWidgetList::reverse_iterator Begin_Reverse();
    UiWidgetList::reverse_iterator End_Reverse();

    int getWidgetCount() { return (int)m_Widgets.size(); }

    UiWidgetList* getWidgetList() { return &m_Widgets; }

    RtwWidget* operator[](int index);

    void Run(DWORD dwDelta);
    void Render(CUiWidgetContainer* pExcludeWidgets = NULL);
    void PrintToConsole(const std::string& name, const std::string& prefix = "");

    bool         PickWidget(const SPoint& Position, RtwWidget** ppWidget = NULL,
                            DWORD FlagMask = 0x00000000, CUiWidgetContainer* pExcludeWidgets = NULL);
    UiWidgetList m_Widgets;

   protected:
    bool RemoveWidget(UiWidgetList::iterator& iter);
};
}  // namespace ui
#endif
