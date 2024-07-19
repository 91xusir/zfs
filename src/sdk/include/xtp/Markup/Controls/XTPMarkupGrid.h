// XTPMarkupGrid.h: interface for the CXTPMarkupGrid class.
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
#if !defined(__XTPMARKUPGRID_H__)
#define __XTPMARKUPGRID_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"


class CXTPMarkupBuilder;
class CXTPMarkupUIElement;
class CXTPMarkupDefinitionBase;
class CXTPMarkupColumnDefinitionCollection;
class CXTPMarkupRowDefinitionCollection;


//===========================================================================
// Summary: CXTPMarkupDefinitionCollection is base class for CXTPMarkupDefinitionCollection and CXTPMarkupDefinitionCollection
//          It implements RowDefinition XAML Tag
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupDefinitionCollection : public CXTPMarkupCollection
{
	DECLARE_MARKUPCLASS(CXTPMarkupDefinitionCollection);
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupDefinitionCollection object
	//-----------------------------------------------------------------------
	CXTPMarkupDefinitionCollection();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Returns CXTPMarkupDefinitionBase by its index
	// Parameters:
	//     nIndex - Index of CXTPMarkupDefinitionBase to retrieve
	// Returns: CXTPMarkupDefinitionBase by its index
	//-----------------------------------------------------------------------
	CXTPMarkupDefinitionBase* GetItem(int nIndex) const;

//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupDefinitionCollection);
#endif
	DECLARE_ENUM_VARIANT(CXTPMarkupDefinitionCollection)

	long OleGetItemCount();
	LPDISPATCH OleGetItem(long nIndex);
	void OleAdd(LPDISPATCH lpElementDisp);
	void OleInsert(long Index, LPDISPATCH lpElementDisp);

//}}AFX_CODEJOCK_PRIVATE
};



//===========================================================================
// Summary: CXTPMarkupGrid is CXTPMarkupPanel derived class. It implements Grid XAML Tag.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupGrid : public CXTPMarkupPanel
{
private:
	enum LayoutTimeSizeType
	{
		sizeTypeNone = 0,
		sizeTypePixel = 1,
		sizeTypeAuto = 2,
		sizeTypeStar = 4
	};

private:
	struct CELLCACHE
	{
		int nColumnIndex;
		int nRowIndex;
		int nColumnSpan;
		int nRowSpan;
		int nSizeTypeU;
		int nSizeTypeV;
		int nNext;
	public:
		BOOL IsStarU() const;
		BOOL IsAutoU() const;
		BOOL IsStarV() const;
		BOOL IsAutoV() const;
	};

	struct SPANKEY
	{
		int nCount;
		int nStart;
		BOOL bU;
		int nValue;
	};

private:
	DECLARE_MARKUPCLASS(CXTPMarkupGrid)

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupGrid object
	//-----------------------------------------------------------------------
	CXTPMarkupGrid();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupGrid object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupGrid();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns collection of constrants and definitions for Grid columns
	// Returns:
	//     Pointer to collection of CXTPMarkupColumnDefinition classes
	//-----------------------------------------------------------------------
	CXTPMarkupColumnDefinitionCollection* GetColumnDefinitions() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns collection of constrants and definitions for Grid rows
	// Returns:
	//     Pointer to collection of CXTPMarkupRowDefinition classes
	//-----------------------------------------------------------------------
	CXTPMarkupRowDefinitionCollection* GetRowDefinitions() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set column index for grid child element
	// Parameters:
	//     pElement - Child element of the grid
	//     nColumn - Column index to assign for pElement
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetColumn(CXTPMarkupUIElement* pElement, int nColumn);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves column index of grid child element
	// Parameters:
	//     pElement - Child element of the grid to check
	// Returns:
	//     Index of the column for pElement element
	//-----------------------------------------------------------------------
	static int AFX_CDECL GetColumn(CXTPMarkupUIElement* pElement);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set row index for grid child element
	// Parameters:
	//     pElement - Child element of the grid
	//     nRow - Row index to assign for pElement
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetRow(CXTPMarkupUIElement* pElement, int nRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves row index of grid child element
	// Parameters:
	//     pElement - Child element of the grid to check
	// Returns:
	//     Index of the row for pElement element
	//-----------------------------------------------------------------------
	static int AFX_CDECL GetRow(CXTPMarkupUIElement* pElement);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set total number of columns that child content spans within a Grid.
	// Parameters:
	//     pElement - Child element of the grid
	//     nColumnSpan - Number of columns that child content spans
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetColumnSpan(CXTPMarkupUIElement* pElement, int nColumnSpan);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves total number of columns that child content spans within a Grid.
	// Parameters:
	//     pElement - Child element of the grid to check
	// Returns:
	//     Number of columns that child content spans
	//-----------------------------------------------------------------------
	static int AFX_CDECL GetColumnSpan(CXTPMarkupUIElement* pElement);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set total number of rows that child content spans within a Grid.
	// Parameters:
	//     pElement - Child element of the grid
	//     nRowSpan - Number of rows that child content spans
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetRowSpan(CXTPMarkupUIElement* pElement, int nRowSpan);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves total number of rows that child content spans within a Grid.
	// Parameters:
	//     pElement - Child element of the grid to check
	// Returns:
	//     Number of rows that child content spans
	//-----------------------------------------------------------------------
	static int AFX_CDECL GetRowSpan(CXTPMarkupUIElement* pElement);

//{{AFX_CODEJOCK_PRIVATE
	// Implementation

protected:
	CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize availableSize);
	CSize ArrangeOverride(CSize arrangeSize);
	void SetPropertyObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupDependencyProperty* pProperty, CXTPMarkupObject* pValue);

private:
	void ValidateDefinitionsLayout(CXTPMarkupDefinitionCollection* pDefinitions, BOOL bTreatStarAsAuto);
	void ValidateCells();
	void ValidateDefinitions();
	int GetLengthTypeForRange(CXTPMarkupDefinitionCollection* pDefinitions, int start, int count);
	void MeasureCellsGroup(CXTPMarkupDrawingContext* pDC, int cellsHead, CSize referenceSize, BOOL ignoreDesiredSizeU, BOOL forceInfinityV);
	void MeasureCell(CXTPMarkupDrawingContext* pDC, int cell, BOOL forceInfinityV);
	int GetMeasureSizeForRange(CXTPMarkupDefinitionCollection* pDefinitions, int start, int count);
	int CalculateDesiredSize(CXTPMarkupDefinitionCollection* pDefinitions);
	void ResolveStar(CXTPMarkupDefinitionCollection* pDefinitions, double availableSize);
	int GetFinalSizeForRange(CXTPMarkupDefinitionCollection* pDefinitions, int start, int count);
	void SetFinalSize(CXTPMarkupDefinitionCollection* pDefinitions, int finalSize);
	void EnsureMinSizeInDefinitionRange(CXTPMarkupDefinitionCollection* definitions, int start, int count, int requestedSize);

	static int AFX_CDECL DistributionOrderComparer(const void *arg1, const void *arg2);
	static int AFX_CDECL StarDistributionOrderComparer(const void *arg1, const void *arg2);
	static int AFX_CDECL SpanMaxDistributionOrderComparer(const void *arg1, const void *arg2);
	static int AFX_CDECL SpanPreferredDistributionOrderComparer(const void *arg1, const void *arg2);

	static void AFX_CDECL OnCellAttachedPropertyChanged(CXTPMarkupObject* d, CXTPMarkupPropertyChangedEventArgs* e);

protected:
	CXTPMarkupRowDefinitionCollection* m_pRowDefinitions;
	CXTPMarkupColumnDefinitionCollection* m_pColumnDefinitions;

public:
	static CXTPMarkupDependencyProperty* m_pColumnDefinitionsProperty;
	static CXTPMarkupDependencyProperty* m_pRowDefinitionsProperty;
	static CXTPMarkupDependencyProperty* m_pRowProperty;
	static CXTPMarkupDependencyProperty* m_pColumnProperty;

	static CXTPMarkupDependencyProperty* m_pColumnSpanProperty;
	static CXTPMarkupDependencyProperty* m_pRowSpanProperty;

private:
	CELLCACHE* m_pCellCachesCollection;
	int m_nCellGroup[4];
	BOOL m_bHasStarCellsU;
	BOOL m_bHasStarCellsV;
	BOOL m_bHasGroup2CellsInAutoRows;

	CXTPMarkupDefinitionCollection* m_pDefinitionsU;
	CXTPMarkupDefinitionCollection* m_pDefinitionsV;

	friend struct CELLCACHE;
	friend class CXTPMarkupDefinitionBase;

//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupGrid);
#endif

	afx_msg int OleGetRow(LPDISPATCH lpElementDisp);
	afx_msg void OleSetRow(LPDISPATCH lpElementDisp, int nValue);
	afx_msg int OleGetRowSpan(LPDISPATCH lpElementDisp);
	afx_msg void OleSetRowSpan(LPDISPATCH lpElementDisp, int nValue);
	afx_msg int OleGetColumn(LPDISPATCH lpElementDisp);
	afx_msg void OleSetColumn(LPDISPATCH lpElementDisp, int nValue);
	afx_msg int OleGetColumnSpan(LPDISPATCH lpElementDisp);
	afx_msg void OleSetColumnSpan(LPDISPATCH lpElementDisp, int nValue);
	afx_msg LPDISPATCH OleGetColumnDefinitions();
	afx_msg LPDISPATCH OleGetRowDefinitions();
//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE CXTPMarkupColumnDefinitionCollection* CXTPMarkupGrid::GetColumnDefinitions() const {
	return m_pColumnDefinitions;
}
AFX_INLINE CXTPMarkupRowDefinitionCollection* CXTPMarkupGrid::GetRowDefinitions() const {
	return m_pRowDefinitions;
}
AFX_INLINE CXTPMarkupDefinitionBase* CXTPMarkupDefinitionCollection::GetItem(int nIndex) const {
	return nIndex >= 0 && nIndex < m_arrItems.GetSize() ? (CXTPMarkupDefinitionBase*)m_arrItems[nIndex] : NULL;
}





#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPGRID_H__)
