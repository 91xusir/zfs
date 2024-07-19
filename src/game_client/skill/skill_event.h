//===========================================================
//		�ļ����ƣ�	skill_event.h
//		��    �ߣ�  ���� 
//		ʱ    �䣺	2003.9.11
//		��    �ݣ�	����SD�������¼���ͷ�ļ�
//		��    ע�� 
//===========================================================
#ifndef SKILL_EVENT_H
#define SKILL_EVENT_H

#include <string>
using std::string;

#include "s_object.h"


class CBaseElement;
class CEffect;
typedef unsigned long ULONG;

//�¼�����
class CEvent
{
public:
	CEvent(){};
	virtual ~CEvent(){};
};

//						����ϵͳ���¼�����

//[�����еļ����б��Ч���б�ͬһ�����ܻ�Ч�����ҽ���һ��]

//��������������������������������������������������������������������������������������������������������������������������
//<******[Ԫ�ػ����¼��Ĵ���]******>
//	=====ͳһ��====
//	1.��ʱ�¼�
class COnTimeEvent:public CEvent
{
public:
	COnTimeEvent(const CS_Object* vpObject)
		:CEvent(),mpObject(vpObject){};
	virtual ~COnTimeEvent(){};
public:
	const CS_Object*	mpObject;
};

//	=====Ч��=====
//2.�ܷ������¼�[Ŀǰ���¼��������⣬����Ҫ�ű�]
class CCanAddEffectEvent:public CEvent
{
public:
	CCanAddEffectEvent(const string& vrEffectType)
		:CEvent(),mrEffectType(vrEffectType){};
	virtual ~CCanAddEffectEvent(){};
public:
	const string& mrEffectType;
};

//	3.����Ч���¼�
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

//	4.ɾ��Ч���¼�
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

//	=====����=====
//	5.�ܷ�ʹ�ü����¼�
//[�ֶ����ʹ�úͶԵص�ʹ��]
class CCanUseSkillEvent:public CEvent
{
public:
	//�����ʹ�ü���
	CCanUseSkillEvent(	const CS_Object*	vpUsingObject,
		const ULONG		vTargetObjectID,
		const int	vSkillID)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(vTargetObjectID),
		mSkillID(vSkillID),
		mX(0),mY(0),mZ(0){};

	//�Եص�ʹ�ü���
	CCanUseSkillEvent( const CS_Object*	vpUsingObject,
		const int	vSkillID,
		const float vX,const float vY,const float vZ)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(NULL),
		mSkillID(vSkillID),
		mX(vX),mY(vY),mZ(vZ){};

	//���Լ�ʹ�ü���
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
//	6.ʹ�ü����¼�
//[���������¼��������Ѿ�����]

//	7.a �Ƿ���ȡ������ʹ���м��ܵ��¼�
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

//	7.ȡ������ʹ���м��ܵ��¼�[Ŀǰ���¼��������⣬����Ҫ�ű�][�����жϵ���������,��׼����ʹ��״̬��ȡ��]
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

//	9.��������ʹ�õļ����¼�[�κμ���,���κ�״̬�¶�����]
class CEndSkillEvent:public CEvent
{
public:
	CEndSkillEvent(const CS_Object*		vpObject)
		:CEvent(),mpObject(vpObject){};
	virtual ~CEndSkillEvent(){};
public:
	const CS_Object*	mpObject;
};

//</******[Ԫ�ػ����¼��Ĵ���]******>

//������������������������������������������������������������������������������������������������������������������������

//<********[�������¼��Ľӿ�]********>
//	=========ͳһ��========
//	1.��ʱ�¼�[����ʱ�¼�����ʹ���м����б���Ч���б�]
//[��Ԫ�ػ����¼��������Ѿ�����]
//	========����========
//	------��ѯ------
//	2.��ѯ�ѻἼ���б���ָ�����ܵĵȼ�[�ü��ܲ����ڣ��ȼ�Ϊ0]
//��ѯ�¼�
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


//	------ѧϰ------
//	3.�����ܷ�ѧϰ�¼�
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

//	4.����Ҫ��ѧϰ�����ӵ���ѧ�Ἴ���¼�
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

//	5.����ѧϰ�¼�[�����⹫��]
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

//	[���������¼�][Ŀǰ��û�������¼������������ݲ���Ҫ]

//	------����------
//	[�ܷ����Ӽ��ܵ��ѻἼ���б��¼�][Ŀǰ��δ�����������Ӽ��ܵ��ѻἼ���б��������ݲ���Ҫ]
//	6.���Ӽ��ܵ��ѻἼ���б��¼�
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

//	7.ɾ�����ܴ��ѻἼ���б��¼�
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

//	------ʹ��------	
//	8.�ܷ�ʹ�ü����¼�[�����ȴ���󣬽����¼�ת�����ѻἼ���б��еĶ�Ӧ���ܣ��缼�ܲ����ڣ�������Ϣ]
//�ܷ�ʹ�ü����¼�
//[��Ԫ�ػ����¼��������Ѿ�����]

//	9.//Ҫ��ʹ�ü����¼�[�ȴ����Ƿ���ʹ�ü��ܣ��ٵ��ü��ܵĿ�ʼʹ���¼�]
//[�ֶ����ʹ�úͶԵص�ʹ��]
class CRequestUseSkillEvent:public CEvent
{
public:
	//�����ʹ�ü���
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

	//�Եص�ʹ�ü���
	CRequestUseSkillEvent( CS_Object*	vpUsingObject,
		const int	vSkillID,
		const float vX,const float vY,const float vZ)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(NULL),
		mSkillID(vSkillID),
		mIsAlreadyEnd(false),
		mX(vX),mY(vY),mZ(vZ){};

	//���Լ�ʹ�ü���
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
	bool  mIsAlreadyEnd;//�ش��ü����Ƿ��Ѿ�����
	const float	  mX;
	const float   mY;
	const float   mZ;
};

//	9b.����ֱ��ʹ��
class CUseSkillEvent:public CEvent
{
public:
	//�����ʹ�ü���
	CUseSkillEvent( CS_Object*	vpUsingObject,
		const ULONG		vTargetObjectID,
		const int	vSkillID)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(vTargetObjectID),
		mSkillID(vSkillID),
		mIsAlreadyEnd(false),
		mX(0),mY(0),mZ(0){};

	//�Եص�ʹ�ü���
	CUseSkillEvent( CS_Object*	vpUsingObject,
		const int	vSkillID,
		const float vX,const float vY,const float vZ)
		:CEvent(),
		mpUsingObject(vpUsingObject),
		mTargetObjectID(NULL),
		mSkillID(vSkillID),
		mIsAlreadyEnd(false),
		mX(vX),mY(vY),mZ(vZ){};
	//���Լ�ʹ�ü���
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
	bool  mIsAlreadyEnd;//�ش��ü����Ƿ��Ѿ�����
	float	  mX;
	float   mY;
	float   mZ;
};

//[��Ԫ�ػ����¼��������Ѿ�����]
//	10.ȡ������ʹ���м��ܵ��¼�[�������ܺ��ܼ���ı�������][������ȡ����ǿ��ȡ��][��׼����ʹ��״̬��ȡ��][��ĳ������]
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

//	11.a �Ƿ���ȡ����������ʹ�õ��������ܵ��¼�[�����⿪��]
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

//	11.ȡ����������ʹ�õ��������ܵ��¼�[������ȡ����ǿ��ȡ��][��׼����ʹ��״̬����ȡ��]
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


//	12.ǿ�ƽ�������ʹ�õļ����¼�[�κμ���,���κ�״̬�¶��ܽ���][��ĳ������]
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

//	13.ǿ�ƽ�����������ʹ�õļ����¼�[�κμ���,���κ�״̬�¶�����]
class CForceEndAllSkillEvent:public CEvent
{
public:
	CForceEndAllSkillEvent(const CS_Object* vpObject)
		:CEvent(),mpObject(vpObject){};
	virtual ~CForceEndAllSkillEvent(){};
public:
	const CS_Object* mpObject;
};


//	[��������ʹ��][��OnTime���е���]

//	========Ч��========
//	13.����Ч���¼�
//[��Ԫ�ػ����¼��������Ѿ�����]
//	14.ɾ��Ч���¼�
//[��Ԫ�ػ����¼��������Ѿ�����]

//������������������������������������δʹ�õ��¼���������������������������������������


//�����¼�
class CAttackEvent:public CEvent
{
public:
	CAttackEvent():CEvent(){};
	virtual ~CAttackEvent(){};
};

//�������¼�
class CUnAttackEvent:public CEvent
{
public:
	CUnAttackEvent():CEvent(){};
	virtual ~CUnAttackEvent(){};
};

//װ���¼�
class CEquipEvent:public CEvent
{
public:
	CEquipEvent():CEvent(){};
	virtual ~CEquipEvent(){};
};

//ж���¼�
class CUnEquipEvent:public CEvent
{
public:
	CUnEquipEvent():CEvent(){};
	virtual ~CUnEquipEvent(){};
};
//�ƶ��¼�
class CMoveEvent:public CEvent
{
public:
	CMoveEvent():CEvent(){};
	virtual ~CMoveEvent(){};

};

//�����¼�
class CUpdateEvent:public CEvent
{
public:
	CUpdateEvent():CEvent(){};
	virtual ~CUpdateEvent(){};
};


#endif//SKILL_EVENT_H