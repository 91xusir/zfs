#ifndef _RTUI_DELEGATE_H
#define _RTUI_DELEGATE_H


namespace ui {

	class RtwWidget;
	struct RtwEventDelegate;

	class IUiEventReceiver
	{
	public:
		IUiEventReceiver() { }
		virtual ~IUiEventReceiver() { }
	};

#define UI_DELEGATE_M(receiver, clazz, handle)	(CUiDelegate((IUiEventReceiver*)static_cast<void*>(receiver), (CUiDelegate::Thiscall)(int (clazz::*)(void*, void*))&clazz::handle))
#define UI_DELEGATE_MF(clazz, handle)			(CUiDelegateF((CUiDelegate::Cdecl)((void clazz::*)())(&clazz::handle))
#define UI_DELEGATE_F(handle)					(CUiDelegateF((CUiDelegate::CdeclF)&handle))
#define RTW_CALLBACK							    UI_DELEGATE_M
#define RTW_CALLBACK_1(receiver, clazz, handle, param1)	(CUiDelegate((IUiEventReceiver*)static_cast<void*>(receiver), (CUiDelegate::Thiscall)(int (clazz::*)(void*, void*))&clazz::handle,param1))
#define RTW_CALLBACK_F(handle)					(CUiDelegateF((CUiDelegate::CdeclF)&handle))

	class CUiDelegateF
	{
	public:
		typedef int (IUiEventReceiver::*Thiscall)(void*, void*);
		typedef int (_fastcall IUiEventReceiver::*Fastcall)(void*, void*);
		typedef int (_cdecl IUiEventReceiver::*Cdecl)(void*, void*);	
		typedef int (_cdecl *CdeclF)(void*, void*, void*);
		typedef int (_stdcall IUiEventReceiver::*Stdcall)(void*, void*);
		typedef int (_stdcall *StdcallF)(void*, void*, void*);

	protected:
		union
		{
			Thiscall	m_handle;
			Thiscall	m_thiscall;
			Fastcall	m_fastcall;
			Cdecl		m_cdecl;		
			CdeclF		m_cdeclf;
			Stdcall		m_stdcall;
			StdcallF	m_stdcallF;
		};

		typedef int (_fastcall CUiDelegateF::*Caller)(IUiEventReceiver*, void*, void*);

		volatile Caller m_caller;

	private:
		int _fastcall DelegateThiscall(IUiEventReceiver* receiver, void* sender, void* arg)
		{
			return (receiver->*m_thiscall)(sender, arg);
		}

		int _fastcall DelegateFastcall(IUiEventReceiver* receiver, void* sender, void* arg)
		{
			return (receiver->*m_fastcall)(sender, arg);
		}

		int _fastcall DelegateCdecl(IUiEventReceiver* receiver, void* sender, void* arg)
		{
			return (receiver->*m_cdecl)(sender, arg);
		}

		int _fastcall DelegateCdeclF(IUiEventReceiver* receiver, void* sender, void* arg)
		{
			return (*m_cdeclf)(receiver, sender, arg);
		}

		int _fastcall DelegateStdcall(IUiEventReceiver* receiver, void* sender, void* arg)
		{
			return (receiver->*m_stdcall)(sender, arg);
		}

		int _fastcall DelegateStdcallF(IUiEventReceiver* receiver, void* sender, void* arg)
		{
			return (*m_stdcallF)(receiver, sender, arg);
		}

	public:
		int Signal(const IUiEventReceiver* receiver, const void* sender, const void* arg) const
		{
			return (const_cast<CUiDelegateF*>(this)->*m_caller)((IUiEventReceiver*)receiver, (void*)sender, (void*)arg);
		}

		int operator ()(const IUiEventReceiver* receiver, const void* sender, const void* arg) const
		{
			return Signal(receiver, sender, arg);
		}

		bool IsNull() const 
		{
			return m_caller == NULL;
		}

		bool IsEqual(CUiDelegateF& Other) const
		{
			return m_caller == Other.m_caller;
		}

		//operator bool () const
		//{
		//    return m_caller != NULL;
		//}

	protected:
		CUiDelegateF(Thiscall handle, Caller caller)
			: m_handle(handle)
			, m_caller(caller)
		{
		}

	public:
		CUiDelegateF()
			: m_handle(NULL)		
			, m_caller(NULL)
		{

		}

		CUiDelegateF(const CUiDelegateF& copy)
			: m_handle(copy.m_handle)
			, m_caller(copy.m_caller)
		{
		}

		CUiDelegateF(Thiscall handle)
			: m_thiscall(handle)
			, m_caller(&ui::CUiDelegateF::DelegateThiscall)
		{
		}

		CUiDelegateF(Fastcall handle)
			: m_fastcall(handle)
			, m_caller(&ui::CUiDelegateF::DelegateFastcall)
		{
		}

		CUiDelegateF(Cdecl handle)
			: m_cdecl(handle)
			, m_caller(&ui::CUiDelegateF::DelegateCdecl)
		{
		}

		CUiDelegateF(CdeclF handle)
			: m_cdeclf(handle)
			, m_caller(&ui::CUiDelegateF::DelegateCdeclF)
		{
		}
	};

	class CUiDelegateM : public CUiDelegateF
	{
	private:
		IUiEventReceiver* m_receiver;

	public:
		int Signal(const void* sender, const void* arg) const
		{
			return CUiDelegateF::Signal((const IUiEventReceiver*)m_receiver, sender, arg);
		}

		int operator () (const void* sender, const void* arg) const
		{
			return CUiDelegateF::Signal(m_receiver, sender, arg);
		}	

		bool IsEqual(CUiDelegateM& Other) const 
		{
			if (!CUiDelegateF::IsEqual(Other))
				return false;

			return m_receiver == Other.m_receiver; 
		}
		void*			  m_param;

		void* GetParam() {return m_param;}

	public:
		CUiDelegateM()
			: m_receiver(NULL)
			, m_param(NULL)
		{
		}

		template<class Q>
		CUiDelegateM(IUiEventReceiver* receiver, Q handle, void* param = NULL)	
			: CUiDelegateF(handle)
			, m_receiver(receiver)
			, m_param(param)
		{
		}

		CUiDelegateM(CdeclF handle)
			: CUiDelegateF(handle)
			, m_receiver(NULL)
			, m_param(NULL)
		{
		}	

		CUiDelegateM(const CUiDelegateF& copy)
			: CUiDelegateF(copy)
			, m_receiver(NULL)
			, m_param(NULL)
		{
		}

		CUiDelegateM(const CUiDelegateM& copy)
			: CUiDelegateF((const CUiDelegateF&)copy)
			, m_receiver(copy.m_receiver)
			, m_param(copy.m_param)
		{
		}	
	};

	typedef CUiDelegateM CUiDelegate;

} // namespace ui



#endif // _RTUI_DELEGATE_H
