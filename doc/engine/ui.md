# UI模块

## 简介![UI](ui/UI.png)

以可视化的界面实现用户与计算机之间得通信。

本UI系统基于图形处理系统（graph）实现。

该产品是一个C++ 静态库，其导出了若干接口，虚基类，及全局函数和全局变量。

## 主要功能

UI部件：创建UI部件，包括按钮，输入框，静态文本框，超链接，选项卡，列表框，树型框，进度条，滚轴，菜单等。

事件处理：采用回调函数实现，一个事件可有多个回调函数，可增减回调函数。

其它：如引用计数，剪切板，UI模板，UI主题，XML解析等。

## 基本流程

首先要创建一个UI部件，

设置其名字，大小，位置，子父类继承关系等属性，或者直接从XML文件中读取其信息，

其次根据其作用不同，为其添加相应事件回调函数。

再调用show()函数将其设为可见状态，最后添加到渲染列表中将其显示到屏幕上。

## 接口与类说明

### 1.   引用计数功能

RtwRefObject，实现基本的引用计数功能，如果对象会用到共享，就可以继承此接口，继承此接口的对象都要通过new生成，不允许包含，每个从RtwRefObject继承的类都要做到：只使用grab()和drop()来正确维护对象的引用计数，不要使用delete删除对象；重载并调用基类的OnBeforeDestroy()，在此函数里完成类成员的释放，而不是在析沟函数里，原则上，每复制和删除一个RtwRefObject继承的指针，都应该增删引用计数；特殊情况是：getXX,operator[]形式的函数不影响引用计数。

类定义如下：

```c++
    class RtwRefObject : public RtObject
    {
       RT_DECLARE_DYNCREATE(RtwRefObject, RtObject, 0, "ui")
       
    public:
       virtual void OnBeforeDestroy()
       /*引用计数加1*/
       void grab()
       /*引用计数减1*/
       void drop();
       /*获得当前的引用计数*/
       int getRefCount();

    };


    class CUiWidgetContainer : public RtwRefObject
    {

    public:

       CUiWidgetContainer();

       virtual ~CUiWidgetContainer();

       //释放所有类成员

       virtual void OnBeforeDestroy();

       //清空所有UI部件

       void ClearWidgets();

       //增加一个UI部件到容器中

       bool AddWidget(RtwWidget* pWidget); 

       //在容器头部插入一个UI部件

       bool InsertTopWidget(RtwWidget* pWidget);

       //合并一个容器

       bool AddWidget(CUiWidgetContainer* pWidgetContainer);

       //删除UI部件

       bool RemoveWidget(DWORD WidgetID, bool bRecursive = false);

       bool RemoveWidget(
       const std::string& WidgetName, 
       bool bRecursive = false
       );
       //找到UI指定UI部件

       bool FindWidget(
       DWORD WidgetID, 
       RtwWidget** ppWidget = NULL,
       bool bRecursive = false);
       bool FindWidget(DWORD WidgetID, UiWidgetList::iterator& iter);
       bool FindWidget(const std::string& WidgetName, 
       RtwWidget** ppWidget = NULL,
       bool bRecursive = false);

       //把已存在的部件移动到顶部
       bool BringWidgetToTop(DWORD WidgetID);
       
       //把已存在的部件移动到底部
       bool BringWidgetToBottom(DWORD WidgetID);
       
       //遍历用到
       UiWidgetList::iterator Begin();

       UiWidgetList::iterator End();

       UiWidgetList::reverse_iterator Begin_Reverse();

       UiWidgetList::reverse_iterator End_Reverse();

       int getWidgetCount() { return (int)m_Widgets.size(); }

       UiWidgetList* getWidgetList() { return &m_Widgets; }

       RtwWidget* operator [](int index);

       void Run(DWORD dwDelta);
       
       void Render(CUiWidgetContainer* pExcludeWidgets = NULL);

       void PrintToConsole(const std::string& name, const std::string& prefix = "");

       //鼠标拣选UI部件

       bool PickWidget(const SPoint& Position, RtwWidget** ppWidget = NULL, DWORD FlagMask = 0x00000000, CUiWidgetContainer* pExcludeWidgets = NULL);
    protected:
       UiWidgetList m_Widgets;
       bool RemoveWidget(UiWidgetList::iterator& iter);

    };
```

### 2.   事件处理类
```c++
//负责添加回调函数的宏

#define  UI_DELEGATE_M(receiver, clazz,handle) (CUiDelegate((IUiEventReceiver*)static_cast<void*>(receiver), (CUiDelegate::Thiscall)(int (clazz::*)(void*, void*))&clazz::handle))

#define UI_DELEGATE_MF(clazz, handle) (CUiDelegateF((CUiDelegate::Cdecl)((void clazz::*)())(&clazz::handle))

#define UI_DELEGATE_F(handle) (CUiDelegateF((CUiDelegate::CdeclF)&handle))

#define RTW_CALLBACK UI_DELEGATE_M

#define RTW_CALLBACK_1   UI_DELEGATE_M

#define RTW_CALLBACK_F(handle) (CUiDelegateF((CUiDelegate::CdeclF)&handle))

 

    /*事件委托（封装了相应的回调函数）*/

    class CUiDelegateF

    {

    public:

       /*回调函数的类型*/

typedef int (IUiEventReceiver::*Thiscall)(void*, void*);

       typedef int (_fastcall IUiEventReceiver::*Fastcall)(void*, void*);

       typedef int (_cdecl IUiEventReceiver::*Cdecl)(void*, void*);  

       typedef int (_cdecl *CdeclF)(void*, void*, void*);

       typedef int (_stdcall IUiEventReceiver::*Stdcall)(void*, void*);

       typedef int (_stdcall *StdcallF)(void*, void*, void*);

 

    protected:

       /*回调函数（六种中的一种）*/

       union

       {

           Thiscall m_handle;

           Thiscall m_thiscall;

           Fastcall  m_fastcall;

           Cdecl   m_cdecl;    

           CdeclF  m_cdeclf;

           Stdcall  m_stdcall;

           StdcallF m_stdcallF;

       };

 

       typedef int (_fastcall CUiDelegateF::*Caller)(IUiEventReceiver*, void*, void*);

 

       volatile Caller m_caller;

 

    private:

       /*以下分别是根据回调函数的类型调用回调函数（六种）*/

       int _fastcall DelegateThiscall(IUiEventReceiver* receiver, void* sender, void* arg)

       int _fastcall DelegateFastcall(IUiEventReceiver* receiver, void* sender, void* arg)

       int _fastcall DelegateCdecl(IUiEventReceiver* receiver, void* sender, void* arg)

       int _fastcall DelegateCdeclF(IUiEventReceiver* receiver, void* sender, void* arg)

       int _fastcall DelegateStdcall(IUiEventReceiver* receiver, void* sender, void* arg)

       int _fastcall DelegateStdcallF(IUiEventReceiver* receiver, void* sender, void* arg)

 

    public:
       /*用以上方式之一调用回调函数*/
       int Signal(const IUiEventReceiver* receiver, const void* sender, const void* arg) const
       /*同上*/
       int operator ()(const IUiEventReceiver* receiver, const void* sender, const void* arg) const
       bool IsNull() const 
       bool IsEqual(CUiDelegateF& Other) const
    protected:
       CUiDelegateF(Thiscall handle, Caller caller);
    public:
       CUiDelegateF();
      
       //各种回调函数

       CUiDelegateF(const CUiDelegateF& copy);

 

       CUiDelegateF(Thiscall handle);

 

       CUiDelegateF(Fastcall handle);

 

       CUiDelegateF(Cdecl handle);

 

       CUiDelegateF(CdeclF handle);

    };

 

    class CUiDelegateM : public CUiDelegateF

    {

    private:

       IUiEventReceiver* m_receiver;

 

    public:

       /*调用回调函数*/

       int Signal(const void* sender, const void* arg) const

       /*调用回调函数*/

       int operator () (const void* sender, const void* arg) const

           

       bool IsEqual(CUiDelegateM& Other) const 

    public:

       CUiDelegateM()

           : m_receiver(NULL)

       {

       }

 

       //各种回调函数

       template<class Q>

       CUiDelegateM(IUiEventReceiver* receiver, Q handle)   

           : CUiDelegateF(handle)

           , m_receiver(receiver)

       {

       }

 

       CUiDelegateM(CdeclF handle)

           : CUiDelegateF(handle)

           , m_receiver(NULL)

       {

       }   

 

       CUiDelegateM(const CUiDelegateF& copy)

           : CUiDelegateF(copy)

           , m_receiver(NULL)

       {

       }

 

       CUiDelegateM(const CUiDelegateM& copy)

           : CUiDelegateF((const CUiDelegateF&)copy)

           , m_receiver(copy.m_receiver)

       {

       }   

    };

 

/*事件的上下文数据（事件的自定义数据局，每个事件有所不同）*/

struct RtwEventDelegate

    {

       int type;

       bool cancle; // if set true, we will stop the event travell(调用完该回调后，事发还调用后面的回调)

       bool del_me; // if set true, we will delete this delegate（是否在调用完所有回调后，删除该回调）

 

       /*自定义数据*/

       union

       {

           …

       }

    };

    

/*事件回调函数的集合(一个事件可能有多个回调函数)*/

    class RtwEventDispatcher

    {

    private:

       /*调用所有回调函数*/

       void Dispatch(RtwWidget* sender, RtwEventDelegate* e);

       /*同上*/

       void operator () (RtwWidget* sender, const RtwEventDelegate& e)

       /*该事件是否有回调函数*/

operator bool ()

       /*为该事件添加一个回调函数*/

       RtwEventDispatcher& operator += (const CUiDelegate& delegate);

       /*同上*/

//add = from += by tooth.shi

       RtwEventDispatcher& operator = (const CUiDelegate& delegate)

       

       /*删除一个回调函数*/

RtwEventDispatcher& operator -= (const CUiDelegate& delegate)

       /*删除所有回调函数*/

       void Reset()

       

       /*增加一个回调函数，并把所有其它的回调函数删除*/

       // begin rtui_event.h [3/15/2009 tooth.shi]

       void ClearAndSetDelegate(const CUiDelegate& delegate);

       

       /*删除所有回调函数*/

       void Clear(){ m_delegates.clear(); }

 

       std::list<CUiDelegate> m_delegates;

 
```

### 3.   UI部件基类

class RtwWidget为最基本的窗口部件类，从RtwWidget继承的类都要做到：

1. (RtwRefObject)重载OnBeforeDestroy()

2. 重载并调用基类的Reset()，完成重置功能(只处理属性和,不处理Widget父子关系)

3. 重载并调用基类的Create()，完成创建功能

4. 重载DrawForeground()和DrawForeground()完成画图功能

5. 重载CalcClient()完成重算Client区域的功能

6. 常量m_DefaultFlag，窗口部件创建时的默认flag

7. m_Name是一个特殊的属性，而且表示着父子关系,如FindWidget(“a.b”,&pWidget)表示从指定XML文件中找出隶属于父窗口a的子窗口b,并将其赋值于pWidget

8. Layer也是从RtwWidget继承的，但是又有一些区别，某些情况需要区分

9. 用户注册的事件处理函数一定要注意:最好只改动自己的数据,不要改动相关g_workspace的数据

10. 注册到其他Widget的事件要调用_RegisterEvent，在自己销毁时需要反注册所有_UnregisterAllEvents，否则会造成指针非法


类定义如下：
```c++

    typedef std::list<RtwWidget*> UiWidgetList;

 

    class RtwWidget : public RtwRefObject, public IUiEventReceiver

    {

       RT_DECLARE_DYNCREATE(RtwWidget, RtwRefObject, 0, "ui")

 

       friend class CUiWidgetFactory;

       friend class CUiWidgetContainer;

       friend class CUiLayer;

       friend class CUiDesktop;

       typedef RtwRefPtr<RtwWidget> Ptr;

 

    public:

       RtwWidget();

       virtual ~RtwWidget();

 

       // Method

    public:

       //类成员的释放

       virtual void OnBeforeDestroy();

       virtual void Reset();

       virtual bool Create(DWORD Id);

       virtual void Run(DWORD dwDelta);

       void Render(const RtwRect* pClipRect = NULL, bool bAlwaysRender = false);

       //绘制UI前景图

       virtual void DrawForeground(const RtwRect* pClipRect = NULL);

       //绘制UI背景图

       virtual void DrawBackground(const RtwRect* pClipRect = NULL);

//克隆，只克隆构架，不克隆运行时数据

virtual bool CloneFrom(const RtwWidget* pWidget); 

//删除一个指定的子部件

       virtual void OnRemoveChild(RtwWidget* pWidget);

       //设置UI部件状态属性，add为增加某种属性，remove为移除某种属性

       inline void ModifyFlags(int add, int remove);

       inline void ModifyFlag(int add, int remove);

       //根椐鼠标当前位置拣选UI部件

bool PickWidget(const SPoint& Position, RtwWidget** ppWidget = NULL, DWORD FlagMask = 0x00000000, CUiWidgetContainer* pExcludeWidgets = NULL);

       //增加一个子部件

       bool AddChild(RtwWidget* pWidget);

       //设置隶属父部件

       void SetParent(RtwWidget* pWidget);

       //以UI容器形式增加多个子部件

       bool AddChild(CUiWidgetContainer* pWidgetContainer);

       //通过部件ID移除部件

       bool RemoveChild(DWORD WidgetID, bool bRecursive = false);

       //通过部件名称移除部件

       bool RemoveChild(const std::string& WidgetName, bool bRecursive); 

       //通过部件ID找到部件

bool FindChild(DWORD WidgetID, RtwWidget** ppWidget = NULL, bool bRecursive = false);

//通过部件名称找到部件

bool FindChild(const std::string& WidgetName, RtwWidget** ppWidget = NULL, bool bRecursive = false);

//通过子部件名称得到子部件指针

       RtwWidget* GetChildWidget(const std::string& WidgetName); 

//移除所有子部件，注意这不删除自动滚动条。

bool RemoveAllChildren(CUiWidgetContainer* pExcludeWidgets = NULL); 

 

       //设置为有效部件

void Enable();

//设置为无效部件

       void Disable();

       //设置名字，只能设置一次，不可改变

       void SetName(const std::string& name, const std::string& replacedName = "");

       //设置窗口文字

       virtual void SetText(const std::string& text);

       //设置文字颜色

       void SetTextColor(RtwPixel color) { m_TextColor = color; }

       //设置字体

       void SetTextFont(const std::string& Tfont) { m_FontType = Tfont;}

       //设置文字大小

       void SetFontSize(const int& iFontsize) { m_FontSize = iFontsize;}

       //设置文字对齐方式

       void SetTextAlignment(SUiTextAlignment& align);

       //设置部件大小

       void SetWidgetSize(SSize& size);

       //设置部件标题

       void SetCaption(const std::string& caption);

       //设置部件可见

       virtual void Show();   modifyed to virtual by chib 2006-10-24 

       //设置部件隐藏

       virtual void Hide(); 

       void ShowModal();

       void ShowAndFocus();

       void Refresh();

       void Close();

       // 仅变化位置

       void Offset(const SSize& size, bool bTriggerEvent = true, bool bAutoScrollParent = true);

       //变化位置,大小也可发生变化。

       void Move(const SPoint& pos, bool bTriggerEvent = true, bool bAutoScrollParent = true);

       void Move(const RtwRect& rect, bool bAbs = false, bool bTriggerEvent = true, bool bAutoScrollParent = true); // 如果bAbs == false，则：如果有父亲Widget，区域是相对于父亲Widget的

       //移动到窗口中心

       void CenterScreen( int nOffsetX = 0 , int nOffsetY = 0 );

       //设置背景图

       void SetBackgroundImage(RtwImage* pImage);

       //设置背景色

       void SetLocationBackgroundColor(RtwPixel color);

       //清空背景图

       void ClearBackGround(){m_pBackGround = NULL;}

       

       //设置图片序列

void SetImageSequence(RtwComplexImage* pImageSequence);

//返回图片序列

       RtwComplexImage* GetImageSequence(){return m_pImageSequence;}

       //设置背景图

       void SetBackgroundImage(IUiComplexImage* pImage);

       //设置Hint窗口

       void SetHint(RtwWidget* pHint);

       void SetHintText(const std::string& HintText);

       void AutoHintPosition();

       void AutoHintSize();

       void AutoHtmlHintSize();

       void AutoSizeByFont();

       void AutoClampScreen();

       //设置激活部件

       void SetFocus();

       //增加锚点

       void AddAnchor(SUiAnchor& Anchor, RtwWidget* pRelativeWidget);

       //设置客户区与窗口边框距离

       void SetBorderSize(int size) { m_BorderSize = size; if (m_BorderSize < 0) m_BorderSize = 0; }

       //设置窗口与客户区之间的非客户区

       void SetPadding(SMargin padding){ m_padding = padding; }

       //得到窗口与客户区之间的非客户区

       SMargin GetPadding() { return m_padding; }

       //设置客户区连框颜色

       void SetBorderColor(RtwPixel color = RtwPixel(DWORD(0x00000000))) { m_BorderColor = color; }

       //设置点击时播放的声音文件

       void SetSoundFile(const std::string& strFile) { m_strSoundFilename = strFile; }

       //设置是否自动滚动

       void SetAutoScrollV(bool bAutoScrollV);

       void SetAutoScrollH(bool bAutoScrollH);

       //设置是否参与父窗口自动滚动

       void SetJoinParentAutoScroll(bool bJoin) { m_bJoinParentAutoScroll = bJoin; }

 

       // add [3/16/2009 tooth.shi]

       void SetBorder(RtwImage* pBorder){}

       RtwImage* GetBorder(){return m_pBackGround;}

       RtwImage* GetBackground() {return m_pBackGround;}

       std::string GetQualifiedID() {return m_Name;}

       //判断键盘按键状态

       bool IsKeyPressed(EUiVirtualKey vk){ return (GetKeyState(vk) & 0xFF00) != 0; }

       //得到用户参数

       const void* GetUser1() const { return m_Param1; }

       const void* GetUser2() const { return m_Param2; }

       void BringToTop(){};

       void SetUserData(void* Param1) { m_Param1 = Param1; }

       const void* GetUserData() const { return m_Param1; }

       //调整窗口大小

       void MoveResize(const RtwRect& rect, bool bAbs = false) ;

       void MoveResize(int x, int y , int height, int bottom, bool bAbs = false);

           //设置用户参数

       void SetParam1(void* Param1) { m_Param1 = Param1; }

       void SetParam2(void* Param2) { m_Param2 = Param2; }

 

       void SetUltraText1(const std::string& text = "", SUiTextAlignment& align = SUiTextAlignment(alignFar, alignFar), int fontSize = 0, RtwPixel textColor = RtwPixel(0xffffffff));

       void SetUltraText2(const std::string& text = "", SUiTextAlignment& align = SUiTextAlignment(alignNear, alignNear), int fontSize = 0, RtwPixel textColor = RtwPixel(0xffffffff));

       //设置蒙板的值

       void SetMaskValue(Real MaskValue) { m_MaskValue = MaskValue; }

       //设置蒙板的颜色

       void SetMaskColor(const RtwPixel& color) { m_MaskColor = color; }

       // 计算子Widget的最小区域

       virtual RtwRect CalcChildrenRect();    

       void PrintToConsole(const std::string& prefix = "");

       // 重新计算滚动条的值

       void CalcAutoScroll();         
 
       void SetHightLight( bool highlight )  { mbHighLight = highlight; }

    public:

       //得到部件唯一的ID

       int getId() const { return m_Id; }

       //得到部件类型

       EWidgetType getWidgetType() const { return m_WidgetType; }

       //得到部件名字

       const std::string& getName() const { return m_Name; }

    protected:

       // Frame的大小变化时的响应函数

       void OnFrameSizeChanged(); 

       // 名字发生改变时的回掉

       void OnNameChanged(const std::string& OldFullName);   
       // Frame的大小变化时，要计算Client的位置和大小

       virtual void CalcClient(); 

       // 事件

       virtual void OnCreate();

       virtual void OnClose();

       virtual void OnShow();

       virtual void OnHide();

       virtual void OnMove(const RtwRect& OrigFrameRect);

       virtual void OnOffset(const SSize& size);

       virtual void OnHint();

       virtual void OnEditBoxHint();

 

    //键盘鼠标事件

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

       virtual void OnDragNull(const RtwEventDelegate& e);

       virtual void DefaultKeyDown(const RtwEventDelegate& e);

       virtual void OnMouseCapture( const RtwEventDelegate& e );

       

       //已经注册的事件

       struct SRegisteredEvent

       {

           DWORD              WidgetId;

           RtwEventDispatcher*      pDispatcher;

           CUiDelegate            Delegate_;

       };

 

// 在其他地方注册过的事件，在退出后要清除掉

       std::list<SRegisteredEvent> m_RegisteredEvents;   

 

       //注册到其它widget事件

void _RegisterEvent(RtwWidget* pWidget, RtwEventDispatcher* _pDispatcher, CUiDelegate& _Delegate);

 

       //反注册所有事件

       void _UnregisterAllEvents();

 

       // 可以注册的事件

    public:

       …

       RtwEventDispatcher EvOffset;          // 大小没有变化，仅仅位置变化

       RtwEventDispatcher EvMouseCapture;    // 控件被鼠标左健选中，但是没有右健释放的状态;

    };

 

    //UI部件工厂

    class CUiWidgetFactory

    {

    public:

       CUiWidgetFactory();

       ~CUiWidgetFactory();

 

    public:

       RtwWidget*  createWidget(EWidgetType type, const std::string& name = "");

       DWORD    grabNextId();

       bool        FindWidget(DWORD Id, RtwWidget** ppWidget = NULL);

       //bool        FindWidget(const std::string& name, RtwWidget** ppWidget = NULL);

 

       DWORD       getNextId() const { return m_NextId; }

       int            getLiveWidgetCount() const { return (int)m_LiveWidgets.size(); }

 

    private:

       DWORD m_NextId;

 

       // 以下为了统计生产的Widget和销毁的Widget

    public:

       EXT_SPACE::hash_map<DWORD, RtwWidget*> m_LiveWidgets;

       void OnDeleteWidget(RtwWidget* pWidget);

 

       //EXT_SPACE::hash_map<const std::string, RtwWidget*> m_LiveWidgets_NameIndex;

       //bool BuildNameIndex();

       //bool AddNameIndex(const RtwWidget* pWidget);

       //bool RemoveNameIndex(const std::string& FullName);

    };

    
```

### 4.   UI各部件的实现
``` c++
rtw_layer:

  CUiLayer            是一个Widget容器

 

rtw_button:

  RtwButton          是继承自Widget的一个按钮类

  CUicheckButton      UI检查铵钮，方框打对勾

  RtwAliasButton       UI技能按钮，可拖动。

 

rtw_chatbox:

  RtwChatBox         聊天显示框，继承自RtwListBox

 

rtw_chatinputbox:

  RtwChatInputBox     文字输入框，继承自RtwTextBox

 

rtw_combobox:

  RtwComboBox       下拉菜单，继承自RtwWidget

 

rtw_font:

  RtwFont            Ui字体类

  IUiFontManager      UI字体管理器

 

rtw_form:

  RtwForm            UI对话框类

 

rtw_htmlreftext

  CuiHtmlText         html文字解析类

 

rtw_htmlview:

  RtwHtmlView html    视窗，继承自己RtwTextBox类。

 

Rtw_hud:

RtwHUD         角色头顶信息，包括显示名字，帮派，血条，说话等。继承自RtwWidget

 

rtw_hyperlink:

  RtwHyperlink        超链接类。

 

rtw_image:

  RtwImage           UI贴图基类,继承自RtwRefObject

  RtwComplexImage     UI动态贴图(有图片的切换)

  CUiImageRegister     UI帖图注册类

  IUiImageFactory      UI工厂类

 

rtw_label:

  RtwLabel            拍卖物品的超链接UI，继承自Widget

 

rtw_listbox:

  RtwListBox         UI列表框，继承自Widget

 

rtw_mailbox:

  RtwMailBox         UI邮件框类，继承自Widget

 

rtw_mediaframe

  CUiMediaFrame      CUiMediaFrame类，继承自RtwWidget

 

rtw_popup_menu

  RtwPopupMenu      UI菜单类,继承自RtwWidget

 

rtw_progressbar

  RtwProgressBar      UI进度条类，继承自RtwWidget

 

rtw_ptr:

  RtwRefPtr           RtwRefPtr类，继承自RtwWidget

 

rtw_richtext:

  CUiRichText         XML文字符读取类，用于文本框类调用。

 

rtw_scrollbar

  RtwScrollBar        滚动条类,继承自RtwWidget

 

rtw_sound:

  IUISound            UI声音基类

  CUiSoundImpl_Rt     UI声音类

 

rtw_tab:

  RtwTab             UI选项卡，继承自RtwWidget

 

rtw_template

  CUiTemplate        UI模版类,继承自RtObject

 

rtw_textbox

  RtwTextBox         UI文本框基类,继承自RtwWidget

 

rtw_theme

  CUiTheme          UI主题类,继承自RtObject

 

rtw_tree

RtwTree            UI树型框类，继承自RtwWidget

 
```

### 5.   UI绘制设备类
``` c++
**UI****绘制设备接口：**

class IUiRenderDevice

    {

       …

    public:

       //对设备进行初使化
       //virtual bool Init() = 0;
       //使设备处于无效状态
       virtual void OnInvalidateDevice() = 0;
       //恢复设备为有效状态
       virtual void OnRestoreDevice() = 0;
       //获取Z值与视窗Rect
       virtual Real getZOrder() = 0;
       virtual RtwRect getViewportRect() = 0;
    
    public:

       //所有的UI绘制都在这两个函数之间
       virtual void BeginRender() = 0;
       virtual void EndRender() = 0;
       //绘制贴图

virtual void DrawImage(RtwImage* pImage,
                       const RtwRect& rc, 
                       const SUiTextAlignment align = SUiTextAlignment(alignNear, alignNear), 
                       const RtwRect* pClipRect = NULL) = 0;

 

//绘制部件边框的贴图
virtual void DrawBorderImage(RtwImage* pImage, const RtwRect& rc, const SMargin& margin, const SUiTextAlignment align = SUiTextAlignment(alignNear,alignNear),const RtwRect* pClipRect = NULL) = 0;

//绘制动态帖图
virtual void DrawComplexImage(IUiComplexImage* image, const RtwRect& rect) = 0;

 
//绘制各种形多边形
virtual void DrawRectangle(const RtwRect& rect, const RtwPixel& color) = 0;
    
virtual void DrawRectangleBorder(const RtwRect& rect,
                                 const RtwPixel& color,
                                 const RtwRect* pClipRect = NULL) = 0;
    
virtual void DrawLine(const SPoint& startPoint, 
                      const SPoint& endPoint, 
                      const RtwPixel& color) = 0;
    
virtual void DrawTriangle(const SPoint& p1, 
                          const SPoint& p2,
                          const SPoint& p3, 
                          const RtwPixel& color) = 0;
virtual void SetTextureRenderTarget( int nTextureSurface , bool bClearUp , long nBackColor ) {}
virtual void RestorRenderTarget( ) {}
};

 

	//**UI****绘制设备的实现：**

    class CUiRenderDeviceImpl_Rt : public IUiRenderDevice

    {

    ...

    };
```

### 6.   UI XML文件解析类
``` c++
class RtwXmlLoader

{

    …

    public:

       …

       //加载XML文件

bool LoadFromFile(const std::string& FileName, 
                  RtwWidget* pParent = NULL, 
                  std::list<DWORD>* pWidgetsCreated = NULL);
       //读入XML字符串

bool LoadFromString(const std::string& strXml,
                    RtwWidget* pParent = NULL, 
                    std::list<DWORD>* pWidgetsCreated = NULL);

     std::list<RtwButton*> m_lFaceButton;
    
    protected:
       RtsXmlDoc* m_pXmlDoc;  //XML文件解析
    public:

       …

    private:

       …

       // 返回一个RtwImage的指针，需要释放！！
       RtwImage* _Process_Image(RtsXmlDoc::NodePtr* pNode); 
	  // 返回一个IUiImage的指针，需要释放！！
       IUiComplexImage* _Process_ComplexImage(RtsXmlDoc::NodePtr* pNode)
}
```

### 7.   UI主类
```c++
//该类为游戏中UI的主类，负责加载UI的XML文件，并对个部件进行初使化，计算每个部件的逻辑并渲染，可直接调全局变量g_workspace，不需额外创建对象。

class CUiDesktop
{

public:

// 鼠标拣选函数，处理顺序：PopupMenu，当前焦点，当前焦点的所有父亲窗口，
// Modal窗口容器，TopRender窗口容器，系统默认层，其他层

bool PickWidget(RtwWidget** ppWidget = NULL, 
CUiLayer** ppLayer = NULL, DWORD FlagMask = 0x00000000, 
UiLayerList* pExcludeLayers = NULL, 
CUiWidgetContainer* pExcludeWidgets = NULL);
       // 设置焦点窗口
       void SetFocusWidget(RtwWidget* pWidget);
       // 设置最上层窗口
       void SetHintWidget(RtwWidget* pWidget);
       //设置鼠标正在拖动的widget
       void SetMouseDragWidget(RtwWidget* pWidget);
       //设置鼠标悬停的widget
       void SetMouseHoverWidget(RtwWidget* pWidget);
       //重置最上层widget
       void ResetHintTargetWidget();
       void ResetHintText() { DROP_RTUI_OBJECT(m_pHintTargetWidget); }
       //显示菜单
       void ShowPopupMenu();
       void ShowPopupMenu(RtwPopupMenu* menu,void* xx,int x,int y);
       void HidePopupMenu();
	  // 当没有焦点时，就把焦点设在此窗口上
	   void SetDefaultFocusWidget(RtwWidget* pDefault); 
       void SetDebugMode(bool bDebugMode) { m_bDebugMode = bDebugMode; }
       void SetRefreshMediaFrame(CUiMediaFrame* pFrame);

       //注册UI贴图与UIXML路径
       bool RegisterTexturePath(const std::string& TexturePath);
       bool RegisterXmlPath(const std::string& XmlPath);
       //返回默认焦点widget
       RtwWidget* getDefaultFocusWidget()
       {
           return m_pFocusWidget;
       }
       //处理鼠标键盘事件

    public:
       bool MouseDown(int button);
       bool MouseUp(int button);
       bool MouseDClick(int button);
       bool MouseMove(int x, int y);
       bool MouseWheel(int delta, int x, int y);
       bool KeyChar(int ch);
       bool KeyDown(int key);
       bool KeyUp(int key);
     // 设置鼠标操作范围
       void SetModeWnd(bool bFlag = false){ m_bModeWnd = bFlag; }; 
    };
```

### 8.   其它类
```c++
**剪切板接口：**

class CClipboard
    {
       …
    public:
       //复制
       virtual bool Copy(const std::string& str) = 0;
       //粘贴
       virtual bool Paste(std::string& outStr) = 0;
    };

	  //**windows****剪切板的类：**
    class CUiClipboardImpl_Windows : public CClipboard
    {

       …

    public: 

       //复制
       virtual bool Copy(const std::string& str); 
       //粘贴
       virtual bool Paste(std::string& outStr);

    };

	// **文本框XML****字符解析类：**

    class CUiRichText
    {
       …
    public:
    //重置
      
       bool Reset();
       //从文件中载入XML
       bool LoadFromFile(const std::string& FileName);
       //读取XML字符串
       bool LoadFromString(const std::string& strXml);

       …

    };
```
##  简单的示例代码
 ```c++
//用工工厂创建一个按钮

RtwWidget *m_Button = g_workspace.getWidgetFactory()->createWidget(wtButton);

//设置按钮名字
     m_Button->SetName("Button");
     //设置按钮显示文本
     m_Button->SetText("Button1");
     //移动按钮位置
     m_Button->Move(10,100);
     //设置按钮大小
     m_Button->SetWidgetSize(SSize(100,20));
     //设置按钮边框距离
     m_Button->SetBorderSize(10);
     //设置按钮边框颜色
     m_Button->SetBorderColor(0xFFC28D3B);
     //设置边框按钮颜色
     m_Button->SetBackgroundColor(0xFFC28D3B);
     //为按钮增加可激活状态
     m_Button->ModifyFlag(wfFocus, 0);
     //设置按钮为有效部件
     m_Button->Enable();
     //设置按钮为可见状态
     m_Button->Show();
     //为按钮添加事件的回调函数
     m_Button->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_Quit));
     //将按钮添加到渲染列表
     g_workspace.AddWidget(m_Button);
 ```

UI属性说明:

```c++
"Name"             // 名称
"Location"         // 位置
"Text"             // 文本
"TextColor"        // 文本颜色
"TextFont"         // 文本字体
"FontSize"         // 字体大小
"Alignment"        // 列对齐
"AlignLine"        // 行对齐
"Show"             // 是否显示
"Active"           // 是否激活
"MouseEvent"       // 鼠标事件
"MouseMove"        // 鼠标移动事件
"Movable"          // 能否移动
----------------------------------
"NormalImage"      // 正常
"PushedImage"      // 按下
"DisabledImage"    // 禁用
"HighlightImage"   // 高亮
"BackgroundImage"  // 背景
----------------------------------
"BackgroundColor"  // 背景颜色
"DragOut"          // 拖出事件
"DragIn"           // 拖入事件
"Enable"           // 是否启用
"Size"             // 尺寸
"Sound"            // 声音
"BorderCorlor"     // 边框颜色
"BorderSize"       // 边框大小
"padding"          // 内边距  padding="0,30,0,0"  左上右下
"Penetrate"        // 是否穿透
"MouseHoverImage"  // 鼠标悬停图像
"MouseHoverSize"   // 鼠标悬停图像大小
"MouseHoverImgRect"// 鼠标悬停图像区域
"Inherit"          // 父类
```