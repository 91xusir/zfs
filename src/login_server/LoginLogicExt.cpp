/////////////////////////////////
//Filename:   LoginLogicExt.h
//Author:     PZH
//CreateTime: 2009-12-23
//Desc:		  一些逻辑功能的扩展，一般是新加的功能
////////////////////////////////

#include "login.h"
#include "LoginLogicExt.h"

int CLoginLogicExt::DeleteOldLogin(unsigned long dwID)
{
	int nRet = 1;
	do
	{
		CLoginServer::TUserMap::iterator iter;
		g_lgs->m_userMap.erase(dwID);
		nRet = 0;
	}while(false);
	return nRet;
}

CLoginLogicExt& CLoginLogicExt::GetCLoginLogicExt()
{
	static CLoginLogicExt s_Logic;
	return s_Logic;
}

