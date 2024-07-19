//===========================================================
//		�ļ����ƣ�	command.h
//		��    �ߣ�  ���� 
//		ʱ    �䣺	
//		��    �ݣ�	���ڼ�����ǵ�ͷ�ļ�
//		��    ע�� 
//===========================================================
#ifndef COMMAND_H
#define COMMAND_H

//<SGI_STL��>
#include <string>
#include <memory>
#include <map>
//</SGI_STL��>

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

		bool SetActiveSkill(int vSkillID){mActiveSkillID=vSkillID;return true;};//���õ�ǰ�����ID
		bool SetHotkey(int vKeyID,bool vKeyDown);	//Ϊ��ǰ�����ͼ�����ÿ�ݼ�
		bool SaveHotKey();							//����ǰ��ݼ����ô���
		bool LoadHotKey();							//����ǰ��ݼ�����
		bool Hotkey(int vKeyID,bool vKeyDown);		//��ݼ�����

		bool RenderLoop(int vSkillTime);				//��Ⱦѭ��

		bool MouseMove(int vX,int vY);
		bool MouseLeftDown(int vX,int vY);

		bool Clear();
		bool SetTarget(ULONG	vTargetObjectID);
		bool SetSkill(int	vSkillID);
		//bool SetCommand(string& vrCommand);
		bool Run();//ִ������

	private:
		CGCCreature*	MousePointToObject(int vMouseX,int vMouseY);//�������λ�õõ����ID
		bool			MousePointToScenePoint(int vMouseX,int vMouseY,float vPoint[3]);//�������λ�õõ������ϵ�һ����


		CGCHandler*		mpHandler;
		map<int,int>	mKey2SkillID;

		int				mSkillID;
		ULONG			mTargetObjectID;
		ULONG			mClickObjectID;//���һ������������
		int				mActiveSkillID;//��ǰ����ļ���ID
		int				mMouseX;//��굱ǰXλ��
		int				mMouseY;//��굱ǰYλ��
	};

}




#endif//COMMAND_H