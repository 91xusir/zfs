
#ifndef GC_TRACK_2_MISSILE_H
#define GC_TRACK_2_MISSILE_H

#include "gc_beeline_missile.h"
#include "gc_circle_missile.h"



const float DEFAULT_TRACK_2_MISSILE_SPEED=400.0f;

class CTrack2Missile:public CCircleMissile
{
public:
	CTrack2Missile(
        RtgVertex3& vrSourcePosition,
        DWORD vTargetID,
		char vpGraphName[],
        CMissileListener* vpListener=NULL,
        float vSpeed=DEFAULT_TRACK_2_MISSILE_SPEED);

	~CTrack2Missile(){};

	virtual void OnRun(float vSecond);

private:
	bool    GetTargetPosition(DWORD vTargetID,RtgVertex3& vrPosition);
	void	Revise(DWORD vTargetID);
	void	DealOffsetMove(float vSecond);

	bool	mFirstRevise;					//�Ƿ��ǵ�һ�ν���
	float   mReviseTime;
	DWORD	mTargetID;
	RtgVertex3	mOldTargetPosition;			//�����Ŀ��λ��
	RtgVertex3  mOffset;					//ƫ���ܵ�·��
	RtgVertex3  mEachOffsetSpeed;			//���������ϵ�ƫ�Ʒ��ٶ�

};







#endif//GC_TRACK_2_MISSILE_H
