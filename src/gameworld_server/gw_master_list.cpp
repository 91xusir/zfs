
#include "gw_server.h"
#include "gw_master_list.h"
extern GWServer                g_server;



void CMasterListManager::ReceiveFromRegion(CG_CmdPacket* vpPacket)		//��Region�õ�һ������
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


void CMasterListManager::Add(DWORD vDBID,const string vMetier,BYTE vLevel,short int vPrenticeSize)			//����һ��
{
	GWUser *pMaster = g_server.FindActorById(vDBID);
	if(pMaster==NULL)
	{//�޷��ҵ�
//		LOG("�޷��ҵ����");
		return;
	}

	CMasterList* pList=NULL;
	if(pMaster->attr.faction == FACTION_SHANG )
	{//�������
		pList=&mShangList;
	}
	else if(pMaster->attr.faction == FACTION_ZHOU)
	{//�������
		pList=&mZhouList;
	}
	//Tianh 09.12.11
	else if(pMaster->attr.faction == FACTION_NEUTRAL)
	{//���������
		pList=&mNeutral;
	}
	else 
	{
//		LOG("�޷�ʶ����Ӫ");
		return;
	}

	if(pList->Overflow())
	{//��ǰ��������//ʧ��//����Ϣ��region
//		LOG("������");
		CG_CmdPacket& rPacket=RegionPack(vDBID);
		rPacket.WriteByte(GW2R_APPLY_FAIL);//���ӵ��б�ʧ��
		rPacket.WriteByte(FAIL_LIST_OVERFLOW);//�����Ѿ���
		SendToRegion(pMaster);
		return;
	}

	if(pList->Exist(vDBID))
	{//������Ѿ�����ʦ���б���//ʧ��
//		LOG("������Ѿ��ڰ���");
		CG_CmdPacket& rPacket=RegionPack(vDBID);
		rPacket.WriteByte(GW2R_APPLY_FAIL);//���ӵ��б�ʧ��
		rPacket.WriteByte(FAIL_MASTER_ALREADY_EXIST);//��ʦ���Ѿ������б���
		SendToRegion(pMaster);
		return;
	}

	//һ��OK���һ����
	SMasterInfo Temp(	vDBID,
		pMaster->attr.faction,
		pMaster->name,
		vMetier,
		vLevel,
		vPrenticeSize);

	(*pList) += Temp;

	//���ɹ���Ϣ����region
	//LOG("���һ���˵�����");
	CG_CmdPacket& rPacket=RegionPack(vDBID);
	rPacket.WriteByte(GW2R_APPLY_SUCCESS);//���ӵ��б�ɹ�
	SendToRegion(pMaster);

	//���б���Ϣ���͵�ÿ��region
	rPacket=RegionPack();
	rPacket.WriteByte(GW2R_LIST_ADD);//����һ��ʦ��
	rPacket << Temp;
	Broadcast();
}

void CMasterListManager::Remove(GWUser *vpUser)		//����һ��
{
	if(vpUser==NULL)
	{
		return;
	}


	//�õ�Ҫ�ӵ��ĸ�ʦ���б���
	CMasterList* pList=NULL;

	if(vpUser->attr.faction == FACTION_SHANG )
	{//�������
		pList=&mShangList;
	}
	else if(vpUser->attr.faction == FACTION_ZHOU)
	{
		pList=&mZhouList;
	}
	//Tianh 09.12.11
	else if(vpUser->attr.faction == FACTION_NEUTRAL)
	{//���������
		pList=&mNeutral;
	}

	if(!pList->Exist(vpUser->userID))
	{//�����ڲ���Ҫ���б����Ƴ�
		return;
	}

	(*pList) -= vpUser->userID;

	//���б���Ϣ���͵�ÿ��region
	CG_CmdPacket& rPacket=RegionPack();
	rPacket.WriteByte(GW2R_LIST_DEL);					//����һ��ʦ��
	rPacket.WriteByte((char)vpUser->attr.faction);		//��Ӫ
	rPacket.WriteLong(vpUser->userID);					//ʦ����DBID
	Broadcast();
}


CG_CmdPacket& CMasterListManager::RegionPack(DWORD vSendToDBID)//0��ʾ������ĳ���ض������
{
	CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_master_list);
	rPacket.WriteLong(vSendToDBID);
	return rPacket;
}


void CMasterListManager::SendToRegion(GWUser* vpSendToUser)					//��ʦ���б���Region�е�ĳ�����
{
	int Session= g_server.GetRegionSession(vpSendToUser->regionID);
	if(Session==-1)
	{
		return;
	}

	g_server.m_region.SendToPeer(Session);
	return ;
}

void CMasterListManager::Broadcast()//�㲥������region
{
	g_server.m_region.BroadcastCmd();
}




