#include "core/rt2_core.h"
#include "gc_include.h"
#include "region_client.h"
#include "gc_cursor.h"

#include "../ui/ui_layer.h"


//登录好

//------------------------------------------脚本调用C函数------------------------------------------
//const int STRING_LENGTH=1024;

void Log(char vpMsg[])
{
	char Temp[STRING_LENGTH];
	rt2_sprintf(Temp,"%s[from python]\n",vpMsg);
	LOG(Temp);
	return;
}

//------------------------------------------C调用脚本------------------------------------------

