

#ifndef TEAM_DATA_H 
#define TEAM_DATA_H

#include "core/rt2_core.h"
const int	MAX_TEAM_MENBER_COUNT=5;
#include <string>
#include "game_string_assistant.h"

struct STeamData
{
	STeamData():mDBID(0),mHp(0),mMaxHp(0),mMp(0),mMaxMp(0),mHead(0),mLevel(0),mSchool(0),mName(),X(0),Y(0),mCombatPower(0),mAnimal(0),mActorID(0){};

	~STeamData(){};

	DWORD			mDBID;
	int				mHp;
	int				mMaxHp;
	int				mMp;
	int				mMaxMp;
	char			mHead;  //头像
	char			mLevel;
	long            X;    //组队玩家坐标
	long            Y;
	std::string		mName; //玩家名字
	int             mSchool; //职业
	std::string     mRegion; //地图名字
	long			mCombatPower; //战力评估
	long			mAnimal; //生肖
	long			mActorID;//模型ID
	

	friend string& operator >>(string& vrString,STeamData& vrData)
	{
		vrString >> vrData.mDBID;
		vrString >> vrData.mHp;
		vrString >> vrData.mMaxMp;
		vrString >> vrData.mMp;
		vrString >> vrData.mMaxMp;
		vrString >> vrData.mHead;
		vrString >> vrData.mLevel;
		vrString >> vrData.X;
		vrString >> vrData.Y;
		vrString >> vrData.mName;
		vrString >> vrData.mSchool;
		vrString >> vrData.mRegion;
		vrString >> vrData.mCombatPower;
		vrString >> vrData.mAnimal;
		vrString >> vrData.mActorID;
		return vrString;
	}

	friend string& operator <<(string& vrString,const STeamData& vrData)
	{
		vrString << vrData.mDBID;
		vrString << vrData.mHp;
		vrString << vrData.mMaxMp;
		vrString << vrData.mMp;
		vrString << vrData.mMaxMp;
		vrString << vrData.mHead;
		vrString << vrData.mLevel;
		vrString << vrData.X;
		vrString << vrData.Y;
		vrString << vrData.mName;
		vrString << vrData.mSchool;
		vrString << vrData.mRegion;
		vrString << vrData.mCombatPower;
		vrString << vrData.mAnimal;
		vrString << vrData.mActorID;
		return vrString;
	}

};

enum TeamPickType{eFreePick, eCaptainPick, eOrderPick};
class CTeamData
{
public:
	CTeamData();
	~CTeamData();

	DWORD GetCaptain();						//得到队长的ID
	const char* GetCaptainName();			//得到队长的名字
	bool TransferCaptain(DWORD vMemberID);	//转让队长
	bool Exist(DWORD vMemberID);			//某个队员是否存在
	bool Exist(std::string& vrMemberName);		//某个队员是否存在
	bool Exist(const char* vpMemberName);			//某个队员是否存在
	bool DeleteMember(DWORD vMemberID);		//将某个队员删除
	bool AddMember(DWORD vMemberID,std::string& vrMemberName,char MemberHead,char  Metier,long BornTag,long  actorID);
	bool Empty();
	void Clear(){mMemberCount=0;};
	int  Begin()const {return 0;};
	int  End()const {return mMemberCount;};
	/* gao 2009.12.25
	得到成员数据的指针
	*/
	STeamData* GetTeamData(){return mMember;}
	/* end */
	int  Size(){return mMemberCount;};

	int Find(DWORD vID)
	{
		for(int i=0;i<mMemberCount;i++)
		{
			if(mMember[i].mDBID ==vID)
			{
				return i;
			}
		}
		return mMemberCount;
	}

	/* gao 2009.12.25
	增加通过名字来查找人物
	*/
	int Find(std::string sName)
	{
		for(int i=0;i<mMemberCount;i++)
		{
			if(mMember[i].mName ==sName)
			{
				return i;
			}
		}
		return mMemberCount;
	}
	/* end */

	STeamData& operator[] (int vIndex) 
	{
		if(vIndex<Begin() || vIndex>=End())
		{
			return mNullTeamData;
		}
		else
		{
			return mMember[vIndex];
		}
	};

	friend string& operator >>(string& vrString,CTeamData& vrData)
	{
		vrString >> vrData.mMemberCount;
		
		for(int i = vrData.Begin();i<vrData.End();++i)
		{
			vrString>>vrData[i];
		}

		return vrString;
	}

	friend string& operator <<(string& vrString, CTeamData& vrData)
	{
		vrString<<vrData.mMemberCount;

		for(int i=vrData.Begin();i<vrData.End();++i)
		{
			vrString<<vrData[i];
		}

		return vrString;
	}

public:
	void PickOK();
	bool SetPickType(TeamPickType type);
	TeamPickType GetPickType(){return m_PickType;}
	STeamData			mMember[MAX_TEAM_MENBER_COUNT]; //成员的数组
private:
	static	STeamData	mNullTeamData;
	
	int					mMemberCount;

	TeamPickType		m_PickType;
};


#endif//TEAM_DATA_H
