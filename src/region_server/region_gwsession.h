#ifndef _INC_REGION_GW_SESSION_H_
#define _INC_REGION_GW_SESSION_H_

#include "core/rt2_core.h"
#include "rt1tort2.h"
#include "gw_protocol.h"
#include "game_mail_shared_const.h"
#include "game_team_share_const.h"

/*
struct CRegionPendingUser
{
	int					userID;
	int					unionID;
	string				unionName;
	string				unionNickName;
	string				unionIcon;
	string				scene;

	string				account;
	string				name;
	string				strSkills;
    string              strEffects;
	string				items;
	int					money;
	string				friends;
	string				teams;
	string				shortcut;
	string				configData;
	string				task;
	short				headModelID;
    char                cSwitchRegion;
	string				bank;
	int					bankMoney;
	string				bankPass;
    long                dungeonUID;     // 副本唯一ID
    long                dungeonTID;     // 副本类型ID
    long                dungeonLastQuit;
	string              fbTask;

	GWUserAttribute		attributes;

	time_t				timeStamp;
	long				seed;
    long                privileges;     // 权限 (enum EUserPrivileges [gw_protocol.h])
	// long				onlineTime;     // 角色累积在线时间

	long				dwState;
};
*/

typedef EXT_SPACE::unordered_map<int, SUserData>	CRegionUnloginUserTable;
typedef list<SUserData>						CRegionPendingUserList;

class CRegionGWSession : public CG_TCPSession
{
	friend class CRegionServer;

public:
	struct SRegionInfo
	{		
		string			host;
		short			port;
		char			valid;
	};
	
private:
	///
	bool					Connect();

	///
	void					Process();

	///
	void					InitChannel();

	///
	bool					SendRepatriateEvent(long userID);

private:
	virtual void			OnConnect(bool ret);

	virtual void			OnDisconnect();

	virtual void			OnReceive(CG_CmdPacket* packet);

public:
	///
	CG_CmdPacket&			BeginSend() { m_sendCmd.BeginWrite(); return m_sendCmd; }

	///
    bool					EndSend()
    {
        bool bResult = SendPacket(&m_sendCmd);
        if (!bResult) ERR("CRegionGWSession::EndSend error.\n");
        return bResult;
    } 

	bool					SaveUserData(long userID, CRegionCreature* cre);
	bool					SaveUserData(SUserData &data);
	SUserData*				LookupUser(long userID);

	bool					AcceptUser(long userID);
	void					RemoveUser(long userID) { m_unloginUsers.erase(userID); }
	bool					RepatriateUser(long userID);

	void					KickUser(long userID) { LOG1("GWS: Kick user %d\n", userID); m_unloginUsers.erase(userID); }
	bool					SendMail(int toID, int fromID, const string& fromName, const string& title, const string& content,const string& StrItemID,long Money);
	bool					DeleteMail(long MailID);
	bool                    SendMailItem(long MailID);
	bool                    SendIsOpenMail(short IsOpen,long MailID);

	void                    SendBlackListName(const char* Name,long UserID);

	void                    SendPakeBlackListName(long SenderID,long userId,short type);

	void                    SendUserWhenFriendName(long vOtherDBID,long userId);
	void                    SendUserIdbyName(const char* name,long senderID);

	void                    SendUserIdbyNameToGw(long UserID ,long SenderID,byte Type);
	 
	void					SendFriend(const DWORD receiverID,SR2rFriendData& data);

	void					SendOneFriendReturn(long SenderID,long UserID,const char* SceneName,long Lev,byte Type,char Metier);

	void					SendChat(const char* receiver, const char* senderName, char channel, const char* content, const char* title="",long oldId = 0 );


	void					SendChat(int receiverID, const char* senderName, char channel, const char* container, const char* title="",long oldId = 0 );

	void					SendSystemNews(const char *Playername,const char *Name,const char *BossName,const char *SceneName,long type,char channel,const char *ItemName);
	

	bool					Logout(long userID, bool pending = true, CRegionUser* user=NULL);

	void                    BeginSwitchRegion(CRegionUser* user);
	void					SwitchRegion(CRegionUser* user);
	bool					CommitPendingData();

    void					UpdateRegionState();

    void                    BroadcastCmdToAllRegion(const char* szCmd);
	void                    BroadcastBulletin(const char* szMessage, char bSysMsg=false, DWORD dwColor=0xFFFFFFFF, DWORD dwGMID=0);
	void                    LockAccount(long m_userId ,short sLockFlag, long lLockSecond);
	void					MoveUserToPosition(long userId, char const* userName, int x, int y);
	void					MoveUserAround(long userId, char const* userName, int range);
	void					MoveUser(char cmd, long userId, char const* userName);

	//--------add start by tony 05.10.08-------//
	bool					LicenseCheck(int nType);

	bool					ReadMd5();
	//--------add end   by tony 05.10.08-------//

	void					SendRefusesetting(long ID,char bRefuseTeam,char bRefuseAction,char bRefuseMail,char bRefuseDuel,char bRefuseFriend,char bRefuseMonster);
public:
	void StartFun()         { CG_CmdPacket &cmd = BeginSend(); cmd.WriteShort(r2g_start_fun); EndSend(); }
	void EndFun()           { CG_CmdPacket &cmd = BeginSend(); cmd.WriteShort(r2g_end_fun); EndSend(); }
    void StartCardTrade()   { CG_CmdPacket &cmd = BeginSend(); cmd.WriteShort(r2g_card_trade_switch); cmd.WriteByte(1); EndSend(); }
	void StopCardTrade()    { CG_CmdPacket &cmd = BeginSend(); cmd.WriteShort(r2g_card_trade_switch); cmd.WriteByte(0); EndSend(); }

private:
	void					PushPendingLogoutUser(int userID, CRegionUser* user);
	void					PushPendingSaveUser(int userID, CRegionCreature* cre);

public:
	CRegionGWSession();
	
public:
	///
	CG_CmdPacket			m_sendCmd;

	///
	CG_CmdPacket			m_recvCmd;

	///
	CCmdChannel				m_channel;

	///
	CRegionUnloginUserTable	m_unloginUsers;	

	CRegionPendingUserList	m_pendingSaveUsers;

	list<int>				m_pendingLogoutUsers;

	///	
	SRegionInfo				m_regions[GW_MAX_REGION_COUNT];

	int						m_disconnectTimeStamp;

	int						m_seed;

    bool                    m_bSafeQuited; // 是否安全的退出

	DWORD                   m_lastProcessTime;

	//--------add start by tony 05.10.08-------//
	std::string				m_strMd5;
	//--------add end   by tony 05.10.08-------//
};

//--------------add start by tony 06.06.02--------------------------//
//- 防私服
class CRegionLsSession : public CG_TCPSession
{
	friend class CRegionServer;

private:
	//////////////////////////////////////////////////////////////////////////
	bool					Connect(const char* remote_ip, const int remote_port);

	///
	void					Close();

	///
	void					Process();

	///
	void					InitChannel();

private:
	virtual void			OnConnect(bool ret);

	virtual void			OnDisconnect();

	virtual void			OnReceive(CG_CmdPacket* packet);

public:
	///
	CG_CmdPacket&			BeginSend() { m_sendCmd.BeginWrite(); return m_sendCmd; }

	///
	bool					EndSend()
	{
		bool bResult = SendPacket(&m_sendCmd);
		if (!bResult) ERR("CRegionLsSession::EndSend error.\n");
		return bResult;
	} 

	void				SaveAuthData(char* p_data);

public:
	CRegionLsSession()
	{
		m_disconnectTimeStamp = 0;
		m_bRecvData = false;
		m_bConnected = false;
	}

public:
	///
	CG_CmdPacket			m_sendCmd;

	///
	CG_CmdPacket			m_recvCmd;

	int						m_disconnectTimeStamp;

	std::string				m_strCheckCode;
	std::string				m_strHostIP;
	int						m_iHostPort;

	bool					m_bRecvData;

	bool					m_bConnected;
};
//--------------add end   by tony 06.06.02--------------------------//

#endif // _INC_REGION_GW_SESSION_H_
