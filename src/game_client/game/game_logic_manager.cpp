//===========================================================
//		文件名称： game_logic_manager.cpp
//		作    者： 吴麟
//		时    间： 2004.6.28
//		内    容： 游戏逻辑部分管理器
//===========================================================
#include "game_logic_manager.h"
#include <algorithm>
#include "game_object.h"

namespace Game
{
	CManager::CManager()
	{
	}

	CManager::~CManager()
	{
	}

	void CManager::AddObject(CObject* vpObject,SafeTypes vType)
	{
		if(vType==SAFE)
		{
			mSafeList.push_back(vpObject);
		}
		else if(vType==UN_SAFE)
		{
			mUnSafeList.push_back(vpObject);
		}
		else if(vType==UPLOAD)
		{
			mUploadList.push_back(vpObject);
		}
		mAllList.push_back(vpObject);
	}

	void CManager::DelObject(CObject* vpObject)
	{
		std::list<CObject*>::iterator It;
		for (It=mAllList.begin();It!=mAllList.end();It++)
		{
			delete (*It);
			(*It)=NULL;
		}
	}


	void CManager::Command(int vID,ObjectCommands vCommand,float vX,float vY,float vZ)
	{
		std::list<CObject*>::iterator It;
		
		for(It=mAllList.begin();It!=mAllList.end();It++)
		{
			if((*It)->ID()==vID)
			{
				break;
			}
		}






	

	}


}

