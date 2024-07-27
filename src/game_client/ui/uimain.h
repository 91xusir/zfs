
#ifndef uimain_h__
#define uimain_h__

#include "ui/rtw_ui.h"
#include "../preLog.h"

RtwWidget* LoadUI(const char* szName);

#define RTW_WIDGET(name) LoadUI(name)
#define RTW_WIDGET_T(class_type,name) LoadUI_T<class_type>(name)
#define LOAD_UI(name) LoadUI(name)
#define LOAD_UI_T(class_type, name) LoadUI_T<class_type>(name)

template <class T>
T* LoadUI_T(const char* szName)
{
	RtwWidget* vpUI = nullptr;
	std::string strName = szName;
	if (strName.find("layworld") == std::string::npos)
		strName = "layworld." + strName;
	if (!g_workspace.FindWidget(strName, &vpUI))
	{
		std::string Temp ("Can't find widget [");
		Temp =Temp + szName + "]!!!!!!!!!!!!!!!!!!!";
        P_LOGERROR(Temp);  //Ԥ��ģʽ�¼�¼������־ add by lyy
		RtCoreLog().Error(Temp.c_str());
		CHECKEX(Temp.c_str());
		return NULL;
	}
	return  (T*)vpUI;
}
#endif // uimain_h__
