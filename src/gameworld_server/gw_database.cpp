#include "gw_server.h"

//PZH
#include "LogicExt.h"

GWUser::GWUser()
   :team(NULL)
{
    enterTime = 0;
    dwWithdrawGiftLockTick = 0;
    dungeonUID = 0;
    dungeonTID = 0;
    dungeonLastQuit = 0;
	dwLastRemoveMPTime = 0;
	dwWithdrawMPGift = 0;
	IsInWarfield = false;
	lWarfieldLastQuit = 0;
	m_bRefuseTeam = false ;       //设置是否拒绝组队
	m_bRefusetrade = false ;       //设置是否拒绝交易
	m_bRefuseMail = false ;        //设置是否拒绝邮件
	m_bRefuseDuel = false ;        //设置是否拒绝决斗
	m_bRefuseFriend = false ;      //设置是否拒绝好友
	m_bRefuseMonster = false ;     //设置是否拒绝师徒
}

GWLoginClient::GWLoginClient()		
{
	CM_MEMPROTECTOR(m_szKickInfo, 50)

	actorCnt  = 0;
	actorData = NULL;
	netLink   = NULL;
	kickCnt   = 0;
    m_switchRegionCnt = 0;
    m_bKickInSwitchRegion = false;

	//PZH
	//ulc = NULL;
	dwulcSeed = 0;
	//
}

GWLoginClient::~GWLoginClient()
{
	CM_MEMUNPROTECTOR(m_szKickInfo)
}

bool GWLoginClient::SendPacket(CG_CmdPacket *packet)
{
	switch(state) 
	{
	case LCS_QUITING:
		break;
	case LCS_SWITCH_REGION:
		break;
	case LCS_SELECT_CHAR:
		if(netLink)
			return netLink->SendPacket(packet);
		else
			return false;
		break;

	case LCS_INGAME:
		int se = g_server.GetRegionSession(m_regionId);
		if (se == -1)
			return false;
		return g_server.m_region.SendToPeer(se,packet);
		break;
	}
	return false;
}
//PZH
void GWLoginClient::SetPower(unsigned long dwSeed/*GWUnloginClient *ulc*/)
{
	//this->ulc = ulc;
	dwulcSeed = dwSeed;
}

bool GWLoginClient::IsPower()
{
	//return ulc != NULL;
	return 0 != dwulcSeed;
}

void GWLoginClient::PowerLogin()
{

	CG_TCPSession* pUlcSession = NULL;
	GWUnloginClient *ulc = g_server.FindUnloginClientBySeed(dwulcSeed);
	if (NULL == ulc)
	{
		return;	
	}
	pUlcSession = ulc->netLink;
	int nNum = 0;
	long ret = LOGIN_RET_SUCCESS;
	if(!g_server.UserLogin(ulc,&info))
	{
		ret = LOGIN_RET_FAILED_UNKNOWN;
	}

    if (pUlcSession)
    {
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(g2c_login_ret);
	    g_sendCmd->WriteLong(ret);
        
		if (ret!=LOGIN_RET_SUCCESS)
		{
			g_sendCmd->WriteShort((short)LOGIN_RET_FAILED_UNKNOWN);
		}
		else
        {
            g_sendCmd->WriteByte(cPointType);
            g_sendCmd->WriteLong(lPoint);
            g_sendCmd->WriteByte(cVIP);
			CLogicExt::GetCLogicExt().DelInNumMap(ulc->GetAccountName());//PZH
        }
        pUlcSession->SendPacket(g_sendCmd);
    }
}
//
GWLoginClientState GWLoginClient::GetState()
{
	return state;
}

void GWLoginClient::SetState(GWLoginClientState st)
{
	state = st;
}

void GWLoginClient::OnSwitchRegion()
{

}

//
// database 
//
GWDatabase::GWDatabase()
: m_mysql(NULL)
, m_shutdown(false)
, m_connectTime(0)
, m_dwDeleteLevel(30)
{
}

GWDatabase::~GWDatabase()
{
	Close();
}

long GWDatabase::GetDeleteLevel()
{
	return m_dwDeleteLevel;
}
void GWDatabase::SetDeleteLevel(long dwDeleteLevel)
{
	m_dwDeleteLevel = dwDeleteLevel;
}

bool GWDatabase::Initalize()
{
	// CHECK(m_mysql == NULL);
	if (!g_ItemMgr.Init())
	{
		ERR("道具系统初始化失败\n");
		return false;
	}

	LoadActorTable();

	/*
	if(g_cfgGW.connectDatabase.sendQueueCapacity < 1000)
		m_commandQueue.Create(1000, "DBCommand");
	else
		m_commandQueue.Create(g_cfgGW.connectDatabase.sendQueueCapacity, "DBCommand");
	
	if(g_cfgGW.connectDatabase.recvQueueCapacity < 1000)
		m_resultQueue.Create(1000, "DBResult");
	else
		m_resultQueue.Create(g_cfgGW.connectDatabase.recvQueueCapacity, "DBResult");
	*/

	m_commandQueue.Create(10000, "DBCommand");
	m_resultQueue.Create(10000, "DBResult");

	m_mysql = mysql_init(NULL);
	if(m_mysql == NULL)
	{
		LOG("Initialize MySQL failed\n");
		return false;
	}

	if (!Connect())
	{
		Close();
		return false;
	}

	LOG("Start database thread.\n");
	Start();
	return true;
}

void GWDatabase::Close()
{
	if (m_mysql != NULL)
	{
		mysql_close(m_mysql);
		m_mysql = NULL;
	}
}

/*
GWUser* GWDatabase::LookupUser(int id)
{
	GWUserTable::iterator iter = m_users.find(id);

	if (iter == m_users.end())
		return NULL;

	return &iter->second;
}

GWUser* GWDatabase::LookupUser(const char* name)
{	
	char uname[GW_MAX_NAME_LENGTH + 1];

    if (name==0 || name[0]==0) return NULL;

	rt2_strcpy(uname, name);
	strlwr(uname);

	stlforeach(GWUserTable, iter, m_users)
	{
		if (iter->second.uname == uname)
			return &iter->second;
	}

	return NULL;
}
*/
int GWDatabase::OnRun()
{
	LOG1("Database thread started at %d\n", rtTimestamp());//timn.yang  %s->%d

	FILE *log = fopen("gwthread.txt","wt");
	time_t tLastCmdTime=time(NULL), tNow;
	GWDBCommand cmd;
	GWDBResult res;

	for(;;)
	{
		m_cmdLock.Enter();
		if(m_shutdown)
		{
			m_cmdLock.Leave();
			return 0;
		}
		m_cmdLock.Leave();

        tNow = time(NULL);
		if(!m_commandQueue.Pop(cmd))
		{
			rtSleep(0.01 * 1000);//tim.yang  rtSleep(m*1000)
            if (tNow>tLastCmdTime+50)
            {
                tLastCmdTime = tNow;
//                mysql_query(m_mysql, "select 0");
            }
			continue;
		}
        tLastCmdTime = tNow;

		m_cmdLock.Enter();
		for(;;)
		{
			if(m_shutdown)
			{
				m_cmdLock.Leave();
				return 0;
			}

			if(m_connectTime)
			{
				m_cmdLock.Leave();
				Sleep(1 * 1000);
				m_cmdLock.Enter();
				continue;
			}

			DWORD cost = rtGetMilliseconds();
			if(mysql_query(m_mysql, cmd.command))
			{
				if (mysql_errno(m_mysql) == CR_CONN_HOST_ERROR)
				{
					m_connectTime = time(NULL);
					continue;
				}
				res.okay = false;
				res.res = NULL;
                ERR("SQL Error: %s\n[%s]\n", cmd.command, mysql_error(m_mysql));
			}
			else
			{
				res.okay = true;		
				if (cmd.type == dbCmdQuery)
					res.res = mysql_store_result(m_mysql);
				else
					res.res = NULL;
				res.affectRows = mysql_affected_rows(m_mysql);
			}
			cost = rtGetMilliseconds() - cost;
//            if(cost > 10000)
			if(cost > 1)
			{
				fprintf(log,"time = %d,cost = %d,sql = %s\n",rtTimestamp(),cost,cmd.command);//timn.yang  %s->%d
				fflush(log);
			}
			break;
		}

		m_cmdLock.Leave();
		res.succeeded = cmd.succeeded;
		res.failed = cmd.failed;
		res.data = cmd.data;
		m_resultQueue.Push(res);
	}
	fclose(log);
	return 0;
}

bool GWDatabase::SaveUserData(GWUser* data, bool sync, int param,GWDBCallback callback)
{
	// ERR1("Save user data [%s]\n",data->name.c_str());
	
	// check db array size
	int size = (int)data->giftsWithdrawed.size() + 5;
	while(m_commandQueue.GetFreeSize() < size)
	{
		rtSleep(0.5 * 1000);
		ERR1("m_commandQueue size = %d,sleep",m_commandQueue.GetFreeSize());
	}

	data->updateTimeStamp = time(NULL);
	if(data->onlineTime < 0)
	{
		ERR1("user [%s] onlime time < 0, modify to 0\n",data->uname.c_str());
		data->onlineTime = 0;
	}

	// 生成属性Assert值
	string strAttribAssert;
	SUserDataAssert uda;
	memset(&uda, 0, sizeof(SUserDataAssert));
	uda.version = SUserDataAssert_Version;
	uda.userID = data->userID;
	uda.bankMoney = data->bankMoney;
	uda.money = data->money;
	uda.metier = data->attr.metier;
	uda.metierLevel = data->attr.metierLevel;
	uda.faction = data->attr.faction;
	uda.level = data->attr.level;
	uda.exp = data->attr.exp;
	uda.ap = data->attr.ap;
	uda.sp = data->attr.sp;
	uda.hp = data->attr.hp;
	uda.mp = data->attr.mp;
	uda.EnP = data->attr.EnP;
	uda.x = data->attr.x;
	uda.y = data->attr.y;
	uda.constitution = data->attr.constitution;
	uda.strength = data->attr.strength;
	uda.dexterity = data->attr.dexterity;
	uda.intellect = data->attr.intellect;
	uda.hit = data->attr.hit;
	uda.pk = data->attr.pk;
	uda.credit = data->attr.credit;
	uda.spClearCnt = data->attr.spClearCnt;
	uda.Soul = data->attr.Soul;
	uda.Vigour = data->attr.Vigour;
	uda.VigourPoint = data->attr.VigourPoint;
	uda.Sex = data->attr.Sex;

	uda.Cul = data->attr.Cul;
	uda.CulExp = data->attr.CulExp;
	uda.GradeLev = data->attr.GradeLev;
	uda.PlayerOnlineTime = data->attr.PlayerOnlineTime;
	uda.HeadImageID = data->attr.HeadImageID;

	uda.Contribute = data->attr.Contribute;
	uda.Point = data->attr.Point;
	uda.Record = data->attr.Record;
	uda.UnionRecord = data->attr.UnionRecord;
	uda.JobRecord = data->attr.JobRecord;
	uda.BornTag = data->attr.BornTag;
	uda.RelivePostion = data->attr.RelivePostion;
    uda.Exploit = data->attr.exploit;
	

	SUserDataAssert_CreateMD5String12(uda, strAttribAssert);

	GWDBCommand cmd;
	cmd.type = dbCmdUpdate;
	cmd.data = (void*)param;
	cmd.succeeded = NULL;
	cmd.failed = NULL;
	
	char cTeamFree;
	if(data->bTeamFree)
	{
		cTeamFree = 1;
	}
	else
	{
		cTeamFree = 0;
	}
	cTeamFree = 1; //暂时不保存
	/*long UpdateTime = (long)time(NULL);*/
	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, 
		"UPDATE `user` SET"
		" Scene='%s'"
		",HP=%d"
		",MP=%d"
		",Level=%d"
		",EXP=%d"
		",AP=%d"
		",SP=%d"
		",X=%d"
		",Y=%d"
		",HP=%d"
		",MP=%d"
		",EXP=%d"
		",Str=%d"
		",Con=%d"
		",Dex=%d"
        ",`Int`=%d"
		",Hit=%d"
		",Skills='%s'"
		",Items='%s'"
        ",Gold=%d"
		",ActorID=%d"
		",Faction=%d"
		",PK=%d"
		",Shortcut='%s'"
        ",Config='%s'"
		",Task='%s'"
		",LastUpdateDate=NOW()"
		",Credit=%d"
		",OnlineTime=%d"
		",MetierLevel=%d"
		",ClearCnt=%d"
		",`c_Soul`=%d"
		",`Vigour` =%d"
		",`VigourPoint`=%d"
		",`Sex` =%d"
		",`c_Contribute`=%d"
		",`c_Point`=%d"
		",`c_Record`=%d"
		",`c_UnionRecord`=%d"
		",`c_JobRecord`=%d"
		",`c_BornTag`=%d"
		",`nRelivePostion`=%d"
		",DungeonTID=%d"
		",DungeonUID=%d"
		",DungeonQuit=%d"
		",DungeonTask='%s'"
		",InWarfield=%d"
		",WarfieldQuit=%d"
		",UsData='%s'"
		",WithdrawMPGift=%d"
		",MyData='%s'"	
		",TeamFree=%d"
		",c_Cul=%d"
		",CulExp=%d"
		",PlayerOnlineTime=%d"
		",Exploit=%d"
		",GradeLev=%d"
		",HeadImageID=%d"
		//",EnP=%d"
		",LineSysGroupID=%d"
		",LineSysLineID=%d"
		",Popularity='%s'"
		",petItemID=%d"
		",isFirstLogin=%d"
		",IsDead=%d"
		",EffectSkill='%s'"
		",ChangeModelID=%d"
		//",UpdateSCDTime=%d"
		" WHERE ID=%d LIMIT 1",
		data->scene.c_str(),
		data->attr.hp,
		data->attr.mp,
		data->attr.level,
		data->attr.exp,
		data->attr.ap,
		data->attr.sp,
		data->attr.x,
		data->attr.y,
		data->attr.hp,
		data->attr.mp,
		data->attr.exp,
		data->attr.strength,
		data->attr.constitution,
		data->attr.dexterity,
		data->attr.intellect,
		data->attr.hit,
		data->strSkills.c_str(),
		data->items.c_str(),
        data->money,
		data->attr.actorID,
		data->attr.faction,
		data->attr.pk,
		data->shortcut.c_str(),
        data->configData.c_str(),
		data->task.c_str(),
		data->attr.credit,
		data->onlineTime,
		data->attr.metierLevel,
		data->attr.spClearCnt,
		data->attr.Soul,
		data->attr.Vigour,
		data->attr.VigourPoint,
		data->attr.Sex,
		data->attr.Contribute,
		data->attr.Point,  
		data->attr.Record,
		data->attr.UnionRecord,
		data->attr.JobRecord,
		data->attr.BornTag,
		data->attr.RelivePostion,
        data->dungeonTID,
        data->dungeonUID,
        data->dungeonLastQuit,
		data->fbTask.c_str(),
		(int)data->IsInWarfield,
		data->lWarfieldLastQuit,
		strAttribAssert.c_str(),
		data->dwWithdrawMPGift,
		data->extraData.data==NULL?"":EscapeString((char*)data->extraData.data,data->extraData.size).c_str(),
		cTeamFree,
		data->attr.Cul,
		data->attr.CulExp,
		data->attr.PlayerOnlineTime,
		data->attr.exploit,
		data->attr.GradeLev,
		data->attr.HeadImageID,	
		//data->attr.EnP,
		data->LineSYSGroupID,
		data->LineSYSLineID,
		data->popularity.c_str(),
		data->petItemID,
		data->isFirstLogin,
		data->isDead,
		data->strEffects.c_str(),
		data->ChangeModelID,
		//data->UpdateSCDTime,
 		data->userID);

	if (sync)
		ExecCommand(cmd.command, NULL);
	else
		PushCommand(cmd);	

	// 礼品
	if (!data->giftsWithdrawed.empty())
	{
		for (int i=0; i<(int)data->giftsWithdrawed.size(); i++)
		{
			if (!GetGiftMgr()->RemoveInDB(data->giftsWithdrawed[i].index))
			{
				ERR("Can NOT remove data from Gift DB!\n");
				CHECK(0);
			}

			GWLoginClient* pLc = g_server.FindLoginClientByAccountId(data->accountId);
			string ip;
			if (pLc)
				ip = pLc->info.ip;
			g_server.LogUserEvent(PLT_WithdrawGift, data->userID, ip.c_str(), data->giftsWithdrawed[i].type, data->giftsWithdrawed[i].attribs.c_str(), 0, "", 0, "");
		}
		data->giftsWithdrawed.clear();
	}

	// 仓库
//	if (data->bankModified)
	{
		rt2_sprintf(cmd.command, "UPDATE T_bank SET Items='%s', BankMoney=%d , Pets='%s' WHERE UserID='%d' LIMIT 1", 
			data->bank.c_str(), 
			data->bankMoney,
			data->bankPet.c_str(),
			data->userID);	

		if (sync)
		{
			if (ExecCommand(cmd.command, NULL))
				data->bankModified = false;
		}
		else
		{			
			PushCommand(cmd);
			data->bankModified = false;
		}
	}

	rt2_snprintf(cmd.command,GW_MAX_DB_COMMAND_LENGTH, "UPDATE T_pet SET Pets='%s' WHERE UserID='%d' LIMIT 1", 
		data->pet.c_str(), 
		data->userID);

	if (sync)
	{
		if (ExecCommand(cmd.command, NULL))
			data->bankModified = false;
	}
	else
	{			
		PushCommand(cmd);
		data->bankModified = false;
	}

	//<add by fox for bug>
	string SafeString;
	EscapeString(SafeString,data->friends);
	//</add by fox for bug>

	rt2_sprintf(cmd.command, "UPDATE community SET "
		"Friend = '%s' "
		"WHERE UserID=%d",
		SafeString.c_str(),//modify by fox for bug
		data->userID);	

	bool ret = true;
	if (sync)
	{
		ExecCommand(cmd.command, NULL);
	}
	else
	{
		if(callback)
		{
			cmd.succeeded = callback;
			cmd.failed = callback;
		}
		else
		{
			cmd.succeeded = GWDatabase::OnSaveUserData;
			cmd.failed = GWDatabase::OnSaveUserData;
		}
		ret = PushCommand(cmd);	
	}	

	data->isModified = false;
	return ret;
}

void GWDatabase::ProcessQueryResult()
{
	time_t now = time(NULL);

	if (m_connectTime && now - m_connectTime > GW_CONNECT_DB_INTERVAL)
	{
		m_connectTime = now;

		Connect();
	}

	GWDBResult res;

	int count = m_resultQueue.GetSize();

	while(count)
	{
		if (!m_resultQueue.Pop(res))
			return;

		if (res.okay)
		{
			if (res.succeeded)
				res.succeeded(&res);

			if (res.res)
				mysql_free_result(res.res);
		}
		else
		{
			if (res.failed)
				res.failed(&res);
		}

		count--;
	}
}

bool GWDatabase::ExecCommand(const char* command, MYSQL_RES** res, bool bLogErr)
{
    return ExecCommandEx(command, res, NULL, bLogErr);
}

bool GWDatabase::ExecCommandRow(const char* command, MYSQL_RES** res, int* iAffectedRows)
{
    return ExecCommandEx(command, res, iAffectedRows, true);
}

bool GWDatabase::ExecCommandEx(const char* command, MYSQL_RES** res, int* iAffectedRows, bool bLogErr)
{
	DWORD lt = rtGetMilliseconds();
	RtThreadLock::AutoLock lock(m_cmdLock);
	lt = rtGetMilliseconds() - lt;

	if (m_connectTime)
		return false;
	if (!m_mysql)
		return false;

    DWORD tBefore = rtGetMilliseconds();
    if (mysql_query(m_mysql, command))
	{
        if (bLogErr)
        {
		    ERR("Exec [%s] failed (%d)\nMYSQL: %s\n", command, mysql_errno(m_mysql), mysql_error(m_mysql));
        }

		if (mysql_errno(m_mysql) == CR_CONN_HOST_ERROR)
			m_connectTime = time(NULL);

		return false;
	}
	else
	{
		if (res)
			*res = mysql_store_result(m_mysql);
	}
    if (iAffectedRows)
    {
	    *iAffectedRows = mysql_affected_rows(m_mysql);
    }

    DWORD tAfter = rtGetMilliseconds();
    if ((tAfter-tBefore)>1000 || lt > 1000)
    {
        ERR3("lt = %d,SQL time = %d, SQL = %s\n",lt, tAfter-tBefore, command);
    }

	return true;
}

bool GWDatabase::QueryActorList(GWLoginClient &account)
{
	char cmd[GW_MAX_DB_COMMAND_LENGTH];	
	long nLev = this->GetDeleteLevel() - 1;
	rt2_snprintf(cmd, GW_MAX_DB_COMMAND_LENGTH, "SELECT "
		"ID,"
		"Name,"
		"X,"
		"Y,"
		"HP,"
		"MP,"
		"EXP,"
		"AP,"
		"Level,"
		"Metier,"
		"MetierLevel,"
		"SP,"
		"Str,"
		"Con,"
		"Dex,"
		"`Int`,"
		"Hit,"		
		"ActorID,"
		"Faction,"
		"PK,"
		"Items,"
		"DeleteDate,"
		"HeadModelID"
		
		//Tianh
		",c_Cul"
		",c_Soul"
		",c_Contribute"
		",c_Point"
		",c_Record"
		",c_UnionRecord"
		",c_JobRecord"
		",c_BornTag"
		",Sex"
		",GradeLev" 
		",Vigour"
		",VigourPoint"
		",nRelivePostion"
		",PlayerOnlineTime"
		",CulExp"
		",Exploit"
		",EnP"
		",HeadImageID"

		//PZH
		",HasPassword"
		",deleted"
		//" FROM user WHERE AccountID='%d' and deleted=0 ORDER BY CreateDate", 
		" FROM user WHERE AccountID='%d' and (deleted=0 or (deleted=1 and TO_DAYS(NOW()) - TO_DAYS(DeleteDate) < 7 and Level > %d)) ORDER BY CreateDate", 
		//
		account.info.accountId, nLev);

	MYSQL_RES* res;

 	if (!ExecCommand(cmd, &res))
		return false;

	int actorCnt = (int)mysql_num_rows(res);
	if(actorCnt > GW_MAX_USER_PER_ACCOUNT)
		//PZH
		//return false;
		actorCnt = GW_MAX_USER_PER_ACCOUNT;
		
 //   int Max_lv = 0;
	//string Max_lv_rolename = "";
	//int AccountId = 0;
	//string AccountName = "";

	account.actorCnt = actorCnt;
	for (int n = 0; n < account.actorCnt; n++)
	{
		MYSQL_ROW row = mysql_fetch_row(res);

		account.users[n].userID = atol(row[0]);
		account.users[n].name = row[1];
		account.users[n].attr.x = atol(row[2]);
		account.users[n].attr.y = atol(row[3]);
		account.users[n].attr.hp = atol(row[4]);
		account.users[n].attr.mp = atol(row[5]);
		account.users[n].attr.exp = atol(row[6]);
		account.users[n].attr.ap = atol(row[7]);
		account.users[n].attr.level = atol(row[8]);
		account.users[n].attr.metier = atol(row[9]);
		account.users[n].attr.metierLevel = atol(row[10]);
		account.users[n].attr.sp = atol(row[11]);
		account.users[n].attr.strength = atol(row[12]);
		account.users[n].attr.constitution = atol(row[13]);
		account.users[n].attr.dexterity = atol(row[14]);
		account.users[n].attr.intellect = atol(row[15]);
		account.users[n].attr.hit = atol(row[16]);
		account.users[n].attr.actorID = atol(row[17]);
		account.users[n].attr.faction = atol(row[18]);
		account.users[n].attr.pk = atol(row[19]);
		account.users[n].items = row[20];
		account.users[n].deleteDate = row[21] ? row[21] : "";
		account.users[n].headModelID = atol(row[22]);
		
		//Tianh
		account.users[n].attr.Cul  = atol(row[23]);
		account.users[n].attr.Soul = atol(row[24]);
		account.users[n].attr.Contribute  = atol(row[25]);
		account.users[n].attr.Point   = atol(row[26]);
		account.users[n].attr.Record  = atol(row[27]);
		account.users[n].attr.UnionRecord = atol(row[28]);
		account.users[n].attr.JobRecord = atol(row[29]);
		account.users[n].attr.BornTag = atol(row[30]);
		account.users[n].attr.Sex    = atol(row[31]);
		account.users[n].attr.GradeLev   = atol(row[32]);
		account.users[n].attr.Vigour  = atol(row[33]);
		account.users[n].attr.VigourPoint  = atol(row[34]);
		account.users[n].attr.RelivePostion = atol(row[35]);
		account.users[n].attr.PlayerOnlineTime = atol(row[36]);
		account.users[n].attr.CulExp = atol(row[37]);
		account.users[n].attr.exploit = atol(row[38]);
		account.users[n].attr.EnP = atol(row[39]);
		account.users[n].attr.HeadImageID = atol(row[40]);
		//PZH
		int nTemp = atol(row[41]);
		account.users[n].bHasPassword = 0 != nTemp;
		nTemp = atol(row[42]);
		account.users[n].bDelete = 0 != nTemp;

		////Tianh  用来找到帐号所有角色里的最大角色等级
		//if (account.users[n].attr.level > Max_lv)
		//{
		//	Max_lv = account.users[n].attr.level;
		//	Max_lv_rolename=account.users[n].name;
		//}
	}

	/*mysql_free_result(res);*/

	//Tianh  把帐号ID,帐号,帐号里的最大角色等级,名字LOG到数据库表t_log_account里
	/*AccountId = account.info.accountId;
	AccountName = account.info.accountName;

	 
    GWDBCommand Ccmd;

	rt2_sprintf(Ccmd.command, "SELECT COUNT(*) FROM `max_role_name_lev` WHERE `id` = '%d'", AccountId);

	if (!ExecCommand(Ccmd.command, &res))
		return false;

	MYSQL_ROW row = mysql_fetch_row(res);

	int count = atol(row[0]);

	if (count == 0)
	{
		rt2_sprintf(Ccmd.command,
			"INSERT INTO `max_role_name_lev`(id,AccountName,Max_lv,Max_lv_rolename)values(%d,'%s',%d,'%s')",
			AccountId,
			AccountName.c_str(),
			Max_lv,
			Max_lv_rolename.c_str());
	}
	else
	{
		rt2_sprintf(Ccmd.command,"UPDATE `max_role_name_lev` SET AccountName='%s', Max_lv_rolename = '%s',Max_lv = %d,WHERE `idx` = '%d'",
			AccountName.c_str(),Max_lv_rolename.c_str(),Max_lv, AccountId);	
	}
	
	PushCommand(Ccmd);

	mysql_free_result(res);*/

	return true;
}

void GWDatabase::OnSaveUserData(GWDBResult* result)
{

}

GWCreateUserResult GWDatabase::CreateUserData(GWUser* data)
{
	char cmd[GW_MAX_DB_COMMAND_LENGTH];

	rt2_sprintf(cmd, "SELECT COUNT(*) FROM user WHERE AccountID = '%d' and deleted=0", data->accountId);

    MYSQL_RES* res;

	if (!ExecCommand(cmd, &res))
		return gws_create_user_error;

	MYSQL_ROW row = mysql_fetch_row(res);

	int count = atol(row[0]);
	mysql_free_result(res);
	if (count >= GW_MAX_USER_PER_ACCOUNT)
		return gws_create_user_error;

	char safeName[GW_MAX_NAME_LENGTH * 2 + 1];
	char uniqueName[GW_MAX_NAME_LENGTH * 2 + 1];
	// char account[GW_MAX_NAME_LENGTH + 1];

	data->onlineTime = 0;
	EscapeString(safeName, data->name.c_str());

    rt2_strcpy(uniqueName, safeName);
    strlwr(uniqueName);

	rt2_sprintf(cmd, "INSERT INTO user"
		" (UniqueName, Name, ActorID, AccountID, Account, Scene, X, Y, HP, MP, EXP, AP, SP, Level, Skills, Items, Gold, Metier,"
		"MetierLevel, HeadModelID, LastDelUnionTime, CreateDate, LastRemoveMPTime, Password, LineSysGroupID, LineSysLineID,HeadImageID,c_BornTag,isFirstLogin,IsDead,GradeLev) "
		" VALUES('%s', '%s', '%d', '%d', '%s', '%s', %d, %d, %d, %d, %d, %d,%d, %d, '%s', '%s', '%d', '%d', '%d', '%d', '%s', NOW(), '%s', '%s', '%d', '%d', '%d', '%d', '%d', '%d', '%d') ",		
		uniqueName,
		safeName,
		data->attr.actorID,
		data->accountId,
		data->accountName.c_str(),
		data->scene.c_str(),
		data->attr.x,
		data->attr.y,
		data->attr.hp,
		data->attr.mp,
		data->attr.exp,
		data->attr.ap,
		data->attr.sp = 1, //Tianh  人物初始给1SP
		data->attr.level,
		data->strSkills.c_str(),
		data->items.c_str(),
		data->money,
		data->attr.metier,
		data->attr.metierLevel,
		data->headModelID,
		"0000-00-00 00:00:00",
		"0000-00-00 00:00:00",
		data->bankPass.c_str(),
		data->LineSYSGroupID,
		data->LineSYSLineID,
		data->attr.HeadImageID,
		data->attr.BornTag,
		data->isFirstLogin = 0,//0是未登陆
		data->isDead = 0,
		data->ChangeModelID = 0,
		data->attr.GradeLev = 1);

	if (!ExecCommand(cmd, &res, false))
	{
		MSG1("Can't create user %s\n", data->name.c_str());

		int errorID = mysql_errno(m_mysql);
		if (errorID == ER_DUP_ENTRY)
			return gws_create_user_duplicate_name;
		else
			return gws_create_user_error;
	}

    // rtSleep(2.f);

	//rt2_sprintf(cmd, "SELECT `ID` FROM `user` WHERE `UniqueName` LIKE CONVERT( _utf8 '%s' USING gb2312 )", uniqueName);
	rt2_sprintf(cmd, "SELECT `ID` FROM `user` WHERE `UniqueName` = '%s' ", uniqueName);

	if (!ExecCommand(cmd, &res, true))
    {
		return gws_create_user_error;
    }

    if (mysql_num_rows(res)!=1)
    {
		MSG1("Can't find created user %s\n", data->name.c_str());
	    mysql_free_result(res);
        return gws_create_user_error;
    }

	row = mysql_fetch_row(res);
	data->userID = atol(row[0]);
	mysql_free_result(res);

	rt2_sprintf(cmd, "INSERT INTO community(UserID) VALUES(%d)", data->userID);

	if (!ExecCommand(cmd, NULL, true))
	{
		LOG1("Can't create community info for user %s\n", data->name.c_str());
		return gws_create_user_error;
	}

	rt2_sprintf(cmd, "SELECT Items,BankMoney,Pets,Password FROM T_bank WHERE UserID = '%d'", data->userID);
	if (!ExecCommand(cmd, &res, true))
		return gws_create_user_error;
	count = mysql_num_rows(res);
	if (count == 0)
	{
	    mysql_free_result(res);
		CBag bagEmpty(g_ItemMgr.GetBankInitLineCount(), BANK_GRID_COLUMN_COUNT, 1, &g_ItemMgr);
		data->bank = bagEmpty.SaveToString(&g_ItemMgr);
		CBag bankPetbagEmpty(BANK_PET_LINE_COUNT, BANK_PET_COLUMN_COUNT, 1, &g_ItemMgr);
		data->bankPet = bankPetbagEmpty.SaveToString(&g_ItemMgr);
		data->bankMoney = 0;
		rt2_sprintf(cmd, "INSERT INTO T_bank(UserID,AccountID,Account,Items,BankMoney,Pets,Password) VALUES('%d',%d,'%s','%s',%d,'%s','%s')",
            data->userID, data->accountId, data->name.c_str(), data->bank.c_str(), 0, data->bankPet.c_str(), "");
		if (!g_server.m_database.ExecCommand(cmd, &res, true))
		{
			return gws_create_user_error;
		}
	}else
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		data->bank = row[0];
		data->bankMoney = atoi(row[1]);
		data->bankPet = row[2];
		data->bankPass = row[3];
	    mysql_free_result(res);
	}
	data->bankModified = false;

	rt2_snprintf(cmd, GW_MAX_DB_COMMAND_LENGTH, "SELECT Pets FROM T_pet WHERE UserID = '%d'", data->userID);
	if (!ExecCommand(cmd, &res, true))
		return gws_create_user_error;
	count = mysql_num_rows(res);
	if (count == 0)
	{
		mysql_free_result(res);
		if (data->pet.empty())
		{
			CBag bagEmpty(g_ItemMgr.GetBankInitLineCount(), BANK_GRID_COLUMN_COUNT, 1, &g_ItemMgr);
			data->pet = bagEmpty.SaveToString(&g_ItemMgr);
		}
		rt2_sprintf(cmd, "INSERT INTO T_pet(UserID,AccountID,Name,Pets) VALUES('%d','%d','%s','%s')",
			data->userID, data->accountId, data->name.c_str(), data->pet.c_str());
		if (!g_server.m_database.ExecCommand(cmd, &res, true))
		{
			return gws_create_user_error;
		}
	}else
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		data->pet = row[0];
		mysql_free_result(res);
	}

#ifdef _DEBUG
	LOG2("User %s created, UserID = %d\n", data->name.c_str(), data->userID);
#endif
	SaveUserData(data, true, 0, NULL);
	return gws_create_user_okay;
}

GWCreateUserResult GWDatabase::CreateUser(DWORD accountID, 
										  const string& name,
										  int actorID,
										  short headModelID,
										  int faction, 
										  std::string password,
										  GWUser* outputData,
										  int Sex,
										  short headImageID,
										  short iAnimal)
{
	if (!IsValidName(name))
		return gws_create_user_invalid_name;

	GWActor* actor = LookupActor(actorID);
	if (actor == NULL)
	{
		LOG1("Can not find actor %d\n", actorID);
		return gws_create_user_error;
	}

	GWUser user;
	user.attr = actor->attr;
	user.attr.faction = faction;
	user.strSkills = actor->skills;
	user.bankModified = true;
	user.unionID = 0;
	user.headModelID = headModelID;
	user.dwState = 0xffffffff;
	user.bankPass = password;
	/**Tianh**09.12.13**玩家信息结构体读取男女信息**/
	user.attr.Sex = (long)Sex;
	user.LineSYSGroupID = 1;
	user.LineSYSLineID = 1;

	user.attr.HeadImageID = headImageID;
	user.attr.BornTag  =  iAnimal;
	
	
	rt2_strcpy(g_strStaticBuffer, actor->items.c_str());
	char const *sp = ";, \t";
	char *pd = strtok(g_strStaticBuffer, sp);
	
	//Tianh 10.04.21 创建人物时在背包和身上加载道具
	CItemContainerBase con(&g_ItemMgr, NULL);
	while (pd)
	{
		SItemID item = g_ItemMgr.CreateItem(atoi(pd));
		if (ItemID_IsValid(item))
		{
			bool res = con.AddItem(item);
			CHECK(res);
		}
		pd = strtok(NULL, sp);
	}
	char Str[1024];
	rt2_strcpy(Str, actor->BodyItem.c_str());
	char *pp = strtok(Str, sp);
	while (pp)
	{
		SItemID item = g_ItemMgr.CreateItem(atoi(pp));
		if (ItemID_IsValid(item))
		{
			bool res = con.Equip(item);
			CHECK(res);
		}
		pp = strtok(NULL, sp);
	}
	user.items = con.SaveToString();
	user.pet = con.SavePetBagToString();
	user.money = 0;

	switch(faction)
	{
	case 0:
		user.scene = actor->zhouScene;
		user.attr.x = actor->zhouPosition[0];
		user.attr.y = actor->zhouPosition[1];
		break;

	case 1:
		user.scene = actor->shangScene;
		user.attr.x = actor->shangPosition[0];
		user.attr.y = actor->shangPosition[1];
		break;

	case 3:
		/*Tianh*09.12.2*这里开始取消阵营的设定，默认开始都是中立*/
		user.scene = actor->neutralscene;
		user.attr.x = actor->neutralPosition[0];
		user.attr.y = actor->neutralPosition[1];
		break;

	default:
		return gws_create_user_error;
	}

	if (user.scene.empty())
		return gws_create_user_error;
		
	user.accountId = accountID;
	user.name = name;
	user.uname = name;
	strlwr((char*)user.uname.c_str());

	user.isModified = false;
	user.updateTimeStamp = time(NULL);

	// user.scene是企划填表的，这里不再检查
	user.regionID = g_server.m_sceneTable[user.scene];

	GWCreateUserResult result = CreateUserData(&user);
	if (result != gws_create_user_okay)
		return result;

	*outputData = user;
	return gws_create_user_okay;
}

void GWDatabase::Stop()
{
	m_cmdLock.Enter();
	m_shutdown = true;
	m_cmdLock.Leave();

	LOG("Stop the database command queue\n");
	m_commandQueue.Stop();
	// Replace Louis.Huang @ Tue 16 Dec 2008
//    Join();
	//this->Terminate(0);
	//去除强制退出的调用避免与OnRun出现竞争条件引发崩溃
	LOG("Stop the database result queue\n");
	m_resultQueue.Stop();
}

/*
void GWDatabase::SaveUserData()
{
	int now = time(NULL);
	int updateGate = now - m_saveUserDataInterval;

	GWUserTable::iterator user;
	
	if (m_saveUserDataIterator != -1)
	{
		user = m_users.find(m_saveUserDataIterator);

		if (user == m_users.end())
			user = m_users.begin();
	}
	else
	{
		if (now - m_saveUserDataTime < m_saveUserDataInterval)
			return;

		user = m_users.begin();
	}

	for (; user != m_users.end(); user++)
	{
		if (user->second.isModified)
		{
			if (user->second.updateTimeStamp < updateGate)
			{
				if (m_commandQueue.IsFull(10))
				{
					m_saveUserDataIterator = user->second.userID;
					return;
				}
				// SaveUserData(&user->second, true, NULL);				
				SaveUserData(&user->second, false, NULL);				
			}
		}
	}

	m_saveUserDataIterator = -1;
	m_saveUserDataTime = now;
}
*/

GWUser* GWDatabase::LoadUserData(DWORD accountID, int userID)
{	
	GWDBCommand cmd;
	
	/*
	GWUserTable::iterator iter = m_users.find(userID);
	if (iter != m_users.end())
		return &iter->second;
	*/
	GWUser *data = RT_NEW GWUser;
	rt2_sprintf(cmd.command, "SELECT "
		"user.ID,"
		"user.Name,"
		"user.AccountID,"
		"user.Scene,"
		"user.X,"
		"user.Y,"
		"user.HP,"
		"user.MP,"
		"user.EXP,"
		"user.AP,"
		"user.Level,"
		"user.Metier,"
		"user.MetierLevel,"
		"user.SP,"
		"user.Str,"
		"user.Con,"
		"user.Dex,"
		"user.Int,"
		"user.Hit,"
		"user.Skills,"
		"user.Items,"		
		"community.Friend,"
		"community.GuildID,"
		"user.ActorID,"
		"user.Faction,"
		"user.PK,"
		"user.Shortcut,"
		"user.Task,"
		"user.HeadModelID,"
		"user.Credit,"
		"user.OnlineTime,"
        "user.Config,"
		"user.Gold,"
		"UNIX_TIMESTAMP(user.LastDelUnionTime),"
		"user.ClearCnt,"
		"user.DungeonTID,"
		"user.DungeonUID,"
		"user.dungeonQuit,"
		"user.DungeonTask,"
		"user.InWarfield,"
		"user.WarfieldQuit,"
		"user.UsData,"
		"UNIX_TIMESTAMP(user.LastRemoveMPTime),"
		"user.WithdrawMPGift,"
		"UNIX_TIMESTAMP(LastUpdateDate),"
		"user.IsOnline,"
		
		/***Tianh**2009.11.22**/
		"user.c_Cul,"
		"user.c_Soul,"
		"user.c_Contribute,"
		"user.c_Point,"
		"user.c_Record,"
		"user.c_UnionRecord,"
		"user.c_JobRecord,"
		"user.c_BornTag,"
		"user.Sex,"
		
		//rendh 2010.01.23
		"user.LineSysGroupID,"
		"user.LineSysLineID,"
		//rendh 2010.01.23
		
		"user.GradeLev," 
		"user.Vigour,"
	    "user.VigourPoint,"
		"user.nRelivePostion,"
		"user.PlayerOnlineTime,"
		"user.CulExp,"
		"user.Exploit,"
		"user.HeadImageID,"
		//"user.EnP,"
		"user.petItemID,"
		"user.Popularity,"
		"user.isFirstLogin,"
		"user.IsDead,"
		"user.EffectSkill,"
		"user.ChangeModelID,"
		/*"user.UpdateSCDTime,"*/
	
		"user.MyData,"
		"user.TeamFree"
		",user.deleted"
		" FROM user,community WHERE user.ID = %d AND community.UserID = %d LIMIT 1 ",
		userID, userID);
	
	MYSQL_RES* res;
	if (!ExecCommand(cmd.command, &res))
		return NULL;

	int count = mysql_num_rows(res);
	if (count == 0)
		return NULL;

    std::string szAccountName;
	string strAttribAssert;
	MYSQL_ROW row = mysql_fetch_row(res);
	unsigned long* rowLen = mysql_fetch_lengths(res);

	//PZH
	try
	{
	int nDel = atol(row[73]);
	if(0 != nDel)
	{
		return NULL;//改角色被冻结不能进入游戏
	}
	//

	data->userID					= userID;
	data->name						= row[1];
	data->uname						= strlwr(row[1]);
	szAccountName					= row[2];
	data->scene						= row[3];
	data->attr.x					= atol(row[4]);
	data->attr.y					= atol(row[5]);
	data->attr.hp					= atol(row[6]);
	data->attr.mp					= atol(row[7]);
	data->attr.exp					= atol(row[8]);
	data->attr.ap					= atol(row[9]);
	data->attr.level				= atol(row[10]);
	data->attr.metier				= atol(row[11]);
	data->attr.metierLevel			= atol(row[12]);
	data->attr.sp					= atol(row[13]);
	data->attr.strength				= atol(row[14]);
	data->attr.constitution			= atol(row[15]);
	data->attr.dexterity			= atol(row[16]);
	data->attr.intellect			= atol(row[17]);
	data->attr.hit					= atol(row[18]);
	data->strSkills					= row[19];
	data->items						= row[20];
	data->friends					= row[21];
	data->unionID					= atol(row[22]);
    data->attr.actorID				= atol(row[23]);	
    data->attr.faction				= atol(row[24]);
    data->attr.pk					= atol(row[25]);
    if (row[26])	
        data->shortcut				= row[26];
    if (row[27])
        data->task					= row[27];
	data->headModelID				= atol(row[28]);
	data->attr.credit				= atol(row[29]);
	data->onlineTime				= atol(row[30]);
    if (row[31])
        data->configData			= row[31];
	data->money						= atol(row[32]);
	if (row[33])
		data->dwLastDelUnion		= atol(row[33]);
	else 
        data->dwLastDelUnion		= 0;
	data->attr.spClearCnt			= atol(row[34]);
    data->dungeonTID                = atol(row[35]);
    data->dungeonUID                = atol(row[36]);
    data->dungeonLastQuit           = atol(row[37]);
	if(row[38])
		data->fbTask                = row[38];
	data->IsInWarfield				= bool(atol(row[39]));
	data->lWarfieldLastQuit			= atol(row[40]);
	if (row[41])
		strAttribAssert				= row[41];
	data->dwLastRemoveMPTime		= atol(row[42]);
	data->dwWithdrawMPGift			= atol(row[43]);
	data->lastUpdateTime            = atol(row[44]);
	data->IsOnline					= bool(atol(row[45]));
	/**********Tianh*****2009.11.22*********/
	data->attr.Cul                  = atol(row[46]);
	data->attr.Soul                 = atol(row[47]);
	data->attr.Contribute           = atol(row[48]);
	data->attr.Point                = atol(row[49]);
	data->attr.Record               = atol(row[50]);
	data->attr.UnionRecord          = atol(row[51]);
	data->attr.JobRecord            = atol(row[52]);
	data->attr.BornTag              = atol(row[53]);
	data->attr.Sex                  = atol(row[54]);
	//rendh~
	data->LineSYSGroupID            = atol(row[55]);
	data->LineSYSLineID             = atol(row[56]);

	data->attr.GradeLev             = atol(row[57]);
	data->attr.Vigour               = atol(row[58]);
	data->attr.VigourPoint          = atol(row[59]);
	data->attr.RelivePostion        = atol(row[60]);
	data->attr.PlayerOnlineTime     = atol(row[61]);
	data->attr.CulExp               = atol(row[62]);
	data->attr.exploit              = atol(row[63]);    
	data->attr.HeadImageID          = atol(row[64]);
	//data->attr.EnP                  = atol(row[65]);
	data->petItemID                 = atol(row[65]);
	data->popularity                = row[66];
	data->isFirstLogin              = atol(row[67]);
	data->isDead                    = atol(row[68]);
	data->strEffects                = row[69];
	data->ChangeModelID             = atol(row[70]);
	//data->UpdateSCDTime             = atol(row[72]);
	/***************************************/
	if(row[71])
		data->extraData.SetData(row[71],rowLen[71]);
	
	int nTeamFree = atol(row[72]);
	if(0 == nTeamFree)
	{
		data->bTeamFree = false;
	}
	else
	{
		data->bTeamFree = true;
	}

	data->isModified = false;
	data->bankModified = false;
	data->updateTimeStamp = time(NULL);

    if (data->dungeonUID==0 && !data->IsInWarfield && data->LineSYSGroupID == 0)
    {
	    if(!g_server.IsValidScene(data->scene.c_str()))
	    {
		    ERR1("error scene %s\n",data->scene.c_str());
		    g_server.GetDefaultPosByFaction(data->attr.faction,data->scene,data->attr.x,
			    data->attr.y);
	    }
    }
	data->regionID = g_server.m_sceneTable[data->scene];

	// 设置状态
	data->dwState = 0xffffffff;
	GWServer::SUserForbid *pForbid = NULL;
	pForbid = g_server.FindUserForbidden(data->userID, CreState_CanSpeak);
	if (pForbid)
		data->dwState &= ~CreState_CanSpeak;
	pForbid = g_server.FindUserForbidden(data->userID, CreState_CanMove);
	if (pForbid)
		data->dwState &= ~CreState_CanMove;
	pForbid = g_server.FindUserForbidden(data->userID, CreState_CanPShop);
	if (pForbid)
		data->dwState &= ~CreState_CanPShop;

	mysql_free_result(res);

	rt2_sprintf(cmd.command, "SELECT Items,BankMoney,Pets,Password FROM T_bank WHERE UserID='%d'", data->userID);
	if (!ExecCommand(cmd.command, &res))
		return NULL;

	count = mysql_num_rows(res);
	if (count == 0)
	{
		CBag bagEmpty(g_ItemMgr.GetBankInitLineCount(), BANK_GRID_COLUMN_COUNT, 1, &g_ItemMgr);
		data->bank = bagEmpty.SaveToString(&g_ItemMgr);
		CBag bankPetbagEmpty(BANK_PET_LINE_COUNT, BANK_PET_COLUMN_COUNT, 1, &g_ItemMgr);
		data->bankPet = bankPetbagEmpty.SaveToString(&g_ItemMgr);
		data->bankMoney = 0;

		// ERR1("First Load User Data[account=%s], Bank reset to empty!!!", szAccountName.c_str());

		rt2_sprintf(cmd.command,
			"INSERT INTO T_bank(UserID,AccountID,Account,Items,BankMoney,Pets,Password) VALUES('%d','%d','%s','%s',%d,'%s','%s')",
			data->userID, accountID, data->name.c_str(), data->bank.c_str(), 0, data->bankPet.c_str(), "");

		if (!g_server.m_database.ExecCommand(cmd.command, &res))
		{
			return NULL;
		}
	}
	else
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		data->bank = row[0];
		data->bankMoney = atoi(row[1]);
		data->bankPet = row[2];
		data->bankPass = row[3];
		mysql_free_result(res);
	}

	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "SELECT Pets FROM T_pet WHERE UserID = '%d'", data->userID);
	if (!ExecCommand(cmd.command, &res, true))
		return NULL;
	count = mysql_num_rows(res);
	if (count == 0)
	{
		mysql_free_result(res);
		if (data->pet.empty())
		{
			CBag bagEmpty(g_ItemMgr.GetBankInitLineCount(), BANK_GRID_COLUMN_COUNT, 1, &g_ItemMgr);
			data->pet = bagEmpty.SaveToString(&g_ItemMgr);
		}
		rt2_sprintf(cmd.command, "INSERT INTO T_pet(UserID,AccountID,Name,Pets) VALUES('%d','%d','%s','%s')",
			data->userID, accountID, data->name.c_str(), data->pet.c_str());
		if (!g_server.m_database.ExecCommand(cmd.command, &res, true))
		{
			return NULL;
		}
	}else
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		data->pet = row[0];
		mysql_free_result(res);
	}

	// 检查是否被非法修改
	if (!strAttribAssert.empty())
	{
		SUserDataAssert uda;
		memset(&uda, 0, sizeof(SUserDataAssert));
		uda.version = SUserDataAssert_Version;
		uda.userID = data->userID;
		uda.bankMoney = data->bankMoney;
		uda.money = data->money;
		uda.metier = data->attr.metier;
		uda.metierLevel = data->attr.metierLevel;
		uda.faction = data->attr.faction;
		uda.level = data->attr.level;
		uda.exp = data->attr.exp;
		uda.ap = data->attr.ap;
		uda.sp = data->attr.sp;
		uda.hp = data->attr.hp;
		uda.mp = data->attr.mp;
		uda.x = data->attr.x;
		uda.y = data->attr.y;
		uda.constitution = data->attr.constitution;
		uda.strength = data->attr.strength;
		uda.dexterity = data->attr.dexterity;
		uda.intellect = data->attr.intellect;
		uda.hit = data->attr.hit;
		uda.pk = data->attr.pk;
		uda.credit = data->attr.credit;
		uda.spClearCnt = data->attr.spClearCnt;

		/***********Tianh***************/
		uda.Cul = data->attr.Cul;
		uda.CulExp = data->attr.CulExp;
		uda.Soul = data->attr.Soul;
		uda.Contribute = data->attr.Contribute;
		uda.Point =	data->attr.Point;          
		uda.Record = data->attr.Record;            
		uda.UnionRecord = data->attr.UnionRecord  ;       
		uda.JobRecord = data->attr.JobRecord ;           
		uda.BornTag = data->attr.BornTag  ;
		uda.Sex = data->attr.Sex;
		uda.GradeLev = data->attr.GradeLev;
		uda.PlayerOnlineTime = data->attr.PlayerOnlineTime;
		uda.Exploit = data->attr.exploit;
		/*********************************/

		string MD5String;
		SUserDataAssert_CreateMD5String12(uda, MD5String);
		if (MD5String != strAttribAssert)
		{
			char cTmpVer1[32];
			char cTmpVer2[32];
			cTmpVer1[0] = MD5String[0];
			cTmpVer1[1] = MD5String[1];
			cTmpVer1[2] = '\0';
			cTmpVer2[0] = strAttribAssert[0];
			cTmpVer2[1] = strAttribAssert[1];
			cTmpVer2[2] = '\0';
			if (strcmp(cTmpVer1, cTmpVer2)==0)
			{
				// Log to LogServer
				char cTmp1024[1024];
				rt2_sprintf(cTmp1024, "GW=%s,UserName=%s,UserID=%d", g_cfgGW.game.szGameworldName.c_str(), data->name.c_str(), data->userID);
				g_server.LogServerSender_Send("UserAssert", cTmp1024);
			}
		}
	}
	}
	catch(...)
	{
		return NULL;
	}
	return data;
}

bool GWDatabase::DeleteUser(int userID)
{
	GWDBCommand cmd;

    bool succeeded = true;

    // 更新删除信息
    rt2_sprintf(cmd.command, "UPDATE `user` SET `deleted`=1, `DeleteDate`=NOW() WHERE `ID`='%d'", userID);
    if (!ExecCommand(cmd.command, NULL))
        return false;

    /*
    rt2_sprintf(cmd.command,
        "INSERT `deleted_user` (`ID`,`UniqueName`,`Name`,`ActorID`,`Faction`,`AccountID`,`Scene`,`HP`,`MP`,`EXP`,`AP`,`Level`,`DeleteDate`,`CreateDate`,`LastUpdateDate`,`Metier`,`MetierLevel`,`X`,`Y`,`Gold`,`SP`,`Str`,`Con`,`Dex`,`Int`,`Hit`,`Skills`,`Items`,`Shortcut`,`PK`,`Task`,`Score`,`HeadModelID`,`Credit`,`OnlineTime`,`usertype`,`delete_time`,`Friend`,`GuildID`)"
        "SELECT `ID`,`UniqueName`,`Name`,`ActorID`,`Faction`,`Account`,`Scene`,`HP`,`MP`,`EXP`,`AP`,`Level`,`DeleteDate`,`CreateDate`,`LastUpdateDate`,`Metier`,`MetierLevel`,`X`,`Y`,`Gold`,`SP`,`Str`,`Con`,`Dex`,`Int`,`Hit`,`Skills`,`Items`,`Shortcut`,`PK`,`Task`,`Score`,`HeadModelID`,`Credit`,`OnlineTime`,`usertype`,NOW(),`Friend`,`GuildID`"
        "FROM `user` LEFT JOIN `community` ON `ID`=`UserID`"
        "WHERE `ID`=%d", userID);
    if (!ExecCommand(cmd.command, NULL))
        return false;

    // 删除角色
	rt2_sprintf(cmd.command, "DELETE FROM user WHERE ID=%d", userID);
	if (!ExecCommand(cmd.command, NULL))
		return false;

    // 删除好友信息
	rt2_sprintf(cmd.command, "DELETE FROM community WHERE UserID=%d", userID);
	if (!ExecCommand(cmd.command, NULL))
		return false;

    // 删除邮件
	rt2_sprintf(cmd.command, "DELETE FROM mail WHERE ReceiverID='%d'", userID);
	if (!ExecCommand(cmd.command, NULL))
		return false;
    */
	
	g_server.OnUserDelete(userID);	
	
	return succeeded;
}

unsigned long GWDatabase::GetInsertId()
{
	return (unsigned long)mysql_insert_id(m_mysql);
}

bool GWDatabase::RestoreUser(int userID)
{
	GWDBCommand cmd;

	rt2_sprintf(cmd.command, "UPDATE user SET DeleteDate=NULL WHERE ID=%d", userID);

	if (!ExecCommand(cmd.command, NULL))
		return false;

	int numRows = mysql_affected_rows(m_mysql);

	if (numRows == 0)
		return false;

	return true;
}

GWActor* GWDatabase::LookupActor(int id)
{
	stlforeach(GWActorTable, actor, m_actors)
		if (actor->attr.actorID == id)
			return &*actor;

	return NULL;
}

//void GWDatabase::SendPlayerInfo()
//{
//
//}

void GWDatabase::LoadActorTable()
{
	SUserActor tmp;
	int i, j;
    char*p = 0;
	char *pd = 0;
	char const*sp = ";, \t";
	RtCsv csv;

	if(!csv.LoadTableFile(R(RES_TABLE_USER)))
	{
		LOG("Load actor table [sv_table/user_actor.csv] failed\n");
		return;
	}

	j = csv.FindLineByString("ID");
		
	if(j < 0)
		return;
	
	csv.SetTitleLine(j);

	int total = csv.GetLineCount();

	for(j++; j < total ; j++)
	{
		GWActor tmp;

		memset(&tmp.attr, 0, sizeof(tmp.attr));

		tmp.attr.level			= 1;
		tmp.attr.actorID		= csv[j]["ID"].Long();
		tmp.attr.metier			= csv[j]["职业"].Long();
		tmp.attr.metierLevel	= csv[j]["转职次数"].Long();		
		tmp.attr.strength		= csv[j]["力量"].Short();
		tmp.attr.dexterity		= csv[j]["敏捷"].Short();
		tmp.attr.hit			= csv[j]["精准"].Short();
		tmp.attr.constitution	= csv[j]["体质"].Short();
		tmp.attr.intellect		= csv[j]["智力"].Short();	
		tmp.zhouScene			= csv[j]["周出生场景"].Str();
		tmp.shangScene			= csv[j]["商出生场景"].Str();
		/*Tianh 09.12.03*/
		tmp.neutralscene        = csv[j]["中立出生场景"].Str();

		tmp.zhouCreate          = csv[j]["周可创建"].Long();
		tmp.shangCreate         = csv[j]["商可创建"].Long();
		tmp.neutralCreate       = csv[j]["中立可创建"].Long();
		tmp.attr.Reset();		

		rt2_strcpy(g_strStaticBuffer, csv[j]["周出生坐标"].Str());
		pd = strtok(g_strStaticBuffer, sp);
		if (pd)
		{
			tmp.zhouPosition[0] = atol(pd);
			tmp.zhouPosition[1] = atol(strtok(NULL, sp));
		}

		rt2_strcpy(g_strStaticBuffer, csv[j]["商出生坐标"].Str());
		pd = strtok(g_strStaticBuffer, sp);		
		if (pd)
		{
			tmp.shangPosition[0] = atol(pd);
			tmp.shangPosition[1] = atol(strtok(NULL, sp));
		}
		
		rt2_strcpy(g_strStaticBuffer, csv[j]["中立出生坐标"].Str());
		pd = strtok(g_strStaticBuffer, sp);
		if (pd)
		{
			tmp.neutralPosition[0] = atol(pd);
			tmp.neutralPosition[1] = atol(strtok(NULL, sp));
		}

		pd = strtok(g_strStaticBuffer, sp);

        // 携带道具
		tmp.items = csv[j]["携带道具"].Str();
		tmp.BodyItem = csv[j]["装备道具"].Str();

        // 携带技能(由表格读出保存成存盘格式，注意版本号)
        rt2_strcpy(g_strStaticBuffer, csv[j]["携带技能"].Str());
        g_strStaticBuffer[100] = 0; // 这个字符串最长为100

        p = g_strStaticBuffer+101;
        *p = 0;
        i = 0;
        pd = strtok(g_strStaticBuffer, sp);
        while (pd)
        {
            i ++;
            rt2_sprintf(p+strlen(p), "%d:0 0 0;", atoi(pd));
            pd = strtok(NULL, sp);
            if (pd==NULL) break;
        }
        rt2_sprintf(g_strStaticBuffer, "1;1;%d;", i);
        tmp.skills = g_strStaticBuffer;
        tmp.skills += g_strStaticBuffer+101;
        tmp.skills += "0;-1";

        m_actors.push_back(tmp);
	}
}

bool GWDatabase::IsValidName(const string& name)
{
	/*
	if (name.length() < GW_MIN_NAME_LENGTH || name.length() > GW_MAX_NAME_LENGTH)
	{
		LOG1("Check user name %s failed: the length of name is too short/long\n", name.c_str());
		return false;
	}
	*/

	if (name.length() <= 0 || name.length() > 16)
	{
		LOG1("Check user name %s failed: the length of name is too short/long\n", name.c_str());
		return false;
	}

	const char* p = name.c_str();
	const char* stop = p + name.length();

	while(p < stop)
	{
		if (*p & 0x80)
		{
			p += 2;
		}
		else
		{
			if (!(*p >= '0' && *p <= '9' ||
				*p >= 'A' && *p <= 'Z' ||
				*p >= 'a' && *p <= 'z' ||
				*p == '_'))
			{
				return false;
			}

			p++;
		}
	}

	return true;
}

void GWDatabase::EscapeString(string& result, const string& str)
{
	result.resize((long)str.size() * 2);
	result.resize(mysql_real_escape_string(m_mysql, &result[0], str.c_str(), (unsigned long)str.length()));
}

void GWDatabase::EscapeString(char* result, const char* str)
{
	mysql_real_escape_string(m_mysql, result, str, (int)strlen(str));
}

const int MAX_ESCAPE_STRING = 1024*16;
std::string GWDatabase::EscapeString(const char *str,int size)
{
	//static char buf[MAX_ESCAPE_STRING];
	S_MEMDEF(buf, MAX_ESCAPE_STRING)
	S_MEMPROTECTOR(buf, MAX_ESCAPE_STRING, bMP)
	unsigned long len;
	if(size==0)
		size = (int)strlen(str);
	len = mysql_real_escape_string(m_mysql,buf,str,(unsigned long)size);
	if(len >= MAX_ESCAPE_STRING) len = 0;
	buf[len] = '\0';
	return buf;
}

bool GWDatabase::Connect()
{
	m_mysql = mysql_init(m_mysql);

	LOG2("Connecting to datatbase server %s on port %d ... \n", 
		g_cfgGW.connectDatabase.ip.c_str(), 
		g_cfgGW.connectDatabase.port);

	mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, "gb2312");

	if (!mysql_real_connect(m_mysql, 
		g_cfgGW.connectDatabase.ip.c_str(), 
		g_cfgGW.connectDatabase.username.c_str(), 
		g_cfgGW.connectDatabase.password.c_str(),
		NULL,
		g_cfgGW.connectDatabase.port,
		NULL, 
		0))
	{
		LOG3("Can't connect to database server %s on port %d\n    MSG= %s\n", 
			g_cfgGW.connectDatabase.ip.c_str(), 
			g_cfgGW.connectDatabase.port,
            mysql_error(m_mysql));

		return false;
	}

	if (mysql_select_db(m_mysql, g_cfgGW.connectDatabase.database.c_str()) < 0)
	{
		LOG2("Can't select database %s\n    MSG= %s\n", g_cfgGW.connectDatabase.database.c_str(), mysql_error(m_mysql));

		return false;
	}

	m_connectTime = 0;

	return true;
}

/*
void GWDatabase::RemoveUserData(int userID)
{
	GWUserTable::iterator iter = m_users.find(userID);

	if (iter == m_users.end())
		return;

	if (m_saveUserDataIterator == userID)
	{
		GWUserTable::iterator t = iter;
		iter++;
		m_users.erase(t);
		if (iter != m_users.end())
			m_saveUserDataIterator = iter->first;
		else
			m_saveUserDataIterator = -1;
	}
	else
	{
		m_users.erase(iter);
	}
}
*/
