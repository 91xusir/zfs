#include "stdafx.h"
#include "CustomItems.h"
//
// string
//
CCustomItemString::CCustomItemString(CString strCaption, string *str)
	:CXTPPropertyGridItem(strCaption)
{
	m_string = str;
	SetString(str);
}

CCustomItemString::CCustomItemString(UINT nID, string *str)
	:CXTPPropertyGridItem(nID)
{
	m_string = str;
	SetString(str);
}

CCustomItemString::~CCustomItemString(void)
{

}

void CCustomItemString::SetValue(CString strValue)
{
	*m_string = (const char*)strValue;
	CXTPPropertyGridItem::SetValue(strValue);
}

void CCustomItemString::SetString(string *str)
{
	*m_string = *str;
	CString strValue;
	strValue.Format(_T("%s"), str->c_str());
	CXTPPropertyGridItem::SetValue(strValue);
}


//
// float
//
CCustomItemFloat::CCustomItemFloat(CString strCaption, float *f)
:CXTPPropertyGridItem(strCaption)
{
	m_float = f;
	SetFloat(f);
}

CCustomItemFloat::~CCustomItemFloat(void)
{

}

void CCustomItemFloat::SetValue(CString strValue)
{
	*m_float = atof((const char*)strValue);
	CXTPPropertyGridItem::SetValue(strValue);
}

void CCustomItemFloat::SetFloat(float *f)
{
	*m_float = *f;
	CString strValue;
	strValue.Format(_T("%.3f"), *f);
	CXTPPropertyGridItem::SetValue(strValue);
}

//
// vertex 3
//
CCustomItemV3::CCustomItemV3(CString strCaption, RtgVertex3 *v3)
:CXTPPropertyGridItem(strCaption)
{
	m_v3 = v3;
	SetValue(v3);
}

CCustomItemV3::~CCustomItemV3(void)
{

}

void CCustomItemV3::SetValue(CString strValue)
{
	RtgVertex3 v;
	if(sscanf(strValue,"%f,%f,%f",&v.x,&v.y,&v.z)
		!= 3)
	{
		MessageBox(NULL,"Get value failed","",MB_OK);
		return;
	}
	*m_v3 = v;
	CXTPPropertyGridItem::SetValue(strValue);
}

void CCustomItemV3::SetValue(RtgVertex3 *v3)
{
	*m_v3 = *v3;

	CString strValue;
	strValue.Format(_T("%.3f,%.3f,%.3f"),v3->x,v3->y,v3->z);
	CXTPPropertyGridItem::SetValue(strValue);
}

//
// vertex 4
//
CCustomItemV4::CCustomItemV4(CString strCaption, RtgVertex4 *v4)
	:CXTPPropertyGridItem(strCaption)
{
	m_v4 = v4;
	SetValue(v4);
}

CCustomItemV4::~CCustomItemV4(void)
{

}

void CCustomItemV4::SetValue(CString strValue)
{
	RtgVertex4 v;
	if(sscanf(strValue,"%f,%f,%f,%f",&v.x,&v.y,&v.z,&v.w)
		!= 4)
	{
		MessageBox(NULL,"Get value failed","",MB_OK);
		return;
	}
	*m_v4 = v;
	CXTPPropertyGridItem::SetValue(strValue);
}

void CCustomItemV4::SetValue(RtgVertex4 *v4)
{
	*m_v4 = *v4;
	
	CString strValue;
	strValue.Format(_T("%.3f,%.3f,%.3f,%.3f"),v4->x,v4->y,v4->z,v4->w);
	CXTPPropertyGridItem::SetValue(strValue);
}

//
// bool
//

CCustomItemBool::CCustomItemBool(CString strCaption, bool *b)
: CXTPPropertyGridItem(strCaption)
, m_strTrueText(_T("True"))
, m_strFalseText(_T("False"))
{
	m_bool = b;
	_Init();
}

CCustomItemBool::~CCustomItemBool(void)
{

}

void CCustomItemBool::_Init()
{
    SetBool(*m_bool);

	m_nFlags = xtpGridItemHasComboButton; // | xtpGridItemHasEdit;
	m_pConstraints->AddConstraint(m_strTrueText);
	m_pConstraints->AddConstraint(m_strFalseText);
	SetConstraintEdit(TRUE);
}

void CCustomItemBool::SetValue(CString strValue)
{
	SetBool(strValue.CompareNoCase(m_strTrueText) == 0);
}

void CCustomItemBool::SetBool(bool bValue)
{	
	*m_bool = bValue;
	CXTPPropertyGridItem::SetValue(bValue ? m_strTrueText: m_strFalseText);
}

/*
bool CCustomItemBool::SetValueText(CString& strValueText, CString strNewText)
{
	// see if the value exists.
	int iIndex = m_pContraints->FindConstaint(strValueText);
	if (iIndex != -1)
	{
		// get its position in the string list.
		POSITION pos = m_pContraints->FindIndex(iIndex);
		if (pos != NULL)
		{
			// if this is the current value change it as well.
			if (GetValue() == strValueText) {
				CXTPPropertyGridItem::SetValue(strNewText);
			}

			// update the value.
			strValueText  = strNewText;
			m_pContraints->SetAt(pos, strValueText);
			return true;
		}
	}
	return false;
}

bool CCustomItemBool::SetTrueFalseText(CString strTrueText, CString strFalseText)
{
	// update the "True" value text
	if (!SetValueText(m_strTrueText, strTrueText))
		return false;

	// update the "False" value text
	if (!SetValueText(m_strFalseText, strFalseText))
		return false;

	return true;
}
*/

//
// color 3 float (RGB)
//
CCustomItemColor3::CCustomItemColor3(CString strCaption, RtgVertex3 *v3)
	: CXTPPropertyGridItem(strCaption)
{
	// m_pBindColor = pBindColor;
	m_v3 = v3;
	SetColor(RGB(v3->x*255,v3->y*255,v3->z*255));
	m_nFlags = xtpGridItemHasEdit | xtpGridItemHasExpandButton;
}

CCustomItemColor3::~CCustomItemColor3(void)
{

}

BOOL CCustomItemColor3::OnDrawItemValue(CDC& dc, CRect rcValue)
{
	COLORREF clr = dc.GetTextColor();
	CRect rcSample(rcValue.left - 2, rcValue.top + 1, rcValue.left + 18, rcValue.bottom - 1);
	CXTPPenDC pen(dc, clr);
	CXTPBrushDC brush(dc, m_clrValue);
	dc.Rectangle(rcSample);
	CRect rcText(rcValue);
	rcText.left += 25;
	dc.DrawText( m_strValue, rcText,  DT_SINGLELINE|DT_VCENTER);
	return TRUE;
}

CRect CCustomItemColor3::GetValueRect()
{
	CRect rcValue(CXTPPropertyGridItem::GetValueRect());
	rcValue.left += 25;
	return rcValue;
}

COLORREF CCustomItemColor3::StringToRGB(CString str)
{	
	CString strRed, strGreen, strBlue;
	
	AfxExtractSubString(strRed, str, 0, ';');
	AfxExtractSubString(strGreen, str, 1, ';');
	AfxExtractSubString(strBlue, str, 2, ';');

	return RGB(__min(_ttoi(strRed), 255), __min(_ttoi(strGreen), 255), __min(_ttoi(strBlue), 255));
}

CString CCustomItemColor3::RGBToString(COLORREF clr)
{
	CString str;
	str.Format(_T("%i; %i; %i"), GetRValue(clr), GetGValue(clr), GetBValue(clr));
	return str;
}

void CCustomItemColor3::SetValue(CString strValue)
{	
	SetColor(StringToRGB(strValue));
}

void CCustomItemColor3::SetColor(COLORREF clr)
{
	m_clrValue = clr;
	if (m_v3)
	{
		m_v3->x = GetRValue(clr)/255.0f;
		m_v3->y = GetGValue(clr)/255.0f;
		m_v3->z = GetBValue(clr)/255.0f;
	}
	CXTPPropertyGridItem::SetValue(RGBToString(clr));
}

void CCustomItemColor3::OnInplaceButtonDown()
{
#ifdef _INCLUDE_CONTROLS 
	CColorDialog dlg( m_clrValue, m_clrValue,
		CPS_XTP_SHOW3DSELECTION|CPS_XTP_SHOWHEXVALUE, m_pGrid );
#else
	CColorDialog dlg( m_clrValue, 0, m_pGrid );
#endif
	if ( dlg.DoModal( ) == IDOK )
	{
		OnValueChanged( RGBToString( dlg.GetColor( ) ) );
		m_pGrid->Invalidate( FALSE );
	}
}


//
// color 4 float (RGBA)
//
CCustomItemColor4::CCustomItemColor4(CString strCaption, RtgVertex4 *v4)
	: CXTPPropertyGridItem(strCaption)
{
	// m_pBindColor = pBindColor;
	m_v4 = v4;
	SetColor(RGB(v4->x*255,v4->y*255,v4->z*255));
	m_nFlags = xtpGridItemHasEdit | xtpGridItemHasExpandButton;
}

CCustomItemColor4::~CCustomItemColor4(void)
{

}

BOOL CCustomItemColor4::OnDrawItemValue(CDC& dc, CRect rcValue)
{
	COLORREF clr = dc.GetTextColor();
	CRect rcSample(rcValue.left - 2, rcValue.top + 1, rcValue.left + 18, rcValue.bottom - 1);
	CXTPPenDC pen(dc, clr);
	CXTPBrushDC brush(dc, m_clrValue);
	dc.Rectangle(rcSample);
	CRect rcText(rcValue);
	rcText.left += 25;
	dc.DrawText( m_strValue, rcText,  DT_SINGLELINE|DT_VCENTER);
	return TRUE;
}

CRect CCustomItemColor4::GetValueRect()
{
	CRect rcValue(CXTPPropertyGridItem::GetValueRect());
	rcValue.left += 25;
	return rcValue;
}

COLORREF CCustomItemColor4::StringToRGB(CString str)
{	
	CString strRed, strGreen, strBlue;
	
	AfxExtractSubString(strRed, str, 0, ';');
	AfxExtractSubString(strGreen, str, 1, ';');
	AfxExtractSubString(strBlue, str, 2, ';');

	return RGB(__min(_ttoi(strRed), 255), __min(_ttoi(strGreen), 255), __min(_ttoi(strBlue), 255));
}

CString CCustomItemColor4::RGBToString(COLORREF clr)
{
	CString str;
	str.Format(_T("%i; %i; %i"), GetRValue(clr), GetGValue(clr), GetBValue(clr));
	return str;
}

void CCustomItemColor4::SetValue(CString strValue)
{	
	SetColor(StringToRGB(strValue));
}

void CCustomItemColor4::SetColor(COLORREF clr)
{
	m_clrValue = clr;
	if (m_v4)
	{
		m_v4->x = GetRValue(clr)/255.0f;
		m_v4->y = GetGValue(clr)/255.0f;
		m_v4->z = GetBValue(clr)/255.0f;
	}
	CXTPPropertyGridItem::SetValue(RGBToString(clr));
}

void CCustomItemColor4::OnInplaceButtonDown()
{
#ifdef _INCLUDE_CONTROLS 
	CColorDialog dlg( m_clrValue, m_clrValue,
		CPS_XTP_SHOW3DSELECTION|CPS_XTP_SHOWHEXVALUE, m_pGrid );
#else
	CColorDialog dlg( m_clrValue, 0, m_pGrid );
#endif
	if ( dlg.DoModal( ) == IDOK )
	{
		OnValueChanged( RGBToString( dlg.GetColor( ) ) );
		m_pGrid->Invalidate( FALSE );
	}
}
