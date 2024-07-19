#ifndef __CHA_ANIM_H__
#define __CHA_ANIM_H__

/*
typedef void(*TPoseEvent)(CRT_AnimObject *object,SRT_Pose *pose,SRT_PoseEvent *event); 
enum EPoseState
{
	POSE_STATE_NONE    = 0;
	POSE_STATE_PLAYING = 1;
	POSE_STATE_ENDING  = 2;
}
*/

class CRT_PoseNotify
{
public:
	CRT_PoseNotify() {}
	virtual ~CRT_PoseNotify() {}

	virtual void OnPoseBegin(SRT_Pose *pose) {}
	virtual void OnPoseEnd(SRT_Pose *pose) {}
	virtual void OnPoseEvent(SRT_Pose *pose,SRT_PoseEvent *event) {}
	virtual void OnPoseBreak(SRT_Pose *oldPose,SRT_Pose *newPose) {}
};

//
// skin instance
//
class CRT_SkinInstance : public CRT_ObjectInst
{
	RT_DECLARE_DYNCREATE(CRT_SkinInstance, CRT_ObjectInst, NULL, "")
	CRT_SkinInstance();
	virtual ~CRT_SkinInstance();
	
	// from CRT_ObjectInst
	virtual void InstanceInit();
	virtual void InstanceExit();
   
	//
	// runtime option
	//
	float           m_visible;
	// float           m_frameVisible;
	void SetVisible(float visible) { m_visible = visible; }

	//
	// finial option
	//
	float GetFinalVisible();

	// render function
	bool Create(CRT_ActorInstance *actor, RtgDevice *dev);
	void Render(RtgDevice *dev, RTGRenderMask mask);
	void RenderShadow(RtgDevice *dev,float fShadowFadeFactor = 0.5f);
	void RenderWireFrame(RtgDevice *dev);
		
	// anim function
	void UseFrame(float frame);
	void Tick(DWORD delta);


	// debug function
	void Output();

	// for scene render
	void GetBoundingBox(RtgRangeAABB &box);
	bool ComputeVC(RtgMatrix16 &world,vector<DWORD> &vcVec,int offset,RtgVertex3 vSkyLight, RtgLightItem* pLights);
	bool SetVC(vector<DWORD> &vcVec,int offset);
	void SetBaseColor(RtgVertex3 &color);

	void GetRelatedRes(vector<SRelatedRes> &res);

	CRT_Skin *m_skin;
	vector<SRT_StaticVtx> m_verVec;
	vector<DWORD>		 *m_vcVec;
	int					  m_vcOffset;
	bool				  m_updateNor;

    // material
    void ChangeMaterial(CRT_ObjectData *data);
    void OnMaterialChange();
    CRT_MtlLibInstance *m_mtlLib;

	RtgDevice *m_renderDev;
	CRT_ActorInstance *m_actor;
};

/*
enum EShadowMode
{
	SHADOW_NULL    = 0,
	SHADOW_STATIC  = 1,
	SHADOW_DYNAMIC = 2,
};

enum ERenderMode
{
	RENDER_SOLID = 0,
	RENDER_WIRE  = 1,
};

enum ELightingMode
{
	LIGHTING_NULL    = 0,
	LIGHTING_FULL    = 1,
};
*/

const long ON_ACTOR_SKIN_ADD		= 1;
const long ON_ACTOR_SKIN_DEL		= 2;
const long ON_ACTOR_EFFECT_ADD		= 3;
const long ON_ACTOR_EFFECT_DEL		= 4;
const long ON_ACTOR_MATERIAL_CHANGE = 5;

class CRT_ActorInstance : public CRT_ObjectInst
{
	RT_DECLARE_DYNCREATE(CRT_ActorInstance, CRT_ObjectInst, NULL, "")
	CRT_ActorInstance();
	virtual ~CRT_ActorInstance();

	// from CRT_ObjectInst
	virtual void InstanceInit();
	virtual void InstanceExit();
	virtual void OnDataChange(DWORD p1, DWORD p2);

	//
	// runtime option
	//
	float           m_animSpeed;
	float           m_visible;
	RtgVertex3      m_scale;
	RtgVertex3      m_offset;

	float GetVisible() { return m_visible; }
	// float SetVisible(float visible) { m_visible = visible; }

	// scale
	void SetScale(RtgVertex3 scale);
	void SetOffset(RtgVertex3 offset);
	void SetVisible(float visible);
	bool IsScaled();
	RtgVertex3 GetFinialScale();

	//
	// finial option
	//
	float GetFinalVisible(); 
	float GetFinalAnimSpeed();

	// pose function
	SRT_Pose *GetPose(const char *name);
	SRT_Pose *GetCurrentPose();
	bool PlayPose(const char *name, bool loop=false, float speed=1.0f);
	bool PlayPoseInTime(const char *name, int mills, bool loop);
	void StopPose();
	void RegisterNotify(CRT_PoseNotify *notify);

	// anim function
	long  GetFrameNum() { return m_actor->m_frameNum; }
	void  UseFrame(float frame);
	float GetCurFrame() { return m_curFrame; }

	// render function
	void Render(RtgDevice *dev, RTGRenderMask mask, bool bEffect=true);
	void RenderShadow(RtgDevice *dev,float fShadowFadeFactor = 0.5f);
	void RenderFrame(RtgDevice *dev, RtgVertex3 &color, RtgVertex3 &scale);
	void RenderHighLight(RtgDevice *dev, RtgVertex3 &color, RtgVertex3 &scale);
	bool Create(RtgDevice *dev,RtgVertex3 scale=RtgVertex3(1,1,1));
	void OnMaterialChange();

  //  void OnMLoad(CRT_ObjectData* pData, EActorResType type);

    // skin 
	void LoadSkin(const char *file , bool breload = false );
	void UnloadSkin(const char *file);

	// effect
	bool ApplyEffect(const char *name);
	bool RemoveEffect(const char *name);

	RtgMatrix12 *GetSlotMatrix(const char *name);

	// for scene render
	bool ComputeVC(RtgMatrix16 &world, vector<DWORD> &vcList, RtgVertex3 vSkyLight, RtgLightItem* pLights);
	bool SetVC(vector<DWORD> &vcList);

	// bool IsEnableVC() { return m_core->m_bUseVC; }
	// bool IsEnableLight() { return m_core->m_bUseLight; }
	
	RtgAABB *GetBoundingBox();
	RtgAABB *GetStaticBoundingBox();

	RtgAABB *GetWorldBoundingBox();
	RtgAABB *GetWorldStaticBoundingBox();

	RtgAABB m_boundingBox;
	RtgAABB	m_staticBoundingBox;
	RtgAABB m_worldBoundingBox;
	RtgAABB	m_worldStaticBoundingBox;

	void  InitBoundingBox();
	void  UpdateBoundingBox();
	void  SetBaseColor(RtgVertex3 &color);

	TPoseMap		*m_poseMap;
	SRT_Pose		*m_curPose;
	CRT_PoseNotify	*m_notify;
	float            m_curFrame;
	float			 m_oldFrame;
	bool			 m_bLoop;
	CRT_Actor		*m_actor;
	long			 m_lastFrame;
	
	struct SSlot {
		string name;
		RtgMatrix12 mat;
	};
	vector<SSlot> m_slotVec;
	vector<CRT_SkinInstance*>   m_skinVec;
	vector<CRT_EffectInstance*> m_effectVec;

	// for link system
    static void PrintValidObject(const char* szFilename);
	static void Update(unsigned long delta);
	static CRT_ActorInstance *m_rootValid,*m_rootUnvalid;
	void SetValid(bool valid,bool force=false);

	RtgMatrix12 *GetLocalMatrix();
	RtgMatrix12 *GetWorldMatrix();
		
	bool LinkParent(CRT_ActorInstance *parent,const char *slot=NULL);
	void UnlinkParent();

	string m_parentSlot;
	RtgMatrix12 *m_parentSlotMat;

	CRT_ActorInstance *m_parent;
	CRT_ActorInstance *m_son;
	CRT_ActorInstance *m_next;
	CRT_ActorInstance *m_prev;
	RtgMatrix12 m_worldMat;		// actor world matrix,world matrix = local*parent
	RtgMatrix12 m_localMat;		// actor local matrix
	RtgMatrix12 m_finalMat;		// actor final matrix
	bool m_bValid;

	vector<RtgLight*> GetLightingList(); 
	RtgDevice *m_renderDev;

	void RealUseFrame(float frame ,float framestart = 0);
	bool IsLineInsertActor(RtgVertex3 vOrig,RtgVertex3 vDir,RtgDevice* dev,RtgVertex3 &v1,RtgVertex3 &v2,RtgVertex3 &v3);
private:
	bool CoreLinkParent(CRT_ActorInstance *parent,const char *slot);
	void CoreUnlinkParent();

	void ProcessPoseEvent(float oldFrame, float curFrame, SRT_Pose *curPose);
	void Tick(unsigned long delta);
};

/*
enum EActorQuality
{
	QUALITY_HIGH   = 0,
	QUALITY_MIDDLE = 1,
	QUALITY_LOW    = 2,
};
bool ActorInit();
bool ActorExit();
CRT_Pool *ActorPool();
void ActorRegisterPath(const char *path);
void ActorSetQuality(EActorQuality &actor,EActorQuality &skin,EActorQuality &mtl,
					 EActorQuality &effect);
void ActorGetQuality(EActorQuality &actor,EActorQuality &skin,EActorQuality &mtl,
					 EActorQuality &effect);

// type: actor,skin,mtllib,effect
CRT_PoolObject *ActorPrecache(const char *name,const char *type);
void ActorUncache(CRT_PoolObject *ob);

CRT_ActorInstance *LoadActorInstance(const char *name);
CRT_SkinInstance  *LoadSkinInstance(const char *name);
CRT_Effect		  *LoadEffect(const char *name);
inline void UnloadActorInstance(CRT_ActorInstance *instance)
{
	if(!instance) return;
	// LOG1("delete actor instance %s\n",instance->m_core->m_poTag.c_str());
	delete instance;
}

RtsSceneUtil *ActorGetScene();*/
class RtsSceneUtil;
void ActorSetScene(RtsSceneUtil *scene);


class CRT_EffectInstance : public CRT_ObjectInst
{
	RT_DECLARE_DYNAMIC(CRT_EffectInstance, CRT_ObjectInst, NULL, "")
	CRT_EffectInstance() {}
	virtual ~CRT_EffectInstance() {}

	// from CRT_ObjectInst
	virtual void InstanceInit() {
		CRT_ObjectInst::InstanceInit();
		m_effect = (CRT_Effect*)m_data;
	}
	virtual void InstanceExit() {}
	
	//
	// CRT_EffectInstance Interface
	//
	virtual bool RequestTick() = 0;
	virtual void Tick(unsigned long delta) = 0;
	virtual bool RequestUseFrame() = 0;
	virtual void UseFrame(unsigned long frame ,unsigned long framestart) = 0;

	virtual bool Init(RtgDevice *dev,CRT_ActorInstance *actor) = 0;
	virtual void Render(RtgDevice *dev,RTGRenderMask mask) = 0;

	virtual string& GetName() { return m_effect->m_name; }
	// virtual void SetName(const char *name) { m_effect->m_name = name; }
	CRT_Effect *m_effect;
};

class CRT_MtlInstance : public CRT_ObjectInst
{
	RT_DECLARE_DYNAMIC(CRT_MtlInstance, CRT_ObjectInst, NULL, "")
	CRT_MtlInstance() {
		m_baseColor.Set(0,0,0,0);
		m_visible = 1;
		m_active = false;
	}
	virtual ~CRT_MtlInstance() {}

	// from CRT_ObjectInst
	virtual void InstanceInit() {
		CRT_ObjectInst::InstanceInit();
		m_mtl = (CRT_Material*)m_data;
	}
	virtual void InstanceExit() {}

	//
	// CRT_MtlInstance Interface
	//
	virtual bool RequestTick() = 0;
	virtual void Tick(DWORD delta) = 0;
	virtual bool RequestUseFrame() = 0;
	virtual void UseFrame(DWORD frame) = 0;

	virtual bool Init(RtgDevice *dev, CRT_SkinInstance *skin, SRT_RenderSurface *rs) = 0;
	virtual void Render(RtgDevice *dev, RTGRenderMask mask) = 0;
	// virtual bool Exit() = 0;
	virtual bool IsTransparent() = 0;
	
	// spec light need update normal
	virtual bool RequestUpdateNormal() = 0;				
	virtual bool GetGeometry(int& alphaMode, int& texId, SRT_RenderSurface **rs,bool &shadow) = 0;
	
	// set the base color to simulate light
	// final color = base color  + current color
	// final pixel = final color * texture color
	void  SetBaseColor(RtgVertex4 &color) { m_baseColor = color; }
	void  GetBaseColor(RtgVertex4 &color) { color = m_baseColor; }
	
	void  SetVisible(float visible) { m_visible = visible; }
	float GetVisible() { return m_visible; }
	
	void  SetActive(bool active) { m_active = active; }
    bool  IsActive() { return m_active; } 

	string& GetName() { return m_mtl->m_name; }

	CRT_Material *m_mtl;
	RtgVertex4    m_baseColor;
	float         m_visible;
	bool          m_active;
	bool          m_bShadow;
};

class CRT_MtlLibInstance : public CRT_ObjectInst
{
	RT_DECLARE_DYNCREATE(CRT_MtlLibInstance, CRT_ObjectInst, NULL, "")
	CRT_MtlLibInstance();
	virtual ~CRT_MtlLibInstance();

	// from CRT_ObjectInst
	virtual void InstanceInit();
	virtual void InstanceExit();

    bool RequestTick();
	void Tick(unsigned long delta);
	bool RequestUseFrame();
	void UseFrame(unsigned long frame);
	
	bool RequestUpdateNormal();
	CRT_MtlInstance *GetMaterial(const char *name);
	CRT_MtlInstance *GetMaterial(int idx);
	
	bool Init(RtgDevice *dev, CRT_SkinInstance *skin);
	void Render(RtgDevice *dev, RTGRenderMask mask);
	void RenderShadow(RtgDevice *dev, float fShadowFadeFactor = 0.5f);

	vector<CRT_MtlInstance*> m_mtlVec;
	CRT_SkinInstance *m_skin;
	CRT_MaterialLib  *m_mtlLib;
	RtgDevice *m_renderDev;
};

#endif
