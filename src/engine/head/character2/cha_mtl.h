#ifndef __CHA_MTL_H__
#define __CHA_MTL_H__

void SetFogWithAdd(RtgDevice *dev);
void RestoreFog(RtgDevice *dev);
void SetupAlphaMode(long alpha, RtgShader &ro, RtgDevice *dev, float visible);
void RestoreAlphaMode(long alpha, RtgShader &ro, RtgDevice *dev, float visible);

/*
struct SHardwareLight
{
	bool	   bEnableLight;
	RtgVertex4 Amb;
	RtgVertex4 Dif;
	RtgVertex4 Spe;
	RtgVertex4 Emi;
	float      Power;
};

const char FB_OVERWRITE = 1;
const char FB_MASK      = 2;
const char FB_BLEND     = 3;
const char FB_ADD	    = 4;
const char FB_SUB       = 5;
const char FB_MODULATE  = 6;
struct SFrameBuffer
{
	char DrawType;
	bool bTwoSide;
	bool bMask;
	bool bZTest;
	bool bZWrite;
};
*/

class CRT_MtlInstStandard : public CRT_MtlInstance
{
	RT_DECLARE_DYNCREATE(CRT_MtlInstStandard, CRT_MtlInstance, NULL, "");
	CRT_MtlInstStandard();
	virtual ~CRT_MtlInstStandard();

	// from CRT_ObjectInst
	virtual void InstanceInit();
	virtual void InstanceExit();
	virtual void OnDataChange(DWORD p1, DWORD p2);

	virtual bool RequestTick();
	virtual void Tick(DWORD delta);
	virtual bool RequestUseFrame();
	virtual void UseFrame(DWORD frame);

	virtual bool Init(RtgDevice *dev, CRT_SkinInstance *skin, SRT_RenderSurface *rs);
	virtual void Render(RtgDevice *dev, RTGRenderMask mask);
	// virtual bool Exit() = 0;
	virtual bool IsTransparent();

	// spec light need update normal
	virtual bool RequestUpdateNormal() { return true; }				
	virtual bool GetGeometry(int& alphaMode,int& texId,SRT_RenderSurface **rs,bool& shadow)
	{
		alphaMode = m_mtlStandard->m_alphaMode;
		shadow = m_mtlStandard->m_bShadow;
		texId = m_ro.Shaders[0].GetTexture();
		*rs = m_rs;
		return true;
	}

	// CRT_MtlInstStandard
	virtual void GetBaseInfo(RtgVertex4 &amb,RtgVertex4 &dif,RtgVertex4 &emi) {
	}
	virtual void SetBaseInfo(RtgVertex4 &amb,RtgVertex4 &dif,RtgVertex4 &emi) {
	}
	float GetFinialVisible();
	void BuildEffectTexStage(string &effectTex, string &effectMaskTex, bool matrix, 
		char uvMode, bool useFactor, long blendMode, int &effectStage, int &effectMaskStage);
	void Update(SMtlStandardKey &key);
	bool UploadVB(RtgDevice *dev);
	bool UploadIB(RtgDevice *dev);

	unsigned long m_delta;
    CRT_SkinInstance  *m_skin;
	SRT_RenderSurface *m_rs;

	RtgShader m_ro;
	RtgMatrix2D9 m_texMtx;

	SMtlStandardKey  m_key;
	CRT_MtlStandard *m_mtlStandard;
	RtgDevice *m_renderDev;

	int m_baseStage;
	int m_hlStage;
	int m_effectStage1;
	int m_effectMaskStage1;
	int m_effectStage2;
	int m_effectMaskStage2;
};

class CRT_MtlInstBlend : public CRT_MtlInstance
{
	RT_DECLARE_DYNCREATE(CRT_MtlInstBlend, CRT_MtlInstance, NULL, "");
	CRT_MtlInstBlend();
	virtual ~CRT_MtlInstBlend();

	// from CRT_ObjectInst
	virtual void InstanceInit();
	virtual void InstanceExit();
	virtual void OnDataChange(DWORD p1, DWORD p2);

	virtual bool RequestTick();
	virtual void Tick(DWORD delta);
	virtual bool RequestUseFrame();
	virtual void UseFrame(DWORD frame);

	virtual bool Init(RtgDevice *dev, CRT_SkinInstance *skin, SRT_RenderSurface *rs);
	virtual void Render(RtgDevice *dev, RTGRenderMask mask);
	// virtual bool Exit() = 0;
	virtual bool IsTransparent();

	// spec light need update normal
	virtual bool RequestUpdateNormal() { return true; }				
	virtual bool GetGeometry(int& alphaMode,int& texId,SRT_RenderSurface **rs,bool& shadow)
	{
		alphaMode = m_mtlBlend->m_fo.alphaMode;
		shadow = m_mtlBlend->m_fo.shadow;
		texId = m_ro.Shaders[0].GetTexture();
		*rs = m_rs;
		return true;
	}

	// CRT_MtlInstStandard
	virtual void GetBaseInfo(RtgVertex4 &amb,RtgVertex4 &dif,RtgVertex4 &emi) {
	}
	virtual void SetBaseInfo(RtgVertex4 &amb,RtgVertex4 &dif,RtgVertex4 &emi) {
	}
	float GetFinialVisible();

	void Update(SMtlBlendKey &key);
	bool UploadVB(RtgDevice *dev);
	bool UploadIB(RtgDevice *dev);

	unsigned long m_delta;
	CRT_SkinInstance  *m_skin;
	SRT_RenderSurface *m_rs;

	RtgShader    m_ro;
	RtgMatrix2D9 m_texMtx;

	SMtlBlendKey  m_key;
	CRT_MtlBlend *m_mtlBlend;
	RtgDevice    *m_renderDev;
};

#endif
