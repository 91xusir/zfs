#ifndef _RTUI_EDITBOX_H
#define _RTUI_EDITBOX_H

//********************************************************************
//	created:	2010.04.08 10:00
//	filename: 	rtw_text.cpp
//	author:		ldr123
//	purpose:	�ı������
//  other:		�ֲ��ع����ע��
//********************************************************************

namespace ui {
//�ı���
class RtwTextBox : public RtwWidget {
    RT_DECLARE_DYNCREATE(RtwTextBox, RtwWidget, 0, "ui")

    friend class CUiWidgetFactory;
    friend class CUiWidgetContainer;
    friend class CUiLayer;
    friend class CUiDesktop;

   public:
    enum EEditItemType {
        eit_Text,           //�ı�
        eit_Image,          //ͼ��
        eit_ImageSequence,  //ͼƬ����
        eit_Name            //����
    };

    struct SItem {
       public:
        EEditItemType type;        //����
        std::string   text;        //�ı�����
        RtwPixel      color;       //�ı���ɫ
        bool          bUnderLine;  // �Ƿ��»���
        RtwPixel      colorDelta;  // ��ɫ��ÿ��������
        SHyperLink    hyperLink;   // ����������
        void*         param1;      // ����1(ǳ����)
        void*         param2;      // ����2(ǳ����)
        long          id;          //heten  �������ID
        int           showcount;   // ѭ������  heten

        //ldr123 ֧�������Զ���
        std::string strFontName;
        int         nFontSize;

        bool    bDraw;
        RtwRect rect;
        int     DrawOffset;         // �ӵڼ����ַ���ʼ��ʾ
        int     SelectBegin;        // ѡ�еĿ�ʼ�ַ�
        int     SelectEnd;          // ѡ�еĽ����ַ�
        RtwRect rcSelect;           // ѡ���ַ�������
        Real    colorChangeOwe[4];  // ��ɫ�������Ƿ
        SSize   imgSize;            //Ҫ��ʾ�Ĵ�С

        //ldr123 ��Ӹ߶ȺͿ�Ⱥ���,����ʵ��ͼƬ�����ɴ�С
        int getLeft() { return rect.left; }

        int getTop() { return rect.top; }

        int getRight() { return rect.right; }

        int getBottom() { return rect.bottom; }

        int getHeight() { return rect.getHeight(); }

        int getWidth() { return rect.getWidth(); }

        //end

        //����ͼƬ(ǰ��ͼ?)
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

        //ͼƬ�л�
        virtual void Run(DWORD dwDelta) { IMAGETICK(image, dwDelta); }

        RtwImage* getImage() { return image; }

        //����ͼƬ����
        void SetImageSequence(RtwComplexImage* pImageSequence) {
            //ɾ���Ѿ����ڵ�����
            DROP_TRUI_IMAGE(imageSequence);
            imageSequence = pImageSequence;
            if (imageSequence) {
                GRAB_TRUI_IMAGE(imageSequence);
            }
        }

        //��ȡͼƬ����
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
            type = eit_Text;  //Ĭ������,�ı�
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

        //��ֵ������
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

        //��¡
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

    struct SLine  // SLine���п��ܻ�û���κ�SItem
    {
        int              height;  // ��һ�еĸ߶�
        std::list<SItem> lineitems;
        bool             bWordWrapTail;  // �Ƿ����Զ����������ӳ�����
        int              absHeight;      // ��Ļ�ľ��Ը߶�
        RtwRect          rect;
        int              m_type;       //����0Ϊ��ͨ ��1ΪHintText��
        int              slleft;       //��������ӵľ���
        int              slright;      //���ұ����ӵľ���
        int              slbottom;     //�еײ����ӵľ���
        int              sltop;        //�ж������ӵľ���
        DWORD            dwBuildTime;  //����ʱ��
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

        //��ֵ
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

        //ldr123 ��Ӹ߶ȺͿ�Ⱥ���,����ʵ��ͼƬ�����ɴ�С
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

    //��ǰλ������ı�
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

    //����
    void SetMultiLine(bool bMultiLine) {
        m_bMultiLine = bMultiLine;
        if (!m_bMultiLine)
            m_bWordWrap = false;
    }

    //������
    void SetEnableInput(bool bEnableInput) { m_bEnableInput = bEnableInput; }

    //����(��һ����ʾ����һ������ʱ,��������ƶ�����һ��)
    void SetWordWrap(bool bWordWrap, bool bRefresh = true) {
        m_bWordWrap = bWordWrap;
        if (bRefresh)
            Refresh();
    }

    //�Զ����������
    void SetAutoScrollV(bool bAutoScrollV, bool bRefresh = true) {
        m_bAutoScrollV = bAutoScrollV;
        if (m_bAutoScrollV)
            m_pScrollBarV->Show();
        else
            m_pScrollBarV->Hide();
        if (bRefresh)
            Refresh();
    }

    //���������
    void SetScrollBarV(bool ScrollBarV) { m_bScrollBarV = ScrollBarV; }

    //��������
    void SetPassword(bool bPassword) { m_bPassword = bPassword; }

    //�����Ƿ�ֻ��������
    void SetNumeric(bool bNumeric) { m_bNumeric = bNumeric; }

    //��������(��ʾ����)
    void SetCapacity(int capacity) { m_Capacity = capacity; }

    //��ָ���ַ�������ʾ������
    void SetPasswordChar(char ShowChar) {
        if (ShowChar)
            m_Password_ShowChar = ShowChar;
    }

    void SetAutoSizeV(bool bAutoSizeV) { m_bAutoSizeV = bAutoSizeV; }

    void SetAutoSizeH(bool bAutoSizeH) { m_bAutoSizeH = bAutoSizeH; }

    void SelectAll();     // ѡȡ����
    void RemoveSelect();  // ɾ��ѡȡ����

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
                    bool& outNear);  // ��һ������ѡȡ�С��С�Itemλ�á��Ƿ�˫�ֽ��ַ�����������
    SItem* PickItem(const SPoint& point);



    void CalcLinePosition(int LineIndex, const SPoint& StartPoint);
    void CalcLinePosition(int LineIndex);
    void RefreshPosition();
    void OffsetItemPosition(SSize Delta, int LineFrom = 0);
    void RefreshCursorPosition();
    void RefreshSelectionRect();
    void RefreshScrollBarVData();

    void AutoAdjectCursor();               // �Զ�������꣬ʹ���ɼ�
    void SetCursorPos(int line, int col);  // ���ù��λ��

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
    virtual void CalcClient();  // ����m_rcFrameRect����m_rcClientRect
   private:
    void _ResetSelf();
    bool _DivideTextItem(SItem& ItemToDivide, int DivideIndexFrom, SItem& outNewItem);
    bool _MergeNeighborItem(SItem* pItem1, SItem* pItem2);  // �ϲ�����Item(�����ºϲ����Item����)
    bool _MergeWrapedLine(int line);  // ��line�к���Զ����ж��ϲ���line��(�����ºϲ����Item����)

    bool _GetItemByLineCol(int line, int col, SItem** ppItem, int* pTextItemIndex,
                           SItem** ppItemBetweenLeft, SItem** ppItemBetweenRight);

    bool   _PrepareEnterOk();  //�����Ƿ���Ҫ����
    SPoint _CalcPoints(int line, int col, int lineStart = 0, int colStart = 0);  //
    void   _CalcPoints();                         // �����ꡢѡ�����ֵȵ���Ļλ��
    void   _CalcItemSelectionRect(SItem* pItem);  // ����item��rect����ѡ�������rect
    bool   _MergeItem(SItem* pItem1, SItem* pItem2);

    // �س����к��Զ����еĴ���
    bool _Intercept(int line, int col,
                    bool bWordWrap);  // ��line��col�к�ָ�����������뵽��һ�У�
    bool _Intercept(int line, int itemIndex, int itemCol,
                    bool bWordWrap);  // ��line��col�к�ָ�����������뵽��һ�У�
    bool _Intercept(int line, int col);  // ��line��col�к�ָ����
    bool _Intercept_Enter(int line, int col);

    bool _MoveTailItemToNextLine(int LineIndex, int ItemIndexFrom, int top = 0, int bottom = 0);
    void _DoSwapWordWrap();

    void _SetSelectionStart(int line, int col);
    void _SetSelectionEnd(int line, int col);
    void _getSelectionStartEnd(int& lineStart, int& colStart, int& lineEnd, int& colEnd);

   public:
    //�����������
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
    RtwVScrollBar* m_pScrollBarV;  // ��ֱ������

    bool m_bEnableInput;       // �Ƿ���������
    bool m_bWordWrap;          // �Ƿ��Զ�����
    bool m_bMultiLine;         // �Ƿ���Զ���
    bool m_bScrollBarV;        // �Ƿ��Զ���ֱ����
    bool m_bPassword;          // �Ƿ�Ϊ������ʾ��ʽ
    bool m_bNumeric;           // �Ƿ�ֻ����������
    int  m_Capacity;           // ����������ַ���
    char m_Password_ShowChar;  // ���뷽ʽ��ʾʱ���ַ�

    std::list<SLine> m_Lines;  // ����

    RtwPixel m_CursorColor;  // ������ɫ

    int m_ViewOffsetLine;   // ��ʾ��ƫ����(��)
    int m_ViewOffsetCol;    // ��ʾ��ƫ����(��)
    int m_ViewLastLine;     // ��ʾ�����һ��
    int m_CursorPosLine;    // ����λ��(��)
    int m_CursorPosCol;     // ����λ��(��)
    int m_CursorPosColBak;  // ����λ��(��)(���ڴ�������ʵ����λ��)

    int     m_SelStartLine;  // ѡ�����ֵĿ�ʼ(��)
    int     m_SelStartCol;   // ѡ�����ֵĿ�ʼ(��)
    int     m_SelEndLine;    // ѡ�����ֵĽ���(��)
    int     m_SelEndCol;     // ѡ�����ֵĽ���(��)
    RtwRect m_ViewRect;      // ��ʾ������
    RtwRect m_CursorRect;    // ��������
    bool    m_addface;       //�Ƿ�������
    bool    m_bAutoSizeV;    // �Ƿ������Widget����Ӧ��С(V)
    bool    m_bAutoSizeH;    // �Ƿ������Widget����Ӧ��С(H)

    bool m_bNeedRefreshPosition;  // �Ƿ���Ҫ���¼���Itemλ��

    CUiRichText* m_pRichTextLoader;  // RichText��Loader
    CUiHtmlText* m_pHtmlTextLoader;  // HtmlText��Loader
    int          m_LeftMargin;
    int          m_RightMargin;
    int          m_BottomMargin;
    int          m_TopMargin;
    char         m_acDoubleByte[2];

    std::string m_SelText;           // ѡ������
    bool        m_bNameColorChange;  //���ֱ�ɫ

    //ldr123
    //�����ı�����
    int m_nLineNum;

   public:
    RtwEventDispatcher EvUpdateText;
    RtwEventDispatcher EvInputChar;
    RtwEventDispatcher EvHyperLink;
};
}  // namespace ui
#endif
