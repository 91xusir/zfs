/**
* @file rtc2_object.h 基本对象和RTTI.
* @author janhail
*/

namespace rt2_core {

/*-----------------------------------------------------------------------------
-   定义
-----------------------------------------------------------------------------*/

/// 取得类的RTTI.
#define RT_RUNTIME_CLASS(TClass)                ((RtRuntimeClass*)(&TClass::type##TClass))

#define RT_REGISTRATION_CLASS(TClass)           static RtRegistrationClassStruct rtRegistrationClassStruct##TClass(&TClass::type##TClass);

/// 静态注册类（用来根据类名字的字符串创建类，也就是串行化的功能，如果需要串行化就需要在程序初始化的时候通过这个宏注册类）.
#define RT_STATIC_REGISTRATION_CLASS(TClass)    rtRegistrationClass(RT_RUNTIME_CLASS(TClass))

/*-----------------------------------------------------------------------------
-   定义
-----------------------------------------------------------------------------*/

/// 定义 - 动态类型识别.
#define RT_DECLARE_DYNAMIC(TClass, TSuperClass, TStaticFlags, TPackage) \
public: \
	typedef TSuperClass Super;\
	typedef TClass ThisClass;\
    static RtRuntimeClass type##TClass;\
	virtual RtRuntimeClass* GetRuntimeClass() const;\

/// 定义 - 动态创建.
#define RT_DECLARE_DYNCREATE(TClass, TSuperClass, TStaticFlags, TPackage) \
	RT_DECLARE_DYNAMIC(TClass, TSuperClass, TStaticFlags, TPackage) \
	static RtObject* CreateObject();\

#define RT_DECLARE_DYNCREATE_ID(TClass, TSuperClass, TClassID) \
    RT_DECLARE_DYNCREATE(TClass, TSuperClass, 0, 0)

/*-----------------------------------------------------------------------------
-   实现
-----------------------------------------------------------------------------*/

/// 实现 - 动态类型识别.
#define RT_IMPLEMENT_RUNTIMETYPE(TClass, TSuperClass, TStaticFlags, TPackage, wSchema, pfnNew) \
	RtRuntimeClass TClass::type##TClass = { \
		#TClass, sizeof(class TClass), wSchema, pfnNew, \
			RT_RUNTIME_CLASS(TSuperClass), NULL }; \
	RtRuntimeClass* TClass::GetRuntimeClass() const \
		{ return RT_RUNTIME_CLASS(TClass); } \
        RT_REGISTRATION_CLASS(TClass)\

/// 实现 - 动态类型识别.
#define RT_IMPLEMENT_DYNAMIC(TClass, TSuperClass, TStaticFlags, TPackage) \
	RT_IMPLEMENT_RUNTIMETYPE(TClass, TSuperClass, TStaticFlags, TPackage, 0xFFFF, NULL)\

/// 实现 - 动态创建.
#define RT_IMPLEMENT_DYNCREATE(TClass, TSuperClass, TStaticFlags, TPackage) \
	RtObject* TClass::CreateObject() { return RT_NEW TClass; } \
	RT_IMPLEMENT_RUNTIMETYPE(TClass, TSuperClass, TStaticFlags, TPackage, 0xFFFF, TClass::CreateObject)\


/// 实现 - 动态类型识别.
#define RT_IMPLEMENT_RUNTIMETYPE_ID(TClass, TSuperClass, TClassID, pfnNew) \
	RtRuntimeClass TClass::type##TClass = { \
		#TClass, sizeof(class TClass), TClassID, pfnNew, \
			RT_RUNTIME_CLASS(TSuperClass), NULL }; \
	RtRuntimeClass* TClass::GetRuntimeClass() const \
		{ return RT_RUNTIME_CLASS(TClass); } \
        RT_REGISTRATION_CLASS(TClass)\

/// 实现 - 动态创建2.
#define RT_IMPLEMENT_DYNCREATE_ID(TClass, TSuperClass, TClassID) \
	RtObject* TClass::CreateObject() { return RT_NEW TClass; } \
    RT_IMPLEMENT_RUNTIMETYPE_ID(TClass, TSuperClass, TClassID, TClass::CreateObject)\

/*-----------------------------------------------------------------------------
-   RtRuntimeClass
-----------------------------------------------------------------------------*/

/** 动态类型识别.
*   这是利用宏和静态变量实现的RTTI
*   类ID的概念是为了减少序列化的文件大小而存在的，ID必须为不重复的1-65535之间的值
*   为了统一管理，如果需要使用，大家需要遵循一个分配原则
*     1 - 9999 为引擎使用，具体的分配如下
*         1 -  99, Core     使用
*       100 - 199, Graph    使用
*       200 - 299, Net      使用
*       300 - 399, Character使用
*       400 - 499, Scene    使用
*       500 - 599, UI       使用
*       600 - 699, Audio    使用
*     1000以上为用户自己定义使用，没有具体的规定
*/
struct RT2_CORE_API RtRuntimeClass
{
    // Attributes
	const char* m_lpszClassName;        // 类的名字
	int         m_nObjectSize;          // 类的大小
    WORD        m_wClassID;             // 类的唯一ID, 这个ID需要用户自己保证他唯一，如果存在这个ID，那序列化的时候会用ID来进行读取和写入
	RtObject* (* m_pfnCreateObject)();  // NULL => abstract class
	RtRuntimeClass* m_pBaseClass;       // 基类指针
	RtRuntimeClass* m_pNextClass;       // 用来链接注册的类

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

/** 动态类型识别/创建基类.
*   只有继承这个类才能具有动态类型识别的功能<BR>
*   继承的类需要在.h中使用RT_DECLARE_DYNCREATE来进行动态创建的注册
*   @code
*   class CYourClass : public RtsObject
*   {
*       RT_DECLARE_DYNAMIC(CYourClass, RtsObject, 0, "YourModuleName")  // 动态类型识别定义，或者使用 RT_DECLARE_DYNCREATE 进行动态创建
*       ...
*   }
*   @endcode
*   同样，在.cpp中也需要使用对应的宏
*   @code
*   RT_IMPLEMENT_DYNAMIC(CYourClass, RtsObject, 0, "YourModuleName")    // 动态类型识别实现，或者使用RT_IMPLEMENT_DYNCREATE进行动态创建
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
