
#ifndef GC_MAIL_H
#define GC_MAIL_H

#include "game_mail_shared_const.h"
#include "gc_friend_with_ui.h"
#include "ui/rtw_ui.h"
#include <string>
#include <list>

class CG_CmdPacket;
typedef unsigned long DWORD;


class CClientMail//客户端邮件
{
public:
	CClientMail():mReceiveBox(),mpFriend(NULL),mSelfName(){};
	virtual ~CClientMail();


	void SetFriendList(CFriendWithUI* vpFriend){mpFriend=vpFriend;};//设置
	
	//------对服务器--------
	void Send(DWORD vDBID,char* vpTitle,char* vpContent,DWORD SendItemID,long money);			//发邮件
	void Receive(CG_CmdPacket *pPacket);							//收邮件
	void Delete(SMailData* vpMail);									//删除本地邮件
	void DeleteServerMail(string& vrDate);							//将服务端邮件删除

	//-----客户端特有的-----
	virtual void OnReceive(SMailData* vpMail)=0;					//接收到邮件后的处理
	virtual void OnDelete(SMailData* vpMail)=0;						//邮件删除后的处理

	//本地邮件地载入和存储

	void Save();
	void Export();

	virtual void Message(const std::string& vrMessage)=0;


protected:
	void Load();


	std::list<SMailData*>		mReceiveBox;						//收件箱
	CFriendWithUI*				mpFriend;							//好友名单
	std::string					mSelfName;
};


#endif//GC_MAIL_H
