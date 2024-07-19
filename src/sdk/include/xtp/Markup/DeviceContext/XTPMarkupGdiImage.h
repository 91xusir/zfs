// XTPMarkupGdiImage.h: interface for the CXTPMarkupGdiImage class.
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
#if !defined(__XTPMARKUPGDIIMAGE_H__)
#define __XTPMARKUPGDIIMAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPImageManager;
class CXTPImageManagerIcon;


//===========================================================================
// Summary: Implements abstract GDI device dependent image.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupGdiImage : public CXTPMarkupDeviceDependentImage
{
	DECLARE_DYNAMIC(CXTPMarkupGdiImage);

public:
	//-----------------------------------------------------------------------
	// Summary: Constructs device dependent image object.
	// Parameters: pSite - A valid pointer to image site interface.
	//-----------------------------------------------------------------------
	explicit CXTPMarkupGdiImage(IXTPMarkupDeviceDependentImageSite* pSite);

	//-----------------------------------------------------------------------
	// Summary: Handles device dependent image object deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupGdiImage();

public:
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
	virtual BOOL Load(LPCWSTR lpszSource, int nWidth);

	//-----------------------------------------------------------------------
	// Summary: Obtains wrapped image pointer.
	// Returns: Wrapped image pointer.
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* GetNativeImage();

private:
	CXTPImageManager* m_pImageManager;
	CXTPImageManagerIcon* m_pImage;
};

AFX_INLINE CXTPImageManagerIcon* CXTPMarkupGdiImage::GetNativeImage()
{
	return m_pImage;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif /*__XTPMARKUPGDIIMAGE_H__*/
