// XTPChartDeviceContext.h
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
#if !defined(__XTPCHARTDEVICECONTEXT_H__)
#define __XTPCHARTDEVICECONTEXT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"


class CXTPChartContainer;
class CXTPChartDeviceCommand;
class CXTPMarkupUIElement;
class CXTPChartFont;
class CXTPChartDrawingObjectFactory;

//===========================================================================
// Summary:
//     This class abstracts a device context. A device context stores,
//     retrieves, and modifies the attributes of graphic objects and
//     specifies graphic modes.This class is a kind of CXTPCmdTarget to enhance
//     the MFC command routing.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartDeviceContext : public CXTPCmdTarget
{
	DECLARE_DYNAMIC(CXTPChartDeviceContext);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartDeviceContext object.
	// Parameters:
	//     pContainer - Parent container pointer.
	//     hDC - Handle to the windows device context.
	//     bIs3D - TRUE identifies device context used for 3D rendering.
	//     pDrawingObjectFactory - Drawing object factory pointer.
	// See also:
	//     CXTPChart2dDrawingObjectFactory, CXTPChart3dDrawingObjectFactory
	//-----------------------------------------------------------------------
	CXTPChartDeviceContext(CXTPChartContainer* pContainer, HDC hDC, BOOL bIs3D, CXTPChartDrawingObjectFactory* pDrawingObjectFactory);

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPChartDeviceContext object, handles cleanup
	//-------------------------------------------------------------------------
	virtual ~CXTPChartDeviceContext() = 0;

	//-------------------------------------------------------------------------
	// Summary:
	//     Initializes devices context.
	// Returns:
	//     TRUE if initialization is successful.
	//-------------------------------------------------------------------------
	virtual BOOL Initialize();

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to trigger the drawing.
	// Parameters:
	//     pCommand - A pointer to chart device command object.
	//-------------------------------------------------------------------------
	virtual void Execute(CXTPChartDeviceCommand* pCommand);

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if the derived device context is a 3D device context.
	// Returns:
	//      TRUE if the derived device context is a 3D device context.
	//-----------------------------------------------------------------------
	BOOL Is3D() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to get the windows device context.
	// Returns:
	//     A handle to the windows device context.
	//-------------------------------------------------------------------------
	HDC GetHDC() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Provides access to the parent container object.
	// Returns:
	//      Parent container object pointer.
	//-----------------------------------------------------------------------
	CXTPChartContainer* GetContainer() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Provides access to the drawing object factory used by 
	//      the device context implementation.
	// Returns:
	//      Drawing object factory instance pointer.
	//-----------------------------------------------------------------------
	CXTPChartDrawingObjectFactory* GetDrawingObjectFactory() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to get the measurements of texts rendered in a
	//     device context using a specific font.
	// Parameters:
	//     pText - The string to be measured.
	//     pFont - Pointer to chart font object.
	//-------------------------------------------------------------------------
	virtual CXTPChartSizeF MeasureString(const CXTPChartString* pText, CXTPChartFont* pFont) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//      Measures a markup element size provided with the fond provided
	//      set as default font for the associated markup context.
	// Parameters:
	//      pMarkupUIElement - Markup element pointer to measure.
	//      pFont - A default markup context font pointer.
	// Returns:
	//      Masured markup element size or empty size in case of an error.
	//-----------------------------------------------------------------------
	virtual CXTPChartSizeF MeasureMarkupElement(CXTPMarkupUIElement* pMarkupUIElement, CXTPChartFont* pFont) = 0;

protected:
	const BOOL m_bIs3D;         // TRUE identifies device context used for 3D rendering
	HDC m_hDC;                  // The windows device context.
	CXTPChartContainer* m_pContainer; // Parent container pointer.
	CXTPChartDrawingObjectFactory* m_pDrawingObjectFactory; // Drawing object factory pointer.
};

AFX_INLINE BOOL CXTPChartDeviceContext::Is3D() const {
	return m_bIs3D;
}

AFX_INLINE HDC CXTPChartDeviceContext::GetHDC() const {
	return m_hDC;
}

AFX_INLINE CXTPChartContainer* CXTPChartDeviceContext::GetContainer() const {
	return m_pContainer;
}

AFX_INLINE CXTPChartDrawingObjectFactory* CXTPChartDeviceContext::GetDrawingObjectFactory() const {
	return m_pDrawingObjectFactory;
}

//-----------------------------------------------------------------------
// Summary:
//      A base implementation of 3D device context.
// See also:
//      CXTPChartDeviceContext
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChart3dDeviceContext : public CXTPChartDeviceContext
{
	DECLARE_DYNAMIC(CXTPChart3dDeviceContext);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChart3dDeviceContext object.
	// Parameters:
	//     pContainer - Parent container pointer.
	//     hDC - Handle to the windows device context.
	//     rcBounds - Rectangle that defined device context boundaries.
	//     pDrawingObjectFactory - Drawing object factory pointer.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChart3dDeviceContext(CXTPChartContainer* pContainer, HDC hDC,
		CRect rcBounds, CXTPChartDrawingObjectFactory* pDrawingObjectFactory);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Rotates current matrix.
	// Parameters:
	//      rotation - Matrix rotation arguments.
	//-----------------------------------------------------------------------
	virtual void Rotate(const CXTPChart3dRotation& rotation) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//      Projects 3D coordinates to 2D space point coordinates.
	// Parameters:
	//      pt3d - 3D space point coordinates.
	//      pt2d - Result 2D space point coordinates.
	//      pdWinZ - If specified the result value is within range 0..1
	//               where 0 defines the near plane and 1 defines the far
	//               plane defined by the view frustum. 
	//      box - 3D box to map to the outer 2D rectangle.
	//      rc - Resuylt 2D outer rectangle for the 3D box specified.
	// Returns:
	//      TRUE if coordinates are mapped successfully.
	// See also:
	//      Unproject
	//-----------------------------------------------------------------------
	virtual BOOL Project(const CXTPPoint3d& pt3d, CPoint& pt2d, double* pdWinZ = NULL) = 0;
	virtual BOOL Project(const CXTPChart3dBox& box, CRect& rc) = 0; // <combine CXTPChart3dDeviceContext::Project@const CXTPPoint3d&@CPoint&@double*>

	//-----------------------------------------------------------------------
	// Summary:
	//      Projects 2D space point coordinates to 3D space point coordinate 
	//      on the specified plane.
	// Parameters:
	//      pt2d - 2D space point coordinates.
	//      pt3d - Result 3D space point coordinates.
	//      dWinZ - Determines how far the point is from the near plane.
	//              The value must be within 0..1 range where 0 determines
	//              the near frustum plane and 1 determines the far plane.
	// Returns:
	//      TRUE is the coordinates are mapped successfully.
	// See also:
	//      Project
	//-----------------------------------------------------------------------
	virtual BOOL Unproject(const CPoint& pt2d, CXTPPoint3d& pt3d, double dWinZ = 0) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//      Transforms vector or box coordinates.
	// Parameters:
	//      pTranslation - Optional translation offsets to add to the vector or box vertices.
	//      pRotation - Optional vector opr box vertices rotation parameters.
	//-----------------------------------------------------------------------
	virtual void Transform(CXTPPoint3d& v,
		const CXTPPoint3d* pTranslation = NULL,
		const CXTPChart3dRotation* pRotation = NULL) = 0;
	virtual void Transform(CXTPChart3dBox& box,
		const CXTPPoint3d* pTranslation = NULL,
		const CXTPChart3dRotation* pRotation = NULL) = 0; // <combine CXTPChart3dDeviceContext::Transform@CXTPPoint3d&@const CXTPPoint3d*@const CXTPChart3dRotation*>

	//-----------------------------------------------------------------------
	// Summary:
	//      Performs depth dest for 2D point provided.
	// Parameters:
	//      point - Viewport offset for which depth test is to be performed.
	//      dWinZ - If succeedes the value contains the distance from the near (0)
	//              plane of the view frustum to the fist visible point.
	// Returns:
	//      TRUE if depth test is successful.
	//-----------------------------------------------------------------------
	virtual BOOL DepthTest(CPoint point, double& dWinZ) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains device contains boundary rectangle.
	// Returns:
	//      Device contains boundary rectangle.
	//-----------------------------------------------------------------------
	const CRect& GetBounds() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Checks if device context is currently rendering anti-aliasing.
	// Returns:
	//      TRUE if device context is currently rendering anti-aliasing.
	// See also:
	//      IsSkippingAntialiasing
	//-----------------------------------------------------------------------
	BOOL IsRenderingAntialiasing() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Checks if device context is skipping anti-aliasing rendering.
	// Returns:
	//      TRUE if device context is skipping anti-aliasing rendering.
	// See also:
	//      IsRenderingAntialiasing
	//-----------------------------------------------------------------------
	BOOL IsSkippingAntialiasing() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Is called when camera state is updated.
	// Parameters:
	//      ptPosition - New camera position.
	//      ptFocal - New camera focal point.
	//      ptUpVector - New camera up vector.
	//-----------------------------------------------------------------------
	void OnUpdateCameraState(const CXTPPoint3f& ptPosition, const CXTPPoint3f& ptFocal, const CXTPPoint3f& ptUpVector);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets limits for distance between view point and scene center point.
	// Parameters:
	//      fMinDistance - Minimal distance allowed.
	//      fMaxDistance = Maximal distance allowed.
	//-----------------------------------------------------------------------
	void SetModelDistanceLimits(float fMinDistance, float fMaxDistance);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets/obtains light position normalized vector where
	//      each component is within range [0..1].
	// Parameters:
	//      position - New light vector.
	// Returns:
	//      Current light vector.
	//-----------------------------------------------------------------------
	void SetLightPosition(const CXTPPoint3f& position);
	const CXTPPoint3f& GetLightPosition() const; // <combine CXTPChart3dDeviceContext::SetLightPosition@const CXTPPoint3f&>

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets/obtains light ambient color.
	// Parameters:
	//      color - New light ambient color.
	// Returns:
	//      Current light ambient color.
	//-----------------------------------------------------------------------
	void SetLightAmbientColor(const CXTPChartColor& color);
	const CXTPChartColor& GetLightAmbientColor() const; // <combine CXTPChart3dDeviceContext::SetLightAmbientColor@const CXTPChartColor&>

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets/obtains light diffuse color.
	// Parameters:
	//      color - New light diffuse color.
	// Returns:
	//      Current light diffuse color.
	//-----------------------------------------------------------------------
	void SetLightDiffuseColor(const CXTPChartColor& color);
	const CXTPChartColor& GetLightDiffuseColor() const; // <combine CXTPChart3dDeviceContext::SetLightDiffuseColor@const CXTPChartColor&>

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets/obtains light specular color.
	// Parameters:
	//      color - New light specular color.
	// Returns:
	//      Current light specular color.
	//-----------------------------------------------------------------------
	void SetLightSpecularColor(const CXTPChartColor& color);
	const CXTPChartColor& GetLightSpecularColor() const; // <combine CXTPChart3dDeviceContext::SetLightSpecularColor@const CXTPChartColor&>

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets/obtains light model ambient color.
	// Parameters:
	//      color - New light model ambient color.
	// Returns:
	//      Current light model ambient color.
	//-----------------------------------------------------------------------
	void SetLightModelAmbientColor(const CXTPChartColor& color);
	const CXTPChartColor& GetLightModelAmbientColor() const; // <combine CXTPChart3dDeviceContext::SetLightModelAmbientColor@const CXTPChartColor&>

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets/obtains material emission color.
	// Parameters:
	//      color - New material emission color.
	// Returns:
	//      Current material emission color.
	//-----------------------------------------------------------------------
	void SetMaterialEmissionColor(const CXTPChartColor& color);
	const CXTPChartColor& GetMaterialEmissionColor() const; // <combine CXTPChart3dDeviceContext::SetMaterialEmissionColor@const CXTPChartColor&>

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets/obtains material diffuse color.
	// Parameters:
	//      color - New material diffuse color.
	// Returns:
	//      Current material diffuse color.
	//-----------------------------------------------------------------------
	void SetMaterialDiffuseColor(const CXTPChartColor& color);
	const CXTPChartColor& GetMaterialDiffuseColor() const; // <combine CXTPChart3dDeviceContext::SetMaterialDiffuseColor@const CXTPChartColor&>

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets/obtains material specular color.
	// Parameters:
	//      color - New material specular color.
	// Returns:
	//      Current material specular color.
	//-----------------------------------------------------------------------
	void SetMaterialSpecularColor(const CXTPChartColor& color);
	const CXTPChartColor& GetMaterialSpecularColor() const; // <combine CXTPChart3dDeviceContext::SetMaterialSpecularColor@const CXTPChartColor&>

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets/obtains material shininess factor, a value within range [0..1].
	// Parameters:
	//      color - New material shininess factor.
	// Returns:
	//      Current material shininess factor.
	//-----------------------------------------------------------------------
	void SetMaterialShininess(float fShininess);
	float GetMaterialShininess() const; // <combine CXTPChart3dDeviceContext::SetMaterialShininess@float>

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Resets saved camera state to default zero values.
	//-----------------------------------------------------------------------
	void ResetSavedCameraState();

protected:
	const CRect m_rcBounds; // Device context bounary rectangle.
	BOOL m_bRenderingAntialiasing; // TRUE if currently rendering anti-aliasing
	BOOL m_bSkippingAntialiasing; // TRUE if skipping anti-aliasing rendering

	struct CameraState
	{
		CXTPPoint3f ptPosition;
		CXTPPoint3f ptFocal;
		CXTPPoint3f ptUpVector;
	} m_cameraState; // Saved camera state

	float m_fMinModelDistance; // Minimal distance allowed to the center of the scene.
	float m_fMaxModelDistance; // Maximal distance allowed to the center of the scene.

	CXTPPoint3f m_ptLightPosition; // Light position vector, each component is a normalized value in range [0..1].
	CXTPChartColor m_clrLightAmbientColor; // Light ambient color.
	CXTPChartColor m_clrLightDiffuseColor; // Light diffuse color.
	CXTPChartColor m_clrLightSpecularColor; // Light specular color.
	CXTPChartColor m_clrLightModelAmbientColor; // Light model ambient color.
	CXTPChartColor m_clrMaterialEmissionColor; // Material emission color.
	CXTPChartColor m_clrMaterialDiffuseColor; // Material diffuse color.
	CXTPChartColor m_clrMaterialSpecularColor; // Material specular color.
	float m_fMaterialShininess; // Material shininess factor in range [0..1].
};

AFX_INLINE const CRect& CXTPChart3dDeviceContext::GetBounds() const {
	return m_rcBounds;
}

AFX_INLINE BOOL CXTPChart3dDeviceContext::IsRenderingAntialiasing() const {
	return m_bRenderingAntialiasing;
}

AFX_INLINE BOOL CXTPChart3dDeviceContext::IsSkippingAntialiasing() const {
	return m_bSkippingAntialiasing;
}

AFX_INLINE void CXTPChart3dDeviceContext::SetModelDistanceLimits(float fMinDistance, float fMaxDistance) {
	m_fMinModelDistance = fMinDistance;
	m_fMaxModelDistance = fMaxDistance;
}

AFX_INLINE void CXTPChart3dDeviceContext::SetLightPosition(const CXTPPoint3f& position) {
	m_ptLightPosition = position;
}

AFX_INLINE const CXTPPoint3f& CXTPChart3dDeviceContext::GetLightPosition() const {
	return m_ptLightPosition;
}

AFX_INLINE void CXTPChart3dDeviceContext::SetLightAmbientColor(const CXTPChartColor& color) {
	m_clrLightAmbientColor = color;
}

AFX_INLINE const CXTPChartColor& CXTPChart3dDeviceContext::GetLightAmbientColor() const {
	return m_clrLightAmbientColor;
}

AFX_INLINE void CXTPChart3dDeviceContext::SetLightDiffuseColor(const CXTPChartColor& color) {
	m_clrLightDiffuseColor = color;
}

AFX_INLINE const CXTPChartColor& CXTPChart3dDeviceContext::GetLightDiffuseColor() const {
	return m_clrLightDiffuseColor;
}

AFX_INLINE void CXTPChart3dDeviceContext::SetLightSpecularColor(const CXTPChartColor& color) {
	m_clrLightSpecularColor = color;
}

AFX_INLINE const CXTPChartColor& CXTPChart3dDeviceContext::GetLightSpecularColor() const {
	return m_clrLightSpecularColor;
}

AFX_INLINE void CXTPChart3dDeviceContext::SetLightModelAmbientColor(const CXTPChartColor& color) {
	m_clrLightModelAmbientColor = color;
}

AFX_INLINE const CXTPChartColor& CXTPChart3dDeviceContext::GetLightModelAmbientColor() const {
	return m_clrLightModelAmbientColor;
}

AFX_INLINE void CXTPChart3dDeviceContext::SetMaterialEmissionColor(const CXTPChartColor& color) {
	m_clrMaterialEmissionColor = color;
}

AFX_INLINE const CXTPChartColor& CXTPChart3dDeviceContext::GetMaterialEmissionColor() const {
	return m_clrMaterialEmissionColor;
}

AFX_INLINE void CXTPChart3dDeviceContext::SetMaterialDiffuseColor(const CXTPChartColor& color) {
	m_clrMaterialDiffuseColor = color;
}

AFX_INLINE const CXTPChartColor& CXTPChart3dDeviceContext::GetMaterialDiffuseColor() const {
	return m_clrMaterialDiffuseColor;
}

AFX_INLINE void CXTPChart3dDeviceContext::SetMaterialSpecularColor(const CXTPChartColor& color) {
	m_clrMaterialSpecularColor = color;
}

AFX_INLINE const CXTPChartColor& CXTPChart3dDeviceContext::GetMaterialSpecularColor() const {
	return m_clrMaterialSpecularColor;
}

AFX_INLINE void CXTPChart3dDeviceContext::SetMaterialShininess(float fShininess) {
	m_fMaterialShininess = fShininess;
}

AFX_INLINE float CXTPChart3dDeviceContext::GetMaterialShininess() const {
	return m_fMaterialShininess;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTDEVICECONTEXT_H__)
