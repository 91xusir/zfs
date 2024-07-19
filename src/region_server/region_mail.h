


#ifndef REGION_MAIL_H
#define REGION_MAIL_H
#include "core/rt2_core.h"
#include "rt1tort2.h"
#include "region.h"
#include "region_object.h"
#include "scene/rts_sv_scene.h"
#include "region_creature.h"
#include "region_scene.h"
#include "cmd_channel.h"
#include "region_server.h"
#include "region_friend.h"


#include <string>
class CRegionCreature;

/*

CMD_FUNCTION(cmd_g2r_chat)


g_region.m_gws->
void					SendMail(int receiverID, int senderID, const string& senderName, const string& title, const string& content);

///
void					DeleteMail(int userID, const string& date);

///
void					SendChat(int receiverID, int senderID, char channel, const string& content);

*/

class CServerMail
{
public:
	CServerMail(CRegionCreature* vpMaster)
		:mpFriend(NULL),mpMaster(vpMaster){};
	~CServerMail(){};

	void	SetFriendList(CServerFriend* vpFriend){mpFriend=vpFriend;};//设置
	void	SendToClient(long MailID,DWORD vFromDBID,std::string& vrReceivedDate,std::string& vrFromName,char* vpTitle,char* vpContent, const char*  StrItemID,long Money,long IsOpen);
	bool	SendToGameWorld(DWORD vDBID,char* vpTitle,char* vpContent,	long ItemID,long Money);
	void    MoveItem(std::string& Item,long UserID,long Money);
	bool    MoveItemSendtoGw(long MailID);
	void    IsOpenMail(short IsOpen,long MailID);
	int		ReceiveFromClient(CRegionCreature* CmdGiver,CG_CmdPacket *cmd);
	void	DeleteInGameWorld(long MailID);
	void	ReceiveFromGameWorld(long MailID,DWORD vFromDBID,std::string& vrReceivedDate,std::string& vrFromName,char* vpTitle,char* vpContent,const char* StrItemID,long Money,long IsOpen);

	//Tianh  判断字符串里全是数字  全是return0
	int     check(char *buf,int len);

private:
	CServerMail*	FindByDBID(DWORD vDBID);

	std::list<SItemID>  m_listItem;
	CServerFriend*		mpFriend;
	CRegionCreature*	mpMaster;
};







#endif//REGION_MAIL_H
