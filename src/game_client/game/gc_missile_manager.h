
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


	void Add(CMissile* vpMissile);											//添加一个发射体到管理类
    void OnAfterScenePreRender(RtgCamera& inCamera, RtScene* pScene);
	//void Render(RtgDevice& vrDevice,RTGRenderMask vMask);		            //渲染处理
	void Run(float vSecond);												//逻辑运行

private:
	std::list<CMissile*>				mMissileList;
};



#endif//GC_MISSILE_MANAGER_H