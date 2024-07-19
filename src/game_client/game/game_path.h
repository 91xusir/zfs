//===========================================================
//		�ļ����ƣ� game_path.h
//		��    �ߣ� ����
//		ʱ    �䣺 2004.6.30
//		��    �ݣ� ��Ϸ�е�·��������
//===========================================================
#ifndef GAME_PATH_H
#define GAME_PATH_H

namespace Game
{
	class CObject;
	enum FindTpyes{WALK_FIND,FLY_FIND,CARRY_FIND};//��·�����У�����

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
