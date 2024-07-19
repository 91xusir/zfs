//===========================================================
//		文件名称： game_net_processor.h
//		作    者： 吴麟
//		时    间： 2004.6.30
//		内    容： 游戏中的网络通讯处理器
//===========================================================
#ifndef GAME_NET_PROCESSOR_H
#define GAME_NET_PROCESSOR_H

namespace Game
{
	class CNet
	{
	public:
		CNet();
		~CNet();

		void SetObjectManager(CManager* vpManager);
		void Server2Client();
		void Client2Server();
	private:
		CManager*				mpManager;
	};
}


#endif//GAME_NET_PROCESSOR_H