
#ifndef uimain_h__
#define uimain_h__

#include "ui/rtw_ui.h"
#include "../preConsole.h"

#define LOAD_UI(name) LoadUI(name)
#define LOAD_UI_T(class_type, name) LoadUI_T<class_type>(name)
RtwWidget* LoadUI(const char* szName);

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
        P_LOGERROR(Temp);  //预览模式下记录错误日志 add by lyy
		RtCoreLog().Error(Temp.c_str());
		CHECKEX(Temp.c_str());
		return NULL;
	}
	return  (T*)vpUI;
}


#endif // uimain_h__
