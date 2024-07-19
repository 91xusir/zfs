
#include "region_mail.h"

using std::string;




//#define GET_PARAMS() CmdGiver = g_region->m_cmdGiver;
#define SAFE_READ(p) if(!(p)) return CMD_ERROR_READ_DATA;

void CServerMail::SendToClient(long MailID,DWORD vFromDBID,string& vrRecievedDate,string& vrFromName,char* vpTitle,char* vpContent,const char* StrItemID,long Money,long IsOpen)
{
	string Str = StrItemID;
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_mail);
	g_sendCmd->WriteByte(R2C_RECEIVE_MAIL);
	g_sendCmd->WriteLong(MailID);
	g_sendCmd->WriteString((char*)vrRecievedDate.c_str());
	g_sendCmd->WriteLong(vFromDBID);
	g_sendCmd->WriteString((char*)vrFromName.c_str());
	g_sendCmd->WriteString(vpTitle);
	g_sendCmd->WriteString(vpContent);
	g_sendCmd->WriteString((char*)Str.c_str());
	g_sendCmd->WriteLong(Money);
	g_sendCmd->WriteLong(IsOpen);
	mpMaster->RecvCmd(g_sendCmd);
}


void CServerMail::DeleteInGameWorld(long MailID)
{
	g_region->m_gws.DeleteMail(MailID);
	return;
}

bool CServerMail::SendToGameWorld(DWORD vDBID,char* vpTitle,char* vpContent,long ItemID,long Money)
{
	/*
	CServerMail* pMail;
	pMail=FindByDBID(vDBID);
	if(pMail!=NULL)
	{
		pMail->ReceiveFromGameWorld(mpFriend->mSelf.mDBID,mpFriend->mSelf.mName,vpTitle,vpContent);
	}
	*/
	SItemID rItem;
	//<add by Tianh>  ������������Ʒ
	string StrItem = "";

	rItem = mpMaster->m_pItemContainer->m_Bag.GetItemFromID(ItemID);
	if (rItem.cBind == IB_Binded)
	{
		rt2_sprintf(g_strStaticBuffer, R(MSG_SEND_BINDED_ITEM));
		mpMaster->SendSystemMessage(g_strStaticBuffer);
		return false;
	}
	if (ItemID_IsValid(rItem) && !ItemCanTrade(rItem))
	{
		rt2_sprintf(g_strStaticBuffer, R(MSG_SEND_BINDED_ITEM));
		mpMaster->SendSystemMessage(g_strStaticBuffer);
		return false;
	}

	CItemManager*  m_pItemManager = mpMaster->m_pItemContainer->GetItemManager();
	//SItemBase*	pItemBase = m_pItemManager->GetItem(rItem.type);
	//if (pItemBase)
	//{	
	//            
	//}

	int nItemCount, page, gridI, gridJ;
	StrItem = rItem.SaveToString(m_pItemManager);
	mpMaster->m_pItemContainer->m_Bag.Find(rItem, &page, &gridI, &gridJ);
	if (Money > 0)
	{
		int bagMoeny = mpMaster->m_pItemContainer->m_Bag.GetMoney();
		if (bagMoeny < Money)
		{
			rt2_sprintf(g_strStaticBuffer, R(MSG_MAIL_NOMONEYTOSEND));
			mpMaster->SendSystemMessage(g_strStaticBuffer);
			return false;
		}
		mpMaster->m_pItemContainer->RemoveMoney(Money);
		CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(-Money, ITEM_CONTAINER_PLAYER_COMMON_BAG, ITEM_REASON_PICK);
		mpMaster->RecvCmd(g_sendCmd);
	}
	if (ItemID >= 0)
	{   
		mpMaster->m_pItemContainer->RemoveItem(rItem); //ɾ�����װ��
		CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ,ITEM_REASON_DELETE);
		mpMaster->RecvCmd(g_sendCmd);
	}


	string Title(vpTitle);
	string Content(vpContent);
	
	{
		rt2_sprintf(g_strStaticBuffer, R(MSG_MAIL_SENDMAILSUCCESS));
		mpMaster->SendSystemMessage(g_strStaticBuffer);
	}
	
	g_region->m_gws.SendMail(vDBID,mpFriend->mSelf.mDBID,mpFriend->mSelf.mName,Title,Content,StrItem,Money);
	return true;

}

//<add by Tian>  �ƶ��ʼ������Ʒ
void  CServerMail::MoveItem(std::string& Item,long UserID,long Money)
{
	const char* str = Item.c_str();
	SItemID rItem;
	string StrName = "";
	CItemManager*  m_pItemManager = mpMaster->m_pItemContainer->GetItemManager();
	if (m_pItemManager)
	{
		rItem.LoadFromString(11,str,m_pItemManager);

		SItemBase*	pItemBase = m_pItemManager->GetItem(rItem.type);
		if (pItemBase)
		{	
			   StrName =  pItemBase->name;   
		}
	}

		CRegionUser* pUser = g_region->FindUser(UserID);
		if (pUser)
		{	
			if (Money > 0)
			{
				pUser->m_dummy->m_pItemContainer->AddMoney(Money);

				rt2_sprintf(g_strStaticBuffer, R(MSG_ADD_MAIL_MONEY), Money);
				pUser->m_dummy->SendSystemMessage(g_strStaticBuffer);

				CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(Money, ITEM_CONTAINER_PLAYER_COMMON_BAG, ITEM_REASON_PICK);
				pUser->m_dummy->RecvCmd(g_sendCmd);
			}	
			if (rItem.id != 0)
			{
			    pUser->m_dummy->m_pItemContainer->AddItem(rItem);
			    pUser->m_dummy->m_pItemContainer->UpdateItemToClient(ITEM_CONTAINER_PLAYER_COMMON_BAG, rItem.id);

			    rt2_sprintf(g_strStaticBuffer, R(MSG_ADD_MAIL_ITEM), StrName.c_str());
			    pUser->m_dummy->SendSystemMessage(g_strStaticBuffer);
			}
		}
}

void  CServerMail::IsOpenMail(short IsOpen,long MailID)
{
	
	g_region->m_gws.SendIsOpenMail(IsOpen,MailID);
}

bool CServerMail::MoveItemSendtoGw(long MailID)
{
	//���������   ��ִ�д˲���
	int FreeSpace = mpMaster->m_pItemContainer->m_Bag.FreeSpace();
	if (FreeSpace <= 0)
	{
		rt2_sprintf(g_strStaticBuffer, R(MSG_CLEAR_BAG_ITEM));
		mpMaster->SendSystemMessage(g_strStaticBuffer);
		return false;
	}
	//if ()
	//{
	//}
	g_region->m_gws.SendMailItem(MailID);
	return true;
}

CServerMail*	CServerMail::FindByDBID(DWORD vDBID)
{
	CRegionUser* pUser=g_region->FindUser(vDBID);
	if(pUser==NULL)
	{//�������Ҳ���
		//mpFriend->SendToClient(R2C_FRIEND_MESSAGE,OBJECT_NOT_FIND);
	}
	else
	{//�ҵ�
		return pUser->m_dummy->m_pMail;
	}
	return NULL;
}


int	CServerMail::ReceiveFromClient(CRegionCreature* CmdGiver,CG_CmdPacket *cmd)
{
	char    Operation;
	long	DBID;
	char*	pTitle;
	char*   pContent;
	char*	pDate;
	long    ItemID;
	long    Money;
	long    MailID;

	SAFE_READ(cmd->ReadByte(&Operation));


	if(Operation==C2R_SEND_MAIL)
	{
		SAFE_READ(cmd->ReadLong(&ItemID));
		SAFE_READ(cmd->ReadLong(&Money));
		SAFE_READ(cmd->ReadLong(&DBID));
		SAFE_READ(cmd->ReadString(&pTitle));
		SAFE_READ(cmd->ReadString(&pContent));
		

		if(strlen(pTitle)>MAIL_TITLE_LIMIT_COUNT)
		{//�������Ƚص�
			pTitle[MAIL_TITLE_LIMIT_COUNT]='\0';
		}

		if(strlen(pContent)>MAIL_CONTENT_LIMIT_COUNT)
		{//�������Ƚص�
			pContent[MAIL_CONTENT_LIMIT_COUNT]='\0';
		}

		SendToGameWorld(DBID,pTitle,pContent,ItemID,Money);

		return CMD_ERROR_NONE;
	}
	else if (Operation==C2R_MOVE_ITEM)
	{
		SAFE_READ(cmd->ReadLong(&MailID));
		MoveItemSendtoGw(MailID);
		return CMD_ERROR_NONE;
	}
	else if (Operation==C2R_SETMAIL_READ)
	{
		SAFE_READ(cmd->ReadLong(&MailID));
		short IsOpen = 1;
		IsOpenMail(IsOpen,MailID);
	}
	else if(Operation==C2R_DELETE_MAIL)
	{
		SAFE_READ(cmd->ReadLong(&MailID));
		//if(strlen(pDate)>MAIL_DATE_LENGTH)
		//{//�������Ƚص�
		//	pDate[MAIL_DATE_LENGTH]='\0';
		//}
		//string Date(pDate);
		////Date=Date.substr(0,MAIL_DATE_LENGTH);//��ʵ���ﲻ�Ǻ�Ӧ���ж��ַ�����//��Ϊ������ַ����ľ��峤�Ȳ�������������֪���ģ�Ӧ����game_world�ϣ�����ȷ
		DeleteInGameWorld(MailID);
		return CMD_ERROR_NONE;
	}
	else
	{
		return mpFriend->ReceiveFromClient(Operation,cmd);
	}
}

////////////////////////////
int  CServerMail::check(char *buf,int len)
{
	int i;
	char tmp[1024];

	memcpy( tmp,buf,len );
	for( i=0;i<len;i++ ){
		if( ( tmp[i]>0x39 || tmp[i]<0x30 ) && ( tmp[i] != '.' ) )
			return 1;
	}
	return 0;
}

void	CServerMail::ReceiveFromGameWorld(long MailID,DWORD vFromDBID,string& vrReceivedDate,string& vrFromName,char* vpTitle,char* vpContent,const char*  StrItemID,long Money,long IsOpen)
{
	SItemID rItem;
	CItemManager*  m_pItemManager = mpMaster->m_pItemContainer->GetItemManager();

	if (m_pItemManager)
	{
		rItem.LoadFromString(11,StrItemID,m_pItemManager);
	}
	
	//std::list<SItemID>::iterator it = m_listItem.begin(); 
	//for (; it!=m_listItem.end(); it++)
	//{
	//	if ((*it).id != rItem.id)
	//	{
	//		m_listItem.push_back(rItem);
	//	}
	//}
	// //���ʼ����е���Ʒ��������   �ǵ�ɾ�����ʼ���ʱ���Ƴ�
	
	SendToClient(MailID,vFromDBID,vrReceivedDate,vrFromName,vpTitle,vpContent,StrItemID,Money,IsOpen);
}

