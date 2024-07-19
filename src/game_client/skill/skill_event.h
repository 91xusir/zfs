//===========================================================
//		文件名称：	skill_event.h
//		作    者：  吴麟 
//		时    间：	2003.9.11
//		内    容：	关于SD技能中事件的头文件
//		备    注： 
//===========================================================
#ifndef SKILL_EVENT_H
#define SKILL_EVENT_H

#include <string>
using std::string;

#include "s_object.h"


class CBaseElement;
class CEffect;
typedef unsigned long ULONG;

//事件基类
class CEvent
{
public:
	CEvent(){};
	virtual ~CEvent(){};
};

//						技能系统的事件处理

//[容器中的技能列表和效果列表同一个技能或效果有且仅有一个]

//—————————————————————————————————————————————————————————————
//<******[元素基类事件的处理]******>
//	=====统一的====
//	1.定时事件
class COnTimeEvent:public CEvent
{
public:
	COnTimeEvent(const CS_Object* vpObject)
		:CEvent(),mpObject(vpObject){};
	virtual ~COnTimeEvent(){};
public:
	const CS_Object*	mpObject;
};

//	=====效果=====
//2.能否增加事件[目前该事件处理不特殊，不需要脚本]
class CCanAddEffectEvent:public CEvent
{
public:
	CCanAddEffectEvent(const string& vrEffectType)
		:CEvent(),mrEffectType(vrEffectType){};
	virtual ~CCanAddEffectEvent(){};
public:
	const string& mrEffectType;
};

//	3.增加效果事件
class CAddEffectEvent:public CEvent
{
public:
	CAddEffectEvent(const CS_Object* vpObject,const int vEffectID,const int vLevel)
		:CEvent(),
		mpObject(vpObject),
		mEffectID(vEffectID),
		mLevel(vLevel){};

	virtual ~CAddEffectEvent(){};
public:
	const CS_Object*	mpObject;
	const int			mEffectID;
	const int			mLevel;

};

//	4.删除效果事件
class CRemoveEffectEvent:public CEvent
{
public:
	CRemoveEffectEvent(const CS_Object*	vpObject,const int vEffectID)
		:CEvent(),mpObject(vpObject),mEffectID(vEffectID){};
	virtual ~CRemoveEffectEvent(){};
public:
	const CS_Object*		mpObject;
	const int				mEffectID;
};

//	=====技能=====
//	5.能否使用技能事件
//[分对物件使用和对地点使用]
class CCanUseSkillEvent:public CEvent
{
public:
	//对物件使用技能
	CCanUseSkillEvent(	const CS_Object*	vpUsingObject,
		const ULONG		vTargetObjectID,
		const int	vSkillID)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(vTargetObjectID),
		mSkillID(vSkillID),
		mX(0),mY(0),mZ(0){};

	//对地点使用技能
	CCanUseSkillEvent( const CS_Object*	vpUsingObject,
		const int	vSkillID,
		const float vX,const float vY,const float vZ)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(NULL),
		mSkillID(vSkillID),
		mX(vX),mY(vY),mZ(vZ){};

	//对自己使用技能
	CCanUseSkillEvent(	const CS_Object* vpUsingObject,
		const int vSkillID)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(NULL),
		mSkillID(vSkillID),
		mX(0),mY(0),mZ(0){};


	CCanUseSkillEvent(  const CS_Object*	vpUsingObject,
		const ULONG			vTargetObjectID,
		const int			vSkillID,
		const float vX,const float vY,const float vZ)
		:CEvent(),mpUsingObject(vpUsingObject),
		mTargetObjectID(vTargetObjectID),
		mSkillID(vSkillID),
		mX(vX),mY(vY),mZ(vZ){};
	virtual ~CCanUseSkillEvent(){};
public:
	const CS_Object*		mpUsingObject;
	const ULONG				mTargetObjectID;
	const int				mSkillID;
	const float mX;
	const float mY;
	const float mZ;
};
//	6.使用技能事件
//[在容器类事件处理中已经定义]

//	7.a 是否能取消正在使用中技能的事件
class CCanCancelEvent:public CEvent
{
public:
	CCanCancelEvent(const CS_Object*	vpCancelObject,const bool vForceCancel=false)
		:CEvent(),mpCancelObject(vpCancelObject),
		mForceCancel(vForceCancel){};
	virtual ~CCanCancelEvent(){};
public:
	const CS_Object*		mpCancelObject;
	const bool				mForceCancel;
};

//	7.取消正在使用中技能的事件[目前该事件处理不特殊，不需要脚本][可以中断的主动技能,在准备和使用状态下取消]
class CCancelEvent:public CEvent
{
public:
	CCancelEvent(const CS_Object*	vpCancelObject,const bool vForceCancel=false)
		:CEvent(),mpCancelObject(vpCancelObject),
		mForceCancel(vForceCancel),mRemoveUsingSkill(false){};
	virtual ~CCancelEvent(){};
public:
	const CS_Object*	mpCancelObject;
	const bool			mForceCancel;
	bool				mRemoveUsingSkill;
};

//	8.

//	9.结束正在使用的技能事件[任何技能,在任何状态下都结束]
class CEndSkillEvent:public CEvent
{
public:
	CEndSkillEvent(const CS_Object*		vpObject)
		:CEvent(),mpObject(vpObject){};
	virtual ~CEndSkillEvent(){};
public:
	const CS_Object*	mpObject;
};

//</******[元素基类事件的处理]******>

//————————————————————————————————————————————————————————————

//<********[容器外事件的接口]********>
//	=========统一的========
//	1.定时事件[将定时事件发给使用中技能列表，和效果列表]
//[在元素基类事件处理中已经定义]
//	========技能========
//	------查询------
//	2.查询已会技能列表中指定技能的等级[该技能不存在，等级为0]
//查询事件
class CQuerySkillEvent:public CEvent
{
public:
	CQuerySkillEvent(const int vSkillID)
		:CEvent(),mSkillID(vSkillID),mGetLevel(0){};
	virtual ~CQuerySkillEvent(){};
public:
	const int		mSkillID;
	int				mGetLevel;
};


//	------学习------
//	3.技能能否学习事件
class CCanStudySkillEvent:public CEvent
{
public:
	CCanStudySkillEvent(const CS_Object*	vpObject,const int vSkillID,const int vLevel)
		:CEvent(),mpObject(vpObject),mSkillID(vSkillID),mLevel(vLevel){};
	virtual ~CCanStudySkillEvent(){};
public:
	const CS_Object*		mpObject;
	const int				mSkillID;
	const int				mLevel;

};

//	4.技能要求学习并增加到已学会技能事件
class CRequestStudySkillEvent:public CEvent
{
public:
	CRequestStudySkillEvent(CS_Object*	vpObject,const int vSkillID,const int vLevel)
		:CEvent(),mpObject(vpObject),mSkillID(vSkillID),mLevel(vLevel){};
	virtual ~CRequestStudySkillEvent(){};
public:
	CS_Object*		mpObject;
	const int				mSkillID;
	const int				mLevel;
};

//	5.技能学习事件[不对外公开]
class CStudySkillEvent:public CEvent
{
public:
	CStudySkillEvent(CS_Object*	vpObject,const int vSkillID,const int vLevel)
		:CEvent(),mpObject(vpObject),mSkillID(vSkillID),mLevel(vLevel){};
	virtual ~CStudySkillEvent(){};
public:
	CS_Object*	mpObject;
	const int			mSkillID;
	const int			mLevel;
};

//	[技能忘记事件][目前还没有忘记事件这样的需求，暂不需要]

//	------增加------
//	[能否增加技能到已会技能列表事件][目前还未碰到不能增加技能到已会技能列表的情况，暂不需要]
//	6.增加技能到已会技能列表事件
class CAddSkillEvent:public CEvent
{
public:
	CAddSkillEvent(CS_Object* vpObject,const int vSkillID,const int vLevel)
		:CEvent(),mpObject(vpObject),mSkillID(vSkillID),mLevel(vLevel){};

	virtual ~CAddSkillEvent(){};
public:
	CS_Object*	mpObject;
	const int			mSkillID;
	int					mLevel;
};

//	7.删除技能从已会技能列表事件
class CDelSkillEvent:public CEvent
{
public:
	CDelSkillEvent(const CS_Object* vpObject,const int vSkillID)
		:CEvent(),mpObject(vpObject),mSkillID(vSkillID)
	{};
	virtual ~CDelSkillEvent(){};
public:
	const CS_Object* mpObject;
	const int mSkillID;
};

//	------使用------	
//	8.能否使用技能事件[容器先处理后，将该事件转发给已会技能列表中的对应技能，如技能不存在，出错信息]
//能否使用技能事件
//[在元素基类事件处理中已经定义]

//	9.//要求使用技能事件[先处理是否能使用技能，再调用技能的开始使用事件]
//[分对物件使用和对地点使用]
class CRequestUseSkillEvent:public CEvent
{
public:
	//对物件使用技能
	CRequestUseSkillEvent( CS_Object*	vpUsingObject,
		const ULONG		vTargetObjectID,
		const int	vSkillID,
		const float vX,const float vY,const float vZ)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(vTargetObjectID),
		mSkillID(vSkillID),
		mIsAlreadyEnd(false),
		mX(vX),mY(vY),mZ(vZ){};

	//对地点使用技能
	CRequestUseSkillEvent( CS_Object*	vpUsingObject,
		const int	vSkillID,
		const float vX,const float vY,const float vZ)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(NULL),
		mSkillID(vSkillID),
		mIsAlreadyEnd(false),
		mX(vX),mY(vY),mZ(vZ){};

	//对自己使用技能
	CRequestUseSkillEvent(CS_Object* vpUsingObject,
		const int vSkillID)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(vpUsingObject->m_oId),
		mSkillID(vSkillID),
		mIsAlreadyEnd(false),
		mX(0),mY(0),mZ(0){};

	virtual ~CRequestUseSkillEvent(){};
public:
	CS_Object*	mpUsingObject;
	const ULONG			mTargetObjectID;
	const int			mSkillID;
	bool  mIsAlreadyEnd;//回传该技能是否已经结束
	const float	  mX;
	const float   mY;
	const float   mZ;
};

//	9b.技能直接使用
class CUseSkillEvent:public CEvent
{
public:
	//对物件使用技能
	CUseSkillEvent( CS_Object*	vpUsingObject,
		const ULONG		vTargetObjectID,
		const int	vSkillID)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(vTargetObjectID),
		mSkillID(vSkillID),
		mIsAlreadyEnd(false),
		mX(0),mY(0),mZ(0){};

	//对地点使用技能
	CUseSkillEvent( CS_Object*	vpUsingObject,
		const int	vSkillID,
		const float vX,const float vY,const float vZ)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(NULL),
		mSkillID(vSkillID),
		mIsAlreadyEnd(false),
		mX(vX),mY(vY),mZ(vZ){};
	//对自己使用技能
	CUseSkillEvent(CS_Object* vpUsingObject,
		const int vSkillID)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(NULL),
		mSkillID(vSkillID),
		mIsAlreadyEnd(false),
		mX(0),mY(0),mZ(0){};
	CUseSkillEvent(CS_Object*	vpUsingObject,
		const ULONG			vTargetObjectID,
		const int			vSkillID,
		const float vX,const float vY,const float vZ)
		:CEvent(),mpUsingObject(vpUsingObject),
		mTargetObjectID(vTargetObjectID),
		mSkillID(vSkillID),
		mIsAlreadyEnd(false),
		mX(vX),mY(vY),mZ(vZ){};

	virtual ~CUseSkillEvent(){};

public:
	CS_Object*	mpUsingObject;
	ULONG		mTargetObjectID;
	int			mSkillID;
	bool  mIsAlreadyEnd;//回传该技能是否已经结束
	float	  mX;
	float   mY;
	float   mZ;
};

//[在元素基类事件处理中已经定义]
//	10.取消正在使用中技能的事件[主动技能和能激活的被动技能][分正常取消和强制取消][在准备和使用状态能取消][仅某个技能]
class CCancelSkillEvent:public CEvent
{
public:
	CCancelSkillEvent(const CS_Object* vpCancelObject,const int vSkillID,
		const bool vForceCancel=false)
		:CEvent(),mpCancelObject(vpCancelObject),mSkillID(vSkillID),
		mForceCancel(vForceCancel){};
	virtual ~CCancelSkillEvent(){};
public:
	const CS_Object* mpCancelObject;
	const int		mSkillID;
	const bool		mForceCancel;
};

//	11.a 是否能取消所有正在使用的主动技能的事件[不对外开放]
class CCanCancelAllSkillEvent:public CEvent
{
public:
	CCanCancelAllSkillEvent(const CS_Object*	vpCancelObject,const bool vForceCancel=false)
		:CEvent(),mpCancelObject(vpCancelObject),
		mForceCancel(vForceCancel){};
	virtual ~CCanCancelAllSkillEvent(){};
public:
	const CS_Object*		mpCancelObject;
	const bool				mForceCancel;
};

//	11.取消所有正在使用的主动技能的事件[分正常取消和强制取消][在准备和使用状态下能取消]
class CCancelAllSkillEvent:public CEvent
{
public:
	CCancelAllSkillEvent(const CS_Object*	vpCancelObject,const bool vForceCancel=false)
		:CEvent(),mpCancelObject(vpCancelObject),
		mForceCancel(vForceCancel){};
	virtual ~CCancelAllSkillEvent(){};
public:
	const CS_Object*	mpCancelObject;
	const bool			mForceCancel;
};


//	12.强制结束正在使用的技能事件[任何技能,在任何状态下都能结束][仅某个技能]
class CForceEndSkillEvent:public CEvent
{
public:
	CForceEndSkillEvent(const CS_Object* vpObject,const int vSkillID)
		:CEvent(),mpObject(vpObject),mSkillID(vSkillID)
	{};
	virtual ~CForceEndSkillEvent(){};
public:
	const CS_Object*	mpObject;
	const int			mSkillID;
};

//	13.强制结束所有正在使用的技能事件[任何技能,在任何状态下都结束]
class CForceEndAllSkillEvent:public CEvent
{
public:
	CForceEndAllSkillEvent(const CS_Object* vpObject)
		:CEvent(),mpObject(vpObject){};
	virtual ~CForceEndAllSkillEvent(){};
public:
	const CS_Object* mpObject;
};


//	[正常结束使用][由OnTime自行调用]

//	========效果========
//	13.增加效果事件
//[在元素基类事件处理中已经定义]
//	14.删除效果事件
//[在元素基类事件处理中已经定义]

//—————————————————还未使用的事件———————————————————


//攻击事件
class CAttackEvent:public CEvent
{
public:
	CAttackEvent():CEvent(){};
	virtual ~CAttackEvent(){};
};

//被攻击事件
class CUnAttackEvent:public CEvent
{
public:
	CUnAttackEvent():CEvent(){};
	virtual ~CUnAttackEvent(){};
};

//装备事件
class CEquipEvent:public CEvent
{
public:
	CEquipEvent():CEvent(){};
	virtual ~CEquipEvent(){};
};

//卸载事件
class CUnEquipEvent:public CEvent
{
public:
	CUnEquipEvent():CEvent(){};
	virtual ~CUnEquipEvent(){};
};
//移动事件
class CMoveEvent:public CEvent
{
public:
	CMoveEvent():CEvent(){};
	virtual ~CMoveEvent(){};

};

//升级事件
class CUpdateEvent:public CEvent
{
public:
	CUpdateEvent():CEvent(){};
	virtual ~CUpdateEvent(){};
};


#endif//SKILL_EVENT_H