/////////////////////////////////
//Filename:   LoginLogicExt.h
//Author:     PZH
//CreateTime: 2009-12-23
//Desc:		  һЩ�߼����ܵ���չ��һ�����¼ӵĹ���
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

