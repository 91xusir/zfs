#ifndef _INC_GW_CHATTRANSMITTAL_H_
#define _INC_GW_CHATTRANSMITTAL_H_

class GWChatTransmittal
{
private:
	struct GWChatGroup
	{
		int				session;
		vector<long>	users;
		GWChatGroup*	next;
		bool			pending;
	};

public:
	void ChatPublic(int faction, const char* toName, const char* fromName, char channel, const char* content,long oldId);

	void ChatWorld(int faction, const char* toName, const char* fromName, char channel, const char* content);		//heten

	void ChatTeam(int toID, const char* toName, const char* fromName, char channel, const char* content);

	void ChatPrivate(int toID, const char* toName, const char* fromName, char channel, const char* content);

	void ChatParty(int toID, const char* fromName, const char* content);

	void ChatCountry(int toID, const char* fromName, const char* content);

	void ChatGM(int toID, const char* content);

	void Chat(int session, int toID, const char* fromName, char channel, const char* content);

	void ChatNewS(const char* fromName,const char* Name,const char* BossName,const char* SceneName,long type,char channel,const char *ItemName);
private:
	void InitChatGroup();

	void BroadcastChat(GWChatGroup* groups, const char* fromName, char channel, const char* content);
	void BroadcastChatNewS(GWChatGroup* groups,const char* fromName,const char* Name,const char* BossName,const char* SceneName,long type,char channel,const char *ItemName);

private:
	vector<GWChatGroup> m_group;
};

#endif // _INC_GW_CHATTRANSMITTAL_H_
