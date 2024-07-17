#include "gc_include.h"

RtwWidget* LoadUI(const char* szName)
{
    RtwWidget* vpUI = 0;
	std::string strName = szName;
	if (strName.find("layworld") == std::string::npos)
		strName = "layworld." + strName;
    if (!g_workspace.FindWidget(strName, &vpUI))
	{
		string Temp ("Can't find widget [");
		Temp =Temp + szName + "]!!!!!!!!!!!!!!!!!!!";
		RtCoreLog().Error(Temp.c_str());
		CHECKEX(Temp.c_str());
		return NULL;
	}
    return  vpUI;
}

