#ifndef __CHA_EFFECT_H__
#define __CHA_EFFECT_H__

class CRT_EftInstEmitter : public CRT_EffectInstance
{
	RT_DECLARE_DYNCREATE(CRT_EftInstEmitter,CRT_EffectInstance,NULL,"");
	CRT_EftInstEmitter();
	virtual ~CRT_EftInstEmitter();

	// from CRT_ObjectInst
	virtual void InstanceInit();
	virtual void InstanceExit();
	virtual void OnDataChange(DWORD p1, DWORD p2);

	// from CRT_EffectInstance
	virtual bool RequestTick();
	virtual void Tick(unsigned long delta);
	virtual bool RequestUseFrame();
	virtual void UseFrame(unsigned long frame,unsigned long framestart = 0);

	virtual bool Init(RtgDevice *dev,CRT_ActorInstance *actor);
	virtual void Render(RtgDevice *dev,RTGRenderMask mask);

	struct SParticle
	{	
		void Init() 
		{
			life = 0;
			texIdx = 1;
			pos.Zero();
			speed.Zero();
			dirSpeed.Zero();
			acc.Zero();
			size.Set(5,5,5);
			opaque = 0;
			rotate.Set(0,0,0);
			scale.Set(1,1,1);
			color.Set(1.0,1.0,1.0);
			next = NULL;
			model = NULL;
		}
		long  life;
		RtgVertex3 speed;
		RtgVertex3 dirSpeed;
		RtgVertex3 acc;
		RtgVertex3 pos;
		RtgVertex3 size;
		float opaque;
		RtgVertex3 scale;
		RtgVertex3 color;
		RtgVertex3 rotate;
		long  texIdx;
		SParticle *next;
		CRT_ActorInstance *model;
	};

	void DelParticle(SParticle *p);
	SParticle *NewParticle();
	void Force(CRT_ForceField *ff, SParticle *p, long delta);
	
	void RenderModel(SParticle *p, RtgMatrix16 &world);
	void BeginRenderBoard(DWORD &light, DWORD &op);
	void EndRenderBoard(DWORD light, DWORD op);

	// com type
	RtgMatrix12 *m_destMat;
	RtgMatrix12 *m_parentMat;

	// runtime var
	long         m_delta;
	RtgShader    m_ro;
	SParticle	*m_pActive;
	SParticle	*m_pDead;
	long         m_lastFrame;
	bool         m_bEnable;
	float        m_emiSpeed;
	long         m_squirtNum;

	RtgDevice			*m_renderDev;
	CRT_ActorInstance	*m_actor;
	CRT_EffectEmitter   *m_eftEmitter;
	CRT_ActorInstance   *m_modelClone;
};

class CRT_EftInstRibbon : public CRT_EffectInstance
{
	RT_DECLARE_DYNCREATE(CRT_EftInstRibbon,CRT_EffectInstance,NULL,"");
	CRT_EftInstRibbon();
	virtual ~CRT_EftInstRibbon();

	// from CRT_ObjectInst
	virtual void InstanceInit();
	virtual void InstanceExit();
	virtual void OnDataChange(DWORD p1, DWORD p2);

	// from CRT_EffectInstance
	virtual bool RequestTick() { return true; }
	virtual void Tick(unsigned long delta);
	virtual bool RequestUseFrame()  { return m_eftRibbon->m_keyList.Size()!=0; }
	virtual void UseFrame(unsigned long frame,unsigned long framestart=0);

	virtual bool Init(RtgDevice *dev,CRT_ActorInstance *actor);
	virtual void Render(RtgDevice *dev,RTGRenderMask mask);
	CRT_EffectRibbon *m_eftRibbon;
	RtgDevice *m_renderDev;

	struct SPiece
	{	
		SRibbonKey keyFrame;
		int begFrame;
		int endFrame;
		
		int subTex;
		RtgVertex3 color;
		RtgVertex3 colorBeg;
		RtgVertex3 colorMid;
		RtgVertex3 colorEnd;
		float mid;

		float visible;
		// long  alphaMode;
		
		void Init() 
		{

		}
		vector<RtgVertex3> verList;
		vector<float> uvList;
	};

	// need fix
	void GenTri(int frame,SPiece *p);

	void DelPiece(SPiece *p) { /*delete p; */ DEL_ONE(p);}
	SPiece *NewPiece() { SPiece *p = RT_NEW SPiece; p->Init(); return p;}
	list<SPiece*> m_list;
	
	RtgShader  m_ro;

	CRT_ActorInstance *m_actor;
	SRT_Bone  *m_headBone;
	SRT_Bone  *m_tailBone;

	// runtime var
	int m_lastFrame;
	int m_ribbonPerFrame;
	int m_texId1;
	int m_texId2;
};

class CRT_EftInstBlur: public CRT_EffectInstance
{
	RT_DECLARE_DYNCREATE(CRT_EftInstBlur,CRT_EffectInstance,NULL,"");
	CRT_EftInstBlur();
	virtual ~CRT_EftInstBlur();

	// from CRT_ObjectInst
	virtual void InstanceInit();
	virtual void InstanceExit();
	virtual void OnDataChange(DWORD p1, DWORD p2);

	// from CRT_EffectInstance
	virtual bool RequestTick() { return true; }
	virtual void Tick(unsigned long delta);
	virtual bool RequestUseFrame()  { return m_eftBlur->m_keyVec.Size()!=0; }
	virtual void UseFrame(unsigned long frame,unsigned long framestart = 0);

	virtual bool Init(RtgDevice *dev,CRT_ActorInstance *actor);
	virtual void Render(RtgDevice *dev,RTGRenderMask mask);
	CRT_EffectBlur *m_eftBlur;
	RtgDevice *m_renderDev;

	struct SFrame
	{
		DWORD       begTime;
		DWORD       time;
		RtgMatrix16 matrix;
		DWORD       frame;
	};

	void Update(SBlurKey &key);

	// runtime var
	long m_delta;
	long m_frameDelta;
	bool m_bEnable;
	CRT_ActorInstance *m_actor;
	CRT_ActorInstance *m_blurAct;
	list<SFrame> m_frameList;
};

class CRT_EftInstProjector : public CRT_EffectInstance
{
	RT_DECLARE_DYNCREATE(CRT_EftInstProjector,CRT_EffectInstance,NULL,"");
	CRT_EftInstProjector();
	virtual ~CRT_EftInstProjector();

	// from CRT_ObjectInst
	virtual void InstanceInit();
	virtual void InstanceExit();
	virtual void OnDataChange(DWORD p1, DWORD p2);

	// from CRT_EffectInstance
	virtual bool RequestTick() { return true; }
	virtual void Tick(unsigned long delta);
	virtual bool RequestUseFrame()  { return m_eftProjector->m_keyVec.Size()!=0; }
	virtual void UseFrame(unsigned long frame,unsigned long framestart  = 0);

	virtual bool Init(RtgDevice *dev,CRT_ActorInstance *actor);
	virtual void Render(RtgDevice *dev,RTGRenderMask mask);

	void DrawHelperObject(RtgDevice *dev);
	void RenderProjectActor(RtgDevice *dev, CRT_ActorInstance *actor);
	CRT_EffectProjector *m_eftProjector;
	RtgDevice *m_renderDev;

	void Update(SProjectorKey &key);
	CRT_ActorInstance *m_actor;
	long m_delta;
	RtgCamera     m_camera;
	RtgMatrix12  *m_matrix;
	SProjectorKey m_key;
	RtgShader     m_ro;
	int           m_stageFadeOut;
	int           m_stageGeo;
	int           m_stageProject;

	bool UploadVB(RtgDevice *dev);
	bool UploadIB(RtgDevice *dev);
	vector<RtgVertexVNT> m_verVec;
	vector<short>        m_idxVec;
};
#endif
