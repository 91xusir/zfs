

#ifndef GC_TRACK_BOMB_MISSILE_H
#define GC_TRACK_BOMB_MISSILE_H
#include "gc_bomb_missile.h"


class CTrackBombMissile:public CBombMissile
{
public:
	CTrackBombMissile(DWORD vTargetID,char vpGraphName[]);
	~CTrackBombMissile(){};


	virtual void OnRun(float vSecond);
private:
	void	SetTargetFromID(DWORD vTargetID);
	
	float   mReviseTime;
	DWORD	mTargetID;

};




#endif//GC_TRACK_BOMB_MISSILE_H