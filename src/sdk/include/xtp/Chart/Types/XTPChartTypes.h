// XTPChartTypes.h
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
#if !defined(__XTPCHARTTYPES_H__)
#define __XTPCHARTTYPES_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPPropExchange;

typedef CXTPPoint2f CXTPChartPointF, CXTPChartPoint2dF;
typedef CXTPPoint3f CXTPChartPoint3dF;
typedef CXTPSizeF   CXTPChartSizeF;
typedef CXTPRectF   CXTPChartRectF;

typedef CArray<CXTPChartPoint2dF, const CXTPChartPoint2dF&> CXTPChartPoints, CXTPChartPoints2d;
typedef CArray<CXTPChartPoint3dF, const CXTPChartPoint3dF&> CXTPChartPoints3d;
typedef CArray<float, float> CXTPChartLineDashArray;
typedef CString CXTPChartString;

//===========================================================================
// Summary:
//     This class abstracts a color entity.This object consists of an ARGB
//     value which represents a color.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartColor
{
public:
	typedef DWORD ARGB;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor, creates a CXTPChartColor object and initializes
	//     the values.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartColor()
	{
		Argb = CXTPChartColor::Empty;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Overloaded constructor, creates an opaque color object from the
	//     specified Red, Green, Blue values.
	// Parameters:
	//     r - Red value.
	//     g - Green value.
	//     b - Blue value.
	// Remarks:
	//     The alpha channel value is set to 255 hence the color is 100% opaque
	//     Color values are not premultiplied.
	//-----------------------------------------------------------------------
	CXTPChartColor(BYTE r, BYTE g, BYTE b)
	{
		Argb = MakeARGB(255, r, g, b);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Overloaded constructor, creates a color object from the specified
	//     Red, Green, Blue values.
	// Parameters:
	//     r - Red value.
	//     g - Green value.
	//     b - Blue value.
	// Remarks:
	//     Color values are not premultiplied.
	//-----------------------------------------------------------------------
	CXTPChartColor(BYTE a, BYTE r, BYTE g, BYTE b)
	{
		Argb = MakeARGB(a, r, g, b);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Overloaded constructor, creates a color object from the specified
	//     premultiplied ARGB values.
	// Parameters:
	//     argb - The ARGB value.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartColor(ARGB argb)
	{
		Argb = argb;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the alpha channel value of the color.
	// Returns:
	//     A BYTE value specifying the alpha value, the range is from
	//     0 to 255.
	// Remarks:
	//-----------------------------------------------------------------------
	BYTE GetAlpha() const
	{
		return (BYTE) (Argb >> AlphaShift);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the alpha channel value of the color.
	// Returns:
	//     A BYTE value specifying the alpha value, the range is from
	//     0 to 255.
	// Remarks:
	//-----------------------------------------------------------------------
	BYTE GetA() const
	{
		return GetAlpha();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the RED channel value of the color.
	// Returns:
	//     A BYTE value specifying the RED value, the range is from
	//     0 to 255.
	// Remarks:
	//-----------------------------------------------------------------------
	BYTE GetRed() const
	{
		return (BYTE) (Argb >> RedShift);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the RED channel value of the color.
	// Returns:
	//     A BYTE value specifying the RED value, the range is from
	//     0 to 255.
	// Remarks:
	//-----------------------------------------------------------------------
	BYTE GetR() const
	{
		return GetRed();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the GREEN channel value of the color.
	// Returns:
	//     A BYTE value specifying the GREEN value, the range is from
	//     0 to 255.
	// Remarks:
	//-----------------------------------------------------------------------
	BYTE GetGreen() const
	{
		return (BYTE) (Argb >> GreenShift);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the GREEN channel value of the color.
	// Returns:
	//     A BYTE value specifying the GREEN value, the range is from
	//     0 to 255.
	// Remarks:
	//-----------------------------------------------------------------------
	BYTE GetG() const
	{
		return GetGreen();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the BLUE channel value of the color.
	// Returns:
	//     A BYTE value specifying the BLUE value, the range is from
	//     0 to 255.
	// Remarks:
	//-----------------------------------------------------------------------
	BYTE GetBlue() const
	{
		return (BYTE) (Argb >> BlueShift);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the BLUE channel value of the color.
	// Returns:
	//     A BYTE value specifying the BLUE value, the range is from
	//     0 to 255.
	// Remarks:
	//-----------------------------------------------------------------------
	BYTE GetB() const
	{
		return GetBlue();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Clamps alpha color component to range 0..1
	// Returns:
	//     Alpha component value clamped to range 0..1.
	//-----------------------------------------------------------------------
	float ClampA() const
	{
		return static_cast<float>(GetA()) / 255;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Clamps red color component to range 0..1
	// Returns:
	//     Red component value clamped to range 0..1.
	//-----------------------------------------------------------------------
	float ClampR() const
	{
		return static_cast<float>(GetR()) / 255;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Clamps green color component to range 0..1
	// Returns:
	//     Green component value clamped to range 0..1.
	//-----------------------------------------------------------------------
	float ClampG() const
	{
		return static_cast<float>(GetG()) / 255;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Clamps blue color component to range 0..1
	// Returns:
	//     Blue component value clamped to range 0..1.
	//-----------------------------------------------------------------------
	float ClampB() const
	{
		return static_cast<float>(GetB()) / 255;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the ARGB value of the color object.
	// Returns:
	//     A ARGB value specifying the color.
	// Remarks:
	//-----------------------------------------------------------------------
	ARGB GetValue() const
	{
		return Argb;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to set the ARGB value of the color object.
	// Parameters:
	//     argb - A ARGB value specifying the color.
	// Remarks:
	//-----------------------------------------------------------------------
	VOID SetValue(ARGB argb)
	{
		Argb = argb;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to set the opaque color value from a COLORREF value.
	// Parameters:
	//     rgb - A RGB value specifying the color.
	//     The alpha channel value is set to 255 hence the color is 100% opaque
	// Remarks:
	//-----------------------------------------------------------------------
	VOID SetFromCOLORREF(COLORREF rgb)
	{
		Argb = MakeARGB(255, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to set the color value from a string value.
	// Parameters:
	//     lpsz - The string contains the color values in a specified format.
	// Remarks:
	//     The format should be "100,100,100,100" for ARGB values
	//     or "100,100,100" for opaque color values
	//-----------------------------------------------------------------------
	void SetFromString(LPCTSTR lpsz);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to convert the object to a COLORREF value.
	// Returns:
	//     A ARGB value specifying the color.
	// Remarks:
	//-----------------------------------------------------------------------
	COLORREF ToCOLORREF() const
	{
		return RGB(GetRed(), GetGreen(), GetBlue());
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to check whether the color object is empty.
	// Returns:
	//     A BOOL value of TRUE if the object is empty and FALSE if the object
	//     is not empty.
	// Remarks:
	//-----------------------------------------------------------------------
	BOOL IsEmpty() const
	{
		return Argb == Empty;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a "darker" color for the current color.
	// Returns:
	//     A "darker" color value.
	//-----------------------------------------------------------------------
	CXTPChartColor GetDarkColor() const;

public:

	// Common color constants

	const static ARGB AliceBlue;
	const static ARGB AntiqueWhite;
	const static ARGB Aqua;
	const static ARGB Aquamarine;
	const static ARGB Azure;
	const static ARGB Beige;
	const static ARGB Bisque;
	const static ARGB Black;
	const static ARGB BlanchedAlmond;
	const static ARGB Blue;
	const static ARGB BlueViolet;
	const static ARGB Brown;
	const static ARGB BurlyWood;
	const static ARGB CadetBlue;
	const static ARGB Chartreuse;
	const static ARGB Chocolate;
	const static ARGB Coral;
	const static ARGB CornflowerBlue;
	const static ARGB Cornsilk;
	const static ARGB Crimson;
	const static ARGB Cyan;
	const static ARGB DarkBlue;
	const static ARGB DarkCyan;
	const static ARGB DarkGoldenrod;
	const static ARGB DarkGray;
	const static ARGB DarkGreen;
	const static ARGB DarkKhaki;
	const static ARGB DarkMagenta;
	const static ARGB DarkOliveGreen;
	const static ARGB DarkOrange;
	const static ARGB DarkOrchid;
	const static ARGB DarkRed;
	const static ARGB DarkSalmon;
	const static ARGB DarkSeaGreen;
	const static ARGB DarkSlateBlue;
	const static ARGB DarkSlateGray;
	const static ARGB DarkTurquoise;
	const static ARGB DarkViolet;
	const static ARGB DeepPink;
	const static ARGB DeepSkyBlue;
	const static ARGB DimGray;
	const static ARGB DodgerBlue;
	const static ARGB Firebrick;
	const static ARGB FloralWhite;
	const static ARGB ForestGreen;
	const static ARGB Fuchsia;
	const static ARGB Gainsboro;
	const static ARGB GhostWhite;
	const static ARGB Gold;
	const static ARGB Goldenrod;
	const static ARGB Gray;
	const static ARGB Green;
	const static ARGB GreenYellow;
	const static ARGB Honeydew;
	const static ARGB HotPink;
	const static ARGB IndianRed;
	const static ARGB Indigo;
	const static ARGB Ivory;
	const static ARGB Khaki;
	const static ARGB Lavender;
	const static ARGB LavenderBlush;
	const static ARGB LawnGreen;
	const static ARGB LemonChiffon;
	const static ARGB LightBlue;
	const static ARGB LightCoral;
	const static ARGB LightCyan;
	const static ARGB LightGoldenrodYellow;
	const static ARGB LightGray;
	const static ARGB LightGreen;
	const static ARGB LightPink;
	const static ARGB LightSalmon;
	const static ARGB LightSeaGreen;
	const static ARGB LightSkyBlue;
	const static ARGB LightSlateGray;
	const static ARGB LightSteelBlue;
	const static ARGB LightYellow;
	const static ARGB Lime;
	const static ARGB LimeGreen;
	const static ARGB Linen;
	const static ARGB Magenta;
	const static ARGB Maroon;
	const static ARGB MediumAquamarine;
	const static ARGB MediumBlue;
	const static ARGB MediumOrchid;
	const static ARGB MediumPurple;
	const static ARGB MediumSeaGreen;
	const static ARGB MediumSlateBlue;
	const static ARGB MediumSpringGreen;
	const static ARGB MediumTurquoise;
	const static ARGB MediumVioletRed;
	const static ARGB MidnightBlue;
	const static ARGB MintCream;
	const static ARGB MistyRose;
	const static ARGB Moccas;
	const static ARGB NavajoWhite;
	const static ARGB Navy;
	const static ARGB OldLace;
	const static ARGB Olive;
	const static ARGB OliveDrab;
	const static ARGB Orange;
	const static ARGB OrangeRed;
	const static ARGB Orchid;
	const static ARGB PaleGoldenrod;
	const static ARGB PaleGreen;
	const static ARGB PaleTurquoise;
	const static ARGB PaleVioletRed;
	const static ARGB PapayaWhip;
	const static ARGB PeachPuff;
	const static ARGB Peru;
	const static ARGB Pink;
	const static ARGB Plum;
	const static ARGB PowderBlue;
	const static ARGB Purple;
	const static ARGB Red;
	const static ARGB RosyBrown;
	const static ARGB RoyalBlue;
	const static ARGB SaddleBrown;
	const static ARGB Salmon;
	const static ARGB SandyBrown;
	const static ARGB SeaGreen;
	const static ARGB SeaShell;
	const static ARGB Sienna;
	const static ARGB Silver;
	const static ARGB SkyBlue;
	const static ARGB SlateBlue;
	const static ARGB SlateGray;
	const static ARGB Snow;
	const static ARGB SpringGreen;
	const static ARGB SteelBlue;
	const static ARGB Tan;
	const static ARGB Teal;
	const static ARGB Thistle;
	const static ARGB Tomato;
	const static ARGB Transparent;
	const static ARGB Turquoise;
	const static ARGB Violet;
	const static ARGB Wheat;
	const static ARGB White;
	const static ARGB WhiteSmoke;
	const static ARGB Yellow;
	const static ARGB YellowGreen;
	const static ARGB Empty;

	//===========================================================================
	// Summary:
	//     Enumeration for shift count of A, R, G, B components
	// Remarks:
	//===========================================================================
	enum
	{
		AlphaShift  = 24,
		RedShift    = 16,
		GreenShift  = 8,
		BlueShift   = 0
	};

	//===========================================================================
	// Summary:
	//     Enumeration for bit mask of A, R, G, B components
	// Remarks:
	//===========================================================================
	enum
	{
		AlphaMask   = 0xff000000,
		RedMask     = 0x00ff0000,
		GreenMask   = 0x0000ff00,
		BlueMask    = 0x000000ff
	};

	// Assemble A, R, G, B values into a 32-bit integer
	//===========================================================================
	// Summary:
	//     Call this static function to assemble A, R, G, B values into a 32-bit
	//     integer ARGB value.
	// Parameters:
	//     a - The Alpha value.
	//     r - The Red value
	//     g - The Green value
	//     b - The Blue value
	// Returns:
	//     A 32 bit ARGB value.
	// Remarks:
	//===========================================================================
	static ARGB AFX_CDECL MakeARGB(BYTE a, BYTE r, BYTE g, BYTE b)
	{
		return (((ARGB) (b) <<  BlueShift) |
				((ARGB) (g) << GreenShift) |
				((ARGB) (r) <<   RedShift) |
				((ARGB) (a) << AlphaShift));
	}

protected:

	ARGB Argb;  //The 32 bit color value.

#ifdef _XTP_ACTIVEX
//{{AFX_CODEJOCK_PRIVATE
public:
	OLE_COLOR ToOleColor();
	static CXTPChartColor AFX_CDECL FromOleColor(OLE_COLOR);
//}}AFX_CODEJOCK_PRIVATE
#endif
};

//===========================================================================
// Summary:
//     This class abstracts a font entity.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartFont : public CXTPCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor, creates a CXTPChartFont object and initializes
	//     the values.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     Overloaded constructor, creates a font object from the LOGFONT structure.
	// Parameters:
	//     pLogFont - A pointer to the LOGFONT structure.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartFont(LOGFONT* pLogFont);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destructor, does the cleaning.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual ~CXTPChartFont();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to set the LOGFONT for this object.
	// Parameters:
	//     lf - A pointer to the LOGFONT structure.
	// Remarks:
	//-----------------------------------------------------------------------
	void SetLogFont(LOGFONT* lf);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this static function to get Tahoma font with size 18.
	// Returns:
	//     A pointer to the CXTPChartFont object.
	// Remarks:
	//     This function creates a new CXTPChartFont and set the underlying font to
	//     Tahoma with size 18.
	//-----------------------------------------------------------------------
	static CXTPChartFont* AFX_CDECL GetTahoma18();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this static function to get Tahoma font with size 12.
	// Returns:
	//     A pointer to the CXTPChartFont object.
	// Remarks:
	//     This function creates a new CXTPChartFont and set the underlying font to
	//     Tahoma with size 12.
	//-----------------------------------------------------------------------
	static CXTPChartFont* AFX_CDECL GetTahoma12();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this static function to get Tahoma font with size 8.
	// Returns:
	//     A pointer to the CXTPChartFont object.
	// Remarks:
	//     This function creates a new CXTPChartFont and set the underlying font to
	//     Tahoma with size 8.
	//-----------------------------------------------------------------------
	static CXTPChartFont* AFX_CDECL GetTahoma8();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to do the clean up.
	//-----------------------------------------------------------------------
	void Release();


#ifdef _XTP_ACTIVEX
public:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPChartFont);
	afx_msg void OleSetFont(LPFONTDISP pFontDisp);
	afx_msg LPFONTDISP OleGetFont();
//}}AFX_CODEJOCK_PRIVATE
#endif

public:
	LOGFONT m_lf;   //The LOGFONT variable.
};

//-----------------------------------------------------------------------
// Summary:
//     Represents a 3D rotation primitive for chart objects using
//     Euler angles Yaw, Pitch and Roll in degrees.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChart3dRotation 
#ifdef _XTP_ACTIVEX
	: public CXTPCmdTarget
#endif
{

#ifdef _XTP_ACTIVEX
	DECLARE_SERIAL(CXTPChart3dRotation)
#endif

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs rotation object with no rotation by default.
	// Parameters:
	//     rhs - Right-hand-side rotation object to copy data from.
	//-----------------------------------------------------------------------
	CXTPChart3dRotation();
	CXTPChart3dRotation(const CXTPChart3dRotation& rhs); // <combine CXTPChart3dRotation::CXTPChart3dRotation>

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs rotation object with provided rotation parameters.
	// Parameters:
	//     dYaw - Yaw angle value in degrees.
	//     dPitch - Pitch angle value in degrees.
	//     dRoll - Roll angle value in degrees.
	//-----------------------------------------------------------------------
	CXTPChart3dRotation(double dYaw, double dPitch, double dRoll);

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if any roration is applied.
	// Returns:
	//     BOOL if any rotation is applied.
	//-----------------------------------------------------------------------
	BOOL IsRotated() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads roration angle value from a string provided
	//     The values are expected to be comma-separated floating point
	//     values in the following order: "yaw, pitch, roll"
	//-----------------------------------------------------------------------
	void SetFromString(LPCTSTR lpsz);

	//-----------------------------------------------------------------------
	// Summary:
	//     Overloads default assignment operator. Assigns rotation components only.
	//-----------------------------------------------------------------------
	CXTPChart3dRotation& operator = (const CXTPChart3dRotation& rhs);

	double m_dYaw; // Yaw angle value in degrees
	double m_dPitch; // Pitch angle value in degrees
	double m_dRoll; // Roll angle value in degrees

#ifdef _XTP_ACTIVEX
public:
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPChart3dRotation);
	DECLARE_OLECREATE_EX(CXTPChart3dRotation);
	//}}AFX_CODEJOCK_PRIVATE
#endif
};

AFX_INLINE BOOL CXTPChart3dRotation::IsRotated() const {
	return 0 != m_dYaw || 0 != m_dPitch || 0 != m_dRoll;
}

AFX_INLINE CXTPChart3dRotation& CXTPChart3dRotation::operator = (const CXTPChart3dRotation& rhs) {
	m_dYaw = rhs.m_dYaw;
	m_dPitch = rhs.m_dPitch;
	m_dRoll = rhs.m_dRoll;
	return *this;
}

//-----------------------------------------------------------------------
// Summary:
//     Represents a 3D box build around included points and
//     provides useful and fast operations.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChart3dBox
{
public:
	// Point list collection type
	typedef CList<CXTPPoint3d, const CXTPPoint3d&> PointList;

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs an empty 3D box.
	//-----------------------------------------------------------------------
	CXTPChart3dBox();

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a copy of a 3D box provided.
	// Parameters:
	//     rhs - A box to be constructed from.
	//-----------------------------------------------------------------------
	CXTPChart3dBox(const CXTPChart3dBox& rhs);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Performs box assignment in an efficient way.
	// Parameters:
	//     rhs - A box to be assigned to.
	// Returns:
	//     Self object reference.
	//-----------------------------------------------------------------------
	CXTPChart3dBox& operator = (const CXTPChart3dBox& rhs);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if a box is valid, e.i. minimal points are less or equal
	//     to maximal points.
	// Returns:
	//     TRUE if 
	//-----------------------------------------------------------------------
	BOOL IsValid() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Includes a point into the box. If point exceeds any box side
	//     then the box size gets increased.
	// Parameters:
	//     pt - A point to include into the box.
	//     bCenterCandidate - If TRUE, the point is concidered a center
	//         candidate when GetCenter is called for center candidates.
	// See also:
	//     GetCenter
	//-----------------------------------------------------------------------
	void Include(const CXTPPoint3d& pt, BOOL bCenterCandidate = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Includes a point into the box. If point exceeds any box side
	//     then the box size gets increased.
	// Parameters:
	//     x - X coordinate of a point to include into the box.
	//     y - Y coordinate of a point to include into the box.
	//     z - Z coordinate of a point to include into the box.
	//     bCenterCandidate - If TRUE, the point is concidered a center
	//         candidate when GetCenter is called for center candidates.
	// See also:
	//     GetCenter
	//-----------------------------------------------------------------------
	void Include(double x, double y, double z, BOOL bCenterCandidate = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Includes a box into the box. The inclusion assumes stretching
	//     to extreme points and copying only center candidates.
	// Parameters:
	//     box - A box to include.
	// See also:
	//     GetCenter
	//-----------------------------------------------------------------------
	void Include(const CXTPChart3dBox& box);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a minimal point in the box, i.e. a point with the minimal
	//     coordinate value along 3D axes.
	// Returns:
	//     A point with minimal coordinates.
	//-----------------------------------------------------------------------
	const CXTPPoint3d& GetMinPoint() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a maximal point in the box, i.e. a point with the maximal
	//     coordinate value along 3D axes.
	// Returns:
	//     A point with maximal coordinates.
	//-----------------------------------------------------------------------
	const CXTPPoint3d& GetMaxPoint() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Computes a center of the box. If bAmongstCenterCandidates is FALSE
	//     than the center of the box is computed as a medium point between
	//     the minimal and maximal points. Otherwise the closest to the physical
	//     box center point added with CentedCandidate = TRUE if returned.
	// Parameters:
	//     bAmongstCenterCandidates - If TRUE, only center candidate points
	//        are considered and the closest one to the physical center is returned.
	// Returns:
	//     A center box point.
	// See also:
	//     Include
	//-----------------------------------------------------------------------
	CXTPPoint3d GetCenter(BOOL bAmongstCenterCandidates = FALSE) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains box corners coordinates.
	// Parameters:
	//     pt - an array of 3D box corners. The first 4 points represent
	//          far box face along Z axis, the next 4 points represent 
	//          the close box face long Z axis.
	//-----------------------------------------------------------------------
	void GetCorners(CXTPPoint3d (&pt)[8]) const;


	//-----------------------------------------------------------------------
	// Summary:
	//      Provides access to the list of center candidate points.
	// Returns:
	//      A reference to the list of center candidate points.
	// See also:
	//      GetCenter
	//-----------------------------------------------------------------------
	const PointList& GetCenterCandidates() const;

private:
	CXTPPoint3d m_ptMin, m_ptMax;
	PointList m_CenterCandidates;
};

AFX_INLINE void CXTPChart3dBox::Include(double x, double y, double z, BOOL bCenterCandidate /*= FALSE*/) {
	Include(CXTPPoint3d(x, y, z) , bCenterCandidate);
}
AFX_INLINE const CXTPPoint3d& CXTPChart3dBox::GetMinPoint() const {
	return m_ptMin;
}
AFX_INLINE const CXTPPoint3d& CXTPChart3dBox::GetMaxPoint() const {
	return m_ptMax;
}
AFX_INLINE const CXTPChart3dBox::PointList& CXTPChart3dBox::GetCenterCandidates() const {
	return m_CenterCandidates;
}

_XTP_EXT_CLASS BOOL AFX_CDECL PX_Color(CXTPPropExchange* pPX, LPCTSTR pszPropName, 
	CXTPChartColor& clrValue, CXTPChartColor clrDefault = CXTPChartColor::Empty);
_XTP_EXT_CLASS BOOL AFX_CDECL PX_Font(CXTPPropExchange* pPX, LPCTSTR pszPropName, CXTPChartFont* pFont);
_XTP_EXT_CLASS BOOL AFX_CDECL PX_Rotation(CXTPPropExchange* pPX, LPCTSTR pszPropName, CXTPChart3dRotation& rotation);


#ifdef _XTP_ACTIVEX
extern double AFX_CDECL VariantToDoubleEx(const VARIANT* pVariant);
extern BOOL AFX_CDECL IsStringVariant(const VARIANT* pVariant);
extern double AFX_CDECL VariantToDouble(const VARIANT* pVariant);
#endif


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTTYPES_H__)
