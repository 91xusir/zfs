
#ifndef GAME_DEBUG_INTERFACE_H
#define GAME_DEBUG_INTERFACE_H

#include "game_debug.h"

//由CMessenger配置出，各种信息输出

//重要等级1//颜色白
class CDebug:public CMessenger
{
public:
	CDebug(const string& vrFrom="debug",const string& vrMessageTo="debug.txt",const int vOutputType= (/*COutput::CONSOLE |*/ COutput::FILES) )
		:CMessenger(vrMessageTo,vrFrom,vOutputType,1,0xffffff){};

	virtual CMessenger* Clone()
	{
		return RT_NEW CDebug(*this);
	};
};


//重要等级3//颜色白
class CLog:public CMessenger
{
public:
	CLog(const string& vrFrom="log",const string& vrMessageTo="log.txt",const int  vOutputType= (COutput::CONSOLE | COutput::FILES) )
		:CMessenger(vrMessageTo,vrFrom,vOutputType,3,0xffffff){};

	virtual CMessenger* Clone()
	{
		return RT_NEW CLog(*this);
	};
};

//重要等级5//颜色黄
class CWarning:public CMessenger
{
public:
	CWarning(const string& vrFrom="warning",const string& vrMessageTo="warning.txt",const int vOutputType= (COutput::CONSOLE | COutput::FILES) )
		:CMessenger(vrMessageTo,vrFrom,vOutputType,5,0x00ffff){};
	virtual CMessenger* Clone()
	{
		return RT_NEW CWarning(*this);
	};
};

//重要等级7//颜色红
class CErr:public CMessenger
{
public:
	CErr(const string& vrFrom="error",const string& vrMessageTo="error.txt",const int vOutputType= (COutput::CONSOLE | COutput::FILES) )
		:CMessenger(vrMessageTo,vrFrom,vOutputType,7,0xff0000){};
	virtual CMessenger* Clone()
	{
		return RT_NEW CErr(*this);
	};
};

#endif//GAME_DEBUG_INTERFACE_H

