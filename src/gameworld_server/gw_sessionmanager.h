#ifndef _INC_GW_SESSION_MANAGER_H_
#define _INC_GW_SESSION_MANAGER_H_

struct GWSession
{
	bool			isValid;
	int				index;
	int				connectTimeStamp;
	DWORD           uniqueId;
	CG_TCPSession*	session;

	int             userData1;
	int             userData2;
};

class GWSessionManager;
typedef bool (*GWSessionEvent)(GWSessionManager* pseManager, GWSession& se);
typedef bool (*GWSessionDeclineEvent)(GWSessionManager* pseManager, CG_TCPSession* session, char cCause);

class GWSessionManager
{
	friend class GWServer;

public:
	bool			Create(const string& name, GWCommandChannel* channel, const string& listenIP, int port, int capacity, SAllowIP* allowIP, int timeout, bool allowError, bool bLogMsg=false);
	bool			IsValidSession(int session,int uniqueId);
	void			Accept(bool bDeclineAll=false);
	void			Process();
	void			Stop(bool bResultCause=false, char cCause=0);
	bool			SendToPeer(int session);
	bool            SendToPeer(int session,CG_CmdPacket *cmd);
	void			BroadcastCmd();

	int				GetValidSessionCount() { return (int)m_valid.size(); }
	GWSession&		GetSession(int index) { return m_sessions[index]; }
	GWSession&		GetSession() { return GetSession(m_currentSession); }
	CG_CmdPacket&	GetSendCmd() { return m_sendCmd; }
	CG_CmdPacket&	BeginCmd(short cmd) { m_sendCmd.BeginWrite(); m_sendCmd.WriteShort(cmd); return m_sendCmd; }
	bool			SendCmd() { return SendToPeer(m_currentSession); }
	bool			NotifyCmd(short cmd) { BeginCmd(cmd); return SendCmd(); }
	bool			NotifyCmd(int session, short cmd) { BeginCmd(cmd); return SendToPeer(session); }
	void            SetBufferSize(int kb) { m_bufSize = kb; }
	void            SetRowBufferSize(int kb) { m_rowBufSize = kb; }

public:
	GWSessionManager();
	virtual ~GWSessionManager();

private:
	int                         m_bufSize;
	int							m_rowBufSize;
	string						m_name;
	vector<GWSession>			m_sessions;
	vector<int>					m_free;
	list<int>					m_valid;
	int							m_currentSession;
	GWCommandChannel*			m_channel;
	CG_TCPListener				m_listener;
	CG_CmdPacket				m_recvCmd;
	CG_CmdPacket				m_sendCmd;
	bool						m_allowError;
    bool                        m_bLogMsg;
	int							m_timeout;
	SAllowIP*                   m_allowIP;
	DWORD                       m_uniqueId;

public:
	GWSessionEvent		    SessionLost;
	GWSessionEvent		    SessionAccept;
    GWSessionDeclineEvent   SessionDecline;
};

#endif // _INC_GW_SESSION_MANAGER_H_

