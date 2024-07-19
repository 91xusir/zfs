

#ifndef GAME_MAIL_SHARED_CONST_H
#define GAME_MAIL_SHARED_CONST_H
#include "core/rt2_core.h"
#include <string>
#include <vector>
#include "game_string_assistant.h"


//客户端
const unsigned char C2R_INVITE_OTHER_FRIEND=0;				//自己邀请别人加入好友[DWORD vOtherRegionID]
const unsigned char C2R_BANISH_OTHER_FRIEND=1;				//自己删除别人从好友中[DWORD vOtherDBID]
const unsigned char C2R_AGREE_OTHER_FRIEND_INVITE=2;		//自己同意别人的邀请[DWORD vOtherDBID]
const unsigned char C2R_REFUSE_OTHER_FRIEND_INVITE=3;		//自己拒绝别人的邀请[DWORD vOtherDBID]
const unsigned char C2R_SEND_MAIL=4;						//发邮件[DWORD vSendToDBID,char* vTitle,char* vContent]
const unsigned char C2R_DELETE_MAIL=5;						//删掉一个邮件[string vMailTime]
const unsigned char C2R_DELETE_OTHER_ENEMY=6;				//删掉一个仇人[DWORD vOtherDBID]
const unsigned char C2R_INVITE_OTHER_FRIEND_BYNAME=7;		//自己邀请别人加入好友[string name]
const unsigned char C2R_MOVE_ITEM = 8;						//玩家移动邮件物品
const unsigned char C2R_SETMAIL_READ = 9;
//const unsigned char C2R_DELETE_OTHER_TEMPORARYFRIEND = 10;  //玩家删除临时好友 add by 李泽华 at 2010.05.13
const unsigned char C2R_INVITE_OTHER_FRIEND_BY_NAME = 10;    //通过名字添加好友 add by 李泽华 at 2010.05.
const unsigned char C2R_ADD_BLACKLIST = 11;                 //添加其他人到黑名单 add by 李泽华 at 2010.05.14 
const unsigned char C2R_DELETE_BLACKLIST = 12;              //删除其他人从黑名单 add by 李泽华 at 2010.05.14    

//RegionServer 到 GameWorld
//const unsigned char R2GW_SEND_MAIL=0;						//发邮件到GameWorld
//const unsigned char R2GW_DELETE_MAIL=1;						//删除邮件


//GameWorld
//const unsigned char GW2R_RECEIVE_MAIL=0;						//接收邮件从GameWorld


//RegionServer
const unsigned char R2C_ADD_PLAYER=0	;						//增加一个好友[DWORD vOtherDBID ,char* vrOtherName]
const unsigned char R2C_DELETE_FRIEND=1	;						//减少一个好友[DWORD vOtherDBID]
const unsigned char R2C_UPDATE_ONE_PLAYER=2	;					//刷新某个成员的数据[DWORD vOtherDBID]
const unsigned char R2C_UPDATE_ALL_PLAYER=3;					//刷新某个成员的数据[看代码]
const unsigned char R2C_FRIEND_MESSAGE=4		;				//刷一个消息[int vMessageID]
const unsigned char R2C_FRIEND_MESSAGE_WITH_ID=5		;		//刷一个消息[DWORD vDBIDInMessage]
//-------别人-----------
const unsigned char R2C_OTHER_FRIEND_INVITE=6;					//别人邀请自己加入好友[DWORD vOtherDBID,char* vOtherName]
const unsigned char R2C_RECEIVE_MAIL=7;							//收邮件//[char* vRecievedDate,DWORD vFromDBID,char* vrFromName,char* vpTitle,char* vpContent]

const unsigned char R2C_DELETE_TEACHER=10;						//减少一个老师[DWORD vOtherDBID]
const unsigned char R2C_DELETE_STUDENT=11;						//减少一个学生[DWORD vOtherDBID]
const unsigned char R2C_DELETE_MASTER=12;						//减少一个师傅[DWORD vOtherDBID]
const unsigned char R2C_DELETE_PRENTICE=13;						//减少一个学徒[DWORD vOtherDBID]
const unsigned char R2C_ADD_WHEN_FRIEND=14;                     //增加一个临时好友




//-----------服务器间---------------
const unsigned char R2R_DELETE_FRIEND=0;						//减少一个好友[DWORD vOtherDBID]
const unsigned char R2R_UPDATE_SELF_TO_FRIEND=1;				//刷新自己的数据到好友[要同步的数据]
const unsigned char R2R_FRIEND_MESSAGE=2;						//刷一个消息[int vMessageID]
const unsigned char R2R_FRIEND_MESSAGE_WITH_ID=3;				//刷一个消息[DWORD vDBIDInMessage,int vMessage]

//-------别人-----------
const unsigned char R2R_SELF_ADD_OTHER_FRIEND_SUCCEED=4;		//自己将别人加入到自己好友成功

const unsigned char R2R_SELF_AGREE_FRIEND_INVITE=5;				//自己同意加入别人的好友
const unsigned char R2R_SELF_REFUSE_FRIEND_INVITE=6;			//自己拒绝加入别人的好友
const unsigned char R2R_SELF_INVITE_FRIEND=7;					//自己邀请别人加入好友
const unsigned char R2R_SELF_BANISH_FRIEND=8;					//自己驱逐别人从好友
const unsigned char R2R_FORCE_OTHER_FRIEND_UPDATE_TO_SELF=9;	//要求别人更新数据给自己
const unsigned char R2R_SELF_INVITE_FRIEND_NREA=10;	//近距离自己邀请别人加入好友

//const unsigned char R2R_SELF_ADD_OTHER_CASUALTY=10;				//自己加入别人的受害者列表
//const unsigned char R2R_DELETE_CASUALTY=11;						//删除一个受害者
//const unsigned char R2R_FORCE_OTHER_ENEMY_UPDATE_TO_SELF=12;    //要求仇人更新数据给自己
//const unsigned char R2R_UPDATE_SELF_TO_CASUALTY=13;				//更新自己的数据到受害者


const unsigned char R2R_UPDATE_OTHER_FRIEND_TO_SELF=R2R_UPDATE_SELF_TO_FRIEND;				//别人刷新数据到自己
const unsigned char R2R_OTHER_FRIEND_ADD_SELF_SUCCEED=R2R_SELF_ADD_OTHER_FRIEND_SUCCEED;	//别人将你成功加入好友

const unsigned char R2R_OTHER_FRIEND_AGREE_INVITE=R2R_SELF_AGREE_FRIEND_INVITE;				//别人同意加入好友
const unsigned char R2R_OTHER_REFUSE_FRIEND_INVITE=R2R_SELF_REFUSE_FRIEND_INVITE;			//别人拒绝加入好友
const unsigned char R2R_OTHER_FRIEND_INVITE=R2R_SELF_INVITE_FRIEND;							//别人邀请自己加入好友
const unsigned char R2R_OTHER_FRIEND_BANISH=R2R_SELF_BANISH_FRIEND;							//别人驱逐自己出好友

//const unsigned char R2R_OTHER_ADD_SELF_CASUALTY=R2R_SELF_ADD_OTHER_CASUALTY;				//别人加入自己的受害者列表
//const unsigned char R2R_UPDATE_OTHER_ENEMY_TO_SELF=R2R_UPDATE_SELF_TO_CASUALTY;				//仇人更新数据到自己



//共用的一些常量
const int MAIL_DATE_LENGTH=19;								//邮件中日期的长度
const int MAIL_CONTENT_LIMIT_COUNT=1024;					//内容长度限制1024个英文字符
const int MAIL_TITLE_LIMIT_COUNT=100;						//信件的标题的长度限制100个英文字符

const int MAX_FRIEND_COUNT=50;								//最大好友数量
const int MAX_ENEMY_COUNT=50;								//最大敌人数量
const int MAX_TEACHER_COUNT=1;								//最大老师数量
const int MAX_MASTER_COUNT=1;								//最大师傅数量
const int MAX_STUDENT_COUNT=2;								//最大学生数量
const int MAX_PRENTICE_COUNT=50;							//最大徒弟数量
const int MAX_WHENFRIEND_COUNT=100;                         //最大临时好友
const int MAX_BLACKLIST_COUNT=100;                           //最大黑名单数量

const int FRIEND_DB_MAX_LENGTH=6*1024;						//6k



const int LOCATION_MAIL_NO_CLOSE		=0;					//本地邮件信息没有正常退出[正常应该非零]



//版本号要变动，要记的更改各个工程game_region game_world
const int LAST_LOCATION_MAIL_HEAD_VERSION=100;				//最新地本地邮件头的版本
const int LAST_LOCATION_MAIL_VERSION=100;					//最新地本地邮件的版本[每条邮件]
//const int LAST_FRIEND_VERSION=101;						//最新的好友版本
//const int LAST_FRIEND_VERSION=105;							//最新的好友版本
//const int LAST_FRIEND_VERSION=106;							//最新的好友版本
//const int LAST_FRIEND_VERSION=107;							//最新的好友版本//add by fox @2005.10.11
const int LAST_FRIEND_VERSION=108;							//最新的好友版本//add by fox @2005.10.14
//版本号要变动，要记的更改各个工程game_region game_world







//------对服务端--------
struct SR2rFriendData
{
	void BeginSend(){mOperation=0;mDBIDInMessage=0;mFromDBID=0;mpFromName=NULL;mParameter=0;mOnline=false;mpName=NULL;IsFristOnline=0;IsLeave=0;}
	unsigned char	mOperation;
	unsigned long	mDBIDInMessage;
	unsigned long	mFromDBID;
	char*	        mpFromName;

	unsigned long	mParameter;
	bool	        mOnline;
	char*	        mpName;
	char            IsFristOnline;
	char            IsLeave;
};






//时间
struct SMailTime
{
	unsigned short int			mYear;
	unsigned char				mMonth;
	unsigned char				mDay;
	unsigned char				mHour;
	unsigned char				mMinute;
};

//邮件数据


struct SMailData
{
public:
	long			m_lMailID;
	unsigned long	mFromDBID;
	//SMailTime		mTime;
	std::string		mTime;
	bool			mAlreadyOpen;
	std::string		mFromName;
	std::string		mTitle;
	std::string		mContent;
	SItemID			m_Item;
	long			m_lMoney;


	friend string& operator<<(string& vrString,SMailData& vrMailData)
	{
		guard;
		int Version=LAST_LOCATION_MAIL_VERSION;

		/*
		vrString<<Version;

		vrString<<vrMailData.mFromDBID;
		vrString<<vrMailData.mTime;
		vrString<<vrMailData.mAlreadyOpen;
		vrString<<vrMailData.mFromName;
		vrString<<vrMailData.mTitle;
		vrString<<vrMailData.mContent;
		*/
		OldToString(vrString,Version);

		OldToString(vrString,vrMailData.mFromDBID);
		OldToString(vrString,vrMailData.mTime);
		OldToString(vrString,vrMailData.mAlreadyOpen);
		OldToString(vrString,vrMailData.mFromName);
		OldToString(vrString,vrMailData.mTitle);
		OldToString(vrString,vrMailData.mContent);

		return vrString;
		unguard;
	}

	friend string& operator>>(string& vrString,SMailData& vrMailData)
	{
		guard;
		int Version;
		//vrString>>Version;
		OldFromString(vrString,Version);

		switch(Version)
		{
		case 100:
			/*
			vrString>>vrMailData.mFromDBID;
			vrString>>vrMailData.mTime;
			vrString>>vrMailData.mAlreadyOpen;
			vrString>>vrMailData.mFromName;
			vrString>>vrMailData.mTitle;
			vrString>>vrMailData.mContent;
			*/
			
			OldFromString(vrString,vrMailData.mFromDBID);
			OldFromString(vrString,vrMailData.mTime);
			OldFromString(vrString,vrMailData.mAlreadyOpen);
			OldFromString(vrString,vrMailData.mFromName);
			OldFromString(vrString,vrMailData.mTitle);
			OldFromString(vrString,vrMailData.mContent);


			break;
		default:
			RtCoreLog().Info("Unknowe mail data!\n");
			break;
		}
		return vrString;
		unguard;
	}
};



//好友数据
using std::string;

struct SConnection
{
public:
	enum
	{
		FRIEND=(1<<0),	//朋友					//0x0001
		ENEMY=(1<<1),	//仇人					//0x0002
		STUDENT=(1<<2),	//学生//未出师			//0x0004
		PRENTICE=(1<<3),//徒弟//已经出师		//0x0008
		TEACHER=(1<<4),//老师//未出师			//0x0010
		MASTER=(1<<5),//师傅//已经出师			//0x0020
		BLACKLIST=(1<<7), //黑名单
		FAKEBLACKLIST=(1<<8),//假黑名单
	};
	//CASUALTY=(1<<2),//受害者
	
	SConnection():mDBID(0),mOnline(false),mName(),mConnection(0),mKilledCount(0),
		mConnectionIndex(0),mJoinIndex(0),mStudentOnlineTime(0),mLevel(0),mResolve(0),mSchool(0),mAddress(){};
	
	SConnection(unsigned long vDBID,bool vOnline,const std::string& vrName,long vConnection,
		int vKilledCount,unsigned long vConnectionIndex,long vSchool,unsigned long vLevel,const std::string& vAddress,long vResolve)
		:mDBID(vDBID),mOnline(vOnline),mName(vrName),mConnection(vConnection),
		mKilledCount(vKilledCount),mConnectionIndex(vConnectionIndex),mSchool(vSchool),mLevel(vLevel),mAddress(vAddress),mResolve(vResolve),mJoinIndex(0),mStudentOnlineTime(0){};


	~SConnection(){};

	//要小心处理存盘中不要超过指定值
	//如果更改了变量类型要小心处理//兼容以前版本必须用当时的类型
	static int			mVersion;			//当前的版本
	unsigned long		mDBID;
	bool				mOnline;
	std::string			mName;
	unsigned short int	mConnection;		//关系类型
	unsigned short int	mKilledCount;		//被这个玩家杀了几次
	unsigned long		mConnectionIndex;	//当是学徒时[未出师]//要记录一个关系索引值
	unsigned char		mJoinIndex;			//师徒进入先后次序//最大只有255[0-255]

	int					mStudentOnlineTime;//学生在线时间[小时]//不需要存盘
	unsigned short int  mSchool;           //帮派
	unsigned short int  mLevel;            //等级
	std::string         mAddress;          //地区
	unsigned short int  mResolve;		   //亲密度


	friend CDebugInterface& operator << (CDebugInterface& vrDebug,SConnection& vrData)
	{
		guard;
		vrDebug<<"DBID [" << vrData.mDBID << "]\n";
		
		vrDebug<< (vrData.Is(SConnection::FRIEND) ? "[朋友]" : "");
		vrDebug<< (vrData.Is(SConnection::ENEMY) ? "[仇人]" : "");
		vrDebug<< (vrData.Is(SConnection::STUDENT)? "[学生]" : "");
		vrDebug<< (vrData.Is(SConnection::PRENTICE)? "[徒弟]" : "");
		vrDebug<< (vrData.Is(SConnection::TEACHER)? "[老师]" : "");
		vrDebug<< (vrData.Is(SConnection::MASTER)? "[师傅]" : "");
		vrDebug<< (vrData.Is(SConnection::BLACKLIST)? "[黑名单]" : "");
		vrDebug<< (vrData.Is(SConnection::FAKEBLACKLIST)? "[假的黑名单]" : "");

		//vrDebug<< (vrData.Is(SConnection::CASUALTY) ? "[被害者]" : "");

		vrDebug<<"Name [ "<< vrData.mName << "]"<< "killed[" << vrData.mKilledCount<< "]"
			<< "DB IndexID[" << vrData.mConnectionIndex << "] join no:["<< ((int)vrData.mJoinIndex) <<"]"
			"Online time : "<< vrData.mStudentOnlineTime << "\n";

		return vrDebug;
		unguard;
	}

	
	friend string& operator<<(string& vrString, SConnection& vrData)
	{
		guard;

		vrString<<vrData.mConnection;//关系
		vrString<<vrData.mDBID;
		vrString<<vrData.mName;

		if(vrData.Is(SConnection::STUDENT) || vrData.Is(SConnection::TEACHER))
		{//是学生或老师[未出师]
			vrString<<vrData.mConnectionIndex;
		}

		if(vrData.Is(SConnection::PRENTICE))
		{
			vrString<<vrData.mJoinIndex;
		}
		
		if(vrData.Is(SConnection::ENEMY))
		{//敌人
			vrString<<vrData.mKilledCount;
		}
		
		return vrString;
		unguard;
	};
	

	friend string& operator>>(string& vrString,SConnection& vrData)
	{
		guard;
		
		switch(mVersion)
		{
		case 100://不支持
		case 101://不支持
		case 102://有错误//不支持
		case 103://调试用了下//不支持了
		case 104://调试用了下//不支持了
		case 105://调试用了下//不支持了
			break;
		case 106:
			vrString>>vrData.mDBID;
			vrString>>vrData.mName;
			vrString>>vrData.mConnection;
			vrString>>vrData.mKilledCount;
			break;
		case 107://调试用了下//不支持了
			break;
		case 108:
			vrString>>vrData.mConnection;
			vrString>>vrData.mDBID;
			vrString>>vrData.mName;

			if(vrData.Is(SConnection::STUDENT) || vrData.Is(SConnection::TEACHER))
			{//是学生或老师[未出师]
				vrString>>vrData.mConnectionIndex;
			}

			if(vrData.Is(SConnection::PRENTICE))
			{//是徒弟[出师]
				vrString>>vrData.mJoinIndex;
			}

			if(vrData.Is(SConnection::ENEMY))
			{//敌人
				vrString>>vrData.mKilledCount;
			}

			break;
		default:
			RtCoreLog().Info("Unknow friend data!\n");
			break;
		}
		return vrString;
		
		unguard;
	}

	void Add(unsigned short int vConnection)//添加某种关系
	{
		mConnection  |= vConnection;//将某种关系添加
	}

	bool Remove(unsigned short int vConnection)//移除某种关系//true表示还有关系
	{
		if((vConnection & ENEMY)!=0)
		{//移除的是仇人关系
			mKilledCount=0;//清除被杀数
		}

		if((vConnection & STUDENT)!=0)
		{//移除的是学生关系
			mStudentOnlineTime=0;//清除在线时间
		}

		mConnection  &= ~vConnection;//将某种关系移除
		if(mConnection==0)
		{
			return false;//没有任何关系
		}
		else
		{
			return true;//还有某种关系
		}
	}

	bool Is(unsigned short int vConnection)//是某种关系
	{
		if((mConnection & vConnection) ==0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool NoConnection()
	{
		if(mConnection==0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

struct IOnConnectionChange 
{
	virtual void OnInsert(unsigned short int vConnection, DWORD vDBID) = 0;
	virtual void OnErase(unsigned short int vConnection, DWORD vDBID) = 0;
};

class CConnectionMap
{
public:
	CConnectionMap():mPrenticeCount(0),mWhenFrienfCount(0), m_pIOnConnectionChange(NULL){};
	int Size(unsigned short int vConnection)//某种关系的玩家的数量
	{
		int Size=0;
		std::map<DWORD,SConnection>::iterator It;
		for(It=mMap.begin();It!=mMap.end();++It)
		{
			if(It->second.Is(vConnection))
			{
				Size++;
			}
		}
		return Size;
	}
	bool Exist(DWORD vDBID)
	{
		if(_Find(vDBID)==NULL)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool Exist(unsigned short int vConnection,DWORD vDBID)
	{
		if(_Find(vConnection,vDBID)==NULL)
		{
			return false;
		}
		else
		{
			return true;
		}
	}



	int _Find(unsigned short int vConnection,std::vector<SConnection*>& vrVector)
	{
		std::map<DWORD,SConnection>::iterator It;
		for(It=mMap.begin();It!=mMap.end();++It)
		{
			if(It->second.Is(vConnection))
			{
				vrVector.push_back(&(It->second));
			}
		}
		return ((int) vrVector.size());
	}

	SConnection* _Find(DWORD vDBID)//查找玩家是否存在
	{
		std::map<DWORD,SConnection>::iterator It;
		It=mMap.find(vDBID);

		if(It==mMap.end())
		{
			return NULL;
		}
		return &(It->second);
	}

	SConnection* _Find(unsigned short int vConnection,const std::string vrName)//查找某种关系的玩家是否存在
	{
		std::map<DWORD,SConnection>::iterator It;
		for(It=mMap.begin();It!=mMap.end();++It)
		{
			if(It->second.mName==vrName)
			{
				if(It->second.Is(vConnection))
				{
					return &(It->second);
				}
				break;
			}
		}

		return NULL;

	}


	SConnection* _Find(unsigned short int vConnection,DWORD vDBID)//查找某种关系的玩家是否存在
	{
		std::map<DWORD,SConnection>::iterator It;
		It=mMap.find(vDBID);

		if(It==mMap.end())
		{
			return NULL;
		}

		if(It->second.Is(vConnection))
		{
			return &(It->second);
		}

		return NULL;
	}

	/*
	bool Erase(unsigned short int vConnection)//清除某种关系
	{
		if(mMap.size()==0)
		{
			return false;
		}

		std::map<DWORD,SConnection>::iterator It;
		for(It=mMap.begin();It!=mMap.end();)
		{
			if(It->second.Remove(vConnection))
			{//没有关系
				It=mMap.erase(It);
			}
			else
			{
				++It;
			}
		}

		return true;
	}
	*/

	bool Erase(unsigned short int vConnection,DWORD vDBID)//将某人的某种关系清除
	{
		std::map<DWORD,SConnection>::iterator It;
		It=mMap.find(vDBID);

		if(It==mMap.end())
		{
			return false;
		}

		if( !(It->second.Remove(vConnection)) )
		{//没有关系了
			mMap.erase(It);
		}

		if(vConnection & SConnection::PRENTICE)
		{//如果去掉一个学徒
			mPrenticeCount--;
			std::map<DWORD,SConnection>::iterator It;
			for(It=mMap.begin();It!=mMap.end();++It)
			{
				if(It->second.Is(SConnection::PRENTICE))
				{
					It->second.mJoinIndex--;//将所有的学徒出师排位提前
				}
			}
		}
		if (NULL != m_pIOnConnectionChange)
		{
			m_pIOnConnectionChange->OnInsert(vConnection, vDBID);
		}

		return true;
	}

	//Tianh 根据某种关系排位找到ID
	DWORD FindIDByIndex(unsigned short int vConnection,int Index)
	{
		std::map<DWORD,SConnection>::iterator It;
		for(It=mMap.begin();It!=mMap.end();++It)
		{
			if(It->second.Is(vConnection))
			{
				if (It->second.mJoinIndex == Index)
				{
					return (It->second.mDBID);
				}			
			}
		}
		return 0;
	}
	
	//Tianh 将某种关系全部删除
	void DeleteConnection(unsigned short int vConnection)
	{
		if(mMap.size()==0)
		{
			return;
		}

		std::map<DWORD,SConnection>::iterator It = mMap.begin();
		for(;It!=mMap.end();)
		{
			if(It->second.Is(vConnection))
			{
				mMap.erase(It++);	
			}
			else
			{
				++It;
			}
		}
	}

	void Insert(DWORD vDBID,SConnection& vrData)//将某人插入列表，如果已经存在则改变这个人的关系
	{
		if(vrData.Is(SConnection::PRENTICE))//学徒
		{
			vrData.mJoinIndex=mPrenticeCount;
			mPrenticeCount++;
		}

		std::map<DWORD,SConnection>::iterator It;
		It=mMap.find(vDBID);



		if(It==mMap.end())
		{//这个人不在列表中
			mMap[vDBID]=vrData;
		}
		else
		{//这个人在列表中
			It->second.Add(vrData.mConnection);//增加某种关系
			It->second.mOnline=vrData.mOnline;//是否在线关系
			
			if(vrData.mName!="")
			{
				It->second.mName=vrData.mName;		//名字重新赋值
			}
			
			/*
			if(It->second.mName=="")
			{//没有名字//重新赋值
				It->second.mName=vrData.mName;
			}
			*/
			
			//仇人可以影响的数值
			if(vrData.Is(SConnection::ENEMY))
			{
				It->second.mKilledCount+=vrData.mKilledCount;//被杀害次数的变化
			}


			if(vrData.Is(SConnection::STUDENT) || vrData.Is(SConnection::TEACHER))
			{//如果是老师或者学生关系//改变关系索引
				It->second.mConnectionIndex=vrData.mConnectionIndex;//师徒关系索引ID
			}

			if(vrData.Is(SConnection::PRENTICE))//学徒
			{
				It->second.mJoinIndex=vrData.mJoinIndex;
			}

			if(vrData.Is(SConnection::STUDENT))
			{
				It->second.mStudentOnlineTime=vrData.mStudentOnlineTime;
			}

		}
		if (NULL != m_pIOnConnectionChange)
		{
			m_pIOnConnectionChange->OnInsert(vrData.mConnection, vDBID);
		}
		return;
	}

	void CountPrentice()
	{
		std::vector<SConnection*> PrenticeVector;
		_Find(SConnection::PRENTICE,PrenticeVector);
		mPrenticeCount=(int)PrenticeVector.size();
	}

	void SetIOnConnectionChange(IOnConnectionChange* pIOnConnectionChange)
	{
		m_pIOnConnectionChange = pIOnConnectionChange;
	}

	std::map<DWORD,SConnection> mMap;
private:
	int mPrenticeCount;
	int mWhenFrienfCount;

	IOnConnectionChange* m_pIOnConnectionChange;
};



//再用数字做一次关联
const int FRIEND_NULL_MESSAGE=0;
const int OBJECT_NOT_IN_REGION=1;							//物体不在该区域！
const int OBJECT_NOT_PLAYER=2;								//该物体不是玩家，无法成为好友！
const int FRIEND_LIST_OVERFLOW=3;							//好友列表已满，无法邀请别人！
const int FRIEND_NOT_IN_LIST=4;								//该好友不在列表中！

const int OTHER_FRIEND_LIST_OVERFLOW=5;						//对方好友人数已满，无法加入！
const int OTHER_ADD_SELF=6;									//[%s]成为你的好友！
const int SELF_FRIEND_LIST_OVERFLOW_OTHER_CANNOT_ADD=7;		//当前好友列表到达上限，[%s]无法加入！
const int OTHER_BANISH_SELF=8;								//[%s]将好友关系解除！
const int OBJECT_NOT_FIND=9;								//该物件不存在！

const int CHOOSE_PLAYER_ADD_FRIEND=10;						//请选择要邀请的玩家。
const int SELF_CAN_NOT_ADD_SELF=11;							//自己不能成为好友！
const int SELF_BANISH_OTHER=12;								//[%s]已经从好友列表中删除！
const int OTHER_INVITE_SELF=13;								//[%s]邀请你加入好友！
const int MAIL_LOCAL_NOT_THIS_DBID_NAME=14;					//本地没有该玩家数据！！

const int OTHER_FRIEND_REFUSE_INVITE=15;					//对方拒绝成为你的好友！！
const int FRIEND_ALREADY_IN_LIST=16;						//该玩家已经是你的好友！
const int FRIEND_WAIT_PLAYER_RESPOND=17;					//等待对方回答...
const int MASTER_BANISH_YOU=18;								//您已经被[%s]逐出师门！
const int PRENTICE_RUN_AWAY=19;								//您的徒弟[%s]已经叛师！


const int MSG_MAIL_SEND_TO_NAME_NULL=30;					//收件人不能为空！
const int MSG_MAIL_TITLE_NULL=31;							//标题不能为空！
const int MSG_MAIL_CONTENT_NULL=32;							//内容不能为空！
const int MSG_MAIL_RECEIVE_PLAYER_NOT_IN_FRIEND_LIST=33;	//收信人不在好友列表中！




#endif//GAME_MAIL_SHARED_CONST_H
