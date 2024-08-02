#include "gw_server.h"
#include "gw_database.h"

//PZH
#include "LogicExt.h"
//
#include "GW_ActivityManager.h"

GW_BEGIN_CMD(cmd_c2g_login)  // 开始处理 cmd_c2g_login 命令
{
    char* name;        // 存储用户名
    char* pwname;      // 存储加密过的用户名
    char* pubwinKey;   // 存储 PubWin 密钥
    char* pwd;         // 存储 MD5 加密后的密码
    char* glompwd;     // 存储 Glob 密码的指针
    char  code;        // 存储登录代码的变量
    char  cPower = 0;  // 存储是否强制登录的标志，

    // 读取用户名
    if (!packet->ReadString(&name))
        return eInvalidData;  // 读取失败，返回无效数据错误
    if (strlen(name) > 50)
        return eInvalidData;  // 用户名长度超过限制，返回无效数据错误

    // 读取加密过的用户名
    if (!packet->ReadString(&pwname))
        return eInvalidData;  // 读取失败，返回无效数据错误

    // 读取 PubWin 密钥
    if (!packet->ReadString(&pubwinKey))
        return eInvalidData;  // 读取失败，返回无效数据错误

    // 读取 Glob 密码
    if (!packet->ReadString(&glompwd))
        return eInvalidData;  // 读取失败，返回无效数据错误

    // 读取 MD5 加密后的密码
    if (!packet->ReadString(&pwd))
        return eInvalidData;  // 读取失败，返回无效数据错误

    // 读取登录代码
    if (!packet->ReadByte(&code))
        return eInvalidData;  // 读取失败，返回无效数据错误

    GWUnloginClient* uc = g_server.m_curUnloginClient;  // 获取当前未登录客户端对象

    // 读取是否强制登录的标志
    if (!packet->ReadByte(&cPower)) {
        cPower = 0;  // 如果读取失败，默认标志为0
    }
    bool bPowerLogic = 0 != cPower;  // 根据标志决定是否强制登录
    uc->SetPowerLogin(bPowerLogic);  // 设置客户端的强制登录标志

    char  ip[32];  // 存储客户端 IP 地址的缓冲区
    short port;    // 存储客户端端口号的变量
    // 获取客户端的远程地址
    if (!uc->netLink->GetSocket().GetRemoteAddr(ip, &port))
        return eNetwork;  // 获取失败，返回网络错误

    // 检查登录时间间隔，避免频繁登录
    if (rtGetMilliseconds() - uc->lastLoginTime < MIN_LOGIN_INTERVAL)
        return eInvalidData;  // 登录间隔过短，返回无效数据错误

    uc->lastLoginTime = rtGetMilliseconds();  // 更新最后登录时间
    long      ret = 0;                        // 存储登录结果
    const int c_nBuffSize = 64;               // 缓冲区大小
    char      szBuff[c_nBuffSize];            // 存储处理后的用户名
    rt2_strcpy(szBuff, name);                 // 复制用户名到缓冲区
    strlwr(szBuff);                           // 将用户名转换为小写
    uc->SetAccountName(szBuff);               // 设置客户端的账户名

    // 检查账户是否被锁定
    if (CLogicExt::GetCLogicExt().IsLockName(name)) {
        ret = -3;                              // 设置返回值为锁定错误代码
        g_sendCmd->BeginWrite();               // 开始写入响应数据
        g_sendCmd->WriteShort(g2c_login_ret);  // 写入登录返回码
        g_sendCmd->WriteLong(LOGIN_RET_FAILED_ACCOUNT_WRONG5);  // 写入锁定错误代码
        g_sendCmd->WriteShort(2);                               // 写入未知数据
        uc->netLink->SendPacket(g_sendCmd);                     // 发送响应数据
    } else if (!g_server.m_login.LoginAccount(uc->seed, szBuff, pwname, pubwinKey, glompwd, pwd, ip,
                                              bPowerLogic)) {
        ret = -2;                                        // 设置返回值为登录失败代码
        g_sendCmd->BeginWrite();                         // 开始写入响应数据
        g_sendCmd->WriteShort(g2c_login_ret);            // 写入登录返回码
        g_sendCmd->WriteLong(LOGIN_RET_FAILED_UNKNOWN);  // 写入登录失败错误代码
        g_sendCmd->WriteShort(2);                        // 写入未知数据
        uc->netLink->SendPacket(g_sendCmd);              // 发送响应数据
    }

    if (ret != 0) {
        uc->valid = false;  // 登录失败，将客户端标记为无效
    }
}

GW_END_CMD;  // 结束处理 cmd_c2g_login 命令


GW_BEGIN_CMD(cmd_c2g_select_char)
{
	long id;

	if(!packet->ReadLong(&id))
		return eInvalidData;

	GWLoginClient *lc = g_server.m_curLoginClient;
	if(lc->GetState() != LCS_SELECT_CHAR) 
		return eInvalidData;	

	if (lc->actorCnt > GW_MAX_USER_PER_ACCOUNT)
		return eInvalidData;

	GWUser* user;
	user = g_server.m_database.LoadUserData(lc->info.accountId, id);
	if(!user)
    {
		ERR1("user ID %d database read error\n", id);
		return eInvalidData;
    }

	lc->actorData = user;
	lc->m_lastSaveTime = time(NULL);
	user->accountName  = lc->info.accountName;
	user->account      = user->accountName;
	user->accountId    = lc->info.accountId;
	user->huiyuanTime  = lc->info.huiYuanTime;//tim.yang  会员时间
	user->bAdult	   = lc->info.bAdult;	//ac.ma  判断是否成人
	user->isVip		   = lc->info.isVip;
	user->IsOnline	   = true;
	if(g_server.DistributeUser(lc,rand()+rtGetMilliseconds(),true))
	{
        // 发送到客户端
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(g2c_select_char_ret);
		GWRegion& rg = g_server.m_regionTable[lc->m_regionId];
		g_sendCmd->WriteLong(user->userID);
		g_sendCmd->WriteLong(lc->m_switchRegionSeed);
		g_sendCmd->WriteString((char*)rg.szListenClientIP.c_str());
		g_sendCmd->WriteShort(rg.lListenClientPort);
		
		// fix later...
		lc->netLink->SendPacket(g_sendCmd);
		DEL_ONE(lc->netLink);
		//lc->netLink = NULL;
	}
	else
	{
		g_server.KickAccount(lc->info.accountId,0,"select char"); // 如果需要在选人后显示下面的信息，就要注释这条语句
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(g2c_select_char_ret);
        g_sendCmd->WriteLong(0);
        //g_sendCmd->WriteString("服务器暂时不可用");//by fox for string
		g_sendCmd->WriteString((char*)R(MSG_SERVER_LOST_1));
        lc->SendPacket(g_sendCmd);
        return eInvalidData; // 如果需要在选人后显示下面的信息，就要注释这条语句
	}

	g_server.UpdateUserIsOnLine(1,user->userID);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_c2g_create_char)
{
	GWLoginClient *lc = g_server.m_curLoginClient;
	
	// 角色数量检查
	if(lc->actorCnt >= GW_MAX_USER_PER_ACCOUNT)
		return eInvalidData;

	char* name;
	char actorId;
	char factor;
	short headModelID;
	char const* password = "";
	char Sex;
	short headImageID;
	short iAnimal;

    
	if (!packet->ReadString(&name))
		return eInvalidData;

	if (!packet->ReadByte(&actorId))
		return eInvalidData;

	// 0 == 周 	FACTION_ZHOU	
	// 3 == 中立 
	if (!packet->ReadByte(&factor))
		return eInvalidData;

	/*****Tianh 09.12.13 ****添加男女***/
	if (!packet->ReadByte(&Sex))
		return eInvalidData;

	if (!packet->ReadShort(&headModelID))
		return eInvalidData;

	if (!packet->ReadShort(&iAnimal))
		return eInvalidData;

	if (!packet->ReadShort(&headImageID))
		return eInvalidData;
	
	//PZH
	/*if (!packet->ReadString(&password))
		return eInvalidData;*/

	if (actorId == 0)
		actorId = 1;

	char	md5String[33];
	rtMD5StringString((char*)password, md5String);
	md5String[32] = '\0';
	std::string strPass = md5String;

	GWCreateUserResult result;
	GWUser user;
	GWActor* actor = g_server.m_database.LookupActor(actorId);
	if(actor == NULL)
	{
		result = gws_create_user_error;
	}
	else  
	{
		bool error = false;

        if (strlen(name)>16)
        {
			error = true;
			result = gws_create_user_invalid_name;
        }

         // 名字检查
        if (!error)
        {
            if ((lc->info.privileges&EUP_GM_Mask)==0)
            {
				if (   strnicmp(name, "gm",   2)==0
                    || strnicmp(name, "Ｇm",  3)==0
                    || strnicmp(name, "gＭ",  3)==0
                    || strnicmp(name, "ＧＭ", 4)==0)
                {
                    error = true;
                    result = gws_create_user_banned_name;
                }
                
            }
        }
		
		if (!error)
        {
	        if(!NAME_IS_VALID(name))
	        {
		        error = true;
		        result = gws_create_user_banned_name;
	        }
        }

		/*Tianh 09.12.03 取消了初始阵营的设定*/
		// 角色检查
        if (!error)
        {
			/*****周和商的阵营可以不必检查了，因为创建的时候一定中立*****/
		    if(factor == 0)
		    {
			    if(!actor->zhouCreate)
			    {
				    error = true;
				    result = gws_create_user_error;
			    }
		    }else if(factor == 1)
		    {
			    if(!actor->shangCreate)
			    {
				    error = true;
				    result = gws_create_user_error;
			    }
		    }
			/***Tianh*判断是否是中立的阵营****/
			else if (factor == 3)
			{
				if(!actor->neutralCreate)
				{
					error = true;
					result = gws_create_user_error;
				}
			}
			else
		    {
			    error = true;
                result = gws_create_user_error;
		    }
        }

		//// 只能创建同一阵营的角色
        //if (!error)
        //{
		//    for(int i=0; i<lc->actorCnt; i++)
		//    {
		//	    if(lc->users[i].attr.faction != factor)
		//	    {
		//		    error = true;
		//		    result = gws_create_user_faction_error;
		//	    }
		//    }
        //}

		if(!error)
		{
			result = g_server.m_database.CreateUser(lc->info.accountId, name, actorId, headModelID, factor, strPass, &user,Sex,headImageID,iAnimal);
		}

		if (result == gws_create_user_okay)
		{
			g_server.LogUserEvent(PLT_UserOperate, user.userID, lc->info.ip.c_str(), 1, 
				user.name.c_str(), user.attr.Sex, "", user.attr.metier, "");
		}
	}

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(g2c_create_char_ret);
	g_sendCmd->WriteLong(result);
	if (result == gws_create_user_okay)
	{ 
		lc->users[lc->actorCnt].userID = user.userID;
		lc->users[lc->actorCnt].name = user.name;
		lc->users[lc->actorCnt].attr = user.attr;
		lc->users[lc->actorCnt].items = user.items;
		lc->users[lc->actorCnt].headModelID = user.headModelID;
		lc->actorCnt++;
		
		g_sendCmd->WriteLong(user.userID);
		g_sendCmd->WriteString((char*)user.name.c_str());
		g_sendCmd->WriteBinary((char*)&user.attr, sizeof(user.attr));
		g_sendCmd->WriteString((char*)user.items.c_str());
		g_sendCmd->WriteShort(user.headModelID);
		g_sendCmd->WriteShort(user.attr.HeadImageID);
	}
	lc->SendPacket(g_sendCmd);
}
GW_END_CMD

GW_BEGIN_CMD(cmd_c2g_char_password_change)
{
	long	id;
	char*	old;
	char*	New;
	long	result = 0;
	char const*	strTmp = "";
	
	if(!packet->ReadLong(&id))
		return eInvalidData;
	if(!packet->ReadString(&old))
		return eInvalidData;
	if(!packet->ReadString(&New))
		return eInvalidData;

	GWLoginClient *lc = g_server.m_curLoginClient;
	if(lc->GetState() != LCS_SELECT_CHAR) 
		return eInvalidData;	

	if (lc->actorCnt > GW_MAX_USER_PER_ACCOUNT)
		return eInvalidData;

	char	OldMd5String[33];
	char	NewMd5String[33];
	char	NullMd5String[33];

	rtMD5StringString("", NullMd5String);
	NullMd5String[32] = '\0';
	rtMD5StringString((char*)old, OldMd5String);
	OldMd5String[32] = '\0';
	rtMD5StringString((char*)New, NewMd5String);
	NewMd5String[32] = '\0';

	GWDBCommand cmd;
	rt2_sprintf(cmd.command, "SELECT Password, HasPassword FROM user WHERE ID = %d LIMIT 1 ",
			id);

	MYSQL_RES* res;
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		LOG("SQL Excute error\n");
		return eInvalidData;
	}

	int count = mysql_num_rows(res);
	if (count == 0)
	{
		LOG("Cannot find user password info\n");
		return eInvalidData;
	}

	MYSQL_ROW row = mysql_fetch_row(res);

	//PZH
	int nTemp = atoi(row[1]);
	if(0 == nTemp)
	{
		result = 0;//该角色没密码
	}
	else
	{
		if( (row[0] == NULL) || (strcmp("", row[0]) == 0) )
		{
			strTmp = NullMd5String;
		}else
		{
			strTmp = row[0];
		}

		if(strcmp(OldMd5String, strTmp) != 0)
		{
			result = 0;
		}else
		{
			result = 1;

			rt2_sprintf(cmd.command, "UPDATE `user` SET `Password`='%s' WHERE `ID`='%d'",
					NewMd5String, id);

			if (!g_server.m_database.ExecCommand(cmd.command, NULL))
			{
				LOG("Update SQL Excute error\n");
				return eInvalidData;
			}
		}
	}
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(g2c_char_password_change_ret);
	g_sendCmd->WriteLong(result);
	lc->SendPacket(g_sendCmd);
}
GW_END_CMD

GW_BEGIN_CMD(cmd_c2g_char_password_confirm)
{
	long	id;
	char*	password;
	long	result = 0;

	if(!packet->ReadLong(&id))
		return eInvalidData;
	if(!packet->ReadString(&password))
		return eInvalidData;

	GWLoginClient *lc = g_server.m_curLoginClient;
	if(lc->GetState() != LCS_SELECT_CHAR) 
		return eInvalidData;	

	if (lc->actorCnt > GW_MAX_USER_PER_ACCOUNT)
		return eInvalidData;

	char	md5String[33];
	char	NullMd5String[33];

	rtMD5StringString("", NullMd5String);
	NullMd5String[32] = '\0';
	rtMD5StringString((char*)password, md5String);
	md5String[32] = '\0';

	GWDBCommand cmd;
	rt2_sprintf(cmd.command, "SELECT Password, HasPassword FROM user WHERE ID = %d LIMIT 1 ",
		id);

	MYSQL_RES* res;
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		LOG("Select SQL Excute error\n");
		return eInvalidData;
	}

	int count = mysql_num_rows(res);
	if (count == 0)
	{
		LOG("Cannot find user password info\n");
		return eInvalidData;
	}

	MYSQL_ROW row = mysql_fetch_row(res);

	char const*	strTmp = "";

	if( (row[0] == NULL) || (strcmp("", row[0]) == 0) )
	{
		strTmp = NullMd5String;
	}else
	{
		strTmp = row[0];
	}

	//PZH 
	int nTemp = atoi(row[1]);
	if(strcmp(md5String, strTmp) != 0/*PZH*/ && 0 != nTemp)
	{
		result = 0;
	}else
	{
		result = 1;
	}

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(g2c_char_password_confirm_ret);
	g_sendCmd->WriteLong(result);
	lc->SendPacket(g_sendCmd);
}
GW_END_CMD

GW_BEGIN_CMD(cmd_c2g_delete_char)
{
	GWLoginClient *lc = g_server.m_curLoginClient;
	char cReD = 1;
	if(lc->actorCnt == 0)
		return eInvalidData;

	long id;
	if (!packet->ReadLong(&id))
		return eInvalidData;	
	
	int n;
	for (n = 0; n < lc->actorCnt; n++)
		if (lc->users[n].userID == id)
		{
			if(lc->users[n].attr.level >= 30)
			{
				cReD = 0;
			}
			break;
		}

	if (n == lc->actorCnt)
		return eInvalidData;

	short faction = lc->users[n].attr.faction;
	short metier = lc->users[n].attr.level;
	string name = lc->users[n].name;
	short Sex = lc->users[n].attr.Sex;

	if (!g_server.m_database.DeleteUser(id))
		return eInvalidData;

	g_server.DeleteAllMail(id);//Tianh 删除数据库邮件

	//LeiJun 删除角色时一并删除其活动相关信息
	GW_ActivityManager::GetSingle()->DeleteAllInfoByUser(name);

	g_server.LogUserEvent(PLT_UserOperate, id, lc->info.ip.c_str(), 2, name.c_str(), Sex, "", metier, "");

	for (n++; n < lc->actorCnt; n++)
		lc->users[n - 1] = lc->users[n];

	lc->actorCnt--;
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(g2c_delete_char_ret);
	g_sendCmd->WriteLong(0);
	g_sendCmd->WriteLong(id);	
	g_sendCmd->WriteByte(cReD);	
	lc->SendPacket(g_sendCmd);
}
GW_END_CMD

GW_BEGIN_CMD(cmd_c2g_restore_char)
{
	
	/*GWLoginClient *lc = 
	GWSession& se = g_server.m_client.GetSession();

	GWUnloginAccount& account = g_server.m_login.GetUnloginAccount(se.index);

	if (account.state != uasCheckPassed)
		return eInvalidData;	

	if (account.count == 0)
		return eInvalidData;

	long id;

	if (!packet->ReadLong(&id))
		return eInvalidData;	

	if (g_server.m_database.RestoreUser(id))
	{
		for (int n = 0; n < account.count; n++)
		{
			if (account.users[n].userID == id)
			{
				account.users[n].deleteDate.clear();
				break;
			}
		}
	}*/
	GWLoginClient *lc = g_server.m_curLoginClient;
	long id;
	if (!packet->ReadLong(&id))
		return eInvalidData;
	GWDBCommand cmd;
	rt2_sprintf(cmd.command, "SELECT deleted FROM user WHERE ID = %d LIMIT 1 ",
		id);

	MYSQL_RES* res;
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		LOG("Select SQL Excute error\n");
		return eInvalidData;
	}

	int count = mysql_num_rows(res);
	
	char cRet = 0;
	int nTemp;
	do
	{
		if(0 == count)
		{
			break;
		}
		MYSQL_ROW row = mysql_fetch_row(res);
		nTemp = atoi(row[0]);
		if(0 == nTemp)
		{
			break;
		}
		rt2_sprintf(cmd.command, "UPDATE `user` SET deleted = %d WHERE ID = %d",
				0, id);

		if (!g_server.m_database.ExecCommand(cmd.command, NULL))
		{
			break;
		}
		cRet = 1;
	}while(false);

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(g2c_restore_char_ret);
	g_sendCmd->WriteByte(cRet);
	lc->SendPacket(g_sendCmd);
}
GW_END_CMD

GW_BEGIN_CMD(cmd_c2g_query_account)
{
	GWLoginClient *lc = g_server.m_curLoginClient;
	
	if(!g_server.m_database.QueryActorList(*lc))
	{
		g_server.UserQuit(lc);
		return eInvalidData;
	}
    
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(g2c_query_account_ret);
	g_sendCmd->WriteLong(lc->actorCnt);
	for (int n = 0; n < lc->actorCnt; n++)
	{
		GWLoginClient::UserInfo& u = lc->users[n];
		g_sendCmd->WriteLong(u.userID);
		g_sendCmd->WriteString((char*)u.name.c_str());
		g_sendCmd->WriteBinary((char*)&u.attr, sizeof(u.attr));
		g_sendCmd->WriteString((char*)u.items.c_str());
		g_sendCmd->WriteString((char*)u.deleteDate.c_str());
		g_sendCmd->WriteShort(u.headModelID);
		//PZH
		char cTemp;
		if(u.bHasPassword)
		{
			cTemp = 1;
		}
		else
		{
			cTemp = 0;
		}
		g_sendCmd->WriteByte(cTemp);//是否设置密码
		if(u.bDelete)
		{
			cTemp = 1;
		}
		else
		{
			cTemp = 0;
		}
		g_sendCmd->WriteByte(cTemp);//是否冻结状态
	}
	lc->SendPacket(g_sendCmd);
}
GW_END_CMD


GW_BEGIN_CMD(cmd_c2g_char_password_set)
{
	GWLoginClient *lc = g_server.m_curLoginClient;
	long	id;
	char*	Pass;

	if(!packet->ReadLong(&id))
		return eInvalidData;
	if(!packet->ReadString(&Pass))
		return eInvalidData;

	char	NewMd5String[33];
	char	NullMd5String[33];

	char const*	strTmp = "";

	rtMD5StringString("", NullMd5String);
	NullMd5String[32] = '\0';
	
	GWDBCommand cmd;
	rt2_sprintf(cmd.command, "SELECT Password, HasPassword FROM user WHERE ID = %d LIMIT 1 ",
			id);

	MYSQL_RES* res;
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		LOG("SQL Excute error\n");
		return eInvalidData;
	}

	int count = mysql_num_rows(res);
	if (count == 0)
	{
		LOG("Cannot find user password info\n");
		return eInvalidData;
	}

	MYSQL_ROW row = mysql_fetch_row(res);

	if(NULL == Pass ||  (strcmp("", row[0]) == 0))
	{
		strTmp = NullMd5String;
	}
	else
	{
		rtMD5StringString((char*)Pass, NewMd5String);
		NewMd5String[32] = '\0';
		strTmp = NewMd5String;
	}

	//PZH
	int nTemp = atoi(row[1]);
    char cResult = 1;
	if(0 == nTemp)
	{
		
		rt2_sprintf(cmd.command, "UPDATE `user` SET `Password`='%s' ,HasPassword=%d WHERE `ID`='%d'",
				strTmp, 1, id);

		if (!g_server.m_database.ExecCommand(cmd.command, NULL))
		{
			LOG("Update SQL Excute error\n");
			return eInvalidData;
		}
		cResult = 0;
	}
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(g2c_char_password_set_ret);
	g_sendCmd->WriteByte(cResult);
	lc->SendPacket(g_sendCmd);
}
GW_END_CMD


GW_BEGIN_CMD(cmd_c2g_char_password_del)
{
	GWLoginClient *lc = g_server.m_curLoginClient;
	long	id;
	char*	Pass;

	if(!packet->ReadLong(&id))
		return eInvalidData;
	if(!packet->ReadString(&Pass))
		return eInvalidData;

	char	NewMd5String[33];
	char	NullMd5String[33];

	char const*	strTmp = "";

	rtMD5StringString("", NullMd5String);
	NullMd5String[32] = '\0';
	
	GWDBCommand cmd;
	rt2_sprintf(cmd.command, "SELECT Password, HasPassword FROM user WHERE ID = %d LIMIT 1 ",
			id);

	MYSQL_RES* res;
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		LOG("SQL Excute error\n");
		return eInvalidData;
	}

	int count = mysql_num_rows(res);
	if (count == 0)
	{
		LOG("Cannot find user password info\n");
		return eInvalidData;
	}

	MYSQL_ROW row = mysql_fetch_row(res);

	if(NULL == Pass ||  (strcmp("", row[0]) == 0))
	{
		strTmp = NullMd5String;
	}
	else
	{
		rtMD5StringString((char*)Pass, NewMd5String);
		NewMd5String[32] = '\0';
		strTmp = NewMd5String;
	}

	int nTemp = atoi(row[1]);
    char cResult = 1;
	do
	{
		if(0 == nTemp)
		{
			cResult = 1;
			break;
		}
		if(strcmp(strTmp, row[0]) != 0)
		{
			cResult = 2;
			break;
		}
		rt2_sprintf(cmd.command, "UPDATE `user` SET HasPassword=%d WHERE `ID`='%d'", 0, id);

		if (!g_server.m_database.ExecCommand(cmd.command, NULL))
		{
			LOG("Update SQL Excute error\n");
			return eInvalidData;
		}
		cResult = 0;
	}while(false);
	
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(g2c_char_password_del_ret);
	g_sendCmd->WriteByte(cResult);
	lc->SendPacket(g_sendCmd);
}
GW_END_CMD

void GWServer::CreateClientChannel()
{
	m_channelUnlogin.RegisterCommand(c2g_login,cmd_c2g_login,"cmd_c2g_login");

	m_channelLogin.RegisterCommand(c2g_select_char,cmd_c2g_select_char,"cmd_c2g_select_char");
	m_channelLogin.RegisterCommand(c2g_create_char,cmd_c2g_create_char,"cmd_c2g_create_char");
	m_channelLogin.RegisterCommand(c2g_delete_char,cmd_c2g_delete_char,"cmd_c2g_delete_char");
	m_channelLogin.RegisterCommand(c2g_restore_char,cmd_c2g_restore_char,"cmd_c2g_restore_char");
	m_channelLogin.RegisterCommand(c2g_query_account,cmd_c2g_query_account,"cmd_c2g_query_account");
	m_channelLogin.RegisterCommand(c2g_char_password_change,cmd_c2g_char_password_change,"cmd_c2g_char_password_change");
	m_channelLogin.RegisterCommand(c2g_char_password_confirm,cmd_c2g_char_password_confirm,"cmd_c2g_char_password_confirm");
	m_channelLogin.RegisterCommand(c2g_char_password_set, cmd_c2g_char_password_set, "cmd_c2g_char_password_set");
	m_channelLogin.RegisterCommand(c2g_char_password_del, cmd_c2g_char_password_del, "cmd_c2g_char_password_del");

}
