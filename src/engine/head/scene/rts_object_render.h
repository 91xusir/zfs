
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rts_object_render.h
//     Author : zxy
// CreateTime : 2010.03.12
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _RtsObjectRender_H_
#define _RtsObjectRender_H_

class RtsObject;
class RtScene;
class RtsDynamicLight;

class RtsObjectRender
{
    friend class RtScene;

    enum 
    {
        Object_Array_Init_Size = 1024,
        Object_Array_Gain_Size = 32
    };

public :

    void AddObject(RtsObject* _obj);
    void DelObject(RtsObject* _obj);
    void Reset();
    void PrepareRender(RtgCamera* cam);
    void ProcessDynamicLight(RtsDynamicLight* light);
    void RenderSceneObjectNormal(RtScene* scene, RtgCamera* cam, RTGRenderMask _Mask);
    void RenderSceneObjectShadow(RtScene* scene, RtgCamera* cam);
    void RenderSceneMirrorObject(RtScene* scene, RtgCamera* cam);
    void Update(float elapse);

    RtsObjectRender();
    virtual ~RtsObjectRender();

protected :
    farray<RtsObject*> m_objArray;
    int                m_objNum;

};

#endif