#include "ui/rtw_ui.h"

RT_IMPLEMENT_DYNAMIC(RtwHyperlink, RtwWidget, 0, "ui")

std::string RtwHyperlink::GetURL()
{
	return m_sUrl;
}

bool RtwHyperlink::FromString(const std::string& str)
{
	//static char cTmp1024[1024];
	S_MEMDEF(cTmp1024, 1024)
	S_MEMPROTECTOR(cTmp1024, 1024, bMP)
	char* token = NULL;
	char sep[] = ":";
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
	else 
		Type = hlinkInvalid;
	token = strtok(NULL, sep);
	UI_ENSURE_B(token);
	Text = token;

	return true;
}

std::string RtwHyperlink::ToString()
{
	std::string strOut;
	switch (Type)
	{
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
