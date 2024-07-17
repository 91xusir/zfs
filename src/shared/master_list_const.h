#ifndef MASTER_LIST_CONST_H 
#define MASTER_LIST_CONST_H

#include "net/rt_net.h"

//gw2r
const unsigned char GW2R_APPLY_SUCCESS=0;	//加入列表成功
const unsigned char GW2R_APPLY_FAIL=1;		//加入列表失败
const unsigned char GW2R_LIST_ADD=2;		//增加一个师傅
const unsigned char GW2R_LIST_DEL=3;		//减少一个师傅
const unsigned char GW2R_LIST_INFO=4;		//师徒列表信息//暂时不需要

//r2gw
const unsigned char R2GW_APPLY_ADD_LIST=0;
const unsigned char R2GW_SET_MAX_COUNT=1;

//r2c
const unsigned char R2C_APPLY_SUCCESS=0;				//加入列表成功
const unsigned char R2C_APPLY_FAIL=1;					//加入列表失败
const unsigned char R2C_LIST_INFO=2;					//师徒列表信息

//c2r
const unsigned char C2R_APPLY_ADD_LIST=0;				//申请加入列表
const unsigned char C2R_APPLY_VIEW_LIST=1;				//申请查看列表

//失败代码
const unsigned char FAIL_LIST_OVERFLOW=0;				//列表满
const unsigned char FAIL_LEVEL_LOW=1;					//等级太低=1;//列表满
const unsigned char FAIL_STUDENT_OVERFLOW=2;			//当前学生太多
const unsigned char FAIL_MASTER_ALREADY_EXIST=3;		//该师傅已经存在列表中

//师傅列表最大值
const int MAX_MASTER_COUNT_IN_LIST=60;

struct SMasterInfo
{
	SMasterInfo(unsigned long vDBID,BYTE vCamp,const string& vrName,const string& vrMetier,
				short int vLevel, short int vPrenticeAmount)
		:mDBID(vDBID), //师傅的数据库ID
		mCamp(vCamp), //阵营//国家
		mName(vrName), //师傅的名字
		mMetier(vrMetier), //师傅的职业
		mLevel(vLevel), //师傅的等级
		mPrenticeAmount(vPrenticeAmount)//已经出师的徒弟数量
	{
	};

	SMasterInfo()
		:mDBID(0),
		mCamp(0),
		mName(),
		mMetier(),
		mLevel(0),
		mPrenticeAmount(0)
	{
	}


	~SMasterInfo(){};

	friend CG_CmdPacket&  operator << ( CG_CmdPacket& vrCmd,SMasterInfo& vrInfo)
	{
		vrCmd.WriteLong(vrInfo.mDBID);
		vrCmd.WriteByte(vrInfo.mCamp);
		vrCmd.WriteString((char*)vrInfo.mName.c_str());
		vrCmd.WriteString((char*)vrInfo.mMetier.c_str());
		vrCmd.WriteShort(vrInfo.mLevel);
		vrCmd.WriteShort(vrInfo.mPrenticeAmount);
		return vrCmd;
	};

	friend CG_CmdPacket&  operator >> ( CG_CmdPacket& vrCmd,SMasterInfo& vrInfo)
	{
		vrCmd.ReadLong((long*)&vrInfo.mDBID);
		vrCmd.ReadByte(&vrInfo.mCamp);
		char* pName;
		vrCmd.ReadString(&pName);
		vrInfo.mName=pName;
		char* pMetier;
		vrCmd.ReadString(&pMetier);
		vrInfo.mMetier=pMetier;

		vrCmd.ReadShort(&vrInfo.mLevel);
		vrCmd.ReadShort(&vrInfo.mPrenticeAmount);
		return vrCmd;
	};

	unsigned long		mDBID;					//师傅的数据库ID
	char				mCamp;					//阵营
	std::string			mName;					//师傅的名字
	std::string			mMetier;				//师傅的职业
	short int			mLevel;					//师傅的等级
	short int			mPrenticeAmount;		//已经出师的徒弟数量
};


class CMasterList//在game_world上的师傅列表
{
public:
	CMasterList()
		:mMaxMasterCount(MAX_MASTER_COUNT_IN_LIST),
		mList()
	{
	};

	~CMasterList(){};


	bool Exist(unsigned long vDBID)
	{
		list<SMasterInfo>::iterator It;
		for(It=mList.begin();It!=mList.end();++It)
		{
			if(It->mDBID == vDBID)
			{
				return true;
			}
		}

		return false;
	};


	CMasterList& operator += (SMasterInfo& vrMaster)//加上一个师傅
	{
		mList.push_back(vrMaster);
		return *this;
	};	


	CMasterList& operator -= (unsigned long  vDBID)			//减去一个师傅
	{
		list<SMasterInfo>::iterator It;
		for(It=mList.begin();It!=mList.end();++It)
		{
			if(It->mDBID == vDBID)
			{
				mList.erase(It);
				return *this;
			}
		}
		return *this;
	};


	friend CG_CmdPacket& operator << ( CG_CmdPacket& vrCmd,CMasterList& vrList )
	{
		long Size=(long)vrList.mList.size();
		vrCmd.WriteLong(Size);
		list<SMasterInfo>::iterator It;
		for(It=vrList.mList.begin();It!=vrList.mList.end();++It)
		{
			vrCmd << (*It); 
		}
		return vrCmd;
	};

	friend CG_CmdPacket& operator >> ( CG_CmdPacket& vrCmd,CMasterList& vrList )
	{
		vrList.mList.clear();
		long Size;
		vrCmd.ReadLong(&Size);
		SMasterInfo Temp;
		for(int i=0;i!=Size;++i)
		{
			vrCmd >> Temp;
			vrList.mList.push_back(Temp);
		}
		return vrCmd;
	};

	bool Overflow()//是否已经超过人数上限//true表示超过
	{
		return ((int) mList.size()) >= mMaxMasterCount ?  true : false;
	};

	void SetMaxCount(int vMaxCount)
	{
		mMaxMasterCount=vMaxCount;
	};
private:
	int						mMaxMasterCount;			//列表中最大的师傅数量
public:
	list<SMasterInfo>		mList;						//已经加入的列表
};



#endif//MASTER_LIST_CONST_H

