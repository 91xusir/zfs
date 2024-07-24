
//////////////////////////////////////////////////////////////////////////
//
//   FileName : cha_mtl.h
//     Author : zxy
// CreateTime : 2010.04.01
//       Desc : refactor
//
//////////////////////////////////////////////////////////////////////////

#ifndef __CHA_MTL_H__
#define __CHA_MTL_H__

void SetFogWithAdd(RtgDevice *dev);
void RestoreFog(RtgDevice *dev);

class CRT_MtlStandard : public CRT_Material
{
	RT_DECLARE_DYNCREATE(CRT_MtlStandard, CRT_Material, NULL, "");

    struct SStandardKey
    {
        RtgVertex3  Rotate;
        RtgVertex3  Translate;
        RtgVertex3  Scale;
        float       Visible;
        RtgVertex4  Amb,Dif,Emi;

        SStandardKey()
        {
            Rotate.Zero();
            Translate.Zero();
            Scale.Set(1.0f,1.0f,1.0f);
            Visible = 1.0f;
            Amb.Set(1.0f,1.0f,1.0f,1.0);
            Dif.Set(1.0f,1.0f,1.0f,1.0);
            Emi.Set(0.0f,0.0f,0.0f,1.0);
        }

        void Interpolate(unsigned long frame,unsigned long f1,SStandardKey &k1,
            unsigned long f2,SStandardKey &k2)
        {
            float b = ((float)frame - f1)/(f2 - f1); 
            Rotate = k1.Rotate + b*(k2.Rotate - k1.Rotate);
            Translate = k1.Translate + b*(k2.Translate - k1.Translate);
            Scale = k1.Scale + b*(k2.Scale - k1.Scale);
            Visible = k1.Visible + b*(k2.Visible - k1.Visible);
            Amb = k1.Amb + b*(k2.Amb - k1.Amb);
            Dif = k1.Dif + b*(k2.Dif - k1.Dif);
            Emi = k1.Emi + b*(k2.Emi - k1.Emi);
        }

        friend RtArchive& operator<<(RtArchive &Ar, SStandardKey &k) 
        {
            Ar << k.Rotate << k.Translate << k.Scale << k.Visible;
            Ar << k.Amb << k.Dif << k.Emi;
            return Ar;
        }
    };

	CRT_MtlStandard();
	CRT_MtlStandard(const CRT_MtlStandard &object);
	virtual ~CRT_MtlStandard();

	virtual long Serialize(RtArchive& Ar);
    virtual bool Apply(RtgDevice* _Dev);
    virtual void Restore(RtgDevice* _Dev);
    virtual void OnLoadFinish();
    virtual bool Create();
    virtual void Destroy();
    virtual void Reset();

    void Update(SStandardKey &key);
    void UpdateVisible(bool force);
    void ConvertOption();

    virtual CRT_PoolObject* NewObject() 
    {
        CRT_MtlStandard* mtl = (CRT_MtlStandard*)RtcGetActorManager()->NewObject(RT_RUNTIME_CLASS(CRT_MtlStandard));
        mtl->m_name         = m_name;
        mtl->m_bUseFrame    = m_bUseFrame;
        mtl->m_bMask        = m_bMask;
        mtl->m_bTwoSide     = m_bTwoSide;
        mtl->m_bAlpha       = m_bAlpha;
        mtl->m_alphaType    = m_alphaType;
        mtl->m_alphaOpt     = m_alphaOpt;
        mtl->m_visible      = m_visible;
        mtl->m_amb          = m_amb;
        mtl->m_dif          = m_dif;
        mtl->m_emi          = m_emi;
        mtl->m_bSpec        = m_bSpec;
        mtl->m_spe          = m_spe;
        mtl->m_spePower     = m_spePower;
        mtl->m_texture      = m_texture;
        mtl->m_bTexMatrix   = m_bTexMatrix;
        mtl->m_baseRotate   = m_baseRotate;
        mtl->m_baseTranslate = m_baseTranslate;
        mtl->m_baseScale    = m_baseScale;
        mtl->m_keyList      = m_keyList;

        mtl->m_poType       = Pool_Type_Memory;
        mtl->m_delta        = 0;
        mtl->m_visibleGlobal = 1.0f;
        mtl->m_visibleLocal  = 1.0f;

        rt2_strncpy(mtl->m_texPath, m_texPath, Text_File_Path_Size);
        m_ro.Clone(&mtl->m_ro);

        return mtl;
    }

    virtual bool IsTransparent()
    {
        if (Abs(m_rtVisible - 1.0f) >= 0.01f || m_bAlpha)
            return true;
        return false;
    }

    virtual string& GetName() 
    { 
        return m_name; 
    }
    
    virtual void SetName(string &name) 
    { 
        m_name = name; 
    }

    virtual bool RequestTick()
    {
        return (m_keyList.Size() != 0);
    }

    // lyymark CRT_MtlStandard 主帧动画处理
    void Tick(float deltaMill)
    {
        unsigned long beg = m_keyList.GetBegFrame();
        unsigned long end = m_keyList.GetEndFrame();
        // 如果起始帧和结束帧相同，则无需处理
        if (beg == end)
            return;
        // 计算帧数
        unsigned long frameCount = static_cast<UINT>(m_delta / 33);
        unsigned long currentFrame = beg + (frameCount % (end - beg));
        // 更新当前帧
        UseFrame(currentFrame);
        // 更新累计时间
        m_delta += deltaMill;
    }

    bool RequestUseFrame()
    {
        return (m_keyList.Size() != 0);
    }

    void UseFrame(unsigned long frame)
    {
        SStandardKey key;
        int size = m_keyList.Size();
        if (!m_keyList.GetKeyFrame(frame, key))
            return;
        Update(key);
    }

    bool SetBaseColor(RtgVertex3 &color)
    {
        m_rtBaseEmi.x = color.x;
        m_rtBaseEmi.y = color.y;
        m_rtBaseEmi.z = color.z;
        return true;
    }

    bool GetBaseColor(RtgVertex3 &color)
    {
        color.x = m_rtBaseEmi.x;
        color.y = m_rtBaseEmi.y;
        color.z = m_rtBaseEmi.z;
        return true;
    }

    virtual void SetVisibleGlobal(float visible) 
    {
        m_visibleGlobal = visible;
        UpdateVisible(false);
    }

    virtual void SetVisibleLocal(float visible) 
    {
        m_visibleLocal = visible;
        UpdateVisible(false);
    }

    virtual bool RequestUpdateNormal() {
        return m_bSpec;
    }

	virtual bool GetGeometry(int& alphaMode, RtgTextItem*& texItem)
	{
		if (m_bMask) 
            alphaMode = ALPHA_COLOR_KEY;
		else if(!m_bAlpha) 
            alphaMode = ALPHA_NULL;
		else 
            alphaMode = m_alphaType;
        texItem = m_ro.Shaders[0].GetTexture();

		return true;
	}

    virtual int GetAlphaMdoe() 
    {
        if (m_bMask) 
            return ALPHA_COLOR_KEY;
        else if(!m_bAlpha) 
            return ALPHA_NULL;
        else 
            return m_alphaType;
    }

    virtual RtgTextItem* GetBaseText()
    {
        return m_ro.Shaders[0].GetTexture();
    }

    virtual string TextureName(int _index)
    {
        if (_index == 0)
            return m_texture;
        return "";
    }

    virtual bool ResourceReady()
    {
        return m_ro.ResourceReady();
    }

    virtual void GetBaseInfo(RtgVertex4 &amb,RtgVertex4 &dif,RtgVertex4 &emi) 
    {
        amb = m_amb;
        dif = m_dif;
        emi = m_emi;
    }

    virtual void SetBaseInfo(RtgVertex4 &amb,RtgVertex4 &dif,RtgVertex4 &emi) 
    {
        m_amb = amb;
        m_dif = dif;
        m_emi = emi;
    }


#ifdef CHAR_VIEWER
	virtual void AttachPropGrid(void *p);
	virtual void AttachKeyGrid(int key,void *p);
	virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam);
	virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam);
	
	// CTimeViewHost Interface
	virtual const char *TVGetName();
	virtual void TVOnNotify(ETV_OP op,long param1,long param2);
	virtual int TVGetKeyNum();
	virtual int TVGetKeyFrame(int key);
	virtual bool CopyFrom(CRT_Material* pMat);
#endif

	string      m_name;
	keyframe<SStandardKey> m_keyList;

	bool		m_bMask;
	bool		m_bTwoSide;
	bool		m_bAlpha;
	long		m_alphaType;
	float		m_visible;
	string      m_alphaOpt;		
	
	RtgVertex4  m_amb;
    RtgVertex4  m_dif;
    RtgVertex4  m_emi;
    RtgVertex4  m_spe;

	float       m_spePower;
    bool        m_bSpec;
	string      m_texture;
    //char        m_texPath[Text_File_Path_Size];
	CM_MEMDEF(m_texPath, Text_File_Path_Size)
	bool        m_bUseFrame;
	
	bool         m_bTexMatrix;
	RtgVertex3	 m_baseRotate;
	RtgVertex3	 m_baseTranslate;
	RtgVertex3   m_baseScale;

    float       m_visibleGlobal;
    float       m_visibleLocal;
	unsigned long m_delta;
	RtgShader   m_ro;
	RtgMatrix2D9 m_texMtx;

	RtgVertex4  m_rtAmb;
    RtgVertex4  m_rtDif;
    RtgVertex4  m_rtEmi;
    RtgVertex4  m_rtBaseEmi;
	float       m_rtVisible;
};

const long UV_BASE = 0;
const long UV_XY   = 1;
const long UV_YZ   = 2;
const long UV_ZX   = 3;
const long UV_CPOS = 4;
const long UV_CNOR = 5;
const long UV_CREL = 6;

const long OP_MODULATE = 0;
const long OP_MODULATE2X = 1;
const long OP_MODULATE4X = 2;

class CRT_MtlMu : public CRT_Material
{
	RT_DECLARE_DYNCREATE(CRT_MtlMu, CRT_Material, NULL, "")

    struct SMuKey
    {
        RtgVertex3  Rotate;
        RtgVertex3  Translate;
        RtgVertex3  Scale;
        RtgVertex3  Color;

        RtgVertex3  Rotate2;
        RtgVertex3  Translate2;
        RtgVertex3  Scale2;
        RtgVertex3  Color2;

        SMuKey()
        {
            Rotate.Zero();
            Translate.Zero();
            Scale.Set(1.0f,1.0f,1.0f);
            Color.Set(1.0f,1.0f,1.0f);

            Rotate2.Zero();
            Translate2.Zero();
            Scale2.Set(1.0f,1.0f,1.0f);
            Color2.Set(1.0f,1.0f,1.0f);
        }

        void Interpolate(unsigned long frame,unsigned long f1,SMuKey &k1,
            unsigned long f2,SMuKey &k2)
        {
            float b = ((float)frame - f1)/(f2 - f1); 
            Rotate    = k1.Rotate + b*(k2.Rotate - k1.Rotate);
            Translate = k1.Translate + b*(k2.Translate - k1.Translate);
            Scale	  = k1.Scale + b*(k2.Scale - k1.Scale);
            Color     = k1.Color + b*(k2.Color - k1.Color);

            Rotate2   = k1.Rotate2 + b*(k2.Rotate2 - k1.Rotate2);
            Translate2= k1.Translate2 + b*(k2.Translate2 - k1.Translate2);
            Scale2	  = k1.Scale2 + b*(k2.Scale2 - k1.Scale2);
            Color2    = k1.Color2 + b*(k2.Color2 - k1.Color2);
        }
        friend RtArchive& operator<<(RtArchive &Ar, SMuKey &k) 
        {
            Ar << k.Rotate << k.Translate << k.Scale << k.Color;
            return Ar;
        }
    };

	CRT_MtlMu();
	CRT_MtlMu(const CRT_MtlMu &object);
	virtual ~CRT_MtlMu();

	virtual long Serialize(RtArchive& Ar);
    virtual void OnLoadFinish();
    virtual bool Create();
    virtual void Destroy();
    virtual void Reset();
    virtual bool Apply(RtgDevice* _Dev);
    virtual void Restore(RtgDevice* _Dev);
    void ConvertOption();

    virtual CRT_PoolObject* NewObject() 
    {
        CRT_MtlMu* mtl = (CRT_MtlMu*)RtcGetActorManager()->NewObject(RT_RUNTIME_CLASS(CRT_MtlMu));
        mtl->m_name         = m_name;
        mtl->m_bUseFrame    = m_bUseFrame;
        mtl->m_bMask        = m_bMask;
        mtl->m_bTwoSide     = m_bTwoSide;
        mtl->m_bAlpha       = m_bAlpha;
        mtl->m_alphaOpt     = m_alphaOpt;
        mtl->m_visible      = m_visible;
        mtl->m_amb          = m_amb;
        mtl->m_dif          = m_dif;
        mtl->m_emi          = m_emi;
        mtl->m_colorOpt     = m_colorOpt;
        mtl->m_uvOpt        = m_uvOpt;
        mtl->m_texBase      = m_texBase;
        mtl->m_texSpec      = m_texSpec;
        mtl->m_keyList      = m_keyList;
        mtl->m_poType       = Pool_Type_Memory;
        mtl->m_delta        = 0;
        rt2_strncpy(mtl->m_texBasePath, m_texBasePath, Text_File_Path_Size);
        rt2_strncpy(mtl->m_texSpecPath, m_texSpecPath, Text_File_Path_Size);
        m_ro.Clone(&mtl->m_ro);

        return mtl;
    }

    virtual void Update(SMuKey &key)
    {
        m_rtColor = key.Color;
        m_rtSpecMat.Unit();
        m_rtSpecMat.Translate(key.Translate.x,key.Translate.y);
        m_rtSpecMat.RotateLZ(key.Rotate.x);
        m_rtSpecMat.Scale(key.Scale.x,key.Scale.y);

        m_rtColor2 = key.Color2;
        m_rtSpecMat2.Unit();
        m_rtSpecMat2.Translate(key.Translate2.x,key.Translate2.y);
        m_rtSpecMat2.RotateLZ(key.Rotate2.x);
        m_rtSpecMat2.Scale(key.Scale2.x,key.Scale2.y);
    }

    virtual bool RequestTick()
    {
        return (!m_bUseFrame && m_keyList.Size() != 0);
    }

    virtual void Tick(float deltaMill)
    {
        unsigned long beg,end;
        beg = m_keyList.GetBegFrame();
        end = m_keyList.GetEndFrame();
        m_delta += deltaMill;
        if (beg == end) 
            return;
        UseFrame(beg + (UINT)(m_delta / 33) % (end - beg));
    }

    virtual bool RequestUseFrame()
    {
        return (m_keyList.Size() != 0);
    }

    virtual void UseFrame(unsigned long frame)
    {
        SMuKey key;
        if(!m_keyList.GetKeyFrame(frame,key))
            return;
        Update(key);
    }

    virtual bool SetBaseColor(RtgVertex3 &color)
    {
        m_rtEmi.x = color.x;
        m_rtEmi.y = color.y;
        m_rtEmi.z = color.z;
        return true;
    }

    virtual bool GetBaseColor(RtgVertex3 &color)
    {
        color.x = m_rtEmi.x;
        color.y = m_rtEmi.y;
        color.z = m_rtEmi.z;
        return true;
    }

    virtual bool IsTransparent()
    {
        if (Abs(m_rtVisible - 1.0f) >= 0.01f || m_bAlpha)
            return true;
        return false;
    }
	
    virtual void SetVisibleGlobal(float visible) 
    {
		m_visibleGlobal = visible;
	}
	
    virtual void SetVisibleLocal(float visible) 
    {
		m_visibleLocal = visible;
	}

	virtual bool RequestUpdateNormal() 
    { 
        return false; 
    }

	virtual string& GetName() 
    { 
        return m_name; 
    }
	
    virtual void SetName(string &name) 
    { 
        m_name = name; 
    }

	virtual bool GetGeometry(int& alphaMode, RtgTextItem*& texItem)
	{
		if (m_bMask)
            alphaMode = ALPHA_COLOR_KEY;
		else if(!m_bAlpha) 
            alphaMode = ALPHA_NULL;
		else 
            alphaMode = m_alphaType;		
        texItem = m_ro.Shaders[0].GetTexture();;

        return true;
	}

    virtual int GetAlphaMdoe() 
    {
        if (m_bMask) 
            return ALPHA_COLOR_KEY;
        else if(!m_bAlpha) 
            return ALPHA_NULL;
        else 
            return m_alphaType;
    }

    virtual RtgTextItem* GetBaseText()
    {
        return m_ro.Shaders[0].GetTexture();
    }

    virtual string TextureName(int _index)
    {
        switch (_index)
        {
        case 0 :
            return m_texBase;
            break;
        case 1 :
            return m_texSpec;
            break;
        case 2 :
            return m_texSpec2;
            break;
        default :
            return "";
        }

        return "";
    }

    virtual bool ResourceReady()
    {
        return m_ro.ResourceReady();
    }

public :
	bool		m_bUseFrame;
	string		m_name;
	bool		m_bMask;
	bool		m_bTwoSide;
	bool		m_bAlpha;
	long		m_alphaType;
	string      m_alphaOpt;		
	float		m_visible;
	RtgVertex4  m_amb;
    RtgVertex4  m_dif;
    RtgVertex4  m_emi;

	string      m_uvOpt;		
	long        m_uvType;
	string      m_colorOpt;
	long        m_colorType;
	RtgVertex3  m_color;

	string      m_uvOpt2;		
	long        m_uvType2;
	string      m_colorOpt2;	
	long        m_colorType2;
	RtgVertex3  m_color2;
	keyframe<SMuKey> m_keyList;
	
    string		m_texBase;
    string		m_texSpec;
    string      m_texSpec2;

    //char        m_texBasePath[Text_File_Path_Size];
    //char        m_texSpecPath[Text_File_Path_Size];
    //char        m_texSpec2Path[Text_File_Path_Size];
	CM_MEMDEF(m_texBasePath, Text_File_Path_Size)
	CM_MEMDEF(m_texSpecPath, Text_File_Path_Size)
	CM_MEMDEF(m_texSpec2Path, Text_File_Path_Size)

	RtgVertex4  m_rtAmb;
    RtgVertex4  m_rtDif;
    RtgVertex4  m_rtEmi;
	RtgVertex3  m_rtColor;
    RtgVertex3  m_rtColor2;

	RtgMatrix2D9 m_rtSpecMat;
    RtgMatrix2D9 m_rtSpecMat2;

	float       m_rtVisible;
	float       m_visibleGlobal;
	float       m_visibleLocal;
	float       m_delta;
	RtgShader   m_ro;

#ifdef CHAR_VIEWER
    virtual void AttachPropGrid(void *p);
    virtual void AttachKeyGrid(int key,void *p);
    virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam);
    virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam);
    virtual bool CopyFrom(CRT_Material* pMat);

    // CTimeViewHost Interface
    virtual const char *TVGetName();
    virtual void TVOnNotify(ETV_OP op,long param1,long param2);
    virtual int TVGetKeyNum();
    virtual int TVGetKeyFrame(int key);
#endif
};


const int MAX_BLEND_CHANNEL = 3; // don't change this value
class CRT_MtlBlend : public CRT_Material
{
	RT_DECLARE_DYNCREATE(CRT_MtlBlend, CRT_Material, NULL, "")
	CRT_MtlBlend();
	virtual ~CRT_MtlBlend();

	// from RtObject
	virtual long Serialize(RtArchive& Ar);

	// from CRT_PoolObject
	virtual CRT_PoolObject *NewObject() {
		CRT_MtlBlend *t = (CRT_MtlBlend*)RtcGetActorManager()->NewObject(RT_RUNTIME_CLASS(CRT_MtlBlend));
		*t = *this;
		t->m_poType = Pool_Type_Memory;
		t->m_delta = 0;
		t->m_skin = NULL;
		t->m_rs = NULL;
		return t;
	}

	// CRT_Material Interface
	virtual bool RequestTick();
	virtual void Tick(float deltaMill);
	virtual bool RequestUseFrame(); 
	virtual void UseFrame(unsigned long frame);

	//virtual bool Init(RtgDevice *dev,CRT_SkinInstance *skin,RtcSkinSubset *rs);
	//virtual void Render(RtgDevice *dev,RTGRenderMask mask);
	//virtual bool Exit();
	//virtual bool IsActive();

	virtual bool IsTransparent();
	virtual void SetVisibleGlobal(float visible) {
		m_visibleGlobal = visible;
		// unsupport
	}
	virtual void SetVisibleLocal(float visible) {
		m_visibleLocal = visible;
		// unsupport
	}

	virtual bool RequestUpdateNormal() { return false; }
	virtual bool SetBaseColor(RtgVertex3 &color);
	virtual bool GetBaseColor(RtgVertex3 &color);

	virtual string& GetName() { return m_name; }
	virtual void SetName(string &name) { m_name = name; }

	virtual bool GetGeometry(int& alphaMode, RtgTextItem*& texItem)
	{
		return false;
		/*
		if(!m_bAlpha) return false;
		alphaMode = m_alphaType;
		texId = m_ro.Shaders[0].GetTexture();
		return true;
		*/
	}

    virtual RtgTextItem* GetBaseText()
    {
        return NULL;
    }

    virtual int GetAlphaMdoe() 
    {
        return ALPHA_NULL;
    }

    virtual string TextureName(int _index)
    {


        return "";
    }

    virtual bool ResourceReady()
    {
        for (int i = 0; i < MAX_BLEND_CHANNEL; ++i)
        {
            if (m_channel[i].texItem && 
                m_channel[i].Weight > 0.01f &&
                m_channel[i].texItem->state == Object_State_Ready)
                return false;
        }

        return true;
    }

    virtual bool Apply(RtgDevice* _Dev);
    virtual void Restore(RtgDevice* _Dev);

    virtual void OnLoadFinish()
    {
        RTASSERT(false);
    }

    virtual bool Create()
    {
        RTASSERT(false);
        return true;

    }
    virtual void Destroy() 
    {
        RTASSERT(false);
    }

    virtual void Reset()
    {
    }


#ifdef CHAR_VIEWER
	virtual void AttachPropGrid(void *p);
	virtual void AttachKeyGrid(int key,void *p);
	virtual bool OnPropGridNotify(WPARAM wParam, LPARAM lParam);
	virtual bool OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam);
	virtual bool CopyFrom(CRT_Material* pMat);

	// CTimeViewHost Interface
	virtual const char *TVGetName();
	virtual void TVOnNotify(ETV_OP op,long param1,long param2);
	virtual int TVGetKeyNum();
	virtual int TVGetKeyFrame(int key);
#endif

	struct SBlendChannel
	{
		string Tex;
			
        RtgTextItem* texItem;
		float  Weight;
		
		friend RtArchive& operator<<(RtArchive &Ar, SBlendChannel &c) 
		{
			Ar << c.Tex;
			BEGIN_VERSION_CONVERT(1)
			// read/write extra data
			END_VERSION_CONVERT
			return Ar;
		}
	};
	struct SBlendKey
	{
		float w1;
		float w2;
		float w3;

		SBlendKey()
		{
			w1 = 1.0f;
			w2 = 0.0f;
			w3 = 0.0f;
		}
		void Interpolate(unsigned long frame,unsigned long f1,SBlendKey &k1,
			unsigned long f2,SBlendKey &k2)
		{
			float b = ((float)frame - f1)/(f2 - f1);
			w1 = k1.w1 + b*(k2.w1 - k1.w1);
			w2 = k1.w2 + b*(k2.w2 - k1.w2);
			w3 = k1.w3 + b*(k2.w3 - k1.w3);
		}
		friend RtArchive& operator<<(RtArchive &Ar, SBlendKey &k) 
		{
			Ar << k.w1 << k.w2 << k.w3;
			return Ar;
		}
	};

	void Update(SBlendKey &key);
	void ConvertOption();

	// basic info
    //表示是否使用帧动画
	bool		m_bUseFrame;
    // name
	string		m_name;
    // 表示是否使用遮罩
	bool		m_bMask;
    // 是否双面渲染
	bool		m_bTwoSide;
    // 透明度
	bool		m_bAlpha;
    // 可见性
	float		m_visible;
	RtgVertex4  m_amb,m_dif,m_emi;
	SBlendChannel m_channel[MAX_BLEND_CHANNEL];
	keyframe<SBlendKey> m_keyList;

	// runtime var
	RtgVertex4 m_rtAmb,m_rtDif,m_rtEmi;
	RtgVertex3 m_rtColor;
	float m_rtVisible;
	bool m_bInit;

	float       m_visibleGlobal;
	float       m_visibleLocal;
	float       m_delta;
	CRT_SkinInstance  *m_skin;
	RtcSkinSubset *m_rs;
	RtgShader		   m_ro;
};
#endif
