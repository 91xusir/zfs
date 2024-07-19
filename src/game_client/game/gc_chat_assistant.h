

#ifndef GC_CHAT_ASSISTANT_H 
#define GC_CHAT_ASSISTANT_H 

#include <string>
typedef unsigned long       DWORD;      

#define MAX_CHANNEL_CHATBOX 7

//enum EChanneLists
//{
//	ALL_LIST = 0,				// 所有频道列表
//	NEAR_LIST,					// 附近
//	TEAM_LIST,					// 队伍频道列表
//	UNION_LIST,					// 帮派频道列表
//	BATTLE_LIST,				// 战场频道
//	WORLD_LIST,					// 世界频道列表
//	PERSONAL_LIST,				// 密语频道
//	MAX_LIST
//};


enum EChannels
{
	NEAR_CHANNEL = 0,			// 附近频道
	TEAM_CHANNEL,				// 队伍频道
	UNION_CHANNEL,				// 帮派频道
	BATTLE_CHANNEL,				// 战场频道
	WORLD_CHANNEL,				// 世界频道
	PERSONAL_CHANNEL,			// 密语频道
	BATTLEINFO_CHANNEL,
	SYSTEM_CHANNEL,				// 系统频道
	GM_CHSNNEL,					// GM频道
	SPEAKER_CHANNEL,			// 喇叭频道
	MAX_CHANNEL
};

#define MAX_CHANNEL_BUTTON MAX_CHANNEL-3
class SChannelAtom
{
public:
	//频道枚举//网络通讯标识//界面过滤器的掩码//频道显示时的前缀//频道的颜色
	SChannelAtom ( EChannels vChannel,char vNetChannel,int vMask,std::string vDescription,DWORD vColor)
		:mChannel(vChannel),
		mNetChannel(vNetChannel),
		mMask(vMask),
		mDescription(vDescription),
		mColor(vColor)
	{
	}

	bool Is(EChannels vChannel)//该频道是否是指定频道枚举的频道
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

	bool Is(char vNetChannel)//该频道是否是指定网络通讯标识的频道
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

	EChannels		mChannel;				//频道枚举
	char			mNetChannel;			//网络通讯标识
	int				mMask;					//界面过滤器的掩码
	std::string		mDescription;			//频道显示时的前缀
	DWORD			mColor;					//频道的颜色
};

#endif//GC_CHAT_ASSISTANT_H 
