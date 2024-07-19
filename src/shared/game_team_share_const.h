
#ifndef GAME_TEAM_SHARE_CONST_H
#define GAME_TEAM_SHARE_CONST_H

//客户端到Region
const unsigned char C2R_INVITE_OTHER=0;				// 自己邀请别人加入队伍[DWORD vOtherRegionID]
const unsigned char C2R_BANISH_OTHER=1;				// 自己驱逐别人从队伍中[DWORD vOtherDBID]
const unsigned char C2R_SELF_LEAVE=2;				// 自己离开队伍
const unsigned char C2R_DISMISS=3;					// 自己解散队伍
const unsigned char C2R_AGREE_OTHER_INVITE=4;		// 自己同意别人的邀请[DWORD vOtherDBID]
const unsigned char C2R_REFUSE_OTHER_INVITE=5;		// 自己拒绝别人的邀请[DWORD vOtherDBID]
const unsigned char C2R_BANISH_OTHER_BY_REGION_ID=6;// 驱逐别人用RegionID[DWORD vOtherRegionID]
const unsigned char C2R_TEAM_ONOFF=7;				// 组队开关
const unsigned char C2R_APPLY_TEAM=8;				// 申请加入某个队伍
const unsigned char C2R_AGREE_APPLY=9;				// 同意别人申请
const unsigned char C2R_REFUSE_APPLY=10;			// 拒绝别人申请
const unsigned char C2R_TRANSFER_CAPTAIN=11;		// 队长转让
const unsigned char C2R_SELECTED_BY_DBID=12;		// 用DBID来选中队员
const unsigned char C2R_INVITE_OTHER_BY_NAME = 13;
const unsigned char C2R_APPLY_TEAM_BY_NAME   = 14;

const unsigned char C2R_SET_PICK_TYPE = 14;//队长设置拾取模式

//Region到客户端
const unsigned char R2C_ADD_TEAM_MEMBER=0;			//增加一个队友[DWORD vMemberDBID,char* vpMemberName]
const unsigned char R2C_DELETE_TEAM_MEMBER=1;		//减少一个队友[DWORD vMemberDBID]
const unsigned char R2C_UPDATE_ONE_MEMBER_DATA=2;	//刷新某个成员的数据[DWORD vMemberDBID,int vHp,int vMaxHp,int vMp,int vMaxMp]
const unsigned char R2C_UPDATE_ALL_MEMBER_DATA=3;	//刷新所有成员的数据[char* vpTeamData]string中存放TeamData信息
const unsigned char R2C_TEAM_MESSAGE=4;				//刷一个消息[int vMessageID]
const unsigned char R2C_TEAM_MESSAGE_WITH_ID=5;		//刷一个消息[DWORD vDBIDInMessage,int vMessageID]


//-------别人-----------
const unsigned char R2C_OTHER_AGREE_INVITE=6;		//别人同意加入队伍[DWORD vOtherDBID,char* vpOtherName]
const unsigned char R2C_OTHER_REFUSE_INVITE=7;		//别人拒绝加入队伍[DWORD vOtherDBID,char* vpOtherName]
const unsigned char R2C_OTHER_INVITE=8;				//别人邀请自己加入队伍[DWORD vOtherDBID,char* vpOtherName]
const unsigned char R2C_OTHER_BANISH=9;				//别人驱逐自己出队伍[DWORD vOtherDBID]
const unsigned char R2C_OTHER_LEAVE=10;				//别人离开队伍[DWORD vOtherDBID]
const unsigned char R2C_OTHER_DISMISS=11;			//别人解散队伍[DWORD vOtherDBID]
const unsigned char R2C_OTHER_APPLY=12;				//别人申请加入自己队伍
const unsigned char R2C_OTHER_AGREE_APPLY=13;		//别人同意自己申请
const unsigned char R2C_OTHER_REFUSE_APPLY=14;		//别人拒绝自己申请
const unsigned char R2C_TRANSFER_CAPTAIN = 15;		//队长转让
const unsigned char R2C_SELECTED_BY_DBID=16;		// 用DBID来选中队员

const unsigned char R2C_SET_PICK_TYPE = 17;//队长设置拾取模式


//Region到GameWorld[在最前面加DWORD vFromID]
const unsigned char R2GW_INVITE_OTHER=0;			//自己邀请别人加入队伍[DWORD vOtherDBID]
const unsigned char R2GW_BANISH_OTHER=1;			//自己驱逐别人从队伍中[DWORD vOtherDBID]
const unsigned char R2GW_SELF_LEAVE=2;				//自己离开队伍
const unsigned char R2GW_DISMISS=3;					//自己解散队伍
const unsigned char R2GW_AGREE_OTHER_INVITE=4;		//自己同意别人的邀请[DWORD vOtherDBID]
const unsigned char R2GW_REFUSE_OTHER_INVITE=5;		//自己拒绝别人的邀请[DWORD vOtherDBID]
const unsigned char R2GW_DATE_CHANGE = 6;				//自己的数据变化影响到队伍其他成员[int vHp,int vMaxHp,int vMp,int vMaxMp]
const unsigned char R2GW_TEAM_ONOFF = 7;
const unsigned char R2GW_APPLY_ADDTEAM = 8;
const unsigned char R2GW_OTHER_AGREE_APPLY =9;
const unsigned char R2GW_REFUSE_TEAM=10;
const unsigned char R2GW_TRANSFER_CAPTAIN = 11;		//队长转让
const unsigned char R2GW_INVITE_OTHER_BY_NAME = 12;
const unsigned char R2GW_INVITE_OTHER_BY_DBID = 13;
const unsigned char R2GW_SETREFUSE_TEAM = 16;  //设置拒绝组队消息

const unsigned char R2GW_SET_PICK_TYPE = 14;//队长设置拾取模式
const unsigned char R2GW_TEAM_PICKER = 15;//组队时通知有人拾了东西了


//GameWorld到Region[在最前面加DWORD vSendToDBID]
const unsigned char GW2R_ADD_TEAM_MEMBER=0;			//增加一个队友[DWORD vMemberDBID,char* vpMemberName]
const unsigned char GW2R_DELETE_TEAM_MEMBER=1;		//减少一个队友[DWORD vMemberDBID]
const unsigned char GW2R_UPDATE_ONE_MEMBER_DATA=2;	//刷新某个成员的数据[DWORD vMemberDBID,int vHp,int vMaxHp,int vMp,int vMaxMp]
const unsigned char GW2R_UPDATE_ALL_MEMBER_DATA=3;	//刷新所有成员的数据[char* vpTeamData]string中存放TeamData信息
const unsigned char GW2R_REQUEST_MEMBER_DATA=4;		//要求region刷新自己的数据到GameWorld[无]
const unsigned char GW2R_TEAM_MESSAGE=5;				//刷一个消息[int vMessageID]
const unsigned char GW2R_TEAM_MESSAGE_WITH_ID=6;		//刷一个消息[带ID][DWORD vDBIDInMessage,int vMessageID]

//-------别人-----------
const unsigned char GW2R_OTHER_AGREE_INVITE=7;		//别人同意加入队伍[DWORD vOtherDBID,char* vpOtherName]
const unsigned char GW2R_OTHER_REFUSE_INVITE=8;		//别人拒绝加入队伍[DWORD vOtherDBID,char* vpOtherName]
const unsigned char GW2R_OTHER_INVITE=9;				//别人邀请自己加入队伍[DWORD vOtherDBID,char* vpOtherName]
const unsigned char GW2R_OTHER_BANISH=10;			//别人驱逐自己出队伍[DWORD vOtherDBID]
const unsigned char GW2R_OTHER_LEAVE=11;				//别人离开队伍[DWORD vOtherDBID]
const unsigned char GW2R_OTHER_DISMISS=12;			//别人解散队伍[DWORD vOtherDBID]

const unsigned char GW2R_APPLY_ADDTEAM = 13;
const unsigned char GW2R_REFUSE_TEAM=14;
const unsigned char GW2R_TRANSFER_CAPTAIN = 15;		//队长转让
const unsigned char GW2R_INVITE_OTHER_BY_NAME = 16;		//队长转让

const unsigned char GW2R_SET_PICK_TYPE = 17;//队长设置拾取模式

const unsigned char GW2R_TEAM_PICKER = 18;//组队时通知有人拾了东西了
const unsigned char GW2R_SETREFUSE_TEAM =19/*6*/; //别人设置了拒绝组队

const unsigned char GW2R_TO_WHENFRIEND = 20; 
/*
//Region到Region
//-----------服务器间---------------
const unsigned char R2R_ADD_TEAM_MEMBER=0;			//增加一个队友
const unsigned char R2R_DELETE_TEAM_MEMBER=1;		//减少一个队友
const unsigned char R2R_UPDATE_ONE_MEMBER_DATA=2;	//刷新某个成员的数据
const unsigned char R2R_TEAM_MESSAGE=3;				//刷一个消息
const unsigned char R2R_TEAM_MESSAGE_WITH_ID=4;		//刷一个消息[带ID]

//-------别人-----------
const unsigned char R2R_SELF_ADD_OTHER_SUCCEED=5;	//自己将别人加入到自己队伍成功
const unsigned char R2R_SELF_ADD_OTHER_FAULT=6;		//自己将别人加入到自己队伍失败
const unsigned char R2R_SELF_AGREE_INVITE=7;			//自己同意加入别人的队伍
const unsigned char R2R_SELF_REFUSE_INVITE=8;		//自己拒绝加入别人的队伍
const unsigned char R2R_SELF_INVITE=9;				//自己邀请别人加入队伍
const unsigned char R2R_SELF_BANISH=10;				//自己驱逐别人出队伍
const unsigned char R2R_SELF_LEAVE=11;				//自己离开队伍
const unsigned char R2R_SELF_DISMISS=12;				//自己解散队伍


const unsigned char R2R_OTHER_REGION_ID_NOT_EXIST=13;//别人的RegionID不存在
const unsigned char R2R_OTHER_DB_ID_NOT_EXIST=14;	//别人的DBID不存在
const unsigned char R2R_FORCE_UPDATE_DATA=15;		//强制一个队友刷新自己的数据到另一个队友

const unsigned char R2R_OTHER_ADD_SELF_SUCCEED=R2R_SELF_ADD_OTHER_SUCCEED;	//别人将你成功加入队伍
const unsigned char R2R_OTHER_ADD_SELF_FAULT=R2R_SELF_ADD_OTHER_FAULT;		//别人将你加入失败
const unsigned char R2R_OTHER_AGREE_INVITE=R2R_SELF_AGREE_INVITE;			//别人同意加入队伍
const unsigned char R2R_OTHER_REFUSE_INVITE=R2R_SELF_REFUSE_INVITE;			//别人拒绝加入队伍
const unsigned char R2R_OTHER_INVITE=R2R_SELF_INVITE;						//别人邀请自己加入队伍
const unsigned char R2R_OTHER_BANISH=R2R_SELF_BANISH;						//别人驱逐自己出队伍
const unsigned char R2R_OTHER_LEAVE=R2R_SELF_LEAVE;							//别人离开队伍
const unsigned char R2R_OTHER_DISMISS=R2R_SELF_DISMISS;						//别人解散队伍
*/




//文字信息再关联一次
//再用数字做一次关联
const int NULL_MESSAGE=0;
const int TEAM_COUNT_OVER=1;
const int NOT_TEAM_CAPTAIN=2;
const int TEAM_NOT_EXIST=3;
const int BANISH_MEMBER=4;
const int MEMBER_NOT_EXIST=5;
const int SELF_LEFT_TEAM=6;
const int TEAM_DISMISS=7;
const int MEMBER_ADD_TEAM=8;
const int REFUSE_INVITE=9;
const int INVITE_ADD_TEAM=10;

const int BANISH_FROM_TEAM=11;
const int LEAVE_TEAM=12;
const int ALREADY_IN_TEAM=13;
const int CHOOSE_MEMBER_ADD_TEAM=14;
const int CHOOSE_MEMBER_BANISH_FROM_TEAM=15;
const int ADD_TEAM_SUCCEED=16; 
const int THAT_PLAY_ALREADY_ADD_OTHER_TEAM=17;
const int NOT_MEMBER_TEAM_DISMISS=18;
const int THAT_OBJECT_NOT_PLAYER=19;
const int THAT_PLAYER_NOT_INLINE=20;

const int TEAM_LOCAL_NOT_THIS_DBID_NAME=21;
const int TEAM_WAIT_PLAYER_RESPOND=22;
const int TEAM_OBJECT_NOT_IN_REGION=23;						//物体不在该区域！
const int THAT_PLAYER_TEAM_OFF=24;							//玩家关闭组队开关！


/*

//------对服务端--------
//发送消息到服务端
struct SR2rData
{
	void BeginSend(){mOperation=0;mDBIDInMessage=0;mFromDBID=0;mpFromName=NULL;
	mParameter=0;mHp=0;mMaxHp=0;mMp=0;mMaxMp=0;mpName=NULL;}

	unsigned char	mOperation;
	DWORD	mDBIDInMessage;
	DWORD	mFromDBID;
	char*	mpFromName;

	DWORD	mParameter;
	int		mHp;
	int		mMaxHp;
	int		mMp;
	int		mMaxMp;
	char*	mpName;
};

*/


#endif//GAME_TEAM_SHARE_CONST_H
