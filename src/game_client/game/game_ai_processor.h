//===========================================================
//		文件名称： game_ai_processor.h
//		作    者： 吴麟
//		时    间： 2004.6.30
//		内    容： 游戏中的AI处理器
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

		int	Run(int vTime){return 0;};//得到当前的状态
	private:
	};



	class CAIManager
	{
	};
}












#endif//GAME_AI_PROCESSOR_H