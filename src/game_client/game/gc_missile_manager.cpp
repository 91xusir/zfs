#include "gc_missile.h"
#include "gc_missile_manager.h"
#include "gc_beeline_missile.h"

CMissileManager::CMissileManager()
{
}

CMissileManager::~CMissileManager()
{
	for(list<CMissile*> ::iterator It = mMissileList.begin() ; It!=mMissileList.end() ;It++)
	{
        (*It)->Destroy();
		DEL_ONE(*It)
	}
}

void CMissileManager::OnAfterScenePreRender(RtgCamera& inCamera, RtScene* pScene)
{
	for(list<CMissile*>::iterator It=mMissileList.begin();It!=mMissileList.end();It++)
	{

        pScene->AddRenderObject((*It)->GetRender());
	}
}

//void CMissileManager::Render(RtgDevice& vrDevice,RTGRenderMask vMask)		//‰÷»æ¥¶¿Ì
//{
//	for(list<CMissile*>::iterator It=mMissileList.begin();It!=mMissileList.end();It++)
//	{
//		(*It)->Render(vrDevice,vMask);
//	}
//}

void CMissileManager::Run(float vSecond)
{
    CMissile* pMissile;
	for(list<CMissile*>::iterator It=mMissileList.begin();It!=mMissileList.end();)
	{
        pMissile = *It;
		pMissile->Run(vSecond);
		if(pMissile->IsExist())
		{
			It++;
		}
		else
		{
            pMissile->Destroy();
			DEL_ONE(pMissile)
			It=mMissileList.erase(It);
		}
	}
}

void CMissileManager::Add(CMissile* vpMissile)
{
	mMissileList.push_back(vpMissile);
	return;
}

