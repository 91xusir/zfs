//===========================================================
//		�ļ����ƣ� game_logic_manager.h
//		��    �ߣ� ����
//		ʱ    �䣺 2004.6.28
//		��    �ݣ� ��Ϸ�߼����ֹ�����
//===========================================================
#ifndef GAME_LOGIC_MANAGER_H
#define GAME_LOGIC_MANAGER_H
#include <list>

namespace Game
{
	class CObject;

	enum SafeTypes{SAFE,UN_SAFE,UPLOAD};
	enum ObjectCommands{WALK};//��·�����У�����
	

	class CManager
	{
	public:
		CManager();
		~CManager();
		
		void AddObject(CObject* vpObject,SafeTypes vType);
		void DelObject(CObject* vpObject);

		//InEyeshot(CObject* vpObject,SafeTypes vType);
		//OutEyeshot(CObject* vpObject,SafeTypes vType);
		void Command(int vID,ObjectCommands vCommand,float vX,float vY,float vZ);	//�ⲿ������
		void TimeTrigger();															//�ⲿ�Ĵ���
	
	private:
		//CTimer				mTimer;			//��ʱ��
		
		std::list<CObject*> mAllList;		//���е�����б�
		
		std::list<CObject*> mSafeList;		//Ҫ������ͬ��������б�
		std::list<CObject*> mUnSafeList;	//����Ҫ������ͬ��������б�
		std::list<CObject*> mUploadList;	//Ҫ�ϴ���������������б�

		CObject*			mpContral;		//��ǰ�Ŀ�����

	};
}










#endif//GAME_LOGIC_MANAGER_H