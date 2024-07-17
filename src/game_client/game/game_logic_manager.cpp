//===========================================================
//		�ļ����ƣ� game_logic_manager.cpp
//		��    �ߣ� ����
//		ʱ    �䣺 2004.6.28
//		��    �ݣ� ��Ϸ�߼����ֹ�����
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

