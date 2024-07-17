#ifndef __LOGIN_H__
#define __LOGIN_H__

#include <time.h>

#include "net/rt_net.h"
#include "rt1tort2.h"
#include "server_config.h"
#include "dbmysql.h"
#include "lgs_gws.h"
#include "gs_protocol.h"
#include "guide_server.h"
#include "gamigo_adb_session.h"
#include "charge_session.h"
#include "wsdl/WsdlWrapper.h"

extern CWsdlWrapper* g_pWSDL;

extern char* g_strStaticBuffer;
class CLoginServer;
class CGameWorldStub;

const int RET_NONE						= 0;
const int RET_ADD_CMD					= 1;
const int RET_READ_PACKET_ERROR			= 2;
const int RET_GET_FIELD_ERROR			= 3;
const int RET_GET_ROW_ERROR				= 4;
const int RET_PUBWIN_ERROR				= 5;

enum EUserState 
{
	USER_STATE_NONE,
	USER_STATE_ALIVE,
	USER_STATE_GWS_LOST,
};

struct SUserInfo 
{
	SAccountInfo	info; 		// keep a copy in gws for recover
	EUserState		state;
	CGameWorldStub *gwsStub;

    time_t          tLastKeepOnlineMsg; // 最后一次收到保持在线的信息的时间，可以认为过一段时间没有收到就是客户端已经退出
    int             iKickCount;

    bool            bReceivePointInfo;  // 是否收到过点数信息
    char            cVIP;               // 是否是VIP
    short           sActorLevel;        // 角色等级, 如果为0表示还没有进入游戏
    time_t          tChargeExpire;      // 过期(0为没有过期)
    time_t          tChargeNextCheck;   // 下次检查(0为不检查)
    int             iChargePoint;       // 点数
    //char            szChargeMsg[100];   // 计费系统最后一次返回的信息
	CM_MEMDEF(szChargeMsg, 100)
    SUserInfo()
    {
		CM_MEMPROTECTOR(szChargeMsg, 100)
        gwsStub = 0;

        tLastKeepOnlineMsg = 0;
        iKickCount = 0;

        bReceivePointInfo = false;
        tChargeExpire = 0;
        tChargeNextCheck = 0;
        iChargePoint = 0;
        sActorLevel = 0;
        szChargeMsg[0] = 0;
        cVIP = 0;
    }
	~SUserInfo()
	{
		CM_MEMUNPROTECTOR(szChargeMsg)
	}
};

const int LIST_CMD_USER_LOGIN		    =  1;   // 登陆
const int LIST_CMD_USER_QUIT		    =  2;	// 退出
const int LIST_CMD_USER_LOGIN_RET       =  3;   // 登陆返回
const int LIST_CMD_LOG_CONCURRENT       =  4;   // 记录当前在线
const int LIST_CMD_RET                  = 10;   // 普通返回
const int LIST_CMD_ACCOUNT_CREATE       = 11;   // 创建帐号
const int LIST_CMD_ACCOUNT_CREATE_RET   = 12;   // 创建帐号返回
const int LIST_CMD_ACCOUNT_LOCK         = 13;   // 锁定or解锁帐号
const int LIST_CMD_ACCOUNT_SETPRIV      = 14;   // 设置帐号权限
const int LIST_CMD_ACCOUNT_SETPWD       = 15;   // 设置帐号密码
const int LIST_CMD_ACCOUNT_DELETE       = 16;   // 删除帐号
const int LIST_CMD_LOGOUT_RET           = 17;   // Logout返回
const int LIST_CMD_CHARGE_CARD_LOG      = 18;   // Log
const int LIST_CMD_USER_POINT			= 19;	//用户扣点
const int LIST_CMD_USER_POINT_RET		= 20;	//用户扣点返回
const int LIST_CMD_CHARGE_CARD			= 21;	//激活码
const int LIST_CMD_CHARGE_CARD_RET		= 21;	//激活码返回
const int LIST_CMD_ADD_GIFT_RET			= 22;	//添加礼物返回
const int LIST_CMD_CARD_TRADE			= 23;	//点卡寄卖
const int LIST_CMD_CARD_TRADE_RET		= 24;	//点卡寄卖返回
const int LIST_CMD_CARD_TRADE_FAILD		= 25;	//点卡寄卖失败
const int LIST_CMD_ADD_POINT			= 26;	//用户加点
const int LIST_CMD_ADD_POINT_RET		= 27;	//用户加点返回
const int LIST_CMD_PRE_USER_POINT		= 28;	//用户扣点
const int LIST_CMD_PRE_USER_POINT_RET	= 29;	//用户扣点返回
const int LIST_CMD_ADD_ACCOUNT_TIME		= 30;//tim.yang  VIP卡
const int LIST_CMD_ADD_ACCOUNT_TIME_RET	= 31;//tim.yang  VIP卡
struct TCmdStep
{
    int         iStep;
    time_t      tStepTime;
};

struct SCmdUserLogin
{
    long        dwLoginOrderID;                     // 流水号, 目前只有登陆命令中会用到，用来标志网络回传的命令
	long        lSeed;                              // identifier whick net link send login cmd 
    TCmdStep    t;
	//char        szUsername[MAX_USER_NAME_SIZE+1];
	//char		szPubWinUsername[MAX_USER_NAME_SIZE + 31];//tim.yang  容量增大30
	//char        szPassword[MAX_USER_PWD_SIZE+1];
	//char        szUserIP[16];
	//char		pubwinKey[MAX_USER_NAME_SIZE + 31];
	//char		glomPubwinPWD[MAX_USER_PWD_SIZE+1];
	CM_MEMDEF(szUsername, MAX_USER_NAME_SIZE+1)
	CM_MEMDEF(szPubWinUsername, MAX_USER_NAME_SIZE+31)
	CM_MEMDEF(szPassword, MAX_USER_PWD_SIZE+1)
	CM_MEMDEF(szUserIP, 16)
	CM_MEMDEF(pubwinKey, MAX_USER_NAME_SIZE+31)
	CM_MEMDEF(glomPubwinPWD, MAX_USER_PWD_SIZE+1)
	bool		bPowerLogin;//PZH

	SCmdUserLogin()
	{
		CM_MEMPROTECTOR(szUsername, MAX_USER_NAME_SIZE+1)
		CM_MEMPROTECTOR(szPubWinUsername, MAX_USER_NAME_SIZE+31)
		CM_MEMPROTECTOR(szPassword, MAX_USER_PWD_SIZE+1)
		CM_MEMPROTECTOR(szUserIP, 16)
		CM_MEMPROTECTOR(pubwinKey, MAX_USER_NAME_SIZE+31)
		CM_MEMPROTECTOR(glomPubwinPWD, MAX_USER_PWD_SIZE+1)
	}
	SCmdUserLogin(const SCmdUserLogin &object)
	{
		memcpy(this, &object, sizeof(SCmdUserLogin));
		CM_MEMPROTECTOR(szUsername, MAX_USER_NAME_SIZE+1)
		CM_MEMPROTECTOR(szPubWinUsername, MAX_USER_NAME_SIZE+31)
		CM_MEMPROTECTOR(szPassword, MAX_USER_PWD_SIZE+1)
		CM_MEMPROTECTOR(szUserIP, 16)
		CM_MEMPROTECTOR(pubwinKey, MAX_USER_NAME_SIZE+31)
		CM_MEMPROTECTOR(glomPubwinPWD, MAX_USER_PWD_SIZE+1)
	}
	~SCmdUserLogin()
	{
		CM_MEMUNPROTECTOR(szUsername)
		CM_MEMUNPROTECTOR(szPubWinUsername)
		CM_MEMUNPROTECTOR(szPassword)
		CM_MEMUNPROTECTOR(szUserIP)
		CM_MEMUNPROTECTOR(pubwinKey)
		CM_MEMUNPROTECTOR(glomPubwinPWD)
	}
};

struct SCmdLoginRet
{
    long        dwLoginOrderID;                    // 流水号, 目前只有登陆命令中会用到，用来标志网络回传的命令
	long        lSeed;
    TCmdStep    t;
	long        lRet;
    short       sRetCode;
};

struct SCmdUserLogout
{
    long        tLogoutTime;
    DWORD       dwIP;
    float       fPoint;
    short       sServer;
};

struct SCmdLogoutRet
{
    long        dwLogoutOrderID;
};

struct SCmdRet
{
	bool    bSucceed;
    bool    bShowMessage;
    long    lShowMessageAccount;
    //char    szInfo[MAX_USER_NAME_SIZE+MAX_USER_PWD_SIZE+16];
	CM_MEMDEF(szInfo, MAX_USER_NAME_SIZE+MAX_USER_PWD_SIZE+16)

	SCmdRet()
	{
		CM_MEMPROTECTOR(szInfo, MAX_USER_NAME_SIZE+MAX_USER_PWD_SIZE+16)
	}
	SCmdRet(const SCmdRet &object)
	{
		memcpy(this, &object, sizeof(SCmdRet));
		CM_MEMPROTECTOR(szInfo, MAX_USER_NAME_SIZE+MAX_USER_PWD_SIZE+16)
	}
	~SCmdRet()
	{
		CM_MEMUNPROTECTOR(szInfo)
	}
};

struct SCmdAccountCreate
{
	//char    szUsername[MAX_USER_NAME_SIZE+1];		
	//char    szPassword[MAX_USER_PWD_SIZE+1];
	CM_MEMDEF(szUsername, MAX_USER_NAME_SIZE+1)
	CM_MEMDEF(szPassword, MAX_USER_PWD_SIZE+1)
    long    lPriv;
    short   sLocked;
    long    tLockBegin;
    long    tLockEnd;
    long    lLoginOrderID;

	SCmdAccountCreate()
	{
		CM_MEMPROTECTOR(szUsername, MAX_USER_NAME_SIZE+1)
		CM_MEMPROTECTOR(szPassword, MAX_USER_PWD_SIZE+1)
	}
	SCmdAccountCreate(const SCmdAccountCreate &object)
	{
		memcpy(this, &object, sizeof(SCmdAccountCreate));
		CM_MEMPROTECTOR(szUsername, MAX_USER_NAME_SIZE+1)
		CM_MEMPROTECTOR(szPassword, MAX_USER_PWD_SIZE+1)
	}
	~SCmdAccountCreate()
	{
		CM_MEMUNPROTECTOR(szUsername)
		CM_MEMUNPROTECTOR(szPassword)
	}
};

struct SCmdAccountCreateRet
{
	bool    bSucceed;
    long    lAccountID;
    long    lLoginOrderID;
};

struct SCmdAccountLock
{
    bool    bUseUsername;
	//char    szUsername[MAX_USER_NAME_SIZE+1];
	CM_MEMDEF(szUsername, MAX_USER_NAME_SIZE+1)
    long    lAccountID;
    short   sLocked;
    long    tLockBegin;
    long    tLockEnd;

	SCmdAccountLock()
	{
		CM_MEMPROTECTOR(szUsername, MAX_USER_NAME_SIZE+1)
	}
	SCmdAccountLock(const SCmdAccountLock &object)
	{
		memcpy(this, &object, sizeof(SCmdAccountLock));
		CM_MEMPROTECTOR(szUsername, MAX_USER_NAME_SIZE+1)
	}
	~SCmdAccountLock()
	{
		CM_MEMUNPROTECTOR(szUsername)
	}
};

struct SCmdAccountSetPriv
{
    bool    bUseUsername;
	//char    szUsername[MAX_USER_NAME_SIZE+1];
	CM_MEMDEF(szUsername, MAX_USER_NAME_SIZE+1)
    long    lAccountID;
    long    lPriv;

	SCmdAccountSetPriv()
	{
		CM_MEMPROTECTOR(szUsername, MAX_USER_NAME_SIZE+1)
	}
	SCmdAccountSetPriv(const SCmdAccountSetPriv &object)
	{
		memcpy(this, &object, sizeof(SCmdAccountSetPriv));
		CM_MEMPROTECTOR(szUsername, MAX_USER_NAME_SIZE+1)
	}
	~SCmdAccountSetPriv()
	{
		CM_MEMUNPROTECTOR(szUsername)
	}
};

struct SCmdAccountSetPwd
{
    bool    bUseUsername;
	//char    szUsername[MAX_USER_NAME_SIZE+1];
	//char    szPassword[MAX_USER_PWD_SIZE+1];
	CM_MEMDEF(szUsername, MAX_USER_NAME_SIZE+1)
	CM_MEMDEF(szPassword, MAX_USER_PWD_SIZE+1)
    long    lAccountID;
    long    lLoginOrderID;

	SCmdAccountSetPwd()
	{
		CM_MEMPROTECTOR(szUsername, MAX_USER_NAME_SIZE+1)
		CM_MEMPROTECTOR(szPassword, MAX_USER_PWD_SIZE+1)
	}
	SCmdAccountSetPwd(const SCmdAccountSetPwd &object)
	{
		memcpy(this, &object, sizeof(SCmdAccountSetPwd));
		CM_MEMPROTECTOR(szUsername, MAX_USER_NAME_SIZE+1)
		CM_MEMPROTECTOR(szPassword, MAX_USER_PWD_SIZE+1)
	}
	~SCmdAccountSetPwd()
	{
		CM_MEMUNPROTECTOR(szUsername)
		CM_MEMUNPROTECTOR(szPassword)
	}
};

struct SCmdAccountDelete
{
    bool    bUseUsername;
	//char    szUsername[MAX_USER_NAME_SIZE+1];
	CM_MEMDEF(szUsername, MAX_USER_NAME_SIZE+1)
    long    lAccountID;
    short   sDeleted;

	SCmdAccountDelete()
	{
		CM_MEMPROTECTOR(szUsername, MAX_USER_NAME_SIZE+1)
	}
	SCmdAccountDelete(const SCmdAccountDelete &object)
	{
		memcpy(this, &object, sizeof(SCmdAccountDelete));
		CM_MEMPROTECTOR(szUsername, MAX_USER_NAME_SIZE+1)
	}
	~SCmdAccountDelete()
	{
		CM_MEMUNPROTECTOR(szUsername)
	}
};

struct SCmdCardChargeLog
{
    enum ECCOP
    {
        CCOP_CHARGE = 1,
        CCOP_CHARGE_SUCCEEDED = 2,
        CCOP_CHARGE_FAILED = 3,
        CCOP_CHARGE_TIMEOUT = 4,
        CCOP_CHARGE_CLOSED = 5,
    };
    long    lSn;
    char    cOP;
    char    cCardType;
    short   sServer;
    long    lAccountID;
    long    lUserID;
    //char    szCardID[40];
	CM_MEMDEF(szCardID, 40)

	SCmdCardChargeLog()
	{
		CM_MEMPROTECTOR(szCardID, 40)
	}
	SCmdCardChargeLog(const SCmdCardChargeLog &object)
	{
		memcpy(this, &object, sizeof(SCmdCardChargeLog));
		CM_MEMPROTECTOR(szCardID, 40)
	}
	~SCmdCardChargeLog()
	{
		CM_MEMUNPROTECTOR(szCardID)
	}
};

struct SChargeCard
{
	long    lAccountID;								//sale
	//char    szAccountName[MAX_USER_NAME_SIZE+1];	//sale
	CM_MEMDEF(szAccountName, MAX_USER_NAME_SIZE+1)
	long    lBuyerAccountID;
	//char    szBuyerAccountName[MAX_USER_NAME_SIZE+1];
	CM_MEMDEF(szBuyerAccountName, MAX_USER_NAME_SIZE+1)
	short   sServer;
	long    lUserID;
	long    lDeductOrderID;
	short   sPoint;
	short   sCurPoint;
	//char    szCardID[40];
	CM_MEMDEF(szCardID, 40)
	long    tTime;
	//char	cResult[200];
	CM_MEMDEF(cResult, 200)
	//char	szUserName[MAX_USER_NAME_SIZE+1];
	CM_MEMDEF(szUserName, MAX_USER_NAME_SIZE+1)
	//char	szCardPassWord[40];
	CM_MEMDEF(szCardPassWord, 40)
	short	sUserLev;
	char	cMetier;
	char	cMetierLev;
	char	cCardType;
	char	bSucceed;
	long	param;
	//char	cUserIp[20];
	CM_MEMDEF(cUserIp, 20)
	long	lCardGroupType;
	long	event;
	long	lAuctionId;

	SChargeCard()
	{
		CM_MEMPROTECTOR(szAccountName, MAX_USER_NAME_SIZE+1)
		CM_MEMPROTECTOR(szBuyerAccountName, MAX_USER_NAME_SIZE+1)
		CM_MEMPROTECTOR(szCardID, 40)
		CM_MEMPROTECTOR(cResult, 200)
		CM_MEMPROTECTOR(szUserName, MAX_USER_NAME_SIZE+1)
		CM_MEMPROTECTOR(szCardPassWord, 40)
		CM_MEMPROTECTOR(cUserIp, 20)
	}
	SChargeCard(const SChargeCard &object)
	{
		memcpy(this, &object, sizeof(SChargeCard));
		CM_MEMPROTECTOR(szAccountName, MAX_USER_NAME_SIZE+1)
		CM_MEMPROTECTOR(szBuyerAccountName, MAX_USER_NAME_SIZE+1)
		CM_MEMPROTECTOR(szCardID, 40)
		CM_MEMPROTECTOR(cResult, 200)
		CM_MEMPROTECTOR(szUserName, MAX_USER_NAME_SIZE+1)
		CM_MEMPROTECTOR(szCardPassWord, 40)
		CM_MEMPROTECTOR(cUserIp, 20)
	}
	~SChargeCard()
	{
		CM_MEMUNPROTECTOR(szAccountName)
		CM_MEMUNPROTECTOR(szBuyerAccountName)
		CM_MEMUNPROTECTOR(szCardID)
		CM_MEMUNPROTECTOR(cResult)
		CM_MEMUNPROTECTOR(szUserName)
		CM_MEMUNPROTECTOR(szCardPassWord)
		CM_MEMUNPROTECTOR(cUserIp)
	}
};

struct SListCmd
{
	CGameWorldStub* pStub;
	long            cmdId;
	SAccountInfo    accountInfo;

	//tim.yang   VIP卡
	bool isVIPCardAddTimeRight;
	std::string VIPTime;
	long VIPRegionTime;
	short GWSId;
	long userId;
	//end

	//union // 纯数值的联合体
	//{
		SCmdUserLogin           userLogin;
		SCmdLoginRet	        userloginRet;
        int                     gwConcurrent[8];    // 连线人数

        SCmdRet                 cmdRet;
		SCmdAccountCreate       accountCreate;
        SCmdAccountCreateRet    accountCreateRet;
        SCmdAccountLock         accountLock;
        SCmdAccountSetPriv      accountSetPriv;
        SCmdAccountSetPwd       accountSetPwd;
        SCmdAccountDelete       accountDelete;
        SCmdUserLogout          userLogout;
        SCmdLogoutRet           logoutRet;
        SCmdCardChargeLog       logCardCharge;
		SChargeCard				chargePoint;
	//};

    SListCmd()
    {
        pStub = NULL;
        cmdId = 0;
		isVIPCardAddTimeRight = false;
		VIPTime.clear();
		VIPRegionTime = 0;
		GWSId = -1;
    }
    SListCmd(CGameWorldStub* stub, long id)
    {
        pStub = stub;
        cmdId = id;
    }
    void SendCmd(CG_CmdPacket *cmd);
};


#include "threadsafelist.h"
#include "loginserver.h"
#include "gameworldstub.h"
#include "lgsthread.h"


void    *db_thread(void *arg);
void	*wsdl_thread(void *arg);
typedef  CThreadSafeList<SListCmd> TCmdList;
extern   SConfigLoginNet    g_cfg;
extern   TCmdList	        g_sendCmdList,g_recvCmdList;
extern   CLoginServer      *g_lgs;
extern   CG_CmdPacket      *g_sendCmd;
extern   long               g_dwLoginOrderID;

#endif
