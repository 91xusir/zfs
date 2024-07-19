

#include "team_data.h"

STeamData CTeamData::mNullTeamData;

CTeamData::CTeamData()
:mMemberCount(0), m_PickType(eFreePick)
{
}

CTeamData::~CTeamData()
{
}

bool CTeamData::TransferCaptain(DWORD vMemberID)
{
	if(Size() > 1)
	{
		for(int i=0;i<mMemberCount;i++)
		{
			if(mMember[i].mDBID ==vMemberID)
			{
				STeamData temp = mMember[i];
				mMember[i] = mMember[0];
				mMember[0] = temp;
				return true;
			}
		}
	}
	return false;
}
const char *CTeamData::GetCaptainName()
{
	if(Size()!=0)
	{
		return mMember[0].mName.c_str();
	}
	return NULL;
}
DWORD CTeamData::GetCaptain()
{
	guard;
	if(Size()!=0)
	{
		return mMember[0].mDBID ;
	}
	else
	{
		return 0;
	}
	unguard;
}

bool CTeamData::Exist(DWORD vMemberID)			//某个队员是否存在 
{
	guard;
	for(int i=0;i<mMemberCount;i++)
	{
		if(mMember[i].mDBID ==vMemberID)
		{
			return true;
		}
	}
	return false;
	unguard;
}

bool CTeamData::Exist(std::string& vrMemberName)		//某个队员是否存在
{
	guard;
	for(int i=0;i<mMemberCount;i++)
	{
		if(mMember[i].mName ==vrMemberName)
		{
			return true;
		}
	}
	return false;
	unguard;
}


bool CTeamData::Exist(const char* vpMemberName)		//某个队员是否存在
{
	guard;
	for(int i=0;i<mMemberCount;i++)
	{
		if(strcmp(mMember[i].mName.c_str(),vpMemberName)==0)
		{
			return true;
		}
	}
	return false;
	unguard;
}



bool CTeamData::DeleteMember(DWORD vMemberID)
{
	guard;
	bool Exist=false;
    int i = 0;
	int nDel = 0xffff;
	for(i=0;i<mMemberCount;i++)
	{
		if(mMember[i].mDBID==vMemberID)
		{
			mMember[i].mDBID =0;
			mMember[i].mHp=0;
			mMember[i].mMaxHp=0;
			mMember[i].mMp=0;
			mMember[i].X=0;
			mMember[i].Y=0;
			mMember[i].mMaxMp=0;
			mMember[i].mName = ""; //玩家名字
			mMember[i].mSchool =0; //职业
			mMember[i].mRegion = ""; //地图名字
			mMember[i].mCombatPower = 0; //战力评估
			mMember[i].mAnimal = 0; //生肖
			mMember[i].mActorID = 0;//模型ID

			Exist=true;
			nDel = i;
			break;
		}
	}

	for(;i<mMemberCount-1;i++)
	{
		mMember[i]=mMember[i+1];
	}
	if(Exist)
	{
		mMemberCount--;
	}
	return Exist;
	unguard;
}

void CTeamData::PickOK()
{
	
}

bool CTeamData::SetPickType(TeamPickType type)
{
	bool bRet = false;
	do 
	{
		if (type == m_PickType)
		{
			break;
		}
		m_PickType = type;
		bRet = true;
	} while (false);
	return bRet;
}

bool CTeamData::AddMember(DWORD vMemberID,std::string& vrMemberName,char MemberHead,char  Metier,long BornTag,long  actorID)
{
	guard;
	if(mMemberCount>=MAX_TEAM_MENBER_COUNT)
	{
		return false;
	}

	//这里只在单个Rigion上查找组队玩家坐标
	//CRegionUser *pRegionUser = g_region->FindUser(vMemberID);
	//if (pRegionUser)
	//{
	//	if (pRegionUser->m_dummy)
	//	{
	//		x = pRegionUser->m_dummy->m_pos[0];
	//		y = pRegionUser->m_dummy->m_pos[1];
	//	}	
	//}

	mMember[mMemberCount].mDBID =vMemberID;
	mMember[mMemberCount].mHp=0;
	mMember[mMemberCount].mMaxHp=0;
	mMember[mMemberCount].mMp=0;
	mMember[mMemberCount].mMaxMp=0;
	mMember[mMemberCount].mName=vrMemberName;
	mMember[mMemberCount].mHead=MemberHead;
	mMember[mMemberCount].X = 0;
	mMember[mMemberCount].Y = 0;
	mMember[mMemberCount].mRegion = ""; //地图名字
	mMember[mMemberCount].mCombatPower = 0; //战力评估

	mMember[mMemberCount].mSchool = Metier; //职业
	mMember[mMemberCount].mAnimal = BornTag; //生肖
	mMember[mMemberCount].mActorID = actorID;//模型ID

	mMemberCount++;
	return true;
	unguard;
}

bool CTeamData::Empty()
{
	guard;
	if(mMemberCount==0)
	{
		return true;
	}
	return false;
	unguard;
}

