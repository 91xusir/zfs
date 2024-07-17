//********************************************************************
//	created:	2010.05.31
//	filename: 	union_base.cpp
//	author:		ldr123
//	purpose:	游戏中,帮派系统基本操作重做
//  other:		version 1.0
//********************************************************************
 
#ifndef UNION_BASE_H
#define UNION_BASE_H

using namespace std;

class CG_CmdPacket;

// 采邑税率
#define UNION_TAXRATE_MIN		10				// 最小税率
#define UNION_TAXRATE_MAX		40				// 最大税率

const int UNION_ENOUNCE_STRING_MAX = 160;

const char UNION_DEFAULT_ICON_NAME[] = "0";

const int UNION_MAX_LEVEL = 5;
const int UNION_MAX_SCORE = 100000;

const int UNION_CREATE_USE_ITEMID	= 470;

const int UNION_BULLETIN_STRING_MAX = 400;		/*公告文字最大长度*/
const int UNION_NAME_STRING_MAX		= 16;		/*名字最大长度*/
const int UNION_CREATE_USE_MONEY	= 100000;	/*创建帮派需要100000银币*/
const int UNION_CREATE_USE_LEVEL	= 30;		/*创建帮派需要等级达到30级*/
const int UNION_MIN_CITIZEN			= 10;		/*帮派最小人数为10人*/
const int UNION_MAX_CITIZEN			= 50;		/*帮派最多人数为50人*/
const int UNION_MAX_KING			= 1;		/*帮主*/
const int UNION_MAX_SAFELEFT		= 1;		/*左护法*/
const int UNION_MAX_SAFERIGHT		= 1;		/*右护法*/
const int UNION_MAX_ELDER			= 4;		/*长老*/
const int UNION_MAX_SUBLEADER		= -1;		/*堂主:不好界定,最大数字-其他人数*/
const int UNION_MAX_MASSES			= -1;		/*帮众:不好界定,最大数字-其他人数*/
const int UNION_MAX_HISTORYLIST		= 300;		/*历史记录最多300条*/
const int UNION_MAX_ALLIANCE		= 8;		/*最大同盟帮派数*/

//职位:帮主 右护法 左护法 长老 堂主 帮众
enum EUnionOfficial
{
	UO_King,			//帮主
	UO_safeguardLeft,	//左护法
	UO_safeguardRight,	//右护法
	UO_Elder,			//长老
	UO_SubLeader,		//堂主
	UO_Masses,			//平民
};

//帮派操作
enum EUnionAction
{   							/*权限名称		   帮主	  左右护法	长老   堂主		帮众*/
	UA_ADDMEMBER,				/*添加帮派成员		√		√							*/
	UA_INVITEMEMBER,			/*邀请加入帮派		√		√		√		√			*/
	UA_BANISHMENT,				/*逐出帮派			√		√		√					*/
	UA_LEAVEUNION,				/*脱离帮派			 		√		√		√		 √	*/
	UA_DEMISEUNION,				/*禅让帮主			√									*/
	UA_LEVELCHANGE,				/*更改职位			√		√		√					*/
	UA_UNIONDISBAND,			/*解散帮派			√									*/
	UA_USECONTRIBUTION,			/*使用帮派仓库		√		√							*/
	UA_VIEWCONTRIBUTION,		/*查看帮派仓库		√		√		√		√			*/
	UA_CONTRIBUTION,			/*帮派贡献转换		√		√		√		√		 √	*/
	UA_UNIONTASK,				/*领取帮派任务		√		√		√		√		 √	*/
	UA_MONDIFYPOST,				/*制作帮派公告		√		√		√					*/
	UA_CHANGEUNIONPIC,			/*登陆帮派图标		√									*/
	UA_DISABLECHANNEL,			/*频道禁言			√		√		√					*/
};

struct SUnionCitizen
{
	SUnionCitizen():
					m_nPlayerID(-1), m_nRegionID(-1), m_bOnLine(false), m_sLevel(-1), m_eUnionOfficial(UO_Masses), 
					m_strPlayerName(""), m_strNickName(""), m_cMetier(0), m_nContribution(-1){}

	~SUnionCitizen() {}

	bool Serialize(CG_CmdPacket *cmd);

	char			m_cMetier;			//职业
	DWORD			m_nContribution;	//帮派贡献
	DWORD			m_nPlayerID;		//玩家id
	DWORD			m_nRegionID;		//region ID
	bool			m_bOnLine;			//是否在线
	short			m_sLevel;			//玩家等级
	EUnionOfficial	m_eUnionOfficial;	//玩家官职
	std::string		m_strPlayerName;	//玩家名字
	std::string		m_strNickName;		//称号


	friend bool operator==(const SUnionCitizen &t1, const SUnionCitizen &t2)
	{
		return (
			t1.m_nPlayerID==t2.m_nPlayerID && t1.m_nRegionID==t2.m_nRegionID && t1.m_bOnLine==t2.m_bOnLine 
			&& t1.m_sLevel==t2.m_sLevel && t1.m_eUnionOfficial==t2.m_eUnionOfficial 
			&& t1.m_strPlayerName==t2.m_strPlayerName && t1.m_strNickName==t2.m_strNickName 
			&& t1.m_nContribution==t2.m_nContribution && t1.m_cMetier==t2.m_cMetier
			);
	}
};

//职业合法?
inline bool UnionOfficial_IsValid(const EUnionOfficial& official)	{return (official>=UO_King && official<=UO_Masses);}
inline bool UnionOfficial_IsValid(const SUnionCitizen& player)	{return UnionOfficial_IsValid(player.m_eUnionOfficial);}


//帮派日志类型
#define LOGUNIONUNKNOW	0x00	/*未知操作*/
#define LOGCREATEUNION	0x01	/*创建帮派*/
#define LOGJOINUNION	0x02	/*加入帮派*/
#define LOGLEAVEUNION	0x03	/*脱离帮派*/
#define LOGBANISHMENT	0x04	/*被驱逐出帮派*/
#define LOGLEVELUP		0x05	/*职位提升*/
#define LOGLEVELDOWN	0x06	/*职位下降*/
#define LOGCONTRIBUTION	0x07	/*帮派贡献*/
#define LOGDEMISEUNION	0x08	/*禅让帮主*/
#define LOGMONDIFYPOST	0x09	/*修改公告*/
#define LOGUSEMONEY		0x0a	/*取帮派金钱*/

//帮派日志
struct SUnionLog
{
	SUnionLog():m_nLogTime(0), m_nLogType(0), m_strLog(""){}

	bool Serialize(CG_CmdPacket *cmd);
	std::string GetLogString();

	DWORD		m_nLogType;	//日志类型
	DWORD		m_nLogTime;	//日志时间(时间戳)
	std::string m_strLog;	//日志内容

	const SUnionLog& operator=(const SUnionLog& dataSrc)
	{
		m_nLogTime	= dataSrc.m_nLogTime;
		m_nLogType	= dataSrc.m_nLogType;
		m_strLog	= dataSrc.m_strLog;

		return *this;
	}

	friend bool operator==(const SUnionLog &t1, const SUnionLog &t2)
	{
		return (t1.m_nLogTime==t2.m_nLogTime && t1.m_nLogType==t2.m_nLogType && t1.m_strLog==t2.m_strLog);
	}
};

struct SUnionData
{
	enum{eItemTypeNum = 4};
    void Clear()
    {
		m_nPic			= 0;
        m_nID			= 0;
		m_nStrength		= 0;
		//m_nFacMoney		= -1;
		m_nPeopleNum	= 0;
		m_strName		= "";
		m_strChatPost	= "";
		m_nCaptainID	= 0;
		m_nLevel		= 0;
		m_nFighting		= 0;		//战斗力
		m_bGetChangeEnable = false;

		m_nKingNum		= 0;
		m_nSafeLeftNum	= 0;
		m_nSafeRightNum	= 0;
		m_nElderNum		= 0;
		m_nSubleaderNum	= 0;
		m_nMassesNum	= 0;

		dwMoney		= 0;
		int i;
		for (i = 0; i < eItemTypeNum; i++)
		{
			m_Item[i] = 0;
		}

		for (int i=0; i<UNION_MAX_ALLIANCE; ++i)
		{
			m_nAlliance[i] = 0;
		}

        vecUnionCitizen.clear();
		vecLogList.clear();
    }

	SUnionData()
	{
        Clear();
	}

	bool Union_ChangeOfficialEnable(const EUnionOfficial &eOfficial);


	~SUnionData() {}

	bool Serialize(CG_CmdPacket *cmd);

	SUnionCitizen*	Find(DWORD playerID);
	SUnionCitizen*	Find(std::string playerName);

	bool AddCitizen(SUnionCitizen &citizen);	// 增加公民
	bool RemoveCitizen(DWORD playerID);			// 删除公民

	int			m_nLevel;		//要塞等级..未用到
	int			m_nCaptainID;	//老大的id
	int			m_nPic;			//图腾id
	DWORD		m_nID;			//帮派id
	int			m_nStrength;	//帮派势力
	//int			m_nFacMoney;	//帮派money
	int			m_nPeopleNum;	//帮派人数
	std::string m_strName;		//帮派名称
	std::string	m_strChatPost;	//帮派公告列表
	DWORD		m_nFighting;	//战斗力

	int			m_nAlliance[UNION_MAX_ALLIANCE];	//同盟帮派id
	std::vector<SUnionCitizen>	vecUnionCitizen;	//帮派成员
	std::vector<SUnionLog>		vecLogList;			//帮派操作历史记录

	bool		m_bGetChangeEnable;	//是否更改内容

	unsigned long dwMoney;
	unsigned long m_Item[eItemTypeNum];

	// const int UNION_MAX_KING			= 1;		/*帮主*/
	// const int UNION_MAX_SAFELEFT		= 1;		/*左护法*/
	// const int UNION_MAX_SAFERIGHT		= 1;		/*右护法*/
	// const int UNION_MAX_ELDER			= 4;		/*长老*/
	// const int UNION_MAX_SUBLEADER		= -1;		/*堂主:不好界定,最大数字-其他人数*/
	// const int UNION_MAX_MASSES			= -1;		/*帮众:不好界定,最大数字-其他人数*/

	int m_nKingNum;
	int m_nSafeLeftNum;
	int m_nSafeRightNum;
	int m_nElderNum;
	int m_nSubleaderNum;
	int m_nMassesNum;

	const SUnionData& operator=(const SUnionData& dataSrc)
	{
		Clear();

		m_nPic			= dataSrc.m_nPic;
		m_nID			= dataSrc.m_nID;
		m_nStrength		= dataSrc.m_nStrength;
		m_nPeopleNum	= dataSrc.m_nPeopleNum;
		m_strName		= dataSrc.m_strName;
		m_strChatPost	= dataSrc.m_strChatPost;
		m_nLevel		= dataSrc.m_nLevel;
		m_nFighting		= dataSrc.m_nFighting;

		vecUnionCitizen = dataSrc.vecUnionCitizen;
		vecLogList		= dataSrc.vecLogList;

		dwMoney			= dataSrc.dwMoney;

		m_nKingNum		= dataSrc.m_nKingNum;
		m_nSafeLeftNum	= dataSrc.m_nSafeLeftNum;
		m_nSafeRightNum	= dataSrc.m_nSafeRightNum;
		m_nElderNum		= dataSrc.m_nElderNum;
		m_nSubleaderNum	= dataSrc.m_nSubleaderNum;
		m_nMassesNum	= dataSrc.m_nMassesNum;

		m_nCaptainID	= dataSrc.m_nCaptainID;

		int i;
		for (i = 0; i < eItemTypeNum; i++)
		{
			m_Item[i] = dataSrc.m_Item[i];
		}

		return *this;
	}
};

//操作对应的级别判断
bool Union_GetEnableFromAuction(const EUnionAction& eUnionAction, SUnionCitizen* user1, SUnionCitizen *user2=NULL);
//end

//以下暂时未用到
//采邑
struct SFeod
{
	std::string	name;
	DWORD		unionID;		// 所属诸侯国
	int			taxRate;		// 当前税率
	int			bank;			// 国库
	int			incomeToday;	// 今日税收

	void Clear()
	{
		name = "";
		unionID = 0;
		taxRate = UNION_TAXRATE_MIN;
		bank = 0;
		incomeToday = 0;
	}

	SFeod() { Clear(); }
	~SFeod() {}
	bool Serialize(CG_CmdPacket *cmd);
};

const int FEOD_COUNT	= 3;
const std::string FeodNameInDB[FEOD_COUNT] = { "XiQi", "ZhaoGe", "ChuanYun" };
const std::string CastleWarAreaName[FEOD_COUNT] = {"西岐城","朝歌城","穿云关"};

#define FEODNAME_MAX	20


#ifndef BOOL_ENSURE
#define BOOL_ENSURE(p)		{ if ( !(p) ) return false; }
#endif

#endif