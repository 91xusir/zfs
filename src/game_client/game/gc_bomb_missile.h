

#ifndef GC_BOMB_MISSILE_H
#define GC_BOMB_MISSILE_H
#include "gc_missile.h"


class CBombMissile:public CMissile,public CRT_PoseNotify
{
public:

	CBombMissile(RtgVertex3& vrPosition,char vpGraphName[]);
	~CBombMissile();

	//CMissile�Ľӿ�
	virtual void OnRun(float vSecond);

	//CRT_PoseNotify�Ľӿ�
	virtual void OnPoseEnd(SRT_Pose* vpPose); 
};




#endif//GC_BOMB_MISSILE_H