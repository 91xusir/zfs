//===========================================================
//		文件名称： game_logic_manager.h
//		作    者： 吴麟
//		时    间： 2004.6.28
//		内    容： 游戏逻辑部分管理器
//===========================================================
#ifndef GAME_LOGIC_MANAGER_H
#define GAME_LOGIC_MANAGER_H
#include <list>

namespace Game
{
	class CObject;

	enum SafeTypes{SAFE,UN_SAFE,UPLOAD};
	enum ObjectCommands{WALK};//走路，飞行，传送
	

	class CManager
	{
	public:
		CManager();
		~CManager();
		
		void AddObject(CObject* vpObject,SafeTypes vType);
		void DelObject(CObject* vpObject);

		//InEyeshot(CObject* vpObject,SafeTypes vType);
		//OutEyeshot(CObject* vpObject,SafeTypes vType);
		void Command(int vID,ObjectCommands vCommand,float vX,float vY,float vZ);	//外部的命令
		void TimeTrigger();															//外部的触发
	
	private:
		//CTimer				mTimer;			//记时器
		
		std::list<CObject*> mAllList;		//所有的物件列表
		
		std::list<CObject*> mSafeList;		//要服务器同步的物件列表
		std::list<CObject*> mUnSafeList;	//不需要服务器同步的物件列表
		std::list<CObject*> mUploadList;	//要上传给服务器的物件列表

		CObject*			mpContral;		//当前的控制者

	};
}










#endif//GAME_LOGIC_MANAGER_H