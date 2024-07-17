// XTPMarkupGdiPlusImage.h: interface for the CXTPMarkupGdiPlusImage class.
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
#if !defined(__XTPMARKUPGDIPLUSIMAGE_H__)
#define __XTPMARKUPGDIPLUSIMAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

namespace Gdiplus
{
	class GpImage;
	class PropertyItem;
}

class CXTPGdiPlus;
class CXTPImageManager;
class CXTPImageManagerIcon;
struct XTP_URI;
struct IXTPMarkupDeviceDependentImageSite;

//===========================================================================
// Summary: Implements abstract GDI+ device dependent image.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupGdiPlusImage : public CXTPMarkupDeviceDependentImage
{
	DECLARE_DYNAMIC(CXTPMarkupGdiPlusImage);

public:
	//-----------------------------------------------------------------------
	// Summary: Constructs device dependent image object.
	// Parameters:
	//     pGdiPlus - GDI+ API pointer.
	//     pSite - A valid pointer to image site interface.
	//-----------------------------------------------------------------------
	CXTPMarkupGdiPlusImage(
		CXTPGdiPlus* pGdiPlus,
		IXTPMarkupDeviceDependentImageSite* pSite);

	//-----------------------------------------------------------------------
	// Summary: Handles device dependent image object deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupGdiPlusImage();

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
	// Summary: Obtains GDI+ wrapped image pointer.
	// Returns: GDI+ wrapped image pointer or NULL.
	//-----------------------------------------------------------------------
	Gdiplus::GpImage* GetGpImage();

	//-----------------------------------------------------------------------
	// Summary: Obtains icon wrapped image pointer.
	// Returns: Icon wrapped image pointer or NULL.
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* GetIcon();

private:
	typedef CMap<
		UINT_PTR,
		UINT_PTR,
		CXTPMarkupGdiPlusImage*,
		CXTPMarkupGdiPlusImage*> AnimationTimerData;
	static AnimationTimerData& AFX_CDECL GetAnimationTimerData();
	static BOOL AFX_CDECL ParseIconSource(LPCWSTR lpszSource, UINT& nIconId);
	BOOL LoadIcon(UINT nIconId, int nWidth);
	BOOL LoadIcon(const XTP_URI& uri, int nWidth);
	BOOL LoadImage(const XTP_URI& uri);
	void Cleanup();
	void LoadAnimationData();
	void RenderAnimation();
	void StopAnimation();
	static void CALLBACK OnAnimationTimer(
		HWND hWnd,
		UINT uMsg,
		UINT_PTR nTimerId,
		DWORD dwTime);

	CXTPGdiPlus* m_pGdiPlus;
	Gdiplus::GpImage* m_pImage;
	UINT m_nFrameCount;
	UINT m_nCurrentFrame;
	Gdiplus::PropertyItem* m_pDelayProperty;
	UINT_PTR m_nTimerId;
	CXTPImageManager* m_pImageManager;
	CXTPImageManagerIcon* m_pIcon;
};

AFX_INLINE Gdiplus::GpImage* CXTPMarkupGdiPlusImage::GetGpImage()
{
	return m_pImage;
}

AFX_INLINE CXTPImageManagerIcon* CXTPMarkupGdiPlusImage::GetIcon()
{
	return m_pIcon;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif /*__XTPMARKUPGDIPLUSIMAGE_H__*/
