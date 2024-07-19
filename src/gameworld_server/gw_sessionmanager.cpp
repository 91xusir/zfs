#include "gw_server.h"

GWSessionManager::GWSessionManager()
: m_channel(NULL)
, m_currentSession(0)
, m_allowError(false)
, SessionLost(NULL)
, SessionAccept(NULL)
{
	m_uniqueId = 0;
	m_bufSize = 0;
}

GWSessionManager::~GWSessionManager()
{
	DEL_ONE(m_channel);
	//if(m_channel)
	//	delete m_channel;
}

void GWSessionManager::Stop(bool bResultCause, char cCause)
{
	stlforeach(vector<GWSession>, se, m_sessions)
	{
		if(se->session)
		{
            if(bResultCause)
            {
                CG_CmdPacket& cmd = BeginCmd(g2c_session_close);
                cmd.WriteByte(cCause);
                se->session->SendPacket(&cmd);
            }
			//delete se->session;
			DEL_ONE(se->session);
			//se->session = NULL;
			se->connectTimeStamp = 0;
			se->isValid = false;
		}
	}
	m_valid.clear();
}

bool GWSessionManager::Create(const string& name, 
							  GWCommandChannel* channel, 
							  const string& listenIP,
							  int port, 
							  int capacity,
							  SAllowIP* allowIP,
							  int timeout,
							  bool allowError,
                              bool bLogMsg)
{
	CHECK(m_channel == NULL);
	CHECK(channel != NULL);
	CHECK(capacity > 0);

	m_sendCmd.SetSize(GW_CMD_PACKET_SIZE);
	m_recvCmd.SetSize(GW_CMD_PACKET_SIZE);

	m_name = name;
	m_channel = channel;
	m_allowError = allowError;
	m_timeout = timeout;
	m_allowIP = allowIP;
    m_bLogMsg = bLogMsg;

    m_sessions.resize(capacity);
	m_free.reserve(capacity);

	for (int index = 0; index < capacity; index++)
	{
		m_sessions[index].isValid = false;
		m_sessions[index].index = index;
		m_sessions[index].connectTimeStamp = 0;
		m_sessions[index].session = NULL;
		m_free.push_back(index);
	}

	if (!m_listener.StartListen(listenIP.c_str(), port, false))
	{
		LOG2("%s: session manager can't listen on port %d\n",
			name.c_str(), port);

		return false;
	}
	else
	{
		LOG3("%s: session manager started on port %d (capacity = %d)\n", 
			name.c_str(), port, capacity);

		return true;
	}	
}

void GWSessionManager::Accept(bool bDeclineAll)
{
	CG_TCPSession* session;	

    if (bDeclineAll)
    {
        while (session = m_listener.AcceptNewSession())
        {
            if (SessionDecline)
                SessionDecline(this, session, 0);
			DEL_ONE(session);
            //delete session;
        }
        return;
    }

	for (int n = 0; n < 10; n++)
	{
		if (m_free.empty())
        {
            while (session = m_listener.AcceptNewSession())
            {
                if (m_bLogMsg)
                {
                    LOG1("%s to capacity, Declined all connection\n", m_name.c_str());
                }
                if (SessionDecline)
                    SessionDecline(this, session, 2);
                //delete session;
				DEL_ONE(session);
            }
            return;
        }

		session = m_listener.AcceptNewSession();
		if (session == NULL)
			return;
        session->SetDebugStr(m_name.c_str());

		if (m_allowIP != NULL)
		{
			unsigned long peerIP;

			session->GetSocket().GetRemoteAddr(NULL, NULL, &peerIP);

			// 0x01000007f is 127.0.0.1
			if (peerIP != 0x0100007f)
			{
                peerIP = InvertIPOrder(peerIP);
                if (!m_allowIP->CheckAllow(peerIP))
                {
                    MSG4("Access denied for session [%d.%d.%d.%d]\n",
                        (peerIP&0xFF000000)>>24,
                        (peerIP&0x00FF0000)>>16,
                        (peerIP&0x00FF00)>>8,
                        (peerIP&0x00FF));
                    if (SessionDecline)
                        SessionDecline(this, session, 1);
                    //delete session;
					DEL_ONE(session);
                    return;
                }
			}
		}

		int index = m_free.back();
		GWSession& se = m_sessions[index];		
		se.isValid = true;			
		se.session = session;
		se.connectTimeStamp = rtGetMilliseconds();
		se.uniqueId = m_uniqueId++;
		se.userData1 = 0;
		se.userData2 = 0;
		if(m_bufSize)
			session->SetBufSize(m_bufSize);

		m_valid.push_back(index);
		m_free.pop_back();

		char peerAddr[128];
		session->GetSocket().GetRemoteAddr(peerAddr, NULL);

		// accept callback
		// fix later...
        if (SessionAccept)
            SessionAccept(this, se);

        if (m_bLogMsg)
        {
            LOG4("Accept a new %s connection from %s (sid = %d, tid = %d) \n",
                m_name.c_str(),
                peerAddr, 
                se.index, 
                se.connectTimeStamp);
        }
	}
}

void GWSessionManager::Process()
{
	short cmdId;
	int timeoutGate = rtGetMilliseconds() - m_timeout;
	for(list<int>::iterator index = m_valid.begin(); index != m_valid.end();)
	{
		GWSession& se = m_sessions[*index];
		CHECK(se.session != NULL);
		if (m_timeout)
		{
			if (se.connectTimeStamp < timeoutGate)
			{
                if (m_bLogMsg)
                {
                    LOG3("Session %s timeout (sid = %d, tid = %d)\n", m_name.c_str(), se.index, se.connectTimeStamp);
                }
				se.isValid = false;
			}
		}

		if (!se.isValid)
		{
            if (m_bLogMsg)
            {
                LOG3("Remove dead %s connection (sid = %d, tid = %d) \n", m_name.c_str(), se.index, se.connectTimeStamp);
            }
			if (SessionLost)
				SessionLost(this, se);

			// if need,logout user
			//delete se.session;
			DEL_ONE(se.session);
			se.isValid = false;
			se.connectTimeStamp = 0;
			se.session = NULL;
			m_free.push_back(*index);

			list<int>::iterator t = index;
			index++;
			m_valid.erase(t);
			continue;
		}

		se.session->DefProc();
		if (se.session->IsDead())
		{
			se.isValid = false;
			continue;				
		}

		m_currentSession = se.index;
		while(se.session->GetPacket(&m_recvCmd))
		{
			if (!m_recvCmd.ReadShort(&cmdId) && !m_allowError)
			{
				se.isValid = false;				
				break;
			}

			int res = m_channel->Invoke(cmdId, &m_recvCmd);
			if (res != eOkay)
			{
				char peer[32];
				short port;

				se.session->GetSocket().GetRemoteAddr(peer, &port);
				if (res == eInvalidCmd)
                {
					LOG3("Invalid %s command %d from %s\n", m_name.c_str(), cmdId, peer);
                }else
				{
					LOG4("Execute %s command %d failed from %s, result = %d\n", m_name.c_str(),cmdId,peer,res);
				}

				if (!m_allowError)
				{
					se.isValid = false;
					break;
				}
			}
		}

		index++;
	}
}

bool GWSessionManager::SendToPeer(int session)
{
	GWSession& se = m_sessions[session];
	if (!se.isValid)
		return false;
	CHECK(se.session != NULL);
	return se.session->SendPacket(&m_sendCmd);
}

bool GWSessionManager::SendToPeer(int session,CG_CmdPacket *cmd)
{
	GWSession& se = m_sessions[session];
	if (!se.isValid)
		return false;
	CHECK(se.session != NULL);
	return se.session->SendPacket(cmd);
}

bool GWSessionManager::IsValidSession(int session,int uniqueId) 
{ 
	if (session < 0 || session >= (int)m_sessions.size())
		return false;

	GWSession& se = m_sessions[session];
	if (!se.isValid)
		return false;

	if (se.uniqueId != uniqueId)
		return false;

	return true;
}

void GWSessionManager::BroadcastCmd()
{
	stlforeach(list<int>, v, m_valid)
	{
		GWSession& se = m_sessions[*v];
		if(se.isValid)
		{
			se.session->SendPacket(&m_sendCmd);
		}
	}
}
