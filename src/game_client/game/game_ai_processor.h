//===========================================================
//		�ļ����ƣ� game_ai_processor.h
//		��    �ߣ� ����
//		ʱ    �䣺 2004.6.30
//		��    �ݣ� ��Ϸ�е�AI������
//===========================================================
#ifndef GAME_AI_PROCESSOR_H
#define GAME_AI_PROCESSOR_H
#include "game_object.h"
namespace Game
{

	class CAIState
	{
	};

	class CAIProcessor
	{
	public:
		CAIProcessor();
		virtual ~CAIProcessor();

		int	Run(int vTime){return 0;};//�õ���ǰ��״̬
	private:
	};



	class CAIManager
	{
	};
}












#endif//GAME_AI_PROCESSOR_H