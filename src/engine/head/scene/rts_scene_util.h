
#ifndef _RT_SCENE_UTIL_H_
#define _RT_SCENE_UTIL_H_

// �����ĸ������ܣ����ں�����ϵͳ�Ľӿڣ�������ϵͳ����Ҫ�õ���������ϵͳ��
//     ȴҪ�ͳ������ֽ����Ļ������԰ѽӿ���ӵ�����
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
