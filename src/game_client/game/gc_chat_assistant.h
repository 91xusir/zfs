

#ifndef GC_CHAT_ASSISTANT_H 
#define GC_CHAT_ASSISTANT_H 

#include <string>
typedef unsigned long       DWORD;      

#define MAX_CHANNEL_CHATBOX 7

//enum EChanneLists
//{
//	ALL_LIST = 0,				// ����Ƶ���б�
//	NEAR_LIST,					// ����
//	TEAM_LIST,					// ����Ƶ���б�
//	UNION_LIST,					// ����Ƶ���б�
//	BATTLE_LIST,				// ս��Ƶ��
//	WORLD_LIST,					// ����Ƶ���б�
//	PERSONAL_LIST,				// ����Ƶ��
//	MAX_LIST
//};


enum EChannels
{
	NEAR_CHANNEL = 0,			// ����Ƶ��
	TEAM_CHANNEL,				// ����Ƶ��
	UNION_CHANNEL,				// ����Ƶ��
	BATTLE_CHANNEL,				// ս��Ƶ��
	WORLD_CHANNEL,				// ����Ƶ��
	PERSONAL_CHANNEL,			// ����Ƶ��
	BATTLEINFO_CHANNEL,
	SYSTEM_CHANNEL,				// ϵͳƵ��
	GM_CHSNNEL,					// GMƵ��
	SPEAKER_CHANNEL,			// ����Ƶ��
	MAX_CHANNEL
};

#define MAX_CHANNEL_BUTTON MAX_CHANNEL-3
class SChannelAtom
{
public:
	//Ƶ��ö��//����ͨѶ��ʶ//���������������//Ƶ����ʾʱ��ǰ׺//Ƶ������ɫ
	SChannelAtom ( EChannels vChannel,char vNetChannel,int vMask,std::string vDescription,DWORD vColor)
		:mChannel(vChannel),
		mNetChannel(vNetChannel),
		mMask(vMask),
		mDescription(vDescription),
		mColor(vColor)
	{
	}

	bool Is(EChannels vChannel)//��Ƶ���Ƿ���ָ��Ƶ��ö�ٵ�Ƶ��
	{
		guard;
		if(mChannel==vChannel)
		{
			return true;
		}
		else
		{
			return false;
		}
		unguard;
	}

	bool Is(char vNetChannel)//��Ƶ���Ƿ���ָ������ͨѶ��ʶ��Ƶ��
	{
		guard;
		if(mNetChannel==vNetChannel)
		{
			return true;
		}
		else
		{
			return false;
		}
		unguard;
	}

	EChannels		mChannel;				//Ƶ��ö��
	char			mNetChannel;			//����ͨѶ��ʶ
	int				mMask;					//���������������
	std::string		mDescription;			//Ƶ����ʾʱ��ǰ׺
	DWORD			mColor;					//Ƶ������ɫ
};

#endif//GC_CHAT_ASSISTANT_H 
