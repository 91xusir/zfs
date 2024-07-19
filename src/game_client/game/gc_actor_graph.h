
#ifndef GC_ACTOR_GRAPH_H
#define GC_ACTOR_GRAPH_H
#include "scene/rts_path_find.h"
#include "scene/rt_scene.h"

class CRT_ActorInstance;
class rt_graph::RtgMatrix12;
class CRT_PoseNotify;
class CRT_ActorInstance;
struct SCreModel;

namespace rt_graph {

    struct _RtgTextItem;
    typedef _RtgTextItem RtgTextItem;
}

//
//	class GcActorGraph
//   角色的图形类
//
class GcActorGraph
{
public:
	GcActorGraph();
	~GcActorGraph();

    CRT_ActorInstance* p()  { return m_pActor; }
    bool LoadActor(const char* szActorName);
    bool ReplaceActor(const char* szActorName);
    void ClearActor();
    void UnloadAllSkin();
    const char* FileName();

    void DisplayActor(bool vDisplay);

	void SetPosition(float vX,float vY,float vZ);
	void SetRotation(RtgMatrix16& vrMatrix16);
	void SetRotation(RtgMatrix12& vrMatrix12);

    void FrameMove(unsigned long _estime);
	void Render(RtgDevice& vrDevice,RTGRenderMask vMask,bool vShadow);
    void RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale);
	void RenderShadow(RtgDevice &vrDevice, int bDynamicShadow, float radius);
	void RenderDynamicShadow(RtgDevice &vrDevice);
	void RenderSimpleShadow(RtgDevice &vrDevice,float radius);
	void RenderSelect(RtgDevice &vrDevice,RtgVertex3 &color, float radius);

	bool ClipScene(RtgAABB &aabb);
	void SetBaseColor(RtgVertex3& vrColor);
	void RenderFloatTexture(RtgDevice &dev,RtgTextItem* texItem,int alphaType,RtgVertex3 &color,float radius);

    bool Link(const char* szLinkName, const char* szActorName);
    bool UnLink(const char* szLinkName);
    bool IsLink(const char* szLinkName);
    const char* GetLinkActorName(const char* szLinkName);
    GcActorGraph* GetLinkActor(const char* szLinkName);
    bool HaveLink();

    bool PlayPose(const char vpPoseName[], bool loop=false, float fSpeed=1.f);

    void RegisterActorNotify(CRT_PoseNotify* vNotify);
	void ApplyShaderEffect(const char vName[]);
	void RemoveShaderEffect(const char vName[]);
	void ApplyEffect(const char vName[]);
	void RemoveEffect(const char vName[]);
	void UnloadSkin(const char *file);
	void LoadSkin(const char *file, bool bDisableMip);
	void EnableShadow(bool vEnable);
	RtgAABB* GetBoundingBox();

	void SetVisible(float visible);
	bool IsDisplay() { return m_bDisplay; };

private:
	void Reset();

    bool                                    m_bDisplay;     //是否显示
    CRT_ActorInstance*					    m_pActor;       // 图形实例
    std::map<std::string, GcActorGraph*>    m_mapChildren;
};

#endif	//GC_ACTOR_GRAPH_H
