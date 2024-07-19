#ifndef __REGION_ANTI_ROBOT_H__
#define __REGION_ANTI_ROBOT_H__

#define ANTIROBOT_SENDPIC_INTERVAL			3000 //(1*60)		// 正常角色每多少秒发送一次
#define ANTIROBOT_SENDPIC_INTERVAL_RAND		600					// 正常时间的随机范围
#define ANTIROBOT_RESPONSE_MAX_TIME			50					// 回复的最大允许时间
#define ANTIROBOT_RESEND_MAX_COUNT			3 
#define ANTIROBOT_ERROR_LOCK_ACCOUNT		3					// 错误多少次后就开始锁定账号
#define ANTIROBOT_KICK_ACCOUNT_TIME			0 // (60*60)		// 锁定账号的时间

enum EAntiRobotState
{
	ARS_Normal		= 0,
	ARS_Asking_1 	= 1, // 需要选择的问答
    ARS_Asking_2    = 2, // 需要输入的问答
	ARS_Lock		= 3
};

class CAntiRobot
{
public:
	CAntiRobot();
	~CAntiRobot();

	void Run();
	void KickRobotUser(DWORD userID);
	void LockRobotUser(DWORD userID); 		// 只在ChangeState()中调用
	void UnlockRobotUser(DWORD userID);		// 只在ChangeState()中调用
	void Enable() { m_bEnable = true; }
	void Disable() { m_bEnable = false; }
    void EnableSendPicture() { m_bEnableSendPicture = true; }
    void DisableSendPicture() { m_bEnableSendPicture = false; }
	void DumpData();
	std::string GetStatusString();
	bool CanIgnoreUser(CRegionUser* pUser);

	void OnAfterAddUser(CRegionUser* pUser);
	void OnBeforeRemoveUser(CRegionUser* pUser);

	void OnRecvUserReply(CRegionUser* pUser, CG_CmdPacket* pPacket);
	void SendRequestToUser(CRegionUser* pUser, bool bResend = false, EAntiRobotState type = ARS_Asking_1);
	void SendResultToUser(CRegionUser* pUser, short nErrorCode);

	void AddUserToIgnoredList(DWORD userID);
	bool FindInIgnoredList(DWORD userID);
	void RemoveFromIgnoredList(DWORD userID);
	void AddUserToSuspiciousList(DWORD userID);
	bool FindInSuspiciousList(DWORD userID);
	void RemoveFromSuspiciousList(DWORD userID);

	bool Set_SendInterval(int value)
	{
		if (value < 0)		return false;
		m_nSendInterval = value;
		m_nSendIntervalRand = value / 5;
		return true;
	}
	bool Set_LockAccTime(int value)
	{
		if (value < 0)	value = 0;
		m_nLockAccountSeconds = value;
		return true;
	}
	int Get_SendInterval() { return m_nSendInterval; }
	int Get_SendIntervalRand() { return m_nSendIntervalRand; }
	int Get_LockAccTime(int value) { return m_nLockAccountSeconds; }

private:
	void InitAntiRobotData(CRegionUser* pUser);
	bool ChangeState(CRegionUser *pUser, EAntiRobotState NewState);
	void SetNextSendTick(CRegionUser *User);

	bool m_bEnable;
    bool m_bEnableSendPicture;              // 不发送任何图片
	std::list<DWORD> m_UsersIgnored;		// 不发送图片的玩家
	std::list<DWORD> m_UsersSuspicious;		// 可以玩家，下次上线后立即发送图片

	int m_nSendInterval;
	int m_nSendIntervalRand;
	int m_nLockAccountSeconds;
	int m_nResponseMaxTime;

	int m_nLockAccCounter;
};

extern CAntiRobot g_AntiRobot;

#endif // __REGION_ANTI_ROBOT_H__
