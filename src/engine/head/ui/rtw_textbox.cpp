#include "ui/rtw_ui.h"
#include "../../../game_client/preConsole.h"
//********************************************************************
//	created:	2010.04.08 10:00
//	filename: 	rtw_text.cpp
//	author:		ldr123
//	purpose:	文本输入框
//  other:		局部重构添加注释
//********************************************************************

RT_IMPLEMENT_DYNAMIC(RtwTextBox, RtwWidget, 0, "ui")

//有几个文字
int RtwTextBox::SItem::getColCount() {
    int count = 0;
    switch (type) {
        case eit_Text:
            count += (int)text.size();
            break;
        case eit_Image:
            count++;
            break;
        case eit_ImageSequence:
            count++;
            break;
        default:
            break;
    }
    return count;
}

//获取一项
RtwTextBox::SItem* RtwTextBox::SLine::getItem(int index) {
    if (index >= (int)lineitems.size())
        return NULL;

    std::list<RtwTextBox::SItem>::iterator iterItem = lineitems.begin();
    for (int i = 0; i < index; ++i)
        iterItem++;

    return &(*iterItem);
}

//获取item在行内的索引
int RtwTextBox::SLine::getItemIndex(const SItem* pItem) {
    int index = 0;
    foreach (std::list<RtwTextBox::SItem>, iterItem, lineitems) {
        RtwTextBox::SItem* _pItem = &(*iterItem);
        if (_pItem == pItem)
            return index;

        index++;
    }
    return -1;
}

//获取一行内的列数
int RtwTextBox::SLine::getColCount() {
    int count = 0;
    foreach (std::list<RtwTextBox::SItem>, iterItem, lineitems) {
        RtwTextBox::SItem& item = *iterItem;
        count += item.getColCount();
    }
    return count;
}

//插入一列
bool RtwTextBox::SLine::InsertItem(RtwTextBox::SItem& item, int index /* = -1*/) {
    if (index > (int)lineitems.size())
        return false;

    int iHeight = item.rect.getHeight();
    int iWidth  = 0;

    if (item.imgSize.height == 0) {
        iHeight = item.nFontSize;
        iWidth  = (int)item.text.size() * item.nFontSize;
    } else {
        iHeight = item.imgSize.height;
        iWidth  = item.imgSize.width;
    }

    //调整行的高度
    if (height < iHeight) {
        height = iHeight /*.rect.getHeight()+1*/;
    }

    rect.right = rect.left + rect.getWidth() + iWidth;

    if (index < 0) {
        lineitems.push_back(item);
        return true;
    } else {
        std::list<RtwTextBox::SItem>::iterator iterItem = lineitems.begin();
        for (int i = 0; i < index; ++i) {
            iterItem++;
        }
        lineitems.insert(iterItem, item);
        return true;
    }
}

//删除指定的item
//beign
bool RtwTextBox::SLine::RemoveItem(int index) {
    if ((int)lineitems.size() <= index)
        return false;

    std::list<RtwTextBox::SItem>::iterator iterItem = lineitems.begin();
    for (int i = 0; i < index; ++i) {
        iterItem++;
    }

    SItem& item = *iterItem;
    lineitems.erase(iterItem);

    return true;
}

bool RtwTextBox::SLine::RemoveItem(RtwTextBox::SItem* pItem) {
    foreach (std::list<RtwTextBox::SItem>, iterItem, lineitems) {
        RtwTextBox::SItem* _pItem = &(*iterItem);
        if (_pItem == pItem) {
            lineitems.erase(iterItem);
            return true;
        }
    }
    return true;
}

//end

RtwTextBox::RtwTextBox()
    : m_pScrollBarV(NULL), m_LeftMargin(0), m_RightMargin(0), m_BottomMargin(0), m_TopMargin(0),
      m_addface(false), m_bEnableInput(true), m_bWordWrap(false), m_bMultiLine(false),
      m_bScrollBarV(false), m_bPassword(false), m_bNumeric(false), m_Capacity(0),
      m_ViewOffsetLine(0), m_ViewOffsetCol(0), m_ViewLastLine(0), m_CursorPosLine(0),
      m_CursorPosCol(0), m_CursorColor(RtwPixel(0xffccaaff)), m_CursorPosColBak(0),
      m_SelStartLine(0), m_SelStartCol(0), m_SelEndLine(0), m_SelEndCol(0),
      m_Password_ShowChar('*'), m_nLineNum(200), m_bNameColorChange(true) {
    m_bAutoScrollV = false;
    m_WidgetType   = wtEditBox;
    m_FontSize     = 12;
    m_FontType     = "";

    m_pRichTextLoader = RT_NEW CUiRichText();
    m_pHtmlTextLoader = RT_NEW CUiHtmlText();
    memset(m_acDoubleByte, 0, sizeof(char) * 2);
}

RtwTextBox::~RtwTextBox() {
    DEL_ONE(m_pRichTextLoader);
    DEL_ONE(m_pHtmlTextLoader);
}

void RtwTextBox::OnBeforeDestroy() {
    _ResetSelf();

    DROP_RTUI_OBJECT(m_pScrollBarV);

    RtwWidget::OnBeforeDestroy();
}

void RtwTextBox::Reset() {
    RtwWidget::Reset();

    _ResetSelf();
}

bool RtwTextBox::Create(DWORD Id) {
    if (!RtwWidget::Create(Id))
        return false;

    _ResetSelf();

    // 创建自动滚动条
    RtwVScrollBar* pSbv = NULL;
    UI_ENSURE_B(g_workspace.FindWidget(g_SystemWidgetName[sw_AutoSbv], (RtwWidget**)&pSbv));

    m_pScrollBarV =
        (RtwVScrollBar*)g_workspace.getWidgetFactory()->createWidget(wtScrollBarV, "$Sbv");

    UI_ENSURE_B(m_pScrollBarV);
    UI_ENSURE_B(m_pScrollBarV->CloneFrom(pSbv));

    DROP_RTUI_OBJECT(pSbv);
    m_pScrollBarV->SetData(0, 0, 0);
    AddChild(m_pScrollBarV);

    m_pScrollBarV->EvScroll += RTW_CALLBACK(this, RtwTextBox, OnScroll_ScrollBarV);
    EvMouseMove += RTW_CALLBACK(this, RtwTextBox, OnMouseMove_This);
    EvMouseLDown += RTW_CALLBACK(this, RtwTextBox, OnMouseLDown_This);
    EvKey += RTW_CALLBACK(this, RtwTextBox, OnKeyDown_This);
    EvKeyChar += RTW_CALLBACK(this, RtwTextBox, OnKeyChar_This);
    EvLClick += RTW_CALLBACK(this, RtwTextBox, OnLClick_This);
    EvHyperLink += RTW_CALLBACK(this, RtwTextBox, OnEvHyperLink);
    EvUnFocus += RTW_CALLBACK(this, RtwTextBox, OnEvUnFocus);
    EvLDClick += RTW_CALLBACK(this, RtwTextBox, OnEvLDClick);
    return true;
}

void RtwTextBox::Run(DWORD dwDelta) {
    if (!getVisible())
        return;

    //逐行计算
    foreach (std::list<SLine>, iterLine, m_Lines) {
        SLine& line = *iterLine;
        if (!m_ViewRect.IsContain(line.rect))
            continue;

        //行内挨个计算
        foreach (std::list<SItem>, iterItem, line.lineitems) {
            SItem& item = *iterItem;
            if (!m_ViewRect.IsContain(item.rect))
                continue;

            //有图片就换图
            item.Run(dwDelta);

            if (item.type == eit_Text && item.colorDelta != 0) {
                item.colorChangeOwe[0] += item.colorDelta.r * (Real)dwDelta / 1000;
                item.colorChangeOwe[1] += item.colorDelta.g * (Real)dwDelta / 1000;
                item.colorChangeOwe[2] += item.colorDelta.b * (Real)dwDelta / 1000;
                item.colorChangeOwe[3] += item.colorDelta.a * (Real)dwDelta / 1000;
                RtwPixel tmpColor(0, 0, 0, 0);
                if (item.colorChangeOwe[0] >= 1) {
                    tmpColor.r             = item.colorChangeOwe[0];
                    item.colorChangeOwe[0] = item.colorChangeOwe[0] - tmpColor.r;
                }
                if (item.colorChangeOwe[1] >= 1) {
                    tmpColor.g             = item.colorChangeOwe[1];
                    item.colorChangeOwe[1] = item.colorChangeOwe[1] - tmpColor.g;
                }
                if (item.colorChangeOwe[2] >= 1) {
                    tmpColor.b             = item.colorChangeOwe[2];
                    item.colorChangeOwe[2] = item.colorChangeOwe[2] - tmpColor.b;
                }
                if (item.colorChangeOwe[3] >= 1) {
                    tmpColor.a             = item.colorChangeOwe[3];
                    item.colorChangeOwe[3] = item.colorChangeOwe[3] - tmpColor.a;
                }
                item.color += tmpColor;
            }
        }
    }

    RtwWidget::Run(dwDelta);
}

//移动
void RtwTextBox::OnMove(const RtwRect& OrigFrameRect) {
    RtwWidget::OnMove(OrigFrameRect);

    if (m_bNeedRefreshPosition) {
        //刷新位置
        RefreshPosition();
        m_bNeedRefreshPosition = false;
    } else {
        //每行每列都得移动
        SSize osize(m_rcFrame.left - OrigFrameRect.left, m_rcFrame.top - OrigFrameRect.top);

        foreach (std::list<SLine>, iterLine, m_Lines) {
            SLine& line = *iterLine;
            line.rect.Offset(osize);
            foreach (std::list<SItem>, iterItem, line.lineitems) {
                SItem& item = *iterItem;
                item.rect.Offset(osize);
            }
        }
    }

    RefreshCursorPosition();
    RefreshScrollBarVData();
}

void RtwTextBox::OnOffset(const SSize& size) {
    RtwWidget::OnOffset(size);

    m_ViewRect.Offset(size);
    m_CursorRect.Offset(size);

    foreach (std::list<SLine>, iterLine, m_Lines) {
        SLine& line = *iterLine;
        line.rect.Offset(size);

        foreach (std::list<SItem>, iterItem, line.lineitems) {
            SItem& item = *iterItem;
            item.rect.Offset(size);
        }
    }

    RefreshCursorPosition();
    RefreshScrollBarVData();
}

bool RtwTextBox::CloneFrom(const RtwWidget* pWidget) {
    UI_ENSURE_B(pWidget->IsKindOf(RtwTextBox::GetRuntimeClass()));

    UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

    RtwTextBox* pOther = (RtwTextBox*)pWidget;

    m_pScrollBarV->CloneFrom(pOther->m_pScrollBarV);
    m_bEnableInput = pOther->m_bEnableInput;
    m_bWordWrap    = pOther->m_bWordWrap;
    m_bMultiLine   = pOther->m_bMultiLine;
    m_bAutoScrollV = pOther->m_bAutoScrollV;

    RemoveAllItems(false);
    m_Lines = pOther->m_Lines;

    m_ViewOffsetLine  = pOther->m_ViewOffsetLine;
    m_ViewOffsetCol   = pOther->m_ViewOffsetCol;
    m_ViewLastLine    = pOther->m_ViewLastLine;
    m_CursorPosLine   = pOther->m_CursorPosLine;
    m_CursorPosCol    = pOther->m_CursorPosCol;
    m_CursorPosColBak = pOther->m_CursorPosColBak;
    m_SelStartLine    = pOther->m_SelStartLine;
    m_SelStartCol     = pOther->m_SelStartCol;
    m_SelEndLine      = pOther->m_SelEndLine;
    m_SelEndCol       = pOther->m_SelEndCol;
    m_SelText         = pOther->m_SelText;
    m_nLineNum        = pOther->m_nLineNum;

    m_CursorRect = pOther->m_CursorRect;

    return true;
}

void RtwTextBox::SetCursorPos(int line, int col) {
    SLine* pLine = getLine(line);
    UI_ENSURE(pLine);
    m_CursorPosLine = line;
    if (col < 0)
        m_CursorPosCol = 0;
    else if (col > (int)pLine->getColCount())
        m_CursorPosCol = (int)pLine->getColCount();
    else
        m_CursorPosCol = col;

    RefreshCursorPosition();
}

//设置显示文本
void RtwTextBox::SetText(const std::string& text) {
    // 删除所有行，并重置相关标志
    RemoveAllItems(false);
    m_ViewOffsetLine  = 0;
    m_ViewOffsetCol   = 0;
    m_ViewLastLine    = 0;
    m_CursorPosLine   = 0;
    m_CursorPosCol    = 0;
    m_CursorPosColBak = 0;
    m_SelStartLine    = 0;
    m_SelStartCol     = 0;
    m_SelEndLine      = 0;
    m_SelEndCol       = 0;
    m_SelText         = "";

    // 添加新行
    SLine line;
    line.height = g_workspace.getFontManager()->getDefaultFont()->getFontSize();
    SItem item;
    item.type        = eit_Text;
    item.color       = m_TextColor;
    item.text        = text;
    item.strFontName = m_FontType;
    item.nFontSize   = m_FontSize;

    line.lineitems.push_back(item);
    InsertLine(line, -1);

    SetCursorPos(0, (int)text.size());
    if (m_Text != text) {
        m_Text = text;
    }
}

void RtwTextBox::AddText(const std::string& text, RtwPixel color) {
    SLine* pLine = getLine(m_CursorPosLine);
    if (!pLine) {
        RemoveAllItems(false);
        m_ViewOffsetLine  = 0;
        m_ViewOffsetCol   = 0;
        m_ViewLastLine    = 0;
        m_CursorPosLine   = 0;
        m_CursorPosCol    = 0;
        m_CursorPosColBak = 0;
        m_SelStartLine    = 0;
        m_SelStartCol     = 0;
        m_SelEndLine      = 0;
        m_SelEndCol       = 0;
        m_SelText         = "";
        SItem item;
        item.type  = eit_Text;
        item.color = color;
        item.text  = text;

        SLine line;
        line.lineitems.push_back(item);
        line.height = g_workspace.getFontManager()->getDefaultFont()->getFontSize();
        InsertLine(line, -1);
        SetCursorPos(0, (int)text.size());
        if (m_Text != text) {
            m_Text = text;
        }
        return;
    }

    SItem item;
    item.type  = eit_Text;
    item.color = color;
    item.text  = text;
    pLine->lineitems.push_back(item);
    m_CursorPosCol += (int)text.size();
    SetCursorPos(0, (int)text.size());
    CalcLinePosition(m_CursorPosLine);
    RefreshCursorPosition();
    AutoAdjectCursor();
    if (m_Text != text) {
        m_Text = text;
    }
}

void RtwTextBox::AddHyperLinkText(const std::string& text, const std::string& link,
                                  RtwPixel color) {
    SHyperLink lLink;
    lLink.Text = text;
    lLink.Type = hlinkClient;
    AddHyperLinkText(text, lLink, color);
}

void RtwTextBox::AddHyperLinkText(const std::string& text, const SHyperLink& hyperLink,
                                  RtwPixel color) {
    SLine* pLine = getLine(m_CursorPosLine);
    if (!pLine) {
        RemoveAllItems(false);
        m_ViewOffsetLine  = 0;
        m_ViewOffsetCol   = 0;
        m_ViewLastLine    = 0;
        m_CursorPosLine   = 0;
        m_CursorPosCol    = 0;
        m_CursorPosColBak = 0;
        m_SelStartLine    = 0;
        m_SelStartCol     = 0;
        m_SelEndLine      = 0;
        m_SelEndCol       = 0;
        m_SelText         = "";
        SItem item;
        item.type      = eit_Text;
        item.color     = color;
        item.text      = text;
        item.hyperLink = hyperLink;

        SLine line;
        line.lineitems.push_back(item);
        line.height = g_workspace.getFontManager()->getDefaultFont()->getFontSize();
        InsertLine(line, -1);
        SetCursorPos(0, (int)text.size());
        if (m_Text != text) {
            m_Text = text;
        }
        return;
    }

    SItem item;
    item.type      = eit_Text;
    item.color     = color;
    item.text      = text;
    item.hyperLink = hyperLink;
    pLine->lineitems.push_back(item);

    m_CursorPosCol += (int)text.size();
    SetCursorPos(0, m_CursorPosCol);
    CalcLinePosition(m_CursorPosLine);
    RefreshCursorPosition();
    AutoAdjectCursor();

    if (m_Text != text) {
        m_Text = text;
    }
}

void RtwTextBox::SetTextnew(const std::string& text) {
    RemoveAllItems(false);
    m_ViewOffsetLine  = 0;
    m_ViewOffsetCol   = 0;
    m_ViewLastLine    = 0;
    m_CursorPosLine   = 0;
    m_CursorPosCol    = 0;
    m_CursorPosColBak = 0;
    m_SelStartLine    = 0;
    m_SelStartCol     = 0;
    m_SelEndLine      = 0;
    m_SelEndCol       = 0;
    m_SelText         = "";

    static const RtwPixel itemColorList[] = {0xffffffff, 0xff30e512, 0xff00aaff,
                                             0xffffd851};  //道具颜色列表

    int         color      = 0xffffffff;
    std::string strMessage = text;

    int nWordsNum  = 0;  //字符数
    int nImageNum  = 0;  //图片数量
    int nLineWidth = 0;  //单行长度
    int nChart     = 0;  //单字符
    int nDouble    = 0;  //双字符
    int nHight     = 0;  //chatbox的高度

    int nTaskNameNum = 0;  //任务名字长度
    int nItemNameNum = 0;  //道具名字长度

    size_t nPos = 0;  //遍历

    bool bTwo           = false;  //是否是双字节
    bool bItemLinkBegin = false;  //item链接开始
    bool bItemLinkEnd   = false;  //item链接结束

    bool bTaskLinkBegin = false;  //任务链接开始
    bool bTaskLinkEnd   = false;  //任务链接结束

    bool   bClientMoveToBegin = false;
    bool   bClientMoveToEnd   = false;
    string strMoveToLink      = "";
    int    nMoveToNum         = 0;
    int    nMoveX             = 0;
    int    nMoveY             = 0;

    bool bLocalImageBegin = false;  //本地文件开始
    bool bLocalImageEnd   = false;  //本地文件结束

    char szLinkInfo[1024];  //超链接内容
    char szFackToken[128];

    RtwImage* pFaceImage[5][8];  //图片数组

    int nItemColor;  //链接的颜色

    SItem itemName;   //名字item
    SItem itemTemp;   //临时item
    SItem deTemp;     //description的item
    SItem itemImage;  //图片item
    SItem itemLink;   //超链接item

    SLine lineTemp;  //临时line

    std::string strItemLink   = "";  //道具超链接内容
    std::string strmTaskLink  = "";  //任务超链接内容
    std::string strTempMsg    = "";  //临时用
    std::string strLocalImage = "";  //本地文件

    itemName.type  = eit_Name;
    itemTemp.type  = eit_Text;
    itemImage.type = eit_Text;

    itemName.strFontName = m_FontType;
    itemName.nFontSize   = m_FontSize;
    itemTemp.strFontName = m_FontType;
    itemTemp.nFontSize   = m_FontSize;

    nLineWidth = m_rcFrame.getWidth() - 10;
    nHight     = m_rcFrame.getHeight() - 10;
    InsertLine(lineTemp);  //插入临时行

    while (nPos < strMessage.size()) {
        if (strMessage[nPos] == '<' && strMessage[nPos + 1] == 'm' && strMessage[nPos + 2] == 'o' &&
            strMessage[nPos + 3] == 'v' && strMessage[nPos + 4] == 'e' &&
            strMessage[nPos + 5] == 't' && strMessage[nPos + 6] == 'o' &&
            strMessage[nPos + 7] == '>' && !bClientMoveToBegin) {
            //道具链接
            bClientMoveToBegin = true;
            nPos += 8;
        }

        if (strMessage[nPos] == '<' && strMessage[nPos + 1] == 'l' && strMessage[nPos + 2] == 'o' &&
            strMessage[nPos + 3] == 'c' && strMessage[nPos + 4] == 'a' &&
            strMessage[nPos + 5] == 'l' && strMessage[nPos + 6] == 'f' &&
            strMessage[nPos + 7] == 'i' && strMessage[nPos + 8] == 'l' &&
            strMessage[nPos + 9] == 'e' && strMessage[nPos + 10] == ':' &&
            strMessage[nPos + 11] == '/' && strMessage[nPos + 12] == '/' && !bLocalImageBegin) {
            //道具链接
            bLocalImageBegin = true;
            nPos += 13;
        }

        if (strMessage[nPos] == 'i' && strMessage[nPos + 1] == 't' && strMessage[nPos + 2] == 'e' &&
            strMessage[nPos + 3] == 'm' && strMessage[nPos + 4] == '[' && !bItemLinkBegin) {
            //道具链接
            bItemLinkBegin = true;
            nPos += 5;
        } else if (strMessage[nPos] == 't' && strMessage[nPos + 1] == 'a' &&
                   strMessage[nPos + 2] == 's' && strMessage[nPos + 3] == 'k' &&
                   strMessage[nPos + 4] == '[' && bItemLinkBegin) {
            //任务链接
            bTaskLinkBegin = true;
            nPos += 5;
        }

        if (strMessage[nPos] == '^' && strMessage[nPos + 1] == '-' && strMessage[nPos + 2] == 'f' &&
            strMessage[nPos + 3] == 'a' && strMessage[nPos + 4] == 'c' &&
            strMessage[nPos + 5] == 'e') {
            //表情
            nPos += 6;
            int  x = 0;
            int  y = 0;
            char faceToken;

            //横纵坐标
            x = (strMessage[nPos] - '0') * 10 + (strMessage[nPos + 1] - '0');
            nPos += 2;

            faceToken = strMessage[nPos];
            nPos++;

            y = (strMessage[nPos] - '0') * 10 + (strMessage[nPos + 1] - '0');
            nPos += 2;

            if ((x == 4 && y == 7 && itemImage.type == eit_Text) || faceToken != '_') {
                //无效图片
                continue;
            }

            rt2_sprintf(szFackToken, "ui\\face\\%d.gif", x * 8 + y);
            pFaceImage[x][y] = g_workspace.getImageFactory()->createImage(szFackToken);

            if (!pFaceImage[x][y]) {
                continue;
            }

            itemImage.type = eit_Image;
            itemImage.SetImage(pFaceImage[x][y]);
            itemImage.imgSize = SSize(31, 31);

            nImageNum++;

            SLine* pLastLine = getLine((int)m_Lines.size() - 1);
            if (pLastLine->rect.getWidth() + itemImage.imgSize.width > nLineWidth) {
                if (pLastLine) {
                    SLine lineNew;
                    InsertLine(lineNew);  //插入空行

                    pLastLine = getLine((int)m_Lines.size() - 1);
                    pLastLine->InsertItem(itemImage);
                    CalcLinePosition((int)m_Lines.size() - 1);

                    strTempMsg = "";
                    nImageNum  = 0;
                    nWordsNum  = 0;
                }
            } else {
                pLastLine->InsertItem(itemImage);
                nWordsNum = 0;
            }
            CalcLinePosition((int)m_Lines.size() - 1);
        } else if (bLocalImageBegin) {
            if (strMessage[nPos] == 'l' && strMessage[nPos + 1] == 'o' &&
                strMessage[nPos + 2] == 'c' && strMessage[nPos + 3] == 'a' &&
                strMessage[nPos + 4] == 'l' && strMessage[nPos + 5] == 'f' &&
                strMessage[nPos + 6] == 'i' && strMessage[nPos + 7] == 'l' &&
                strMessage[nPos + 8] == 'e' && strMessage[nPos + 9] == '/' &&
                strMessage[nPos + 10] == '>' && !bLocalImageEnd) {
                //本地图片
                bLocalImageEnd   = true;
                bLocalImageBegin = false;
                nPos += 11;
            }

            if (bLocalImageBegin) {
                strLocalImage += strMessage[nPos];
                if (strMessage[nPos] & 0x80) {
                    //汉字
                    nPos++;
                    strLocalImage += strMessage[nPos];
                    nItemNameNum += 2;
                } else {
                    nItemNameNum++;
                }
                nPos++;
            }

            if (bLocalImageEnd) {
                rt2_sprintf(szFackToken, "%s", strLocalImage.c_str());
                RtwImage* pLocalImage = g_workspace.getImageFactory()->createImage(szFackToken);

                if (!pLocalImage) {
                    continue;
                }

                pLocalImage->SetBlend(true);
                itemImage.type = eit_Image;
                itemImage.SetImage(pLocalImage);
                itemImage.imgSize = pLocalImage->getRenderImage()->getSize();

                nImageNum++;

                SLine* pLastLine = getLine((int)m_Lines.size() - 1);
                if (pLastLine->rect.getWidth() + itemImage.imgSize.width > nLineWidth) {
                    if (pLastLine) {
                        SLine lineNew;
                        InsertLine(lineNew);  //插入空行

                        pLastLine = getLine((int)m_Lines.size() - 1);
                        pLastLine->InsertItem(itemImage);
                        CalcLinePosition((int)m_Lines.size() - 1);

                        strTempMsg = "";
                        nImageNum  = 0;
                        nWordsNum  = 0;
                    }
                } else {
                    pLastLine->InsertItem(itemImage);
                    nWordsNum = 0;
                }
                CalcLinePosition((int)m_Lines.size() - 1);
                bItemLinkEnd = false;
                strItemLink  = "";
            }
        } else if (bItemLinkBegin) {
            //道具链接
            if (strMessage[nPos] == ']' && strMessage[nPos + 1] == 'm' &&
                strMessage[nPos + 2] == 'e' && strMessage[nPos + 3] == 't' &&
                strMessage[nPos + 4] == 'i' && bItemLinkBegin) {
                //道具链接结束
                bItemLinkEnd   = true;
                bItemLinkBegin = false;
                nPos += 5;
            }

            if (bItemLinkBegin) {
                strItemLink += strMessage[nPos];
                if (strMessage[nPos] & 0x80) {
                    //汉字
                    nPos++;
                    strItemLink += strMessage[nPos];
                    nItemNameNum += 2;
                } else {
                    nItemNameNum++;
                }
                nPos++;
            }

            if (bItemLinkEnd) {
                //解析道具id,颜色,道具名
                sscanf(strItemLink.c_str(), "%d^)!%s", &nItemColor, &szLinkInfo);
                if (strlen(szLinkInfo) == 0) {
                    continue;
                }

                bool bIsName = true;
                int  i       = 0;

                std::string tmpItemName = "";
                std::string tmpLinkInfo = "";

                while (i < (int)string(szLinkInfo).size()) {
                    if (szLinkInfo[i] == '^' && szLinkInfo[i + 1] == ')' &&
                        szLinkInfo[i + 2] == '!') {
                        bIsName = false;
                        i += 3;
                    }

                    if (bIsName) {
                        tmpItemName += szLinkInfo[i];
                    } else {
                        tmpLinkInfo += szLinkInfo[i];
                    }
                    i++;
                }

                SHyperLink tmpLink;

                //道具链接
                tmpLink.childType = hlinkItem;
                tmpLink.Type      = hlinkClient;

                rt2_sprintf(szLinkInfo, "//showItemInfo %s", tmpLinkInfo.c_str());
                tmpLink.Text = szLinkInfo;

                itemLink.type = eit_Text;
                itemLink.text = "[";
                itemLink.text += tmpItemName.c_str();
                itemLink.text += "]";
                itemLink.color     = itemColorList[nItemColor % 4];
                itemLink.hyperLink = tmpLink;
                nWordsNum += (int)itemLink.text.size();

                SLine* pLastLine = getLine((int)m_Lines.size() - 1);
                if ((pLastLine->rect.getWidth() +
                         ((int)itemLink.text.size() * itemLink.nFontSize) / 2 >
                     nLineWidth)) {
                    string strTemp = itemLink.text;
                    int    num     = (nLineWidth - pLastLine->rect.getWidth()) /
                              itemLink.nFontSize;  //得到不需要换行的个数
                    num <<= 1;

                    int iHead = strTemp[num] & 0xff;
                    int iTail = strTemp[num + 1] & 0xff;
                    //gb2312编码
                    if (iHead >= 0xa1 && iHead <= 0xf7 && iTail >= 0xa1 && iTail <= 0xfe) {
                        num--;
                    }

                    itemLink.text = strTemp.substr(0, num);

                    pLastLine->InsertItem(itemLink);
                    CalcLinePosition((int)m_Lines.size() - 1);

                    SLine lineNew;
                    InsertLine(lineNew);  //插入空行
                    itemLink.text    = strTemp.substr(num);
                    SLine* pLastLine = getLine((int)m_Lines.size() - 1);
                    pLastLine->InsertItem(itemLink);

                    strTempMsg   = "";
                    nWordsNum    = nItemNameNum;
                    nItemNameNum = 0;
                } else {
                    pLastLine->InsertItem(itemLink);
                    nItemNameNum = 0;
                    nWordsNum    = 0;
                }

                CalcLinePosition((int)m_Lines.size() - 1);

                bItemLinkEnd = false;

                strItemLink = "";
            }
        } else if (bClientMoveToBegin) {
            //寻路链接
            if (strMessage[nPos] == '<' && strMessage[nPos + 1] == '/' &&
                strMessage[nPos + 2] == 'o' && strMessage[nPos + 3] == 't' &&
                strMessage[nPos + 4] == 'e' && strMessage[nPos + 5] == 'v' &&
                strMessage[nPos + 6] == 'o' && strMessage[nPos + 7] == 'm' &&
                strMessage[nPos + 8] == '>' && bClientMoveToBegin) {
                //寻路链接结束
                bClientMoveToEnd   = true;
                bClientMoveToBegin = false;
                nPos += 9;
            }

            if (bClientMoveToBegin) {
                strMoveToLink += strMessage[nPos];
                if (strMessage[nPos] & 0x80) {
                    //汉字
                    nPos++;
                    strMoveToLink += strMessage[nPos];
                    nMoveToNum += 2;
                } else {
                    nMoveToNum++;
                }
                nPos++;
            }

            if (bClientMoveToEnd) {
                //解析移动的坐标
                char buf[128];
                sscanf(strMoveToLink.c_str(), "%d,%d,%s", &nMoveX, &nMoveY, buf);

                SHyperLink tmpLink;

                //道具链接
                tmpLink.childType = hlinkItem;
                tmpLink.Type      = hlinkClient;

                rt2_sprintf(szLinkInfo, "//clientmoveto %d %d", nMoveX, nMoveY);
                tmpLink.Text = szLinkInfo;

                itemLink.type = eit_Text;
                itemLink.text = "[";
                itemLink.text += buf;
                itemLink.text += "]";
                itemLink.color      = 0xaabbcc00;
                itemLink.bUnderLine = true;
                itemLink.hyperLink  = tmpLink;

                nWordsNum += strlen(buf);

                SLine* pLastLine = getLine((int)m_Lines.size() - 1);
                if ((pLastLine->rect.getWidth() +
                         ((int)itemLink.text.size() * itemLink.nFontSize) / 2 >
                     nLineWidth)) {
                    string strTemp = itemLink.text;
                    int    num     = (nLineWidth - pLastLine->rect.getWidth()) /
                              itemLink.nFontSize;  //得到不需要换行的个数
                    num <<= 1;

                    int iHead = strTemp[num] & 0xff;
                    int iTail = strTemp[num + 1] & 0xff;
                    //gb2312编码
                    if (iHead >= 0xa1 && iHead <= 0xf7 && iTail >= 0xa1 && iTail <= 0xfe) {
                        num--;
                    }

                    itemLink.text = strTemp.substr(0, num);

                    pLastLine->InsertItem(itemLink);
                    CalcLinePosition((int)m_Lines.size() - 1);

                    SLine lineNew;
                    InsertLine(lineNew);  //插入空行
                    itemLink.text    = strTemp.substr(num);
                    SLine* pLastLine = getLine((int)m_Lines.size() - 1);
                    pLastLine->InsertItem(itemLink);

                    strTempMsg = "";
                    nWordsNum  = nItemNameNum;
                    nMoveToNum = 0;
                } else {
                    pLastLine->InsertItem(itemLink);
                    nMoveToNum = 0;
                    nWordsNum  = 0;
                }

                CalcLinePosition((int)m_Lines.size() - 1);

                bClientMoveToEnd = false;

                strMoveToLink = "";
            }
        } else if (bTaskLinkBegin) {
            //任务链接开启
            bTaskLinkBegin = false;  //暂未开放任务链接功能
        } else {
            SLine* pLastLine = getLine((int)m_Lines.size() - 1);

            //普通文本
            strTempMsg += strMessage[nPos];
            if (strMessage[nPos] & 0x80) {
                nPos++;
                nWordsNum += 2;
                strTempMsg += strMessage[nPos];
            } else {
                nWordsNum += 1;
            }
            nPos++;

            if (pLastLine->rect.getWidth() + (int)strTempMsg.size() * 6 > nLineWidth) {
                SItem itemText;
                itemText.text  = strTempMsg;
                itemText.type  = eit_Text;
                itemText.color = RtwPixel(color);

                pLastLine->InsertItem(itemText);

                CalcLinePosition((int)m_Lines.size() - 1);

                SLine lineTemp;
                InsertLine(lineTemp);

                nWordsNum  = 0;
                strTempMsg = "";
            }
        }

        if (!strTempMsg.empty()) {
            SLine* pLastLine = getLine((int)m_Lines.size() - 1);

            itemTemp.text  = strTempMsg;
            itemTemp.type  = eit_Text;
            itemTemp.color = RtwPixel(color);
            pLastLine->InsertItem(itemTemp);
            nWordsNum = 0;

            CalcLinePosition((int)m_Lines.size() - 1);
            strTempMsg = "";
        }
    }

    SetCursorPos(0, (int)text.size());
    if (m_Text != text) {
        m_Text = text;
    }
}

//获取文本内容
//begin
std::string RtwTextBox::getText() const {
    return GetText();
}

std::string RtwTextBox::GetText() const {
    char        szBuf[1024] = "\0";
    std::string str         = "";
    for (std::list<SLine>::const_iterator iterLine = m_Lines.begin(); iterLine != m_Lines.end();
         iterLine++) {
        const SLine& line = *iterLine;
        for (std::list<SItem>::const_iterator iterItem = line.lineitems.begin();
             iterItem != line.lineitems.end(); iterItem++) {
            const SItem& item = *iterItem;
            if (item.type == eit_Text || item.type == eit_Name || item.type == eit_Image) {
                if (item.hyperLink.childType == hlinkInvalid) {
                    str += item.text;
                } else if (item.hyperLink.childType == hlinkTask) {
                    str += item.text;
                } else if (item.hyperLink.childType == hlinkItem) {
                    if ((int)item.hyperLink.vecParam.size() == 3) {
                        rt2_sprintf(szBuf, "item[%ld, %ld, %ld]meti", item.hyperLink.vecParam[0],
                                    item.hyperLink.vecParam[1], item.hyperLink.vecParam[2]);
                        str += szBuf;
                    }
                } else {
                    str += item.text;
                }
            }
        }
    }
    return str;
}

//end

//获取指定行
RtwTextBox::SLine* RtwTextBox::getLine(int index) {
    if (index < 0)
        return NULL;

    if (index >= (int)m_Lines.size())
        return NULL;

    std::list<SLine>::iterator iterLine = m_Lines.begin();
    for (int i = 0; i < index; ++i) {
        iterLine++;
    }

    return &(*iterLine);
}

//绘制前台
void RtwTextBox::DrawForeground(const RtwRect* pClipRect /* = NULL*/) {
    RtwRect rcAfterClip = m_ViewRect;
    if (pClipRect)
        rcAfterClip.ClipBy(*pClipRect);

    bool bClick, bHover;  //item是否有鼠标事件
    // 内容
    foreach (std::list<SLine>, iterLine, m_Lines) {
        SLine& line = *iterLine;
        if (!rcAfterClip.IsIntersect(line.rect))
            continue;

        foreach (std::list<SItem>, iterItem, line.lineitems) {
            bClick = false;
            bHover = false;

            SItem& item = *iterItem;
            if (!rcAfterClip.IsIntersect(item.rect))
                continue;
            RtwRect          rcRender = item.rect;
            SUiTextAlignment align(alignFar, alignFar);

            SPoint curPoint = g_workspace.getMousePos();

            if (g_workspace.getMouseCapture() == this) {
                if (PickItem(curPoint) == &item) {
                    bClick = true;
                }
            } else if (g_workspace.getMouseHover() == this) {
                if (PickItem(curPoint) == &item) {
                    bHover = true;
                }
            }

            switch (item.type) {
                case eit_Name:
                case eit_Text: {
                    // 绘制文本选中区域
                    RtwRect _rcSelect = item.rcSelect;
                    _rcSelect.ClipBy(rcAfterClip);
                    g_workspace.getRenderDevice()->DrawRectangle(_rcSelect, RtwPixel(0xff3950BA));

                    // 创建字体
                    RtwFont* pFont = NULL;
                    if (item.strFontName != "" || item.nFontSize > 0) {
                        pFont = g_workspace.getFontManager()->CreateFont(
                            item.strFontName, item.nFontSize, RtwFont::FONT_NORMAL);
                    } else {
                        pFont =
                            g_workspace.getFontManager()->CreateFont("", 12, RtwFont::FONT_NORMAL);
                    }

                    //密码形式显示
                    if (!m_bPassword) {
                        RtwPixel txtColor = item.color;
                        //支持链接颜色
                        if (!item.hyperLink.Text.empty()) {
                            if (item.hyperLink.normalColor != -1) {
                                txtColor = item.hyperLink.normalColor;
                            }
                            if (bHover) {
                                if (item.hyperLink.hilghtColor != -1) {
                                    txtColor = item.hyperLink.hilghtColor;
                                } else {
                                    txtColor =
                                        g_workspace.getHyperLinkDefaultStyle().HighlightColor;
                                }
                            } else if (bClick) {
                                if (item.hyperLink.pushedColor != -1) {
                                    txtColor = item.hyperLink.pushedColor;
                                }
                            }
                        }

                        if (item.type == eit_Name) {
                            //名字变色
                            if (bHover && m_bNameColorChange) {
                                txtColor = 0xffff0000;
                                txtColor *= 1.0;
                            }
                        }

                        g_workspace.getFontManager()->DrawString(
                            item.text.c_str(), rcRender, SETCOLORALPHA(txtColor, m_Transparency),
                            pFont, false, align, item.bUnderLine, &rcAfterClip);

                    } else {
                        std::string strPass;
                        for (int i = 0; i < (int)item.text.size(); ++i)
                            strPass.push_back(m_Password_ShowChar);

                        g_workspace.getFontManager()->DrawString(
                            strPass.c_str(), rcRender, SETCOLORALPHA(item.color, m_Transparency),
                            pFont, false, align, item.bUnderLine, &rcAfterClip);
                    }

                    if (item.bDraw) {
                        RtwRect selectrect = item.rect;
                        selectrect.left += 1;
                        selectrect.top += 1;
                        selectrect.bottom -= 1;
                        g_workspace.getRenderDevice()->DrawRectangle(selectrect, 0xb24060f0);
                    }
                } break;
                case eit_Image: {
                    //图片
                    rcRender.bottom = rcRender.top + item.imgSize.height;
                    rcRender.right  = rcRender.left + item.imgSize.width;
                    //   					rcRender.ClipBy(rcAfterClip);

                    RtwImage* pCurrentImage = item.image;
                    // 					SPoint curPoint = g_workspace.getMousePos();
                    //
                    // 					if (g_workspace.getMouseCapture() == this)
                    // 					{
                    // 						if (PickItem(curPoint) == &item)
                    // 						{
                    // 							pCurrentImage = item.pPushedImage;
                    // 						}
                    // 					}
                    // 					else if (g_workspace.getMouseHover() == this)
                    // 					{
                    // 						if (PickItem(curPoint) == &item)
                    // 						{
                    // 							pCurrentImage = item.pHoverImage;
                    // 						}
                    // 					}

                    if (pCurrentImage) {
                        pCurrentImage->getRenderImage()->SetCtrlTransparent(m_Transparency);
                        g_workspace.getRenderDevice()->DrawImage(pCurrentImage->getRenderImage(),
                                                                 rcRender, align, pClipRect);
                    }
                } break;
                case eit_ImageSequence:
                    //图片序列(估计用不到了)
                    if (item.getImageSequence() && item.getImageSequence()->getCurrentImage()) {
                        if (rcRender.top > item.rect.top)
                            align.lineAlign = alignFar;
                        item.getImageSequence()->getCurrentImage()->SetCtrlTransparent(
                            m_Transparency);
                        g_workspace.getRenderDevice()->DrawImage(
                            item.getImageSequence()->getCurrentImage(), rcRender, align,
                            &rcAfterClip);
                    }
                default:
                    break;
            }
        }
    }

    RtwWidget::DrawForeground(pClipRect);

    // 画光标位置 @@@@@
    if (g_workspace.getFocusWidget() == this && getEnableInput()) {
        if ((rtMilliseconds() / 500) & 1) {
            if (m_CursorRect.top >= rcAfterClip.top && m_CursorRect.bottom <= rcAfterClip.bottom)
                g_workspace.getRenderDevice()->DrawRectangle(
                    m_CursorRect, SETCOLORALPHA(m_CursorColor, m_Transparency));
        }
    }
}

//绘制背景
void RtwTextBox::DrawBackground(const RtwRect* pClipRect /* = NULL*/) {
    RtwWidget::DrawBackground(pClipRect);
}

//计算矩形
void RtwTextBox::CalcClient() {
    m_rcClient = m_rcFrame;
    //自动纵向滚动条
    if (m_bAutoScrollV) {
        m_rcClient.right -= g_workspace.getDefaultScrollVWidth() + 2;
    }

    m_ViewRect.left   = m_rcClient.left + m_LeftSpace + m_LeftMargin;
    m_ViewRect.right  = m_rcClient.right - m_LeftSpace - m_RightMargin;
    m_ViewRect.top    = m_rcClient.top + m_LineSpace + m_TopMargin;
    m_ViewRect.bottom = m_rcClient.bottom - m_LineSpace - m_BottomMargin;

    if (m_pScrollBarV) {
        m_pScrollBarV->Move(RtwRect(m_rcFrame.right - g_workspace.getDefaultScrollVWidth(),
                                    m_rcFrame.top, m_rcFrame.right, m_rcFrame.bottom));
        m_pScrollBarV->Offset(SSize(-m_rcFrame.left, -m_rcFrame.top));
    }
}

//光标自适应位置
void RtwTextBox::AutoAdjectCursor() {
    // 	if (m_bAutoSizeV || m_bAutoSizeH)
    // 		return ;
    //
    while (!m_ViewRect.IsContain(m_CursorRect)) {
        bool bProcessed = false;
        if (m_ViewRect.left >= m_CursorRect.right) {
            if (false == m_bWordWrap) {
                m_ViewOffsetCol += 48;
                OffsetItemPosition(SSize(48, 0), 0);
                bProcessed = true;
            } else {
                m_CursorPosLine -= 1;
                m_CursorPosLine = m_CursorPosLine < 0 ? 0 : m_CursorPosLine;
                m_CursorPosCol  = 0;
                RefreshCursorPosition();
            }
        } else if (m_ViewRect.right <= m_CursorRect.left) {
            if (false == m_bWordWrap) {
                m_ViewOffsetCol -= 48;
                OffsetItemPosition(SSize(-48, 0), 0);
                bProcessed = true;
            } else {
                m_CursorPosLine += 1;
                SLine* pLine = getLine(m_CursorPosLine);
                if (NULL == pLine) {
                    SLine line;
                    InsertLine(line, m_CursorPosLine);
                }
                m_CursorPosCol = 0;
                RefreshCursorPosition();
                ScrollToBottom();
            }
        }
        if (!bProcessed)
            break;
    }
}

bool RtwTextBox::_DivideTextItem(SItem& ItemToDivide, int DivideIndexFrom, SItem& outNewItem) {
    //分割字符串
    UI_ENSURE_B(ItemToDivide.type == eit_Text);
    UI_ENSURE_B(DivideIndexFrom >= 0 && DivideIndexFrom < (int)ItemToDivide.text.size());

    outNewItem        = ItemToDivide;
    outNewItem.text   = outNewItem.text.substr(DivideIndexFrom);
    ItemToDivide.text = ItemToDivide.text.substr(0, DivideIndexFrom);

    return true;
}

//合并临近的item
bool RtwTextBox::_MergeNeighborItem(SItem* pItem1, SItem* pItem2) {
    if (!pItem1 || !pItem2 || pItem1->type != eit_Text || pItem2->type != eit_Text ||
        pItem1->color != pItem2->color)
        return false;

    char p = pItem2->text[0];
    if (p & 0x80)
        pItem1->text = pItem2->text + pItem1->text;
    else
        pItem1->text += pItem2->text;
    return true;
}

//合并两个item
bool RtwTextBox::_MergeItem(SItem* pItem1, SItem* pItem2) {
    if (!pItem1 || !pItem2 || pItem1->type != eit_Text || pItem2->type != eit_Text)
        return false;

    pItem1->text += pItem2->text;
    return true;
}

//当前行和下一行合并
bool RtwTextBox::_MergeWrapedLine(int line) {
    //得到行
    SLine* pLine = getLine(line);

    int    MergeLineIndex = line + 1;
    SLine* pNextLine      = getLine(MergeLineIndex);

    if (!pLine || pLine->lineitems.empty()          //存在此行,且内容不是空
        || !pNextLine || !pNextLine->bWordWrapTail  //存在下一行,
    ) {
        return false;
    }

    SItem* pItem = &(pLine->lineitems.back());  //第一行的最后一个item
    while (pNextLine && pNextLine->bWordWrapTail) {
        if (pNextLine->lineitems.empty())
            return false;

        SItem* _pItem = &(pNextLine->lineitems.front());  //下一行的第一个item

        UI_CHECK_B(_pItem->type == eit_Text);
        UI_CHECK_B(_MergeItem(pItem, _pItem));

        //依次合并
        pNextLine->lineitems.pop_front();
        foreach (std::list<SItem>, iterItem, pNextLine->lineitems) {
            _pItem = &(*iterItem);
            pLine->lineitems.push_back(*_pItem);
        }
        //合并完了就删掉
        UI_CHECK_B(RemoveLine(MergeLineIndex));
        pNextLine = getLine(MergeLineIndex);
    }
    return true;
}

//删除行
bool RtwTextBox::RemoveLine(int line) {
    //越界
    if (line < 0 || line >= (int)m_Lines.size())
        return false;

    //行高
    int LineHeight = 0;
    int LineIndex  = 0;

    std::list<SLine>::iterator iterLine = m_Lines.begin();
    for (; iterLine != m_Lines.end(); iterLine++, ++LineIndex) {
        if (LineIndex == line) {
            SLine& _Line = *iterLine;
            LineHeight   = _Line.rect.getHeight();
            break;
        }
    }
    //删掉
    m_Lines.erase(iterLine);

    //删除item后,下面的移上来
    OffsetItemPosition(SSize(0, LineHeight + m_LineSpace), line);

    return true;
}

//删除所以行
bool RtwTextBox::RemoveAllLines() {
    while (!m_Lines.empty()) {
        UI_ENSURE_B(RemoveLine((int)m_Lines.size() - 1));
    }

    return true;
}

//删除所有item
bool RtwTextBox::RemoveAllItems(bool bRefreshItemPos /* = true*/) {
    while (!m_Lines.empty()) {
        UI_ENSURE_B(RemoveLine(0));
    }
    return true;
}

//在光标位置插入字符串
bool RtwTextBox::InsertStringToCurrentCursor(const std::string& str) {
    for (size_t s = 0; s < str.size(); ++s) {
        //在光标位置插入字符串
        InsertCharacterToCurrentCursor(str[s]);
    }
    return false;
}

bool RtwTextBox::InsertCharacterToCurrentCursor(const char character) {
    bool bProcessed = false;

    if (m_Lines.empty()) {
        // 如果还没有行，就添加一个
        SLine line;
        InsertLine(line, -1);
    }

    //功能键排除
    //begin
    if (character == vkBack)
        return false;

    if (character == vkEscape)
        return false;

    else if (character == vkEnter)
        return false;

    std::string sText = getText();
    if (m_Capacity > 0 && m_Capacity <= (int)sText.size()) {
        return false;
    }
    //end

    SLine* pLine = getLine(m_CursorPosLine);
    UI_ENSURE_B(pLine);

    //行内没有item
    if (pLine->lineitems.empty()) {
        //默认字体
        SItem item;
        item.text  = character;
        item.type  = eit_Text;
        item.color = m_TextColor;
        pLine->InsertItem(item, 0);
        bProcessed = true;
    } else {
        if (m_addface) {
            SItem pItem;
            pItem.type = eit_Text;
            pItem.text.insert(0, 1, character);
            getLine(m_CursorPosLine)->InsertItem(pItem);
            m_addface  = false;
            bProcessed = true;
        } else {
            SItem* _pItem = pLine->getItem((int)pLine->lineitems.size() - 1);
            if (_pItem->type != eit_Text)
                return false;
            if (_pItem->hyperLink.childType == hlinkChildInvalid) {
                int _InsertIndex = m_CursorPosCol;
                if (m_CursorPosCol > (int)_pItem->text.size()) {
                    _InsertIndex = (int)_pItem->text.size();
                }

                if (m_Capacity > 0 && (int)_pItem->text.size() >= m_Capacity) {
                    return false;
                }

                _pItem->text.insert(_InsertIndex, 1, character);
            } else {
                SItem item;
                item.text  = character;
                item.type  = eit_Text;
                item.color = m_TextColor;
                pLine->InsertItem(item);
            }

            bProcessed = true;
        }
    }

    if (bProcessed) {
        m_CursorPosCol++;
        CalcLinePosition(m_CursorPosLine);
        RefreshCursorPosition();
        AutoAdjectCursor();
    }

    return bProcessed;
}

bool RtwTextBox::InsertItemToCurrentCursor(SItem& item) {
    return false;
}

//捡取item
bool RtwTextBox::PickItem(SPoint& point, int& outLine, int& outCol, int& outItemIndex,
                          bool& bDoubleByte, bool& outNear) {
    outLine                             = 0;
    std::list<SLine>::iterator iterLine = m_Lines.begin();
    for (; iterLine != m_Lines.end(); ++iterLine) {
        SLine& line = *iterLine;
        int    j    = 0;
        outCol      = 0;

        // 根据高度判断
        if ((point.y >= line.rect.top) && (point.y <= line.rect.bottom)) {
            foreach (std::list<SItem>, iterItem, line.lineitems) {
                SItem& item = *iterItem;
                if (item.rect.IsContain(point)) {
                    switch (item.type) {
                        case eit_Name:
                        case eit_Text: {
                            int  iIndex = 0;
                            bool bNear  = true;

                            RtwFont* pkFont = g_workspace.getFontManager()->CreateFont(
                                item.strFontName, item.nFontSize);

                            if (pkFont->PickCharacter(point, iIndex, bDoubleByte, bNear, item.text,
                                                      item.rect)) {
                                outItemIndex = j;
                                outCol += iIndex;
                                outNear = bNear;
                            }
                        }
                            return true;
                        case eit_Image:
                        case eit_ImageSequence:
                            outItemIndex = j;
                            if (item.rect.getCenter().x > point.x)
                                outNear = true;
                            else
                                outNear = false;
                            return true;
                        default:
                            break;
                    }
                } else {
                    outCol += item.getColCount();
                }
                j++;
            }
            return false;
        }

        outLine++;
    }

    return false;
}

//返回捡取到得item
RtwTextBox::SItem* RtwTextBox::PickItem(const SPoint& point) {
    for (std::list<SLine>::iterator iterLine = m_Lines.begin(); iterLine != m_Lines.end();
         ++iterLine) {
        SLine& line = *iterLine;

        // 根据高度判断
        if ((point.y >= line.rect.top) && (point.y <= line.rect.bottom)) {
            foreach (std::list<SItem>, iterItem, line.lineitems) {
                SItem& item = *iterItem;
                if (item.rect.IsContain(point)) {
                    return &item;
                }
            }
            return NULL;
        }
    }

    return NULL;
}

void RtwTextBox::OnEvUnFocus(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    _SetSelectionStart(0, 0);
    _SetSelectionEnd(0, 0);
    RefreshSelectionRect();
}

void RtwTextBox::OnEvLDClick(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    if (!getEnableInput())
        return;
    if (m_Lines.empty())
        return;
    int  line, col, itemIndex;
    bool bDoubleByte;
    bool bNear;

    if (!m_Lines.empty()) {
        //捡取item
        if (PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y), line, col, itemIndex, bDoubleByte,
                     bNear)) {
            SelectAll();
        }
    }
}

//mousemove
void RtwTextBox::OnMouseMove_This(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    if (!getEnableInput())
        return;
    static bool wasMouseIn = false;      // 记录鼠标上次是否在输入框内
    static int  startX = 0, startY = 0;  // 记录鼠标按下时的位置
    int         mouseX = pEvent->mouse.x;
    int         mouseY = pEvent->mouse.y;

    // 获取输入框的区域
    RtwRect inputRect = getRect();

    if (pEvent->mouse.button == 1) {  // 如果左键仍然按下
        if (inputRect.IsContain(SPoint(mouseX, mouseY))) {
            // 鼠标在输入框内
            wasMouseIn = true;

            int  line, col, itemIndex;
            bool bDoubleByte;
            bool bNear;

            bool isPicked =
                PickItem(SPoint(mouseX, mouseY), line, col, itemIndex, bDoubleByte, bNear);

            if (isPicked) {
                m_CursorPosLine = line;
                m_CursorPosCol  = col;
                if (!bNear) {
                    if (bDoubleByte)
                        m_CursorPosCol += 2;
                    else
                        m_CursorPosCol += 1;
                }
                SetCursorPos(m_CursorPosLine, m_CursorPosCol);
                _SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
            }

            RefreshSelectionRect();
        } else {
            // 鼠标离开输入框
            if (wasMouseIn) {
                // 计算鼠标移动量
                int dx = mouseX - startX;
                int dy = mouseY - startY;

                // 以鼠标移动量调整光标位置
                // 这里假设光标移动的像素量对应一定的列数移动
                int colMove = dx / (m_FontSize >> 2);  // 假设每列的宽度
                int rowMove = dy / m_FontSize;         // 假设每行的高度

                m_CursorPosCol += colMove;
                m_CursorPosLine += rowMove;

                // 限制光标位置在文本框内
                if (m_CursorPosLine < 0)
                    m_CursorPosLine = 0;
                if (m_CursorPosCol < 0)
                    m_CursorPosCol = 0;

                // 更新选择区域的结束位置
                SetCursorPos(m_CursorPosLine, m_CursorPosCol);
                _SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);

                // 刷新选择区域
                RefreshSelectionRect();
            }
        }
    } else {
        // 鼠标左键抬起，重置状态
        wasMouseIn = false;
    }

    // 更新记录的鼠标位置
    startX = mouseX;
    startY = mouseY;
}

//点击事件
void RtwTextBox::OnLClick_This(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    // 处理超链接
    SItem* pItem = PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y));
    if (pItem && pItem->type == eit_Text && pItem->hyperLink.Type != hlinkInvalid) {}
}

void RtwTextBox::OnEvHyperLink(RtwWidget* pWidget, RtwEventDelegate* pEvent) {}

//点击事件
void RtwTextBox::OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    if (!getEnableInput())
        return;

    int  line, col, itemIndex;
    bool bDoubleByte;
    bool bNear;

    if (!m_Lines.empty()) {
        //捡取item
        if (PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y), line, col, itemIndex, bDoubleByte,
                     bNear)) {
            // 光标
            m_CursorPosLine = line;
            m_CursorPosCol  = col;
            if (!bNear) {
                if (bDoubleByte)
                    m_CursorPosCol += 2;
                else
                    m_CursorPosCol += 1;
            }
        } else {
            //没捡取到,默认位置
            int textsize  = 0;
            int imagesize = 0;

            m_CursorPosLine  = (int)m_Lines.size() - 1;
            SLine* pLastLine = getLine((int)m_Lines.size() - 1);

            if (pLastLine && !pLastLine->lineitems.empty()) {
                foreach (std::list<SItem>, iterItem, pLastLine->lineitems) {
                    SItem& item = *iterItem;
                    if (item.type == eit_Text) {
                        textsize += (int)item.text.size();
                    } else if (item.type == eit_Image) {
                        imagesize++;
                    }
                }
                m_CursorPosCol = textsize + imagesize;
            } else {
                m_CursorPosCol = 0;
            }
        }
        // 重置选中字符
        _SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
        _SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
        RefreshSelectionRect();
    }
    RefreshCursorPosition();
}

//键盘按下
void RtwTextBox::OnKeyDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    if (!getEnableInput())
        return;
    pEvent->cancle = false;
    switch (pEvent->key.code) {
        case vkTab:
            return;
        case vkLeft: {
            //移动
            if (m_CursorPosCol > 0) {
                SItem* pItemIn           = NULL;
                SItem* pItemBetweenLeft  = NULL;
                SItem* pItemBetweenRight = NULL;
                int    nItemIndex        = 0;
                if (_GetItemByLineCol(m_CursorPosLine, m_CursorPosCol, &pItemIn, &nItemIndex,
                                      &pItemBetweenLeft, &pItemBetweenRight)) {
                    if (pItemIn) {
                        if (pItemIn->type == eit_Text) {
                            // 判断前一个字符是不是双字节字符
                            if (nItemIndex <= 1)
                                m_CursorPosCol -= 1;
                            else {
                                char p = pItemIn->text[nItemIndex - 2];
                                if (p & 0x80) {
                                    char q = pItemIn->text[nItemIndex - 1];
                                    if (q & 0x80)
                                        m_CursorPosCol -= 2;
                                    else
                                        m_CursorPosCol -= 1;
                                } else {
                                    m_CursorPosCol -= 1;
                                }
                            }
                        } else {
                            m_CursorPosCol -= 1;
                            m_CursorRect.left -= 20;
                            m_CursorRect.right -= 20;
                        }
                    } else if (pItemBetweenLeft) {
                        if (pItemBetweenLeft->type == eit_Text) {
                            // 判断前一个字符是不是双字节字符
                            int strsize = (int)pItemBetweenLeft->text.size();
                            if (strsize <= 1)
                                m_CursorPosCol -= 1;
                            else {
                                char p = pItemBetweenLeft->text[strsize - 2];
                                if (p & 0x80) {
                                    char q = pItemBetweenLeft->text[strsize - 1];
                                    if (q & 0x80)
                                        m_CursorPosCol -= 2;
                                    else
                                        m_CursorPosCol -= 1;
                                } else {
                                    m_CursorPosCol -= 1;
                                }
                            }
                        } else {
                            m_CursorPosCol -= 1;
                            m_CursorRect.left -= 20;
                            m_CursorRect.right -= 20;
                        }
                    }
                }
                if (m_CursorPosCol < 0)
                    m_CursorPosCol = 0;

                // 重置选中字符
                _SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
                _SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
                RefreshSelectionRect();

                RefreshCursorPosition();

                AutoAdjectCursor();
            }
        } break;
        case vkRight: {
            SItem* pItemIn           = NULL;
            int    nItemIndex        = 0;
            SItem* pItemBetweenLeft  = NULL;
            SItem* pItemBetweenRight = NULL;
            if (_GetItemByLineCol(m_CursorPosLine, m_CursorPosCol, &pItemIn, &nItemIndex,
                                  &pItemBetweenLeft, &pItemBetweenRight)) {
                if (pItemIn) {
                    if (pItemIn->type == eit_Text) {
                        char p = pItemIn->text[nItemIndex];
                        if (p & 0x80)
                            m_CursorPosCol += 2;
                        else
                            m_CursorPosCol += 1;
                    } else {
                        m_CursorPosCol += 1;
                        m_CursorRect.left += 20;
                        m_CursorRect.right += 20;
                    }
                } else if (pItemBetweenRight) {
                    if (pItemBetweenRight->type == eit_Text) {
                        if (!pItemBetweenRight->text.empty()) {
                            char p = pItemBetweenRight->text[0];
                            if (p & 0x80)
                                m_CursorPosCol += 2;
                            else
                                m_CursorPosCol += 1;
                        }
                    } else {
                        m_CursorPosCol += 1;
                        m_CursorRect.left += 20;
                        m_CursorRect.right += 20;
                    }
                }
            }
            if (m_CursorPosCol < 0)
                m_CursorPosCol = 0;

            // 重置选中字符
            _SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
            _SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
            RefreshSelectionRect();

            RefreshCursorPosition();

            AutoAdjectCursor();
        } break;
        case vkUp:
            if (m_CursorPosLine > 0) {
                SItem* pItemIn           = NULL;
                int    nItemIndex        = 0;
                SItem* pItemBetweenLeft  = NULL;
                SItem* pItemBetweenRight = NULL;
                if (_GetItemByLineCol(m_CursorPosLine, m_CursorPosCol, &pItemIn, &nItemIndex,
                                      &pItemBetweenLeft, &pItemBetweenRight)) {
                    if (pItemIn) {
                        if (pItemIn->type == eit_Text) {
                            char p = pItemIn->text[nItemIndex];
                            if (p & 0x80) {
                                // @@@@@
                            }
                        }
                    }
                }

                // 光标位置
                m_CursorPosLine--;

                // 重置选中字符
                _SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
                _SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
                RefreshSelectionRect();

                RefreshCursorPosition();
            }
            break;
        case vkDown:
            if (m_CursorPosLine < (int)m_Lines.size() - 1) {
                SItem* pItemIn           = NULL;
                int    nItemIndex        = 0;
                SItem* pItemBetweenLeft  = NULL;
                SItem* pItemBetweenRight = NULL;
                if (_GetItemByLineCol(m_CursorPosLine, m_CursorPosCol, &pItemIn, &nItemIndex,
                                      &pItemBetweenLeft, &pItemBetweenRight)) {
                    if (pItemIn) {
                        if (pItemIn->type == eit_Text) {
                            char p = pItemIn->text[nItemIndex];
                            if (p & 0x80) {
                                // @@@@@
                            }
                        }
                    }
                }

                // 光标位置
                m_CursorPosLine++;

                // 重置选中字符
                _SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
                _SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
                RefreshSelectionRect();

                RefreshCursorPosition();
            }
            break;
        case vkHome:
            if (m_CursorPosCol > 0) {
                m_CursorPosCol = 0;

                // 重置选中字符
                _SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
                _SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
                RefreshSelectionRect();

                RefreshCursorPosition();

                AutoAdjectCursor();
            }
            break;
        case vkEnd: {
            SLine* pLine = getLine(m_CursorPosLine);
            if (pLine && !pLine->lineitems.empty()) {
                SItem* pItem   = pLine->getItem(0);
                m_CursorPosCol = pItem->getColCount();

                // 重置选中字符
                _SetSelectionStart(m_CursorPosLine, m_CursorPosCol);
                _SetSelectionEnd(m_CursorPosLine, m_CursorPosCol);
                RefreshSelectionRect();

                RefreshCursorPosition();

                AutoAdjectCursor();
            }
        } break;
        case vkDelete: {
            if (m_SelStartLine == m_SelEndLine && m_SelStartCol == m_SelEndCol) {
                SLine* pLine = getLine(m_CursorPosLine);
                UI_ENSURE(pLine);
                SItem* pItemIn           = NULL;
                int    nItemIndex        = 0;
                SItem* pItemBetweenLeft  = NULL;
                SItem* pItemBetweenRight = NULL;
                if (_GetItemByLineCol(m_CursorPosLine, m_CursorPosCol, &pItemIn, &nItemIndex,
                                      &pItemBetweenLeft, &pItemBetweenRight)) {
                    if (pItemIn) {
                        if (pItemIn->type == eit_Text) {
                            char p = pItemIn->text[nItemIndex];
                            if (p & 0x80)
                                pItemIn->text.erase(nItemIndex, 2);
                            else
                                pItemIn->text.erase(nItemIndex, 1);
                        } else {
                            //pLine->RemoveItem(pItemBetweenRight);
                        }
                    } else if (pItemBetweenRight) {
                        if (pItemBetweenRight->type == eit_Text) {
                            char p = pItemBetweenRight->text[0];
                            if (p & 0x80)
                                pItemBetweenRight->text.erase(nItemIndex, 2);
                            else
                                pItemBetweenRight->text.erase(nItemIndex, 1);
                            if (pItemBetweenRight->text.empty())
                                pLine->RemoveItem(pItemBetweenRight);
                        } else {
                            //pLine->RemoveItem(pItemBetweenRight);
                        }
                    }
                }
                CalcLinePosition(m_CursorPosLine);
            } else {
                RemoveSelect();
            }

            AutoAdjectCursor();

            if (m_bWordWrap) {
                int         nSavePosLine = m_CursorPosLine;
                int         nSavePosCol  = m_CursorPosCol;
                std::string sStringTemp  = getText();
                RemoveAllLines();
                SetText(sStringTemp);
                SLine* kLine = getLine(nSavePosLine);
                if (NULL == kLine) {
                    kLine = getLine(nSavePosLine - 1);
                    if (kLine) {
                        nSavePosLine  = nSavePosLine - 1;
                        SItem* pkItem = kLine->getItem(0);
                        if (pkItem) {
                            nSavePosCol = (int)pkItem->text.size();
                            SetCursorPos(nSavePosLine, nSavePosCol);
                        } else {
                            SetCursorPos(nSavePosLine, 0);
                        }
                    } else {
                        SetCursorPos(0, 0);
                    }
                } else {
                    SetCursorPos(nSavePosLine, nSavePosCol);
                }
            }
        } break;
        case vkBack: {
            if (m_SelStartLine == m_SelEndLine && m_SelStartCol == m_SelEndCol) {
                SLine* pLine = getLine(m_CursorPosLine);
                UI_ENSURE(pLine);
                SItem* pItemIn           = NULL;
                int    nItemIndex        = 0;
                SItem* pItemBetweenLeft  = NULL;
                SItem* pItemBetweenRight = NULL;
                if (_GetItemByLineCol(m_CursorPosLine, m_CursorPosCol, &pItemIn, &nItemIndex,
                                      &pItemBetweenLeft, &pItemBetweenRight)) {
                    if (pItemIn) {
                        if (pItemIn->type == eit_Text) {
                            // 判断前一个字符是不是双字节字符
                            if (nItemIndex == 1) {
                                pItemIn->text.erase(0, 1);
                                m_CursorPosCol -= 1;
                            } else {
                                char p = pItemIn->text[nItemIndex - 2];
                                if (p & 0x80) {
                                    char q = pItemIn->text[nItemIndex - 1];
                                    if (q & 0x80) {
                                        pItemIn->text.erase(nItemIndex - 2, 2);
                                        m_CursorPosCol -= 2;
                                    } else {
                                        pItemIn->text.erase(nItemIndex - 1, 1);
                                        m_CursorPosCol -= 1;
                                    }
                                } else {
                                    pItemIn->text.erase(nItemIndex - 1, 1);
                                    m_CursorPosCol -= 1;
                                }
                            }
                        } else {
                            //pLine->RemoveItem(pItemBetweenLeft);
                            //m_CursorPosCol -= 1;
                        }
                    } else if (pItemBetweenLeft) {
                        if (pItemBetweenLeft->type == eit_Text) {
                            // 判断前一个字符是不是双字节字符
                            int strsize = (int)pItemBetweenLeft->text.size();
                            if (strsize == 1) {
                                pItemBetweenLeft->text.erase(0, 1);
                                m_CursorPosCol -= 1;
                            } else {
                                char p = pItemBetweenLeft->text[strsize - 2];
                                if (p & 0x80) {
                                    char q = pItemBetweenLeft->text[strsize - 1];
                                    if (q & 0x80) {
                                        pItemBetweenLeft->text.erase(strsize - 2, 2);
                                        m_CursorPosCol -= 2;
                                    } else {
                                        pItemBetweenLeft->text.erase(strsize - 1, 1);
                                        m_CursorPosCol -= 1;
                                    }
                                } else {
                                    pItemBetweenLeft->text.erase(strsize - 1, 1);
                                    m_CursorPosCol -= 1;
                                }
                            }

                            if (pItemBetweenLeft->text.empty())
                                pLine->RemoveItem(pItemBetweenLeft);
                        } else {
                            pLine->RemoveItem(pItemBetweenLeft);
                            m_CursorPosCol -= 1;
                        }
                    }
                }

                CalcLinePosition(m_CursorPosLine);
                RefreshCursorPosition();
            } else {
                RemoveSelect();
            }

            AutoAdjectCursor();
            if (m_bWordWrap) {
                int         nSavePosLine = m_CursorPosLine;
                int         nSavePosCol  = m_CursorPosCol;
                std::string sStringTemp  = getText();
                RemoveAllLines();
                SetText(sStringTemp);
                SLine* kLine = getLine(nSavePosLine);
                if (NULL == kLine) {
                    kLine = getLine(nSavePosLine - 1);
                    if (kLine) {
                        nSavePosLine  = nSavePosLine - 1;
                        SItem* pkItem = kLine->getItem(0);
                        if (pkItem) {
                            nSavePosCol = (int)pkItem->text.size();
                            SetCursorPos(nSavePosLine, nSavePosCol);
                        } else {
                            SetCursorPos(nSavePosLine, 0);
                        }
                    } else {
                        SetCursorPos(0, 0);
                    }
                } else {
                    SetCursorPos(nSavePosLine, nSavePosCol);
                }
            }
        } break;
        case 'c':
        case 'C':
            if (g_workspace.isKeyPressed(vkCtrl)) {
                if (g_workspace.getClipboard()) {
                    g_workspace.getClipboard()->Copy(getSelectText());
                }
                return;
            }
            break;
        case 'v':
        case 'V':
            if (g_workspace.isKeyPressed(vkCtrl)) {
                if (g_workspace.getClipboard()) {
                    std::string strClipboard;
                    g_workspace.getClipboard()->Paste(strClipboard);
                    if (!strClipboard.empty()) {
                        std::string sText = getText();
                        if (m_Capacity > 0 &&
                            m_Capacity <= (int)sText.size() + (int)strClipboard.size()) {
                            return;
                        }

                        RemoveSelect();
                        //AddText(strClipboard);
                        InsertStringToCurrentCursor(strClipboard);
                        AutoAdjectCursor();
                    }
                }
                return;
            }
            break;
        case 'x':
        case 'X':
            if (g_workspace.isKeyPressed(vkCtrl)) {
                if (g_workspace.getClipboard()) {
                    g_workspace.getClipboard()->Copy(getSelectText());
                    RemoveSelect();
                    AutoAdjectCursor();
                }
                return;
            }
            break;

        default:
            break;
    }
}

//
bool RtwTextBox::_PrepareEnterOk() {
    if (!m_bWordWrap || !m_bMultiLine || !m_bEnableInput) {
        return false;
    }

    SLine* pLine = getLine(m_CursorPosLine);
    if (!pLine) {
        return false;
    }

    int nStep = m_ViewRect.right - pLine->rect.right;
    if (nStep <= m_FontSize) {
        return true;
    }

    return false;
}

//键盘输入
void RtwTextBox::OnKeyChar_This(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    if (!getEnableInput())
        return;

    if (g_workspace.isKeyPressed(vkCtrl) || g_workspace.isKeyPressed(vkTab))
        return;

    //取消选择
    RemoveSelect();

    //输入的文本
    char character = pEvent->key.code;

    //不能输入数字?
    if (m_bNumeric && (character < 48 || character > 57)) {
        if (character == '-') {
            if (m_CursorPosCol != 0)  //不是第一个字符
            {
                return;
            }

            string tmp = getText();
            if (!tmp.empty()) {
                if (tmp[0] == '-') {
                    return;
                }
            }
        } else {
            return;
        }
    }

    //汉字?
    if (!m_acDoubleByte[0] && (character & 0x80)) {
        m_acDoubleByte[0] = character;
        return;
    }

    if (m_acDoubleByte[0])
        m_acDoubleByte[1] = character;

    if (m_acDoubleByte[0] && m_acDoubleByte[1]) {
        int iHead = m_acDoubleByte[0] & 0xff;
        int iTail = m_acDoubleByte[1] & 0xff;
        //gb2312编码
        if (iHead >= 0xa1 && iHead <= 0xf7 && iTail >= 0xa1 && iTail <= 0xfe) {
            if (_PrepareEnterOk()) {
                _Intercept_Enter(m_CursorPosLine, m_CursorPosCol);
            }
            InsertCharacterToCurrentCursor(m_acDoubleByte[0]);
            InsertCharacterToCurrentCursor(m_acDoubleByte[1]);
        } else {
            memset(m_acDoubleByte, 0, sizeof(char) * 2);
            return;
        }
    } else {
        InsertCharacterToCurrentCursor(character);
    }

    if (character == vkEnter) {
        if (!m_bMultiLine) {
            EvUpdateText(this, weUpdateText);
        }
        // 		else
        // 		{
        // 			_Intercept_Enter(m_CursorPosLine, m_CursorPosCol);
        // 		}
    }
    memset(m_acDoubleByte, 0, sizeof(char) * 2);
    EvInputChar(this, etInputChar);
}

//重置
void RtwTextBox::_ResetSelf() {
    m_WidgetType     = wtEditBox;
    m_Flag           = m_DefaultFlag;
    m_TextAlign      = SUiTextAlignment(alignNear, alignNear);
    m_bEnableInput   = true;
    m_bWordWrap      = false;
    m_bMultiLine     = false;
    m_bAutoScrollV   = false;
    m_ViewOffsetLine = 0;
    m_ViewOffsetCol  = 0;
    m_CursorPosLine  = 0;
    m_CursorPosCol   = 0;
    m_SelStartLine   = 0;
    m_SelStartCol    = 0;
    m_SelEndLine     = 0;
    m_SelEndCol      = 0;
    m_SelText        = "";
    m_bAutoSizeV     = false;
    m_bAutoSizeH     = false;
    m_Lines.clear();
    m_bNeedRefreshPosition = true;
}

//添加line,名字是错的. 在某个index添加line
bool RtwTextBox::AddItem(SLine& line, int indexBefore /* = -1*/, bool bRefreshItemPos /* = true*/) {
    if (indexBefore > (int)m_Lines.size())
        return false;

    if (indexBefore < 0) {
        m_Lines.push_back(line);
    } else {
        std::list<SLine>::iterator iterLine = m_Lines.begin();
        for (int i = 0; i < indexBefore; ++i) {
            iterLine++;
        }
        m_Lines.insert(iterLine, line);
    }

    return true;
}

//获取line的索引
int RtwTextBox::getLineIndex(SLine* pLine) {
    int index = 0;
    foreach (std::list<SLine>, iterLine, m_Lines) {
        SLine* _pLine = &(*iterLine);
        if (_pLine == pLine) {
            return index;
        }

        index++;
    }
    return -1;
}

//获取item的索引
int RtwTextBox::getItemIndex(SLine* pLine, SItem* pItem) {
    if (!pLine || !pItem)
        return 0;

    int index = 0;
    foreach (std::list<SItem>, iterItem, pLine->lineitems) {
        SItem* _pItem = &(*iterItem);
        if (_pItem == pItem) {
            return index;
        }

        index++;
    }

    return -1;
}

//获取某行某列的item
bool RtwTextBox::_GetItemByLineCol(int line, int col, SItem** ppItemIn, int* pTextItemIndex,
                                   SItem** ppItemBetweenLeft, SItem** ppItemBetweenRight) {
    *ppItemIn           = NULL;
    *pTextItemIndex     = 0;
    *ppItemBetweenLeft  = NULL;
    *ppItemBetweenRight = NULL;

    SLine* pLine = getLine(line);
    UI_ENSURE_B(pLine);

    if (col == 0) {
        *ppItemIn           = NULL;
        *pTextItemIndex     = 0;
        *ppItemBetweenLeft  = NULL;
        *ppItemBetweenRight = pLine->lineitems.empty() ? NULL : &(pLine->lineitems.front());
        return true;
    }

    bool bFound = false;
    int  index  = 0;
    foreach (std::list<SItem>, iterItem, pLine->lineitems) {
        SItem& item = *iterItem;
        switch (item.type) {
            case eit_Text:
                if (index + (int)item.text.size() > col) {
                    bFound              = true;
                    int delta           = col - index;
                    *ppItemIn           = &item;
                    *pTextItemIndex     = delta;
                    *ppItemBetweenLeft  = NULL;
                    *ppItemBetweenRight = NULL;
                } else if (index + (int)item.text.size() == col) {
                    bFound             = true;
                    *ppItemIn          = NULL;
                    *pTextItemIndex    = 0;
                    *ppItemBetweenLeft = &item;
                    iterItem++;
                    if (iterItem == pLine->lineitems.end())
                        *ppItemBetweenRight = NULL;
                    else
                        *ppItemBetweenRight = &(*iterItem);
                } else {
                    index += (int)item.text.size();
                }
                break;
            case eit_Image:
            case eit_ImageSequence:
                if (index + 1 == col) {
                    bFound             = true;
                    *ppItemIn          = NULL;
                    *pTextItemIndex    = 0;
                    *ppItemBetweenLeft = &item;
                    iterItem++;
                    if (iterItem == pLine->lineitems.end())
                        *ppItemBetweenRight = NULL;
                    else
                        *ppItemBetweenRight = &(*iterItem);
                } else {
                    index++;
                }
                break;
            default:
                break;
        }

        if (bFound)
            break;
    }

    return bFound;
}

//滚动条移动
void RtwTextBox::OnScroll_ScrollBarV(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    SLine* pFirstLine = getLine(0);
    UI_ENSURE(pFirstLine);

    SPoint DefaultStartPoint(m_ViewRect.left, m_ViewRect.top);
    SSize  OrigOffset(pFirstLine->rect.left - DefaultStartPoint.x,
                      pFirstLine->rect.top - DefaultStartPoint.y);

    int OrigValue = -OrigOffset.height;
    int value     = m_pScrollBarV->getValue();

    OffsetItemPosition(SSize(0, OrigValue - value), 0);
}

bool RtwTextBox::_Intercept(int line, int col, bool bWordWrap) {
    int    itemIndex, itemCol;
    SItem *pItemIn, *pItemBetweenLeft, *pItemBetweenRight;
    SLine* pLine = getLine(line);
    UI_ENSURE_B(pLine);
    UI_ENSURE_B(
        _GetItemByLineCol(line, col, &pItemIn, &itemCol, &pItemBetweenLeft, &pItemBetweenRight));
    if (pItemIn) {
        itemIndex = pLine->getItemIndex(pItemIn);
        return _Intercept(line, itemIndex, itemCol, bWordWrap);
    } else {
        if (pItemBetweenLeft) {
            itemIndex = pLine->getItemIndex(pItemBetweenLeft);
            return _Intercept(line, itemIndex, pItemBetweenLeft->getColCount(), bWordWrap);
        } else if (pItemBetweenRight) {
            itemIndex = pLine->getItemIndex(pItemBetweenRight);
            return _Intercept(line, itemIndex, 0, bWordWrap);
        } else {
            return false;
        }
    }
}

bool RtwTextBox::_Intercept(int line, int itemIndex, int itemCol, bool bWordWrap) {
    SLine* pLine = getLine(line);
    UI_ENSURE_B(pLine);
    SItem* pItem = pLine->getItem(itemIndex);
    UI_ENSURE_B(pItem);
    switch (pItem->type) {
        case eit_Text: {
            int _StartItemIndex = 0;

            SLine* pNewLine        = NULL;
            SItem* pItemInsertInto = NULL;
            if (bWordWrap) {
                SLine* pNextLine = getLine(line + 1);
                if (pNextLine && pNextLine->bWordWrapTail) {
                    pItemInsertInto = pNextLine->getItem(0);
                    pNewLine        = pNextLine;
                }
            }

            if (!pNewLine) {
                SLine NewLine;
                NewLine.height =
                    pLine->height;  //g_workspace.getFontManager()->getDefaultFont()->getFontSize();
                UI_ENSURE_B(AddItem(NewLine, line + 1, false));
                pNewLine = getLine(line + 1);
            }

            // 处理当前Item
            if (itemCol == 0) {
                _StartItemIndex = itemIndex;
            } else {
                _StartItemIndex = itemIndex + 1;

                if (itemCol >= (int)pItem->text.size()) {
                    itemCol = (int)pItem->text.size();
                }
                std::string textLeft = pItem->text.substr(itemCol);
                pItem->text          = pItem->text.substr(0, itemCol);
                if (pItemInsertInto) {
                    pItemInsertInto->text.insert(0, textLeft);
                } else {
                    SItem NewItem;
                    NewItem.type       = eit_Text;
                    NewItem.color      = pItem->color;
                    NewItem.bUnderLine = pItem->bUnderLine;
                    NewItem.colorDelta = pItem->colorDelta;
                    NewItem.text       = textLeft;
                    pNewLine->InsertItem(NewItem, 0);
                }
            }
            // 处理后续Item
            for (int i = _StartItemIndex; i < (int)pLine->lineitems.size(); ++i) {
                SItem* _pItem = pLine->getItem(i);
                if (!_pItem)
                    break;
                pNewLine->InsertItem(*_pItem);
            }
            for (int i = _StartItemIndex; i < (int)pLine->lineitems.size(); ++i) {
                pLine->RemoveItem(i);
            }
        } break;
        case eit_Image:
            break;
        default:
            break;
    }

    return true;
}

bool RtwTextBox::_Intercept(int line, int col) {
    int    itemIndex, itemCol;
    SItem *pItemIn, *pItemBetweenLeft, *pItemBetweenRight;
    SLine* pLine = getLine(line);
    UI_ENSURE_B(pLine);
    UI_ENSURE_B(
        _GetItemByLineCol(line, col, &pItemIn, &itemCol, &pItemBetweenLeft, &pItemBetweenRight));
    if (pItemIn) {
        UI_ENSURE_B(pItemIn);
        itemIndex = pLine->getItemIndex(pItemIn);

        std::string textLeft = pItemIn->text.substr(itemCol);
        pItemIn->text        = pItemIn->text.substr(0, itemCol);

        SItem NewItem;
        NewItem.type       = eit_Text;
        NewItem.color      = pItemIn->color;
        NewItem.bUnderLine = pItemIn->bUnderLine;
        NewItem.colorDelta = pItemIn->colorDelta;
        NewItem.text       = textLeft;
        pLine->InsertItem(NewItem, itemIndex + 1);

        CalcLinePosition(line);

        return true;
    } else {
        return true;
    }
}

//处理回车
bool RtwTextBox::_Intercept_Enter(int line, int col) {
    int    itemIndex, itemCol;
    SItem *pItemIn, *pItemBetweenLeft, *pItemBetweenRight;
    SLine* pLine = getLine(line);
    UI_ENSURE_B(pLine);
    UI_ENSURE_B(
        _GetItemByLineCol(line, col, &pItemIn, &itemCol, &pItemBetweenLeft, &pItemBetweenRight));
    if (pItemIn) {
        itemIndex = pLine->getItemIndex(pItemIn);

        std::string textLeft = pItemIn->text.substr(itemCol);
        pItemIn->text        = pItemIn->text.substr(0, itemCol);

        SLine NewLine;
        SItem NewItem;
        NewItem.type       = eit_Text;
        NewItem.color      = pItemIn->color;
        NewItem.bUnderLine = pItemIn->bUnderLine;
        NewItem.colorDelta = pItemIn->colorDelta;
        NewItem.text       = textLeft;
        NewLine.InsertItem(NewItem);
        InsertLine(NewLine, line + 1);

        _MoveTailItemToNextLine(line, itemIndex + 1);

        m_CursorPosLine++;
        m_CursorPosCol = 0;

        CalcLinePosition(line);
        CalcLinePosition(line + 1);

        AutoAdjectCursor();
    } else if (pItemBetweenRight) {
        itemIndex = pLine->getItemIndex(pItemBetweenRight);

        SLine NewLine;
        InsertLine(NewLine, line + 1);
        _MoveTailItemToNextLine(line, itemIndex);

        m_CursorPosLine++;
        m_CursorPosCol = 0;

        CalcLinePosition(line);
        CalcLinePosition(line + 1);

        AutoAdjectCursor();
    } else {
        SLine NewLine;
        InsertLine(NewLine, line + 1);

        m_CursorPosLine++;
        m_CursorPosCol = 0;

        CalcLinePosition(line);
        CalcLinePosition(line + 1);

        AutoAdjectCursor();
    }
    return true;
}

//段字
void RtwTextBox::_DoSwapWordWrap() {
    m_bWordWrap = !m_bWordWrap;
}

//计算item中选择的矩形
void RtwTextBox::_CalcItemSelectionRect(SItem* pItem) {
    int _SelBegin = pItem->SelectBegin - m_ViewOffsetCol;
    int _SelEnd   = pItem->SelectEnd - m_ViewOffsetCol;

    if (_SelBegin >= 0 && _SelEnd <= (int)pItem->text.size()) {
        pItem->rcSelect.left =
            pItem->rect.left + g_workspace.getFontManager()
                                   ->CreateFont(pItem->strFontName, pItem->nFontSize)
                                   ->GetStringWidth(pItem->text, _SelBegin);
        if (_SelEnd >= 0)
            pItem->rcSelect.right =
                pItem->rect.left + g_workspace.getFontManager()
                                       ->CreateFont(pItem->strFontName, pItem->nFontSize)
                                       ->GetStringWidth(pItem->text, _SelEnd);
        else
            pItem->rcSelect.right =
                pItem->rect.left + g_workspace.getFontManager()
                                       ->CreateFont(pItem->strFontName, pItem->nFontSize)
                                       ->GetStringWidth(pItem->text);
        pItem->rcSelect.top    = pItem->rect.top;
        pItem->rcSelect.bottom = pItem->rect.bottom;
    }
}

//得到下一个item
RtwTextBox::SItem* RtwTextBox::getNextItem(SLine* pLine, SItem* pItem) {
    UI_ENSURE_P(pLine && pItem);

    std::list<SItem>::iterator iterItem = pLine->lineitems.begin();
    for (; iterItem != pLine->lineitems.end(); ++iterItem) {
        SItem* _pItem = &(*iterItem);
        if (_pItem == pItem) {
            iterItem++;
            if (iterItem != pLine->lineitems.end())
                return &(*iterItem);
            else
                return NULL;
        }
    }

    return NULL;
}

//设置开始选择的位置;
void RtwTextBox::_SetSelectionStart(int line, int col) {
    m_SelStartLine = line;
    m_SelStartCol  = col;
}

//设置结束的位置
void RtwTextBox::_SetSelectionEnd(int line, int col) {
    m_SelEndLine = line;
    m_SelEndCol  = col;
}

//获取被选中的行,列,起始位置
void RtwTextBox::_getSelectionStartEnd(int& lineStart, int& colStart, int& lineEnd, int& colEnd) {
    if (m_SelEndLine < m_SelStartLine ||
        ((m_SelEndLine == m_SelStartLine) && (m_SelEndCol < m_SelStartCol))) {
        lineStart = m_SelEndLine;
        colStart  = m_SelEndCol;
        lineEnd   = m_SelStartLine;
        colEnd    = m_SelStartCol;
    } else {
        lineStart = m_SelStartLine;
        colStart  = m_SelStartCol;
        lineEnd   = m_SelEndLine;
        colEnd    = m_SelEndCol;
    }
}

//选择所有
void RtwTextBox::SelectAll() {
    if (!m_Lines.empty()) {
        SLine* pLastLine = &(m_Lines.back());
        _SetSelectionStart(0, 0);
        _SetSelectionEnd((int)m_Lines.size() - 1, pLastLine->getColCount());
        RefreshSelectionRect();
    }
}

//删除选择的item
void RtwTextBox::RemoveSelect() {
    if (m_SelStartLine == m_SelEndLine && m_SelStartCol == m_SelEndCol)
        return;

    std::list<int> arrRemoveLines;

    int SelStartLine, SelStartCol, SelEndLine, SelEndCol;
    _getSelectionStartEnd(SelStartLine, SelStartCol, SelEndLine, SelEndCol);

    for (int lineIndex = SelStartLine; lineIndex <= SelEndLine; ++lineIndex) {
        SLine* pLine = getLine(lineIndex);
        if (!pLine)
            continue;

        SItem* pFirstItem = pLine->getItem(0);
        if (!pFirstItem)
            continue;

        int _SelStart = 0;
        int _SelEnd   = (int)pFirstItem->text.size();
        if (lineIndex == SelStartLine) {
            if (SelStartCol >= 0 && SelStartCol <= (int)pFirstItem->text.size())
                _SelStart = SelStartCol;
        }
        if (lineIndex == SelEndLine) {
            if (SelEndCol >= 0 && SelEndCol <= (int)pFirstItem->text.size())
                _SelEnd = SelEndCol;
        }

        if (SelStartCol == 0 && SelEndCol >= (int)pFirstItem->text.size()) {
            pLine->RemoveItem(pFirstItem);
            CalcLinePosition(lineIndex);
        } else {
            pFirstItem->text.erase(_SelStart, _SelEnd - _SelStart);
            CalcLinePosition(lineIndex);
        }

        if (pLine->lineitems.empty()) {
            if (lineIndex > SelStartLine && lineIndex < SelEndLine) {
                arrRemoveLines.push_front(lineIndex);
            }
        }
    }

    // arrRemoveLines必须从大到小排列
    foreach (std::list<int>, iterRemove, arrRemoveLines) {
        int _Index = *iterRemove;
        RemoveLine(_Index);
    }

    _SetSelectionStart(0, 0);
    _SetSelectionEnd(0, 0);
    RefreshSelectionRect();

    m_CursorPosLine = SelStartLine;
    m_CursorPosCol  = SelStartCol;
    RefreshCursorPosition();
}

void RtwTextBox::ScrollToTop() {
    if (m_pScrollBarV) {
        m_pScrollBarV->ScrollToTop();
    }
}

void RtwTextBox::ScrollToBottom() {
    if (m_pScrollBarV) {
        m_pScrollBarV->ScrollToBottom();
    }
}

//获取被选中的文本
std::string RtwTextBox::getSelectText() {
    // @@@@@
    std::string strSelection;

    if (m_SelStartLine == m_SelEndLine && m_SelStartCol == m_SelEndCol)
        return strSelection;

    int SelStartLine, SelStartCol, SelEndLine, SelEndCol;
    _getSelectionStartEnd(SelStartLine, SelStartCol, SelEndLine, SelEndCol);

    SLine* pLine = getLine(0);
    if (!pLine || pLine->lineitems.empty())
        return strSelection;
    SItem* pItem = &(pLine->lineitems.front());

    if (SelStartCol < 0)
        SelStartCol = 0;
    if (SelEndCol > (int)pItem->text.size())
        SelEndCol = (int)pItem->text.size();
    strSelection = pItem->text.substr(SelStartCol, SelEndCol - SelStartCol);

    return strSelection;
}

//删除子控件
void RtwTextBox::OnRemoveChild(RtwWidget* pWidget) {
    if (pWidget == m_pScrollBarV)
        DROP_RTUI_OBJECT(m_pScrollBarV);
}

//是否只能输入文本
void RtwTextBox::SetNumberOnly(bool bNum) {
    m_bNumeric = bNum;
}

//富文本
void RtwTextBox::SetRichText(const std::string& richtext) {
    if (!m_pRichTextLoader->LoadFromString(richtext))
        return;

    for (std::list<SLine>::iterator iterLine = m_pRichTextLoader->m_Lines.begin();
         iterLine != m_pRichTextLoader->m_Lines.end(); iterLine++) {
        SLine& _Line = *iterLine;
        InsertLine(_Line);
    }

    SetCursorPos(0, 0);
}

//设置html文本
void RtwTextBox::SetHtmlText(const std::string& htmltext) {
    // 删除所有行，并重置相关标志
    RemoveAllLines();
    m_CursorPosLine = 0;
    m_CursorPosCol  = 0;
    m_SelStartLine  = 0;
    m_SelStartCol   = 0;
    m_SelEndLine    = 0;
    m_SelEndCol     = 0;

    SetMultiLine(true);
    SetWordWrap(true);

    if (!m_pHtmlTextLoader->LoadFromString(htmltext))
        return;
    for (std::list<SLine>::iterator iterLine = m_pHtmlTextLoader->m_Lines.begin();
         iterLine != m_pHtmlTextLoader->m_Lines.end(); ++iterLine) {
        SLine& _Line = *iterLine;
        InsertLine(_Line);
    }
    SetCursorPos(0, 0);
}

//从文件里加载
void RtwTextBox::LoadFromFile(const char* szFileName) {
    if (!m_pRichTextLoader->LoadFromFile(szFileName))
        return;

    for (std::list<SLine>::iterator iterLine = m_pRichTextLoader->m_Lines.begin();
         iterLine != m_pRichTextLoader->m_Lines.end(); ++iterLine) {
        SLine& _Line = *iterLine;
        InsertLine(_Line);
    }

    SetCursorPos(0, 0);
}

//计算行的位置
void RtwTextBox::CalcLinePosition(int LineIndex, const SPoint& StartPoint) {
    SLine* pLine = getLine(LineIndex);
    UI_ENSURE(pLine);

    RtwRect OrigLineRect = pLine->rect;
    if (pLine->m_type == 1) {
        pLine->rect.left = StartPoint.x + pLine->slleft;
        pLine->rect.top  = StartPoint.y + pLine->sltop;
    } else {
        pLine->rect.left = StartPoint.x;
        pLine->rect.top  = StartPoint.y;
    }
    pLine->rect.right  = pLine->rect.left - 1;
    pLine->rect.bottom = pLine->rect.top - 1;

    if (pLine->lineitems.empty()) {
        pLine->rect.bottom = pLine->rect.top + m_FontSize - 1;
        pLine->rect.right  = pLine->rect.left + 1;
    }

    SPoint CurrPoint(pLine->rect.left + m_ViewOffsetCol, pLine->rect.top);
    pLine->rect.left += m_ViewOffsetCol;
    pLine->rect.right += m_ViewOffsetCol;
    int ItemIndex = -1;

    int imageheight = 0;
    foreach (std::list<SItem>, iterItem, pLine->lineitems) {
        SItem& item = *iterItem;
        if (item.getImage()) {
            // 			if (item.imgSize.width != 31)
            // 			{
            // // 				imageheight = 12;
            // 				break;
            // 			}
            switch (item.type) {
                case eit_Image: {
                    int height = item.getImage() ? item.imgSize.height : 0;
                    if (imageheight < height) {
                        imageheight = height;
                    }
                } break;
            }
        }
    }

    foreach (std::list<SItem>, iterItem, pLine->lineitems) {
        SItem& item = *iterItem;
        ItemIndex++;

        item.rect.left   = CurrPoint.x;
        item.rect.top    = CurrPoint.y;
        item.rect.right  = item.rect.left - 1;
        item.rect.bottom = item.rect.top - 1;

        bool bWrapped = false;  // 是否已经做过自动换行操作了

        switch (item.type) {
            case eit_Image: {
                int width  = item.getImage() ? item.imgSize.width : 0;
                int height = item.getImage() ? item.imgSize.height : 0;

                item.rect.right += width;
                item.rect.bottom += height;
                // 				if (item.rect.right >= m_ViewRect.right)
                // 				{
                // 					if (m_bWordWrap )
                // 					{
                // 						bWrapped = true;
                //
                // 						item.rect.right = item.rect.left - 1;
                // 						item.rect.bottom = item.rect.top - 1;
                //
                // 						_MoveTailItemToNextLine(LineIndex, ItemIndex);
                // 					}
                // 				}
            } break;
            case eit_ImageSequence: {
                int width =
                    item.getImageSequence() ? item.getImageSequence()->getImageSize().width : 0;
                int height =
                    item.getImageSequence() ? item.getImageSequence()->getImageSize().height : 0;

                item.rect.right += width;
                item.rect.bottom += height;
                if (item.rect.right >= m_ViewRect.right) {
                    if (m_bWordWrap) {
                        bWrapped = true;

                        item.rect.right  = item.rect.left - 1;
                        item.rect.bottom = item.rect.top - 1;

                        _MoveTailItemToNextLine(LineIndex, ItemIndex);
                    }
                }
            } break;
            case eit_Name:
            case eit_Text: {
                int width =
                    (int)item.text.length() *
                    (m_FontSize >> 1);  //g_workspace.getFontManager()->GetStringWidth(item.text);
                int height = 0;
                if (imageheight == 0) {
                    height = m_FontSize;  //g_workspace.getFontManager()->getFontSize();
                } else {
                    height = imageheight;
                }

                item.rect.right += width;
                item.rect.bottom += height;

                if (item.rect.right >= m_ViewRect.right) {
                    if (m_bWordWrap) {
                        int      PickIndex;
                        bool     bDoubleByte;
                        bool     bNear;
                        RtwFont* pkFont = NULL;
                        pkFont = g_workspace.getFontManager()->CreateFont(m_FontType, m_FontSize);
                        if (pkFont->PickCharacter(m_ViewRect.right - item.rect.left, PickIndex,
                                                  bDoubleByte, bNear, item.text)) {
                            item.rect.right = m_ViewRect.right - 1;

                            SItem NewItem;
                            _DivideTextItem(item, PickIndex, NewItem);
                            _MoveTailItemToNextLine(LineIndex, ItemIndex + 1, pLine->sltop);
                            SLine* pNextLine = getLine(LineIndex + 1);
                            UI_CHECK(pNextLine);
                            if (!pNextLine->lineitems.empty()) {
                                // 与下一行的第一个Item合并
                                SItem& _ItemFirst = pNextLine->lineitems.front();
                                if (_ItemFirst.type == eit_Text &&
                                    NewItem.color == _ItemFirst.color) {
                                    _MergeNeighborItem(&NewItem, &_ItemFirst);
                                    pNextLine->lineitems.pop_front();
                                }
                            }
                            NewItem.rect.top += pLine->sltop;
                            pNextLine->lineitems.push_front(NewItem);
                        }
                    }
                }
            } break;
            default:
                break;
        }

        CurrPoint.x = item.rect.right + 1;
        CurrPoint.y = item.rect.top;

        if (item.rect.right > pLine->rect.right && item.type != eit_Image) {
            pLine->rect.right = item.rect.right + pLine->slright;
        }
        if (item.rect.bottom > pLine->rect.bottom) {
            pLine->rect.bottom = item.rect.bottom;
        }

        if (bWrapped)
            break;
    }

    // 让行中所有Item都底端对齐

    // 对其方式
    SSize _AlignDelta;
    if (pLine->SlAlign.align == alignCenter && pLine->m_type == 1) {
        int _Width        = pLine->rect.getWidth();
        int _LeftFrom     = m_ViewRect.getCenter().x - (_Width >> 1);
        _AlignDelta.width = _LeftFrom - pLine->rect.left;
    }
    switch (m_TextAlign.align) {
        case alignNear:
            break;
        case alignCenter: {
            int _Width        = pLine->rect.getWidth();
            int _LeftFrom     = m_ViewRect.getCenter().x - (_Width >> 1);
            _AlignDelta.width = _LeftFrom - pLine->rect.left;
        } break;
        case alignFar: {
            _AlignDelta.width = m_ViewRect.right - pLine->rect.right;
        } break;
        default:
            break;
    }
    switch (m_TextAlign.lineAlign) {
        case alignNear:
            break;
        case alignCenter: {
        } break;
        case alignFar:
            break;
        default:
            break;
    }

    if (_AlignDelta.width != 0 || _AlignDelta.height != 0) {
        foreach (std::list<SItem>, iterItem, pLine->lineitems) {
            SItem* pItem = &(*iterItem);
            pItem->rect.Offset(_AlignDelta);
        }
    }

    // 移动后面的所有行
    SSize DeltaOffset(pLine->rect.left - OrigLineRect.left,
                      pLine->rect.bottom - OrigLineRect.bottom);
    OffsetItemPosition(DeltaOffset, LineIndex + 1);

    // 计算后面的Wrap行
    SLine* pNextLine = getLine(LineIndex + 1);
    if (pNextLine && pNextLine->bWordWrapTail) {
        SPoint _Point(pLine->rect.left, pLine->rect.bottom + m_LineSpace);
        CalcLinePosition(LineIndex + 1, _Point);
    }

    // 调整ScrollBarV的数据
    RefreshScrollBarVData();

    // 处理自动调整大小
    // 检查是否需要自动调整尺寸
    if (m_bAutoSizeV || m_bAutoSizeH) {
        // 创建一个矩形来存储自动调整后的区域
        RtwRect rcAuto;
        rcAuto.left   = m_rcFrame.left;  // 初始化左边界
        rcAuto.top    = m_rcFrame.top;   // 初始化上边界
        rcAuto.right  = m_rcFrame.left;  // 初始化右边界（稍后会调整）
        rcAuto.bottom = m_rcFrame.top;   // 初始化下边界（稍后会调整）

        // 遍历所有行以调整矩形大小
        for (const SLine& line : m_Lines) {
            rcAuto.Expand(line.rect);  // 扩展矩形以包含当前行的矩形
        }

        // 根据间隔和边距调整矩形大小
        rcAuto.right += m_LeftSpace + m_LeftMargin;
        rcAuto.bottom += m_LineSpace + m_BottomMargin + 2; //lmk 修正文字下边界 + 2

        // 如果不需要垂直自动调整，则设置矩形的下边界为原始框架的下边界
        if (!m_bAutoSizeV) {
            rcAuto.bottom = m_rcFrame.bottom;
            rcAuto.top    = m_rcFrame.top;
        }

        // 如果不需要水平自动调整，则设置矩形的右边界为原始框架的右边界
        if (!m_bAutoSizeH) {
            rcAuto.right = m_rcFrame.right;
            rcAuto.left  = m_rcFrame.left;
        }

        // 如果调整后的矩形与原始矩形不同，则进行移动操作
        if (m_rcFrame != rcAuto) {
            Move(rcAuto, true);
        }
    }
}

//刷新滚动条数据
void RtwTextBox::RefreshScrollBarVData() {
    if (m_pScrollBarV && m_bScrollBarV) {
        int _OrigValue = m_pScrollBarV->getValue();

        SLine* pFirstLine = getLine(0);
        if (!pFirstLine /* || pFirstLine->lineitems.empty()*/) {
            m_pScrollBarV->SetData(0, 0, 0);
        } else {
            SLine* pLastLine = getLine((int)m_Lines.size() - 1);
            UI_CHECK(pLastLine);
            int LineTotalHeight = 0;
            //for (int i=0; i<(int)m_Lines.size(); ++i)
            //{
            //	if (getLine(i)->height == 0)
            //	{
            //		getLine(i)->height = m_FontSize;
            //	}
            //	LineTotalHeight += getLine(i)->height;
            //}
            LineTotalHeight = pLastLine->getBottom() - getLine(0)->getTop();
            LineTotalHeight += 12;

            if (LineTotalHeight < m_ViewRect.getHeight()) {
                m_pScrollBarV->SetData(0, 0, 0);
            } else {
                int Delta = LineTotalHeight - m_ViewRect.getHeight();
                int _Value =
                    _OrigValue;  //(_OrigValue >= 0 && _OrigValue <= Delta) ? _OrigValue : 0;
                if (_Value > Delta)
                    _Value = Delta;
                else if (_Value < 0)
                    _Value = 0;
                Real _Rate = (Real)m_ViewRect.getHeight() / LineTotalHeight;
                m_pScrollBarV->SetData(0, Delta, _Value, _Rate);
            }
        }
    }
}

//移动item到下一行
bool RtwTextBox::_MoveTailItemToNextLine(int LineIndex, int ItemIndexFrom, int top, int bottom) {
    SLine* pLine = getLine(LineIndex);
    UI_ENSURE_B(pLine);

    SLine* pNextLine = getLine(LineIndex + 1);
    if (!pNextLine || !pNextLine->bWordWrapTail)  // @@@@@ 外层判断bWordWrapTail
    {
        SLine NewLine;
        NewLine.bWordWrapTail = true;
        if (top > 0) {
            NewLine.m_type = 1;
        }
        NewLine.sltop = top;
        InsertLine(NewLine, LineIndex + 1);
        pNextLine = getLine(LineIndex + 1);
    }
    if (pNextLine) {
        // 把余下的Item转移到下一行
        int ItemCount = (int)pLine->lineitems.size();
        for (int iCut = ItemIndexFrom; iCut < ItemCount; ++iCut) {
            SItem& _ItemLast = pLine->lineitems.back();
            if (!pNextLine->lineitems.empty()) {
                // 与下一行的第一个Item合并
                SItem& _ItemFirst = pNextLine->lineitems.front();
                if (_ItemLast.type == eit_Text && _ItemFirst.type == eit_Text &&
                    _ItemLast.color == _ItemFirst.color) {
                    _MergeNeighborItem(&_ItemLast, &_ItemFirst);
                    pNextLine->lineitems.pop_front();
                }
            }
            pNextLine->lineitems.push_front(_ItemLast);
            pLine->lineitems.pop_back();
        }
    }

    return true;
}

//计算行位置
void RtwTextBox::CalcLinePosition(int LineIndex) {
    SLine* pLine = getLine(LineIndex);
    UI_ENSURE(pLine);

    SPoint point(m_ViewRect.left, m_ViewRect.top);
    SLine* pPreLine = getLine(LineIndex - 1);
    if (pPreLine)
        point.y = pPreLine->rect.bottom + m_LineSpace;

    CalcLinePosition(LineIndex, point);
}

void RtwTextBox::RefreshPosition() {
    int    LineIndex = 0;
    SPoint StartPoint(m_ViewRect.left, m_ViewRect.top);
    SLine* pLine = getLine(LineIndex);
    while (pLine) {
        CalcLinePosition(LineIndex, StartPoint);
        pLine = getLine(LineIndex);

        LineIndex++;
        StartPoint.y = pLine->rect.bottom + m_LineSpace;
        pLine        = getLine(LineIndex);
    }

    RefreshCursorPosition();
    RefreshSelectionRect();
}

//移动item
void RtwTextBox::OffsetItemPosition(SSize Delta, int LineFrom /* = 0*/) {
    for (int i = LineFrom; i < (int)m_Lines.size(); ++i) {
        SLine* pLine = getLine(i);
        if (!pLine)
            continue;
        foreach (std::list<SItem>, iterItem, pLine->lineitems) {
            SItem& item = *iterItem;
            item.rect.Offset(Delta);
        }
        pLine->rect.Offset(Delta);
    }

    RefreshCursorPosition();
    RefreshSelectionRect();
}

void RtwTextBox::RefreshCursorPosition() {
    UI_ENSURE(m_bEnableInput);

    SLine* pLine = getLine(m_CursorPosLine);
    if (!pLine) {
        m_CursorRect.left   = m_ViewRect.left;
        m_CursorRect.right  = m_CursorRect.left + 1;
        m_CursorRect.top    = m_ViewRect.top;
        m_CursorRect.bottom = m_CursorRect.top + m_FontSize;
        return;
    } else if (pLine->lineitems.empty()) {
        m_CursorRect.left   = m_ViewRect.left;
        m_CursorRect.right  = m_CursorRect.left + 1;
        m_CursorRect.top    = pLine->rect.top;
        m_CursorRect.bottom = m_CursorRect.top + m_FontSize;
        return;
    }

    int    num         = 0;
    int    textsize    = 0;
    int    linenum     = 0;
    int    oldnum      = 0;
    SItem* pItem       = 0;
    int    Image_Width = 0;
    int    Imagenum    = 0;

    if (g_workspace.GetMouseLDown()) {
        foreach (std::list<SItem>, iterItem, pLine->lineitems) {
            if (linenum == 0) {
                num++;
                SItem& item = *iterItem;
                if (item.type == eit_Text) {
                    textsize += (int)item.text.size();
                    if (textsize >= m_CursorPosCol) {
                        linenum = num;
                    }
                } else if (item.type == eit_Image) {
                    textsize++;
                    Image_Width += 20;
                    Imagenum++;
                    if (textsize >= m_CursorPosCol) {
                        linenum = num;
                    }
                }
            }
        }
        pItem = (pLine->getItem(linenum - 1));
        UI_CHECK(pItem);
    } else {
        foreach (std::list<SItem>, iterItem, pLine->lineitems) {
            if (linenum == 0) {
                num++;
                SItem& item = *iterItem;
                if (item.type == eit_Text) {
                    textsize += (int)item.text.size();
                    if (textsize >= m_CursorPosCol) {
                        linenum = num;
                    }

                } else if (item.type == eit_Image) {
                    textsize++;
                    if (textsize <= m_CursorPosCol) {
                        Image_Width += item.imgSize.width;
                        ;
                        Imagenum++;
                    }
                }
            }
        }
        pItem = (pLine->getItem(linenum - 1));
        UI_CHECK(pItem);
    }
    int _Count = m_CursorPosCol - Imagenum;
    int _Width = _Count * (m_FontSize >> 1);

    m_CursorRect.left   = pLine->rect.left + _Width + Image_Width;
    m_CursorRect.right  = m_CursorRect.left + 1;
    m_CursorRect.top    = pLine->rect.top;
    m_CursorRect.bottom = m_CursorRect.top + m_FontSize;
}

void RtwTextBox::RefreshSelectionRect() {
    int SelStartLine, SelEndLine, SelStartCol, SelEndCol;
    _getSelectionStartEnd(SelStartLine, SelStartCol, SelEndLine, SelEndCol);

    int LineIndex = 0;
    foreach (std::list<SLine>, iterLine, m_Lines) {
        if (LineIndex < SelStartLine || LineIndex > SelEndLine)
            continue;

        SLine* pLine = &(*iterLine);
        SItem* pItem = pLine->getItem(0);
        if (!pItem || (pItem->type != eit_Text && pItem->type != eit_Name))
            continue;

        pItem->SelectBegin = 0;
        pItem->SelectEnd   = pItem->getColCount();
        if (LineIndex == SelStartLine) {
            if (SelStartCol >= 0 && SelStartCol <= pItem->getColCount())
                pItem->SelectBegin = SelStartCol;
            else if (SelStartCol > pItem->getColCount())
                pItem->SelectBegin = pItem->getColCount();
        }
        if (LineIndex == SelEndLine) {
            if (SelEndCol >= 0 && SelEndCol <= pItem->getColCount())
                pItem->SelectEnd = SelEndCol;
            else if (SelEndCol > pItem->getColCount())
                pItem->SelectEnd = pItem->getColCount();
        }

        if (pItem->SelectBegin == pItem->SelectEnd) {
            pItem->rcSelect = RtwRect(0, 0, -1, -1);
        } else {
            int _width1 =
                pItem->SelectBegin *
                (m_FontSize >>
                 1);  //g_workspace.getFontManager()->GetStringWidth(pItem->text, pItem->SelectBegin);
            int _width2 =
                pItem->SelectEnd *
                (m_FontSize >>
                 1);  //g_workspace.getFontManager()->GetStringWidth(pItem->text, pItem->SelectEnd);
            pItem->rcSelect.left   = pItem->rect.left + _width1;
            pItem->rcSelect.right  = pItem->rect.left + _width2;
            pItem->rcSelect.top    = pItem->rect.top;
            pItem->rcSelect.bottom = pItem->rect.bottom;
        }

        LineIndex++;
    }
}

//插入行
bool RtwTextBox::InsertLine(SLine& line, int indexBefore /* = -1*/) {
    //ldr123
    //如果行数大于规定的行数,则删除最后一行
    int nLine = (int)m_Lines.size();
    if (m_nLineNum <= nLine) {
        RemoveLine(0);
        nLine--;
    }
    //end

    if (indexBefore < 0 || indexBefore > nLine) {
        m_Lines.push_back(line);
        CalcLinePosition((int)m_Lines.size() - 1);
    } else {
        std::list<SLine>::iterator iterLine = m_Lines.begin();
        for (int i = 0; i < indexBefore; ++i) {
            iterLine++;
        }
        m_Lines.insert(iterLine, line);
        CalcLinePosition(indexBefore);
    }

    return true;
}

//获取行内item的个数是否为1
bool RtwTextBox::getlineitemssize() {
    if (getLine(m_CursorPosLine)->lineitems.size() == 1) {
        return true;
    }
    return false;
}

void RtwTextBox::InsertImage(RtwImage* image) {
    SItem item;
    item.type = eit_Image;
    item.SetImage(image);
    item.text    = "^-";
    item.text    = item.text + image->m_Name;
    item.imgSize = SSize(m_FontSize, m_FontSize);
    if (!getLine(m_CursorPosLine)) {
        SLine line;
        InsertLine(line, -1);
    }
    getLine(m_CursorPosLine)->InsertItem(item);

    m_addface = true;
    m_CursorPosCol++;
    CalcLinePosition(m_CursorPosLine);
    RefreshCursorPosition();
    AutoAdjectCursor();
}
