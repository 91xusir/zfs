

#ifndef GC_CIRCLE_MISSILE_H
#define GC_CIRCLE_MISSILE_H
#include "gc_missile.h"

const float DEFAULT_CIRCLE_MISSILE_SPEED=240.0f;			//Ĭ�ϵ��ٶ�
const float DEFAULT_LONG_WIDTH_RATIO=0.3f;					//Ĭ�ϵ���Բ�ĳ����
const float DEFAULT_SELF_RAD=1080.0f;						//Ĭ�ϵ���ת�ٶ�[����]

//�������Բֻ���켣�����Բ���ٶȴ����䣬�����ٶȻ�ı�[�����]
class CCircleMissile:public CMissile
{
public:
	CCircleMissile(
        RtgVertex3& vrSourcePosition,
        RtgVertex3& vrTargetPosition,
		char vpGraphName[],
        float vRatio=DEFAULT_LONG_WIDTH_RATIO,
        float vSelfRad=DEFAULT_SELF_RAD,
		CMissileListener* vpListener=NULL,
        float vSpeed=DEFAULT_CIRCLE_MISSILE_SPEED);

	~CCircleMissile(){};

	void SetTargetEffect(char vpEffectName[]);				//����Ŀ�ĵص���Ч

	void SetTargetPosition(RtgVertex3& vrTargetPosition);	//����Ŀ��λ��
protected:
	
	void DealMove(float vSecond);
	virtual void OnRun(float vSecond);

	bool		mPassMiddleTime;			//�Ƿ񾭹����м�ʱ��
	float		mTime;						//��ʣ����ʱ�䵽Ŀ�ĵ�
	float		mPassTime;					//�����˶���ʱ��

	virtual void OnArrive();
private:

	float		mRatio;						//��Բ�ĳ����
	float		mSpeed;						//Բ���˶������ٶ�
	RtgVertex3	mTargetPosition;			//Ŀ���λ��
	RtgVertex3	mCenter;					//Բ������λ��
	float		mStartDegree;				//��ʼ�Ƕ�
	float		mRad;						//���ٶ�[����]
	float		mSelfRad;					//��ת���ٶ�[����]
	float		mR;							//�뾶
	std::string	mTargetEffectName;			//Ŀ���Ҫ���ŵ���Ч������
};



#endif//GC_CIRCLE_MISSILE_H