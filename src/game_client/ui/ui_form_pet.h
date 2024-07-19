
#ifndef __UI_FORM_PET_H__
#define __UI_FORM_PET_H__

#include "ui/rtw_ui.h"

const unsigned char UIPET_SAMELEV_IMAGE_NUM		= 4;
const unsigned char UIPET_LINKUP_BEADS_NUM		= 4;
const unsigned char UIPET_SKILL_NUM				= 6;
const unsigned char UIPET_CTRL_BUTTON_NUM		= 4;

extern char PET_SOMELEV_IMAGE_1[];
extern char PET_SOMELEV_IMAGE_2[];
extern char PET_BEADS_IMAGE_1[];
extern char PET_BEADS_IMAGE_2[];
extern char PET_BEADS_IMAGE_3[];
extern char PET_BEADS_IMAGE_4[];

void SendCtrlToServer(const unsigned char command,DWORD param1 = 0,DWORD param2 = 0);

class UIFromPetMain
{
public:
	UIFromPetMain(void);
	~UIFromPetMain(void);

	void Refresh();
	void Show();
	void Show(SItemID pet);
	void Hide();
	void OnPreDraw();
	bool IsVisible();
	void Reset();
	void SetSelectPet(long id);
	bool IsSelectCalledPet();

	void OnHide(RtwWidget *sender,void *);

	SItemID m_CurPetItem;
	list<SItemID> m_listPet;

private:
	void CallOutPet(void *,void *);
	void DeletePet(void *,void *);
	void RealDeletePet(RtwWidget *sender,void *);
	void ChangePetName(void *,void *);
	void CallAptitudeUI(void *,void *);
	void OnClickPetName(void *,void *);
	void GetHintPetType(void *,void *);

private:
	RtwForm			*m_pFormMain;
public:
	Rtw3DView		*m_pFormView;
	RtwListBox		*m_pListPet;
	RtwButton		*m_pBtnCallOut;
	RtwButton		*m_pBtnCallBack;
	RtwButton		*m_pBtnAptitude;
	RtwButton		*m_pBtnChangeName;
	RtwTextBox		*m_pTexChangetName;
	RtwLabel		*m_pLabelLev;
	RtwLabel		*m_pLabelHeartNeed;
	RtwLabel		*m_pLabelHp;
	RtwLabel		*m_pLabelMp;
	RtwLabel		*m_pLabelExp;
	RtwLabel		*m_pLabelPetNum;

	RtwLabel		*m_pLabelAtt;
	RtwLabel		*m_pLabelDef;
	RtwLabel		*m_pLabelHit;
	RtwLabel		*m_pLabelDodge;
	RtwLabel		*m_pLabelFireAtt;
	RtwLabel		*m_pLabelWaterAtt;
	RtwLabel		*m_pLabelPosionAtt;
	RtwLabel		*m_pLabelFireDef;
	RtwLabel		*m_pLabelWaterDef;
	RtwLabel		*m_pLabelPosionDef;

	RtwProgressBar	*m_pProExp;
	RtwProgressBar	*m_pProHp;
	RtwProgressBar	*m_pProMp;
	
	RtwLabel		*m_pLbPetTypeImage;

	DWORD			m_dwSelectPet;
};

#endif