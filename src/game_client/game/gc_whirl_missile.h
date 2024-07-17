
#ifndef GC_WHIRL_MISSILE_H
#define GC_WHIRL_MISSILE_H

#include "gc_circle_missile.h"


//const float DEFAULT_WHIRL_MISSILE_SPEED=400.0f;


class CWhirlMissile:public CCircleMissile
{
public:
	CWhirlMissile(DWORD vSourceID,DWORD vTargetID,char vpGraphName[]);
	~CWhirlMissile(){};

	virtual void OnRun(float vSecond);
	virtual void OnArrive();

private:
	void	Revise(DWORD vSourceID,DWORD vTargetID);
	void    DealOffsetMove(float vSecond);
	bool	GetSourcePosition(DWORD vSourceID,RtgVertex3& vrPosition);
	bool    GetTargetPosition(DWORD vTargetID,RtgVertex3& vrPosition);


	bool	mFirstRevise;					//�Ƿ��ǵ�һ�ν���
	float   mReviseTime;					//����ʱ��
	DWORD	mSourceID;						//ԴID
	DWORD	mTargetID;						//Ŀ��ID
	RtgVertex3	mOldSourcePosition;			//�����Դλ��
	RtgVertex3	mOldTargetPosition;			//�����Ŀ��λ��
	RtgVertex3  mOffset;					//ƫ���ܵ�·��
	RtgVertex3  mEachOffsetSpeed;			//���������ϵ�ƫ�Ʒ��ٶ�
};







#endif//GC_WHIRL_MISSILE_H
