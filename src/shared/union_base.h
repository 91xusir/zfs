//********************************************************************
//	created:	2010.05.31
//	filename: 	union_base.cpp
//	author:		ldr123
//	purpose:	��Ϸ��,����ϵͳ������������
//  other:		version 1.0
//********************************************************************
 
#ifndef UNION_BASE_H
#define UNION_BASE_H

using namespace std;

class CG_CmdPacket;

// ����˰��
#define UNION_TAXRATE_MIN		10				// ��С˰��
#define UNION_TAXRATE_MAX		40				// ���˰��

const int UNION_ENOUNCE_STRING_MAX = 160;

const char UNION_DEFAULT_ICON_NAME[] = "0";

const int UNION_MAX_LEVEL = 5;
const int UNION_MAX_SCORE = 100000;

const int UNION_CREATE_USE_ITEMID	= 470;

const int UNION_BULLETIN_STRING_MAX = 400;		/*����������󳤶�*/
const int UNION_NAME_STRING_MAX		= 16;		/*������󳤶�*/
const int UNION_CREATE_USE_MONEY	= 100000;	/*����������Ҫ100000����*/
const int UNION_CREATE_USE_LEVEL	= 30;		/*����������Ҫ�ȼ��ﵽ30��*/
const int UNION_MIN_CITIZEN			= 10;		/*������С����Ϊ10��*/
const int UNION_MAX_CITIZEN			= 50;		/*�����������Ϊ50��*/
const int UNION_MAX_KING			= 1;		/*����*/
const int UNION_MAX_SAFELEFT		= 1;		/*�󻤷�*/
const int UNION_MAX_SAFERIGHT		= 1;		/*�һ���*/
const int UNION_MAX_ELDER			= 4;		/*����*/
const int UNION_MAX_SUBLEADER		= -1;		/*����:���ý綨,�������-��������*/
const int UNION_MAX_MASSES			= -1;		/*����:���ý綨,�������-��������*/
const int UNION_MAX_HISTORYLIST		= 300;		/*��ʷ��¼���300��*/
const int UNION_MAX_ALLIANCE		= 8;		/*���ͬ�˰�����*/

//ְλ:���� �һ��� �󻤷� ���� ���� ����
enum EUnionOfficial
{
	UO_King,			//����
	UO_safeguardLeft,	//�󻤷�
	UO_safeguardRight,	//�һ���
	UO_Elder,			//����
	UO_SubLeader,		//����
	UO_Masses,			//ƽ��
};

//���ɲ���
enum EUnionAction
{   							/*Ȩ������		   ����	  ���һ���	����   ����		����*/
	UA_ADDMEMBER,				/*��Ӱ��ɳ�Ա		��		��							*/
	UA_INVITEMEMBER,			/*����������		��		��		��		��			*/
	UA_BANISHMENT,				/*�������			��		��		��					*/
	UA_LEAVEUNION,				/*�������			 		��		��		��		 ��	*/
	UA_DEMISEUNION,				/*���ð���			��									*/
	UA_LEVELCHANGE,				/*����ְλ			��		��		��					*/
	UA_UNIONDISBAND,			/*��ɢ����			��									*/
	UA_USECONTRIBUTION,			/*ʹ�ð��ɲֿ�		��		��							*/
	UA_VIEWCONTRIBUTION,		/*�鿴���ɲֿ�		��		��		��		��			*/
	UA_CONTRIBUTION,			/*���ɹ���ת��		��		��		��		��		 ��	*/
	UA_UNIONTASK,				/*��ȡ��������		��		��		��		��		 ��	*/
	UA_MONDIFYPOST,				/*�������ɹ���		��		��		��					*/
	UA_CHANGEUNIONPIC,			/*��½����ͼ��		��									*/
	UA_DISABLECHANNEL,			/*Ƶ������			��		��		��					*/
};

struct SUnionCitizen
{
	SUnionCitizen():
					m_nPlayerID(-1), m_nRegionID(-1), m_bOnLine(false), m_sLevel(-1), m_eUnionOfficial(UO_Masses), 
					m_strPlayerName(""), m_strNickName(""), m_cMetier(0), m_nContribution(-1){}

	~SUnionCitizen() {}

	bool Serialize(CG_CmdPacket *cmd);

	char			m_cMetier;			//ְҵ
	DWORD			m_nContribution;	//���ɹ���
	DWORD			m_nPlayerID;		//���id
	DWORD			m_nRegionID;		//region ID
	bool			m_bOnLine;			//�Ƿ�����
	short			m_sLevel;			//��ҵȼ�
	EUnionOfficial	m_eUnionOfficial;	//��ҹ�ְ
	std::string		m_strPlayerName;	//�������
	std::string		m_strNickName;		//�ƺ�


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

//ְҵ�Ϸ�?
inline bool UnionOfficial_IsValid(const EUnionOfficial& official)	{return (official>=UO_King && official<=UO_Masses);}
inline bool UnionOfficial_IsValid(const SUnionCitizen& player)	{return UnionOfficial_IsValid(player.m_eUnionOfficial);}


//������־����
#define LOGUNIONUNKNOW	0x00	/*δ֪����*/
#define LOGCREATEUNION	0x01	/*��������*/
#define LOGJOINUNION	0x02	/*�������*/
#define LOGLEAVEUNION	0x03	/*�������*/
#define LOGBANISHMENT	0x04	/*�����������*/
#define LOGLEVELUP		0x05	/*ְλ����*/
#define LOGLEVELDOWN	0x06	/*ְλ�½�*/
#define LOGCONTRIBUTION	0x07	/*���ɹ���*/
#define LOGDEMISEUNION	0x08	/*���ð���*/
#define LOGMONDIFYPOST	0x09	/*�޸Ĺ���*/
#define LOGUSEMONEY		0x0a	/*ȡ���ɽ�Ǯ*/

//������־
struct SUnionLog
{
	SUnionLog():m_nLogTime(0), m_nLogType(0), m_strLog(""){}

	bool Serialize(CG_CmdPacket *cmd);
	std::string GetLogString();

	DWORD		m_nLogType;	//��־����
	DWORD		m_nLogTime;	//��־ʱ��(ʱ���)
	std::string m_strLog;	//��־����

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
		m_nFighting		= 0;		//ս����
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

	bool AddCitizen(SUnionCitizen &citizen);	// ���ӹ���
	bool RemoveCitizen(DWORD playerID);			// ɾ������

	int			m_nLevel;		//Ҫ���ȼ�..δ�õ�
	int			m_nCaptainID;	//�ϴ��id
	int			m_nPic;			//ͼ��id
	DWORD		m_nID;			//����id
	int			m_nStrength;	//��������
	//int			m_nFacMoney;	//����money
	int			m_nPeopleNum;	//��������
	std::string m_strName;		//��������
	std::string	m_strChatPost;	//���ɹ����б�
	DWORD		m_nFighting;	//ս����

	int			m_nAlliance[UNION_MAX_ALLIANCE];	//ͬ�˰���id
	std::vector<SUnionCitizen>	vecUnionCitizen;	//���ɳ�Ա
	std::vector<SUnionLog>		vecLogList;			//���ɲ�����ʷ��¼

	bool		m_bGetChangeEnable;	//�Ƿ��������

	unsigned long dwMoney;
	unsigned long m_Item[eItemTypeNum];

	// const int UNION_MAX_KING			= 1;		/*����*/
	// const int UNION_MAX_SAFELEFT		= 1;		/*�󻤷�*/
	// const int UNION_MAX_SAFERIGHT		= 1;		/*�һ���*/
	// const int UNION_MAX_ELDER			= 4;		/*����*/
	// const int UNION_MAX_SUBLEADER		= -1;		/*����:���ý綨,�������-��������*/
	// const int UNION_MAX_MASSES			= -1;		/*����:���ý綨,�������-��������*/

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

//������Ӧ�ļ����ж�
bool Union_GetEnableFromAuction(const EUnionAction& eUnionAction, SUnionCitizen* user1, SUnionCitizen *user2=NULL);
//end

//������ʱδ�õ�
//����
struct SFeod
{
	std::string	name;
	DWORD		unionID;		// ��������
	int			taxRate;		// ��ǰ˰��
	int			bank;			// ����
	int			incomeToday;	// ����˰��

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
const std::string CastleWarAreaName[FEOD_COUNT] = {"��᪳�","�����","���ƹ�"};

#define FEODNAME_MAX	20


#ifndef BOOL_ENSURE
#define BOOL_ENSURE(p)		{ if ( !(p) ) return false; }
#endif

#endif