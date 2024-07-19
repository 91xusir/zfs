
#ifndef GC_TRACK_MISSILE_H
#define GC_TRACK_MISSILE_H

#include "gc_beeline_missile.h"



const float DEFAULT_TRACK_MISSILE_SPEED=400.0f;

class CTrackMissile:public CBeelineMissile
{
public:
	CTrackMissile(RtgVertex3& vrSourcePosition,DWORD vTargetID,
		char vpGraphName[],CMissileListener* vpListener=NULL,float vSpeed=DEFAULT_TRACK_MISSILE_SPEED);

	~CTrackMissile(){};

	virtual void OnRun(float vSecond);

private:
	void	SetTargetFromID(DWORD vTargetID);

	float   mReviseTime;
	DWORD	mTargetID;
};







#endif//GC_TRACK_MISSILE_H
