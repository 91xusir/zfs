// XTPMarkupDeviceDependentImage.h: interface for the CXTPMarkupDeviceDependentImage class.
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
#if !defined(__XTPMARKUPDEVICEDEPENDENTIMAGE_H__)
#define __XTPMARKUPDEVICEDEPENDENTIMAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupContext;

//===========================================================================
// Summary: Describes image site interface.
//===========================================================================
struct IXTPMarkupDeviceDependentImageSite
{
	//-----------------------------------------------------------------------
	// Summary: Implementation must to return a valid pointer to markup
	//          context in which image is to be rendered.
	// Returns: Valid markup context pointer.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupContext* GetMarkupContext() = 0;

	//-----------------------------------------------------------------------
	// Summary: Optional implementation can re-draw context when this method
	//          is called, usually when new animation frame is to be rendered.
	//-----------------------------------------------------------------------
	virtual void OnImageUpdateRequired() = 0;
};


//===========================================================================
// Summary: Implements abstract device dependent image. Be it GDI or GDI+
//          device context, each have it's own implementation. optimization,
//          set of supported formats, capabilities.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupDeviceDependentImage : public CObject
{
	DECLARE_DYNAMIC(CXTPMarkupDeviceDependentImage);

protected:
	//-----------------------------------------------------------------------
	// Summary: Constructs device dependent image object.
	// Parameters: pSite - A valid pointer to image site interface.
	//-----------------------------------------------------------------------
	explicit CXTPMarkupDeviceDependentImage(IXTPMarkupDeviceDependentImageSite* pSite);

public:
	//-----------------------------------------------------------------------
	// Summary: Handles device dependent image object deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupDeviceDependentImage();

	//-----------------------------------------------------------------------
	// Summary: Loads image from the resource specified.
	// Parameters:
	//     lpszSource - Poitner to a string that indicates image source. Can
	//                  be a relative or full file path, file:// path, res://
	//                  path or an integer in decimal representation that indicates
	//                  icon index.
	//     nWidth - The width of image to be loaded. Used only for icons. Can
	//              be 0 if not relevant.
	// Returns: TRUE if an image is loaded successfuly.
	//-----------------------------------------------------------------------
	virtual BOOL Load(LPCWSTR lpszSource, int nWidth) = 0;

	//-----------------------------------------------------------------------
	// Summary: Obtains image size in pixels.
	// Returns: Image size.
	//-----------------------------------------------------------------------
	const CSize& GetSize() const;

	//-----------------------------------------------------------------------
	// Summary: Checks if an image is loaded.
	// Returns: TRUE if an image is loaded.
	//-----------------------------------------------------------------------
	BOOL IsLoaded() const;

	//-----------------------------------------------------------------------
	// Summary: Checks if an image loaded is an icon.
	// Returns: TRUE if an image loaded is an icon.
	//-----------------------------------------------------------------------
	BOOL IsIcon() const;

	//-----------------------------------------------------------------------
	// Summary: Obtains site object pointer.
	// Returns: Site object pointer.
	//-----------------------------------------------------------------------
	IXTPMarkupDeviceDependentImageSite* GetSite() const;

protected:
	IXTPMarkupDeviceDependentImageSite* m_pSite;
	CSize m_imageSize;
	BOOL m_bIsIcon;
};

AFX_INLINE const CSize& CXTPMarkupDeviceDependentImage::GetSize() const
{
	return m_imageSize;
}

AFX_INLINE BOOL CXTPMarkupDeviceDependentImage::IsLoaded() const
{
	return (0 != m_imageSize.cx && 0 != m_imageSize.cy);
}

AFX_INLINE BOOL CXTPMarkupDeviceDependentImage::IsIcon() const
{
	return m_bIsIcon;
}

AFX_INLINE IXTPMarkupDeviceDependentImageSite* CXTPMarkupDeviceDependentImage::GetSite() const
{
	return m_pSite;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif /*__XTPMARKUPDEVICEDEPENDENTIMAGE_H__*/
