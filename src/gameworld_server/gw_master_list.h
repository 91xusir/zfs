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

	//继续向gw提交
	void ReceiveFromRegion(CG_CmdPacket* vpPacket);		//从Region得到一个命令
	void Remove(GWUser *vpUser);		//减少一个

private:
	void Add(DWORD vDBID,const string vMetier,BYTE vLevel,short int vPrenticeSize);			//增加一个

	CG_CmdPacket& RegionPack(DWORD vSendToDBID=0);//0表示不发给某个特定的玩家

	void SendToRegion(GWUser* vpSendToUser);					//将师傅列表发给Region中的某个玩家
	
	void Broadcast();//广播到所有region
private:
	CMasterList mShangList;
	CMasterList mZhouList;
	//Tianh  09.12.11 //关于中立师徒
	CMasterList mNeutral;
};












#endif//GW_MASTER_LIST_H

