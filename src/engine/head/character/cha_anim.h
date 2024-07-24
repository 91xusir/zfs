
//////////////////////////////////////////////////////////////////////////
//
//   FileName : cha_anim.h
//     Author : zxy
// CreateTime : 2010.04.01
//       Desc : refactor
//
//////////////////////////////////////////////////////////////////////////

#ifndef __CHA_ANIM_H__
#define __CHA_ANIM_H__

void	RegAnimMemory(void);

class CRT_PoseNotify
{
public:
	CRT_PoseNotify() {}
	virtual ~CRT_PoseNotify() {}
	virtual void OnPoseBegin(SRT_Pose* pose) {}
	virtual void OnPoseEnd(SRT_Pose* pose) {}
	virtual void OnPoseEvent(SRT_Pose* pose, SRT_PoseEvent* event) {}
	virtual void OnPoseBreak(SRT_Pose* oldPose, SRT_Pose* newPose) {}
};

class CRT_SkinInstance : public RtObject
{
	RT_DECLARE_DYNCREATE(CRT_SkinInstance, RtObject, NULL, "")

		friend class CRT_VaSkin;
	friend class CRT_SkelSkin;

	CRT_SkinInstance();
	virtual ~CRT_SkinInstance();

	bool Create();
	void OnLoadFinish();
	void Reset();
	void Render(RtgDevice* dev, RTGRenderMask mask);
	void RenderShadow();
	void RenderHLight();
	void RenderFrame(const RtgVertex3& _color);
	void RenderNormal();
	void SetVisible(float visible);
	void UseFrame(float frame);
	void Tick(float deltaMill);
	bool ComputeVC(RtgMatrix16& world, vcArray_t& aryVc, int offset, RtgVertex3 vSkyLight, RtgLightItem* pLights);
	bool SetVC(vcArray_t& aryVc, int offset);
	void SetBaseColor(RtgVertex3& color);
	bool Save(RtArchive* ar);
	bool SaveAll();
	void ChangeMaterial(const char* name);
	void OnMaterialChange();
	void Output();
	void SetCoreObject(CRT_Skin* core);

	void ProcessComnandList();
	void ClearComnandList();

	bool ResourceReady() const
	{
		return GetState() == Object_State_Ready;
	}

	void AttachActor(CRT_ActorInstance* _actor)
	{
		m_actor = _actor;
	}

	CRT_MaterialLib* GetMaterialLib()
	{
		return m_mtlLib;
	}

	verArray_t* Get_VertexArray()
	{
		if (m_core->m_bStatic)
			return m_core->Get_VertexArray();
		return &m_aryVertex;
	}

	CRT_Skin* GetCore()
	{
		return m_core;
	}

	bool IsEnableDynShadow() const
	{
		if (!m_core)
			return false;
		return m_core->m_bDynShadow;
	}

	void EnableMip(bool bEnable = true)
	{
		m_bEnableMip = bEnable;
	}

protected:
	void SetBaseColorImpl(RtgVertex3& color);
	void SetVisibleImpl(float visible);
	void ChangeMaterialImpl(const char* name);

public:
	string           m_Name;

private:
	CRT_Skin* m_core;
	CRT_ActorInstance* m_actor;
	CRT_MaterialLib* m_mtlLib;
	bool             m_bHide;
	int              m_vcOffset;
	float            m_visible;
	float            m_frameVisible;
	float            m_oldFrameVisible;
	bool             m_updateNor;
	bool             m_bEnableMip;
	verArray_t       m_aryVertex;
	vcArray_t* m_aryVc;
	RtgBufferItem* m_pvc;
	float            m_fadeOut;
	list<void*>      m_listCommand;
};

class TagPoseNotify : public CRT_PoseNotify
{
public:
	virtual void OnPoseBegin(SRT_Pose* pose);
	virtual void OnPoseEnd(SRT_Pose* pose);
};

typedef enum _ActorNotifType
{
	Actor_Notify_Unknow = 0,
	Actor_Load_Finish,
	Actor_Create_Finish
} ActorNotifType;

typedef void (*CreateFinishNotify_t)(CRT_ActorInstance* _actor, ActorNotifType nttype, void* param0, void* param1);

struct ActorNotify
{
	ActorNotify()
		: param0(NULL),
		param1(NULL),
		notify(NULL)
	{
	}

	ActorNotify(CreateFinishNotify_t _notify, void* _param0, void* _param1)
		: param0(_param0),
		param1(_param1),
		notify(_notify)
	{
	}

	ActorNotify(const ActorNotify& _n)
		: param0(_n.param0),
		param1(_n.param1),
		notify(_n.notify)
	{
	}

	ActorNotify& operator = (const ActorNotify& _n)
	{
		notify = _n.notify;
		param0 = _n.param0;
		param1 = _n.param1;
		return *this;
	}

	void Notify(CRT_ActorInstance* _actor, ActorNotifType nttype) const
	{
		if (notify)
			notify(_actor, nttype, param0, param1);
	}

	void* param0;
	void* param1;
	CreateFinishNotify_t notify;
};

enum
{
	AIF_Light_Enable = (1 << 0),
	AIF_ZTest_Enable = (1 << 1),
	AIF_Vc_Enable = (1 << 2)
};


class CRT_ActorInstance : public RtObject
{
	RT_DECLARE_DYNCREATE(CRT_ActorInstance, RtObject, NULL, "")


		typedef vector<CRT_ActorInstance*> childArray_t;
	typedef vector<SRT_Bone> boneArray_t;

	friend class RtcAcotrManager;
	friend bool ActorInit();
	friend bool ActorExit();

public:

	// 创建对象的实例
	bool Create();
	// 销毁对象的实例
	void Destroy();
	// 当加载完成时的处理函数
	void OnLoadFinish();
	// 重置对象状态
	void Reset();
	// 链接当前对象到指定的父对象和插槽
	bool CoreLinkParent(CRT_ActorInstance* parent, const char* slot);
	// 解除当前对象的父对象
	void CoreUnlinkParent();
	// 设置当前对象的父对象
	bool LinkParent(CRT_ActorInstance* parent, const char* slot = NULL);
	// 解除当前对象的父对象
	void UnlinkParent();
	// 设置对象的可见性
	void SetVisible(float visible);
	void SetSkinVisible(float visible);
	// 改变对象的材质
	void ChangeMaterial(size_t _skinIndex, const char _mtlName[]);
	// 重置对象的 Ribbon 效果
	void ResetRibbonEffect();
	// 处理命令列表中的命令
	void ProcessComnandList();
	// 清空命令列表
	void ClearComnandList();

	bool ImportActor(const char* name);
	void LoadSkin(const char* name, bool bdisableMip = false);
	void UnloadSkin(const char* name);
	bool ApplyEffect(const char* name);
	bool RemoveEffect(const char* name);
	void UnloadAllSkin();
	// 更新对象的状态和子对象（如果需要）每帧一次
	void Tick(float deltaMill, bool bUpdateChild = true);
	void RealUseFrame(float frame);
	bool UpdateBone(float frame);

	void Render(RtgDevice* dev, RTGRenderMask mask, bool bEffect = true, bool bRenderchild = false);
	void RenderShadow(RtgDevice* dev, bool bRenderchild = true, float fShadowFadeFactor = 0.5f);
	void RenderFrame(RtgDevice* dev, RtgVertex3& color, RtgVertex3& scale);
	void RenderHighLight(RtgDevice* dev, RtgVertex3& color, RtgVertex3& scale);
	void RenderBone(RtgDevice* dev, RTGRenderMask mask);
	void RenderNormal();

	bool PlayPose(const char* name, bool loop = false, float speed = 1.0f);
	bool PlayPoseInTime(const char* name, int mills, bool loop);
	void ProcessPoseEvent(float oldFrame, float curFrame, SRT_Pose* curPose);
	bool PlayPose(bool loop = false, float speed = 1.0f);

	bool ComputeVC(RtgMatrix16& world, vcArray_t& aryVc, RtgVertex3 vSkyLight, RtgLightItem* pLights);
	bool SetVC(vcArray_t& aryVc);
	void SetBaseColor(RtgVertex3& color, bool bSetchild = false);

	RtgAABB* UpdateBoundingBox();
	void SetDrawScale(RtgVertex3 scale);
	bool Save(RtArchive* ar);
	bool IsRegIntersect(const RtgVertex3& reg0, const RtgVertex3& reg1);

	bool ResourceReady() const
	{
		return GetState() == Object_State_Ready;
	}

	CRT_Actor* GetCore()
	{
		return m_core;
	}

	void OnMaterialChange()
	{
		for (size_t i = 0; i < m_skinList.size(); ++i)
			m_skinList[i]->OnMaterialChange();
	}
	//lyymark act对象帧
	void UseFrame(float frame)
	{
		m_curFrame = frame;
	}

	RtgAABB* GetBoundingBox()
	{
		return &m_boundingBox;
	}

	RtgAABB* GetStaticBoundingBox()
	{
		return &m_staticBoundingBox;
	}

	void SetCoreObject(CRT_Actor* core)
	{
		m_core = core;
	}

	void SetTagPoseNotify()
	{
		RegisterNotify(&m_tagPoseNotify);
	}

	long GetFrameNum()
	{
		if (!m_core)
			return 0;
		return m_core->m_frameNum;
	}

	float GetCurFrame()
	{
		return m_curFrame;
	}

	void SetOffset(RtgVertex3 offset)
	{
		m_offset = offset;
	}

	bool GetBoneMat(RtgMatrix12* _Mat, const char _Name[])
	{
		RTASSERT(_Mat);
		size_t boneIndex = 0;

		if (GetState() != Object_State_Ready || (boneIndex = m_core->GetBoneIndex(_Name)) == Error_BoneIndex)
		{
			(*_Mat) = RtgMatrix12::matIdentity;
			return false;
		}

		(*_Mat) = m_bones[boneIndex].skinMat;

		return true;

	}

	RtgMatrix12* GetBoneMat(size_t boneIndex)
	{
		if (boneIndex >= m_bones.size())
			return NULL;
		return &m_bones[boneIndex].skinMat;
	}

	RtgMatrix12* GetBoneNormalMat(size_t boneIndex)
	{
		if (boneIndex >= m_bones.size())
			return NULL;
		return &m_bones[boneIndex].normMat;
	}

	bool IsEnableVC()
	{
		if (!m_core)
			return false;
		return m_core->m_bUseVC;
	}

	bool IsEnableLight()
	{
		if (!m_core)
			return false;
		return m_core->m_bUseLight;
	}

	void UpdateMatrix()
	{
		RtgMatrix12 _parentMat;
		// 检查是否有父对象
		if (m_parent)
		{        // 如果有父对象，尝试获取父对象插槽信息m_parentSlot矩阵

			if (m_parent->GetBoneMat(&_parentMat, m_parentSlot.c_str()))
				// 如果成功获取到父对象的插槽矩阵，则按以下顺序更新当前对象的矩阵：
		 // 1. 当前对象的本地矩阵 `m_localMat`
		 // 2. 父对象link点插槽骨骼矩阵 `_parentMat`
		 // 3. 父对象的矩阵 `m_parent->m_matrix`
				m_matrix = m_localMat * _parentMat * (m_parent->m_matrix);
			else
				// 如果无法获取到父对象的link点插槽矩阵，则仅按以下顺序更新当前对象的矩阵：
		  // 1. 当前对象的本地矩阵 `m_localMat`
		  // 2. 父对象的矩阵 `m_parent->m_matrix`
				m_matrix = m_localMat * (m_parent->m_matrix);
		}
		else
		{
			// 如果没有父对象，矩阵仅由当前对象的本地矩阵 `m_localMat` 组成
			m_matrix = m_localMat;
		}
		// 更新当前对象的世界包围盒
		Update_WorldBoundBox();
		// 递归更新所有子对象的矩阵
		for (size_t i = 0; i < m_childs.size(); ++i)
			m_childs[i]->UpdateMatrix();
	}

	template<typename _mtype>
	void SetMatrix(const _mtype& _mat)
	{
		CopyMatrix(&m_localMat, &_mat);
		UpdateMatrix();
	}

	void SetMatrix_Row(int _row, const RtgVertex3& _v3)
	{
		RTASSERT(_row < 4);
		m_localMat.SetRow(_row, _v3);
		UpdateMatrix();
	}

	void SetMatrix_Col(int _col, const RtgVertex3& _v3)
	{
		RTASSERT(_col < 4);
		m_localMat.SetCol(_col, _v3);
		UpdateMatrix();
	}

	const RtgMatrix12* GetMatrix()
	{
		return &m_localMat;
	}

	template<typename _mtype>
	void SetWorldMatrix(const _mtype& _mat)
	{
		CopyMatrix(&m_matrix, &_mat);
		Update_WorldBoundBox();

		for (size_t i = 0; i < m_childs.size(); ++i)
			m_childs[i]->UpdateMatrix();
	}

	const RtgMatrix12* GetWorldMatrix()
	{
		return &m_matrix;
	}

	void ShowBone(bool bShow)
	{
		if (!m_core)
			return;
		for (vector<SRT_Bone>::iterator i = m_bones.begin();
			i != m_bones.end(); ++i)
			i->isPaint = bShow;
	}

	void StopPose()
	{
		if (m_curPose.IsVaild())
			m_curPose.UnActive();
		m_curPose.Invalid();
	}

	void RegisterNotify(CRT_PoseNotify* notify)
	{
		m_notify = notify;
	}

	TPoseMap* GetPoseMap()
	{
		if (!m_core)
			return NULL;
		return &m_core->m_poseMap;
	}

	SRT_Pose* GetPose(const char* name)
	{
		if (!m_core)
			return NULL;
		return m_core->GetPose(name);
	}

	const SRT_Pose& GetCurrentPose()
	{
		return m_curPose;
	}

	bool IsPlayingPose() const
	{
		return m_curPose.IsVaild();
	}

	float GetVisible()
	{
		return m_visible;
	}

	bool IsEnableLight() const
	{
		if (HasAttrib(AIF_Light_Enable))
			return m_bUseLight;
		return m_core->m_bUseLight;
	}

	bool IsEnableZText() const
	{
		if (HasAttrib(AIF_ZTest_Enable))
			return m_bZTest;
		return m_core->m_bZTest;
	}

	bool IsEnableVc() const
	{
		if (HasAttrib(AIF_Vc_Enable))
			return m_bUseVC;
		return m_core->m_bUseVC;
	}

	bool HasAttrib(DWORD _attrib) const
	{
		return !!(m_Flags & _attrib);
	}

	void Enable(DWORD _attrib)
	{
		if (_attrib & AIF_Light_Enable)
			m_bUseLight = true;
		if (_attrib & AIF_ZTest_Enable)
			m_bUseVC = true;
		if (_attrib & AIF_Vc_Enable)
			m_bZTest = true;
		m_Flags |= _attrib;
	}

	void Disable(DWORD _attrib)
	{
		if (_attrib & AIF_Light_Enable)
			m_bUseLight = false;
		if (_attrib & AIF_ZTest_Enable)
			m_bUseVC = false;
		if (_attrib & AIF_Vc_Enable)
			m_bZTest = false;
		m_Flags |= _attrib;
	}

	void Register_ActorNotify(CreateFinishNotify_t _notify, void* _param0, void* _param1)
	{
		RTASSERT(!m_Notify.notify);
		m_Notify.notify = _notify;
		m_Notify.param0 = _param0;
		m_Notify.param1 = _param1;
	}

	void Register_ActorNotify(const ActorNotify& _Notify)
	{
		RTASSERT(!m_Notify.notify);
		m_Notify = _Notify;
	}

	void Unregister_ActorNotify()
	{
		m_Notify.notify = NULL;
	}

	bool IsEnableDynShadow() const
	{
		if (m_core)
			return m_core->m_bDynamicShadow;
		return false;
	}

	void AddEffect(CRT_Effect* _effect)
	{
		m_effectList.push_back(_effect);
	}

	void OnCreateFinish()
	{
	}

	void Update_WorldBoundBox()
	{
		RtgVertex3 _min = m_boundingBox.vPos - m_boundingBox.vExt;
		RtgVertex3 _max = m_boundingBox.vPos + m_boundingBox.vExt;

		RtgVertex3 _box[] = {
			RtgVertex3(_min.x, _min.y, _min.z),
			RtgVertex3(_max.x, _min.y, _min.z),
			RtgVertex3(_max.x, _max.y, _min.z),
			RtgVertex3(_min.x, _max.y, _min.z),
			RtgVertex3(_min.x, _min.y, _max.z),
			RtgVertex3(_max.x, _min.y, _max.z),
			RtgVertex3(_max.x, _max.y, _max.z),
			RtgVertex3(_min.x, _max.y, _max.z),
		};

		_min.Set(+1e10);
		_max.Set(-1e10);

		for (size_t i = 0; i < sizeof(_box) / sizeof(RtgVertex3); ++i)
		{
			RtgVertex3 _v = _box[i] * m_matrix;

			if (_v.x < _min.x)
				_min.x = _v.x;
			if (_v.x > _max.x)
				_max.x = _v.x;
			if (_v.y < _min.y)
				_min.y = _v.y;
			if (_v.y > _max.y)
				_max.y = _v.y;
			if (_v.z < _min.z)
				_min.z = _v.z;
			if (_v.z > _max.z)
				_max.z = _v.z;
		}

		m_aabboxWorld.vPos.Set((_min.x + _max.x) * 0.5f, (_min.y + _max.y) * 0.5f, (_min.z + _max.z) * 0.5f);
		m_aabboxWorld.vExt.Set((_max.x - _min.x) * 0.5f, (_max.y - _min.y) * 0.5f, (_max.z - _min.z) * 0.5f);
	}

	const RtgAABB* Get_WorldBoundBox() const
	{
		return &m_aabboxWorld;
	}

	void AddChild(CRT_ActorInstance* _act)
	{
		m_childs.push_back(_act);
	}

	void DelChild(CRT_ActorInstance* _act)
	{
		size_t _childNum = m_childs.size();
		size_t i = 0;

		while (i < _childNum)
		{
			if (m_childs[i] == _act)
				break;
			++i;
		}

		if (i < _childNum)
		{
			swap(m_childs[i], m_childs[_childNum - 1]);
			m_childs.pop_back();
		}
		else
		{
			RtCoreLog().Warn("delete error child actor\n");
		}
	}

	void RotateLX(float degree)
	{
		m_localMat.RotateLX(degree);
		UpdateMatrix();
		Update_WorldBoundBox();
	}

	void RotateLY(float degree)
	{
		m_localMat.RotateLY(degree);
		UpdateMatrix();
		Update_WorldBoundBox();
	}

	void RotateLZ(float degree)
	{
		m_localMat.RotateLZ(degree);
		UpdateMatrix();
		Update_WorldBoundBox();
	}

	void RotateX(float degree)
	{
		m_localMat.RotateX(degree);
		UpdateMatrix();
		Update_WorldBoundBox();
	}

	void RotateY(float degree)
	{
		m_localMat.RotateY(degree);
		UpdateMatrix();
		Update_WorldBoundBox();
	}

	void RotateZ(float degree)
	{
		m_localMat.RotateZ(degree);
		UpdateMatrix();
		Update_WorldBoundBox();
	}

	void TranslateX(const float Distance)
	{
		m_localMat.TranslateX(Distance);
		UpdateMatrix();
		Update_WorldBoundBox();
	}

	void TranslateY(const float Distance)
	{
		m_localMat.TranslateY(Distance);
		UpdateMatrix();
		Update_WorldBoundBox();
	}

	void TranslateZ(const float Distance)
	{
		m_localMat.TranslateZ(Distance);
		UpdateMatrix();
		Update_WorldBoundBox();
	}

	void Translate(float x, float y, float z)
	{
		m_localMat.Translate(x, y, z);
		UpdateMatrix();
		Update_WorldBoundBox();
	}

	void Enable(bool bDisplay, bool bUpdate)
	{
		m_bDisplay = bDisplay;
		m_bUpdate = bUpdate;
	}

	bool IsDisplay()
	{
		return m_bDisplay;
	}

private:
	CRT_ActorInstance();
	virtual ~CRT_ActorInstance();

protected:
	bool PlayPoseImpl(const char name[], bool loop, float speed, DWORD _mode);
	bool PlayPoseImpl(SRT_Pose* pose, bool loop, float speed, DWORD _mode);
	bool SetVCImpl(vcArray_t* _vc);
	bool CoreLinkParentImpl(CRT_ActorInstance* parent, const char* slot);
	void CoreUnlinkParentImpl();
	void LoadSkinImpl(const char* name, bool bdisableMip);
	void UnloadSkinImpl(const char* name);
	bool ApplyEffectImpl(const char* name);
	bool RemoveEffectImpl(const char* name);
	void SetDrawScaleImpl(RtgVertex3 scale);
	void ChangeMaterialImpl(size_t _skinIndex, const char _mtlName[]);
	void SetVisibleImpl(float visible);
	void SetBaseColorImpl(RtgVertex3& color, bool bSetchild = false);

public:
	static bool m_bGlobalRenderActor;
	static bool m_bGlobalRenderShadow;

private:
	CRT_Actor* m_core;

public:

	SRT_Pose         m_curPose;
	SRT_Pose         m_prePose;
	CRT_PoseNotify* m_notify;
	float            m_curFrame;
	float			 m_oldFrame;
	bool			 m_bLoop;
	long			 m_lastFrame;
	RtgAABB          m_aabboxWorld;
	RtgAABB			 m_boundingBox;
	RtgAABB			 m_staticBoundingBox;
	std::string      m_strHighLightBone;
	bool             m_bRenderByParent;
	TagPoseNotify    m_tagPoseNotify;
	bool             m_bDisplay;
	bool             m_bUpdate;
	float            m_animSpeed;
	string           m_parentSlot;
	RtgMatrix12      m_matrix;
	RtgMatrix12      m_localMat;
	bool             m_bValid;
	bool             m_bEnableShadow;
	float            m_visible;
	bool             m_bDrawScale;
	RtgVertex3       m_drawScale;
	RtgVertex3       m_offset;
	bool             m_bRenderEffect;
	ActorNotify      m_Notify;
	bool             m_bUseLight;
	bool             m_bUseVC;
	bool             m_bZTest;
	DWORD            m_Flags;
	string           m_Name;
	CRT_ActorInstance* m_parent;
	childArray_t     m_childs;
	boneArray_t      m_bones;
	UINT             m_lastTickFrame;

public:
	vector<CRT_SkinInstance*> m_skinList;
	vector<CRT_Effect*>       m_effectList;
	list<void*>               m_listCommand;
	int                       m_linkCount;

public:

#ifdef CHAR_VIEWER
	void AttachPoseGrid(void* p);
	void AttachLinkBoxGrid(void* p);
	bool OnPoseGridNotify(WPARAM wParam, LPARAM lParam);
	bool OnLinkBoxGridNotify(WPARAM wParam, LPARAM lParam);
	string GetSelectedPose(void* p);

	void AttachEffectGrid(void* p);
	bool OnEffectGridNotify(WPARAM wParam, LPARAM lParam);
	CRT_Effect* GetSelectedEffect(void* p);

	void AttachGrid(void* p);
	bool OnGridNotify(WPARAM wParam, LPARAM lParam);
	CRT_Material* GetSelectedMaterial(void* p);
	CRT_Skin* GetSelectedSkin(void* p);
	bool UpdateSkin(RtgDevice* dev);
	bool UpdateEffect(RtgDevice* dev);
#endif

};

enum EActorQuality
{
	QUALITY_HIGH = 0,
	QUALITY_MIDDLE = 1,
	QUALITY_LOW = 2,
};

bool ActorInit();
bool ActorExit();

RtcAcotrManager* RtcGetActorManager();

void ActorSetQuality(EActorQuality& actor, EActorQuality& skin, EActorQuality& mtl,
	EActorQuality& effect);
void ActorGetQuality(EActorQuality& actor, EActorQuality& skin, EActorQuality& mtl,
	EActorQuality& effect);

class RtsSceneUtil;
RtsSceneUtil* ActorGetScene();
void ActorSetScene(RtsSceneUtil* scene);

#endif
