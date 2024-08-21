#include "ui/rtw_ui.h"
#include "../../../game_client/preConsole.h"

//********************************************************************
//	created:	2010.04.06 17:00
//	filename: 	rtw_base.cpp.
//	author:		ldr123
//	purpose:	游戏ui系统中要用到的一些公共函数和类
//  other:		局部重构添加注释
//********************************************************************

/* TIPS & Problems
Widget名字格式为 "[父亲名字.]自己名字" 目前只支持增加父亲名字，不支持减，主要是考虑减掉会造成重名问题，也就是去处父子关系时不会改变名字。
如果有人错误的使用了引用计数，引用没有释放时widgetFactory会帮助找到，释放没有引用时堆栈出错，很难找到
*/

unsigned long dwCurrentTime = 0;
std::string   ui::g_WidgetTypeShortName[] = {"nav", "unk", "wgt", "lyr", "lab", "btn",
                                             "frm", "edt", "sbv", "sbh", "lst", "pgb",
                                             "cmb", "tab", "chk", "tre", "mnu", "mfm"};

std::string ui::g_WidgetTypeName[] = {
    "NotAva",      "Unknown",  "Widget",       "Layer",      "Label",     "Button",
    "Form",        "EditBox",  "ScrollBarV",   "ScrollBarH", "ListBox",   "ProgressBar",
    "ComboBox",    "TabCtrl",  "CheckButton",  "Tree",       "PopupMenu", "MediaFrame",
    "AliasButton", "HtmlView", "ChatInputBox", "ChatBox",    "Hud",       "D3DView",
    "Count",       "Column"};

RT_IMPLEMENT_DYNAMIC(RtwRefObject, RtObject, 0, "ui")

//释放正在被使用的组件
void RtwRefObject::drop() {
    if (m_bIsDestroying)
        return;

    if (m_RefCounter <= 0)  //引用计数已经为0，错误的使用了引用计数
    {

        UI_CHECKEX(("[UI] 错误的使用了引用计数，或者UiObject已经被释放" + m_base_name).c_str() );

        P_LOGWARN("错误的使用了引用计数: "+m_base_name);
            
        return;
    }

    m_RefCounter--;         //引用计数自减
    if (m_RefCounter == 0)  //当为0时,表示可以删除组件和从组件管理器里剔除掉本组件
    {
        if (IsKindOf(RT_RUNTIME_CLASS(RtwWidget))) {
            g_workspace.getWidgetFactory()->OnDeleteWidget((RtwWidget*)this);  //删除控件
        } else if (IsKindOf(RT_RUNTIME_CLASS(RtwImage)))  //是否是图片类型？
        {
            CUiImageRegister* pkImageRegister = IUiImageFactory::UiImageFactory();
            pkImageRegister->unregisterImage(
                (RtwImage*)this);  //所有的图片都在这里解除记录//add by fox
        }
        OnBeforeDestroy();
        RtwRefObject* pThis = this;
        DEL_ONE(pThis);
    }
}

//按照格式取大小
bool SSize::FromString(const std::string& str) {
    sscanf(str.c_str(), "%d, %d", &width, &height);
    return true;
}

//将size输出到字符串
std::string SSize::ToString() {
    char szBuf[16] = "\0";
    rt2_sprintf(szBuf, "%d, %d", width, height);
    return szBuf;
}

//从字符串中读取坐标点
bool SPoint::FromString(const std::string& str) {
    sscanf(str.c_str(), "%d, %d", &x, &y);

    return true;
}

//将坐标点输出到字符串
std::string SPoint::ToString() {
    char szBuf[64] = "\0";
    rt2_sprintf(szBuf, "%d, %d", x, y);
    return szBuf;
}

//参数提供的矩形是否在本矩形内部或本矩形是否在提供的矩形内部
bool RtwRect::IsIntersect(const RtwRect& rc) const {
    if (rc.left < left && rc.right < left)
        return false;
    if (rc.left > right && rc.right > right)
        return false;
    if (rc.top < top && rc.bottom < top)
        return false;
    if (rc.top > bottom && rc.bottom > bottom)
        return false;

    return true;
}

//缩小本身以适应参数提供矩形
void RtwRect::ClipBy(const RtwRect& rc) {
    if (rc.left > left) {
        left = rc.left;
    }

    if (rc.right < right) {
        right = rc.right;
    }

    if (rc.top > top) {
        top = rc.top;
    }

    if (rc.bottom < bottom) {
        bottom = rc.bottom;
    }
}

//重置大小
void RtwRect::ReSize(const SSize& size) {
    right = left + size.width - 1;
    bottom = top + size.height - 1;
}

//矩形对齐(本矩形与参数矩形对齐,XAlign,YAlign为横竖对齐方式)
void RtwRect::Align(const RtwRect& TargetRect, EAlignment XAlign, EAlignment YAlign) {
    int w = getWidth();
    int h = getHeight();

    switch (XAlign) {
        case alignNear:  //左对齐
            left = TargetRect.left;
            break;
        case alignCenter:  //居中对齐
            //left = ( 2*TargetRect.left + TargetRect.getWidth() - w )/2 ;
            //位运算test
            left = ((TargetRect.left << 1) + TargetRect.getWidth() - w) >> 1;
            break;
        case alignFar:  //非右对齐,源矩形的左边靠着目标矩形的右边
            left = TargetRect.right - w + 1;
            break;
        default:
            break;
    }

    switch (YAlign) {
        case alignNear:  //顶对齐
            top = TargetRect.top;
            break;
        case alignCenter:  //纵向居中
            top = ((TargetRect.top << 1) + TargetRect.getHeight() - h) >> 1;
            break;
        case alignFar:  //非低对齐, 源矩形的顶端靠着目标矩形的底端
            top = TargetRect.bottom - h + 1;
            break;
        default:
            break;
    }

    right = left + w - 1;
    bottom = top + h - 1;
}

//扩大自己以适应参数给出的矩形
void RtwRect::Expand(const RtwRect& rcOther) {
    if (rcOther.left < left) {
        left = rcOther.left;
    }

    if (rcOther.right > right) {
        right = rcOther.right;
    }

    if (rcOther.top < top) {
        top = rcOther.top;
    }

    if (rcOther.bottom > bottom) {
        bottom = rcOther.bottom;
    }
}

//lyymark 1.UI.XML.RtwRect.FromString 从xml字符串抽出矩形
//  转为 (x,y,width,height)模式 这里为 (x1,y1,x2,y2)模式
bool RtwRect::FromString(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    int width, height;
    sscanf(str.c_str(), "%d, %d, %d, %d", &left, &top, &width, &height);

    right = left + width;
    bottom = top + height;
    return true;
}

//将矩形保存到字符串
std::string RtwRect::ToString() {
    char szBuf[64] = "\0";
    rt2_sprintf(szBuf, "%d,%d,%d,%d", left, top, getWidth(), getHeight());
    return szBuf;
}

//从字符串读取边缘值
SMargin::SMargin(const char* str) {
    funguard;
    left = right = top = bottom = 0;  // 默认初始化为 0
    if (str && strlen(str) > 0) {
        const int count = sscanf(str, "%d,%d,%d,%d", &left, &top, &right, &bottom);

        // 根据解析到的值的数量进行调整
        switch (count) {
            case 1:
                right = top = bottom = left;
                break;
            case 2:
                right = left;
                bottom = top;
                break;
            case 3:
                bottom = top;
                break;
            default:
                break;
        }
    }
    /*int* data = &left;
	char* t = strdup(str);
	int at = 0;

	char* token = strtok(t, " ,");

	while(token && at < 4)
	{
		data[at] = atol(token);

		token = strtok(NULL, " ,");
		at++;
	}*/

    //由free改来，这里在客户端启动时崩溃，没办法之下注释
    //DEL_ARRAY(t);

    /*switch(at)
	{
	case 0:	left = right = top = bottom = 0;	break;
	case 1: right = top = bottom = left;		break;
	case 2: right = left; bottom = top;			break;
	case 3: bottom = top;						break;
	default:									break;
	}*/

    fununguard;
}

//解析超链接字符串
bool SHyperLink::FromString(const std::string& str) {
    //static char cTmp1024[1024];
    S_MEMDEF(cTmp1024, 1024)
    S_MEMPROTECTOR(cTmp1024, 1024, bMP)
    char* token = NULL;
    char  sep[] = ":";
    UI_ENSURE_B(str.size() < 1024);

    rt2_strcpy(cTmp1024, str.c_str());
    token = strtok(cTmp1024, sep);
    UI_ENSURE_B(token);
    if (strcmp(token, "File") == 0)
        Type = hlinkFile;
    else if (strcmp(token, "Widget") == 0)
        Type = hlinkWidget;
    else if (strcmp(token, "String") == 0)
        Type = hlinkString;
    else if (strcmp(token, "rs") == 0)
        Type = hlinkRServer;
    else if (strcmp(token, "client") == 0)
        Type = hlinkClient;
    else if ((strncmp(token, "task", 4) == 0) || (strncmp(token, "zhou", 4) == 0) ||
             (strncmp(token, "shang", 5) == 0))
        Type = hlinkNpc;
    else if (strcmp(token, "url") == 0)
        Type = hlinkURL;
    else
        Type = hlinkInvalid;

    if (Type != hlinkNpc) {
        token = strtok(NULL, sep);
    }
    UI_ENSURE_B(token);
    Text = token;

    return true;
}

//将超链接保存到字符串
std::string SHyperLink::ToString() {
    std::string strOut;
    switch (Type) {
        case hlinkFile:
            strOut += "File:";
            break;
        case hlinkWidget:
            strOut += "Widget:";
            break;
        case hlinkString:
            strOut += "String:";
            break;
        default:
            strOut += "Unknown:";
            break;
    }

    strOut += Text;

    return strOut;
}
