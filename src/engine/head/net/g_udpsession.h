//
// g_udpsession.h: interface for the CG_UdpSession class.
//
#ifndef __CG_UDP_SESSION_H__
#define __CG_UDP_SESSION_H__

class CG_NetAddress
{
public:
	CG_NetAddress()
	{ 
		memset(&m_addr,0,sizeof(m_addr));
//		SetRemote(NULL);
		m_remote = "";
		SetPort(0);
	};
	~CG_NetAddress(){};

	SOCKADDR_IN	*GetAddr() {return &m_addr;}
	bool BuildAddr()
	{
		hostent *host=NULL;
		memset(&m_addr,0,sizeof(m_addr));
		m_addr.sin_family=AF_INET;
		m_addr.sin_port=htons(m_port);
		if(!m_remote.empty())
			m_addr.sin_addr.s_addr = inet_addr(m_remote.c_str());
		if(m_addr.sin_addr.s_addr==INADDR_NONE)
		{
			if(m_remote.empty()) return false;
			host=gethostbyname(m_remote.c_str());
			if(!host) return false;
			memcpy(&m_addr.sin_addr,host->h_addr_list[0],host->h_length);
		}
		m_ip = inet_ntoa(m_addr.sin_addr);
		m_addrLen = sizeof(m_addr);
        return true;
	}
	void SetAddr(SOCKADDR_IN *addr,int len)
	{
		memcpy(&m_addr,addr,len);
		m_addrLen = len;
		m_ip = inet_ntoa(m_addr.sin_addr);
		m_port = ntohs(m_addr.sin_port);
	}
	void SetRemote(const char *remote) {m_remote = remote;}
	void SetPort(unsigned short port) {m_port = port;}
	const char *GetRemote() {return m_remote.c_str();}
	const char *GetIP() {return m_ip.c_str();}
	unsigned short GetPort() {return m_port;}
	
private:
	SOCKADDR_IN		m_addr;
	int				m_addrLen;
	unsigned short	m_port;
	std::string		m_remote;
	std::string		m_ip;
};

class CG_UDPSession  
{
public:
	CG_UDPSession();
	virtual ~CG_UDPSession();

	void Close();
	bool RecvFrom(CG_CmdPacket *cmd,CG_NetAddress *fromAddr);
	bool SendTo(CG_CmdPacket *cmd,CG_NetAddress *ToAddr);
	bool Create(const char *addr,short port);

private:
	static CG_CmdPacket  m_sendCmdPkt;	
	static CG_CmdPacket  m_recvCmdPkt;
	static CG_NetPacket  m_sendNetPkt;	
	static CG_NetPacket  m_recvNetPkt;
	//static char m_buffer[MAX_NET_PACKET_SIZE];
	S_MEMDEF(m_buffer, MAX_NET_PACKET_SIZE)

	CG_NetSocket m_socket;
};


struct RtMemoryBlock
{
	RtMemoryBlock() { data = NULL; size = 0; }
	~RtMemoryBlock() { char *pData = (char*)data; if(data) DEL_ARRAY(pData); }
	void SetData(void *buf,int len)
	{
		char *pData = (char*)data;
		if(data) DEL_ARRAY(pData);
		data = NULL;
		size = len;
		if(size != 0)
		{
			data = RT_NEW char[size];
			memcpy(data,buf,size);
		}
	}
	void *data;
	int   size;

	const RtMemoryBlock&  			operator=(const RtMemoryBlock &block)
	{
		SetData(block.data, block.size);
		return block;
	}
	bool operator==(const RtMemoryBlock &block)
	{
		return false;
	}

	friend CG_CmdPacket& operator<<( CG_CmdPacket& cmd, RtMemoryBlock &block)
	{
		if(cmd.IsReading()) {
			char *tmp;
			int size;
			cmd.ReadBinary(&tmp,&size);
			block.SetData(tmp,size);
		}
		else {
			cmd.WriteBinary((char*)block.data,block.size);
		}	
		return cmd;
	}
};

class RtKeyValue;
class RtDict;

class RtKeyValue
{
public:
	RtKeyValue();
	virtual ~RtKeyValue();

	enum	EKeyType {KT_STRING,KT_DICT,KT_NUMBER,KT_BLOCK};
	const	char *GetString() { return m_string.c_str(); }
	RtDict  *GetDict() { return m_dict; }

	operator float()		{ return (float)m_number; }
	operator double()		{ return (double)m_number; }
	operator int()			{ return (int)m_number; }
	operator unsigned int()	{ return (unsigned int)m_number; }

	operator char()		{ return (char)m_number; }
	operator short()	{ return (short)m_number; }
	operator long()		{ return (long)m_number; }
	operator unsigned char()	{ return (unsigned char)m_number; }
	operator unsigned short()	{ return (unsigned short)m_number; }
	operator unsigned long()	{ return (unsigned long)m_number; }

	operator RtDict&()	{ return *m_dict; }
    operator std::string&()  { return m_string; }
	operator const char*() { return m_string.c_str(); }

    const std::string&  	operator=(const std::string &str)	{ m_kt = KT_STRING; m_string = str; return str; };
	const char *			operator=(const char *str)			{ m_kt = KT_STRING; m_string = str; return str; };
	const float             operator=(const float &v)			{ m_kt = KT_NUMBER; m_number = v; return v; }
	const double            operator=(const double &v)			{ m_kt = KT_NUMBER; m_number = v; return v; }
	const int				operator=(const int &v)				{ m_kt = KT_NUMBER; m_number = v; return v; }
	const unsigned int		operator=(const unsigned int &v)    { m_kt = KT_NUMBER; m_number = v; return v; }
	const char              operator=(const char &v)			{ m_kt = KT_NUMBER; m_number = v; return v; }
	const short             operator=(const short &v)			{ m_kt = KT_NUMBER; m_number = v; return v; }
	const long              operator=(const long &v)			{ m_kt = KT_NUMBER; m_number = v; return v; }
	const unsigned char     operator=(const unsigned char &v)   { m_kt = KT_NUMBER; m_number = v; return v; }
	const unsigned short    operator=(const unsigned short &v)  { m_kt = KT_NUMBER; m_number = v; return v; }
	const unsigned long     operator=(const unsigned long &v)   { m_kt = KT_NUMBER; m_number = v; return v; }

	const RtMemoryBlock&    operator=(const RtMemoryBlock &block) { m_kt = KT_BLOCK; m_block = block; return block; };
	const RtDict&     		operator=(const RtDict &dict)	{ 
		m_kt = KT_DICT; SetDict((RtDict*)&dict); return dict; 
	};
	const RtKeyValue		operator=(const RtKeyValue &kv) { 
		m_kt = kv.m_kt; 
		switch(m_kt) {
			case KT_STRING: m_string = kv.m_string; break;
			case KT_DICT:   SetDict(kv.m_dict);  break;
			case KT_BLOCK:  m_block  = kv.m_block;  break;
		}
		return kv;
	}
	bool operator==(const RtKeyValue &kv);

private:
	void SetDict(RtDict *dict);
	
	EKeyType	   m_kt;
	double         m_number;
    std::string    m_string;
	RtDict        *m_dict;
	RtMemoryBlock  m_block;
};

class RtDict
{
public:
	RtDict() {}
	virtual ~RtDict() {}

	bool GetValue(const char *key, RtKeyValue &value);
	void SetValue(const char *key, RtKeyValue &value);
	// RtKeyValue& RtDict::operator[](const char *key);

	const RtDict&			operator= (const RtDict& dict)
	{
		m_map = dict.m_map;
		return dict;
	}
	bool					operator==(const RtDict& dict)
	{
		if(m_map.size() != dict.m_map.size())
			return false;

		return false;
	}
	void SerializeDelta(RtDict &old);

private:
	HASH_STR(std::string, RtKeyValue) m_map;
};

#endif
