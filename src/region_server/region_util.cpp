#include "region.h"
#include "rs_national_war.h"
#include "PlayerLogic.h"

extern bool g_bClientCommand;

#define CHECK_PRIVILEGES(pm, ps, pyGMOP)    if ((cmdGiver->m_userInfo->privileges&pm)==0 && !pyGMOP && !g_bClientCommand) {return ps;}
const std::string s_ErrString = "Error: Invalid Format!\n";
const std::string s_SuccessString = "Operate Success!\n";

char* GetCheaterCheckerInfo(char* szSendCmd)
{
    rt2_sprintf(szSendCmd, "CheaterChecker: %s Cur:%d(%d), Cnt:%d",
        (g_region->m_bKickCheater?"Runing":"Stoped"),
        g_region->m_cheaterMap.size(),
        g_region->m_iCheaterValve,
        g_region->m_iCheaterCnt);
    return szSendCmd;
}

bool RunScriptFunction(CRegionCreature *cmdGiver, char* szSendCmd, const char seps[])
{
	if(!cmdGiver)
		return false;

	char *pCallName = szSendCmd;
	szSendCmd = strtok( NULL, seps );
	const int iMaxParamCount = 5;
	int iValue[iMaxParamCount] = {0};
	std::vector<std::string> oParams;
	for(int ii=0; ii<iMaxParamCount; ii++)
	{
		if(!szSendCmd)
			break;
		oParams.push_back(szSendCmd);
		szSendCmd = strtok( NULL, seps );
	}

	int iParamCount = (int)oParams.size();
	switch(iParamCount)
	{
	case 0:
		cmdGiver->CallScript(pCallName, false);
		return true;
	case 1:
		iValue[0] = atoi(oParams[0].c_str());
		cmdGiver->CallScript(pCallName, false, iValue[0]);
		return true;
	case 2:
		iValue[0] = atoi(oParams[0].c_str());
		iValue[1] = atoi(oParams[1].c_str());
		cmdGiver->CallScript(pCallName, false, iValue[0], iValue[1]);
		return true;
	case 3:
		iValue[0] = atoi(oParams[0].c_str());
		iValue[1] = atoi(oParams[1].c_str());
		iValue[2] = atoi(oParams[2].c_str());
		cmdGiver->CallScript(pCallName, false, iValue[0], iValue[1], iValue[2]);
		break;
	case 4:
		iValue[0] = atoi(oParams[0].c_str());
		iValue[1] = atoi(oParams[1].c_str());
		iValue[2] = atoi(oParams[2].c_str());
		iValue[3] = atoi(oParams[3].c_str());
		cmdGiver->CallScript(pCallName, false, iValue[0], iValue[1], iValue[2], iValue[3]);
		break;
	default:
		return false;
	}

	return true;
}

// 执行客户端上传的命令字符串, 如果被解释就返回true, 否则返回false
const char* DoClientCommand(CRegionCreature *cmdGiver,const char* szCommand, bool bPyGMOP, long pyGmAccount)
{
    //static char szCmdCopy[512];
    //static char szSendCmd[512];
	S_MEMDEF(szCmdCopy, 512)
	S_MEMDEF(szSendCmd, 512)
	S_MEMPROTECTOR(szCmdCopy, 512, bMP1)
	S_MEMPROTECTOR(szSendCmd, 512, bMP2)

    char seps[]   = " \t";
    char *token;
    DWORD id;
	string username;

	if (bPyGMOP)
	{
		char GmCommandLog[256] = {0};
		CG_CmdPacket& packet = g_region->m_gws.BeginSend();
		packet = g_region->m_gws.BeginSend();
		packet.WriteShort(r2g_log_event);
		packet.WriteByte(PLT_GmCommand);
		packet.WriteLong(cmdGiver->m_userInfo->m_userId);
		packet.WriteString((char*)cmdGiver->m_userInfo->m_userIpAddress.c_str()); // userIP
		packet.WriteString((char*)szCommand);
		rt2_sprintf(GmCommandLog,"通过网站使用，使用的GM为Account=%d。",pyGmAccount);
		packet.WriteString(GmCommandLog);
		g_region->m_gws.EndSend();
	}
    else if ((cmdGiver->m_userInfo->privileges&0x80000000)==0)
    {
	    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	    packet = g_region->m_gws.BeginSend();
	    packet.WriteShort(r2g_log_event);
	    packet.WriteByte(PLT_GmCommand);
	    packet.WriteLong(cmdGiver->m_userInfo->m_userId);
	    packet.WriteString((char*)cmdGiver->m_userInfo->m_userIpAddress.c_str()); // userIP
	    packet.WriteString((char*)szCommand);
		packet.WriteString("客户端直接使用。");
	    g_region->m_gws.EndSend();
    }

    rt2_strncpy(szCmdCopy, szCommand, 511); //szCmdCopy[511] = 0;
    token = strtok( (char*)szCmdCopy, seps );
    if (token)
    {
        if (stricmp(token, "?")==0 || stricmp(token, "help")==0)
        {
        }
		else if (stricmp(token, "script")==0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			if (token)
			{
				return RunScriptFunction(cmdGiver, token, seps) ? "RunScript successful." : "RunScript failed.";
			}
			return "RunScript failed.";
		}
		else if (stricmp(token, "bulletin")==0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			g_region->m_gws.BroadcastBulletin(szCommand + 9);
			return "Send bulletin successful.";
		}
		else if (stricmp(token, "reloadchatbanned")==0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			InitChatBannedString(R(RES_TABLE_CZCHATBANNED));
			return "Reload chat banned string successful.";
		}
		else if (stricmp(token, "lockaccount")==0)
		{
			// Lock account for a given time. Louis.Huang @ Mon 23 Mar 2009
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			if (token)
			{
				char *roleName = token;
				long lockTime = 365 * 24 * 60 *60;	// Default lock for one year.
				token = strtok( NULL, seps );
				if (token)
				{
					long myLockTime = atoi(token);
					if (myLockTime > 0)
						lockTime = myLockTime;
				}
				CRegionUser* pUser = g_region->FindUser(roleName);
				if (!pUser)
					return "User not online now.";
				pUser->LockAccount(true, lockTime);
				rt2_sprintf(szSendCmd, "Lock %s user account for %d seconds!",
						roleName, lockTime);
				return szSendCmd;
			}
			return "wrong format!!\n-lockaccount <rolename> [locktime in seconds]";
		}
		else if (stricmp(token, "npc")==0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			if (token)
			{
                if (stricmp(token, "find")==0)
                {
                    CRegionCreature* pFindUser;
                    std::list<DWORD>::iterator itNpcList;
                    for (itNpcList=g_region->m_serviceNpcList.begin(); itNpcList!=g_region->m_serviceNpcList.end();)
                    {
                        pFindUser = g_region->m_defScene->FindCreature(*itNpcList);
                        if (pFindUser)
                        {
                            rt2_sprintf(szSendCmd, "%d %s[%d] (%d,%d)",
                                pFindUser->m_oId,
                                pFindUser->m_core.Name.c_str(), 
                                pFindUser->m_npcId,
                                pFindUser->m_pos[0], pFindUser->m_pos[1]);

                            cmdGiver->SendSystemMessage(szSendCmd);

                            itNpcList++;
                        }else
                        {
                            itNpcList = g_region->m_serviceNpcList.erase(itNpcList);
                        }
                    }
					return R(MSG_CLIENT_COMMAND_INFO);
                }else if (stricmp(token, "list")==0)
                {
                }
			}
			return R(MSG_CLIENT_COMMAND_NPC_LIST);
		}
		else if (stricmp(token, "processcheck")==0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			if (token==NULL)
			{
				return R(MSG_CLIENT_COMMAND_CHECK_PROCESS);
			}

            rt2_sprintf(szSendCmd, "processcheck %s", token);
			CG_CmdPacket& packet = g_region->m_gws.BeginSend();
			packet.WriteShort(r2g_broadcast_to_all_region);
			packet.WriteString(szSendCmd);
			g_region->m_gws.EndSend();
            return szSendCmd;
            /*
            g_sendCmd->BeginWrite();
	        g_sendCmd->WriteShort(r2c_event);
	        g_sendCmd->WriteLong(g_region->m_time);
	        g_sendCmd->WriteShort(EVENT_CHECK_PROCESS);
	        g_sendCmd->WriteLong(1);
	        g_sendCmd->WriteString(token);

            long lNowTime = (long)time(NULL)+100; // 100秒超时
	        CRegionUser *user;
	        EXT_SPACE::hash_map<ULONG,CRegionUser*>::iterator it;
	        for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
	        {
		        user = (*it).second;
                if (user && user->m_dummy)
                {
                    user->m_dummy->m_lLastCheckProcessTime = lNowTime;
		            user->RecvCmd(g_sendCmd);
                }
	        }
            */
			return R(MSG_CLIENT_COMMAND_GET_INFO);
		}
        else if (stricmp(token, "wgcheck")==0)
        {
			rt2_sprintf(szSendCmd, R(MSG_CLIENT_COMMAND_WG_CHECK), g_region->m_iWgCheckedTotal, g_region->GetCurrentCheckedWgUser());
            return szSendCmd;
        }
		else if (stricmp(token, "follow")==0)
		{
			CHECK_PRIVILEGES(EUP_FastMove, R(MSG_CLIENT_COMMAND_NOT_FAST_MOVE), bPyGMOP);
			token = strtok( NULL, seps );
			if (!token)
			{
				return s_ErrString.c_str();
				//return "格式: follow [UserName]";
			}
			username = token;

			g_region->m_gws.MoveUser(1, cmdGiver->m_userInfo->m_userId, username.c_str());

			return R(MSG_CLIENT_COMMAND_MOVING);
		}
		else if (stricmp(token, "moveto")==0)
		{
			CHECK_PRIVILEGES(EUP_FastMove, R(MSG_CLIENT_COMMAND_NOT_FAST_MOVE), bPyGMOP);
			int x, y;
			token = strtok( NULL, seps );
			if (!token)
			{
				//return "格式: moveto [x] [y]";
				return s_ErrString.c_str();
			}
			x = atoi(token);
			token = strtok(NULL, seps);
			if (!token)
			{
				//return "格式: moveto [x] [y]";
				return s_ErrString.c_str();
			}
			y = atoi(token);

			g_region->m_gws.MoveUserToPosition(cmdGiver->m_userInfo->m_userId, 0, x, y);

			return R(MSG_CLIENT_COMMAND_MOVING);
		}
		else if (stricmp(token, "tele")==0)
		{
			CHECK_PRIVILEGES(EUP_FastMove, R(MSG_CLIENT_COMMAND_NOT_FAST_MOVE), bPyGMOP);
			int dis;
			long lDir[2];
			token = strtok( NULL, seps );
			if (!token)
			{
				return s_ErrString.c_str();
			}
			dis = atoi(token);

			cmdGiver->DoTeleportSkill(dis);

			return R(MSG_CLIENT_COMMAND_MOVING);
		}
		
		else if (stricmp(token, "gmnotice1")==0)
		{
			CHECK_PRIVILEGES(EUP_GM_Mask, R(MSG_CLIENT_NORIGHTGMMSG), bPyGMOP);
			token = strtok( NULL, seps );
			string str = "";
			if (!token)
			{
				return s_ErrString.c_str();
			}
			str = token;
			g_region->m_gws.BroadcastBulletin(str.c_str(),false,0xEE7600);

			return R(MSG_CLIENT_GMMSGSUCCESS);
		}
		else if (stricmp(token, "gmnotice2")==0)
		{
			CHECK_PRIVILEGES(EUP_GM_Mask, R(MSG_CLIENT_NORIGHTGMMSG), bPyGMOP);
			token = strtok( NULL, seps );
			string str = "";
			if (!token)
			{
				return s_ErrString.c_str();
			}
			str = token;
			g_region->m_gws.BroadcastBulletin(str.c_str(),false,0x97FFFF);

			return R(MSG_CLIENT_GMMSGSUCCESS);
		}
		else if (stricmp(token, "gmnotice3")==0)
		{
			CHECK_PRIVILEGES(EUP_GM_Mask, R(MSG_CLIENT_NORIGHTGMMSG), bPyGMOP);
			token = strtok( NULL, seps );
			string str = "";
			if (!token)
			{
				return s_ErrString.c_str();
			}
			str = token;
			g_region->m_gws.BroadcastBulletin(str.c_str(),true);

			return R(MSG_CLIENT_GMMSGSUCCESS);
		}
		//else if (stricmp(token, "linemoveto")==0)
		//{
		//	CHECK_PRIVILEGES(EUP_FastMove, R(MSG_CLIENT_COMMAND_NOT_FAST_MOVE), bPyGMOP);
		//	int line,x, y;
		//	token = strtok( NULL, seps );
		//	if (!token)
		//	{
		//		//return "格式: moveto [x] [y]";
		//		return s_ErrString.c_str();
		//	}
		//	line = atoi(token);
		//	token = strtok(NULL, seps);
		//	if (!token)
		//	{
		//		//return "格式: moveto [x] [y]";
		//		return s_ErrString.c_str();
		//	}
		//	x = atoi(token);
		//	token = strtok(NULL, seps);
		//	if (!token)
		//	{
		//		//return "格式: moveto [x] [y]";
		//		return s_ErrString.c_str();
		//	}
		//	y = atoi(token);

		//	cmdGiver->LineMoveto(line,x,y); //分线传送

		//	return R(MSG_CLIENT_COMMAND_MOVING);
		//}
		else if (stricmp(token, "enterline")==0)
		{
			
			DWORD dwGroupID = 0;
			long pos[3];
			memset(pos, 0, sizeof(pos));

			token = strtok( NULL, seps );
			if (!token)
			{
				return s_ErrString.c_str();
			}

			dwGroupID = atoi(token);

			token = strtok(NULL, seps);
			if (!token)
			{
				return s_ErrString.c_str();
			}
			pos[0] = atoi(token);

			token = strtok(NULL, seps);
			if (!token)
			{
				return s_ErrString.c_str();
			}
			pos[1] = atoi(token);

			CLineSysCmdSend_R2G::Send_r2g_linesys_EnterLineReq(dwGroupID, cmdGiver->m_userInfo->m_userId, pos);

			return R(MSG_LINESYS_COMMAND_ENTERLINE);
		}
		else if (stricmp(token, "leaveline")==0)
		{

			DWORD dwGroupID, dwLineID;
			string strmap; 
			long pos[3];
			memset(pos, 0, sizeof(pos));

			token = strtok( NULL, seps );
			if (!token)
			{
				return s_ErrString.c_str();
			}
			dwGroupID = atoi(token);

			token = strtok( NULL, seps );
			if (!token)
			{
				return s_ErrString.c_str();
			}
			dwLineID = atoi(token);

			token = strtok( NULL, seps );
			if (!token)
			{
				return s_ErrString.c_str();
			}
			strmap = token;

			token = strtok(NULL, seps);
			if (!token)
			{
				return s_ErrString.c_str();
			}
			pos[0] = atoi(token);

			token = strtok(NULL, seps);
			if (!token)
			{
				return s_ErrString.c_str();
			}
			pos[1] = atoi(token);

			CLineSysCmdSend_R2G::Send_r2g_linesys_LeaveLineReq(dwGroupID, dwLineID, cmdGiver->m_userInfo->m_userId, strmap.c_str(), pos);

			return R(MSG_LINESYS_COMMAND_LEAVELINE);
		}
		else if (stricmp(token, "lineinfo")==0)
		{

			DWORD dwGroupID, dwLineID;
			dwGroupID = dwLineID = 0;

			token = strtok( NULL, seps );
			if (!token)
			{
				return s_ErrString.c_str();
			}
			dwGroupID = atoi(token);

			token = strtok( NULL, seps );
			if (!token)
			{
				return s_ErrString.c_str();
			}
			dwLineID = atoi(token);


			CLineSysCmdSend_R2G::Send_r2g_linesys_LineInfoReq(dwGroupID, dwLineID, cmdGiver->m_userInfo->m_userId);

			return R(MSG_LINESYS_COMMAND_LINEINFO);
		}
		else if (stricmp(token, "switchline")==0)
		{

			DWORD dwGroupID, dwNewLineID, dwOldLineID;
			dwGroupID = dwNewLineID = dwOldLineID = 0;

			token = strtok( NULL, seps );
			if (!token)
			{
				return s_ErrString.c_str();
			}
			dwGroupID = atoi(token);

			token = strtok( NULL, seps );
			if (!token)
			{
				return s_ErrString.c_str();
			}
			dwNewLineID = atoi(token);

			token = strtok( NULL, seps );
			if (!token)
			{
				return s_ErrString.c_str();
			}
			dwOldLineID = atoi(token);



			CLineSysCmdSend_R2G::Send_r2g_linesys_SwitchLineReq(dwGroupID, dwNewLineID, dwOldLineID ,cmdGiver->m_userInfo->m_userId);

			return R(MSG_LINESYS_COMMAND_SWITCHLINE);
		}
		else if (stricmp(token, "moveuser")==0)
		{
			CHECK_PRIVILEGES(EUP_MoveUser, R(MSG_CLIENT_COMMAND_NOT_ALLOW_MOVE_USER), bPyGMOP);
			token = strtok( NULL, seps );
			if (!token)
			{
				//return "格式: moveuser [username]";
				return s_ErrString.c_str();
			}
			username = token;

			g_region->m_gws.MoveUser(3, cmdGiver->m_userInfo->m_userId, username.c_str());

			return R(MSG_CLIENT_COMMAND_MOVING);
		}
		else if (stricmp(token, "moveuserto")==0)
		{
			CHECK_PRIVILEGES(EUP_MoveUser, R(MSG_CLIENT_COMMAND_NOT_ALLOW_MOVE_USER), bPyGMOP);

			token = strtok( NULL, seps );
			if (!token)
			{
				//return "格式: moveuserto [username] [x] [y]";
				return s_ErrString.c_str();
			}
			username = token;

			int x, y;
			token = strtok( NULL, seps );
			if (!token)
			{
				//return "格式: moveuserto [username] [x] [y]";
				return s_ErrString.c_str();
			}
			x = atoi(token);
			token = strtok(NULL, seps);
			if (!token)
			{
				//return "格式: moveuserto [username] [x] [y]";
				return s_ErrString.c_str();
			}
			y = atoi(token);

			g_region->m_gws.MoveUserToPosition(cmdGiver->m_userInfo->m_userId, username.c_str(), x, y);

			return R(MSG_CLIENT_COMMAND_MOVING);
		}
		else if (stricmp(token, "movearound")==0)
		{
			CHECK_PRIVILEGES(EUP_MoveUser, R(MSG_CLIENT_COMMAND_NOT_ALLOW_MOVE_AROUND), bPyGMOP);
			token = strtok( NULL, seps );
			if (!token)
			{
				//return "格式: movearound [UserName] [range(40-500,default=120)]";
				return s_ErrString.c_str();
			}
			username = token;
			token = strtok(NULL, seps);
			int nRange = 0;
			if (!token)				nRange = 120;
			else					nRange = atoi(token);
			if (nRange>500 || nRange<40)
				nRange = 120;

			g_region->m_gws.MoveUserAround(cmdGiver->m_userInfo->m_userId, username.c_str(), nRange);

			return R(MSG_CLIENT_COMMAND_MOVING);
		}
		else if (stricmp(token, "kick")==0)
		{
			CHECK_PRIVILEGES(EUP_MoveUser, R(MSG_CLIENT_COMMAND_NOT_ALLOW_KICK_USER), bPyGMOP);
			token = strtok( NULL, seps );
			if (!token)
			{
				//return "格式: tick [UserName]";
				return s_ErrString.c_str();
			}
			username = token;

			CG_CmdPacket& packet = g_region->m_gws.BeginSend();
			packet.WriteShort(r2g_kick);
			packet.WriteLong(cmdGiver->m_userInfo->m_userId);
			packet.WriteString((char*)username.c_str());
			g_region->m_gws.EndSend();

			return R(MSG_CLIENT_COMMAND_KICK_USER);
		}
		else if (stricmp(token, "add")==0)
		{
			token = strtok( NULL, seps );
			if (token)
			{
				if (stricmp(token, "vigour")==0)
				{
					CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
					token = strtok( NULL, seps );
					if(token)
					{
						int vigour = atoi(token);
						cmdGiver->m_core.VigourPoint += vigour;

						return R(MSG_CLIENT_COMMAND_MODIFY_VIGOUR);
					}
					//return "格式: add sp 点数";
					return s_ErrString.c_str();
				}
				if (stricmp(token, "sp")==0)
				{
					CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
					token = strtok( NULL, seps );
					if(token)
					{
						int iSP = atoi(token);
						cmdGiver->AddExp(0, iSP);

						return R(MSG_CLIENT_COMMAND_MODIFY_SP);
					}
					//return "格式: add sp 点数";
					return s_ErrString.c_str();
				}
				else if (stricmp(token, "point")==0)
				{
					CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
					token = strtok( NULL, seps );
					if(token)
					{
						int point = atoi(token);
						cmdGiver->AddPoint(point);
						return R(MSG_CLIENT_COMMAND_ADD_POINT);
					}
					//return "格式: add point 点数";
					return s_ErrString.c_str();
				}
				else if (stricmp(token, "credit")==0)
				{
					CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
					token = strtok( NULL, seps );
					if(token)
					{
						int iSP = atoi(token);
						cmdGiver->AddCredit(iSP);
						return R(MSG_CLIENT_COMMAND_ADD_CREDIT);
					}
					//return "格式: add sp 点数";
					return s_ErrString.c_str();
				}
				else if (stricmp(token, "exploit")==0)
				{
					CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
					token = strtok(NULL, seps);
					if(token)
					{
						int iSP = atoi(token);
						cmdGiver->AddExploit(iSP);
						return R(MSG_CLIENT_COMMAND_ADD_EXPLOIT);
					}
					//return "格式: add sp 点数";
					return s_ErrString.c_str();
				}
				else if (stricmp(token,"enp")==0)
				{
					CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
					token = strtok( NULL, seps );
					if(token)
					{
						int num = atoi(token);
						cmdGiver->m_core.AddEnp(num);
						if ((cmdGiver->m_core.MaxEnP.GetValue()) < 10000)
						{
							(cmdGiver->m_core.MaxEnP.Base) += num;
						}					
						return R(MSG_CLIENT_COMMAND_ADD_EXPLOIT);
					}
					//return "格式: add sp 点数";
					return s_ErrString.c_str();
				}

				/*Tianh GM命令增加灵力值*/
				else if (stricmp(token,"soul")==0)
				{
					CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
					token = strtok( NULL, seps );
					if (token)
					{
						int num = atoi(token);
						cmdGiver->m_core.AddSoul(num);
						return  R(MSG_CLIENT_COMMAND_SOUL);
					}
				}
				else if (stricmp(token, "god")==0)
				{
					CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
					token = strtok( NULL, seps );
					if(token)
					{
						int idx = atoi(token);
						token = strtok( NULL, seps );
						if(token)
						{
							int num = atoi(token);
							g_WarfieldMgr.ConjureHero(cmdGiver->m_core.Faction,num,idx);
							return  R(MSG_CLIENT_COMMAND_CONJURE_HERO);
						}
					}
					return s_ErrString.c_str();
				}
				else if (stricmp(token, "money")==0)
				{
					CHECK_PRIVILEGES(EUP_AddItems, R(MSG_CLIENT_COMMAND_NOT_ALLOW_MONEY), bPyGMOP);
					token = strtok( NULL, seps );
					if(token)
					{
						int count = atoi(token);
						if (count>0)
							cmdGiver->m_pItemContainer->AddMoney(count);
						else 
							cmdGiver->m_pItemContainer->RemoveMoney(abs(count));
						CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(count, ITEM_CONTAINER_PLAYER_COMMON_BAG, ITEM_REASON_NULL);
						cmdGiver->RecvCmd(g_sendCmd);
					}
					return "add money success";
				}
				else if (stricmp(token, "fbitem")==0)
				{
					CHECK_PRIVILEGES(EUP_AddItems, R(MSG_CLIENT_COMMAND_NOT_ALLOW_MONEY), bPyGMOP);
					int kk,itemDif,num;
					token = strtok( NULL, seps );
					if(token)
					{
						id = atoi(token);
						token = strtok(NULL, seps);
						if(token)
						{
							num = atoi(token);
							token = strtok(NULL, seps);
							if(token)
							{
								itemDif = atoi(token);
								for(kk=0; kk<num; kk++)
									cmdGiver->AddDungeonMissionItem(id,itemDif);
							}
						}
					}
					return "add fbitem ok";
				}
				else if (stricmp(token, "pet")==0)
				{
					CHECK_PRIVILEGES(EUP_AddItems, R(MSG_CLIENT_COMMAND_NOT_ALLOW_MONEY), bPyGMOP);
					token = strtok(NULL, seps);
					if (token)
					{
						id = atoi(token);
						if (cmdGiver->AddPetToBag(id))
							return "add pet ok";
						return "add pet error";
					}
					return "add pet error";
				}
				else if (stricmp(token, "item")==0)
				{
					CHECK_PRIVILEGES(EUP_AddItems, R(MSG_CLIENT_COMMAND_NOT_ALLOW_MONEY), bPyGMOP);
					int itemLevel = 0;
					DWORD TimeLimit = 0;
					int count = 1;
					EItemColor itemColor = ItemColor_White;
					EItemBind itemBind = IB_NotBind;
					token = strtok( NULL, seps );
					const char* Name;
					CItemContainerBase *pCon = NULL;

					if(token)
					{
						Name = token;


						if (stricmp(token, "self")==0)
						{
							pCon = cmdGiver->m_pItemContainer;
						}
						else
						{
							CRegionUser *RegionUser = g_region->FindUser(Name);
							if (RegionUser)
							{
								if (RegionUser->m_dummy)
								{
									pCon = RegionUser->m_dummy->m_pItemContainer;
								}
								
								cmdGiver = RegionUser->m_dummy;
							}

						}

						token = strtok(NULL, seps);
						if (token)
						{
							id = atoi(token);

							token = strtok(NULL, seps);
							if (token)
							{
								count = atoi(token);
								token = strtok(NULL, seps);
								if (token)
								{
									itemLevel = atoi(token);
									token = strtok(NULL ,seps);
									if (token)
									{
										if (stricmp(token, "green")==0)
											itemColor = ItemColor_Green;
										else if (stricmp(token, "blue")==0)
											itemColor = ItemColor_Blue;
										else if (stricmp(token, "golden")==0)
											itemColor = ItemColor_Golden;
										else 
											itemColor = ItemColor_White;

										token = strtok(NULL, seps);
										if (token)
										{
											if (stricmp(token, "binded")==0)
												itemBind = IB_Binded;
											else if (stricmp(token, "bind_equip")==0)
												itemBind = IB_BindWhenEquip;
											else if (stricmp(token, "bind_pick")==0)
												itemBind = IB_BindWhenPick;
											else 
												itemBind = IB_NotBind;

											token = strtok(NULL, seps);
											if (token)
											{
												int nMinutes = atoi(token);
												TimeLimit = (DWORD)time(NULL) + nMinutes * 60;
											}
										}
									}
								}
							}

							if (count<=0 || count > 50)
								count = 1;
							if (itemLevel<0 || itemLevel > ITEM_MAX_ITEM_LEVEL)
							{
								itemLevel = 0;
							}

							for (int iii=0; iii<count; iii++)
							{
								SItemID item = g_region->m_pItemManager->CreateItem(id, itemColor, itemLevel, itemBind, 0, 0, 0, 0, TimeLimit);
								if ( ItemID_IsValid(item) )
								{
									if (pCon)
									{

										int page, i, j;
										if ( ItemIsMissionItem(item) )
										{
											if(!pCon->m_MissionBag.AddItem(item, &page, &i, &j))
											{
												rt2_sprintf(szCmdCopy, R(MSG_CLIENT_COMMAND_BIG_FULL));
												return szCmdCopy;
											}
											else
											{
												CItemCmdBuilder_Svr::Build_r2c_bag_additem(item, ITEM_CONTAINER_PLAYER_MISSION_BAG, page, i, j, ITEM_REASON_PICK);
												cmdGiver->RecvCmd(g_sendCmd);
												rt2_sprintf(szCmdCopy, R(MSG_CLIENT_COMMAND_ADD_ITEM), id);
												//										return szCmdCopy;
											}
										}
										else if ( ItemIsPet(item) )
										{
											//if(!pCon->m_PetBag.AddItem(item, &page, &i, &j))
											//{
											//	rt2_sprintf(szCmdCopy, R(MSG_CLIENT_COMMAND_BIG_FULL));
											//	return szCmdCopy;
											//}
											//else
											//{
											//	CItemCmdBuilder_Svr::Build_r2c_bag_additem(item, ITEM_CONTAINER_PLAYER_PET_BAG, page, i, j, ITEM_REASON_PICK);
											//	cmdGiver->RecvCmd(g_sendCmd);
											//	rt2_sprintf(szCmdCopy, R(MSG_CLIENT_COMMAND_ADD_ITEM), id);
											//	//										return szCmdCopy;
											//}
											return "";
										}
										else
										{
											if(!pCon->m_Bag.AddItem(item, &page, &i, &j))
											{
												rt2_sprintf(szCmdCopy, R(MSG_CLIENT_COMMAND_BIG_FULL));
												return szCmdCopy;
											}
											else
											{
												CItemCmdBuilder_Svr::Build_r2c_bag_additem(item, ITEM_CONTAINER_PLAYER_COMMON_BAG, page, i, j, ITEM_REASON_PICK);
												cmdGiver->RecvCmd(g_sendCmd);
												rt2_sprintf(szCmdCopy, R(MSG_CLIENT_COMMAND_ADD_ITEM), id);
												//										return szCmdCopy;
											}
										}
									}

								}
								else
								{
									rt2_sprintf(szCmdCopy, R(MSG_CLIENT_COMMAND_ADD_ITEM_FAIL));
									return szCmdCopy;
								}
							}
							return szCmdCopy;
						}
					}
					//return "格式: add item [ItemTypeID] [count] [level] [green|blue|golden]";
					return s_ErrString.c_str();
				}
				else if (stricmp(token, "skill")==0)
                {
					CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
                    token = strtok( NULL, seps );
                    if (token)
                    {
                        id = atoi(token);
                        SSkill* pSkill = Skill()->FindSkill(id);
                        if (pSkill)
                        {
                            if (cmdGiver->m_Skill.AddSkill(id, 0, true))
                            {
								rt2_sprintf(szCmdCopy, R(MSG_CLIENT_COMMAND_ADD_SKILL), pSkill->szName);
                            }else
                            {
								rt2_sprintf(szCmdCopy, R(MSG_CLIENT_COMMAND_ADD_SKILL_FAIL1), pSkill->szName);
                            }
                        }else
                        {
							rt2_sprintf(szCmdCopy, R(MSG_CLIENT_COMMAND_ADD_SKILL_FAIL2), id);
                        }
                        return szCmdCopy;
                    }
                    //return "格式: add skill [SkillTypeID]";
					return s_ErrString.c_str();
                }else if (stricmp(token, "level")==0)
                {
					CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
					return R(MSG_CLIENT_COMMAND_NOT_SUPPORT_LEVEL);
                }
				else if (stricmp(token, "exp")==0)
                {/*Tianh 09.12.04*GM命令增加经验**/
									
                    CHECK_PRIVILEGES(EUP_ModifyAttrib,R(MSG_CLIENT_COMMAND_NOT_MODIFY) , bPyGMOP);
					const char* Name;
					token = strtok(NULL,seps);
					if(token)
					{
						Name = token;

						if (stricmp(token, "self")==0)
						{

						}
						else
						{
							CRegionUser *RegionUser = g_region->FindUser(Name);
							if (RegionUser)
							{
								cmdGiver = RegionUser->m_dummy;
							}

						}

						token = strtok(NULL,seps);
						if (token)
						{
							rt2_sprintf(g_strStaticBuffer, R(MSG_ADD_EXP), atoi(token));
							cmdGiver->SendBattleInfoMessage(g_strStaticBuffer);
							cmdGiver->AddExp(atoi(token),0);
							return R(MSG_CLIENT_COMMAND_SUPPORT_EXP);
						} 
					}
					else
						return R(MSG_CLIENT_COMMAND_NOT_SUPPORT_EXP);
					
                }
				else if (stricmp(token, "cul")==0)
				{/*Tianh *GM命令增加修为**/

					CHECK_PRIVILEGES(EUP_ModifyAttrib,R(MSG_CLIENT_COMMAND_NOT_MODIFY) , bPyGMOP);

					token = strtok(NULL,seps);
					if (token)
					{
						rt2_sprintf(g_strStaticBuffer, R(MSG_ADD_CUL), atoi(token));
						cmdGiver->SendBattleInfoMessage(g_strStaticBuffer);
						cmdGiver->AddCulExp(atoi(token));
						return R(MSG_CLIENT_COMMAND_SUPPORT_CUL);
					} 
					else
						return R(MSG_CLIENT_COMMAND_NOT_SUPPORT_CUL);

				}
				else if (stricmp(token, "unionscore")==0)
				{
                    CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
					token = strtok( NULL, seps );
					int scoreCount = atoi(token);
					if (token)
					{
						int scoreCount = atoi(token);
						if (scoreCount<-100000 || scoreCount>100000)
							return R(MSG_CLIENT_COMMAND_VALUE_OVERFLOW);
					}

					CG_CmdPacket& packet = g_region->m_gws.BeginSend();
					packet.WriteShort(r2g_union);
					packet.WriteShort(r2g_union_add_score);
					packet.WriteLong(cmdGiver->m_userInfo->m_userId);
					packet.WriteLong(cmdGiver->m_unionID);
					packet.WriteLong(scoreCount);
					g_region->m_gws.EndSend();
					return R(MSG_CLIENT_COMMAND_OPERATING);
				}
            }
            //return "格式: add [item|skill|level|exp|unionscore]";
			return s_ErrString.c_str();
        }
        else if (stricmp(token, "clear")==0)
        {
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
            token = strtok( NULL, seps );
            if (token)
            {
                if (stricmp(token, "skill")==0)
                {
                    cmdGiver->m_Skill.ClearSkill(false);
                    cmdGiver->m_Skill.ClearEffect(true);
					cmdGiver->m_Skill.ClearFightSkill(true);
					return R(MSG_CLIENT_COMMAND_REMOVE_SKILL);
                }else if (stricmp(token, "effect")==0)
                {
                    cmdGiver->m_Skill.ClearEffect(true);
					return R(MSG_CLIENT_COMMAND_REMOVE_EFFECT);
                }else if (stricmp(token, "item")==0)
                {
					int money = cmdGiver->m_pItemContainer->GetMoney();
					cmdGiver->m_pItemContainer->m_Bag.Clear();
					cmdGiver->m_pItemContainer->AddMoney(money);
					CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG, &(cmdGiver->m_pItemContainer->m_Bag));
					cmdGiver->RecvCmd(g_sendCmd);
					return R(MSG_CLIENT_COMMAND_REMOVE_BAG_ITEM);
                }
            }
            //return "格式: clear [item|skill|effect]";
			return s_ErrString.c_str();
        }
		else if (stricmp(token, "ride")==0)
		{
			if(!cmdGiver->IsRidePet()) 
			{
				if(!cmdGiver->m_pet) return "ride error";
				cmdGiver->RidePet();
				// CmdGiver->m_bRidePet = true;

                /*
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_ride_ret);
				g_sendCmd->WriteByte(RIDE_ON);
				g_sendCmd->WriteLong(cmdGiver->m_pet->m_oId);
				cmdGiver->RecvCmd(g_sendCmd);
                */
			}else 
			{
				// CmdGiver->m_bRidePet = false;
				if(!cmdGiver->m_pet) return "ride error";
				cmdGiver->RideOffPet();
                /*
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_ride_ret);
				g_sendCmd->WriteByte(RIDE_OFF);
				g_sendCmd->WriteLong(cmdGiver->m_pet->m_oId);
				cmdGiver->RecvCmd(g_sendCmd);
                */
			}
		}
		else if (stricmp(token, "showid") == 0)
		{
			CHECK_PRIVILEGES(EUP_GM_Mask, "GM显示人物ID", bPyGMOP);
			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(r2c_gm_showid);
			cmdGiver->RecvCmd(g_sendCmd);
		}
		else if (stricmp(token, "spawn_map") == 0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			g_stateSnapshot.BuildSpawnMap("spawn_map.bmp");
		}
		else if (stricmp(token, "terrain_map") == 0)
		{
            CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			g_stateSnapshot.BuildTerrainMap("terrain_map.bmp");
		}
		else if (stricmp(token, "reload_script") == 0)
		{
			CHECK_PRIVILEGES(EUP_ReloadScript, R(MSG_CLIENT_COMMAND_NOT_RELOAD_SCRIPT), bPyGMOP);
			g_factory->ReloadScript();
			return "reload script finish";
		}
		else if (stricmp(token,"set_master_list_max")==0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			int Max =atoi(token);
			g_region->m_MasterList.SetMaxCount(Max);
			return "set master list max ok";
		}
		else if (stricmp(token, "add_teacher") == 0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			int IndexID=atoi(token);
			token = strtok( NULL, seps );
			int ID=atoi(token);
			token = strtok( NULL, seps );
			cmdGiver->m_pFriend->AddTeacher(IndexID,ID,token);
			return "add teacher ok";
		}
		else if (stricmp(token, "remove_teacher") == 0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			int ID=atoi(token);
			cmdGiver->m_pFriend->Remove(SConnection::TEACHER,ID);
			return "remove teacher ok";
		}
		else if (stricmp(token, "add_student") == 0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			int IndexID=atoi(token);
			token = strtok( NULL, seps );
			int ID=atoi(token);
			token = strtok( NULL, seps );
			cmdGiver->m_pFriend->AddStudent(IndexID,ID,token);
			return "add student ok";
		}
		else if (stricmp(token, "remove_student") == 0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			int ID=atoi(token);
			cmdGiver->m_pFriend->Remove(SConnection::STUDENT,ID);
			return "remove student ok";
		}
		else if (stricmp(token, "add_master") == 0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			int ID=atoi(token);
			token = strtok( NULL, seps );
			cmdGiver->m_pFriend->AddMaster(ID,token);
			return "add master ok";
		}
		else if (stricmp(token, "remove_master") == 0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			int ID=atoi(token);
			cmdGiver->m_pFriend->Remove(SConnection::MASTER,ID);
			return "remove master ok";
		}
		else if (stricmp(token, "add_prentice") == 0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			int ID=atoi(token);
			token = strtok( NULL, seps );
			cmdGiver->m_pFriend->AddPrentice(ID,token);
			return "add prentice ok";
		}
		else if (stricmp(token, "remove_prentice") == 0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			int ID=atoi(token);
			cmdGiver->m_pFriend->Remove(SConnection::PRENTICE,ID);
			return "remove prentice ok";
		}
		else if (stricmp(token, "set_card") == 0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			token = strtok( NULL, seps );
			int t=atoi(token);
			cmdGiver->m_cardTime = t;
			return "set_card time ok";
		}
        else if (stricmp(token, "enter_dungeon") == 0)
        {
			if(cmdGiver->m_dwGroupID != 0 || cmdGiver->m_dwLineID != 0)
			{
				return R(MSG_FB_CANNOTENTERWHILEONLINE);
			}
            CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
            token = strtok( NULL, seps );
            if (token)
            {
				int iUserLV = 1;
				if(cmdGiver->m_pTeam)
				{
					if(cmdGiver->m_pTeam->mData.Size() > 0)
					{
						for(int ii=cmdGiver->m_pTeam->mData.Begin();
							ii != cmdGiver->m_pTeam->mData.End();
							ii++)
							iUserLV += (int)cmdGiver->m_pTeam->mData[ii].mLevel;
						iUserLV /= cmdGiver->m_pTeam->mData.Size();
					}
					else iUserLV = cmdGiver->m_core.Lev;
				}
				else iUserLV = cmdGiver->m_core.Lev;
                token = (char*)g_region->m_fbMgr->RequestEnter(cmdGiver->m_userInfo, atoi(token),1,iUserLV,cmdGiver->m_pos[0],cmdGiver->m_pos[1]);
                if (token)
                {
					////Tianh//09.12.1///
					return R(MSG_DUNGEON_NOT_EXIST);

                }else
                {
			        return token;
                }
            }
			return R(MSG_CLIENT_COMMAND_DUNGEON_ID);
        }
        else if (stricmp(token, "leave_dungeon") == 0)
        {
            CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
            g_region->m_fbMgr->RequestLeave(cmdGiver->m_userInfo);
			return R(MSG_CLIENT_COMMAND_LEVEL_DUNGEON);
		}
		else if (stricmp(token, "killme") == 0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			cmdGiver->m_core.SetHp(0);
			cmdGiver->Die(NULL);
			return R(MSG_CLIENT_COMMAND_KILLME);
		}
		//无敌
		else if (stricmp(token, "ivcb") == 0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			cmdGiver->m_IsInvincible = 1;
			(&(cmdGiver->m_Skill))->ImmunoHarmful = 1;
			/*cmdGiver->AddInvincibility();*/
			return R(MSG_CLIENT_COMMAND_IVCB);
		}
		//解除无敌
		else if (stricmp(token, "clearivcb") == 0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			cmdGiver->m_IsInvincible = 0;
			(&(cmdGiver->m_Skill))->ImmunoHarmful = 0;
			return R(MSG_CLIENT_COMMAND_CHIVCB);
		}
		else if (stricmp(token, "enter_fb") == 0)
		{
			if(cmdGiver->m_dwGroupID != 0 || cmdGiver->m_dwLineID != 0)
			{
				return R(MSG_FB_CANNOTENTERWHILEONLINE);
			}
            CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			static int uId = 0;
			int dif=0,fid=0;
			token = strtok( NULL, seps );
			if(token)
			{
				fid = atol(token);
			}
			token = strtok( NULL, seps );
			if(token)
			{
				dif = atol(token);
			}
			LOG1("create fb dif = %d\n",dif);
			g_region->m_fbMgr->CreateDungeon(fid,uId++,dif,cmdGiver->m_core.Lev, cmdGiver->m_pos[0],cmdGiver->m_pos[1]);
			g_region->m_fbMgr->AddUserToDungeon(uId-1,cmdGiver->m_userInfo->m_userId, 1, 0, 0);
			return "enter fb";
		}
		else if (stricmp(token,"movetofb") == 0)
		{
			if(cmdGiver->m_dwGroupID != 0 || cmdGiver->m_dwLineID != 0)
			{
				return R(MSG_FB_CANNOTENTERWHILEONLINE);
			}
			if (OB_TYPE(cmdGiver->m_scene) != OBJECT_TYPE_SCENE_FB)
			{
				CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
				static int uId = 0;
				int dif=0,fid=0;
				LOG1("create fb dif = %d\n",dif);
				g_region->m_fbMgr->CreateDungeon(fid,uId++,dif,cmdGiver->m_core.Lev, cmdGiver->m_pos[0],cmdGiver->m_pos[1]);
				g_region->m_fbMgr->AddUserToDungeon(uId-1,cmdGiver->m_userInfo->m_userId, 1, 0, 0);
			}

			CHECK_PRIVILEGES(EUP_FastMove, R(MSG_CLIENT_COMMAND_NOT_FAST_MOVE), bPyGMOP);
			int x, y;
			token = strtok( NULL, seps );
			if (!token)
			{
				//return "格式: moveto [x] [y]";
				return s_ErrString.c_str();
			}
			x = atoi(token);
			token = strtok(NULL, seps);
			if (!token)
			{
				//return "格式: moveto [x] [y]";
				return s_ErrString.c_str();
			}
			y = atoi(token);

			g_region->m_gws.MoveUserToPosition(cmdGiver->m_userInfo->m_userId, 0, x, y);

			return R(MSG_CLIENT_COMMAND_MOVING);
		}
		else if (stricmp(token,"setrage") == 0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib,R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
			token = strtok(NULL,seps);
			int lev = atoi(token);
			if(lev>3 || lev < 0)
				return "rage level is [1:3]";
			cmdGiver->SetRageLevel(atoi(token));
// 			CG_CmdPacket* packet = RT_NEW CG_CmdPacket;
 			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(r2c_killmonster_addrige);
			g_sendCmd->WriteLong(cmdGiver->m_iRigelevel);
			g_sendCmd->WriteLong(cmdGiver->m_iKillMonsterNum);
			cmdGiver->RecvCmd(g_sendCmd);
			return "set rage level finish";
		}
		else if (stricmp(token, "leave_fb") == 0)
		{
            CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			CRegionSceneFB *pFB = dynamic_cast<CRegionSceneFB*>(cmdGiver->m_scene);
			if(pFB)
			{
				SFuBen *pDungeonInfo = g_TableFB.Find(cmdGiver->m_dungeonTID);
				long pos[3];
				pos[0] = pFB->m_lExitPos[0];//pDungeonInfo->exitPos[0]; 
				pos[1] = pFB->m_lExitPos[1];//pDungeonInfo->exitPos[1];
				pos[2] = pDungeonInfo->exitPos[2];
				cmdGiver->m_scene->RemoveCreature(cmdGiver->m_oId);
				g_region->m_defScene->AddCreature(cmdGiver,pos,0);
				cmdGiver->ForceSetPosition(pos[0],pos[1],0);
			}
			return "leave fb";
		}
		else if (stricmp(token, "setlev") == 0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
			token = strtok(NULL,seps);
			const char* Name;
			if(token)
			{
				Name = token;

				if (stricmp(token, "self")==0)
				{

				}
				else
				{
					CRegionUser *RegionUser = g_region->FindUser(Name);
					if (RegionUser)
					{
						cmdGiver = RegionUser->m_dummy;
					}

				}
				token = strtok( NULL, seps );
				if(token)
				{
					int lev = atoi(token);
					cmdGiver->SetLev(lev);
					//cmdGiver->m_core.Lev = lev;

					// 				int setexp = 0;
					// 				if(lev<0 || lev>MAX_LEVEL)
					// 					return "level must in [1,99]";
					// 				if(lev >= cmdGiver->m_core.Lev)
					// 				{
					// 					lev = lev - cmdGiver->m_core.Lev;
					// 					for(int i=0; i<lev; ++lev)
					// 					{
					// 						setexp = g_TableExp.GetNextLevExp(cmdGiver->m_core.Lev + 1);
					// 						cmdGiver->AddExp(setexp,0);
					// 					}
					// 				}
					// 				else
					// 				{
					// 					lev = cmdGiver->m_core.Lev - lev;
					// 					for(int i=0; i<lev; ++i)
					// 					{
					// 						setexp = g_TableExp.GetNextLevExp(cmdGiver->m_core.Lev - 1);
					// 						cmdGiver->AddExp(-setexp,0);
					// 					}
					// 				}
				}
			}
			return "set level finish";
		}
		else if (stricmp(token, "levelup")==0 || stricmp(token, "updatelev")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
			token = strtok( NULL, seps );
			if(token)
			{
				int expAdd = 0;
				int count = atoi(token);
				if (count<0 || count>MAX_LEVEL)
					return "level must in[1,99]";
				if (count+cmdGiver->m_core.Lev > MAX_LEVEL)
					count = MAX_LEVEL - cmdGiver->m_core.Lev;
				for (int i=0; i<count; i++)
				{
					expAdd = g_TableExp.GetNextLevExp(cmdGiver->m_core.Lev + 1);
					cmdGiver->AddExp(expAdd, 0);
				}
				// expAdd -= cmdGiver->m_core.Exp - 1;
				return "update level success";
			}
			else
			{
				int expAdd = g_TableExp.GetNextLevExp(cmdGiver->m_core.Lev) - cmdGiver->m_core.Exp + 1;
				if(cmdGiver->m_core.Lev > MAX_LEVEL)	return "reach max level";
				cmdGiver->AddExp(expAdd, 0);
				return "update level success";
			}
		}
		else if (stricmp(token, "unionlevelup")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
			CG_CmdPacket& packet = g_region->m_gws.BeginSend();
			packet.WriteShort(r2g_union);
			packet.WriteShort(c2r_union_update_level);
			packet.WriteLong(cmdGiver->m_userInfo->m_userId);
			g_region->m_gws.EndSend();
			return R(MSG_CLIENT_COMMAND_OPERATING);
		}
		else if(stricmp(token, "info")==0)
		{
			rt2_sprintf(szCmdCopy, "user = %d,object = %d, pk_Increase = %d",g_region->m_userMap.size(),g_factory->GetObjectNum(),g_pkConfig->iStateChangeTimeIncrease);
			return szCmdCopy;
		}
		else if(stricmp(token, "set")==0)
		{
            CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
			token = strtok( NULL, seps );
			if(token)
			{
                int iCmdId;
				if(stricmp(token, "exp_rate")==0)
				{
					iCmdId = 1;
				}else if(stricmp(token, "gold_rate")==0)
				{
					iCmdId = 2;
				}else if(stricmp(token, "next_enter")==0)
				{
					iCmdId = 3;
				}else
                {
                    iCmdId = 0;
                }

			    char* szValue = strtok( NULL, seps );
			    if(iCmdId!=0 && szValue)
                {
                    float fValue = (float)atof(szValue);
                    rt2_sprintf(szSendCmd, "set %s %f", token, fValue);
			        CG_CmdPacket& packet = g_region->m_gws.BeginSend();
			        packet.WriteShort(r2g_broadcast_to_region);
			        packet.WriteString(szSendCmd);
			        g_region->m_gws.EndSend();
                    return szSendCmd;
                }
            }
			return R(MSG_CLIENT_COMMAND_SET_OPTION);
		}
		else if(stricmp(token, "mlev")==0)
		{ 
				CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
			    cmdGiver->m_core.MetierLev++;
			    cmdGiver->m_core.MetierLev %= 4;
				return "mlev ok";
		}
		else if (stricmp(token, "crash")==0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			// kill the server...
			int *p = NULL;
			*p = 1;
		}
//-------------add start by tony 05.06.13------------------------------------------//
		else if (stricmp(token, "startwar")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);

            std::string	feod_name;	

			token = strtok( NULL, seps );
			if(token)
			{
				if(stricmp(token, "xiqi")==0)
				{
					feod_name = "XiQi";					
				}else if(stricmp(token, "zhaoge")==0)
				{
					feod_name = "ZhaoGe";
				}else if(stricmp(token, "chuanyun")==0)
				{
					feod_name = "ChuanYun";
				}else
				{
					//
				}

				token = strtok( NULL, seps );
				if(token)
				{
					int interval;

					if(stricmp(token, "-")==0)
					{
						token = strtok( NULL, seps );

						if(token)
						{
							interval = atoi(token);

							interval = -interval;
						}
					}else
					{
						interval = atoi(token);
					}

					STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(feod_name);

					if(p_war_info != NULL)
					{
						p_war_info->start_time = g_RegionCastleWarMgr.GetCurrTime() + interval * 60;
						p_war_info->end_time = p_war_info->start_time + g_RegionCastleWarMgr.GetDurationTime(0);

						//if(p_war_info->end_time <= g_RegionCastleWarMgr.GetCurrTime())
						//{
						//	//g_RegionCastleWarMgr.EndWar(feod_name, p_war_info->master_union_id);

						//	p_war_info->state = WARFARE_STATE;

						//}
						//
						//if(p_war_info->start_time < g_RegionCastleWarMgr.GetCurrTime() + 5 * 60)
						//{
						//	p_war_info->state = WAR_PREPARE_STATE;
						//}
						
						return "set start time finish";
					}
				}			
			}
		}
		else if(stricmp(token, "resetwar") == 0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);

			std::string	feod_name;	

			token = strtok( NULL, seps );
			if(token)
			{
				if(stricmp(token, "xiqi")==0)
				{
					feod_name = "XiQi";					
				}else if(stricmp(token, "zhaoge")==0)
				{
					feod_name = "ZhaoGe";
				}else if(stricmp(token, "chuanyun")==0)
				{
					feod_name = "ChuanYun";
				}else
				{
					//
				}

				STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(feod_name);

				if(p_war_info)
				{
					g_RegionCastleWarMgr.EndWar(feod_name, 0);
				}

				return "reset ok";
			}
		}
		else if (stricmp(token, "showwarinfo") == 0)//tim.yang  显示城战信息
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);

			std::string	feod_name;	

			token = strtok( NULL, seps );
			if(token)
			{
				if(stricmp(token, "xiqi")==0)
				{
					feod_name = "XiQi";					
				}else if(stricmp(token, "zhaoge")==0)
				{
					feod_name = "ZhaoGe";
				}else if(stricmp(token, "chuanyun")==0)
				{
					feod_name = "ChuanYun";
				}else
				{
					//
				}

				STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(feod_name);
				
				if(p_war_info)
				{
					//std::string str_pos = "";
					//str_pos += "采邑名字:" + p_war_info->feod_name;
					//str_pos += "\n所属诸侯国id: " + std::string(rtFormatNumber(p_war_info->master_union_id));
					//str_pos += "\n所属诸侯国名字: " + p_war_info->master_union_name;
					//str_pos += "\n所属诸侯国标志: " + p_war_info->master_union_logo;
					//str_pos += "\n诸侯国诸侯名字: " + p_war_info->vassal_name;
					//str_pos += "\n采邑目前状态: " + std::string(rtFormatNumber(p_war_info->state));
					//struct tm *start_time = localtime((time_t*)&p_war_info->start_time);
					//struct tm *end_time = localtime((time_t*)&p_war_info->end_time);

					//if (p_war_info->last_end_time != 0)
					//{
					//	struct tm *last_end_time = localtime((time_t*)&p_war_info->last_end_time);
					//}
					RtLocateTime start_time(p_war_info->start_time);
					RtLocateTime end_time(p_war_info->end_time);
					RtLocateTime last_end_time(p_war_info->last_end_time);
					//struct tm *last_end_time = localtime((time_t*)&p_war_info->last_end_time);
					//str_pos += "\n下次城战开始时间: " + std::string(rtFormatNumber(p_war_info->start_time));
					//str_pos += "\n下次城战结束时间: " + std::string(rtFormatNumber(p_war_info->end_time));
					//str_pos += "\n上次城战结束时间: " + std::string(rtFormatNumber(p_war_info->last_end_time));
					//str_pos += "\n";

					// HACK,HACK,HACK thread-safe?
					//static char buf[1024];
					S_MEMDEF(buf, 1024)
					S_MEMPROTECTOR(buf, 1024, bMP)
					memset(buf,0,sizeof(buf));
					//rt2_sprintf(buf,"采邑名字:%s",(p_war_info->feod_name).c_str());

					rt2_sprintf(buf,"采邑名字:%s \n所属诸侯国名字:%s \n所属诸侯国标志:%s \n诸侯国诸侯名字:%s \
						\n采邑目前状态:%d \n下次城战开始时间:%d月%d日 星期%d %d时%d分 \n下次城战结束时间:%d月%d日 星期%d %d时%d分 \n上次城战结束时间:%d月%d日 星期%d %d时%d分\n",
						p_war_info->feod_name.c_str(),
						p_war_info->master_union_name.c_str(),
						p_war_info->master_union_logo.c_str(),
						p_war_info->vassal_name.c_str(),
						p_war_info->state,
						start_time.iMonth,
						start_time.iDay,
						start_time.iDayOfWeek,
						start_time.iHour,
						start_time.iMin,
						end_time.iMonth,
						end_time.iDay,
						end_time.iDayOfWeek,
						end_time.iHour,
						end_time.iMin,
						last_end_time.iMonth,
						last_end_time.iDay,
						last_end_time.iDayOfWeek,
						last_end_time.iHour,
						last_end_time.iMin);
					return buf;
				}
				else
				{
					return "error";
				}
			}
		}
		else if (stricmp(token, "addnpc")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);

			FILE*	fp = NULL;

			if((fp = fopen("sv_table/guarder_temp.txt", "a")) == NULL)
			{
				return "error";
			}

			std::string str_pos = "";

			str_pos = std::string(rtFormatNumber(cmdGiver->m_pos[0]));
			str_pos += ";";
			str_pos += std::string(rtFormatNumber(cmdGiver->m_pos[1]));
			str_pos += "	";
			str_pos += std::string(rtFormatNumber((int)cmdGiver->m_dir));
			str_pos += "\n";

			fputs((char*)str_pos.c_str(), fp);

			fclose(fp);

			return "add npc pos finish";
		}
		else if (stricmp(token, "startwarfield")==0)
		{
			CHECK_PRIVILEGES(EUP_FastMove, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			// start warfield
			CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
			cmd.WriteShort(r2g_warfield_open);
			g_region->m_gws.EndSend();

			return R(MSG_CLIENT_COMMAND_WAIT_WAR_FIELD_OPEN);
		}
		else if (stricmp(token, "stopwarfield")==0)
		{
			CHECK_PRIVILEGES(EUP_FastMove, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			// stop warfield
			CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
			cmd.WriteShort(r2g_warfield_stop);
			g_region->m_gws.EndSend();
			
			return R(MSG_CLIENT_COMMAND_WAIT_WAR_FIELD_CLOSE);
		}
		else if(stricmp(token, "reloadwar")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);

			std::string	feod_name;	

			token = strtok( NULL, seps );
			if(token)
			{
				if(stricmp(token, "xiqi")==0)
				{
					feod_name = "XiQi";					
				}else if(stricmp(token, "zhaoge")==0)
				{
					feod_name = "ZhaoGe";
				}else if(stricmp(token, "chuanyun")==0)
				{
					feod_name = "ChuanYun";
				}else
				{
					//
				}

				STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByName(feod_name);

				if(p_war_info)
				{
					CCastleWarCmdSend_R2G::Send_r2g_castlewar_reload_time(feod_name);
				}

				return "reload ok";
			}
		}
		else if(stricmp(token, "createnpc")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);

			int npc_id = 0;
			std::string npc_name = "";
			static int Default_Faction = 0;
			static int Default_unitedPoint = 1;
			static int Default_loyalPoint = 1;
			static int Default_bravePoint = 1;
			static int Default_angryPointLevel1 = 30;
			static int Default_angryPointLevel2 = 70;

			token = strtok( NULL, seps );
			if(token)
			{
				npc_id = atol(token);
			}
			else 
			{
				return s_ErrString.c_str();
			}
			//token = strtok( NULL, seps);
			//if (token)
			//{
			//	npc_name = token;
			//}
			//else 
			//{
			//	return s_ErrString.c_str();
			//}

			CRegionCreature *cre = g_region->CreateNpc(npc_id);
			if(!cre/* || (cre->m_core.Name!=npc_name)*/) 
			{
				return s_ErrString.c_str();
			}

			cre->m_faction = Default_Faction;
			cre->m_unitedPoint = Default_unitedPoint;
			cre->m_loyalPoint = Default_loyalPoint;
			cre->m_bravePoint = Default_bravePoint;
			cre->m_angryPointLevel1 = Default_angryPointLevel1;
			cre->m_angryPointLevel2 = Default_angryPointLevel2;

			if(cmdGiver->m_scene->AddCreature(cre, cmdGiver->m_pos, cmdGiver->m_dir))
			{
				if(cre->IsScriptLoaded())
					cre->CallScript("OnCreate", false);
			}
			else
			{
				g_factory->DestroyObject(cre);
				cre = NULL;
				return "";
			}
			return "create npc ok";
		}
		else if(stricmp(token, "addspirit")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);

			int npc_id = 0;
			std::string npc_name = "";

			token = strtok( NULL, seps );
			if(token)
			{
				npc_id = atol(token);
			}
			else 
			{
				return s_ErrString.c_str();
			}

			CRegionCreature *cre = g_region->CreateSpirit(npc_id);
			if(!cre) 
			{
				return s_ErrString.c_str();
			}

			cre->m_faction = 0;
			cre->m_unitedPoint = 1;
			cre->m_loyalPoint = 1;
			cre->m_bravePoint = 1;
			cre->m_angryPointLevel1 = 30;
			cre->m_angryPointLevel2 = 70;

			if(cmdGiver->m_scene->AddCreature(cre, cmdGiver->m_pos, cmdGiver->m_dir))
			{

			}
			else
			{
				g_factory->DestroyObject(cre);
				cre = NULL;
				return "";
			}
			cre->SetTimer(CRE_TIMER_KILL_SELF, 30000);
			return "create spirit ok";
		}
		else if(stricmp(token, "petskill")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);

			int id = 0;
			std::string npc_name = "";

			token = strtok( NULL, seps );
			if(token)
			{
				id = atol(token);
			}
			else 
			{
				return s_ErrString.c_str();
			}

			if (cmdGiver->m_pet/* && cmdGiver->m_pet->m_ai*/)
			{

				if (id == PET_SKILL_DROPRIDE)
				{
					if (cmdGiver->IsRidePet())
						cmdGiver->DropUnRide(0.5f);
				}
				else if (id == PET_SKILL_RIDE)
				{
					if (cmdGiver->IsRidePet())
						cmdGiver->RideOffPet();
					else
						cmdGiver->RidePet();
				}
				else if (id == PET_SKILL_LINKUP && cmdGiver->m_pet->m_ai)
				{
					SItemID pet = ((CRegionPetAI *)cmdGiver->m_pet->m_ai)->m_pItemPet;
					if(cmdGiver->IsLinkUpWithPet())
						cmdGiver->LinkUpWithPet(0);
					else
						cmdGiver->LinkUpWithPet(pet.id);
				}
			}

			return "petskill use ok";
		}
		else if (stricmp(token, "removenpc") == 0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);

			token = strtok(NULL, seps);
			if (!token)
			{
				return s_ErrString.c_str();
			}
			std::string NpcName = token;

			CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT];
			int nFindCount = cmdGiver->m_scene->GetAroundObjectByGrid(pFindCreature, cmdGiver->m_pos, 120 ,20);
			for (int creCounter=0; creCounter<nFindCount; creCounter++)
			{
				CRegionObject* ob = pFindCreature[creCounter];
				if (OB_TYPE(ob) != OBJECT_TYPE_CREATURE &&
					OB_TYPE(ob) != OBJECT_TYPE_NPC_SERVICE &&
					OB_TYPE(ob) != OBJECT_TYPE_NPC_COMBATIVE)
					continue;
				CRegionCreature* pCre = (CRegionCreature*)pFindCreature[creCounter];
				if (pCre->IsUser())
					continue;
				if (pCre->Distance(cmdGiver->m_pos) > 120)
					continue;

				if (pCre->m_core.Name == NpcName)
				{
					if (OB_TYPE(ob) == OBJECT_TYPE_NPC_SERVICE)
					{
						pCre->m_scene->RemoveCreature(pCre->m_oId);
						g_factory->DestroyObject(pCre);
					}
					else
					{
						pCre->Die(cmdGiver);
					}
					return s_SuccessString.c_str();
				}
			}
			return "Error: Creature Not Found!\n";
		}
		else if (stricmp(token, "removebody") == 0)
		{
			CHECK_PRIVILEGES(EUP_GM_Mask, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
			long bodyid = 0;
			token = strtok( NULL, seps );
			if(token)
			{
				bodyid = atol(token);
				if (bodyid && cmdGiver->m_scene)
				{
					CRegionCreature *cre = cmdGiver->m_scene->FindCreature(bodyid);
					if (cre)
					{
						if (cre->IsUser() || cre->IsUserPet())
						{
							return "removebody error";
						}
						if (cre->IsMonster() && cre->m_spawn)
						{
							cre->m_spawn->KillNpc(cre);
						}
						if (cre->m_ai)
						{
							cre->DeleteAI();
						}
						cmdGiver->m_scene->RemoveCreature(cre->m_oId);
						g_factory->DestroyObject(cre);
					}
				}
			}
			return "removebody";
		}
		else if(stricmp(token, "setstarttime")==0)
		{
		}

//-------------add end   by tony 05.06.13------------------------------------------//
		else if (stricmp(token, "see")==0)
		{
//			CHECK_PRIVILEGES(EUP_ModifyAttrib, "您没有权限察看数据");
			token = strtok( NULL, seps );
			if(token)
			{
				username = token;
				CRegionUser* pUser = g_region->FindUser(username.c_str());
				if (!pUser || !pUser->m_dummy)
				{
					CG_CmdPacket& packet = g_region->m_gws.BeginSend();
					packet.WriteShort(r2g_query_user_info);
					packet.WriteLong(cmdGiver->m_userInfo->m_userId);
					packet.WriteString((char*)username.c_str());
					g_region->m_gws.EndSend();

					return R(MSG_CLIENT_COMMAND_NAT_SAME_AREA);
				}
				else 
				{
					SCreatureInfo info;
					pUser->m_dummy->ToCreatureInfo(&info);

					g_sendCmd->BeginWrite();
					g_sendCmd->WriteShort(r2c_user_data);
					info.Serialize(g_sendCmd);
					cmdGiver->RecvCmd(g_sendCmd);
					return "";
				}
			}
			else 
			{
				//return "格式: see [username]";
				return s_ErrString.c_str();
			}
		}
		else if (stricmp(token, "show")==0)
		{
//			CHECK_PRIVILEGES(EUP_ModifyAttrib, "您没有权限察看数据");
			cmdGiver->State_SetVisible(true);
			return R(MSG_CLIENT_COMMAND_SET_OPTION_OK);
		}
		else if (stricmp(token, "hide")==0)
		{
//			CHECK_PRIVILEGES(EUP_ModifyAttrib, "您没有权限察看数据");
			cmdGiver->State_SetVisible(false);
			return R(MSG_CLIENT_COMMAND_SET_OPTION_OK);
		}
		else if (stricmp(token, "AccountOP")==0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);
            if (cmdGiver && cmdGiver->m_userInfo)
            {
			    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
			    packet.WriteShort(r2g_login_server_command);
                packet.WriteLong(cmdGiver->m_userInfo->m_userId);
			    packet.WriteString((char*)szCommand+strlen("AccountOP")+1);
			    g_region->m_gws.EndSend();
            }
        }
		else if (stricmp(token, "startfun")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_CANNOT_START_FUN), bPyGMOP);
			g_region->m_gws.StartFun();
			// g_region->StartFun(false);
			return "start ok";
		}
		else if (stricmp(token, "endfun")==0)
		{
			g_region->m_gws.EndFun();
			// g_region->EndFun();
			return "end ok";
		}
		else if (stricmp(token, "ban")==0)
		{
			CHECK_PRIVILEGES(EUP_MoveUser, R(MSG_CLIENT_COMMAND_CANNOT_BAN), bPyGMOP);
			token = strtok(NULL, seps);
			ECreatureState forbidType = CreState_Visible;
			long forbidTime = 0;
			if (token)
			{
				if (stricmp(token, "speak")==0)
					forbidType = CreState_CanSpeak;
				else if (stricmp(token, "pshop")==0)
					forbidType = CreState_CanPShop;
				else if (stricmp(token, "move")==0)
					forbidType = CreState_CanMove;
				else 
					//return "格式: forbid [speak|pshop|move] [user] [time(minutes)]";
					return s_ErrString.c_str();
			}
			else 
			{
				//return "格式: forbid [speak|pshop|move] [user] [time(minutes)]";
				return s_ErrString.c_str();
			}

			token = strtok(NULL, seps);
			if (!token)
				//return "格式: forbid [speak|pshop|move] [user] [time(minutes)]";
				return s_ErrString.c_str();
			username = token;

			token = strtok(NULL, seps);
			if (token)
				forbidTime = atoi(token);
			else 
				forbidTime = 240; // 默认4小时

			if (forbidTime<1)
				forbidTime = 240;

			forbidTime *= 60000;

			CG_CmdPacket& packet = g_region->m_gws.BeginSend();
			packet.WriteShort(r2g_forbid_user);
			packet.WriteByte(1);
			packet.WriteString((char*)username.c_str());
			packet.WriteLong(forbidType);
			packet.WriteLong(forbidTime);
			g_region->m_gws.EndSend();

			switch (forbidType)
			{
			case CreState_CanSpeak:
				return R(MSG_CLIENT_COMMAND_BAN_SPEAK);
			case CreState_CanPShop:
				return R(MSG_CLIENT_COMMAND_BAN_SHOP);
			case CreState_CanMove:
				return R(MSG_CLIENT_COMMAND_BAN_MOVE);
			default:
				//return "格式: forbid [speak|pshop|move] [user] [time(minutes)]";
				return s_ErrString.c_str();
			}
			//return "格式: forbid [speak|pshop|move] [user] [time(minutes)]";
			return s_ErrString.c_str();
		}
		else if (stricmp(token, "unban")==0)
		{
			CHECK_PRIVILEGES(EUP_MoveUser, R(MSG_CLIENT_COMMAND_CANNOT_UNBAN), bPyGMOP);
			token = strtok(NULL, seps);
			ECreatureState forbidType = CreState_Visible;
			if (token)
			{
				if (stricmp(token, "speak")==0)
					forbidType = CreState_CanSpeak;
				else if (stricmp(token, "pshop")==0)
					forbidType = CreState_CanPShop;
				else if (stricmp(token, "move")==0)
					forbidType = CreState_CanMove;
				else 
					//return "格式: forbid [speak|pshop|move] [user] [time(minutes)]";
					return s_ErrString.c_str();
			}
			else 
			{
				//return "格式: forbid [speak|pshop|move] [user] [time(minutes)]";
				return s_ErrString.c_str();
			}

			token = strtok(NULL, seps);
			if (!token)
				//return "格式: forbid [speak|pshop|move] [user] [time(minutes)]";
				return s_ErrString.c_str();
			username = token;

			CG_CmdPacket& packet = g_region->m_gws.BeginSend();
			packet.WriteShort(r2g_forbid_user);
			packet.WriteByte(0);
			packet.WriteString((char*)username.c_str());
			packet.WriteLong(forbidType);
			packet.WriteLong(0);
			g_region->m_gws.EndSend();

			switch (forbidType)
			{
			case CreState_CanSpeak:
				return R(MSG_CLIENT_COMMAND_UNBAN_SPEAK);
			case CreState_CanPShop:
				return R(MSG_CLIENT_COMMAND_UNBAN_SHOP);
			case CreState_CanMove:
				return R(MSG_CLIENT_COMMAND_UNBAN_MOVE);
			default:
				//return "格式: forbid [speak|pshop|move] [user] [time(minutes)]";
				return s_ErrString.c_str();
			}
			//return "格式: forbid [speak|pshop|move] [user] [time(minutes)]";
			return s_ErrString.c_str();
		}
		else if (stricmp(token, "feodchangeunion")==0)
		{
			CHECK_PRIVILEGES(EUP_Developer, R(MSG_CLIENT_COMMAND_NOT_DEVELOPER), bPyGMOP);

			token = strtok(NULL, seps);
			if (!token)
				//return "格式: feodchangeunion [feodname] [unionname]";
				return s_ErrString.c_str();
			string feodname = token;
			token = strtok(NULL, seps);
			if (!token)
				//return "格式: feodchangeunion [feodname] [unionname]";
				return s_ErrString.c_str();
			string unionname = token;

			SUnionData* pUnionData = g_region->FindUnion(unionname.c_str());
			if (!pUnionData)
				return R(MSG_CLIENT_COMMAND_NOT_UNION);

			CFeodCmdBuilder_Svr::NetSend_r2g_feod_change_union((char*)feodname.c_str(), pUnionData->m_nID);
		}
		else if (stricmp(token, "ar")==0) //"antirobot")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);

			token = strtok(NULL, seps);
			if (!token)
				return s_ErrString.c_str();
			std::string subCmd = token;
			if (subCmd=="on")
			{
				g_AntiRobot.Enable();
			}
			else if (subCmd=="off")
			{
				g_AntiRobot.Disable();
			}
            else if (subCmd=="send")
            {
                g_AntiRobot.SendRequestToUser(cmdGiver->m_userInfo);
            }
            else if (subCmd=="send2")
            {
                g_AntiRobot.SendRequestToUser(cmdGiver->m_userInfo, false, ARS_Asking_2);
            }
			else if (subCmd=="dump")
			{
				g_AntiRobot.DumpData();
			}
			else if (subCmd=="set")
			{
				token = strtok(NULL, seps);
				if (!token)
					return s_ErrString.c_str();
				std::string setCmd = token;
				if (setCmd=="interval")
				{
					token = strtok(NULL, seps);
					if (!token)
						return s_ErrString.c_str();
					int interval = atoi(token);
					g_AntiRobot.Set_SendInterval(interval);
				}
				else if (setCmd=="lockacctime")
				{
					token = strtok(NULL, seps);
					if (!token)
						return s_ErrString.c_str();
					int lockaccMinites = atoi(token);
					g_AntiRobot.Set_LockAccTime(lockaccMinites*60);
				}
			}
			else if (subCmd=="status")
			{
				static std::string strStatus;
				strStatus = g_AntiRobot.GetStatusString();
				return strStatus.c_str();
			}
			else if (subCmd=="ignore")
			{
				token = strtok(NULL, seps);
				if (!token)
					return s_ErrString.c_str();
				std::string ig = token;

				CRegionUser* pIgUser = g_region->FindUser(ig.c_str());
				if (pIgUser)
				{
					g_AntiRobot.AddUserToIgnoredList(pIgUser->m_userId);
				}
			}
			else if (subCmd=="?")
			{
				return "ar on|off|send|dump|status|set interval|set lockacctime";
			}
		}
		else if (stricmp(token, "startcardtrade")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
			g_region->m_gws.StartCardTrade();
        }
		else if (stricmp(token, "stopcardtrade")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
			g_region->m_gws.StopCardTrade();
        }
		else if (stricmp(token, "kickcheater")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
			token = strtok(NULL, seps);
			if (token)
            {
                g_region->m_bKickCheater = (atoi(token)!=0);
            }
            return GetCheaterCheckerInfo(szSendCmd);
        }
		else if (stricmp(token, "cheatervalve")==0)
		{
			CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
			token = strtok(NULL, seps);
			if (token)
            {
                g_region->m_iCheaterValve = atoi(token);
            }
            return GetCheaterCheckerInfo(szSendCmd);
        }
		else if (stricmp(token, "cheaterstate")==0)
		{
            return GetCheaterCheckerInfo(szSendCmd);
        }
		else if (stricmp(token,"updatemcini") == 0)
		{
			CMonCastle *m_mc = CMonCastle::GetMCInstance();
			if (m_mc->GetMcState() == MC_STATE_DONOTHING)
			{
				ULONG zhaogeid = m_mc->zhaoge->m_targetObjId;
				ULONG xiqiid = m_mc->xiqi->m_targetObjId;
				if(m_mc->LoadMCConfig(R(INI_MC)))
				{
					m_mc->zhaoge->m_targetObjId = zhaogeid;
					m_mc->xiqi->m_targetObjId = xiqiid;
					m_mc->zhaoge->m_bfinish = false;
					m_mc->xiqi->m_bfinish = false;
					return "update mc ini succeed!";
				}			
			}
			return "update mc ini unsucceed!";
		}
		else if (stricmp(token, "mc")==0)
		{
			token = strtok( NULL, seps );
			int   monsterDeadNumMax = 0;
			byte  easterDay = 0;
			int   startTimeHour = 0;
			int   endTimeHour = 0;	
			int   startTimeMin = 0;
			int   endTimeMin = 0;
			if (token)
			{
				easterDay = atoi(token);
				easterDay |= 1<<(easterDay-1);
				token = strtok(NULL, seps);
				if (token)
				{
					startTimeHour = atoi(token);
					token = strtok(NULL, seps);
					if (token)
					{
						endTimeHour = atoi(token);
						token = strtok(NULL ,seps);
						if (token)
						{
							startTimeMin = atoi(token);
							token = strtok(NULL ,seps);
							if (token)
							{
								endTimeMin = atoi(token);
								token = strtok(NULL ,seps);
								if (token)
								{
									monsterDeadNumMax = atoi(token);
									token = strtok(NULL ,seps);
									CMonCastle *m_mc = CMonCastle::GetMCInstance();
									m_mc->WriteConfig(easterDay,startTimeHour,endTimeHour,startTimeMin,endTimeMin,monsterDeadNumMax);
									return "add mc task succeed!";
								}
							}
						}
					}
				}
			}
			return "add mc task failed!";
		}
		else if (stricmp(token, "setMaxDungeon")==0)//tim.yang  设置副本最大数量
		{
			token = strtok( NULL, seps );
			long maxdungeon = 0;
			if (token)
			{
				maxdungeon = atoi(token);
				CG_CmdPacket& packet = g_region->m_gws.BeginSend();
				packet.WriteShort(r2g_modify_maxdungeon);
				packet.WriteLong(cmdGiver->m_userInfo->m_userId);
				packet.WriteLong(maxdungeon);
				g_region->m_gws.EndSend();
				return "set setMaxDungeon succeed!";
			}
			return "set setMaxDungeon failed!";
		}
		else if (stricmp(token, "kickFunUser")==0)			//ac.ma  弹出副本角色
		{
			token = strtok( NULL, seps );
			if (token)
			{
				EXT_SPACE::hash_map<ULONG,CRegionUser*>::iterator it;
				if (stricmp(token, "all")==0)
				{
					CHECK_PRIVILEGES(EUP_ModifyAttrib, R(MSG_CLIENT_COMMAND_NOT_MODIFY), bPyGMOP);
					
					for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
					{
						CRegionUser *user;
						user = (*it).second;
						if(!user)
							continue;
						if(g_region->IsInFunMap(user))
						{
							g_region->KickFunUser(user);
						}
					}
					return R(MSG_CLIENT_COMMAND_KICK_FUN_USER_ALL);		
				}else
				{
					for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
					{
						CRegionUser *user;
						user = (*it).second;
						if(!user)
							continue;
						std::string roleName = user->m_username;
						if (stricmp(token,roleName.c_str())==0)
						{
							if(g_region->IsInFunMap(user))
							{
								g_region->KickFunUser(user);
							}
							return R(MSG_CLIENT_COMMAND_KICK_FUN_USER_USERNAME);	
						}
					}//for
				}//else
			}
			return s_ErrString.c_str();
		}
		else if (stricmp(token, "addcardbottlemaxnum")==0)			//ac.ma  弹出副本角色
		{
			token = strtok( NULL, seps );
			if (token)
			{
				int maxusenum = atoi(token);
				if (g_region)
				{
					if(g_region->WriteUseCardMinBottleMaxNum(maxusenum))
					{
						g_region->SetUseCardMinBottleMaxNum(maxusenum);
						return "Set max num success!";
					}
				}
			}
			return s_ErrString.c_str();
		}else if (stricmp(token, "GetPrivileges")==0)			//ac.ma  弹出副本角色
		{
			switch(cmdGiver->m_userInfo->privileges)
			{
			case EUP_LoginNormal:
				return "EUP_LoginNormal";
				break;
			case EUP_LoginGM:
				return "EUP_LoginGM";
				break;
			case EUP_FastMove:
				return "EUP_FastMove";
			    break;
			case EUP_MoveUser:
				return "EUP_MoveUser";
			    break;
			case EUP_AddItems:
				return "EUP_AddItems";
				break;
			case EUP_ModifyAttrib:
				return "EUP_ModifyAttrib";
				break;
			case EUP_ReloadScript:
				return "EUP_ReloadScript";
			    break;
			case EUP_Developer:
				return "EUP_Developer";
			    break;
			default:
				return "EUP_Developer";
			    break;
			}

			return "EUP_Developer";
		}
		else 
        {
			bool bP = false;
			const char* pRet = CRSLogicExt::GetRSLogicExt().DoClientCommand(cmdGiver, szCommand, bPyGMOP, pyGmAccount, bP);
			if (bP)
			{
				return pRet;
			}
            //return "不支持的命令, 输入 -help 取得帮助信息.";
			return s_ErrString.c_str();
        }
    }
	
    //return "命令列表: ?|help|add|ride|spawn_map|terrain_map|reload_script|setlev|updatelev|levelup|gm|startwar";
	return "";
}
