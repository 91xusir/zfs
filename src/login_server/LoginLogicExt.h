/////////////////////////////////
//Filename:   LoginLogicExt.h
//Author:     PZH
//CreateTime: 2009-12-23
//Desc:		  一些逻辑功能的扩展，一般是新加的功能
////////////////////////////////

#ifndef _LOGIN_LOGIC_EXT_H__
#define _LOGIN_LOGIC_EXT_H__

class CLoginLogicExt
{
public:
	int DeleteOldLogin(unsigned long dwID);
	static CLoginLogicExt& GetCLoginLogicExt();
private:

};
#endif//_LOGIN_LOGIC_EXT_H__

