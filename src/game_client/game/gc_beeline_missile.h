

#ifndef GC_BEELINE_MISSILE_H
#define GC_BEELINE_MISSILE_H
#include "gc_missile.h"

const float DEFAULT_BEELINE_MISSILE_SPEED=100.0f;

class CBeelineMissile:public CMissile
{
public:
	CBeelineMissile(RtgVertex3& vrSourcePosition,RtgVertex3& vrTargetPosition,
		char vpGraphName[],CMissileListener* vpListener=NULL,float vSpeed=DEFAULT_BEELINE_MISSILE_SPEED,bool isParbola = false);

	~CBeelineMissile(){};

	void SetTargetEffect(char vpEffectName[]);				//����Ŀ�ĵص���Ч

protected:
	void SetTargetPosition(RtgVertex3& vrTargetPosition);	//����Ŀ��λ�ú�Missile����תλ��
	void DealMove(float vSecond);
	virtual void OnRun(float vSecond);

	float		mTime;						//��ʣ����ʱ�䵽Ŀ�ĵ�
	float       mTotalTime;
	float       mCurrentMatrix_Z;
	bool         mParabola;//  ���������߱�־
private:
	
	virtual void OnArrive();

	
	float		mSpeed;						//������ƶ��ٶ�
	RtgVertex3	mTargetPosition;			//Ŀ���λ��
	RtgVertex3  mEachSpeed;					//���������ϵķ��ٶ�
	std::string	mTargetEffectName;			//Ŀ���Ҫ���ŵ���Ч������
};



#endif//GC_BEELINE_MISSILE_H