#ifndef GW_MASTER_LIST_H 
#define GW_MASTER_LIST_H


#include <string>
#include <list>
#include "master_list_const.h"

class	CG_CmdPacket;
struct	GWUser;

using std::string;
using std::list;


class CMasterListManager
{
public:

	//������gw�ύ
	void ReceiveFromRegion(CG_CmdPacket* vpPacket);		//��Region�õ�һ������
	void Remove(GWUser *vpUser);		//����һ��

private:
	void Add(DWORD vDBID,const string vMetier,BYTE vLevel,short int vPrenticeSize);			//����һ��

	CG_CmdPacket& RegionPack(DWORD vSendToDBID=0);//0��ʾ������ĳ���ض������

	void SendToRegion(GWUser* vpSendToUser);					//��ʦ���б���Region�е�ĳ�����
	
	void Broadcast();//�㲥������region
private:
	CMasterList mShangList;
	CMasterList mZhouList;
	//Tianh  09.12.11 //��������ʦͽ
	CMasterList mNeutral;
};












#endif//GW_MASTER_LIST_H

