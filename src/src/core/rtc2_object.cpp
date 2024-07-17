#include "core/rt2_core.h"

namespace rt2_core {

/*------------------------------------------------------------------------
	RtRegistrationClass
------------------------------------------------------------------------*/

RtRegistrationClassStruct::RtRegistrationClassStruct(RtRuntimeClass* pLinkClass)
{
    rtRegistrationClass(pLinkClass);
}

struct RtRuntimeClassList
{
    RtRuntimeClass* pFirst;

    RtRuntimeClassList()
    {
        //RtObject::typeRtObject = {"RtObject", sizeof(RtObject), 0, NULL, NULL, NULL};
        pFirst = (RtRuntimeClass*)&RtObject::typeRtObject;
    }
};

RtRuntimeClassList* rtGetRuntimeClassList()
{
    static RtRuntimeClassList listRuntimeClass;
    return &listRuntimeClass;
}

RtRuntimeClass* rtFindRuntimeClass(const char* lpszClassName)
{
	RtRuntimeClass* pClass;
	for (pClass=rtGetRuntimeClassList()->pFirst; pClass!=NULL; pClass = pClass->m_pNextClass)
	{
		if (strcmp(pClass->m_lpszClassName, lpszClassName) == 0)
			return pClass;
	}
	return NULL;
}

void rtRegistrationClass(RtRuntimeClass* pLinkClass)
{
    // 先检查是否有重复ID的现象
    if (pLinkClass->m_wClassID)
    {
        RtRuntimeClass* pClass;
        for (pClass=rtGetRuntimeClassList()->pFirst; pClass!=NULL; pClass = pClass->m_pNextClass)
	    {
			// C [12/19/2008 Louis.Huang]
/*
            if (pLinkClass->m_wClassID==pClass->m_wClassID)
		    {
                CHECKEX("错误: 类的类型ID重复");
                return;
		    }*/

	    }
    }

    // 链接到链表上
    if (pLinkClass->m_pNextClass==NULL)
    {
        pLinkClass->m_pNextClass = rtGetRuntimeClassList()->pFirst;
        rtGetRuntimeClassList()->pFirst = pLinkClass;
    }
}

void rtDumpObjectClass(RtLog* pLog)
{
    RtRuntimeClass* pClass;
    pLog->Info("Dump Class:\n  %20s  |  %-17s | Dynamic Create | Class Size\n", "Class name", "Base class name");
    for (pClass=rtGetRuntimeClassList()->pFirst; pClass!=NULL; pClass = pClass->m_pNextClass)
	{
        pLog->Info("  %20s <-= %-20s %-16s %d\n",
            pClass->m_lpszClassName,
            (pClass->m_pBaseClass?pClass->m_pBaseClass->m_lpszClassName:"None"),
            (pClass->m_pfnCreateObject==NULL)?"---":"Yes",
            pClass->m_nObjectSize);
	}
}

/*------------------------------------------------------------------------
	RtRuntimeClass
------------------------------------------------------------------------*/

struct RtRuntimeClass RtObject::typeRtObject =
    {"RtObject", sizeof(RtObject), 0, NULL, NULL, NULL};

rt2_core::RtObject* RtRuntimeClass::CreateObject()
{
     if (m_pfnCreateObject == NULL)
     {
          RtCoreLog().Error("Error: Trying to create object which is not RT_DECLARE_DYNCREATE: %hs.\n", m_lpszClassName);
          return NULL;
     }

    RtObject* pObject = NULL;

#ifdef  _DEBUG
    try {
        pObject = (*m_pfnCreateObject)();
    }catch(...)
    {
        RtCoreLog().Error("创建失败.\n", m_lpszClassName);
    }
#else
    pObject = (*m_pfnCreateObject)();
#endif

    return pObject;
}

BOOL RtRuntimeClass::IsDerivedFrom(const RtRuntimeClass* pBaseClass) const
{
	funguard;
    // simple SI case
    const RtRuntimeClass* pClassThis = this;
    while (pClassThis != NULL)
    {
        if (pClassThis == pBaseClass)
            return TRUE;
        pClassThis = pClassThis->m_pBaseClass;
    }
    return FALSE;    // walked to the top, no match
	fununguard;
}

void RtRuntimeClass::Store(RtArchive& ar) const
{
	funguard;
    CHECK(ar.IsStoring());
//    CHECK(m_wClassID!=0xFFFF);
    if (m_wClassID==0)
    {
	    BYTE nLen = (BYTE)strlen(m_lpszClassName);
	    ar << (WORD&)m_wClassID << nLen;
	    ar.Serialize((void*)m_lpszClassName, nLen*sizeof(char));
    }else
    {
		WORD wLen = (WORD)strlen(m_lpszClassName);
        ar << (WORD&)m_wClassID << wLen;
		ar.Serialize((void*)m_lpszClassName, wLen);
    }
	fununguard;
}

RtRuntimeClass* RtRuntimeClass::Load(RtArchive& ar, UINT* pwSchemaNum)
{
	funguard;
	char szClassName[64];
	RtRuntimeClass* pClass;

    CHECK(ar.IsLoading());

//    BYTE cSaveFlag;
    WORD wClassID;
	ar << wClassID;

    if (pwSchemaNum)
    {
        *pwSchemaNum = 0;
    }

    if (wClassID==0)
    {
	    BYTE nLen;
        ar << nLen;
	    if (nLen >= countof(szClassName)) return NULL;
        ar.Serialize((void*)szClassName, nLen*sizeof(char));
	    szClassName[nLen] = '\0';
        // search app specific classes
        for (pClass=rtGetRuntimeClassList()->pFirst; pClass!=NULL; pClass = pClass->m_pNextClass)
	    {
		    if (strcmp(szClassName, pClass->m_lpszClassName) == 0)
		    {
			    return pClass;
		    }
	    }
    }else if (wClassID==0xFFFF)
    {
        WORD wLen;
        ar << wLen;
	    if (wLen >= countof(szClassName)) return NULL;
        ar.Serialize((void*)szClassName, wLen*sizeof(char));
	    szClassName[wLen] = '\0';
        // search app specific classes
        for (pClass=rtGetRuntimeClassList()->pFirst; pClass!=NULL; pClass = pClass->m_pNextClass)
	    {
		    if (strcmp(szClassName, pClass->m_lpszClassName) == 0)
		    {
			    return pClass;
		    }
	    }
    }else
    {
        ar << wClassID;
        // search app specific classes
        for (pClass=rtGetRuntimeClassList()->pFirst; pClass!=NULL; pClass = pClass->m_pNextClass)
	    {
            if (wClassID==pClass->m_wClassID)
		    {
			    return pClass;
		    }
	    }
    }

    RtCoreLog().Warn("Warning: Cannot load %hs from archive.  Class not defined.\n", szClassName);
	return NULL; // not found
	fununguard;
}

/*------------------------------------------------------------------------
	RtObject
------------------------------------------------------------------------*/

RtObject::RtObject()
    : m_state(Object_State_Ready)
{
}

RtObject::~RtObject()
{
}

BOOL RtObject::IsSerializable() const
{
    return FALSE;
}

RtRuntimeClass* RtObject::GetRuntimeClass() const
{
    return RT_RUNTIME_CLASS(RtObject);
}

BOOL RtObject::IsKindOf(const RtRuntimeClass* pClass) const
{
    // simple SI case
    RtRuntimeClass* pClassThis = GetRuntimeClass();
    return pClassThis->IsDerivedFrom(pClass);
}

long RtObject::Serialize(RtArchive& ar)
{
    RtCoreLog().Error("Error: Trying to serialize object which is not RT_DECLARE_DYNCREATE: %hs.\n", GetRuntimeClass()->m_lpszClassName);
    return 0;
}


} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
