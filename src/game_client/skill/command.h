//===========================================================
//		文件名称：	command.h
//		作    者：  吴麟 
//		时    间：	
//		内    容：	关于技能外壳的头文件
//		备    注： 
//===========================================================
#ifndef COMMAND_H
#define COMMAND_H

//<SGI_STL库>
#include <string>
#include <memory>
#include <map>
//</SGI_STL库>

#include "character_interface.h"

namespace fox
{
	using std::string;	
	using std::map;

	typedef unsigned long ULONG;
	class CCommand
	{
	public:
		CCommand();
		~CCommand();
		bool Init();

		bool SetActiveSkill(int vSkillID){mActiveSkillID=vSkillID;return true;};//设置当前激活技能ID
		bool SetHotkey(int vKeyID,bool vKeyDown);	//为当前激活的图标设置快捷键
		bool SaveHotKey();							//将当前快捷键设置存盘
		bool LoadHotKey();							//将当前快捷键载入
		bool Hotkey(int vKeyID,bool vKeyDown);		//快捷键按下

		bool RenderLoop(int vSkillTime);				//渲染循环

		bool MouseMove(int vX,int vY);
		bool MouseLeftDown(int vX,int vY);

		bool Clear();
		bool SetTarget(ULONG	vTargetObjectID);
		bool SetSkill(int	vSkillID);
		//bool SetCommand(string& vrCommand);
		bool Run();//执行命令

	private:
		CGCCreature*	MousePointToObject(int vMouseX,int vMouseY);//根据鼠标位置得到物件ID
		bool			MousePointToScenePoint(int vMouseX,int vMouseY,float vPoint[3]);//根据鼠标位置得到场景上的一个点


		CGCHandler*		mpHandler;
		map<int,int>	mKey2SkillID;

		int				mSkillID;
		ULONG			mTargetObjectID;
		ULONG			mClickObjectID;//最后一次鼠标点击到物件
		int				mActiveSkillID;//当前激活的技能ID
		int				mMouseX;//鼠标当前X位置
		int				mMouseY;//鼠标当前Y位置
	};

}




#endif//COMMAND_H