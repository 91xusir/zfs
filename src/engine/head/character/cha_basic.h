
//////////////////////////////////////////////////////////////////////////
//
//   FileName : cha_basic.h
//     Author : zxy
// CreateTime : 2010.04.01
//       Desc : refactor
//
//////////////////////////////////////////////////////////////////////////

#ifndef __CHA_BASIC_H__
#define __CHA_BASIC_H__

#pragma warning(disable : 4018)
#pragma warning(disable : 4267)

class CRT_ActorInstance;
class CRT_SkinInstance;
class CRT_PoolObject;
class CRT_Material;
class CRT_MaterialLib;
class CRT_Effect;
class CRT_Skin;
class CRT_VaSkin;
class CRT_SkelSkin;
class CRT_Actor;

template<typename _T>
RtArchive& operator<<(RtArchive &ar, farray<_T>& _ary)
{
    long num = 0;

    if (ar.IsLoading())
    {
        ar<<num;
        _ary.alloc(num);
        ar.Serialize(_ary.pointer(), _ary.size() * sizeof(_T));
    }
    else
    {
        num = (long)_ary.size();
        ar<<num;
        ar.Serialize(_ary.pointer(), _ary.size() * sizeof(_T));
    }

    return ar;
}

template<class T>
RtArchive& operator<< (RtArchive& Ar, vector<T>& v)
{
    long num = 0;

    if (Ar.IsLoading())
    {
        Ar<<num;
        v.resize(num);
        for (int i = 0; i < num; ++i)
            Ar<<v[i];
    }
    else
    {
        num = (long)v.size();
        Ar<<num;
        for (int i = 0; i < num; ++i)
            Ar<<v[i];
    }

    return Ar;
};


template<class T1,class T2>
RtArchive& operator<<(RtArchive& Ar, EXT_SPACE::unordered_map<T1,T2>& h)
{
	long i,num = 0;

	if(Ar.IsLoading())
	{
		Ar << num;
		for( i=0; i<num; i++)
		{
			T1 t1;
			T2 t2;
			Ar << t1 << t2;
			h[t1] = t2;
		}
	}
	else
	{
		num = (long)h.size();
		Ar << num;
		EXT_SPACE::unordered_map<T1,T2>::iterator it;
		for( it=h.begin(); it!=h.end(); it++)
		{
			T1 t1;
			T2 t2;
			t1 = (*it).first;
			t2 = (*it).second;
			Ar << t1 << t2;
		}
	}
	return Ar;
};

#define BEGIN_VERSION_CONVERT(v) char __end = v;\
                                 Ar << __end;\
                                 if(!__end)\
                                 {
#define END_VERSION_CONVERT   }

class RtcSkinSubset
{
public :

    RtcSkinSubset()
        : m_pib(NULL),
         _Id(0)
    {
    }

    ~RtcSkinSubset()
    {
    }

    bool Create();
    void Destroy();

    RtgBufferItem* RequestIB()
    {
        return m_pib;
    }

    friend RtArchive& operator<< (RtArchive &Ar, RtcSkinSubset& subset) 
    {
        Ar<<subset.Material<<subset.Texture<<subset.faceList;
        BEGIN_VERSION_CONVERT(1)
        END_VERSION_CONVERT
        return Ar;
    }

public :

    string Material;
    string Texture;
	farray<short> faceList;
    RtgBufferItem* m_pib;
    int _Id;

};

struct SRT_PoseEvent
{
	string	Action;		
	string	Param;
    string	Param1;
    string	Param2;
	long	KeyFrame;
    int     nTagActIndex;

    SRT_PoseEvent()
        : KeyFrame(0),
          nTagActIndex(-1)
    {

    }

	friend RtArchive& operator<< (RtArchive& Ar, SRT_PoseEvent& pe) 
	{
		Ar<<pe.Action<<pe.Param<<pe.KeyFrame;

        if (pe.Action == "animation")
        {
            Ar<<pe.Param1<<pe.Param2;
        }

		BEGIN_VERSION_CONVERT(1)
		END_VERSION_CONVERT

		return Ar;
	}
};


struct SRT_Pose
{
	string	Name;
	long	StartFrm;
	long	EndFrm;
	vector<SRT_PoseEvent> EventList;
    vector<CRT_ActorInstance*> vecTagActs;
    CRT_ActorInstance* pParentAct;

    SRT_Pose()
        : pParentAct(NULL)
    {

    }

    SRT_Pose(const SRT_Pose& _Pose)
    {
        Name = _Pose.Name;
        StartFrm = _Pose.StartFrm;
        EndFrm = _Pose.EndFrm;
        EventList = _Pose.EventList;
        pParentAct = NULL;
    }

    ~SRT_Pose()
    {
        UnActive();
        
    }

    void Active(CRT_ActorInstance* _ParentAct);
    void UnActive();

    bool ResourceReady() const;

    bool IsVaild() const
    {
        return StartFrm <= EndFrm;
    }

    void Invalid()
    {
        Name.clear();
        StartFrm = 1;
        EndFrm = 0;
    }

    void InitFrom(const SRT_Pose* _pose)
    {
        Name = _pose->Name;
        StartFrm = _pose->StartFrm;
        EndFrm = _pose->EndFrm;
        EventList = _pose->EventList;
        vecTagActs.clear();
        pParentAct = NULL;
    }

    SRT_Pose& operator = (const SRT_Pose& _Pose)
    {
        Name = _Pose.Name;
        StartFrm = _Pose.StartFrm;
        EndFrm = _Pose.EndFrm;
        EventList = _Pose.EventList;
        pParentAct = _Pose.pParentAct;
        return *this;
    }

	friend RtArchive& operator<<(RtArchive &Ar, SRT_Pose &p) 
	{
		Ar << p.Name << p.StartFrm << p.EndFrm << p.EventList;
		BEGIN_VERSION_CONVERT(1)
		END_VERSION_CONVERT
		return Ar;
	}
};

struct SRT_VaVtxPos
{
	short x,y,z;

	void SetValue(RtgVertex3 &v)
	{
		x = v.x * 100; y = v.y * 100; z = v.z * 100;
	}

	void GetValue(RtgVertex3 &v)
	{
		v.x = (float)x/100; v.y = (float)y/100; v.z = (float)z/100;
	}	
	
    friend RtArchive& operator<<(RtArchive &Ar, SRT_VaVtxPos &v) 
	{
		Ar << v.x << v.y << v.z;
		return Ar;
	}
};

struct SRT_VaVtxNor
{
	union
	{ 
		struct {long x:11;long y:11;long z:10;};
		long xyz;
	};

	void SetValue(RtgVertex3 &v)
	{
		x = v.x * 1000; y = v.y * 1000; z = v.z * 500;
	}

	void GetValue(RtgVertex3 &v)
	{
		v.x = (float)x/1000; v.y = (float)y/1000; v.z = (float)z/500;
	}	

	friend RtArchive& operator<<(RtArchive &Ar, SRT_VaVtxNor &v) 
	{
		return Ar << v.xyz;
	}
};

struct SRT_VaVtxUV
{
	unsigned char UV[2];

	void SetValue(RtgVertex2 &u)
	{
		UV[0] = u.x*255; UV[1] = u.y*255;
	}

	void GetValue(RtgVertex2 &u)
	{
		u.x = UV[0]/255.0f; u.y = UV[1]/255.0f;
	}

	friend RtArchive& operator<<(RtArchive &Ar, SRT_VaVtxUV &v) 
	{
		return Ar << v.UV[0] << v.UV[1];
	}
};

struct SRT_VaFrame
{
	farray<SRT_VaVtxPos> verList;
	farray<SRT_VaVtxNor> norList;
	farray<SRT_VaVtxUV>  uvList;
	float visible;

	friend RtArchive& operator<<(RtArchive &Ar, SRT_VaFrame &f) 
	{
		Ar << f.verList << f.norList << f.uvList << f.visible;
		BEGIN_VERSION_CONVERT(1)
		END_VERSION_CONVERT
		return Ar;
	}
};

struct SRT_StaticVtx
{
	RtgVertex3 pos;
	RtgVertex3 nor;
	RtgVertex2 uv;

    enum { fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 };

	friend RtArchive& operator<<(RtArchive &Ar, SRT_StaticVtx &v) 
	{
		return Ar << v.pos << v.nor << v.uv;
	}
};

typedef farray<SRT_StaticVtx> verArray_t;
typedef farray<DWORD>         vcArray_t;

struct SRibbonFrame
{
	short segNum;
	farray<SRT_VaVtxPos> pos;		// pos num = segNum + 1
	
	friend RtArchive& operator<<(RtArchive &Ar, SRibbonFrame &v) 
	{
		return Ar << v.segNum << v.pos;
	}
};

struct SRT_Bone 
{
    string              Name;
	string              ParentName;
	farray<RtgVertex3>  aryTrans;
	farray<RtgVertex4>  aryRotas;
	bool                AbsMode;
	bool                ExportTranslate;
	bool                isPaint;
    short               maxRibbonDis;
    vector<SRibbonFrame> ribbonFrame;
    vector<size_t>      childList;
    RtgMatrix12         skinMat;
    RtgMatrix12         normMat;

	SRT_Bone()
        : isPaint(false)
    {
    }

    SRT_Bone(const SRT_Bone& _Bone)
    {
        Name        = _Bone.Name;
        ParentName  = _Bone.ParentName;
        aryTrans    = _Bone.aryTrans;
        aryRotas    = _Bone.aryRotas;
        AbsMode     = _Bone.AbsMode;
        ExportTranslate = _Bone.ExportTranslate;
        isPaint     = _Bone.isPaint;
        childList   = _Bone.childList;
        maxRibbonDis = _Bone.maxRibbonDis;
        ribbonFrame = _Bone.ribbonFrame;
        skinMat     = _Bone.skinMat;
        normMat     = _Bone.normMat;
    }

    SRT_Bone& operator = (const SRT_Bone& _Bone)
    {
        Name        = _Bone.Name;
        ParentName  = _Bone.ParentName;
        aryTrans    = _Bone.aryTrans;
        aryRotas    = _Bone.aryRotas;
        AbsMode     = _Bone.AbsMode;
        ExportTranslate = _Bone.ExportTranslate;
        isPaint     = _Bone.isPaint;
        childList   = _Bone.childList;
        maxRibbonDis = _Bone.maxRibbonDis;
        ribbonFrame = _Bone.ribbonFrame;
        skinMat     = _Bone.skinMat;
        normMat     = _Bone.normMat;

        return *this;
    }

    friend RtArchive& operator<< (RtArchive& Ar, SRT_Bone& b) 
	{
		Ar << b.Name << b.ParentName << b.aryTrans << b.aryRotas;
		Ar << b.AbsMode << b.ExportTranslate;
		
		BEGIN_VERSION_CONVERT(0)
		Ar << b.maxRibbonDis << b.ribbonFrame;	
        BEGIN_VERSION_CONVERT(1)
        END_VERSION_CONVERT
		END_VERSION_CONVERT

		return Ar;
	}

    void UseFrame(float frame, CRT_ActorInstance* _actor);
    void UseFrame(RtgMatrix12& parent, float frame, CRT_ActorInstance* _actor);
};

struct SRT_BoneRef
{
	unsigned char bondIndex;
	float         Weight;
	RtgVertex3    PosOffset;
	RtgVertex3    NorOffset;

	bool operator< (const SRT_BoneRef& ref) const 
	{
		return Weight < ref.Weight ? true : false;
	}

	friend RtArchive& operator<<(RtArchive& Ar, SRT_BoneRef& b) 
	{
		Ar << b.bondIndex << b.Weight << b.PosOffset << b.NorOffset;
		BEGIN_VERSION_CONVERT(1)
		END_VERSION_CONVERT
		return Ar;
	}

	void Output()
	{
        RtCoreLog().Info("idx = %d,weight = %.3f\n", bondIndex, Weight);
	}
};

typedef vector<SRT_Bone>      boneArray_t;
typedef map<string, size_t>   bonemap_t;

struct SRT_DynamicVtx
{
#ifdef MAX_PLUGIN_EXPORTS
	RtgVertex3 pos;
#endif
	RtgVertex3 nor;
	RtgVertex2 uv;
	vector<SRT_BoneRef> boneList;
    
	friend RtArchive& operator<< (RtArchive& Ar, SRT_DynamicVtx &v) 
	{
		Ar << v.nor << v.uv << v.boneList;
		BEGIN_VERSION_CONVERT(1)
		END_VERSION_CONVERT
		return Ar;	
	}

	void Normalize()
	{
		float total = 0;
		for (size_t i = 0; i < boneList.size(); ++i)
			total += boneList[i].Weight;
		for (size_t i = 0; i < boneList.size(); ++i)
			boneList[i].Weight /= total;
	}

	void Output()
	{
		for(int i=0; i<(int)boneList.size(); i++)
			boneList[i].Output();
	}
};

const char Pool_Type_None   = 1;	// c++ new,should not appear,can delete 
const char Pool_Type_File   = 2;	// load from diskfile,delete by check ref
const char Pool_Type_Memory = 3;    // load from memory,clone or copy,delete direct 
const char Pool_Type_Dummy  = 4;    // dummy object,use master ref,don't delete

class CRT_PoolObject : public RtObject
{
public :
	RT_DECLARE_DYNAMIC(CRT_PoolObject,RtObject, NULL, "")

	CRT_PoolObject() 
        : m_poRef(0),
          m_poType(Pool_Type_None)
    { 
        SetState(Object_State_Ready);
		m_poTag = "";
		m_diskFile = "";
    }

	virtual ~CRT_PoolObject() 
    {
    }

	virtual CRT_PoolObject *NewObject() = 0;  	
	virtual void Precache() {}
	virtual void Uncache()  {}
    virtual void OnLoadFinish() {}
    virtual void OnCreateFinish() {}
    
public :
	string m_diskFile;
	string m_poTag;
	int    m_poRef;
	char   m_poType;
};

template<class T>
RtArchive& operator<< (RtArchive& Ar, T*& ob)
{
	if (Ar.IsStoring())
		Ar.WriteObject(ob);
	else
		ob = (T*)Ar.ReadObject(NULL);
    return Ar;
}

class CRT_Effect : public CRT_PoolObject
#ifdef CHAR_VIEWER
	,public CTimeViewHost
#endif
{
public :
	RT_DECLARE_DYNAMIC(CRT_Effect, CRT_PoolObject, NULL, "")

	CRT_Effect() 
    {
    }

	virtual ~CRT_Effect() 
    {
    }

    virtual void OnLoadFinish() = 0;
    virtual bool Create() = 0;
    virtual void Reset() = 0;
    virtual void LinkActor(CRT_ActorInstance* _actor) = 0;
    virtual void Destroy() = 0;

    virtual bool RequestTick() = 0;
	virtual void Tick(float deltaMill) = 0;
	virtual bool RequestUseFrame() = 0;
	virtual void UseFrame(unsigned long frame) = 0;
	virtual void Render(RtgDevice *dev,RTGRenderMask mask) = 0;

	virtual void ResetRibbonDynamic(){}

	virtual const char *GetName() = 0;
	virtual void SetName(const char *name) = 0;
    virtual bool Recreate() { Destroy(); return Create(); }

    virtual void SetName2(const char _name[])
    {
        m_Name = _name;
    }

    virtual const char* GetName2() const
    {
        return m_Name.c_str();
    }

#ifdef CHAR_VIEWER
	virtual void AttachPropGrid(void *p) = 0;
	virtual void AttachKeyGrid(int key,void *p) = 0;
	virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam) = 0;
	virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam) = 0;
#endif

public :
    string m_Name;
};

class CRT_EffectBase : public CRT_Effect
{
	RT_DECLARE_DYNAMIC(CRT_EffectBase, CRT_Effect, NULL, "")

	CRT_EffectBase() 
        : m_actor(NULL)
    { 
    }

	virtual ~CRT_EffectBase() 
    {
        Destroy();
    }

    virtual void OnLoadFinish()
    {
    }

    virtual bool Create()
    {
        return true;
    }

    virtual void Reset() 
    {
    }

    virtual void LinkActor(CRT_ActorInstance* _actor)
    {
        m_actor = _actor;
    }

    virtual void Destroy()
    {
    }

	long CRT_EffectBase::Serialize(RtArchive& Ar)
	{
		Ar<<m_eftName;
		BEGIN_VERSION_CONVERT(1)
		END_VERSION_CONVERT
		return Ar.IsError() ? 0 : 1;
	}

	virtual const char* GetName() 
    { 
        return m_eftName.c_str(); 
    }

	virtual void SetName(const char* name) 
    { 
        m_eftName = name; 
    }

	string m_eftName;
    CRT_ActorInstance* m_actor;
};

class CRT_Material : public CRT_PoolObject

#ifdef CHAR_VIEWER
	,public CTimeViewHost
#endif

{
	RT_DECLARE_DYNAMIC(CRT_Material, CRT_PoolObject, NULL, "")

	CRT_Material() 
    {
    }

	virtual ~CRT_Material() 
    {
    }

    virtual bool RequestTick() = 0;
	virtual void Tick(float deltaMill) = 0;
	virtual bool RequestUseFrame() = 0;
	virtual void UseFrame(unsigned long frame) = 0;
    virtual bool ResourceReady() = 0;

    virtual bool Create() = 0;
    virtual void Destroy() = 0;
    virtual void Reset() = 0;

    virtual bool Apply(RtgDevice* _Dev) = 0;
    virtual void Restore(RtgDevice* _Dev) = 0;

    virtual bool IsTransparent() = 0;
    virtual void SetVisibleGlobal(float visible) = 0;
    virtual void SetVisibleLocal(float visible) = 0;
    virtual bool RequestUpdateNormal() = 0;				
    virtual bool SetBaseColor(RtgVertex3 &color) = 0;
	virtual bool GetBaseColor(RtgVertex3 &color) = 0;

	virtual string& GetName() = 0;
	virtual void SetName(string &name) = 0;

	virtual bool GetGeometry(int& alphaMode, RtgTextItem*& texItem) = 0;
    virtual int  GetAlphaMdoe() = 0;
    virtual RtgTextItem* GetBaseText() = 0;
    virtual string TextureName(int _index) = 0;
    virtual bool Recreate() { Destroy(); OnLoadFinish(); return Create(); }
    virtual void OnLoadFinish() {}

#ifdef CHAR_VIEWER
	virtual void AttachPropGrid(void *p) = 0;
	virtual void AttachKeyGrid(int key,void *p) = 0;
	virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam) = 0;
	virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam) = 0;
	virtual bool CopyFrom(CRT_Material* pMat) = 0;
#endif
};

class CRT_MaterialLib : public CRT_PoolObject
{
public :

	RT_DECLARE_DYNCREATE(CRT_MaterialLib, CRT_PoolObject, NULL, "")
	
    bool Create();
    void OnLoadFinish();
	virtual CRT_PoolObject* NewObject(); 
    bool DelMaterial(const char* name);

    CRT_MaterialLib();
    virtual ~CRT_MaterialLib();

    virtual long Serialize(RtArchive& Ar)
    {
        Ar<<m_mtlList;
        BEGIN_VERSION_CONVERT(1)
        END_VERSION_CONVERT
        return Ar.IsError() ? 0 : 1;
    }

    virtual bool Swap(CRT_MaterialLib* _mtl)
    {
        if (!_mtl)
            return false;
        vector<CRT_Material*> _mtllist = m_mtlList;
        m_mtlList = _mtl->m_mtlList;
        _mtl->m_mtlList = _mtllist;
        return true;
    }

    size_t GetMtlNum() const
    {
        return m_mtlList.size();
    }

    bool RequestTick()
    {
        return true;
    }

    //lyymark 材质base tick
    void Tick(float deltaMill)
    {
        if (GetState() != Object_State_Ready)
            return;

        for (size_t i = 0; i < m_mtlList.size(); ++i)
            if (m_mtlList[i]->RequestTick())
                m_mtlList[i]->Tick(deltaMill);
    }

    bool RequestUseFrame()
    {
        return true;
    }

    void CRT_MaterialLib::UseFrame(unsigned long frame)
    {
        for (size_t i = 0; i < m_mtlList.size(); ++i)
            if (m_mtlList[i]->RequestUseFrame())
                m_mtlList[i]->UseFrame(frame);
    }

    bool RequestUpdateNormal()
    {
        for (size_t i = 0; i < m_mtlList.size(); ++i)
            if (m_mtlList[i]->RequestUpdateNormal())
                return true;
        return false;
    }

    bool RemoveMaterial(const std::string strName)
    {
        for (vector<CRT_Material*>::iterator it = m_mtlList.begin();
            it != m_mtlList.end(); ++it)
        {
            if ((*it)->GetName() == strName)
            {
                m_mtlList.erase(it);
                return true;
            }
        }
        return false;
    }

    bool AddMaterial(CRT_Material *mtl)
    {
        if (GetMaterial(mtl->GetName().c_str()))
            return false;
        m_mtlList.push_back(mtl);
        return true;
    }

    int GetMaterial_Id(const char* name)
    {
        for (size_t i = 0; i < m_mtlList.size(); ++i)
            if (m_mtlList[i]->GetName() == name) 
                return (int)i;
        return 0;
    }

    CRT_Material* GetMaterial(const char* name)
    {
        for (size_t i = 0; i < m_mtlList.size(); ++i)
            if (m_mtlList[i]->GetName() == name) 
                return m_mtlList[i];
        return NULL;
    }

    CRT_Material* GetMaterial(size_t _index)
    {
        if (_index < 0 || _index >= m_mtlList.size()) 
            return NULL;
        return m_mtlList[_index];  
    }

    bool ResourceReady() const
    {
        for (size_t i = 0; i < m_mtlList.size(); ++i)
            if (m_mtlList[i]->GetState() < Object_State_Ready) 
                return false;
        return true;
    }

    void Reset()
    {
        for (size_t i = 0; i < m_mtlList.size(); ++i)
            m_mtlList[i]->Reset(); 
    }

public :

    vector<CRT_Material*> m_mtlList;
    string m_Name;

#ifdef CHAR_VIEWER
	void AttachGrid(void *p);
	bool OnGridNotify(WPARAM wParam, LPARAM lParam);
	CRT_Material *GetSelectedMtl(void *p);
	/*
	 *	Convert one material to another type.
	 *	strNewType: can be one of standard, mu, blend
	 */
	bool ConvertMaterial(const std::string strName, const std::string strNewType);
#endif

};

class CRT_Skin : public CRT_PoolObject
{
	RT_DECLARE_DYNAMIC(CRT_Skin, CRT_PoolObject, NULL, "")

	CRT_Skin()
        : m_bStatic(false),
          m_bBoard(false),
          m_mtlLib(NULL),
          m_bDynShadow(true)
	{
		CM_MEMPROTECTOR(m_szSkinVersion, 10)
        SetState(Object_State_New);
	}
	CRT_Skin(const CRT_Skin &object)
	{
		memcpy(this, &object, sizeof(CRT_Skin));
		CM_MEMPROTECTOR(m_szSkinVersion, 10)
	}

	virtual ~CRT_Skin() 
    {
		CM_MEMUNPROTECTOR(m_szSkinVersion)
        Destroy();
    }

	virtual long Serialize(RtArchive& Ar);
    virtual bool Create(CRT_Actor* act);
    virtual bool UseFrame(CRT_SkinInstance* skin, float frame, float* visible, bool bUpdateNom) = 0;
    virtual void OnLoadFinish();
    virtual bool GetBoardPos(RtgVertex3 &pos) = 0;
    virtual void Destroy();

    virtual bool OnCreateInstance(CRT_SkinInstance* skin)
    {
        return false;
    }

    virtual RtgBufferItem* RequestVB(CRT_SkinInstance* skin, DWORD* _Offset, DWORD* _Size)
    {
        return NULL;
    }
    
    virtual UINT vertexNum() const
    {
        return 0;
    }

    virtual UINT vertexSize() const
    {
        return 0;
    }

    virtual DWORD vertexFvf() const
    {
        return 0;
    }

    virtual verArray_t* Get_VertexArray()
    {
        return NULL;
    }

    virtual bool RequestCreate(CRT_Actor* act)
    {
        if (GetState() < Object_State_Creating)
            return Create(act);
        return true;
    }

   /* virtual bool LinkActor(CRT_Actor *actor) 
    {
        if (m_actor && m_actor != actor)
        {
            RtCoreLog().Error("% s: multi link actor error\n", __FUNCTION__);
            return false;
        }

        m_actor = actor;
        return true;
    }*/

public :
	string m_name;
	bool   m_bDynShadow;
	bool   m_bBoard;
    bool   m_bStatic;
	//char   m_szSkinVersion[10];
	CM_MEMDEF(m_szSkinVersion, 10)

    vector<RtcSkinSubset> m_rsList;
    CRT_MaterialLib*      m_mtlLib;
};

class CRT_VaSkin : public CRT_Skin
{
	RT_DECLARE_DYNCREATE(CRT_VaSkin, CRT_Skin, NULL, "")

	CRT_VaSkin() 
        : m_pvb(NULL)
    { 
        SetState(Object_State_New);
        m_boardPos.Zero(); 
    }

	virtual ~CRT_VaSkin() 
    {
        Destroy();
    }

    virtual bool UseFrame(CRT_SkinInstance* skin, float frame, float* visible, bool bUpdateNom);
    virtual bool Create(CRT_Actor* act);
    virtual void Destroy();
    virtual RtgBufferItem* RequestVB(CRT_SkinInstance* skin, DWORD* _Offset, DWORD* _Size);
    virtual bool OnCreateInstance(CRT_SkinInstance* skin);
    void Output();
	
	virtual long Serialize(RtArchive& Ar)
	{
		CRT_Skin::Serialize(Ar);
		
		Ar<<m_bNor 
          <<m_bUV 
          <<m_aryVertex
          <<m_frmList;

		if (m_bBoard)		
        {
			Ar<<m_boardPos 
              <<m_boardPosList;
        }

		BEGIN_VERSION_CONVERT(1)
        END_VERSION_CONVERT

		return Ar.IsError() ? 0 : 1;
	}
	
	virtual CRT_PoolObject *NewObject() 
    {
		++m_poRef;
		return this;
	}

	virtual bool GetBoardPos(RtgVertex3 &pos)
	{
		pos = m_boardPos;
		return true;
	}

	void SetVertex(const vector<SRT_StaticVtx>& verVertex)
    {
        m_aryVertex = verVertex;
    }

    virtual UINT vertexNum() const
    {
        return m_aryVertex.size();
    }

    virtual UINT vertexSize() const
    {
        return sizeof(SRT_StaticVtx);
    }

    virtual DWORD vertexFvf() const
    {
        return SRT_StaticVtx::fvf;
    }

    virtual verArray_t* Get_VertexArray()
    {
        return &m_aryVertex;
    }

	long PushVertex(const SRT_StaticVtx& vt);

	bool m_bUV;
	bool m_bNor;

    RtgBufferItem*          m_pvb;
	verArray_t	            m_aryVertex;
	vector<SRT_VaFrame>		m_frmList;
	RtgVertex3              m_boardPos;
	vector<RtgVertex3>		m_boardPosList;
};

class CRT_SkelSkin : public CRT_Skin
{
	RT_DECLARE_DYNCREATE(CRT_SkelSkin, CRT_Skin, NULL, "")

	CRT_SkelSkin() 
        : m_actor(NULL)
    {  
    }

	virtual ~CRT_SkelSkin() 
    {
    }
	
    virtual bool Create(CRT_Actor* act);
    virtual void Destroy();
    virtual bool UseFrame(CRT_SkinInstance* skin, float frame, float* visible, bool bUpdateNom);
	//virtual bool LinkActor(CRT_Actor *actor);
    void Update(CRT_SkinInstance* skin);
    void UpdateWithNormal(CRT_SkinInstance* skin);
    void Output();
    virtual RtgBufferItem* RequestVB(CRT_SkinInstance* skin, DWORD* _Offset, DWORD* _Size);
    virtual bool OnCreateInstance(CRT_SkinInstance* skin);

    virtual long Serialize(RtArchive& Ar)
    {
        CRT_Skin::Serialize(Ar);
        Ar<<m_verVertex;
        BEGIN_VERSION_CONVERT(1)
        END_VERSION_CONVERT
        return Ar.IsError() ? 0 : 1;
    }

    virtual CRT_PoolObject *NewObject() 
    {
        ++m_poRef;
        return this;
    }

    virtual bool GetBoardPos(RtgVertex3 &pos) 
    { 
        return false; 
    }

    virtual UINT vertexNum() const
    {
        return m_verVertex.size();
    }

    virtual UINT vertexSize() const
    {
        return sizeof(SRT_StaticVtx);
    }

    virtual DWORD vertexFvf() const
    {
        return SRT_StaticVtx::fvf;
    }

    virtual verArray_t* Get_VertexArray()
    {
        RTASSERT(false);
        return NULL;
    }

#ifdef MAX_PLUGIN_EXPORTS
	long PushVertex(const SRT_DynamicVtx& vt);
#endif

public :
	vector<SRT_DynamicVtx>  m_verVertex;
	CRT_Actor*              m_actor;

};

const char ACTOR_ANIM_STATIC = 0;
const char ACTOR_ANIM_VA     = 1;
const char ACTOR_ANIM_SKEL   = 2;
typedef EXT_SPACE::unordered_map<string, SRT_Pose> TPoseMap;

typedef struct _CRT_Slot
{
    string name;
    RtgMatrix12 mat;

} CRT_Slot;

#define Error_BoneIndex (-1)

class CRT_Actor : public CRT_PoolObject	
{
public :

	RT_DECLARE_DYNCREATE(CRT_Actor, CRT_PoolObject, NULL, "")

    virtual long Serialize(RtArchive& Ar);

	void RebuildBoneMap() 
	{
		m_boneMap.clear();
		for (int i = (int)m_bones.size() - 1; i >= 0; --i)
			m_boneMap[m_bones[i].Name] = i;     
	}

	bool RequestCreate();
 //   bool UseFrame(long frame);
    void OnLoadFinish();
    
    bool Merge2(CRT_Actor* act);
	bool Merge(CRT_Actor* act);
	bool MergeEvent(CRT_Actor* act);
	void Output();

    CRT_Actor();
	CRT_Actor(const CRT_Actor& _actor);
    
    virtual ~CRT_Actor();

    size_t GetBoneIndex(const char boneName[])
    {
        bonemap_t::iterator _it = m_boneMap.find(boneName);
        if (_it == m_boneMap.end())
            return Error_BoneIndex;
        return _it->second;
    }

    SRT_Bone* GetBone(const char boneName[])
    {
        bonemap_t::iterator _it = m_boneMap.find(boneName);
        if (_it == m_boneMap.end())
            return NULL;
        return &m_bones[_it->second];
    }

    SRT_Pose* GetPose(const char *name)
    {
        if (m_poseMap.empty())
            return NULL;
        if (!stricmp(name, "first"))
             return &(m_poseMap.begin()->second);

        TPoseMap::iterator it = m_poseMap.find(name);
        if (it != m_poseMap.end())
            return &((*it).second);
        return NULL;
    }

    virtual CRT_PoolObject *NewObject()
    {
        ++m_poRef;
        return this;
    }

public :
	float               m_animSpeed;	// 动画速度的倍率。控制动画播放的速度。
	TPoseMap            m_poseMap;      // 动画姿势映射。存储不同动画姿势的数据。
    long                m_frameNum;		// 当前动画的帧数。用于跟踪动画播放的位置。				
	bool                m_bUseLight;    // 是否启用光照。布尔值，决定是否在渲染时应用光照。
	bool                m_bUseVC;
	bool                m_bZTest;
	bool                m_bDynamicShadow;
	char                m_animType;
    boneArray_t         m_bones;        // 骨骼数组。存储角色的骨骼数据。
    bonemap_t           m_boneMap;      // 骨骼索引映射。用于快速查找骨骼。
    vector<CRT_Skin*>   m_skinList;
	vector<CRT_Effect*> m_eftList;
	vector<RtgAABB>     m_boundBoxList;

    CM_MEMDEF(m_szVersion, 10)
};

template<class T>
class keyframe
{
public:
	keyframe() {}
	virtual ~keyframe() {}

	struct SKeyFrame 
	{
		unsigned long frame;
		T data;
		friend RtArchive& operator<<(RtArchive &Ar, SKeyFrame &k) 
		{
			return Ar << k.frame << k.data;
		}
	};
    // 遍历关键帧列表，查找frame对应的关键帧
	bool GetKeyFrame(unsigned long frame,T &key)
	{    // 遍历关键帧列表，寻找包含指定帧的区间
		for(unsigned long i=1; i<m_keyList.size(); i++) {
			if(m_keyList[i].frame >= frame && m_keyList[i-1].frame <= frame) {
				unsigned long f1,f2;
				T &prev = m_keyList[i-1].data;// 前一个关键帧的数据
				f1 = m_keyList[i-1].frame;// 前一个关键帧的帧号
				T &next = m_keyList[i].data;// 当前关键帧的数据
				f2 = m_keyList[i].frame;// 当前关键帧的帧号
				key.Interpolate(frame,f1,prev,f2,next);//插值计算
				return true;
			}
		}
		return false;
	}
	bool AddKeyFrame(unsigned long frame,T &key)
	{
		SKeyFrame k;
		vector<SKeyFrame>::iterator it;
        if(m_keyList.size() == 0)
		{
			k.frame = frame;
			k.data = key;
			m_keyList.push_back(k);
			return true;
		}
		if(GetKeyFrame(frame)) return false;
		for(it=m_keyList.begin(); it!=m_keyList.end(); it++) 
		{
			if(frame < (*it).frame)
			{
				k.frame = frame;
				k.data = key;
				m_keyList.insert(it,k);
				return true;
			}
		}
		k.frame = frame;
		k.data = key;
		m_keyList.insert(it,k);
        return true;
	}
	bool DelKeyFrame(unsigned long frame)
	{
		vector<SKeyFrame>::iterator it;
		for(it=m_keyList.begin(); it!=m_keyList.end(); it++) 
		{
			if(frame == (*it).frame)
			{
				m_keyList.erase(it);
				return true;
			}
		}
		return false;
	}
	unsigned long Size()
	{
		return (unsigned long)m_keyList.size();
	}
	T *GetKeyFrame(int idx,unsigned long &frame)
	{
		if(idx < 0 || idx >= (int)m_keyList.size()) return NULL;
		frame = m_keyList[idx].frame;
		return &m_keyList[idx].data;
	}
	T *GetKeyFrame(unsigned long frame)
	{
		for(unsigned long i=0; i<m_keyList.size(); i++)
		{
			if(m_keyList[i].frame == frame) return &m_keyList[i].data;
		}
		return NULL;
	}
	friend RtArchive& operator<<(RtArchive &Ar, keyframe &k) 
	{
		Ar << k.m_begFrame << k.m_endFrame << k.m_keyList;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return Ar;
	}
    ////获取关键帧起始帧号
	unsigned long GetBegFrame()
	{
		unsigned long size = (unsigned long)m_keyList.size();
        if(size==0)
			return 0;
		return m_keyList[0].frame;
	}
    ////获取关键帧结束帧号
	unsigned long GetEndFrame()
	{
		unsigned long size = (unsigned long)m_keyList.size();
		if(size==0)
			return 0;
		return m_keyList[size-1].frame;
	}

private:
	vector<SKeyFrame> m_keyList;
	unsigned long m_begFrame,m_endFrame;
};

const long ALPHA_COLOR_KEY  = -2;
const long ALPHA_NULL       = -1;
const long ALPHA_BLEND		= 0;
const long ALPHA_ADD		= 1;
const long ALPHA_SUB		= 2;
const long ALPHA_MODULATE	= 3;

#endif
