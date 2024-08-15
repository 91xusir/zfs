#ifndef _RTUI_EDITBOX_H
#define _RTUI_EDITBOX_H

//********************************************************************
//	created:	2010.04.08 10:00
//	filename: 	rtw_text.cpp
//	author:		ldr123
//	purpose:	文本输入框
//  other:		局部重构添加注释
//********************************************************************

namespace ui {
//文本框
class RtwTextBox : public RtwWidget {
    RT_DECLARE_DYNCREATE(RtwTextBox, RtwWidget, 0, "ui")

    friend class CUiWidgetFactory;
    friend class CUiWidgetContainer;
    friend class CUiLayer;
    friend class CUiDesktop;

   public:
    enum EEditItemType {
        eit_Text,           //文本
        eit_Image,          //图像
        eit_ImageSequence,  //图片序列
        eit_Name            //名称
    };

    struct SItem {
       public:
        EEditItemType type;        //类型
        std::string   text;        //文本内容
        RtwPixel      color;       //文本颜色
        bool          bUnderLine;  // 是否下划线
        RtwPixel      colorDelta;  // 颜色的每秒钟增量
        SHyperLink    hyperLink;   // 超链接属性
        void*         param1;      // 参数1(浅复制)
        void*         param2;      // 参数2(浅复制)
        long          id;          //heten  保存玩家ID
        int           showcount;   // 循环次数  heten

        //ldr123 支持字体自定义
        std::string strFontName;
        int         nFontSize;

        bool    bDraw;
        RtwRect rect;
        int     DrawOffset;         // 从第几个字符开始显示
        int     SelectBegin;        // 选中的开始字符
        int     SelectEnd;          // 选中的结束字符
        RtwRect rcSelect;           // 选中字符的区域
        Real    colorChangeOwe[4];  // 颜色渐变的赊欠
        SSize   imgSize;            //要显示的大小

        //ldr123 添加高度和宽度函数,用以实现图片的自由大小
        int getLeft() { return rect.left; }

        int getTop() { return rect.top; }

        int getRight() { return rect.right; }

        int getBottom() { return rect.bottom; }

        int getHeight() { return rect.getHeight(); }

        int getWidth() { return rect.getWidth(); }

        //end

        //设置图片(前景图?)
        void SetImage(RtwImage* pImage = NULL, RtwImage* HightImage = NULL,
                      RtwImage* PushedImage = NULL) {
            DROP_TRUI_IMAGE(image);
            image = pImage;
            if (pImage) {
                GRAB_TRUI_IMAGE(pImage);
                image->m_FillMode = ImageFillMode_Stretch;
            }

            DROP_TRUI_IMAGE(pHoverImage);
            pHoverImage = HightImage;
            if (pHoverImage) {
                GRAB_TRUI_IMAGE(pHoverImage);
                pHoverImage->m_FillMode = ImageFillMode_Stretch;
            }

            DROP_TRUI_IMAGE(pPushedImage);
            pPushedImage = PushedImage;
            if (pPushedImage) {
                GRAB_TRUI_IMAGE(pPushedImage);
                pPushedImage->m_FillMode = ImageFillMode_Stretch;
            }
        }

        //图片切换
        virtual void Run(DWORD dwDelta) { IMAGETICK(image, dwDelta); }

        RtwImage* getImage() { return image; }

        //设置图片序列
        void SetImageSequence(RtwComplexImage* pImageSequence) {
            //删除已经存在的序列
            DROP_TRUI_IMAGE(imageSequence);
            imageSequence = pImageSequence;
            if (imageSequence) {
                GRAB_TRUI_IMAGE(imageSequence);
            }
        }

        //获取图片序列
        RtwComplexImage* getImageSequence() { return imageSequence; }

        RtwImage* image;
        RtwImage* pPushedImage;
        RtwImage* pHoverImage;

       private:
        RtwComplexImage* imageSequence;

       public:
        SItem()
            : bDraw(false), bUnderLine(false), color(0xffffffff), colorDelta((DWORD)0x00000000),
              image(NULL), imageSequence(NULL), param2(NULL), param1(NULL), id(0), showcount(3),
              SelectEnd(0), SelectBegin(0), strFontName(""), nFontSize(12), imgSize(SSize(0, 0)),
              pHoverImage(NULL), pPushedImage(NULL) {
            type = eit_Text;  //默认类型,文本
            for (int i = 0; i < 4; ++i)
                colorChangeOwe[i] = 0;
        }

        ~SItem() {
            DROP_TRUI_IMAGE(image);
            DROP_TRUI_IMAGE(imageSequence);
        }

        SItem(const SItem& Other) {
            type = Other.type;
            text = Other.text;
            bUnderLine = Other.bUnderLine;
            color = Other.color;
            colorDelta = Other.colorDelta;
            bDraw = Other.bDraw;
            rect = Other.rect;
            DrawOffset = Other.DrawOffset;
            SelectBegin = Other.SelectBegin;
            SelectEnd = Other.SelectEnd;
            rcSelect = Other.rcSelect;
            hyperLink = Other.hyperLink;
            param1 = Other.param1;
            param2 = Other.param2;
            id = Other.id;  //heten
            image = NULL;
            imageSequence = NULL;
            pPushedImage = NULL;
            pHoverImage = NULL;
            strFontName = Other.strFontName;
            nFontSize = Other.nFontSize;
            imgSize = Other.imgSize;

            if (Other.image) {
                image = Other.image;
                GRAB_TRUI_IMAGE(image);
            }

            if (Other.pPushedImage) {
                pPushedImage = Other.pPushedImage;
                GRAB_TRUI_IMAGE(pPushedImage);
            }

            if (Other.pHoverImage) {
                pHoverImage = Other.pHoverImage;
                GRAB_TRUI_IMAGE(pHoverImage);
            }

            if (Other.imageSequence) {
                imageSequence = Other.imageSequence;
                GRAB_TRUI_IMAGE(imageSequence);
            }

            for (int i = 0; i < 4; ++i)
                colorChangeOwe[i] = Other.colorChangeOwe[i];
        }

        //赋值操作符
        const SItem& operator=(const SItem& Other) {
            type = Other.type;
            text = Other.text;
            bUnderLine = Other.bUnderLine;
            color = Other.color;
            colorDelta = Other.colorDelta;
            bDraw = Other.bDraw;
            rect = Other.rect;
            DrawOffset = Other.DrawOffset;
            SelectBegin = Other.SelectBegin;
            SelectEnd = Other.SelectEnd;
            rcSelect = Other.rcSelect;
            hyperLink = Other.hyperLink;
            param1 = Other.param1;
            param2 = Other.param2;
            id = Other.id;  //heten

            strFontName = Other.strFontName;
            nFontSize = Other.nFontSize;
            imgSize = Other.imgSize;

            DROP_TRUI_IMAGE(image);
            DROP_TRUI_IMAGE(pHoverImage);
            DROP_TRUI_IMAGE(pPushedImage);
            DROP_TRUI_IMAGE(imageSequence);

            if (Other.image) {
                image = Other.image;
                GRAB_TRUI_IMAGE(image);
            }

            if (Other.pPushedImage) {
                pPushedImage = Other.pPushedImage;
                GRAB_TRUI_IMAGE(pPushedImage);
            }

            if (Other.pHoverImage) {
                pHoverImage = Other.pHoverImage;
                GRAB_TRUI_IMAGE(pHoverImage);
            }

            if (Other.imageSequence) {
                imageSequence = Other.imageSequence;
                GRAB_TRUI_IMAGE(imageSequence);
            }

            for (int i = 0; i < 4; ++i)
                colorChangeOwe[i] = Other.colorChangeOwe[i];
            return *this;
        }

        //克隆
        bool CloneFrom(const SItem* pItem) {
            type = pItem->type;
            text = pItem->text;
            bUnderLine = pItem->bUnderLine;
            color = pItem->color;
            colorDelta = pItem->colorDelta;
            bDraw = pItem->bDraw;
            rect = pItem->rect;
            DrawOffset = pItem->DrawOffset;
            SelectBegin = pItem->SelectBegin;
            SelectEnd = pItem->SelectEnd;
            rcSelect = pItem->rcSelect;
            hyperLink = pItem->hyperLink;
            param1 = pItem->param1;
            param2 = pItem->param2;
            id = pItem->id;  //heten

            strFontName = pItem->strFontName;
            nFontSize = pItem->nFontSize;
            imgSize = pItem->imgSize;

            DROP_TRUI_IMAGE(image);
            DROP_TRUI_IMAGE(pHoverImage);
            DROP_TRUI_IMAGE(pPushedImage);
            DROP_TRUI_IMAGE(imageSequence);

            if (pItem->image) {
                image = pItem->image;
                GRAB_TRUI_IMAGE(image);
            }

            if (pItem->pPushedImage) {
                pPushedImage = pItem->pPushedImage;
                GRAB_TRUI_IMAGE(pPushedImage);
            }

            if (pItem->pHoverImage) {
                pHoverImage = pItem->pHoverImage;
                GRAB_TRUI_IMAGE(pHoverImage);
            }

            if (pItem->imageSequence) {
                imageSequence = pItem->imageSequence;
                GRAB_TRUI_IMAGE(imageSequence);
            }

            for (int i = 0; i < 4; ++i)
                colorChangeOwe[i] = pItem->colorChangeOwe[i];
            return true;
        }

        int getColCount();

        void Reset() {
            type = eit_Text;
            text = "";
            color = RtwPixel(0xffffffff);
            bUnderLine = false;
            colorDelta = DWORD(0);
            param1 = NULL;
            param2 = NULL;
            id = 0;
            strFontName = "";
            nFontSize = 12;

            hyperLink.Reset();
            SetImage(NULL);
            SetImageSequence(NULL);
        }
    };

    struct SLine  // SLine中有可能会没有任何SItem
    {
        int              height;  // 这一行的高度
        std::list<SItem> lineitems;
        bool             bWordWrapTail;  // 是否是自动换行所增加出来的
        int              absHeight;      // 屏幕的绝对高度
        RtwRect          rect;
        int              m_type;       //类型0为普通 ，1为HintText框
        int              slleft;       //行左边增加的距离
        int              slright;      //行右边增加的距离
        int              slbottom;     //行底部增加的距离
        int              sltop;        //行顶部增加的距离
        DWORD            dwBuildTime;  //生成时间
        SUiTextAlignment SlAlign;

        SLine()
            : height(0), absHeight(0), bWordWrapTail(false), m_type(0), slleft(0), slright(0),
              slbottom(0), sltop(0), dwBuildTime(0) {
            SlAlign.align = alignNear;
        }

        ~SLine() {}

        SLine(const SLine& Other) {
            height = Other.height;
            bWordWrapTail = Other.bWordWrapTail;
            absHeight = Other.absHeight;
            rect = Other.rect;
            m_type = Other.m_type;
            slleft = Other.slleft;
            slright = Other.slright;
            slbottom = Other.slbottom;
            sltop = Other.sltop;
            lineitems.clear();
            lineitems = Other.lineitems;
            SlAlign.align = Other.SlAlign.align;
            dwBuildTime = Other.dwBuildTime;
        }

        //赋值
        const SLine& operator=(const SLine& Other) {
            height = Other.height;
            bWordWrapTail = Other.bWordWrapTail;
            absHeight = Other.absHeight;
            rect = Other.rect;
            m_type = Other.m_type;
            slleft = Other.slleft;
            slright = Other.slright;
            slbottom = Other.slbottom;
            sltop = Other.sltop;
            SlAlign.align = Other.SlAlign.align;
            lineitems.clear();
            lineitems = Other.lineitems;
            dwBuildTime = Other.dwBuildTime;
            return *this;
        }

        bool CloneFrom(const SLine* pLine) {
            height = pLine->height;
            bWordWrapTail = pLine->bWordWrapTail;
            absHeight = pLine->absHeight;
            rect = pLine->rect;
            m_type = pLine->m_type;
            slleft = pLine->slleft;
            slright = pLine->slright;
            slbottom = pLine->slbottom;
            sltop = pLine->sltop;
            SlAlign.align = pLine->SlAlign.align;
            lineitems.clear();
            lineitems = pLine->lineitems;
            dwBuildTime = pLine->dwBuildTime;
            return true;
        }

        //ldr123 添加高度和宽度函数,用以实现图片的自由大小
        int getLeft() { return rect.left; }

        int getTop() { return rect.top; }

        int getRight() { return rect.right; }

        int getBottom() { return rect.bottom; }

        int getHeight() { return rect.getHeight(); }

        int getWidth() { return rect.getWidth(); }

        //end

        SItem* getItem(int index);
        int    getItemIndex(const SItem* pItem);
        int    getColCount();
        bool   InsertItem(SItem& item, int index = -1);
        bool   RemoveItem(int index);
        bool   RemoveItem(SItem* pItem);

        void Reset() {
            height = 0;
            rect = RtwRect(0, 0, 0, 0);
            bWordWrapTail = false;
            lineitems.clear();
        }
    };

   protected:
    RtwTextBox();
    virtual ~RtwTextBox();
    virtual void OnBeforeDestroy();

   public:
    virtual void Reset();
    virtual bool Create(DWORD Id);
    virtual void DrawForeground(const RtwRect* pClipRect = NULL);
    virtual void DrawBackground(const RtwRect* pClipRect = NULL);
    virtual void Run(DWORD dwDelta);

    virtual bool CloneFrom(const RtwWidget* pWidget);
    virtual void OnRemoveChild(RtwWidget* pWidget);
    virtual void OnMove(const RtwRect& OrigFrameRect);
    virtual void OnOffset(const SSize& size);

    //当前位置添加文本
    void AddHyperLinkText(const std::string& text, const std::string& link,
                          RtwPixel color = 0xffffffff);
    void AddHyperLinkText(const std::string& text, const SHyperLink& hyperLink,
                          RtwPixel color = 0xffffffff);

    void AddText(const std::string& text, RtwPixel color = 0xffffffff);
    void AddColorText(string& vrAddText, RtwPixel color = 0xffffffff);
    //end

    virtual void SetText(const std::string& text);
    void         SetTextnew(const std::string& text);
    void         SetRichText(const std::string& richtext);
    void         SetHtmlText(const std::string& htmltext);

    //多行
    void SetMultiLine(bool bMultiLine) {
        m_bMultiLine = bMultiLine;
        if (!m_bMultiLine)
            m_bWordWrap = false;
    }

    //可输入
    void SetEnableInput(bool bEnableInput) { m_bEnableInput = bEnableInput; }

    //短字(当一行显示不了一个单词时,这个单词移动到下一行)
    void SetWordWrap(bool bWordWrap, bool bRefresh = true) {
        m_bWordWrap = bWordWrap;
        if (bRefresh)
            Refresh();
    }

    //自动纵向滚动条
    void SetAutoScrollV(bool bAutoScrollV, bool bRefresh = true) {
        m_bAutoScrollV = bAutoScrollV;
        if (m_bAutoScrollV)
            m_pScrollBarV->Show();
        else
            m_pScrollBarV->Hide();
        if (bRefresh)
            Refresh();
    }

    //纵向滚动条
    void SetScrollBarV(bool ScrollBarV) { m_bScrollBarV = ScrollBarV; }

    //设置密码
    void SetPassword(bool bPassword) { m_bPassword = bPassword; }

    //设置是否只输入数字
    void SetNumeric(bool bNumeric) { m_bNumeric = bNumeric; }

    //设置能力(显示个数)
    void SetCapacity(int capacity) { m_Capacity = capacity; }

    //以指定字符代替显示的文字
    void SetPasswordChar(char ShowChar) {
        if (ShowChar)
            m_Password_ShowChar = ShowChar;
    }

    void SetAutoSizeV(bool bAutoSizeV) { m_bAutoSizeV = bAutoSizeV; }

    void SetAutoSizeH(bool bAutoSizeH) { m_bAutoSizeH = bAutoSizeH; }

    void SelectAll();     // 选取所有
    void RemoveSelect();  // 删除选取内容

    void ScrollToTop();
    void ScrollToBottom();

    virtual std::string GetText() const;
    virtual std::string getText() const;
    SLine*              getLine(int index);

    int getLineCount() { return (int)m_Lines.size(); }

    bool getEnableInput() { return m_bEnableInput; }

    bool getMultiLine() { return m_bMultiLine; }

    bool getWordWrap() { return m_bWordWrap; }

    int getCursorLine() { return m_CursorPosLine; }

    int getCursorCol() { return m_CursorPosCol; }

    RtwRect getRect() { return m_ViewRect; }

    RtwVScrollBar* getScrollBarV() { return m_pScrollBarV; }

    std::string getSelectText();

    bool getAutoSizeV() { return m_bAutoSizeV; }

    bool getAutoSizeH() { return m_bAutoSizeH; }

    void AutoSize(bool bV, bool bH);

    void SetCursorColor(RtwPixel color) { m_CursorColor = color; }

    void LoadFromFile(const char* szFileName);

   public:
    bool AddItem(SLine& line, int indexBefore = -1, bool bRefreshItemPos = true);
    bool RemoveAllItems(bool bRefreshItemPos = true);

    bool InsertLine(SLine& line, int indexBefore = -1);
    bool RemoveLine(int line);
    bool RemoveAllLines();

    bool getlineitemssize();
    bool InsertCharacterToCurrentCursor(const char character);
    bool InsertStringToCurrentCursor(const std::string& str);
    bool InsertItemToCurrentCursor(SItem& item);

    int    getLineIndex(SLine* pLine);
    int    getItemIndex(SLine* pLine, SItem* pItem);
    SItem* getNextItem(SLine* pLine, SItem* pItem);

    bool   PickItem(SPoint& point, int& outLine, int& outCol, int& outItemIndex, bool& bDoubleByte,
                    bool& outNear);  // 从一个点来选取行、列、Item位置、是否双字节字符、靠近左右
    SItem* PickItem(const SPoint& point);



    void CalcLinePosition(int LineIndex, const SPoint& StartPoint);
    void CalcLinePosition(int LineIndex);
    void RefreshPosition();
    void OffsetItemPosition(SSize Delta, int LineFrom = 0);
    void RefreshCursorPosition();
    void RefreshSelectionRect();
    void RefreshScrollBarVData();

    void AutoAdjectCursor();               // 自动调整光标，使光标可见
    void SetCursorPos(int line, int col);  // 设置光标位置

    void SetLeftMargin(int margin) { m_LeftMargin = margin; }

    void SetRightMargin(int margin) { m_RightMargin = margin; }

    void SetBottomMargin(int margin) { m_BottomMargin = margin; }

    void SetTopMargin(int margin) { m_TopMargin = margin; }

    int GetLeftMargin() { return m_LeftMargin; }

    int GetRightMargin() { return m_RightMargin; }

    int GetBottomMargin() { return m_BottomMargin; }

    int GetTopMargin() { return m_TopMargin; }

    void SetNumberOnly(bool bNum);

    void SetAlphaNumberOnly(bool bAlphaNum) {}

    void SetCapsLock(int lock) {}

    void InsertImage(RtwImage* image);

   public:
    static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfFocus | wfInput | wfMouseMove;
    static const int m_LeftSpace = 2;
    static const int m_LineSpace = 2;

   protected:
    virtual void CalcClient();  // 根据m_rcFrameRect计算m_rcClientRect
   private:
    void _ResetSelf();
    bool _DivideTextItem(SItem& ItemToDivide, int DivideIndexFrom, SItem& outNewItem);
    bool _MergeNeighborItem(SItem* pItem1, SItem* pItem2);  // 合并两个Item(不更新合并后的Item区域)
    bool _MergeWrapedLine(int line);  // 把line行后的自动换行都合并到line中(不更新合并后的Item区域)

    bool _GetItemByLineCol(int line, int col, SItem** ppItem, int* pTextItemIndex,
                           SItem** ppItemBetweenLeft, SItem** ppItemBetweenRight);

    bool   _PrepareEnterOk();  //计算是否需要换行
    SPoint _CalcPoints(int line, int col, int lineStart = 0, int colStart = 0);  //
    void   _CalcPoints();                         // 计算光标、选中文字等的屏幕位置
    void   _CalcItemSelectionRect(SItem* pItem);  // 根据item的rect计算选中区域的rect
    bool   _MergeItem(SItem* pItem1, SItem* pItem2);

    // 回车换行和自动换行的处理
    bool _Intercept(int line, int col,
                    bool bWordWrap);  // 把line行col列后分割出来（，插入到下一行）
    bool _Intercept(int line, int itemIndex, int itemCol,
                    bool bWordWrap);  // 把line行col列后分割出来（，插入到下一行）
    bool _Intercept(int line, int col);  // 把line行col列后分割出来
    bool _Intercept_Enter(int line, int col);

    bool _MoveTailItemToNextLine(int LineIndex, int ItemIndexFrom, int top = 0, int bottom = 0);
    void _DoSwapWordWrap();

    void _SetSelectionStart(int line, int col);
    void _SetSelectionEnd(int line, int col);
    void _getSelectionStartEnd(int& lineStart, int& colStart, int& lineEnd, int& colEnd);

   public:
    //设置最大行数
    //begin
    inline void SetTextLineNum(int nLineNum) {
        if (nLineNum > 0)
            m_nLineNum = nLineNum;
    }

    inline void SetNameColorChange(bool bChange = true) { m_bNameColorChange = bChange; }

    //end

    inline void SetNeedUpdate(bool bNeed) { m_bNeedRefreshPosition = bNeed; }

   private:
    void OnScroll_ScrollBarV(RtwWidget* pWidget, RtwEventDelegate* pEvent);

    void OnMouseMove_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnKeyDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnKeyChar_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnLClick_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnEvHyperLink(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnEvUnFocus(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnEvLDClick(RtwWidget* pWidget, RtwEventDelegate* pEvent);
   protected:
    RtwVScrollBar* m_pScrollBarV;  // 垂直滚动条

    bool m_bEnableInput;       // 是否允许输入
    bool m_bWordWrap;          // 是否自动换行
    bool m_bMultiLine;         // 是否可以多行
    bool m_bScrollBarV;        // 是否自动垂直滚动
    bool m_bPassword;          // 是否为密码显示方式
    bool m_bNumeric;           // 是否只能输入数字
    int  m_Capacity;           // 限制输入的字符数
    char m_Password_ShowChar;  // 密码方式显示时的字符

    std::list<SLine> m_Lines;  // 内容

    RtwPixel m_CursorColor;  // 光标的颜色

    int m_ViewOffsetLine;   // 显示的偏移量(行)
    int m_ViewOffsetCol;    // 显示的偏移量(列)
    int m_ViewLastLine;     // 显示的最后一行
    int m_CursorPosLine;    // 光标的位置(行)
    int m_CursorPosCol;     // 光标的位置(列)
    int m_CursorPosColBak;  // 光标的位置(列)(用于储存光标真实的列位置)

    int     m_SelStartLine;  // 选中文字的开始(行)
    int     m_SelStartCol;   // 选中文字的开始(列)
    int     m_SelEndLine;    // 选中文字的结束(行)
    int     m_SelEndCol;     // 选中文字的结束(列)
    RtwRect m_ViewRect;      // 显示的区域
    RtwRect m_CursorRect;    // 光标的区域
    bool    m_addface;       //是否加入表情
    bool    m_bAutoSizeV;    // 是否根据子Widget来适应大小(V)
    bool    m_bAutoSizeH;    // 是否根据子Widget来适应大小(H)

    bool m_bNeedRefreshPosition;  // 是否需要重新计算Item位置

    CUiRichText* m_pRichTextLoader;  // RichText的Loader
    CUiHtmlText* m_pHtmlTextLoader;  // HtmlText的Loader
    int          m_LeftMargin;
    int          m_RightMargin;
    int          m_BottomMargin;
    int          m_TopMargin;
    char         m_acDoubleByte[2];

    std::string m_SelText;           // 选中文字
    bool        m_bNameColorChange;  //名字变色

    //ldr123
    //设置文本行数
    int m_nLineNum;

   public:
    RtwEventDispatcher EvUpdateText;
    RtwEventDispatcher EvInputChar;
    RtwEventDispatcher EvHyperLink;
};
}  // namespace ui
#endif
