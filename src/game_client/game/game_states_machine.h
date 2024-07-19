
#ifndef GAME_STATES_MACHINE_H
#define GAME_STATES_MACHINE_H

#include <string>
#include <map>


namespace Game
{
	//�������޻���״̬��
	class CFSMState
	{
	public:
		CFSMState(int vStateID,unsigned int vMaxTransitions);
		~CFSMState();

		void AddTransition(int vInput,int vOutputID);
		void DeleteTransition(int vOutputID);

		int	GetID(){return mStateID;};
		int GetOutput(int vInput);

	private:
		unsigned int	mMaxTransitions;		//����ת����
		int				mStateID;				//״̬��ʶ
		int*			mpInputs;				//Ϊ��ת����ʹ�õ���������
		int*			mpOutput;				//���״̬����

	};


	class CFSM;

	class CFSMManager
	{
	public:
		CFSMManager();
		~CFSMManager();

		CFSM*	Get(std::string& vrType);

	private:
		CFSM*	GetFSM(std::string& vrType);
		std::map<std::string,CFSM*>			mMap;
	};

	
	//��������״̬����
	class CFSM
	{
		friend CFSMManager;
	public:


		void		AddState(CFSMState* vpState);					// ����״̬����ָ��
		void		DeleteState(int vStateID);						//ɾ��״̬����ָ��
		int			GetCurrentState(){return mCurrentState;}
		void		SetCurrentState(int vStateID){mCurrentState=vStateID;}

		int			Transition(int vInput);							//�������룬���ݵ�ǰ״̬�õ�ת�����״̬
		int			Transition(int vInput,int vCurrentState);		//������ǰ״̬������õ�ת�����״̬

		static CFSMManager	mManager;
	private:
		CFSM();		//��ʼ��״̬
		~CFSM();

		CFSMState*	GetState(int vStateID);							//����FSMState����ָ��
		int							mCurrentState;					//��ǰ״̬��ID
		std::map<int,CFSMState*>			mMap;					//״̬��������״̬
	};
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	class CAIProcessor
	{
	public:
		CAIProcessor();
		virtual ~CAIProcessor();

		int	Run(int vTime){return 0;};//�õ���ǰ��״̬
	private:
	};


}


#endif//GAME_STATES_MACHINE_H

