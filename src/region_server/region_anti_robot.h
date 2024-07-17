#ifndef __REGION_ANTI_ROBOT_H__
#define __REGION_ANTI_ROBOT_H__

#define ANTIROBOT_SENDPIC_INTERVAL			3000 //(1*60)		// ������ɫÿ�����뷢��һ��
#define ANTIROBOT_SENDPIC_INTERVAL_RAND		600					// ����ʱ��������Χ
#define ANTIROBOT_RESPONSE_MAX_TIME			50					// �ظ����������ʱ��
#define ANTIROBOT_RESEND_MAX_COUNT			3 
#define ANTIROBOT_ERROR_LOCK_ACCOUNT		3					// ������ٴκ�Ϳ�ʼ�����˺�
#define ANTIROBOT_KICK_ACCOUNT_TIME			0 // (60*60)		// �����˺ŵ�ʱ��

enum EAntiRobotState
{
	ARS_Normal		= 0,
	ARS_Asking_1 	= 1, // ��Ҫѡ����ʴ�
    ARS_Asking_2    = 2, // ��Ҫ������ʴ�
	ARS_Lock		= 3
};

class CAntiRobot
{
public:
	CAntiRobot();
	~CAntiRobot();

	void Run();
	void KickRobotUser(DWORD userID);
	void LockRobotUser(DWORD userID); 		// ֻ��ChangeState()�е���
	void UnlockRobotUser(DWORD userID);		// ֻ��ChangeState()�е���
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
    bool m_bEnableSendPicture;              // �������κ�ͼƬ
	std::list<DWORD> m_UsersIgnored;		// ������ͼƬ�����
	std::list<DWORD> m_UsersSuspicious;		// ������ң��´����ߺ���������ͼƬ

	int m_nSendInterval;
	int m_nSendIntervalRand;
	int m_nLockAccountSeconds;
	int m_nResponseMaxTime;

	int m_nLockAccCounter;
};

extern CAntiRobot g_AntiRobot;

#endif // __REGION_ANTI_ROBOT_H__
