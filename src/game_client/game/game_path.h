//===========================================================
//		文件名称： game_path.h
//		作    者： 吴麟
//		时    间： 2004.6.30
//		内    容： 游戏中的路径处理器
//===========================================================
#ifndef GAME_PATH_H
#define GAME_PATH_H

namespace Game
{
	class CObject;
	enum FindTpyes{WALK_FIND,FLY_FIND,CARRY_FIND};//走路，飞行，传送

	class CPathProcessor
	{
	public:
		CPathProcessor(CObject* vpMaster){};
		virtual ~CPathProcessor(){};

		void Reset(){};

		void Input(float vX,float vY,float vZ,float vSpeed,FindTpyes vMoveType=WALK_FIND){};
		void TimeTrigger(int vTime){};
	private:


	};

}




#endif//GAME_PATH_H
