#ifndef GC_PET_H
#define GC_PET_H
#include "gc_actor.h"

class GcPet :
	public GcActor
{
public:
	GcPet(void);
	~GcPet(void);

	void SetPetItem(SItemID petItem);
	void CallOutPet(DWORD id, GcActor *pet);
	void CallBackPet();
	SItemID GetPetItem();

	GcActor	*GetPetActor(){ return m_pPet; }
	void SetPetActor(GcActor *pet);
	void LinkUpWithPet(long petItemID);
	bool IsLinkUpPet() { return m_lLinkPetItem != 0; }
	void OnRun(float fSecond);
	void SetActor(GcActor *pPet) { m_pPet = pPet; }
	void ProcessSwitchScene();

private:
	SItemID m_CurPetItem;
	GcActor	*m_pPet;
	long m_lLinkPetItem;
	bool m_bChangeDisplay;
	bool m_bWaitRide;
};


#endif // GC_PET_H
