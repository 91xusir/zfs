#ifndef MASTER_LIST_CONST_H 
#define MASTER_LIST_CONST_H

#include "net/rt_net.h"

//gw2r
const unsigned char GW2R_APPLY_SUCCESS=0;	//�����б�ɹ�
const unsigned char GW2R_APPLY_FAIL=1;		//�����б�ʧ��
const unsigned char GW2R_LIST_ADD=2;		//����һ��ʦ��
const unsigned char GW2R_LIST_DEL=3;		//����һ��ʦ��
const unsigned char GW2R_LIST_INFO=4;		//ʦͽ�б���Ϣ//��ʱ����Ҫ

//r2gw
const unsigned char R2GW_APPLY_ADD_LIST=0;
const unsigned char R2GW_SET_MAX_COUNT=1;

//r2c
const unsigned char R2C_APPLY_SUCCESS=0;				//�����б�ɹ�
const unsigned char R2C_APPLY_FAIL=1;					//�����б�ʧ��
const unsigned char R2C_LIST_INFO=2;					//ʦͽ�б���Ϣ

//c2r
const unsigned char C2R_APPLY_ADD_LIST=0;				//��������б�
const unsigned char C2R_APPLY_VIEW_LIST=1;				//����鿴�б�

//ʧ�ܴ���
const unsigned char FAIL_LIST_OVERFLOW=0;				//�б���
const unsigned char FAIL_LEVEL_LOW=1;					//�ȼ�̫��=1;//�б���
const unsigned char FAIL_STUDENT_OVERFLOW=2;			//��ǰѧ��̫��
const unsigned char FAIL_MASTER_ALREADY_EXIST=3;		//��ʦ���Ѿ������б���

//ʦ���б����ֵ
const int MAX_MASTER_COUNT_IN_LIST=60;

struct SMasterInfo
{
	SMasterInfo(unsigned long vDBID,BYTE vCamp,const string& vrName,const string& vrMetier,
				short int vLevel, short int vPrenticeAmount)
		:mDBID(vDBID), //ʦ�������ݿ�ID
		mCamp(vCamp), //��Ӫ//����
		mName(vrName), //ʦ��������
		mMetier(vrMetier), //ʦ����ְҵ
		mLevel(vLevel), //ʦ���ĵȼ�
		mPrenticeAmount(vPrenticeAmount)//�Ѿ���ʦ��ͽ������
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

	unsigned long		mDBID;					//ʦ�������ݿ�ID
	char				mCamp;					//��Ӫ
	std::string			mName;					//ʦ��������
	std::string			mMetier;				//ʦ����ְҵ
	short int			mLevel;					//ʦ���ĵȼ�
	short int			mPrenticeAmount;		//�Ѿ���ʦ��ͽ������
};


class CMasterList//��game_world�ϵ�ʦ���б�
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


	CMasterList& operator += (SMasterInfo& vrMaster)//����һ��ʦ��
	{
		mList.push_back(vrMaster);
		return *this;
	};	


	CMasterList& operator -= (unsigned long  vDBID)			//��ȥһ��ʦ��
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

	bool Overflow()//�Ƿ��Ѿ�������������//true��ʾ����
	{
		return ((int) mList.size()) >= mMaxMasterCount ?  true : false;
	};

	void SetMaxCount(int vMaxCount)
	{
		mMaxMasterCount=vMaxCount;
	};
private:
	int						mMaxMasterCount;			//�б�������ʦ������
public:
	list<SMasterInfo>		mList;						//�Ѿ�������б�
};



#endif//MASTER_LIST_CONST_H

