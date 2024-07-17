//===========================================================
//		�ļ����ƣ� game_net_processor.h
//		��    �ߣ� ����
//		ʱ    �䣺 2004.6.30
//		��    �ݣ� ��Ϸ�е�����ͨѶ������
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