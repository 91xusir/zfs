
#ifndef _RT_SCENE_UTIL_H_
#define _RT_SCENE_UTIL_H_

// 场景的辅助功能，用于和其他系统的接口，当其他系统不需要用到整个场景系统，
//     却要和场景部分交互的话，可以把接口添加到这里
class RtsSceneUtil
{
public:
    RtsSceneUtil(){}
    virtual ~RtsSceneUtil(){}

   // virtual RtgLight* CreateDynamicLight()=0;
 //   virtual void DestroyDynamicLight(RtgLight* pLight)=0;
 //   virtual void UpdateDynamicLight(RtgLight* pLight)=0;

	/*
	bool ClipTerrain(const RtgAABB& inAABB, RtgVertex3* pVertices, int& iVertexNum, short* pIndices, int& iIndexNum);
	bool ClipObjectStatic(const RtgAABB& inAABB, vector<CRT_ActorInstance> &actVec);
	bool ClipObjectDynamic(const RtgAABB& inAABB, vector<CRT_ActorInstance> &actVec);
	*/
};

#endif // _RT_SCENE_UTIL_H_
/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
