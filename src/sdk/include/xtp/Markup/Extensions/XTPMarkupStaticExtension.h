// XTPMarkupStaticExtension.h: interface for the CXTPMarkupStaticExtension class.
//
// (c)1998-2018 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPMARKUPSTATICEXTENSION_H__)
#define __XTPMARKUPSTATICEXTENSION_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupObject;
class CXTPMarkupColor;

//===========================================================================
// Summary: A base class for a markup static extension composite object.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtension
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs composite or property CXTPMarkupStaticExtension instance.
	// Parameters:
	//     lpzName - A pointer to extension name.
	//     bProperty - TRUE is an object is a property object, e.i. it cannot be
	//             extended with other extensions and it provides a final
	//             value of an extension.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtension(LPCWSTR lpzName, BOOL bProperty);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs composite CXTPMarkupStaticExtension instance.
	// Parameters:
	//     lpzName - A pointer to extension name.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtension(LPCWSTR lpzName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupStaticExtension object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupStaticExtension();

	//-----------------------------------------------------------------------
	// Summary:
	//     Parses static extension expression of form Name.Value[.Value[...]].
	//     Where both Name and Value has to be of form [a-zA-Z\_]+.
	// Parameters:
	//     lpzValue - Entire static extension value.
	//     lpzSubValue - On successful completion contains a pointer to sub-value 
	//                   of the extension object.
	// Returns:
	//     TRUE if parsed successfuly.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL Parse(LPCWSTR lpzValue, LPCWSTR& lpzSubValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains extension name.
	// Returns:
	//     Extension name reference.
	//-----------------------------------------------------------------------
	LPCWSTR GetName() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains extension property flag.
	// Returns:
	//     TRUE if extension is a property extension.
	//-----------------------------------------------------------------------
	BOOL IsProperty() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Extends an extension object with another sub-extension.
	// Parameters:
	//     pExtension - A pointer to a new sub-extension object to be
	//                  added to the current extension. An object will be deallocated
	//                  during parent extension destruction.
	//-----------------------------------------------------------------------
	void Extend(CXTPMarkupStaticExtension* pExtension);

	//-----------------------------------------------------------------------
	// Summary:
	//     Tries to find an extension by its name or relative path.
	// Parameters:
	//     lpzName - A pointer to extension name or relative path of form Name[.Name[...]].
	// Returns:
	//     A pointer to an extension if such is found, otherwise returns NULL.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtension* FindExtension(LPCWSTR lpzName) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Provides a value of a composite extension object.
	// Parameters:
	//     lpzValue - Composite extension value of form Value[.Value[...]]
	// Returns:
	//     A pointer to a markup object that represents a value of the composite
	//     extension object or NULL if no value found. A caller is responsible for
	//     releasing an object.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupObject* ProvideValue(LPCWSTR lpzValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Provides a value of a property extension object.
	// Returns:
	//     A pointer to a markup object that represents a value of the property
	//     extension object or NULL if no value found. A caller is responsible for
	//     releasing an object.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupObject* ProvideValue();

private:
	static BOOL AFX_CDECL ValidateExtensionName(LPCWSTR lpzBegin, LPCWSTR lpzEnd);

private:
	BOOL m_bProperty;
	LPWSTR m_lpzName;
	CMap<LPCWSTR, LPCWSTR, CXTPMarkupStaticExtension*, CXTPMarkupStaticExtension*> m_mapExtensions;
};

AFX_INLINE LPCWSTR CXTPMarkupStaticExtension::GetName() const
{
	return m_lpzName;
}

AFX_INLINE BOOL CXTPMarkupStaticExtension::IsProperty() const
{
	return m_bProperty;
}




//===========================================================================
// Summary: A convenience base class for all static extensions property.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionProperty : public CXTPMarkupStaticExtension
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs CXTPMarkupStaticExtensionProperty with optional initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     pObject - An optional pointer to initial property value. The class is
	//               is responsible for releasing value object instance.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionProperty(LPCWSTR lpzName, CXTPMarkupObject* pObject = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupStaticExtensionProperty object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	~CXTPMarkupStaticExtensionProperty();

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets property value.
	// Parameters:
	//     pObject - An pointer to property value or NULL. The class is
	//               is responsible for releasing value object instance.
	//-----------------------------------------------------------------------
	void SetValue(CXTPMarkupObject* pObject);

	//-----------------------------------------------------------------------
	// Summary:
	//     Provides a value of a property extension object.
	// Returns:
	//     A pointer to a markup object that represents a value of the property
	//     extension object or NULL if no value found. A caller is responsible for
	//     releasing an object.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupObject* ProvideValue();

	//-----------------------------------------------------------------------
	// Summary:
	//     Provides a value of a composite extension object.
	// Parameters:
	//     lpzValue - Composite extension value of form Value[.Value[...]]
	// Returns:
	//     A pointer to a markup object that represents a value of the composite
	//     extension object or NULL if no value found. A caller is responsible for
	//     releasing an object.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupObject* ProvideValue(LPCWSTR lpzValue);

protected:
	CXTPMarkupObject* m_pObject;
};




//===========================================================================
// Summary: A convenience class that represents a string static extensions property.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionStringProperty : public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs CXTPMarkupStaticExtensionStringProperty with optional initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     lpzValue - An optional pointer to initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionStringProperty(LPCWSTR lpzName, LPCWSTR lpzValue = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs CXTPMarkupStaticExtensionStringProperty with optional initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     lpzValue - An optional pointer to initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionStringProperty(LPCWSTR lpzName, LPCSTR lpzValue = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets property value.
	// Parameters:
	//     lpzValue - An pointer to property value or NULL. The string will be duplicated.
	//-----------------------------------------------------------------------
	void SetValue(LPCWSTR lpzValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets property value.
	// Parameters:
	//     lpzValue - An pointer to property value or NULL. The string will be duplicated.
	//-----------------------------------------------------------------------
	void SetValue(LPCSTR lpzValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a pointer to a raw object value.
	// Returns:
	//     Raw object value pointer or NULL is no value is assigned.
	//-----------------------------------------------------------------------
	LPCWSTR GetRawValue() const;
};




//===========================================================================
// Summary: A convenience class that represents an integer static extensions property.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionIntegerProperty : public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs CXTPMarkupStaticExtensionIntegerProperty with optional initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     nValue - An optional initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionIntegerProperty(LPCWSTR lpzName, int nValue = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets property value.
	// Parameters:
	//     nValue - New property value.
	//-----------------------------------------------------------------------
	void SetValue(int nValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains property value.
	// Returns:
	//     Property value.
	//-----------------------------------------------------------------------
	int GetRawValue() const;
};




//===========================================================================
// Summary: A convenience class that represents a boolean static extensions property.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionBooleanProperty : public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs CXTPMarkupStaticExtensionBooleanProperty with optional initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     bValue - An optional initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionBooleanProperty(LPCWSTR lpzName, BOOL bValue = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets property value.
	// Parameters:
	//     bValue - New property value.
	//-----------------------------------------------------------------------
	void SetValue(BOOL bValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains property value.
	// Returns:
	//     Property value.
	//-----------------------------------------------------------------------
	BOOL GetRawValue() const;
};




//===========================================================================
// Summary: A convenience class that represents a double static extensions property.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionDoubleProperty : public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs CXTPMarkupStaticExtensionDoubleProperty with optional initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     dblValue - An optional initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionDoubleProperty(LPCWSTR lpzName, double dblValue = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets property value.
	// Parameters:
	//     dblValue - New property value.
	//-----------------------------------------------------------------------
	void SetValue(double dblValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains property value.
	// Returns:
	//     Property value.
	//-----------------------------------------------------------------------
	double GetRawValue() const;
};




//===========================================================================
// Summary: A convenience class that represents an RGB color static extensions property.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionColorProperty : public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs CXTPMarkupStaticExtensionColorProperty with optional initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     crValue - An optional initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionColorProperty(LPCWSTR lpzName, COLORREF crValue = RGB(0, 0, 0));

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs CXTPMarkupStaticExtensionColorProperty with optional initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     crValue - Initial property color value.
	//     bAlpha - Initial property color alpha-channel value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionColorProperty(LPCWSTR lpzName, COLORREF crValue, BYTE bAlpha);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets property value.
	// Parameters:
	//     crValue - New property value.
	//-----------------------------------------------------------------------
	void SetValue(COLORREF crValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets property value.
	// Parameters:
	//     crValue - Initial property color value.
	//     bAlpha - Initial property color alpha-channel value.
	//-----------------------------------------------------------------------
	void SetValue(COLORREF crValue, BYTE bAlpha);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains property value.
	// Returns:
	//     Property value.
	//-----------------------------------------------------------------------
	COLORREF GetRawValue() const;
};

//===========================================================================
// Summary: A convenience class that represents an RGB color static extensions property.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionBrushProperty : public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs CXTPMarkupStaticExtensionBrushProperty with optional initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     crValue - An optional initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionBrushProperty(LPCWSTR lpzName, COLORREF crValue = RGB(0, 0, 0));

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs CXTPMarkupStaticExtensionColorProperty with optional initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     crValue - Initial property color value.
	//     bAlpha - Initial property color alpha-channel value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionBrushProperty(LPCWSTR lpzName, COLORREF crValue, BYTE bAlpha);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets property value.
	// Parameters:
	//     crValue - New property value.
	//-----------------------------------------------------------------------
	void SetValue(COLORREF crValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets property value.
	// Parameters:
	//     crValue - Initial property color value.
	//     bAlpha - Initial property color alpha-channel value.
	//-----------------------------------------------------------------------
	void SetValue(COLORREF crValue, BYTE bAlpha);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains property value.
	// Returns:
	//     Property value.
	//-----------------------------------------------------------------------
	COLORREF GetRawValue() const;
};



#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSTATICEXTENSION_H__)
