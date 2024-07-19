#ifndef __CHA_BASIC_H__
#define __CHA_BASIC_H__
//
// character system basic define file
//
#pragma warning(disable : 4018)
#pragma warning(disable : 4267)

#include "cha_compress.h"
//
// auto save/load with stl container
//

const char ACTOR_ANIM_STATIC = 0;
const char ACTOR_ANIM_VA     = 1;
const char ACTOR_ANIM_SKEL   = 2;


const long ACTOR_FILE_VERSION_0  = 100;
const long ACTOR_FILE_VERSION_1  = 101;
const long ACTOR_FILE_VERSION_ZFS  = 102;

const long LIGHTING_MODE_NULL  = 0;
const long LIGHTING_MODE_FULL  = 1;

const long SHADOW_MODE_NULL    = 0;
const long SHADOW_MODE_STATIC  = 1;
const long SHADOW_MODE_DYNAMIC = 2;



enum
{
	RTCharactorNormal      = 0,
	RTCharactorConvertSave = 1,
};
struct SSerialize
{		
	static int  ms_nCurrentFileVersion;
	///!判断是否为存储 Skin
	///!这样做的原因是 Skin 的存取没有存储版本号，没有办法做到版本的兼容
	///!所以必须记取教训,所有的文件的存储必须要有版本号.
	static bool ms_bIsLoadingSkin;
};

// vector
template<class T>
RtArchive& operator<<(RtArchive &ar, vector<T> &v)
{
	long i,num = 0;
	if(ar.IsLoading())
	{
		ar << num;
		v.reserve(num);
		for(i=0; i<num; i++)
		{
			T tmp;
			ar << tmp;
			v.push_back(tmp);
		}
	}
	else
	{
		num = (long)v.size();
		ar << num;
        for(i=0; i<num; i++)
			ar << v[i];
	}
	return ar;
};

template<class T>
RtArchive& operator<<(RtArchive &ar, list<T> &v)
{
	long i,num = 0;
	if(ar.IsLoading())
	{
		ar << num;
		for(i=0; i<num; i++)
		{
			T tmp;
			ar << tmp;
			v.push_back(tmp);
		}
	}
	else
	{
		num = (long)v.size();
		ar << num;
		list<T>::iterator it;
		for(it=v.begin(); it!=v.end(); it++)
			ar << (*it);
	}
	return ar;
};

// unordered_map
template<class T1,class T2>
RtArchive& operator<<(RtArchive &ar, EXT_SPACE::unordered_map<T1,T2> &h)
{
	long i,num = 0;
	if(ar.IsLoading())
	{
		ar << num;
		for(i=0; i<num; i++)
		{
			T1 t1;
			T2 t2;
			ar << t1 << t2;
			h[t1] = t2;
		}
	}
	else
	{
		num = (long)h.size();
		ar << num;
		EXT_SPACE::unordered_map<T1,T2>::iterator it;
		for(it=h.begin(); it!=h.end(); it++)
		{
			T1 t1;
			T2 t2;
			t1 = (*it).first;
			t2 = (*it).second;
			ar << t1 << t2;
		}
	}
	return ar;
};

//
// serialize object
//
template<class T>
RtArchive& operator<<(RtArchive &Ar, T *&ob)
{
	if(Ar.IsStoring())
	{
		Ar.WriteObject(ob);
	}
	else
	{
		ob = (T*)Ar.ReadObject(NULL);
	}
	return Ar;
}

class CRT_ObjectData;
    class CRT_Material;
    class CRT_MaterialLib;
    class CRT_Effect;
    class CRT_Skin;
	    class CRT_VaSkin;
	    class CRT_SkelSkin;
    class CRT_Actor;

class CRT_ObjectInst;
	class CRT_ActorInst;
	class CRT_SkinInst;
	class CRT_MaterialInst;
	class CRT_EffectInst;
	class CRT_MaterialLibInst;

// 
// VERSION_CONVERT
// 
#define BEGIN_VERSION_CONVERT(v) char __end = v;\
                                 ar << __end;\
                                 if(!__end)\
                                 {
#define END_VERSION_CONVERT   }


//
// a surface for render
//
struct SRT_RenderSurface
{
    string material;
    string texture;
	vector<short> idxVec;

    friend RtArchive& operator<<(RtArchive &ar, SRT_RenderSurface &s) 
	{
		ar << s.material << s.texture << s.idxVec;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar;
	}
};

//
// a pose event
//
struct SRT_PoseEvent
{
	string	action;		
	string	param;
	long	keyFrame;
	friend RtArchive& operator<<(RtArchive &ar, SRT_PoseEvent &pe) 
	{
		ar << pe.action << pe.param << pe.keyFrame;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar;
	}
};
//
// pose define
//
struct SRT_Pose
{
	string	name;
	long	startFrm;
	long	endFrm;
	vector<SRT_PoseEvent> eventVec;

	friend RtArchive& operator<<(RtArchive &ar, SRT_Pose &p) 
	{
		ar << p.name << p.startFrm << p.endFrm << p.eventVec;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar;
	}
};

//
// compressed vertex position
//
struct SRT_VaVtxPos
{
	short x,y,z;

	void SetValue(RtgVertex3 &v) {
		x = v.x*100; y = v.y*100; z = v.z*100;
	}
	void GetValue(RtgVertex3 &v) {
		v.x = (float)x/100; v.y = (float)y/100; v.z = (float)z/100;
	}	
	friend RtArchive& operator<<(RtArchive &ar, SRT_VaVtxPos &v) 
	{
		ar << v.x << v.y << v.z;
		return ar;
	}
};
typedef EXT_SPACE::unordered_map<string,SRT_Pose> TPoseMap;
//
// compressed vertex normal
//
struct SRT_VaVtxNor
{
	union
	{ 
		struct {long x:11;long y:11;long z:10;};
		long xyz;
	};

	void SetValue(RtgVertex3 &v) {
		x = v.x*1000; y = v.y*1000; z = v.z*500;
	}
	void GetValue(RtgVertex3 &v) {
		v.x = (float)x/1000; v.y = (float)y/1000; v.z = (float)z/500;
	}	
	friend RtArchive& operator<<(RtArchive &ar, SRT_VaVtxNor &v) 
	{
		return ar << v.xyz;
	}
};

//
// compressed vertex uv
//
struct SRT_VaVtxUV
{
	unsigned char uv[2];

	void SetValue(RtgVertex2 &u) {
		uv[0] = u.x*255; uv[1] = u.y*255;
	}
	void GetValue(RtgVertex2 &u) {
		u.x = uv[0]/255.0f;
#ifdef FLIP_V
		u.y = 1-uv[1]/255.0f;
#else
		u.y = uv[1]/255.0f;
#endif
	}
	friend RtArchive& operator<<(RtArchive &ar, SRT_VaVtxUV &v) 
	{
		return ar << v.uv[0] << v.uv[1];
	}
};

//
// a vertex animation frame 
//
struct SRT_VaFrame
{
	vector<SRT_VaVtxPos> verList;
	vector<SRT_VaVtxNor> norList;
	vector<SRT_VaVtxUV>  uvList;

	// need fix
	// ...
	float visible;
	friend RtArchive& operator<<(RtArchive &ar, SRT_VaFrame &f) 
	{
		ar << f.verList << f.norList << f.uvList << f.visible;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar;
	}
};

struct SRT_StaticVtx
{
	RtgVertex3 pos;
	RtgVertex3 nor;
	RtgVertex2 uv;

	friend RtArchive& operator<<(RtArchive &ar, SRT_StaticVtx &v) 
	{
		if( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_1 && ar.IsLoading() &&
			SSerialize::ms_bIsLoadingSkin      == false )
		{
			ar << v.pos;
			short sTemp[ 5 ] = { 0 };
			ar.Serialize( sTemp , 10 );			
			v.nor.x = sTemp[0]  * 0.01f;
			v.nor.y = sTemp[1]  * 0.01f;
			v.nor.z = sTemp[2]  * 0.01f;
			v.uv.x  = sTemp[3]  * 0.01f;
			v.uv.y  = sTemp[4]  * 0.01f;
		}
		if( ar.IsStoring() && SSerialize::ms_bIsLoadingSkin == false )
		{
			short nx(0),ny(0),nz(0),ux(0),uy(0);
			ar << v.pos;
			nx= v.nor.x * 100;
			ny= v.nor.y * 100;
			nz= v.nor.z * 100;
			ux= v.uv.x  * 100;
			uy= v.uv.y  * 100;
			ar << nx << ny << nz;
			ar << ux << uy ;			
		}
		if( ( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_0 && ar.IsLoading() )  || SSerialize::ms_bIsLoadingSkin )
		{
            ar << v.pos << v.nor << v.uv;
		}	
		return ar;
	}
};

struct SRibbonFrame
{
	short segNum;
	vector<SRT_VaVtxPos> pos;		// pos num = segNum + 1
	
	friend RtArchive& operator<<(RtArchive &ar, SRibbonFrame &v) 
	{
		return ar << v.segNum << v.pos;
	}
};

//
// bone matrix export type
//
const short BONE_MODE_FULL_MATRIX    = 1; 
// export matrix direct
// 12*sizeof(float) = 48 bytes

const short BONE_MODE_P_MATRIX       = 2; 
// export parent/son matrix without scale
// (3+4)*sizeof(float) = 28 bytes

const short BONE_MODE_P_MATRIX_SCALE = 3; 
// export parent/son matrix with scale
// 28 + 3*sizeof(float) = 31 bytes

struct SRT_Bone 
{
    string			name;
	string			parentName;
	unsigned char   mode;

	// full matrix
	vector<RtgMatrix12> matrixVec;

	// p matrix
	// not imp...
	vector<RtgVertex3> transVec;
	vector<RtgVertex4> rotatVec;

	// p matrix with scale
	// not imp...
	vector<RtgVertex3> scaleVec;

	vector<SRibbonFrame> ribbonFrame;

	// runtime var,don't serialize
	vector<SRT_Bone*>  childList;
	RtgMatrix12 skinMat;
	RtgMatrix12 normMat;
	
	void UseFrame(float frame);
	void UseFrame(RtgMatrix12 &parent,float frame);

    friend RtArchive& operator<<(RtArchive &ar, SRT_Bone &b) 
	{
		if( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_1 && ar.IsLoading() && SSerialize::ms_bIsLoadingSkin == false )
		{
			ar << b.name << b.parentName << b.mode;
			if(b.mode == BONE_MODE_FULL_MATRIX)
			{
				std::vector<SShortMatrix12>  kaShortMatrixArray;
				ar << kaShortMatrixArray;
				for( int i = 0 ; i < kaShortMatrixArray.size() ; i ++ )
				{
					RtgMatrix12 kInsert;
					ConvertShortMatrixToRtgMatrix( kaShortMatrixArray[ i ] , kInsert );
					b.matrixVec.push_back( kInsert );
				}			
			}			
			/*
			else if(mode == BONE_MODE_P_MATRIX)
			ar << b.transVec << b.rotatVec;
			else if(mode == BONE_MODE_P_MATRIX_SCALE)
			ar << b.transVec << b.rotatVec << b.scaleVec; 
			*/
			ar << b.ribbonFrame;
			BEGIN_VERSION_CONVERT(1)
			END_VERSION_CONVERT		
		}
		if( ar.IsStoring() && SSerialize::ms_bIsLoadingSkin == false )
		{
            ar << b.name << b.parentName << b.mode;
			if(b.mode == BONE_MODE_FULL_MATRIX)
			{
				std::vector<SShortMatrix12>  kaShortMatrixArray;
				for( int i = 0 ; i < b.matrixVec.size() ; i ++ )
				{
                    SShortMatrix12 kInsert;
					ConvertRtgMatrixToShortMatrix( b.matrixVec[ i ] ,kInsert );
					kaShortMatrixArray.push_back( kInsert );
				}
				ar << kaShortMatrixArray;
			}			
			/*
			else if(mode == BONE_MODE_P_MATRIX)
			ar << b.transVec << b.rotatVec;
			else if(mode == BONE_MODE_P_MATRIX_SCALE)
			ar << b.transVec << b.rotatVec << b.scaleVec; 
			*/
			ar << b.ribbonFrame;
			BEGIN_VERSION_CONVERT(1)
			END_VERSION_CONVERT
		}
		if( ( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_0 && ar.IsLoading() )  || SSerialize::ms_bIsLoadingSkin )
		{
			ar << b.name << b.parentName << b.mode;
			if(b.mode == BONE_MODE_FULL_MATRIX)
			{
				ar << b.matrixVec;
			}
			/*
			else if(mode == BONE_MODE_P_MATRIX)
			ar << b.transVec << b.rotatVec;
			else if(mode == BONE_MODE_P_MATRIX_SCALE)
			ar << b.transVec << b.rotatVec << b.scaleVec; 
			*/
			ar << b.ribbonFrame;		

			BEGIN_VERSION_CONVERT(1)
			END_VERSION_CONVERT
		}
		return ar;
	}
	void Output()
	{
		RtCoreLog().Info("name = [%s], parent = [%s], matrix = %d\n", name.c_str(),
			parentName.c_str(),matrixVec.size());
	}
};

struct SRT_BoneRef
{
	unsigned char idx;
	float  weight;
	RtgVertex3 posOffset;
	RtgVertex3 norOffset;

	// runtime var
	SRT_Bone *bone;
	bool operator<(const SRT_BoneRef &ref) const 
	{
		return weight < ref.weight ? true : false;
	}
	friend RtArchive& operator<<(RtArchive &ar, SRT_BoneRef &b) 
	{		
		ar << b.idx << b.weight << b.posOffset << b.norOffset;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar;
	}
	void Output()
	{
        RtCoreLog().Info("idx = %d, weight = %.2f ", idx, weight);
	}
};

struct SRT_DynamicVtx
{
#ifdef MAX_UTILITY
	RtgVertex3 pos;
#endif
	RtgVertex3 nor;
	RtgVertex2 uv;
	vector<SRT_BoneRef> boneVec;
    
	friend RtArchive& operator<<(RtArchive &ar, SRT_DynamicVtx &v) 
	{		
		ar << v.nor << v.uv << v.boneVec;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar;	
	}
	void Normalize()
	{
		int i;
		float total = 0;
		for(i=0; i<(int)boneVec.size(); i++)
		{
			total += boneVec[i].weight;
		}
		for(i=0; i<(int)boneVec.size(); i++)
		{
			boneVec[i].weight /= total;
		}
	}
	void Output()
	{
		for(int i=0; i<(int)boneVec.size(); i++)
		{
			boneVec[i].Output();
		}
		RtCoreLog().Info("\n");
	}
};

//
// speed up load object and save memory
//
// const char POOL_TYPE_NONE   = 1;	// c++ RT_NEW,should not appear,can delete 
// const char POOL_TYPE_DISK   = 2;	// load from diskfile,delete by check ref
// const char POOL_TYPE_COPY   = 3;	// copy from disk object,delete by check ref
// const char POOL_TYPE_MEMORY = 3;    // load from memory,clone or copy,delete direct 
// const char POOL_TYPE_DUMMY  = 4;    // dummy object,use master ref,don't delete

const char POOL_TYPE_OBJ_UNKNOWN = 1;
const char POOL_TYPE_OBJ_MAIN	 = 2;
const char POOL_TYPE_OBJ_SUB	 = 3;
const char POOL_TYPE_OBJ_COPY	 = 4;

const long RELATED_RES_TEXTURE   = 1;
struct SRelatedRes
{
	long type;
	string res;
};

class CRT_ObjectData : public RtObject
{
	RT_DECLARE_DYNAMIC(CRT_ObjectData, RtObject, NULL, "")
	CRT_ObjectData() { 
        m_poRef  = 0; 
		m_poType = POOL_TYPE_OBJ_UNKNOWN;
    } 
	virtual ~CRT_ObjectData() {}

    // CRT_PoolObject interface
	virtual void Precache() {}					// preload related resource
	virtual void Uncache() {}					// unload related resource
	virtual void GetRelatedRes(vector<SRelatedRes> &res) {}
	virtual CRT_ObjectInst *CreateInstance() { return NULL; }
	virtual bool CopyFrom(CRT_ObjectData *from) { return false; }
	virtual void DataChange(DWORD p1, DWORD p2); // notify instance
	// void AddRef() { m_poRef++; }
	// void Release() { m_poRef--; }

	string m_diskFile;
	string m_poTag;
	int    m_poRef;
	char   m_poType;
	list<CRT_ObjectInst*> m_instList;
};

class CRT_Skin : public CRT_ObjectData
{
	RT_DECLARE_DYNAMIC(CRT_Skin, CRT_ObjectData, NULL, "")
	CRT_Skin();
	virtual ~CRT_Skin(); 
	virtual CRT_ObjectInst *CreateInstance();

	// From RtObject
	virtual long Serialize(RtArchive& ar);

	// CRT_Skin Interface
	virtual bool Create(vector<SRT_StaticVtx> &verList) = 0;
	virtual bool GetFrame(float frame,vector<SRT_StaticVtx> &verList,float &visible,bool updateNor) = 0;
	virtual bool LinkActor(CRT_Actor *actor) = 0;
	virtual bool GetBoardPos(RtgVertex3 &pos) = 0;
	virtual void Output() {}
	virtual long GetVertexNum() { return 0; }

	string m_name;
	bool   m_bBoard;
	
	// help skin, no graph
	bool   m_bHelper;
	bool   m_bHelpDynamic;

    vector<SRT_RenderSurface>	 m_rsVec;
    CRT_MaterialLib             *m_mtlLib;
	RtDict m_dict;
public:	
};

//
// skin use vertex animation
//
class CRT_VaSkin : public CRT_Skin
{
	RT_DECLARE_DYNCREATE(CRT_VaSkin, CRT_Skin, NULL, "")
	CRT_VaSkin() { m_boardPos.Zero(); }
	virtual ~CRT_VaSkin() {}

	// from RtObject
	virtual long Serialize(RtArchive& ar);
	
	// from CRT_Skin
	virtual bool Create(vector<SRT_StaticVtx> &verList)
	{
		verList.clear();
		verList = m_verList;
		return true;
	}
	virtual bool GetFrame(float frame,vector<SRT_StaticVtx> &verList,float &visible,bool updateNor);
	virtual bool GetBoardPos(RtgVertex3 &pos)
	{
		pos = m_boardPos;
		return true;
	}
	virtual bool LinkActor(CRT_Actor *actor)
	{
		return true;
	}
	virtual long GetVertexNum() { return m_verList.size(); }

	void Output();
	long PushVertex(SRT_StaticVtx &v);

	bool m_bUV;
	bool m_bNor;
	bool m_bZup;
	vector<SRT_StaticVtx>	m_verList;
	vector<SRT_VaFrame>		m_frmList;
	RtgVertex3 m_boardPos;
	vector<RtgVertex3>		m_boardPosList;
};

//
// skin use skeletel animation
//
class CRT_SkelSkin : public CRT_Skin
{
	RT_DECLARE_DYNCREATE(CRT_SkelSkin, CRT_Skin, NULL, "")
	CRT_SkelSkin() { m_actor = NULL; }
	virtual ~CRT_SkelSkin() {}
	
	// from RtObject
	virtual long Serialize(RtArchive& ar);

	// from CRT_Skin
	virtual bool Create(vector<SRT_StaticVtx> &verList)
	{
		verList.clear();
		for(int i=0; i<m_verList.size(); i++)
		{
			SRT_StaticVtx v;
			v.pos = RtgVertex3(0,0,0);
			v.nor = RtgVertex3(0,0,0);
			v.uv  = m_verList[i].uv;
			verList.push_back(v);
		}
		return true;
	}
	virtual bool GetFrame(float frame,vector<SRT_StaticVtx> &verList,float &visible,bool updateNor);
	virtual bool LinkActor(CRT_Actor *actor);
	virtual bool GetBoardPos(RtgVertex3 &pos) { return false; }
	virtual long GetVertexNum() { return m_verList.size(); }

	void Output();
	long PushVertex(SRT_DynamicVtx &v);

	vector<SRT_DynamicVtx> m_verList;
	CRT_Actor *m_actor;

private:
	void Update(vector<SRT_StaticVtx> &verList);
	void UpdateWithNormal(vector<SRT_StaticVtx> &verList);
};



struct SActorInfo 
{
	long verNum,triNum,boneNum,skinNum,mtlNum,eftNum;
};

class CRT_Actor : public CRT_ObjectData	
{
	RT_DECLARE_DYNCREATE(CRT_Actor, CRT_ObjectData, NULL, "")
	CRT_Actor(); 
	virtual ~CRT_Actor();

	void GetInfo(SActorInfo &info);
	bool LoadRelatedActorFile();
	virtual CRT_ObjectInst *CreateInstance();
	virtual void GetRelatedRes(vector<SRelatedRes> &res);

	// RtObject interface
	virtual long Serialize(RtArchive& ar);

	bool Save(RtArchive& ar);
	bool Load(RtArchive& ar);
	bool SerializeData(RtArchive& ar);
	//bool SerializeOldData(RtArchive& ar);

	bool Merge(CRT_Actor *act);
	bool MergeEvent(CRT_Actor *act);

	bool Build();
	bool UseFrame(float frame, RtgMatrix12 *m);
	SRT_Bone *GetBone(const char *name);
	SRT_Bone *GetBone(unsigned char idx);
	bool GetBoneIndex(const char *name, unsigned char &idx);
	void Output();
	SRT_Pose *FindPose(const char *name); 
    
	// serialize var
	vector<SRT_Bone> m_boneVec;
	TPoseMap m_poseMap;
    long	 m_frameNum;				// m_frameNum==0, means static object			
	char	 m_animType;
	
    vector<CRT_Skin*>   m_skinVec;
	vector<CRT_Effect*> m_eftVec;
	vector<RtgAABB>     m_boundBoxVec;
	vector<string>      m_relatedActFile;

	// string option
	string m_strLightingMode;
	string m_strShadowMode;

	long m_shadowMode;
	long m_lightingMode;
	// ERenderMode     m_renderMode;
	float			m_animSpeed;
	bool			m_bZTest;
	bool            m_bZWrite;
	bool			m_bFog;
	float           m_visible;
	RtgVertex3      m_scale;
	RtgVertex3      m_offset;

	RtDict              m_dict;

#ifdef ACTOR_EDITOR
	void AttachPoseGrid(void *p);
	bool OnPoseGridNotify(WPARAM wParam, LPARAM lParam);
	string GetSelectedPose(void *p);

	void AttachEffectGrid(void *p);
	bool OnEffectGridNotify(WPARAM wParam, LPARAM lParam);
	CRT_Effect *GetSelectedEffect(void *p);

	void AttachGrid(void *p);
	bool OnGridNotify(WPARAM wParam, LPARAM lParam);
	CRT_Material *GetSelectedMaterial(void *p);
	CRT_Skin *GetSelectedSkin(void *p);
	// bool Save(RtArchive *ar);
	bool UpdateSkin(RtgDevice *dev);
	bool UpdateEffect(RtgDevice *dev);
#endif

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
		friend RtArchive& operator<<(RtArchive &ar, SKeyFrame &k) 
		{
            ar << k.frame << k.data;
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			return ar;
		}
	};
	bool GetKeyFrame(unsigned long frame,T &key)
	{
		for(unsigned long i=1; i<m_keyList.size(); i++) {
			if(m_keyList[i].frame >= frame && m_keyList[i-1].frame <= frame) {
				unsigned long f1,f2;
				T &prev = m_keyList[i-1].data;
				f1 = m_keyList[i-1].frame;
				T &next = m_keyList[i].data;
				f2 = m_keyList[i].frame;
				key.Interpolate(frame,f1,prev,f2,next);
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
	friend RtArchive& operator<<(RtArchive &ar, keyframe &k) 
	{
		ar << k.m_begFrame << k.m_endFrame << k.m_keyList;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar;
	}
	unsigned long GetBegFrame()
	{
		unsigned long size = (unsigned long)m_keyList.size();
        if(size==0)
			return 0;
		return m_keyList[0].frame;
	}
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

const char ALPHA_NULL       = 0;
const char ALPHA_MASK       = 1;
const char ALPHA_BLEND		= 2;
const char ALPHA_ADD		= 3;
const char ALPHA_SUB		= 4;
const char ALPHA_MODULATE	= 5;
const char ALPHA_MODULATE2X	= 6;

const char HL_MODE_NULL     = 0;
const char HL_MODE_VERTEX   = 1;
const char HL_MODE_TEXTURE  = 2;

template<class T>
void InterpolateLiner(DWORD f, T &v, DWORD f1, T &v1, DWORD f2, T &v2)
{
	float b = ((float)f - f1)/(f2 - f1);
	v = v1 + b*(v2 - v1);
}

class CRT_Material : public CRT_ObjectData
#ifdef ACTOR_EDITOR
	, public CTimeViewHost
#endif
{
public:
	RT_DECLARE_DYNAMIC(CRT_Material, CRT_ObjectData, NULL, "")
	CRT_Material() {}
	virtual ~CRT_Material() {}

	virtual long Serialize(RtArchive& ar)
	{
		ar << m_name << m_dict;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar.IsError() ? 0 : 1;
	}
	string& GetName() { return m_name; }
	void SetName(string &name) { m_name = name; }
	virtual void Output() {}
	
	string m_name;
	RtDict m_dict;

#ifdef ACTOR_EDITOR
	virtual void AttachPropGrid(void *p) = 0;
	virtual void AttachKeyGrid(int key,void *p) = 0;
	virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam) = 0;
	virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam) = 0;
#endif
};

struct SMtlStandardKey
{
	float       visible;
	RtgVertex4  amb,dif,emi,spe;
	float       spePower;

	RtgVertex3  envColor;
	RtgVertex3  baseRotate;
	RtgVertex3  baseTranslate;
	RtgVertex3  baseScale;

	RtgVertex3  effectRotate1;
	RtgVertex3  effectTranslate1;
	RtgVertex3  effectScale1;

	RtgVertex3  effectRotate2;
	RtgVertex3  effectTranslate2;
	RtgVertex3  effectScale2;
	SMtlStandardKey()
	{
		visible = 1.0f;
		envColor.Set(1.0,1.0,1.0);
		amb.Set(1.0f,1.0f,1.0f,1.0);
		dif.Set(1.0f,1.0f,1.0f,1.0);
		emi.Set(0.0f,0.0f,0.0f,1.0);
		spe.Set(1.0f,1.0f,1.0f,1.0);
		baseRotate.Zero();
		baseTranslate.Zero();
		baseScale.Set(1,1,1);
		effectRotate1.Zero();
		effectTranslate1.Zero();
		effectScale1.Set(1,1,1);
		effectRotate2.Zero();
		effectTranslate2.Zero();
		effectScale2.Set(1,1,1);
		spePower = 10.0;
	}
	void Interpolate(unsigned long frame, unsigned long f1, SMtlStandardKey &k1,
		unsigned long f2, SMtlStandardKey &k2)
	{
		float b = ((float)frame - f1)/(f2 - f1); 
		spePower = k1.spePower + b*(k2.spePower - k1.spePower);
		// visible = k1.visible + b*(k2.visible - k1.visible);
		InterpolateLiner(frame, visible, f1, k1.visible, f2, k2.visible);
		amb = k1.amb + b*(k2.amb - k1.amb);
		dif = k1.dif + b*(k2.dif - k1.dif);
		emi = k1.emi + b*(k2.emi - k1.emi);
		spe = k1.spe + b*(k2.spe - k1.spe);
		envColor = k1.envColor + b*(k2.envColor - k1.envColor);

		baseRotate    = k1.baseRotate    + b*(k2.baseRotate    - k1.baseRotate);
		baseTranslate = k1.baseTranslate + b*(k2.baseTranslate - k1.baseTranslate);
		baseScale     = k1.baseScale     + b*(k2.baseScale     - k1.baseScale);
		effectRotate1     = k1.effectRotate1     + b*(k2.effectRotate1     - k1.effectRotate1);
		effectTranslate1  = k1.effectTranslate1  + b*(k2.effectTranslate1  - k1.effectTranslate1);
		effectScale1	  = k1.effectScale1      + b*(k2.effectScale1      - k1.effectScale1);
		effectRotate2     = k1.effectRotate2     + b*(k2.effectRotate2     - k1.effectRotate2);
		effectTranslate2  = k1.effectTranslate2  + b*(k2.effectTranslate2  - k1.effectTranslate2);
		effectScale2	  = k1.effectScale2      + b*(k2.effectScale2      - k1.effectScale2);
	}
	friend RtArchive& operator<<(RtArchive &ar, SMtlStandardKey &k) 
	{
		if( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_1 && ar.IsLoading() && SSerialize::ms_bIsLoadingSkin == false )
		{
			unsigned char ucVisible(0);			
			ar << ucVisible << k.spePower;
			k.visible = ucVisible / 255.0f;

			SMtlStandardKeyTable  kTable;
			ar.Serialize( &kTable , sizeof( kTable ) );

#define Get_Color_From_Serial_4( nIndex , kDestColor )\
		if( nIndex != 0xF )\
		{\
			kDestColor = GetTableColorByIndex( nIndex );\
		}else\
		{\
			long lTemp = 0;\
			ar << lTemp;\
			kDestColor = LongToVector4_Color( lTemp );\
		}
#define Get_Color_From_Serial_3( nIndex , kDestColor )\
		if( nIndex != 0xF )\
		{\
			kDestColor = GetTableColorByIndex_3( nIndex );\
		}else\
		{\
			long lTemp = 0;\
			ar << lTemp;\
			kDestColor = LongToVector3_Color( lTemp );\
		}
#define Get_Matrix_From_Serial( nIndex , kDestVector )\
		if( nIndex != 0xF )\
		{\
			kDestVector = GetTableColorByIndex_3( nIndex );\
		}else\
		{\
			SShortVec3 kTemp;\
			ar << kTemp;\
			kDestVector = Vector3ToShort3_Vector_100_Inverse( kTemp );\
		}

			Get_Color_From_Serial_4( kTable.cAmb , k.amb );
			Get_Color_From_Serial_4( kTable.cDif , k.dif );
			Get_Color_From_Serial_4( kTable.cEmb , k.emi );
			Get_Color_From_Serial_4( kTable.cSpe , k.spe );
			Get_Color_From_Serial_3( kTable.cEnv , k.envColor );

			Get_Matrix_From_Serial(  kTable.cRot0 , k.baseRotate     );
			Get_Matrix_From_Serial(  kTable.cTra0 , k.baseTranslate  );
			Get_Matrix_From_Serial(  kTable.cSca0 , k.baseScale      );

			Get_Matrix_From_Serial(  kTable.cRot1 , k.effectRotate1     );
			Get_Matrix_From_Serial(  kTable.cTra1 , k.effectTranslate1  );
			Get_Matrix_From_Serial(  kTable.cSca1 , k.effectScale1      );	
			BEGIN_VERSION_CONVERT(0)
				Get_Matrix_From_Serial( kTable.cRot2 , k.effectRotate2     );
				Get_Matrix_From_Serial( kTable.cTra2 , k.effectTranslate2  );
				Get_Matrix_From_Serial( kTable.cSca2 , k.effectScale2      );			
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT	
		}		
		if( ar.IsStoring() && SSerialize::ms_bIsLoadingSkin == false )
		{
			unsigned char ucVisible(0);			
			ucVisible = k.visible * 255;
			ar << ucVisible << k.spePower;

			SMtlStandardKeyTable  kTable;
			kTable.cAmb  = GetColorTableIndex( k.amb );
			kTable.cDif  = GetColorTableIndex( k.dif );
			kTable.cEmb  = GetColorTableIndex( k.emi );
			kTable.cSpe  = GetColorTableIndex( k.spe );
			kTable.cEnv  = GetColorTableIndex( k.envColor );
			kTable.cRot0 = GetColorTableIndex( k.baseRotate );
			kTable.cTra0 = GetColorTableIndex( k.baseTranslate );
			kTable.cSca0 = GetColorTableIndex( k.baseScale );
			kTable.cRot1 = GetColorTableIndex( k.effectRotate1 );
			kTable.cTra1 = GetColorTableIndex( k.effectTranslate1 );
			kTable.cSca1 = GetColorTableIndex( k.effectScale1 );
			kTable.cRot2 = GetColorTableIndex( k.effectRotate2 );
			kTable.cTra2 = GetColorTableIndex( k.effectTranslate2 );
			kTable.cSca2 = GetColorTableIndex( k.effectScale2 );

			ar.Serialize( &kTable , sizeof( kTable ) );
			long  nAmb(0), nDif(0),nEmi(0),nSpe(0),nEnv(0);			
			nAmb = Vector4ToLong_Color( k.amb );
			nDif = Vector4ToLong_Color( k.dif );
			nEmi = Vector4ToLong_Color( k.emi );
			nSpe = Vector4ToLong_Color( k.spe );
			nEnv = Vector3ToLong_Color( k.envColor );			

			SShortVec3  kTemp0 = Vector3ToShort3_Vector_100( k.baseRotate    );
			SShortVec3  kTemp1 = Vector3ToShort3_Vector_100( k.baseTranslate );
			SShortVec3  kTemp2 = Vector3ToShort3_Vector_100( k.baseScale     );	

			if( kTable.cAmb == 0xF ) ar << nAmb;
			if( kTable.cDif == 0xF ) ar << nDif;
			if( kTable.cEmb == 0xF ) ar << nEmi;
			if( kTable.cSpe == 0xF ) ar << nSpe;
			if( kTable.cEnv == 0xF ) ar << nEnv;
			if( kTable.cRot0 == 0xF ) ar << kTemp0;
			if( kTable.cTra0 == 0xF ) ar << kTemp1;
			if( kTable.cSca0 == 0xF ) ar << kTemp2;
			kTemp0 = Vector3ToShort3_Vector_100( k.effectRotate1    );
			kTemp1 = Vector3ToShort3_Vector_100( k.effectTranslate1 );
			kTemp2 = Vector3ToShort3_Vector_100( k.effectScale1     );
			if( kTable.cRot1 == 0xF ) ar << kTemp0;
			if( kTable.cTra1 == 0xF ) ar << kTemp1;
			if( kTable.cSca1 == 0xF ) ar << kTemp2;

			BEGIN_VERSION_CONVERT(0)
				kTemp0 = Vector3ToShort3_Vector_100( k.effectRotate2    );
				kTemp1 = Vector3ToShort3_Vector_100( k.effectTranslate2 );
				kTemp2 = Vector3ToShort3_Vector_100( k.effectScale2     );
			if( kTable.cRot2 == 0xF ) ar << kTemp0;
			if( kTable.cTra2 == 0xF ) ar << kTemp1;
			if( kTable.cSca2 == 0xF ) ar << kTemp2;				
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT		
		} 
		if( ( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_0 && ar.IsLoading() )  || SSerialize::ms_bIsLoadingSkin )
		{
			ar << k.visible << k.amb << k.dif << k.emi << k.spe << k.spePower << k.envColor;
			ar << k.baseRotate << k.baseTranslate << k.baseScale;
			ar << k.effectRotate1 << k.effectTranslate1 << k.effectScale1;
			BEGIN_VERSION_CONVERT(0)
				ar << k.effectRotate2 << k.effectTranslate2 << k.effectScale2;
			BEGIN_VERSION_CONVERT(1)
				// read/write extra data
				END_VERSION_CONVERT
				END_VERSION_CONVERT
		}
		return ar;
	}
};

//
// frame buffer option
// how blend with the current frame buffer
//
struct SFrameBufferOption
{
	SFrameBufferOption()
	{
		twoSide = false;
		fog		= true;
		ztest	= true;
		zwrite	= true;
		shadow	= true;
		alphaMode = ALPHA_NULL;
		strAlphaMode = "None";
	}
	bool twoSide,fog,ztest,zwrite,shadow;
	long alphaMode;
	string strAlphaMode;
	
	friend RtArchive& operator<<(RtArchive &ar, SFrameBufferOption &k) 
	{
		if( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_1 && ar.IsLoading() && SSerialize::ms_bIsLoadingSkin == false )
		{
			SFrameBufferOptionTable  kTable;		
			ar.Serialize( &kTable , sizeof( kTable ) );
			k.fog      = kTable.bFog;
			k.twoSide  = kTable.bTwoSize;
			k.ztest    = kTable.bZTest;
			k.zwrite   = kTable.bZWrite;
			k.shadow   = kTable.bShadow;
			k.alphaMode= kTable.nAlphaMode;	
			BEGIN_VERSION_CONVERT(0)
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT
		}
		if( ar.IsStoring() && SSerialize::ms_bIsLoadingSkin == false )
		{
			SFrameBufferOptionTable  kTable;
			kTable.bFog      = k.fog;
			kTable.bTwoSize  = k.twoSide;
			kTable.bZTest    = k.ztest;
			kTable.bZWrite   = k.zwrite;
			kTable.bShadow   = k.shadow;
			kTable.nAlphaMode= k.alphaMode;			
			ar.Serialize( &kTable , sizeof( kTable ) );
			BEGIN_VERSION_CONVERT(0)
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT
		}
		if( ( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_0 && ar.IsLoading() )  || SSerialize::ms_bIsLoadingSkin )
		{
			ar << k.twoSide << k.fog << k.ztest << k.zwrite << k.shadow << k.alphaMode;
			BEGIN_VERSION_CONVERT(0)
				ar << k.strAlphaMode;
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT
		}
		return ar;
	}
};

//
// render option
// visible,light...
struct SRenderOption
{
	float       visible;
	RtgVertex4  amb,dif,emi,spe;
	float       spePower;
	
	SRenderOption() 
	{
		visible = 1;
		amb.Set(1,1,1,1);
		dif.Set(1,1,1,1);
		emi.Set(0,0,0,1);
		spe.Set(1,1,1,1);
		spePower = 10;
	}
	void Interpolate(unsigned long frame, unsigned long f1, SRenderOption &k1,
		unsigned long f2, SRenderOption &k2)
	{
		float b = ((float)frame - f1)/(f2 - f1); 
		spePower = k1.spePower + b*(k2.spePower - k1.spePower);
		visible = k1.visible + b*(k2.visible - k1.visible);
		amb = k1.amb + b*(k2.amb - k1.amb);
		dif = k1.dif + b*(k2.dif - k1.dif);
		emi = k1.emi + b*(k2.emi - k1.emi);
		spe = k1.spe + b*(k2.spe - k1.spe);
	}
	friend RtArchive& operator<<(RtArchive &ar, SRenderOption &k) 
	{
		ar << k.visible << k.amb << k.dif << k.emi << k.spe << k.spePower;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar;
	}
};

const long TEX_UV_BASE = 0; 
const long TEX_UV_CPOS = 1;
const long TEX_UV_CNOR = 2;
const long TEX_UV_CREL = 3;

//
// texture stage 
//
struct SRenderStage
{
	long        uvMode;
	RtgVertex3  rotate;
	RtgVertex3  translate;
	RtgVertex3  scale;

	SRenderStage() 
	{
		uvMode = TEX_UV_BASE;
		rotate.Zero();
		translate.Zero();
		scale.Set(1,1,1);
	}
	void Interpolate(unsigned long frame, unsigned long f1, SRenderStage &k1,
		unsigned long f2, SRenderStage &k2)
	{
		float b = ((float)frame - f1)/(f2 - f1); 
		rotate    = k1.rotate    + b*(k2.rotate    - k1.rotate);
		translate = k1.translate + b*(k2.translate - k1.translate);
		scale     = k1.scale     + b*(k2.scale     - k1.scale);
	}
	friend RtArchive& operator<<(RtArchive &ar, SRenderStage &k) 
	{
		ar << k.uvMode << k.rotate << k.translate << k.scale;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar;
	}
};

const char ENV_UV_BASE = 0; 
const char ENV_UV_XY   = 1; 
const char ENV_UV_YZ   = 2;
const char ENV_UV_ZX   = 3;
const char ENV_UV_CPOS = 4;
const char ENV_UV_CNOR = 5;
const char ENV_UV_CREL = 6;

class CRT_MtlStandard : public CRT_Material
{
public:
	RT_DECLARE_DYNCREATE(CRT_MtlStandard, CRT_Material, NULL, "")
	CRT_MtlStandard()
	{
		m_bTwoSide = false;
		m_bSyncActorFrame = false;
		m_bFog = true;
		m_bZTest = true;
		m_bBaseTexMatrix = false;
		m_alphaMode = ALPHA_NULL;
		m_hlMode = HL_MODE_NULL;

		m_effectUV1 = ENV_UV_BASE;
		m_bEffectMatrix1 = false;
		m_effectBlendMode1 = ALPHA_ADD;
		m_strEftBlendMode1 = "Add";
		m_strEffectUv1 = "Base";

		m_effectUV2 = ENV_UV_BASE;
		m_bEffectMatrix2 = false;
		m_effectBlendMode2 = ALPHA_ADD;
		m_strEftBlendMode2 = "Add";
		m_strEffectUv2 = "Base";

		m_strAlphaMode = "Null";
		m_strHlMode = "Null";
	}
	virtual ~CRT_MtlStandard() {}
	virtual CRT_ObjectInst *CreateInstance();
	virtual void GetRelatedRes(vector<SRelatedRes> &res);

	virtual long Serialize(RtArchive& ar)
	{

		CRT_Material::Serialize(ar);

		if( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_1 && ar.IsLoading() && SSerialize::ms_bIsLoadingSkin == false )
		{
			ar << m_texBase << m_texHl;
			ar << m_texEffect1 << m_effectUV1;
			ar << m_bBaseTexMatrix << m_bEffectMatrix1;
			ar << m_defKey << m_keyVec; 
			ar << m_bTwoSide << m_bSyncActorFrame;
			ar << m_bFog << m_bZTest << m_alphaMode << m_hlMode;			
			ar << m_bShadow;
			BEGIN_VERSION_CONVERT(0)
				ar << m_texEffectMask1;
			BEGIN_VERSION_CONVERT(0)
				ar << m_effectBlendMode1 ;
			BEGIN_VERSION_CONVERT(0)
				ar << m_texEffect2 << m_texEffectMask2 << m_bEffectMatrix2 << m_effectBlendMode2 ;
			ar <<  m_effectUV2;
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT
			END_VERSION_CONVERT
			END_VERSION_CONVERT
		}
		if( ar.IsStoring() && SSerialize::ms_bIsLoadingSkin == false )
		{
			ar << m_texBase << m_texHl;
			ar << m_texEffect1 << m_effectUV1;
			ar << m_bBaseTexMatrix << m_bEffectMatrix1;
			ar << m_defKey << m_keyVec; 
			ar << m_bTwoSide << m_bSyncActorFrame;
			ar << m_bFog << m_bZTest << m_alphaMode << m_hlMode;			
			ar << m_bShadow;
			BEGIN_VERSION_CONVERT(0)
				ar << m_texEffectMask1;
			BEGIN_VERSION_CONVERT(0)
				ar << m_effectBlendMode1 ;
			BEGIN_VERSION_CONVERT(0)
				ar << m_texEffect2 << m_texEffectMask2 << m_bEffectMatrix2 << m_effectBlendMode2 ;
			ar <<  m_effectUV2;
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT
			END_VERSION_CONVERT
			END_VERSION_CONVERT
		}
		if( ( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_0 && ar.IsLoading() )  || SSerialize::ms_bIsLoadingSkin )
		{
			ar << m_texBase << m_texHl;
			ar << m_texEffect1 << m_effectUV1;
			ar << m_bBaseTexMatrix << m_bEffectMatrix1;
			ar << m_defKey << m_keyVec; 
			ar << m_bTwoSide << m_bSyncActorFrame;
			ar << m_bFog << m_bZTest << m_alphaMode << m_hlMode;

			ar << m_strAlphaMode << m_strHlMode << m_strEffectUv1;
			ar << m_bShadow;
			BEGIN_VERSION_CONVERT(0)
				ar << m_texEffectMask1;
			BEGIN_VERSION_CONVERT(0)
				ar << m_effectBlendMode1 << m_strEftBlendMode1;
			BEGIN_VERSION_CONVERT(0)
				ar << m_texEffect2 << m_texEffectMask2 << m_bEffectMatrix2 << m_effectBlendMode2 << m_strEftBlendMode2;
			ar << m_strEffectUv2 << m_effectUV2;
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT
			END_VERSION_CONVERT
			END_VERSION_CONVERT
		}
		return ar.IsError() ? 0 : 1;
	}
	virtual void Output() {}

	string m_texBase;
	string m_texHl;

	string m_texEffectMask1;
	string m_texEffect1;
	string m_strEffectUv1;
	char   m_effectUV1;
	long   m_effectBlendMode1;
	string m_strEftBlendMode1;
	bool   m_bEffectMatrix1;

	string m_texEffectMask2;
	string m_texEffect2;
	string m_strEffectUv2;
	char   m_effectUV2;
	long   m_effectBlendMode2;
	string m_strEftBlendMode2;
	bool   m_bEffectMatrix2;

	// string m_texBump;
	// string m_texGlow;
	bool   m_bBaseTexMatrix;
	SMtlStandardKey m_defKey;
	keyframe<SMtlStandardKey> m_keyVec;
	
	bool	   m_bTwoSide;
	bool	   m_bSyncActorFrame;
	bool       m_bFog;
	bool       m_bZTest;
	bool       m_bShadow;
	char       m_alphaMode;
	char       m_hlMode;

	string     m_strAlphaMode;
	string     m_strHlMode;
#ifdef ACTOR_EDITOR
	virtual void AttachPropGrid(void *p);
	virtual void AttachKeyGrid(int key,void *p);
	virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam);
	virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam);

	// CTimeViewHost Interface
	string m_tvName;
	virtual const char *TVGetName();
	virtual void TVOnNotify(ETV_OP op,long param1,long param2);
	virtual int  TVGetKeyNum();
	virtual int  TVGetKeyFrame(int key);
#endif
};

//
// Blend Material
//
// white = material1 
// final color = color*mt1 + (1-color)*mtl2
// float  m_blendVar;
// final color = var*mtl1 + (1-var)*mtl2

struct SMtlBlendKey
{
	SRenderOption ro;
	SRenderStage  stage[3];
	float mixAmount;
	SMtlBlendKey()
	{
		mixAmount = 0.5;
	}
   
	void Interpolate(unsigned long frame, unsigned long f1, SMtlBlendKey &k1,
		unsigned long f2, SMtlBlendKey &k2)
	{
		float b = ((float)frame - f1)/(f2 - f1); 
		mixAmount = k1.mixAmount + b*(k2.mixAmount - k1.mixAmount);
		ro.Interpolate(frame, f1, k1.ro, f2, k2.ro);
		for(int i=0; i<3; i++)
			stage[i].Interpolate(frame,f1,k1.stage[i],f2,k2.stage[i]);
	}
	friend RtArchive& operator<<(RtArchive &ar, SMtlBlendKey &k) 
	{
		ar << k.ro;
		for(int i=0; i<3; i++)
			ar << k.stage[i];
		BEGIN_VERSION_CONVERT(0)
		ar << k.mixAmount;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		END_VERSION_CONVERT
		return ar;
	}
};

class CRT_MtlBlend : public CRT_Material
{
public:
	RT_DECLARE_DYNCREATE(CRT_MtlBlend, CRT_Material, NULL, "")
	CRT_MtlBlend()
	{
		m_bSyncActorFrame = true;
		m_mixAmount = 0.5;
		m_bUseMixAmount = false;
		m_bTexMatrix1 = false;
		m_bTexMatrix2 = false;
		m_bTexMatrixMix = false;
		m_uv1 = TEX_UV_BASE;
		m_uv2 = TEX_UV_BASE;
		m_uvMix = TEX_UV_BASE;
		m_strUv1 = "Base";
		m_strUv2 = "Base";
		m_strUvMix = "Base";
	}
	virtual ~CRT_MtlBlend() {}
	virtual CRT_ObjectInst *CreateInstance();
	virtual void GetRelatedRes(vector<SRelatedRes> &res);

	virtual long Serialize(RtArchive& ar)
	{
		CRT_Material::Serialize(ar);
		ar << m_tex1 << m_tex2 << m_texMix;
		ar << m_bUseMixAmount << m_mixAmount;
		ar << m_fo << m_defKey << m_keyVec;
		ar << m_bSyncActorFrame;
		ar << m_bTexMatrix1 << m_bTexMatrix2 << m_bTexMatrixMix;
		ar << m_uv1 << m_uv2 << m_uvMix;
		ar << m_strUv1 << m_strUv2 << m_strUvMix;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar.IsError() ? 0 : 1;
	}
	virtual void Output() {}

	string m_tex1;
	string m_tex2;
	string m_texMix;
    bool   m_bUseMixAmount;
	float  m_mixAmount;
	bool   m_bTexMatrix1,m_bTexMatrix2,m_bTexMatrixMix;
	long   m_uv1,m_uv2,m_uvMix;

	string m_strAlphaMode;
	string m_strUv1,m_strUv2,m_strUvMix;

	bool m_bSyncActorFrame;
	SFrameBufferOption m_fo;
	SMtlBlendKey m_defKey;
	keyframe<SMtlBlendKey> m_keyVec;

	
#ifdef ACTOR_EDITOR
	virtual void AttachPropGrid(void *p);
	virtual void AttachKeyGrid(int key,void *p);
	virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam);
	virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam);

	// CTimeViewHost Interface
	string m_tvName;
	virtual const char *TVGetName();
	virtual void TVOnNotify(ETV_OP op,long param1,long param2);
	virtual int  TVGetKeyNum();
	virtual int  TVGetKeyFrame(int key);
#endif
};

//
// material lib, the container of material
//
class CRT_MaterialLib : public CRT_ObjectData
{
public:
	RT_DECLARE_DYNCREATE(CRT_MaterialLib, CRT_ObjectData, NULL, "")
	CRT_MaterialLib();
	virtual ~CRT_MaterialLib();

	virtual CRT_ObjectInst *CreateInstance();
	virtual void GetRelatedRes(vector<SRelatedRes> &res);

	// RtObject interface
	virtual long Serialize(RtArchive& ar);
	
	int GetMtlNum();
	CRT_Material *GetMtlByIdx(int idx);
	CRT_Material *GetMtlByName(const char *name);
	bool AddMaterial(CRT_Material *mtl);
	void DelMaterial(const char *name);	
	void Output();
	
	vector<CRT_Material*> m_mtlVec;
	RtDict m_dict;

#ifdef ACTOR_EDITOR
	void AttachGrid(void *p);
	bool OnGridNotify(WPARAM wParam, LPARAM lParam);
	CRT_Material *GetSelectedMtl(void *p);
#endif
};

class CRT_Effect : public CRT_ObjectData
#ifdef ACTOR_EDITOR
	, public CTimeViewHost
#endif
{
public:
	RT_DECLARE_DYNCREATE(CRT_Effect, CRT_ObjectData, NULL, "")
	CRT_Effect() {}
	virtual ~CRT_Effect() {}

	virtual long Serialize(RtArchive& ar)
	{
		ar << m_name << m_dict;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar.IsError() ? 0 : 1;
	}
	virtual void Output() {}
	string& GetName() { return m_name; }
	void SetName(string &name) { m_name = name; }

	string m_name;
	RtDict m_dict;

#ifdef ACTOR_EDITOR
	virtual void AttachPropGrid(void *p) = 0;
	virtual void AttachKeyGrid(int key,void *p) = 0;
	virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam) = 0;
	virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam) = 0;
#endif
};

struct SEmitterKey
{
	bool  bEnable;
	float emiSpeed;
	SEmitterKey() 
	{
		bEnable = true;
		emiSpeed = 0;
	}
	void Interpolate(unsigned long frame,unsigned long f1,SEmitterKey &k1,
		unsigned long f2,SEmitterKey &k2) 
	{
		float b = ((float)frame - f1)/(f2 - f1); 
		emiSpeed = k1.emiSpeed + b*(k2.emiSpeed - k1.emiSpeed);
		bEnable = k1.bEnable;
	}
	friend RtArchive& operator<<(RtArchive &ar, SEmitterKey &k) 
	{
		ar << k.bEnable << k.emiSpeed;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar;
	}
};

const long FORCE_FIELD_VORTEX = 1;

class CRT_ForceField : public RtObject
{
	RT_DECLARE_DYNCREATE(CRT_ForceField, RtObject, NULL, "");
	
	long m_ffType;
	string m_ffName;
	virtual long Serialize(RtArchive &ar)
	{
		ar << m_ffType << m_ffName;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar.IsError() ? 0 : 1;
	}
};

class CRT_FFVortex : public CRT_ForceField
{
	RT_DECLARE_DYNCREATE(CRT_FFVortex, CRT_ForceField, NULL, "");
	CRT_FFVortex()
	{
		m_ffType = FORCE_FIELD_VORTEX;
		m_radius = 50;
		m_speed = 100;
		m_centripetal = 1;
	}

	float m_radius;
	float m_speed;
	float m_centripetal;
	virtual long Serialize(RtArchive &ar)
	{
		CRT_ForceField::Serialize(ar);
		ar << m_radius << m_speed << m_centripetal;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar.IsError() ? 0 : 1;
	}
};

const char PARTICLE_BILLBOARD_NONE   = 0;
const char PARTICLE_BILLBOARD_FULL   = 1;
const char PARTICLE_BILLBOARD_X	     = 2;
const char PARTICLE_BILLBOARD_Y	     = 3;
const char PARTICLE_BILLBOARD_Z	     = 4;
const char PARTICLE_BILLBOARD_SPEEDX = 5;
const char PARTICLE_BILLBOARD_SPEEDY = 6;
const char PARTICLE_BILLBOARD_SPEEDZ = 7;

const long PARTICLE_GEN_MODE_NORMAL  = 0;
const long PARTICLE_GEN_MODE_SQUIRT  = 1;

const long PARTICLE_PATH_MODE_NORMAL = 0;
const long PARTICLE_PATH_MODE_DEST   = 1;
const long PARTICLE_PATH_MODE_PATH   = 2;

const long PARTICLE_GEOMETRY_BOARD		 = 0;
const long PARTICLE_GEOMETRY_MODEL		 = 1;
const long PARTICLE_GEOMETRY_MODEL_CLONE = 2;

const long PARTICLE_BOARDCENTER_X0Y0 = 0;
const long PARTICLE_BOARDCENTER_X0Y1 = 1;

//
// 产生模式:普通模式,爆裂模式
// 运动模式:速度模式,目标点模式,路径模式
// 力场:单向力场,向心力
//
class CRT_EffectEmitter : public CRT_Effect
{
	RT_DECLARE_DYNCREATE(CRT_EffectEmitter, CRT_Effect, NULL, "");
	CRT_EffectEmitter();
	virtual ~CRT_EffectEmitter();

	virtual CRT_ObjectInst *CreateInstance();
	virtual bool CopyFrom(CRT_ObjectData *from);
	virtual void GetRelatedRes(vector<SRelatedRes> &res);

	// from RtObject
	virtual long Serialize(RtArchive& ar);

	keyframe<SEmitterKey> m_keyVec;
	string     m_strGenMode;
	string     m_strGeometryMode;
	string     m_strPathMode;

	string     m_strAlphaMode;	
	string	   m_strBoardMode;
	string     m_strBoardCenter;

	long       m_genMode;
	long       m_geometryMode;
	long       m_pathMode;

	long       m_alphaMode;
	long       m_boardMode;
	
	bool       m_autoDeleteModelPar;
	bool       m_bModelLoop;
	string     m_model;
	string     m_modelPose;
	
    // base info
	long  m_parLife;			// particle lift,use ms
	float m_emiSpeed;			// generate how many particle per second

	// render 
	bool	   m_bAlwaysFront;

	// size
	RtgVertex3 m_sizeMin,m_sizeMax;
	RtgVertex3 m_emiSizeMin,m_emiSizeMax;
	long       m_boardCenter;

	// anim info
	float m_midVal;
	float m_opaqueBeg;
	float m_opaqueMid;
	float m_opaqueEnd;
	RtgVertex3 m_colorBeg;
	RtgVertex3 m_colorMid;
	RtgVertex3 m_colorEnd;
	long m_texBeg;
	long m_texMid;
	long m_texEnd;
	RtgVertex3 m_scaleBeg;
	RtgVertex3 m_scaleMid;
	RtgVertex3 m_scaleEnd;
	RtgVertex3 m_rotateBeg;
	RtgVertex3 m_rotateMid;
	RtgVertex3 m_rotateEnd;

	vector<CRT_ForceField*> m_forceVec;

	// texture info
	string	   m_texture;
	long       m_texUSub;
	long       m_texVSub;
	bool	   m_bTexSub;
	bool	   m_bTexRand;

	string       m_parent;
	// RtgMatrix12 *m_parentMat;
	long         m_type;
	long		 m_boardType;

	//
	// particle generate mode
	//

	
	//
	// particle path mode
	//

	// normal
	RtgVertex3 m_accMin,m_accMax;
	RtgVertex3 m_speedMin,m_speedMax;
	bool	   m_bUseParentDir;
	bool       m_bUseParentCoor;

	// dest
	string m_dest;
	long   m_destTime;
	long   m_destSpeedRand;

	// path
	string m_pathBone;

#ifdef ACTOR_EDITOR
	virtual void AttachPropGrid(void *p);
	virtual void AttachKeyGrid(int key,void *p);
	virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam);
	virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam);

	// CTimeViewHost Interface
	string m_tvName;
	virtual const char *TVGetName();
	virtual void TVOnNotify(ETV_OP op,long param1,long param2);
	virtual int TVGetKeyNum();
	virtual int TVGetKeyFrame(int key);
#endif
};

struct SRibbonKey
{
	long fadeOutFrame,endFrame; 
	long life,fadeIn,fadeOut;
	float visible;
	float mid;
	RtgVertex3 colorBeg;
	RtgVertex3 colorMid;
	RtgVertex3 colorEnd;
	RtgVertex3 color;
	string m_strAlphaMode;
	long m_alphaMode;
	SRibbonKey()
	{
		m_alphaMode = ALPHA_ADD;
		m_strAlphaMode = "Add";
		fadeOutFrame = 0;
		endFrame = 0;
		life    = 100;
		fadeIn  = 100;
		fadeOut = 100;
		visible = 1;
		color.Set(1,1,1);
		mid = 0.5;
		colorBeg.Set(1,1,1);
		colorMid.Set(1,1,1);
		colorEnd.Set(1,1,1);
	}
	void Interpolate(unsigned long frame,unsigned long f1,SRibbonKey &k1,
		unsigned long f2,SRibbonKey &k2)
	{
		// float b = ((float)frame - f1)/(f2 - f1); 
		// interval = k1.interval;
	}
	friend RtArchive& operator<<(RtArchive &ar, SRibbonKey &k) 
	{
		if( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_1 && ar.IsLoading() && SSerialize::ms_bIsLoadingSkin == false )
		{
			long  nColor(0);
			ar << k.endFrame;
			ar << k.fadeIn << k.life << k.fadeOut;
			ar << k.fadeOutFrame;		
			ar << k.visible << nColor;
			ar << k.m_alphaMode;
			k.color = LongToVector3_Color( nColor );
			BEGIN_VERSION_CONVERT(0)
				long  nColorBeg(0),nColorEnd(0),nColorMid(0);			
			ar << k.mid << nColorBeg << nColorEnd << nColorMid;
			k.colorBeg = LongToVector3_Color( nColorBeg );
			k.colorEnd = LongToVector3_Color( nColorEnd );
			k.colorMid = LongToVector3_Color( nColorMid );
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT
		}
		if( ar.IsStoring() && SSerialize::ms_bIsLoadingSkin == false )
		{
			ar << k.endFrame;
			ar << k.fadeIn << k.life << k.fadeOut;
			ar << k.fadeOutFrame;

			long  nColor(0);
			nColor = Vector3ToLong_Color( k.color );
			ar << k.visible << nColor;
			ar << k.m_alphaMode;	
			BEGIN_VERSION_CONVERT(0)
				long  nColorBeg(0),nColorEnd(0),nColorMid(0);
			nColorBeg = Vector3ToLong_Color( k.colorBeg );
			nColorEnd = Vector3ToLong_Color( k.colorEnd );
			nColorMid = Vector3ToLong_Color( k.colorMid );
			ar << k.mid << nColorBeg << nColorEnd << nColorMid;
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT
		}
		if( ( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_0 && ar.IsLoading() )  || SSerialize::ms_bIsLoadingSkin )
		{
			ar << k.endFrame;
			ar << k.fadeIn << k.life << k.fadeOut;
			ar << k.fadeOutFrame;
			ar << k.visible << k.color;
			ar << k.m_strAlphaMode << k.m_alphaMode;	
			BEGIN_VERSION_CONVERT(0)
				ar << k.mid << k.colorBeg << k.colorEnd << k.colorMid;
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT
		}
		return ar;
	}
};

class CRT_EffectRibbon : public CRT_Effect
{
	RT_DECLARE_DYNCREATE(CRT_EffectRibbon, CRT_Effect, NULL, "");
	CRT_EffectRibbon() {
		m_color.Set(1,1,1);
		m_visible = 1;
		m_strAlphaMode1 = "Add";
		m_alphaMode1 = ALPHA_ADD;
		m_strAlphaMode2 = "Sub";
		m_alphaMode2 = ALPHA_SUB;

		m_bTexSub = false;
		m_bTexRand = false;
		m_texUSub = 1;
		m_texVSub = 1;
		m_texSpeed = 1;
	}
	virtual ~CRT_EffectRibbon() {}

	// from RtObject
	virtual long Serialize(RtArchive& ar)
	{
		CRT_Effect::Serialize(ar);		
		if( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_1 && ar.IsLoading() && SSerialize::ms_bIsLoadingSkin == false )
		{					
			ar << m_alphaMode1 << m_keyList << m_bZTest << m_tex1;
			ar << m_head << m_tail;
			long  nColor(0);		
			ar << m_visible << nColor;
			m_color = LongToVector3_Color( nColor );
			BEGIN_VERSION_CONVERT(0)
				ar << m_tex2 <<  m_alphaMode2;
			ar << m_bTexSub << m_bTexRand << m_texUSub << m_texVSub << m_texSpeed;
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT
		}
		if( ar.IsStoring() && SSerialize::ms_bIsLoadingSkin == false )
		{
				
			ar << m_alphaMode1 << m_keyList << m_bZTest << m_tex1;
			ar << m_head << m_tail;

			long  nColor(0);
			nColor = Vector3ToLong_Color( m_color );
			ar << m_visible << nColor;

			BEGIN_VERSION_CONVERT(0)
				ar << m_tex2 <<  m_alphaMode2;
			ar << m_bTexSub << m_bTexRand << m_texUSub << m_texVSub << m_texSpeed;
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT

		}
		if( ( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_0 && ar.IsLoading() )  || SSerialize::ms_bIsLoadingSkin )
		{			
			ar << m_strAlphaMode1;
			ar << m_alphaMode1 << m_keyList << m_bZTest << m_tex1;
			ar << m_head << m_tail;
			ar << m_visible << m_color;		
			BEGIN_VERSION_CONVERT(0)
				ar << m_tex2 << m_strAlphaMode2 << m_alphaMode2;
			ar << m_bTexSub << m_bTexRand << m_texUSub << m_texVSub << m_texSpeed;
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			END_VERSION_CONVERT
		}
		return ar.IsError() ? 0 : 1;
	}

	// from CRT_ObjectData
	virtual CRT_ObjectInst *CreateInstance();
	virtual bool CopyFrom(CRT_ObjectData *from);
	virtual void GetRelatedRes(vector<SRelatedRes> &res);

#ifdef ACTOR_EDITOR
	virtual void AttachPropGrid(void *p);
	virtual void AttachKeyGrid(int key,void *p);
	virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam);
	virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam);

	// CTimeViewHost Interface
	std::string m_tvName;
	virtual const char *TVGetName();
	virtual void TVOnNotify(ETV_OP op,long param1,long param2);
	virtual int  TVGetKeyNum();
	virtual int  TVGetKeyFrame(int key);
#endif

	string     m_strAlphaMode1;
	long	   m_alphaMode1;
	string     m_strAlphaMode2;
	long	   m_alphaMode2;
	bool	   m_bZTest;

	long       m_texUSub;
	long       m_texVSub;
	bool	   m_bTexSub;
	bool	   m_bTexRand;
	long       m_texSpeed;

	string     m_head;
	string     m_tail;
	string     m_tex1;
	string     m_tex2;
	float      m_visible;
	RtgVertex3 m_color;

	keyframe<SRibbonKey> m_keyList;
};

struct SBlurKey
{
	bool bEnable;
	friend RtArchive& operator<<(RtArchive &ar, SBlurKey &k) 
	{
		ar << k.bEnable;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar;
	}
	void Interpolate(unsigned long frame,unsigned long f1,SBlurKey &k1,
		unsigned long f2,SBlurKey &k2)
	{
		bEnable = k1.bEnable;
	}
};

class CRT_EffectBlur: public CRT_Effect
{
	RT_DECLARE_DYNCREATE(CRT_EffectBlur, CRT_Effect, NULL, "");
	CRT_EffectBlur() 
	{
		m_bAlwaysFront = false;			
		m_blurNum = 5;
		m_life = 250;
		m_blurInterval = 50;
		m_visibleBeg = 0.5;
		m_visibleEnd = 0.1;
	}
	virtual ~CRT_EffectBlur() {

	}
	virtual CRT_ObjectInst *CreateInstance();
	virtual bool CopyFrom(CRT_ObjectData *from);
	virtual void GetRelatedRes(vector<SRelatedRes> &res);

	// from RtObject
	virtual long Serialize(RtArchive& ar) 
	{
		CRT_Effect::Serialize(ar);
		ar << m_bAlwaysFront << m_blurNum << m_blurInterval;
		ar << m_visibleBeg << m_visibleEnd << m_blurModel;
		ar << m_keyVec;
		ar << m_life;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar.IsError() ? 0 : 1;
	}

#ifdef ACTOR_EDITOR
	virtual void AttachPropGrid(void *p);
	virtual void AttachKeyGrid(int key,void *p); 
	virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam);
	virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam);

	// CTimeViewHost Interface
	std::string m_tvName;
	virtual const char *TVGetName() { 
		m_tvName = "EffectBlur";
		m_tvName += " (";
		m_tvName += m_name;
		m_tvName += " )";
		return m_tvName.c_str();
	}
	virtual void TVOnNotify(ETV_OP op,long param1,long param2);
	virtual int TVGetKeyNum();
	virtual int TVGetKeyFrame(int key); 
#endif

	bool  m_bAlwaysFront;			
	long  m_blurNum;
	long  m_blurInterval;
	long  m_life;
	float m_visibleBeg;
	float m_visibleEnd;
	string m_blurModel;
	keyframe<SBlurKey> m_keyVec;
};

struct SProjectorKey
{
	SRenderOption ro;
	SRenderStage  stage;
	RtgVertex3    color;
	
	SProjectorKey() {
		color.Set(1,1,1);
	}
	void Interpolate(unsigned long frame, unsigned long f1, SProjectorKey &k1,
		unsigned long f2, SProjectorKey &k2)
	{
		float b = ((float)frame - f1)/(f2 - f1); 
		color = k1.color + b*(k2.color - k1.color);
		ro.Interpolate(frame, f1, k1.ro, f2, k2.ro);
		stage.Interpolate(frame,f1,k1.stage,f2,k2.stage);
	}
	friend RtArchive& operator<<(RtArchive &ar, SProjectorKey &k) 
	{
		ar << k.ro << k.stage;
		ar << k.color;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar;
	}
};

class CRT_EffectProjector : public CRT_Effect
{
	RT_DECLARE_DYNCREATE(CRT_EffectProjector, CRT_Effect, NULL, "");
	CRT_EffectProjector() {
		m_fov = 45;
		m_aspect = 1;
		m_near = 20;
		m_far = 200;
		m_offsetPos.Zero();
		m_offsetRotate.Zero();
		m_bClipTerrain = true;
		m_bClipStaticActor  = false;
		m_bClipDynamicActor = false;
		m_bTexClampMode = false;
		m_bStaticProjector = false;
		m_bCullBackFaces = true;
		m_bProjectSelf = true;
		m_fo.alphaMode = ALPHA_ADD;
		m_fo.strAlphaMode = "Add";
	} 
	virtual ~CRT_EffectProjector() {}

	virtual CRT_ObjectInst *CreateInstance();
	virtual bool CopyFrom(CRT_ObjectData *from);
	virtual void GetRelatedRes(vector<SRelatedRes> &res);
	virtual long Serialize(RtArchive& ar) 
	{
		CRT_Effect::Serialize(ar);
		ar << m_fov << m_aspect << m_near << m_far;
		ar << m_parent << m_offsetPos << m_offsetRotate;
		ar << m_bClipTerrain << m_bClipStaticActor << m_bClipDynamicActor;
		ar << m_bClipAlphaObject;
		ar << m_fo << m_tex << m_keyVec;
		ar << m_defKey;
		ar << m_bTexClampMode << m_bStaticProjector << m_bCullBackFaces;
		ar << m_bProjectSelf << m_texFadeOut;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return ar.IsError() ? 0 : 1;
	}

	float m_fov,m_aspect,m_near,m_far;
	RtgVertex3 m_offsetPos,m_offsetRotate;
	string m_parent;
    
    bool m_bClipTerrain;
	bool m_bClipStaticActor;
	bool m_bClipDynamicActor;
	bool m_bClipAlphaObject;

	SProjectorKey m_defKey;
	SFrameBufferOption m_fo;
	string m_tex;
	keyframe<SProjectorKey> m_keyVec;
	bool m_bTexClampMode;
	bool m_bStaticProjector;	
	bool m_bCullBackFaces;
	bool m_bProjectSelf;
	string m_texFadeOut;

#ifdef ACTOR_EDITOR
	virtual void AttachPropGrid(void *p);
	virtual void AttachKeyGrid(int key,void *p); 
	virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam);
	virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam);

	// CTimeViewHost Interface
	std::string m_tvName;
	virtual const char *TVGetName() { 
		m_tvName = "EffectProjector";
		m_tvName += " (";
		m_tvName += m_name;
		m_tvName += " )";
		return m_tvName.c_str();
	}
	virtual void TVOnNotify(ETV_OP op,long param1,long param2);
	virtual int TVGetKeyNum();
	virtual int TVGetKeyFrame(int key); 
#endif
};

//
// dymanic effect, material
//
// clip terrain, procedural map, project ...
//
#endif
