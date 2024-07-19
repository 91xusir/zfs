
#include "gc_bomb_missile.h"
#include "gc_global.h"

float MAX_BOMB_MISSILE_EXIST_TIME=50.0f;



CBombMissile::CBombMissile(RtgVertex3& vrPosition,char vpGraphName[])
:CMissile(vrPosition,vpGraphName,NULL,MAX_BOMB_MISSILE_EXIST_TIME)
{
	UpdateGraph();
	Graph().RegisterActorNotify(this);//登记动画播放的回调函数
	
	if(Graph().PlayPose("effect"))  //特效的播放
	{
	}
	else
	{
		// RtCoreLog().Error("播放动作失败,模型文件[%s] 动作[%s] ",Graph().FileName(),"effect");
	}
}

CBombMissile::~CBombMissile()
{
}

void CBombMissile::OnRun(float vSecond)
{
	//不需要做些什么
	return ;
}


void CBombMissile::OnPoseEnd(SRT_Pose* vpPose)
{
	Arrive();//Missile的接口表示到达目标，这里表示该Missile的生存期结束
	DeleteSelf();
}



