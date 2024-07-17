
#ifndef GC_MISSILE_MANAGER_H
#define GC_MISSILE_MANAGER_H
#include <list>
#include "scene/rt_scene.h"


class CMissile;

class CMissileManager
{
public:
	CMissileManager();
	~CMissileManager();


	void Add(CMissile* vpMissile);											//���һ�������嵽������
    void OnAfterScenePreRender(RtgCamera& inCamera, RtScene* pScene);
	//void Render(RtgDevice& vrDevice,RTGRenderMask vMask);		            //��Ⱦ����
	void Run(float vSecond);												//�߼�����

private:
	std::list<CMissile*>				mMissileList;
};



#endif//GC_MISSILE_MANAGER_H