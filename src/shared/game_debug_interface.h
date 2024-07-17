
#ifndef GAME_DEBUG_INTERFACE_H
#define GAME_DEBUG_INTERFACE_H

#include "game_debug.h"

//��CMessenger���ó���������Ϣ���

//��Ҫ�ȼ�1//��ɫ��
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


//��Ҫ�ȼ�3//��ɫ��
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

//��Ҫ�ȼ�5//��ɫ��
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

//��Ҫ�ȼ�7//��ɫ��
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

