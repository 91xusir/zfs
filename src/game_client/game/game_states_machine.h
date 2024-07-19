
#ifndef GAME_STATES_MACHINE_H
#define GAME_STATES_MACHINE_H

#include <string>
#include <map>


namespace Game
{
	//定义有限机的状态类
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
		unsigned int	mMaxTransitions;		//最大的转换数
		int				mStateID;				//状态标识
		int*			mpInputs;				//为了转换而使用的输入数组
		int*			mpOutput;				//输出状态数组

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

	
	//定义有限状态机类
	class CFSM
	{
		friend CFSMManager;
	public:


		void		AddState(CFSMState* vpState);					// 增加状态对象指针
		void		DeleteState(int vStateID);						//删除状态对象指针
		int			GetCurrentState(){return mCurrentState;}
		void		SetCurrentState(int vStateID){mCurrentState=vStateID;}

		int			Transition(int vInput);							//给出输入，根据当前状态得到转换后的状态
		int			Transition(int vInput,int vCurrentState);		//给出当前状态和输入得到转换后的状态

		static CFSMManager	mManager;
	private:
		CFSM();		//初始化状态
		~CFSM();

		CFSMState*	GetState(int vStateID);							//返回FSMState对象指针
		int							mCurrentState;					//当前状态的ID
		std::map<int,CFSMState*>			mMap;					//状态机的所有状态
	};
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	class CAIProcessor
	{
	public:
		CAIProcessor();
		virtual ~CAIProcessor();

		int	Run(int vTime){return 0;};//得到当前的状态
	private:
	};


}


#endif//GAME_STATES_MACHINE_H

