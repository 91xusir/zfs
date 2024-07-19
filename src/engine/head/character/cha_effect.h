
//////////////////////////////////////////////////////////////////////////
//
//   FileName : cha_effect.h
//     Author : zxy
// CreateTime : 2010.04.01
//       Desc : refactor
//
//////////////////////////////////////////////////////////////////////////

#ifndef __CHA_EFFECT_H__
#define __CHA_EFFECT_H__

const long BILLBOARD_NONE = 0;
const long BILLBOARD_FULL = 1;
const long BILLBOARD_X	  = 2;
const long BILLBOARD_Y	  = 3;
const long BILLBOARD_Z	  = 4;

const long PAR_TYPE_DIF = 0;
const long PAR_TYPE_COM = 1;

class CRT_EffectEmitter : public CRT_EffectBase
{
	RT_DECLARE_DYNCREATE(CRT_EffectEmitter, CRT_EffectBase, NULL, "");

    struct SParticle
    {	
        void Init() 
        {
            Life = 0;
            TexIdx = 1;
            Pos.Zero();
            Speed.Zero();
            Acc.Zero();
            Size = 5.0;
            Opaque = 0;
            Scale = 1.0;
            Color.Set(1.0,1.0,1.0);
            Next = NULL;
        }
        long  Life;
        RtgVertex3 Speed;
        RtgVertex3 Acc;
        RtgVertex3 Pos;
        float Size;
        float Opaque;
        float Scale;
        RtgVertex3 Color;
        long  TexIdx;
        SParticle *Next;
    };

    struct SEmitterKey
    {
        bool  bEnable;
        float EmiSpeed;
        SEmitterKey()
        {
            bEnable = true;
            EmiSpeed = 0;
        }
        void Interpolate(unsigned long frame,unsigned long f1,SEmitterKey &k1,
            unsigned long f2,SEmitterKey &k2)
        {
            float b = ((float)frame - f1)/(f2 - f1); 
            EmiSpeed = k1.EmiSpeed + b*(k2.EmiSpeed - k1.EmiSpeed);
            bEnable = k1.bEnable;
        }
        friend RtArchive& operator<<(RtArchive &Ar, SEmitterKey &k) 
        {
            Ar << k.bEnable << k.EmiSpeed;
            BEGIN_VERSION_CONVERT(1)
            END_VERSION_CONVERT
            return Ar;
        }
    };

public :

	CRT_EffectEmitter();

	virtual ~CRT_EffectEmitter()
    {
		CM_MEMUNPROTECTOR(m_texPath);
        Destroy();
    }

    virtual long Serialize(RtArchive& Ar);
    virtual void OnLoadFinish();
    virtual void Reset();
    virtual bool Create();
    virtual void Destroy();

	virtual void Tick(float deltaMill);
	virtual void Render(RtgDevice *dev,RTGRenderMask mask);
	virtual void UseFrame(unsigned long frame);

    void ConvertOption();

    virtual CRT_PoolObject* NewObject() 
    {
        CRT_EffectEmitter *t = (CRT_EffectEmitter*)RtcGetActorManager()->NewObject(RT_RUNTIME_CLASS(CRT_EffectEmitter));
        *t = *this;
        t->m_poType = Pool_Type_Memory;
        t->m_delta = 0;
        t->m_pActive = NULL;
        t->m_pDead = NULL;
        t->m_actor = NULL;
        return t;
    }

    SParticle* NewParticle()
    {
        SParticle* p = NULL;

        if (m_pDead)
        {
            p = m_pDead;
            m_pDead = m_pDead->Next;
        }
        else
        {
            p = RT_NEW SParticle;
        }

        p->Init();
        return p;
    }

    void DelParticle(SParticle* p)
    {
        p->Next = m_pDead;
        m_pDead = p;
    }

    virtual bool RequestTick() 
    { 
        return true; 
    }

    virtual bool RequestUseFrame() 
    { 
        return m_keyList.Size()!= 0; 
    }

    virtual bool ResourceReady()
    {
        if (!m_ro.ResourceReady())
            return false;
        return true;
    }

public :
	keyframe<SEmitterKey> m_keyList;
	
    long        m_parLife;
	float       m_emiSpeed;
    float	    m_sizeMin;
    float       m_sizeMax;
	RtgVertex3  m_emiSizeMin;
    RtgVertex3  m_emiSizeMax;
	long	    m_alphaType;
	string      m_alphaOpt;	
	bool	    m_bAlwaysFront;

	float       m_midVal;
	float       m_opaqueBeg;
	float       m_opaqueMid;
	float       m_opaqueEnd;
	RtgVertex3  m_colorBeg;
	RtgVertex3  m_colorMid;
	RtgVertex3  m_colorEnd;
	long        m_texBeg;
	long        m_texMid;
	long        m_texEnd;
	float       m_scaleBeg;
	float       m_scaleMid;
	float       m_scaleEnd;

	string	    m_texture;
    //char        m_texPath[Text_File_Path_Size];
	CM_MEMDEF(m_texPath, Text_File_Path_Size)
	long        m_texUSub;
	long        m_texVSub;
	bool	    m_bTexSub;
	bool	    m_bTexRand;

	string       m_parent;
	string      m_typeOpt;
	long        m_type;
	long        m_boardType;
	string      m_boardOpt;

	RtgVertex3  m_accMin;
    RtgVertex3  m_accMax;
	RtgVertex3  m_speedMin;
    RtgVertex3  m_speedMax;
	bool	    m_bSquirt;
	bool	    m_bUseParentRotate;
	bool        m_bUseParentCoor;

	string      m_dest;
	long        m_destTime;
	long        m_destSpeedRand;

	float        m_delta;
	RtgShader    m_ro;
	SParticle*   m_pActive;
	SParticle*   m_pDead;
	long         m_lastFrame;
	bool         m_rtEnable;
	long         m_rtEmiSpeed;
	long         m_rtSquirtNum;

#ifdef CHAR_VIEWER
	virtual void AttachPropGrid(void *p);
	virtual void AttachKeyGrid(int key,void *p);
	virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam);
	virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam);
	std::string m_tvName;
	virtual const char *TVGetName() { 
		m_tvName = "EffectEmitter";
		m_tvName += " (";
		m_tvName += m_eftName;
		m_tvName += " )";
		return m_tvName.c_str();
	}
	virtual void TVOnNotify(ETV_OP op,long param1,long param2);
	virtual int TVGetKeyNum();
	virtual int TVGetKeyFrame(int key);
#endif
};

class CRT_EffectRibbon : public CRT_EffectBase
{
	RT_DECLARE_DYNCREATE(CRT_EffectRibbon, CRT_EffectBase, NULL, "");

    struct SPiece
    {	
        void Init()
        {
            Life = 0;
            Visible = 1;
            Color.Set(1.0,1.0,1.0);
            Next = NULL;
        }

        long  Life;
        RtgVertex3 V[6];
        float Visible;
        RtgVertex3 Color;
        SPiece *Next;
    };

    struct SRibbonKey
    {
        bool  bEnable;
        SRibbonKey()
        {
            bEnable = true;
        }
        void Interpolate(unsigned long frame,unsigned long f1,SRibbonKey &k1,
            unsigned long f2,SRibbonKey &k2)
        {
            bEnable = k1.bEnable;
        }

        friend RtArchive& operator<<(RtArchive &Ar, SRibbonKey &k) 
        {
            Ar << k.bEnable;
            BEGIN_VERSION_CONVERT(1)
            END_VERSION_CONVERT
            return Ar;
        }
    };

public :
	CRT_EffectRibbon();

	virtual ~CRT_EffectRibbon()
    {
        Destroy();
    }

    virtual long Serialize(RtArchive& Ar);
    virtual void OnLoadFinish();
    virtual bool Create();
    virtual void Destroy();

	virtual void Tick(float deltaMill);
	virtual void Render(RtgDevice *dev,RTGRenderMask mask);
	virtual void UseFrame(unsigned long frame);

    void ConvertOption();

    virtual CRT_PoolObject *NewObject()
    {
        CRT_EffectRibbon *t = (CRT_EffectRibbon*)RtcGetActorManager()->NewObject(RT_RUNTIME_CLASS(CRT_EffectRibbon));
        *t = *this;
        t->m_poType = Pool_Type_Memory;
        t->m_delta = 0;
        t->m_actor = NULL;
        t->m_list.clear();
        return t;
    }

    virtual bool RequestTick() 
    { 
        return true; 
    }
    
    virtual bool RequestUseFrame() 
    { 
        return m_keyList.Size() != 0; 
    }

	void DelPiece(SPiece* p) 
    { 
        DEL_ONE(p); 
    }
	
    SPiece* NewPiece() 
    { 
        SPiece *p = RT_NEW SPiece; 
        p->Init(); 
        return p;
    }
	
public :

    list<SPiece*> m_list;
	SPiece*       m_pActive;
	SPiece*       m_pDead;

	RtgShader  m_ro;
	long	   m_alphaType;
	string     m_alphaOpt;	
	bool	   m_bAlwaysFront;

	string     m_head;
	string     m_mid;
	string     m_tail;

	RtgVertex3 m_srcHeadPos;
	RtgVertex3 m_srcMidPos;
	RtgVertex3 m_srcTailPos;
	bool       m_bSrcValid;

	long       m_life;
	long       m_interval;
	float m_opaqueBeg;
	float m_opaqueEnd;
	
	float m_scaleBeg;
	float m_scaleEnd;

	RtgVertex3 m_colorBeg;
	RtgVertex3 m_colorEnd;
	keyframe<SRibbonKey> m_keyList;

	float m_delta;
	bool m_rtEnable;

#ifdef CHAR_VIEWER
    virtual void AttachPropGrid(void *p);
    virtual void AttachKeyGrid(int key,void *p);
    virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam);
    virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam);

    // CTimeViewHost Interface
    std::string m_tvName;
    virtual const char *TVGetName() { 
        m_tvName = "EffectRibbon";
        m_tvName += " (";
        m_tvName += m_eftName;
        m_tvName += " )";
        return m_tvName.c_str();
    }
    virtual void TVOnNotify(ETV_OP op,long param1,long param2);
    virtual int TVGetKeyNum();
    virtual int TVGetKeyFrame(int key);
#endif
};

class CRT_Effect_Dynamic_Ribbon : public CRT_EffectBase
{
    RT_DECLARE_DYNCREATE(CRT_Effect_Dynamic_Ribbon, CRT_EffectBase, NULL, "");

    struct Node
    {
        RtgVertex3 v0;
        RtgVertex3 v1;
        float life;
        Node* next;

        Node(const RtgVertex3& _v0 = RtgVertex3(0, 0, 0), 
             const RtgVertex3& _v1 = RtgVertex3(0, 0, 0), 
             UINT _life = 0, 
             Node* _n = NULL)
            : v0(_v0), v1(_v1), life(_life), next(_n)
        {

        }
    };

public :

    CRT_Effect_Dynamic_Ribbon();

    virtual ~CRT_Effect_Dynamic_Ribbon()
    {
		CM_MEMUNPROTECTOR(m_texPath)
        Destroy();
    }

    virtual void OnLoadFinish();
    virtual bool Create();
    virtual void Destroy();

    virtual void UseFrame(unsigned long frame);
    virtual void Clear();
    virtual void Render(RtgDevice *dev,RTGRenderMask mask);

    virtual long Serialize(RtArchive& Ar)
    {
        CRT_EffectBase::Serialize(Ar);
        Ar<<m_alphaOpt
          <<m_bAlwaysFront 
          <<m_Color
          <<m_texture 
          <<m_alphaType 
          <<m_fSize 
          <<m_fLife 
          <<m_linkBone;
        return Ar.IsError() ? 0 : 1;
    }

    virtual CRT_PoolObject *NewObject()
    {
        CRT_Effect_Dynamic_Ribbon *t = 
            (CRT_Effect_Dynamic_Ribbon*)RtcGetActorManager()->NewObject(RT_RUNTIME_CLASS(CRT_Effect_Dynamic_Ribbon));
        *t = *this;
        t->m_poType = Pool_Type_Memory;
        t->m_actor = NULL;
        t->m_pNodeHead = 0;
        t->m_pNodeTail = 0;
        t->m_nNodeNum = 0;
        t->m_linkBone = m_linkBone;
        return t;
    }

    virtual bool RequestTick()
    {
        return true;
    }

    virtual void Tick(float deltaMill) 
    {
    }

    virtual bool RequestUseFrame()
    {
        return true;
    }

	virtual void ResetRibbonDynamic();

public :
    float m_fSize;
    float m_fLife;
    string     m_texture;
    //char       m_texPath[Text_File_Path_Size];
	CM_MEMDEF(m_texPath, Text_File_Path_Size)
    long	   m_alphaType;
    string     m_alphaOpt;	
    bool	   m_bAlwaysFront;
    RtgShader  m_ro;
    RtgVertex3 m_Color;
    string     m_linkBone;
    
    unsigned long m_LastFrame;
    Node* m_pNodeHead;
    Node* m_pNodeTail;
    int   m_nNodeNum;


public :

#ifdef CHAR_VIEWER


    virtual void AttachPropGrid(void *p);
    virtual void AttachKeyGrid(int key,void *p) 
    {

    }
    virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam) ;
    virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam) 
    {
        return true;
    }

    // CTimeViewHost Interface
    std::string m_tvName;
    virtual const char *TVGetName() { 
        m_tvName = "EffectRibbon2";
        m_tvName += " (";
        m_tvName += m_eftName;
        m_tvName += " )";
        return m_tvName.c_str();
    }
    virtual void TVOnNotify(ETV_OP op,long param1,long param2)
    {
        
    }
    virtual int  TVGetKeyNum()
    {
        return 0;
    }
    virtual int  TVGetKeyFrame(int key)
    {
        return 0;
    }

#endif
};

class CRT_EffectRibbon2 : public CRT_EffectBase
{
	RT_DECLARE_DYNCREATE(CRT_EffectRibbon2, CRT_EffectBase, NULL, "");

    struct SRibbonKey2
    {
        long fadeOutFrame,endFrame; 
        long life,fadeIn,fadeOut;
        
        SRibbonKey2()
        {
            fadeOutFrame = 0;
            endFrame = 0;
            life    = 1000;
            fadeIn  = 500;
            fadeOut = 1000;
        }
        
        void Interpolate(unsigned long frame,unsigned long f1,SRibbonKey2 &k1,
            unsigned long f2,SRibbonKey2 &k2)
        {
        }

        friend RtArchive& operator<<(RtArchive &Ar, SRibbonKey2 &k) 
        {
            Ar << k.endFrame;
            BEGIN_VERSION_CONVERT(0)
            Ar << k.fadeIn << k.life << k.fadeOut;
            BEGIN_VERSION_CONVERT(0)
            Ar << k.fadeOutFrame;
            BEGIN_VERSION_CONVERT(1)
            END_VERSION_CONVERT
            END_VERSION_CONVERT
            END_VERSION_CONVERT
            return Ar;
        }
    };

    struct SPiece
    {	
        SRibbonKey2 keyFrame;
        int begFrame;
        int endFrame;

        void Init() 
        {

        }
        vector<RtgVertex3> verList;
        vector<float> uvList;
    };

public :

	CRT_EffectRibbon2();
	
    virtual ~CRT_EffectRibbon2()
    {
		CM_MEMUNPROTECTOR(m_texPath)
        Destroy();
    }

    virtual void OnLoadFinish();
    virtual bool Create();
    virtual void Destroy();

	virtual void Tick(float deltaMill);
	virtual void Render(RtgDevice *dev,RTGRenderMask mask);
	virtual void UseFrame(unsigned long frame);
    void GenTri(int frame, SPiece *p);

	void ConvertOption();

    long Serialize(RtArchive& Ar)
    {
        CRT_EffectBase::Serialize(Ar);
        Ar<<m_alphaOpt 
          <<m_keyList
          <<m_bAlwaysFront
          <<m_pieceNum
          <<m_texture
          <<m_head
          <<m_tail;

        BEGIN_VERSION_CONVERT(1)
        END_VERSION_CONVERT
        return Ar.IsError() ? 0 : 1;
    }

    virtual CRT_PoolObject *NewObject() 
    {
        CRT_EffectRibbon2 *t = (CRT_EffectRibbon2*)RtcGetActorManager()->NewObject(RT_RUNTIME_CLASS(CRT_EffectRibbon2));
        *t = *this;
        t->m_poType = Pool_Type_Memory;
        t->m_actor = NULL;
        t->m_list.clear();
        t->m_lastFrame = 0;
        t->m_ribbonPerFrame = 0;
        t->m_headBone = NULL;
        t->m_tailBone = NULL;
        return t;
    }

    virtual bool RequestTick() 
    { 
        return true; 
    }
    
    virtual bool RequestUseFrame() 
    { 
        return m_keyList.Size() != 0; 
    }

	void DelPiece(SPiece *p) 
    { 
        DEL_ONE(p); 
    }
	
    SPiece* NewPiece() 
    { 
        SPiece *p = RT_NEW SPiece; 
        p->Init(); 
        return p;
    }

public :
	list<SPiece*> m_list;
	
	RtgShader  m_ro;
	long	   m_alphaType;
	string     m_alphaOpt;	
	bool	   m_bAlwaysFront;

	SRT_Bone  *m_headBone;
	SRT_Bone  *m_tailBone;
	string     m_head;
	string     m_tail;
	string     m_texture;
    //char       m_texPath[Text_File_Path_Size];
	CM_MEMDEF(m_texPath, Text_File_Path_Size)
	long       m_pieceNum;

	keyframe<SRibbonKey2> m_keyList;

	// runtime var
	int m_lastFrame;
	int m_ribbonPerFrame;


#ifdef CHAR_VIEWER
    virtual void AttachPropGrid(void *p);
    virtual void AttachKeyGrid(int key,void *p);
    virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam);
    virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam);

    // CTimeViewHost Interface
    std::string m_tvName;
    virtual const char *TVGetName() { 
        m_tvName = "EffectRibbon2";
        m_tvName += " (";
        m_tvName += m_eftName;
        m_tvName += " )";
        return m_tvName.c_str();
    }
    virtual void TVOnNotify(ETV_OP op,long param1,long param2);
    virtual int  TVGetKeyNum();
    virtual int  TVGetKeyFrame(int key);
#endif
};

class CRT_EffectBlur: public CRT_EffectBase
{
public :
    struct SFrame
    {
        RtgMatrix16 WorldMat;
    };

    struct SBlurKey
    {
        bool bEnable;
        
        friend RtArchive& operator<<(RtArchive& Ar, SBlurKey& k) 
        {
            Ar<<k.bEnable;
            BEGIN_VERSION_CONVERT(1)
            END_VERSION_CONVERT
            return Ar;
        }

        void Interpolate(unsigned long frame, unsigned long f1, SBlurKey& k1,
            unsigned long f2, SBlurKey& k2)
        {
            bEnable = k1.bEnable;
        }
    };
 
public :
    RT_DECLARE_DYNCREATE(CRT_EffectBlur, CRT_EffectBase, NULL, "");
    CRT_EffectBlur();
    virtual ~CRT_EffectBlur();

    virtual long Serialize(RtArchive& Ar)
    {
        Ar<<m_blurNum<<m_blurInterval<<m_visible<<m_bAlwaysFront<<m_keyList;

        BEGIN_VERSION_CONVERT(0)
        CRT_EffectBase::Serialize(Ar);
        BEGIN_VERSION_CONVERT(1)
        END_VERSION_CONVERT
        END_VERSION_CONVERT
        return Ar.IsError() ? 0 : 1;
    }

    virtual CRT_PoolObject *NewObject() 
    {
        CRT_EffectBlur *t = (CRT_EffectBlur*)RtcGetActorManager()->NewObject(RT_RUNTIME_CLASS(CRT_EffectBlur));
        *t = *this;
        t->m_poType = Pool_Type_Memory;
        t->m_delta = 0;
        t->m_actor = NULL;
        return t;
    }

    virtual void OnLoadFinish()
    {
        CRT_EffectBase::OnLoadFinish();
        m_frameList.clear();	
    }

    virtual bool Create()
    {
        return true;
    }

    virtual void Destroy()
    {
    }

    virtual void Tick(float deltaMill)
    {
        if (!m_actor || !m_bEnable) 
            return;

        m_frameDelta += deltaMill;

        if (m_frameDelta > m_blurInterval && m_blurNum > 0)
        {
            m_frameDelta = 0;
            SFrame tmp;
            if(m_frameList.size()>=m_blurNum)
                m_frameList.pop_front();
            tmp.WorldMat.Set4X3Matrix(m_actor->GetWorldMatrix()->m_m);
            m_frameList.push_back(tmp);
        }
    }

    virtual void UseFrame(unsigned long frame)
    {
        SBlurKey key;
        if (!m_keyList.GetKeyFrame(frame, key))
            return;
        Update(key);
    }

    void Update(SBlurKey &key)
    {
        m_bEnable = key.bEnable;
    }

    virtual void Render(RtgDevice *dev,RTGRenderMask mask);
    virtual bool RequestTick() { return true; }
    virtual bool RequestUseFrame() { return m_keyList.Size() != 0; }


public :

    bool  m_bAlwaysFront;
    long  m_blurNum;
    long  m_blurInterval;
    float m_visible;
    keyframe<SBlurKey> m_keyList;

    long m_delta;
    long m_frameDelta;
    bool m_bEnable;
    list<SFrame> m_frameList;

#ifdef CHAR_VIEWER
    virtual void AttachPropGrid(void *p);
    virtual void AttachKeyGrid(int key,void *p); 
    virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam);
    virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam);
    std::string m_tvName;
    virtual const char *TVGetName() { 
        m_tvName = "EffectBlur";
        m_tvName += " (";
        m_tvName += m_eftName;
        m_tvName += " )";
        return m_tvName.c_str();
    }
    virtual void TVOnNotify(ETV_OP op,long param1,long param2);
    virtual int TVGetKeyNum();
    virtual int TVGetKeyFrame(int key); 
#endif

};

#endif
