

#ifndef GAME_MAIL_SHARED_CONST_H
#define GAME_MAIL_SHARED_CONST_H
#include "core/rt2_core.h"
#include <string>
#include <vector>
#include "game_string_assistant.h"


//�ͻ���
const unsigned char C2R_INVITE_OTHER_FRIEND=0;				//�Լ�������˼������[DWORD vOtherRegionID]
const unsigned char C2R_BANISH_OTHER_FRIEND=1;				//�Լ�ɾ�����˴Ӻ�����[DWORD vOtherDBID]
const unsigned char C2R_AGREE_OTHER_FRIEND_INVITE=2;		//�Լ�ͬ����˵�����[DWORD vOtherDBID]
const unsigned char C2R_REFUSE_OTHER_FRIEND_INVITE=3;		//�Լ��ܾ����˵�����[DWORD vOtherDBID]
const unsigned char C2R_SEND_MAIL=4;						//���ʼ�[DWORD vSendToDBID,char* vTitle,char* vContent]
const unsigned char C2R_DELETE_MAIL=5;						//ɾ��һ���ʼ�[string vMailTime]
const unsigned char C2R_DELETE_OTHER_ENEMY=6;				//ɾ��һ������[DWORD vOtherDBID]
const unsigned char C2R_INVITE_OTHER_FRIEND_BYNAME=7;		//�Լ�������˼������[string name]
const unsigned char C2R_MOVE_ITEM = 8;						//����ƶ��ʼ���Ʒ
const unsigned char C2R_SETMAIL_READ = 9;
//const unsigned char C2R_DELETE_OTHER_TEMPORARYFRIEND = 10;  //���ɾ����ʱ���� add by ���� at 2010.05.13
const unsigned char C2R_INVITE_OTHER_FRIEND_BY_NAME = 10;    //ͨ��������Ӻ��� add by ���� at 2010.05.
const unsigned char C2R_ADD_BLACKLIST = 11;                 //��������˵������� add by ���� at 2010.05.14 
const unsigned char C2R_DELETE_BLACKLIST = 12;              //ɾ�������˴Ӻ����� add by ���� at 2010.05.14    

//RegionServer �� GameWorld
//const unsigned char R2GW_SEND_MAIL=0;						//���ʼ���GameWorld
//const unsigned char R2GW_DELETE_MAIL=1;						//ɾ���ʼ�


//GameWorld
//const unsigned char GW2R_RECEIVE_MAIL=0;						//�����ʼ���GameWorld


//RegionServer
const unsigned char R2C_ADD_PLAYER=0	;						//����һ������[DWORD vOtherDBID ,char* vrOtherName]
const unsigned char R2C_DELETE_FRIEND=1	;						//����һ������[DWORD vOtherDBID]
const unsigned char R2C_UPDATE_ONE_PLAYER=2	;					//ˢ��ĳ����Ա������[DWORD vOtherDBID]
const unsigned char R2C_UPDATE_ALL_PLAYER=3;					//ˢ��ĳ����Ա������[������]
const unsigned char R2C_FRIEND_MESSAGE=4		;				//ˢһ����Ϣ[int vMessageID]
const unsigned char R2C_FRIEND_MESSAGE_WITH_ID=5		;		//ˢһ����Ϣ[DWORD vDBIDInMessage]
//-------����-----------
const unsigned char R2C_OTHER_FRIEND_INVITE=6;					//���������Լ��������[DWORD vOtherDBID,char* vOtherName]
const unsigned char R2C_RECEIVE_MAIL=7;							//���ʼ�//[char* vRecievedDate,DWORD vFromDBID,char* vrFromName,char* vpTitle,char* vpContent]

const unsigned char R2C_DELETE_TEACHER=10;						//����һ����ʦ[DWORD vOtherDBID]
const unsigned char R2C_DELETE_STUDENT=11;						//����һ��ѧ��[DWORD vOtherDBID]
const unsigned char R2C_DELETE_MASTER=12;						//����һ��ʦ��[DWORD vOtherDBID]
const unsigned char R2C_DELETE_PRENTICE=13;						//����һ��ѧͽ[DWORD vOtherDBID]
const unsigned char R2C_ADD_WHEN_FRIEND=14;                     //����һ����ʱ����




//-----------��������---------------
const unsigned char R2R_DELETE_FRIEND=0;						//����һ������[DWORD vOtherDBID]
const unsigned char R2R_UPDATE_SELF_TO_FRIEND=1;				//ˢ���Լ������ݵ�����[Ҫͬ��������]
const unsigned char R2R_FRIEND_MESSAGE=2;						//ˢһ����Ϣ[int vMessageID]
const unsigned char R2R_FRIEND_MESSAGE_WITH_ID=3;				//ˢһ����Ϣ[DWORD vDBIDInMessage,int vMessage]

//-------����-----------
const unsigned char R2R_SELF_ADD_OTHER_FRIEND_SUCCEED=4;		//�Լ������˼��뵽�Լ����ѳɹ�

const unsigned char R2R_SELF_AGREE_FRIEND_INVITE=5;				//�Լ�ͬ�������˵ĺ���
const unsigned char R2R_SELF_REFUSE_FRIEND_INVITE=6;			//�Լ��ܾ�������˵ĺ���
const unsigned char R2R_SELF_INVITE_FRIEND=7;					//�Լ�������˼������
const unsigned char R2R_SELF_BANISH_FRIEND=8;					//�Լ�������˴Ӻ���
const unsigned char R2R_FORCE_OTHER_FRIEND_UPDATE_TO_SELF=9;	//Ҫ����˸������ݸ��Լ�
const unsigned char R2R_SELF_INVITE_FRIEND_NREA=10;	//�������Լ�������˼������

//const unsigned char R2R_SELF_ADD_OTHER_CASUALTY=10;				//�Լ�������˵��ܺ����б�
//const unsigned char R2R_DELETE_CASUALTY=11;						//ɾ��һ���ܺ���
//const unsigned char R2R_FORCE_OTHER_ENEMY_UPDATE_TO_SELF=12;    //Ҫ����˸������ݸ��Լ�
//const unsigned char R2R_UPDATE_SELF_TO_CASUALTY=13;				//�����Լ������ݵ��ܺ���


const unsigned char R2R_UPDATE_OTHER_FRIEND_TO_SELF=R2R_UPDATE_SELF_TO_FRIEND;				//����ˢ�����ݵ��Լ�
const unsigned char R2R_OTHER_FRIEND_ADD_SELF_SUCCEED=R2R_SELF_ADD_OTHER_FRIEND_SUCCEED;	//���˽���ɹ��������

const unsigned char R2R_OTHER_FRIEND_AGREE_INVITE=R2R_SELF_AGREE_FRIEND_INVITE;				//����ͬ��������
const unsigned char R2R_OTHER_REFUSE_FRIEND_INVITE=R2R_SELF_REFUSE_FRIEND_INVITE;			//���˾ܾ��������
const unsigned char R2R_OTHER_FRIEND_INVITE=R2R_SELF_INVITE_FRIEND;							//���������Լ��������
const unsigned char R2R_OTHER_FRIEND_BANISH=R2R_SELF_BANISH_FRIEND;							//���������Լ�������

//const unsigned char R2R_OTHER_ADD_SELF_CASUALTY=R2R_SELF_ADD_OTHER_CASUALTY;				//���˼����Լ����ܺ����б�
//const unsigned char R2R_UPDATE_OTHER_ENEMY_TO_SELF=R2R_UPDATE_SELF_TO_CASUALTY;				//���˸������ݵ��Լ�



//���õ�һЩ����
const int MAIL_DATE_LENGTH=19;								//�ʼ������ڵĳ���
const int MAIL_CONTENT_LIMIT_COUNT=1024;					//���ݳ�������1024��Ӣ���ַ�
const int MAIL_TITLE_LIMIT_COUNT=100;						//�ż��ı���ĳ�������100��Ӣ���ַ�

const int MAX_FRIEND_COUNT=50;								//����������
const int MAX_ENEMY_COUNT=50;								//����������
const int MAX_TEACHER_COUNT=1;								//�����ʦ����
const int MAX_MASTER_COUNT=1;								//���ʦ������
const int MAX_STUDENT_COUNT=2;								//���ѧ������
const int MAX_PRENTICE_COUNT=50;							//���ͽ������
const int MAX_WHENFRIEND_COUNT=100;                         //�����ʱ����
const int MAX_BLACKLIST_COUNT=100;                           //������������

const int FRIEND_DB_MAX_LENGTH=6*1024;						//6k



const int LOCATION_MAIL_NO_CLOSE		=0;					//�����ʼ���Ϣû�������˳�[����Ӧ�÷���]



//�汾��Ҫ�䶯��Ҫ�ǵĸ��ĸ�������game_region game_world
const int LAST_LOCATION_MAIL_HEAD_VERSION=100;				//���µر����ʼ�ͷ�İ汾
const int LAST_LOCATION_MAIL_VERSION=100;					//���µر����ʼ��İ汾[ÿ���ʼ�]
//const int LAST_FRIEND_VERSION=101;						//���µĺ��Ѱ汾
//const int LAST_FRIEND_VERSION=105;							//���µĺ��Ѱ汾
//const int LAST_FRIEND_VERSION=106;							//���µĺ��Ѱ汾
//const int LAST_FRIEND_VERSION=107;							//���µĺ��Ѱ汾//add by fox @2005.10.11
const int LAST_FRIEND_VERSION=108;							//���µĺ��Ѱ汾//add by fox @2005.10.14
//�汾��Ҫ�䶯��Ҫ�ǵĸ��ĸ�������game_region game_world







//------�Է����--------
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






//ʱ��
struct SMailTime
{
	unsigned short int			mYear;
	unsigned char				mMonth;
	unsigned char				mDay;
	unsigned char				mHour;
	unsigned char				mMinute;
};

//�ʼ�����


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



//��������
using std::string;

struct SConnection
{
public:
	enum
	{
		FRIEND=(1<<0),	//����					//0x0001
		ENEMY=(1<<1),	//����					//0x0002
		STUDENT=(1<<2),	//ѧ��//δ��ʦ			//0x0004
		PRENTICE=(1<<3),//ͽ��//�Ѿ���ʦ		//0x0008
		TEACHER=(1<<4),//��ʦ//δ��ʦ			//0x0010
		MASTER=(1<<5),//ʦ��//�Ѿ���ʦ			//0x0020
		BLACKLIST=(1<<7), //������
		FAKEBLACKLIST=(1<<8),//�ٺ�����
	};
	//CASUALTY=(1<<2),//�ܺ���
	
	SConnection():mDBID(0),mOnline(false),mName(),mConnection(0),mKilledCount(0),
		mConnectionIndex(0),mJoinIndex(0),mStudentOnlineTime(0),mLevel(0),mResolve(0),mSchool(0),mAddress(){};
	
	SConnection(unsigned long vDBID,bool vOnline,const std::string& vrName,long vConnection,
		int vKilledCount,unsigned long vConnectionIndex,long vSchool,unsigned long vLevel,const std::string& vAddress,long vResolve)
		:mDBID(vDBID),mOnline(vOnline),mName(vrName),mConnection(vConnection),
		mKilledCount(vKilledCount),mConnectionIndex(vConnectionIndex),mSchool(vSchool),mLevel(vLevel),mAddress(vAddress),mResolve(vResolve),mJoinIndex(0),mStudentOnlineTime(0){};


	~SConnection(){};

	//ҪС�Ĵ�������в�Ҫ����ָ��ֵ
	//��������˱�������ҪС�Ĵ���//������ǰ�汾�����õ�ʱ������
	static int			mVersion;			//��ǰ�İ汾
	unsigned long		mDBID;
	bool				mOnline;
	std::string			mName;
	unsigned short int	mConnection;		//��ϵ����
	unsigned short int	mKilledCount;		//��������ɱ�˼���
	unsigned long		mConnectionIndex;	//����ѧͽʱ[δ��ʦ]//Ҫ��¼һ����ϵ����ֵ
	unsigned char		mJoinIndex;			//ʦͽ�����Ⱥ����//���ֻ��255[0-255]

	int					mStudentOnlineTime;//ѧ������ʱ��[Сʱ]//����Ҫ����
	unsigned short int  mSchool;           //����
	unsigned short int  mLevel;            //�ȼ�
	std::string         mAddress;          //����
	unsigned short int  mResolve;		   //���ܶ�


	friend CDebugInterface& operator << (CDebugInterface& vrDebug,SConnection& vrData)
	{
		guard;
		vrDebug<<"DBID [" << vrData.mDBID << "]\n";
		
		vrDebug<< (vrData.Is(SConnection::FRIEND) ? "[����]" : "");
		vrDebug<< (vrData.Is(SConnection::ENEMY) ? "[����]" : "");
		vrDebug<< (vrData.Is(SConnection::STUDENT)? "[ѧ��]" : "");
		vrDebug<< (vrData.Is(SConnection::PRENTICE)? "[ͽ��]" : "");
		vrDebug<< (vrData.Is(SConnection::TEACHER)? "[��ʦ]" : "");
		vrDebug<< (vrData.Is(SConnection::MASTER)? "[ʦ��]" : "");
		vrDebug<< (vrData.Is(SConnection::BLACKLIST)? "[������]" : "");
		vrDebug<< (vrData.Is(SConnection::FAKEBLACKLIST)? "[�ٵĺ�����]" : "");

		//vrDebug<< (vrData.Is(SConnection::CASUALTY) ? "[������]" : "");

		vrDebug<<"Name [ "<< vrData.mName << "]"<< "killed[" << vrData.mKilledCount<< "]"
			<< "DB IndexID[" << vrData.mConnectionIndex << "] join no:["<< ((int)vrData.mJoinIndex) <<"]"
			"Online time : "<< vrData.mStudentOnlineTime << "\n";

		return vrDebug;
		unguard;
	}

	
	friend string& operator<<(string& vrString, SConnection& vrData)
	{
		guard;

		vrString<<vrData.mConnection;//��ϵ
		vrString<<vrData.mDBID;
		vrString<<vrData.mName;

		if(vrData.Is(SConnection::STUDENT) || vrData.Is(SConnection::TEACHER))
		{//��ѧ������ʦ[δ��ʦ]
			vrString<<vrData.mConnectionIndex;
		}

		if(vrData.Is(SConnection::PRENTICE))
		{
			vrString<<vrData.mJoinIndex;
		}
		
		if(vrData.Is(SConnection::ENEMY))
		{//����
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
		case 100://��֧��
		case 101://��֧��
		case 102://�д���//��֧��
		case 103://����������//��֧����
		case 104://����������//��֧����
		case 105://����������//��֧����
			break;
		case 106:
			vrString>>vrData.mDBID;
			vrString>>vrData.mName;
			vrString>>vrData.mConnection;
			vrString>>vrData.mKilledCount;
			break;
		case 107://����������//��֧����
			break;
		case 108:
			vrString>>vrData.mConnection;
			vrString>>vrData.mDBID;
			vrString>>vrData.mName;

			if(vrData.Is(SConnection::STUDENT) || vrData.Is(SConnection::TEACHER))
			{//��ѧ������ʦ[δ��ʦ]
				vrString>>vrData.mConnectionIndex;
			}

			if(vrData.Is(SConnection::PRENTICE))
			{//��ͽ��[��ʦ]
				vrString>>vrData.mJoinIndex;
			}

			if(vrData.Is(SConnection::ENEMY))
			{//����
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

	void Add(unsigned short int vConnection)//���ĳ�ֹ�ϵ
	{
		mConnection  |= vConnection;//��ĳ�ֹ�ϵ���
	}

	bool Remove(unsigned short int vConnection)//�Ƴ�ĳ�ֹ�ϵ//true��ʾ���й�ϵ
	{
		if((vConnection & ENEMY)!=0)
		{//�Ƴ����ǳ��˹�ϵ
			mKilledCount=0;//�����ɱ��
		}

		if((vConnection & STUDENT)!=0)
		{//�Ƴ�����ѧ����ϵ
			mStudentOnlineTime=0;//�������ʱ��
		}

		mConnection  &= ~vConnection;//��ĳ�ֹ�ϵ�Ƴ�
		if(mConnection==0)
		{
			return false;//û���κι�ϵ
		}
		else
		{
			return true;//����ĳ�ֹ�ϵ
		}
	}

	bool Is(unsigned short int vConnection)//��ĳ�ֹ�ϵ
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
	int Size(unsigned short int vConnection)//ĳ�ֹ�ϵ����ҵ�����
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

	SConnection* _Find(DWORD vDBID)//��������Ƿ����
	{
		std::map<DWORD,SConnection>::iterator It;
		It=mMap.find(vDBID);

		if(It==mMap.end())
		{
			return NULL;
		}
		return &(It->second);
	}

	SConnection* _Find(unsigned short int vConnection,const std::string vrName)//����ĳ�ֹ�ϵ������Ƿ����
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


	SConnection* _Find(unsigned short int vConnection,DWORD vDBID)//����ĳ�ֹ�ϵ������Ƿ����
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
	bool Erase(unsigned short int vConnection)//���ĳ�ֹ�ϵ
	{
		if(mMap.size()==0)
		{
			return false;
		}

		std::map<DWORD,SConnection>::iterator It;
		for(It=mMap.begin();It!=mMap.end();)
		{
			if(It->second.Remove(vConnection))
			{//û�й�ϵ
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

	bool Erase(unsigned short int vConnection,DWORD vDBID)//��ĳ�˵�ĳ�ֹ�ϵ���
	{
		std::map<DWORD,SConnection>::iterator It;
		It=mMap.find(vDBID);

		if(It==mMap.end())
		{
			return false;
		}

		if( !(It->second.Remove(vConnection)) )
		{//û�й�ϵ��
			mMap.erase(It);
		}

		if(vConnection & SConnection::PRENTICE)
		{//���ȥ��һ��ѧͽ
			mPrenticeCount--;
			std::map<DWORD,SConnection>::iterator It;
			for(It=mMap.begin();It!=mMap.end();++It)
			{
				if(It->second.Is(SConnection::PRENTICE))
				{
					It->second.mJoinIndex--;//�����е�ѧͽ��ʦ��λ��ǰ
				}
			}
		}
		if (NULL != m_pIOnConnectionChange)
		{
			m_pIOnConnectionChange->OnInsert(vConnection, vDBID);
		}

		return true;
	}

	//Tianh ����ĳ�ֹ�ϵ��λ�ҵ�ID
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
	
	//Tianh ��ĳ�ֹ�ϵȫ��ɾ��
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

	void Insert(DWORD vDBID,SConnection& vrData)//��ĳ�˲����б�����Ѿ�������ı�����˵Ĺ�ϵ
	{
		if(vrData.Is(SConnection::PRENTICE))//ѧͽ
		{
			vrData.mJoinIndex=mPrenticeCount;
			mPrenticeCount++;
		}

		std::map<DWORD,SConnection>::iterator It;
		It=mMap.find(vDBID);



		if(It==mMap.end())
		{//����˲����б���
			mMap[vDBID]=vrData;
		}
		else
		{//��������б���
			It->second.Add(vrData.mConnection);//����ĳ�ֹ�ϵ
			It->second.mOnline=vrData.mOnline;//�Ƿ����߹�ϵ
			
			if(vrData.mName!="")
			{
				It->second.mName=vrData.mName;		//�������¸�ֵ
			}
			
			/*
			if(It->second.mName=="")
			{//û������//���¸�ֵ
				It->second.mName=vrData.mName;
			}
			*/
			
			//���˿���Ӱ�����ֵ
			if(vrData.Is(SConnection::ENEMY))
			{
				It->second.mKilledCount+=vrData.mKilledCount;//��ɱ�������ı仯
			}


			if(vrData.Is(SConnection::STUDENT) || vrData.Is(SConnection::TEACHER))
			{//�������ʦ����ѧ����ϵ//�ı��ϵ����
				It->second.mConnectionIndex=vrData.mConnectionIndex;//ʦͽ��ϵ����ID
			}

			if(vrData.Is(SConnection::PRENTICE))//ѧͽ
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



//����������һ�ι���
const int FRIEND_NULL_MESSAGE=0;
const int OBJECT_NOT_IN_REGION=1;							//���岻�ڸ�����
const int OBJECT_NOT_PLAYER=2;								//�����岻����ң��޷���Ϊ���ѣ�
const int FRIEND_LIST_OVERFLOW=3;							//�����б��������޷�������ˣ�
const int FRIEND_NOT_IN_LIST=4;								//�ú��Ѳ����б��У�

const int OTHER_FRIEND_LIST_OVERFLOW=5;						//�Է����������������޷����룡
const int OTHER_ADD_SELF=6;									//[%s]��Ϊ��ĺ��ѣ�
const int SELF_FRIEND_LIST_OVERFLOW_OTHER_CANNOT_ADD=7;		//��ǰ�����б������ޣ�[%s]�޷����룡
const int OTHER_BANISH_SELF=8;								//[%s]�����ѹ�ϵ�����
const int OBJECT_NOT_FIND=9;								//����������ڣ�

const int CHOOSE_PLAYER_ADD_FRIEND=10;						//��ѡ��Ҫ�������ҡ�
const int SELF_CAN_NOT_ADD_SELF=11;							//�Լ����ܳ�Ϊ���ѣ�
const int SELF_BANISH_OTHER=12;								//[%s]�Ѿ��Ӻ����б���ɾ����
const int OTHER_INVITE_SELF=13;								//[%s]�����������ѣ�
const int MAIL_LOCAL_NOT_THIS_DBID_NAME=14;					//����û�и�������ݣ���

const int OTHER_FRIEND_REFUSE_INVITE=15;					//�Է��ܾ���Ϊ��ĺ��ѣ���
const int FRIEND_ALREADY_IN_LIST=16;						//������Ѿ�����ĺ��ѣ�
const int FRIEND_WAIT_PLAYER_RESPOND=17;					//�ȴ��Է��ش�...
const int MASTER_BANISH_YOU=18;								//���Ѿ���[%s]���ʦ�ţ�
const int PRENTICE_RUN_AWAY=19;								//����ͽ��[%s]�Ѿ���ʦ��


const int MSG_MAIL_SEND_TO_NAME_NULL=30;					//�ռ��˲���Ϊ�գ�
const int MSG_MAIL_TITLE_NULL=31;							//���ⲻ��Ϊ�գ�
const int MSG_MAIL_CONTENT_NULL=32;							//���ݲ���Ϊ�գ�
const int MSG_MAIL_RECEIVE_PLAYER_NOT_IN_FRIEND_LIST=33;	//�����˲��ں����б��У�




#endif//GAME_MAIL_SHARED_CONST_H
