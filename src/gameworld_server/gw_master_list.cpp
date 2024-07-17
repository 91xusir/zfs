
#include "gw_server.h"
#include "gw_master_list.h"
extern GWServer                g_server;



void CMasterListManager::ReceiveFromRegion(CG_CmdPacket* vpPacket)		//从Region得到一个命令
{
	//LOG("CMasterListManager::ReceiveFromRegion");
	char	Operation=0;
	vpPacket->ReadByte(&Operation );
	switch(Operation)
	{
	case R2GW_APPLY_ADD_LIST:
		{
//			LOG("R2GW_APPLY_ADD_LIST");
			long			DBID;
			char*			pMetier;
			char			Level;
			short int		PrenticeSize;

			vpPacket->ReadLong( &DBID );
			vpPacket->ReadString(&pMetier);
			vpPacket->ReadByte(&Level);
			vpPacket->ReadShort(&PrenticeSize);

			Add(DBID,pMetier,Level,PrenticeSize);
		}

		break;
	case R2GW_SET_MAX_COUNT:
		{
//			LOG("R2GW_SET_MAX_COUNT");
			long MaxCount=0;
			vpPacket->ReadLong(&MaxCount);
			mShangList.SetMaxCount(MaxCount);
			mZhouList.SetMaxCount(MaxCount);
			//Tianh  09.12.11
			mNeutral.SetMaxCount(MaxCount);
		}
		break;
	default:
		break;
	}
	return;
}


void CMasterListManager::Add(DWORD vDBID,const string vMetier,BYTE vLevel,short int vPrenticeSize)			//增加一个
{
	GWUser *pMaster = g_server.FindActorById(vDBID);
	if(pMaster==NULL)
	{//无法找到
//		LOG("无法找到玩家");
		return;
	}

	CMasterList* pList=NULL;
	if(pMaster->attr.faction == FACTION_SHANG )
	{//如果是商
		pList=&mShangList;
	}
	else if(pMaster->attr.faction == FACTION_ZHOU)
	{//如果是周
		pList=&mZhouList;
	}
	//Tianh 09.12.11
	else if(pMaster->attr.faction == FACTION_NEUTRAL)
	{//如果是中立
		pList=&mNeutral;
	}
	else 
	{
//		LOG("无法识别阵营");
		return;
	}

	if(pList->Overflow())
	{//当前人数已满//失败//发消息到region
//		LOG("人数满");
		CG_CmdPacket& rPacket=RegionPack(vDBID);
		rPacket.WriteByte(GW2R_APPLY_FAIL);//增加到列表失败
		rPacket.WriteByte(FAIL_LIST_OVERFLOW);//人数已经满
		SendToRegion(pMaster);
		return;
	}

	if(pList->Exist(vDBID))
	{//这个人已经存在师傅列表中//失败
//		LOG("这个人已经在板上");
		CG_CmdPacket& rPacket=RegionPack(vDBID);
		rPacket.WriteByte(GW2R_APPLY_FAIL);//增加到列表失败
		rPacket.WriteByte(FAIL_MASTER_ALREADY_EXIST);//该师傅已经存在列表中
		SendToRegion(pMaster);
		return;
	}

	//一切OK添加一个人
	SMasterInfo Temp(	vDBID,
		pMaster->attr.faction,
		pMaster->name,
		vMetier,
		vLevel,
		vPrenticeSize);

	(*pList) += Temp;

	//将成功消息发到region
	//LOG("添加一个人到板上");
	CG_CmdPacket& rPacket=RegionPack(vDBID);
	rPacket.WriteByte(GW2R_APPLY_SUCCESS);//增加到列表成功
	SendToRegion(pMaster);

	//将列表信息发送到每个region
	rPacket=RegionPack();
	rPacket.WriteByte(GW2R_LIST_ADD);//增加一个师傅
	rPacket << Temp;
	Broadcast();
}

void CMasterListManager::Remove(GWUser *vpUser)		//减少一个
{
	if(vpUser==NULL)
	{
		return;
	}


	//得到要加到哪个师傅列表中
	CMasterList* pList=NULL;

	if(vpUser->attr.faction == FACTION_SHANG )
	{//如果是商
		pList=&mShangList;
	}
	else if(vpUser->attr.faction == FACTION_ZHOU)
	{
		pList=&mZhouList;
	}
	//Tianh 09.12.11
	else if(vpUser->attr.faction == FACTION_NEUTRAL)
	{//如果是中立
		pList=&mNeutral;
	}

	if(!pList->Exist(vpUser->userID))
	{//不存在不需要从列表中移除
		return;
	}

	(*pList) -= vpUser->userID;

	//将列表信息发送到每个region
	CG_CmdPacket& rPacket=RegionPack();
	rPacket.WriteByte(GW2R_LIST_DEL);					//减少一个师傅
	rPacket.WriteByte((char)vpUser->attr.faction);		//阵营
	rPacket.WriteLong(vpUser->userID);					//师傅的DBID
	Broadcast();
}


CG_CmdPacket& CMasterListManager::RegionPack(DWORD vSendToDBID)//0表示不发给某个特定的玩家
{
	CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_master_list);
	rPacket.WriteLong(vSendToDBID);
	return rPacket;
}


void CMasterListManager::SendToRegion(GWUser* vpSendToUser)					//将师傅列表发给Region中的某个玩家
{
	int Session= g_server.GetRegionSession(vpSendToUser->regionID);
	if(Session==-1)
	{
		return;
	}

	g_server.m_region.SendToPeer(Session);
	return ;
}

void CMasterListManager::Broadcast()//广播到所有region
{
	g_server.m_region.BroadcastCmd();
}




