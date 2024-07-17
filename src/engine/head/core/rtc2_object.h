/**
* @file rtc2_object.h ���������RTTI.
* @author janhail
*/

namespace rt2_core {

/*-----------------------------------------------------------------------------
-   ����
-----------------------------------------------------------------------------*/

/// ȡ�����RTTI.
#define RT_RUNTIME_CLASS(TClass)                ((RtRuntimeClass*)(&TClass::type##TClass))

#define RT_REGISTRATION_CLASS(TClass)           static RtRegistrationClassStruct rtRegistrationClassStruct##TClass(&TClass::type##TClass);

/// ��̬ע���ࣨ�������������ֵ��ַ��������࣬Ҳ���Ǵ��л��Ĺ��ܣ������Ҫ���л�����Ҫ�ڳ����ʼ����ʱ��ͨ�������ע���ࣩ.
#define RT_STATIC_REGISTRATION_CLASS(TClass)    rtRegistrationClass(RT_RUNTIME_CLASS(TClass))

/*-----------------------------------------------------------------------------
-   ����
-----------------------------------------------------------------------------*/

/// ���� - ��̬����ʶ��.
#define RT_DECLARE_DYNAMIC(TClass, TSuperClass, TStaticFlags, TPackage) \
public: \
	typedef TSuperClass Super;\
	typedef TClass ThisClass;\
    static RtRuntimeClass type##TClass;\
	virtual RtRuntimeClass* GetRuntimeClass() const;\

/// ���� - ��̬����.
#define RT_DECLARE_DYNCREATE(TClass, TSuperClass, TStaticFlags, TPackage) \
	RT_DECLARE_DYNAMIC(TClass, TSuperClass, TStaticFlags, TPackage) \
	static RtObject* CreateObject();\

#define RT_DECLARE_DYNCREATE_ID(TClass, TSuperClass, TClassID) \
    RT_DECLARE_DYNCREATE(TClass, TSuperClass, 0, 0)

/*-----------------------------------------------------------------------------
-   ʵ��
-----------------------------------------------------------------------------*/

/// ʵ�� - ��̬����ʶ��.
#define RT_IMPLEMENT_RUNTIMETYPE(TClass, TSuperClass, TStaticFlags, TPackage, wSchema, pfnNew) \
	RtRuntimeClass TClass::type##TClass = { \
		#TClass, sizeof(class TClass), wSchema, pfnNew, \
			RT_RUNTIME_CLASS(TSuperClass), NULL }; \
	RtRuntimeClass* TClass::GetRuntimeClass() const \
		{ return RT_RUNTIME_CLASS(TClass); } \
        RT_REGISTRATION_CLASS(TClass)\

/// ʵ�� - ��̬����ʶ��.
#define RT_IMPLEMENT_DYNAMIC(TClass, TSuperClass, TStaticFlags, TPackage) \
	RT_IMPLEMENT_RUNTIMETYPE(TClass, TSuperClass, TStaticFlags, TPackage, 0xFFFF, NULL)\

/// ʵ�� - ��̬����.
#define RT_IMPLEMENT_DYNCREATE(TClass, TSuperClass, TStaticFlags, TPackage) \
	RtObject* TClass::CreateObject() { return RT_NEW TClass; } \
	RT_IMPLEMENT_RUNTIMETYPE(TClass, TSuperClass, TStaticFlags, TPackage, 0xFFFF, TClass::CreateObject)\


/// ʵ�� - ��̬����ʶ��.
#define RT_IMPLEMENT_RUNTIMETYPE_ID(TClass, TSuperClass, TClassID, pfnNew) \
	RtRuntimeClass TClass::type##TClass = { \
		#TClass, sizeof(class TClass), TClassID, pfnNew, \
			RT_RUNTIME_CLASS(TSuperClass), NULL }; \
	RtRuntimeClass* TClass::GetRuntimeClass() const \
		{ return RT_RUNTIME_CLASS(TClass); } \
        RT_REGISTRATION_CLASS(TClass)\

/// ʵ�� - ��̬����2.
#define RT_IMPLEMENT_DYNCREATE_ID(TClass, TSuperClass, TClassID) \
	RtObject* TClass::CreateObject() { return RT_NEW TClass; } \
    RT_IMPLEMENT_RUNTIMETYPE_ID(TClass, TSuperClass, TClassID, TClass::CreateObject)\

/*-----------------------------------------------------------------------------
-   RtRuntimeClass
-----------------------------------------------------------------------------*/

/** ��̬����ʶ��.
*   �������ú�;�̬����ʵ�ֵ�RTTI
*   ��ID�ĸ�����Ϊ�˼������л����ļ���С�����ڵģ�ID����Ϊ���ظ���1-65535֮���ֵ
*   Ϊ��ͳһ���������Ҫʹ�ã������Ҫ��ѭһ������ԭ��
*     1 - 9999 Ϊ����ʹ�ã�����ķ�������
*         1 -  99, Core     ʹ��
*       100 - 199, Graph    ʹ��
*       200 - 299, Net      ʹ��
*       300 - 399, Characterʹ��
*       400 - 499, Scene    ʹ��
*       500 - 599, UI       ʹ��
*       600 - 699, Audio    ʹ��
*     1000����Ϊ�û��Լ�����ʹ�ã�û�о���Ĺ涨
*/
struct RT2_CORE_API RtRuntimeClass
{
    // Attributes
	const char* m_lpszClassName;        // �������
	int         m_nObjectSize;          // ��Ĵ�С
    WORD        m_wClassID;             // ���ΨһID, ���ID��Ҫ�û��Լ���֤��Ψһ������������ID�������л���ʱ�����ID�����ж�ȡ��д��
	RtObject* (* m_pfnCreateObject)();  // NULL => abstract class
	RtRuntimeClass* m_pBaseClass;       // ����ָ��
	RtRuntimeClass* m_pNextClass;       // ��������ע�����

    RtObject* CreateObject();
	BOOL IsDerivedFrom(const RtRuntimeClass* pBaseClass) const;
	void Store(RtArchive& ar) const;
	static RtRuntimeClass* Load(RtArchive& ar, UINT* pwSchemaNum);
};

void rtDumpObjectClass(RtLog* pLog);
void rtRegistrationClass(RtRuntimeClass* pClass);
RtRuntimeClass* rtFindRuntimeClass(const char* lpszClassName);

struct RtRegistrationClassStruct
{
    RtRegistrationClassStruct(RtRuntimeClass* pClass);
};

typedef long RtObjState;

enum
{
    Object_State_New = 0,
    Object_State_Loading,
    Object_State_Creating,
    Object_State_Ready,
    Object_State_Need_Destroy
};

/*-----------------------------------------------------------------------------
-   RtObject
-----------------------------------------------------------------------------*/

/** ��̬����ʶ��/��������.
*   ֻ�м̳��������ܾ��ж�̬����ʶ��Ĺ���<BR>
*   �̳е�����Ҫ��.h��ʹ��RT_DECLARE_DYNCREATE�����ж�̬������ע��
*   @code
*   class CYourClass : public RtsObject
*   {
*       RT_DECLARE_DYNAMIC(CYourClass, RtsObject, 0, "YourModuleName")  // ��̬����ʶ���壬����ʹ�� RT_DECLARE_DYNCREATE ���ж�̬����
*       ...
*   }
*   @endcode
*   ͬ������.cpp��Ҳ��Ҫʹ�ö�Ӧ�ĺ�
*   @code
*   RT_IMPLEMENT_DYNAMIC(CYourClass, RtsObject, 0, "YourModuleName")    // ��̬����ʶ��ʵ�֣�����ʹ��RT_IMPLEMENT_DYNCREATE���ж�̬����
*   @endcode
*/
class RT2_CORE_API RtObject
{
public:
    RtObject();
    virtual ~RtObject();

	BOOL IsSerializable() const;
	BOOL IsKindOf(const RtRuntimeClass* pClass) const;
	virtual RtRuntimeClass* GetRuntimeClass() const;
	virtual long Serialize(RtArchive& ar);

    void SetState(RtObjState _state)
    {
        m_state = _state;
    }

    RtObjState GetState() const
    {
        return m_state;
    }

    void Loack_SetState(RtObjState _state)
    {
#ifdef WIN32
        InterlockedExchange(&m_state, _state);
#else
        m_state = _state;
#endif
    }

    bool ReourceReady() const
    {
        return m_state == Object_State_Ready;
    }

    virtual void OnLoadFinish() {}
    virtual void OnCreateFinish() {}

#ifndef _DEBUG
    inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
    inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
    inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
    inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
#endif

public:
	static RtRuntimeClass typeRtObject;
private :
    RtObjState m_state;
};

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
