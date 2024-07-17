//===========================================================
//		�ļ����ƣ�	skill_system.h
//		��    �ߣ�  ���� 
//		ʱ    �䣺	
//		��    �ݣ�	
//		��    ע�� 
//===========================================================
#ifndef SKILL_SYSTEM_H
#define SKILL_SYSTEM_H

#include "skill_manager.h"
#include "effect_manager.h"
#include "game_assistant.h"

//using fox::TimerManagerClass;

typedef unsigned char       BYTE;

class CSkillSystem
{
	friend class Atom;
public:
	CSkillSystem();						//����
	virtual ~CSkillSystem();			//����

	virtual bool Init();				//��ʼ��
	virtual void OnFrame(unsigned long time,unsigned long delta){};
	virtual void Exit(){};
	virtual const char *GetName(){return "CSkillSystem";};

public:
	CSkillManager   mSkillManager;					//���ܹ���
	CEffectManager  mEffectManager;					//Ч������

private:
	virtual bool ConcreteInit()=0;					//����ĳ�ʼ��

	string			mChoiceAttrib;					//��ǰѡ�������
	string			mChoiceSkillName;				//��ǰѡ��ļ�����
};



#endif//SKILL_SYSTEM_H





