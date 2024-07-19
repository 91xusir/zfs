#ifndef _INC_UI_FORM_BANK_PET_H_
#define _INC_UI_FORM_BANK_PET_H_

class GcActor;

class UIFormBankPet
{
public:
	UIFormBankPet();
	~UIFormBankPet();

	void Refresh();
	void OnPreDraw();
	void Show(DWORD npcID);
	void Hide();
	bool IsVisible();
	void Reset();
	bool SerializeBankBag(CG_CmdPacket *cmd);
	void ToMineResult(bool secceed);
	void ToBankResult(bool secceed);

private:
	void OnLClickListPet(RtwListBox *sender,void *);
	void OnLClickToMine(RtwButton *sender,void *);
	void OnLClickToBank(RtwButton *sender,void *);

private:
	RtwForm		*m_pFmThis;				
	RtwForm		*m_pFmPetLookMine;		
	RtwForm		*m_pFmPetLookBank;		

	RtwListBox	*m_pListPetMine;
	RtwListBox	*m_pListPetBank;

	RtwButton	*m_pBtnToMine;
	RtwButton	*m_pBtnToBank;

	CBag m_BankBag;
	std::list<SItemID>	m_listPetMine;
	std::list<SItemID>	m_listPetBank;
	int m_nNpcID;
	SItemID m_ToMinePet;
	SItemID m_ToBankPet;
};

#endif