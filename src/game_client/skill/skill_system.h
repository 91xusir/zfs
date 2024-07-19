//===========================================================
//		文件名称：	skill_system.h
//		作    者：  吴麟 
//		时    间：	
//		内    容：	
//		备    注： 
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
	CSkillSystem();						//构造
	virtual ~CSkillSystem();			//析构

	virtual bool Init();				//初始化
	virtual void OnFrame(unsigned long time,unsigned long delta){};
	virtual void Exit(){};
	virtual const char *GetName(){return "CSkillSystem";};

public:
	CSkillManager   mSkillManager;					//技能管理
	CEffectManager  mEffectManager;					//效果管理

private:
	virtual bool ConcreteInit()=0;					//子类的初始化

	string			mChoiceAttrib;					//当前选择的属性
	string			mChoiceSkillName;				//当前选择的技能名
};



#endif//SKILL_SYSTEM_H





