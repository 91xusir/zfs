
#ifndef GC_MAIL_H
#define GC_MAIL_H

#include "game_mail_shared_const.h"
#include "gc_friend_with_ui.h"
#include "ui/rtw_ui.h"
#include <string>
#include <list>

class CG_CmdPacket;
typedef unsigned long DWORD;


class CClientMail//�ͻ����ʼ�
{
public:
	CClientMail():mReceiveBox(),mpFriend(NULL),mSelfName(){};
	virtual ~CClientMail();


	void SetFriendList(CFriendWithUI* vpFriend){mpFriend=vpFriend;};//����
	
	//------�Է�����--------
	void Send(DWORD vDBID,char* vpTitle,char* vpContent,DWORD SendItemID,long money);			//���ʼ�
	void Receive(CG_CmdPacket *pPacket);							//���ʼ�
	void Delete(SMailData* vpMail);									//ɾ�������ʼ�
	void DeleteServerMail(string& vrDate);							//��������ʼ�ɾ��

	//-----�ͻ������е�-----
	virtual void OnReceive(SMailData* vpMail)=0;					//���յ��ʼ���Ĵ���
	virtual void OnDelete(SMailData* vpMail)=0;						//�ʼ�ɾ����Ĵ���

	//�����ʼ�������ʹ洢

	void Save();
	void Export();

	virtual void Message(const std::string& vrMessage)=0;


protected:
	void Load();


	std::list<SMailData*>		mReceiveBox;						//�ռ���
	CFriendWithUI*				mpFriend;							//��������
	std::string					mSelfName;
};


#endif//GC_MAIL_H
