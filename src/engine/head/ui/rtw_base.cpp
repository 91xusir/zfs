#include "ui/rtw_ui.h"
#include "../../../game_client/preConsole.h"

//********************************************************************
//	created:	2010.04.06 17:00
//	filename: 	rtw_base.cpp.
//	author:		ldr123
//	purpose:	��Ϸuiϵͳ��Ҫ�õ���һЩ������������
//  other:		�ֲ��ع����ע��
//********************************************************************

/* TIPS & Problems
Widget���ָ�ʽΪ "[��������.]�Լ�����" Ŀǰֻ֧�����Ӹ������֣���֧�ּ�����Ҫ�ǿ��Ǽ���������������⣬Ҳ����ȥ�����ӹ�ϵʱ����ı����֡�
������˴����ʹ�������ü���������û���ͷ�ʱwidgetFactory������ҵ����ͷ�û������ʱ��ջ���������ҵ�
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

//�ͷ����ڱ�ʹ�õ����
void RtwRefObject::drop() {
    if (m_bIsDestroying)
        return;

    if (m_RefCounter <= 0)  //���ü����Ѿ�Ϊ0�������ʹ�������ü���
    {

        UI_CHECKEX(("[UI] �����ʹ�������ü���������UiObject�Ѿ����ͷ�" + m_base_name).c_str() );

        P_LOGWARN("�����ʹ�������ü���: "+m_base_name);
            
        return;
    }

    m_RefCounter--;         //���ü����Լ�
    if (m_RefCounter == 0)  //��Ϊ0ʱ,��ʾ����ɾ������ʹ�������������޳��������
    {
        if (IsKindOf(RT_RUNTIME_CLASS(RtwWidget))) {
            g_workspace.getWidgetFactory()->OnDeleteWidget((RtwWidget*)this);  //ɾ���ؼ�
        } else if (IsKindOf(RT_RUNTIME_CLASS(RtwImage)))  //�Ƿ���ͼƬ���ͣ�
        {
            CUiImageRegister* pkImageRegister = IUiImageFactory::UiImageFactory();
            pkImageRegister->unregisterImage(
                (RtwImage*)this);  //���е�ͼƬ������������¼//add by fox
        }
        OnBeforeDestroy();
        RtwRefObject* pThis = this;
        DEL_ONE(pThis);
    }
}

//���ո�ʽȡ��С
bool SSize::FromString(const std::string& str) {
    sscanf(str.c_str(), "%d, %d", &width, &height);
    return true;
}

//��size������ַ���
std::string SSize::ToString() {
    char szBuf[16] = "\0";
    rt2_sprintf(szBuf, "%d, %d", width, height);
    return szBuf;
}

//���ַ����ж�ȡ�����
bool SPoint::FromString(const std::string& str) {
    sscanf(str.c_str(), "%d, %d", &x, &y);

    return true;
}

//�������������ַ���
std::string SPoint::ToString() {
    char szBuf[64] = "\0";
    rt2_sprintf(szBuf, "%d, %d", x, y);
    return szBuf;
}

//�����ṩ�ľ����Ƿ��ڱ������ڲ��򱾾����Ƿ����ṩ�ľ����ڲ�
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

//��С��������Ӧ�����ṩ����
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

//���ô�С
void RtwRect::ReSize(const SSize& size) {
    right = left + size.width - 1;
    bottom = top + size.height - 1;
}

//���ζ���(��������������ζ���,XAlign,YAlignΪ�������뷽ʽ)
void RtwRect::Align(const RtwRect& TargetRect, EAlignment XAlign, EAlignment YAlign) {
    int w = getWidth();
    int h = getHeight();

    switch (XAlign) {
        case alignNear:  //�����
            left = TargetRect.left;
            break;
        case alignCenter:  //���ж���
            //left = ( 2*TargetRect.left + TargetRect.getWidth() - w )/2 ;
            //λ����test
            left = ((TargetRect.left << 1) + TargetRect.getWidth() - w) >> 1;
            break;
        case alignFar:  //���Ҷ���,Դ���ε���߿���Ŀ����ε��ұ�
            left = TargetRect.right - w + 1;
            break;
        default:
            break;
    }

    switch (YAlign) {
        case alignNear:  //������
            top = TargetRect.top;
            break;
        case alignCenter:  //�������
            top = ((TargetRect.top << 1) + TargetRect.getHeight() - h) >> 1;
            break;
        case alignFar:  //�ǵͶ���, Դ���εĶ��˿���Ŀ����εĵ׶�
            top = TargetRect.bottom - h + 1;
            break;
        default:
            break;
    }

    right = left + w - 1;
    bottom = top + h - 1;
}

//�����Լ�����Ӧ���������ľ���
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

//lyymark 1.UI.XML.RtwRect.FromString ��xml�ַ����������
//  תΪ (x,y,width,height)ģʽ ����Ϊ (x1,y1,x2,y2)ģʽ
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

//�����α��浽�ַ���
std::string RtwRect::ToString() {
    char szBuf[64] = "\0";
    rt2_sprintf(szBuf, "%d,%d,%d,%d", left, top, getWidth(), getHeight());
    return szBuf;
}

//���ַ�����ȡ��Եֵ
SMargin::SMargin(const char* str) {
    funguard;
    left = right = top = bottom = 0;  // Ĭ�ϳ�ʼ��Ϊ 0
    if (str && strlen(str) > 0) {
        const int count = sscanf(str, "%d,%d,%d,%d", &left, &top, &right, &bottom);

        // ���ݽ�������ֵ���������е���
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

    //��free�����������ڿͻ�������ʱ������û�취֮��ע��
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

//�����������ַ���
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

//�������ӱ��浽�ַ���
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
