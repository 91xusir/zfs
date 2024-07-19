
#include "gc_include.h"
#include "game_states_machine.h"
#include "gc_complex_missile.h"

namespace Game
{
	using	std::string;
	using	std::map;
	const	int		NULL_STATE=-1;
	//----------------------------------------------------
	//	״̬����״̬��
	//----------------------------------------------------
	CFSMState::CFSMState(int vStateID,unsigned int vMaxTransitions)
		:mMaxTransitions(1),mStateID(vStateID),
		mpInputs(NULL),mpOutput(NULL)
	{
		if(vMaxTransitions!=0)
		{
			mMaxTransitions=vMaxTransitions;
		}


		try
		{
			mpInputs= RT_NEW int[mMaxTransitions];
		}
		catch (...)
		{
			throw ;
		}
		try
		{
			mpOutput=RT_NEW int[mMaxTransitions];
		}
		catch(...)
		{
			DEL_ARRAY(mpInputs)
			throw;
		}



		for(unsigned int i=0;i<mMaxTransitions;++i)
		{
			mpInputs[i]=NULL_STATE;
			mpOutput[i]=NULL_STATE;
		}
	}


	CFSMState::~CFSMState()
	{
		DEL_ARRAY(mpInputs)
		DEL_ARRAY(mpOutput)
	}


	void CFSMState::AddTransition(int vInput,int vOutputID)
	{
		unsigned int i;
		for(i=0;i<mMaxTransitions;++i)
		{
			if(mpOutput[i]==NULL_STATE)
			{
				break;
			}
		}

		if(i<mMaxTransitions)
		{
			mpOutput[i]=vOutputID;
			mpInputs[i]=vInput;
		}
	}


	void CFSMState::DeleteTransition(int vOutputID)
	{
		//����Ҫɾ����״̬
		unsigned int i;
		for(i=0;i<mMaxTransitions;++i)
		{
			if(mpOutput[i]==vOutputID)
			{
				break;
			}
		}

		if(i<mMaxTransitions)
		{
			//��ɾ�������״̬�ĺ�������״̬ǰ��
			for(;i<mMaxTransitions-1;++i)
			{
				if(mpOutput[i]==NULL_STATE)
				{
					break;
				}
				mpInputs[i]=mpInputs[i+1];
				mpOutput[i]=mpOutput[i+1];
			}
			mpInputs[i]=NULL_STATE;
			mpOutput[i]=NULL_STATE;
		}
		return;
	}


	int CFSMState::GetOutput(int vInput)
	{
		int OutputID=mStateID;

		for(unsigned int i=0;i<mMaxTransitions;++i)
		{
			if(mpOutput[i]==NULL_STATE)
			{
				break;
			}

			if(mpInputs[i]==vInput)
			{
				OutputID=mpOutput[i];
				break;
			}
		}

		return OutputID;

	}



	//----------------------------------------------------
	//	״̬����
	//----------------------------------------------------
	CFSMManager CFSM::mManager;
	CFSM::CFSM()
		:mCurrentState(NULL_STATE),mMap()
	{
	}

	CFSM::~CFSM()
	{
		map<int,CFSMState*>::iterator It;
		for(It=mMap.begin();It!=mMap.end();It++)
		{
			DEL_ONE((*It).second)
		}
	}

	CFSMState* CFSM::GetState(int vStateID)
	{
		CFSMState* pState=NULL;
		map<int,CFSMState*>::iterator It;
		It=mMap.find(vStateID);
		if(It!=mMap.end())
		{
			pState=(*It).second;
		}

		return pState;
	}

	void CFSM::AddState(CFSMState* vpState)					// ����״̬����ָ��
	{
		if(GetState(vpState->GetID())==NULL)
		{
			mMap[vpState->GetID()]=vpState;
		}
		return;
	}

	void CFSM::DeleteState(int vStateID)					//ɾ��״̬����ָ��
	{
		map<int,CFSMState*>::iterator It;
		It=mMap.find(vStateID);
		if(It!=mMap.end())
		{
			DEL_ONE((*It).second)
			mMap.erase(It);
		}
		return;
	}

	int	CFSM::Transition(int vInput)							// ���ݡ���ǰ״̬���͡����롱��ɡ�״̬����ת����
	{
		if(mCurrentState==NULL_STATE)
		{
			return mCurrentState;
		}

		CFSMState* pState=GetState(mCurrentState);
		if(pState==NULL)
		{
			//��ǰ��״̬����Ч��״̬
			ShowErr("����:һ����Ч��״̬ת��");
			mCurrentState=NULL_STATE;
			return mCurrentState;
		}

		mCurrentState=pState->GetOutput(vInput);
		return mCurrentState;
	}

	int CFSM::Transition(int vInput,int vCurrentState)
	{
		SetCurrentState(vCurrentState);
		return Transition(vInput);
	}



	
	//----------------------------------------------------
	//	״̬��������
	//----------------------------------------------------
	CFSMManager::CFSMManager()
	{

	}

	CFSMManager::~CFSMManager()
	{
		map<string,CFSM*>::iterator It;
		for(It=mMap.begin();It!=mMap.end();++It)
		{
			DEL_ONE((*It).second)
		}
	}


	CFSM* CFSMManager::Get(string& vrType)
	{
		CFSM* pFSM=NULL;

		map<string,CFSM*>::iterator It;
		It=mMap.find(vrType);
		if(It!=mMap.end())
		{
			pFSM=(*It).second;
		}
		

		if(pFSM==NULL)
		{
			pFSM=GetFSM(vrType);
			if(pFSM!=NULL)
			{//����������״̬��
				mMap[vrType]=pFSM;
			}
		}
		return pFSM;
	}

	CFSM* CFSMManager::GetFSM(string& vrType)
	{
        if(vrType=="trump_state")
		{
			CFSM* pCFSM=RT_NEW CFSM;

			CFSMState* p;

			p=RT_NEW CFSMState(CComplexMissile::NULL_STATE,1);
			p->AddTransition(CComplexMissile::FOLLOW_COMMAND,CComplexMissile::FOLLOW_STATE);
			pCFSM->AddState(p);

			p=RT_NEW CFSMState(CComplexMissile::FOLLOW_STATE,1);
			p->AddTransition(CComplexMissile::ATTACK_COMMAND,CComplexMissile::ATTACK_STATE);
			pCFSM->AddState(p);

			p=RT_NEW CFSMState(CComplexMissile::ATTACK_STATE,2);
			p->AddTransition(CComplexMissile::RECOIL_COMMAND,CComplexMissile::RECOIL_STATE);
			p->AddTransition(CComplexMissile::STOP_COMMAND,CComplexMissile::BACK_HOME_STATE);
			pCFSM->AddState(p);

			p=RT_NEW CFSMState(CComplexMissile::RECOIL_STATE,2);
			p->AddTransition(CComplexMissile::READY_ATTACK_COMMAND,CComplexMissile::READY_ATTACK_STATE);
			p->AddTransition(CComplexMissile::STOP_COMMAND,CComplexMissile::BACK_HOME_STATE);
			pCFSM->AddState(p);

			p=RT_NEW CFSMState(CComplexMissile::READY_ATTACK_STATE,2);
			p->AddTransition(CComplexMissile::ATTACK_COMMAND,CComplexMissile::ATTACK_STATE);
			p->AddTransition(CComplexMissile::STOP_COMMAND,CComplexMissile::BACK_HOME_STATE);
			pCFSM->AddState(p);

			p=RT_NEW CFSMState(CComplexMissile::BACK_HOME_STATE,2);
			p->AddTransition(CComplexMissile::ATTACK_COMMAND,CComplexMissile::ATTACK_STATE);
			p->AddTransition(CComplexMissile::FOLLOW_COMMAND,CComplexMissile::FOLLOW_STATE);
			pCFSM->AddState(p);

			return pCFSM;
		}
		
		return NULL;
	}
}



