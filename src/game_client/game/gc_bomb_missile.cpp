
#include "gc_bomb_missile.h"
#include "gc_global.h"

float MAX_BOMB_MISSILE_EXIST_TIME=50.0f;



CBombMissile::CBombMissile(RtgVertex3& vrPosition,char vpGraphName[])
:CMissile(vrPosition,vpGraphName,NULL,MAX_BOMB_MISSILE_EXIST_TIME)
{
	UpdateGraph();
	Graph().RegisterActorNotify(this);//�ǼǶ������ŵĻص�����
	
	if(Graph().PlayPose("effect"))  //��Ч�Ĳ���
	{
	}
	else
	{
		// RtCoreLog().Error("���Ŷ���ʧ��,ģ���ļ�[%s] ����[%s] ",Graph().FileName(),"effect");
	}
}

CBombMissile::~CBombMissile()
{
}

void CBombMissile::OnRun(float vSecond)
{
	//����Ҫ��Щʲô
	return ;
}


void CBombMissile::OnPoseEnd(SRT_Pose* vpPose)
{
	Arrive();//Missile�Ľӿڱ�ʾ����Ŀ�꣬�����ʾ��Missile�������ڽ���
	DeleteSelf();
}



