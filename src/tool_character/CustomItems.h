#ifndef __CUSTOM_ITEMS_H__
#define __CUSTOM_ITEMS_H__

#include "stdafx.h"
#include <cstringt.h>
#include <string>
using namespace std;
class CCustomItemString : public CXTPPropertyGridItem
{
public:
	CCustomItemString(CString strCaption, string *str);
	CCustomItemString(UINT nID, string *str);

	~CCustomItemString(void);

	void SetValue(CString strValue);

protected:
	void SetString(string *str);
	string *m_string;
};

class CCustomItemFloat : public CXTPPropertyGridItem
{
public:
	CCustomItemFloat(CString strCaption, float *f);
	~CCustomItemFloat();

	void SetValue(CString strValue);

protected:
	void SetFloat(float *f);
	float *m_float;
};

class CCustomItemV3 : public CXTPPropertyGridItem
{
public:
	CCustomItemV3(CString strCaption, RtgVertex3 *v3);
	~CCustomItemV3(void);

	void SetValue(CString strValue);

protected:
	void SetValue(RtgVertex3 *v4);
	RtgVertex3 *m_v3;
};

class CCustomItemV4 : public CXTPPropertyGridItem
{
public:
	CCustomItemV4(CString strCaption, RtgVertex4 *v4);
	~CCustomItemV4(void);

	void SetValue(CString strValue);

protected:
	void SetValue(RtgVertex4 *v4);
	RtgVertex4 *m_v4;
};

class CCustomItemBool : public CXTPPropertyGridItem
{
public:
	CCustomItemBool(CString strCaption, bool *b);
	// CCustonItemBool(UINT nID, BOOL bValue = FALSE, BOOL* pBindBool = NULL);

	virtual ~CCustomItemBool();
	void SetBool(bool bValue);
	// bool SetTrueFalseText(CString strTrueText, CString strFalseText);

protected:
	void SetValue(CString strValue);
	// bool SetValueText(CString& strValueText, CString strNewText);

private:
	void _Init();

protected:
	bool    *m_bool;
	CString  m_strTrueText;  // Text of the TRUE label.
	CString  m_strFalseText; // Text of the FALSE label.
};

class CCustomItemColor3 : public CXTPPropertyGridItem
{
public:
	CCustomItemColor3(CString strCaption, RtgVertex3 *v3);
	virtual ~CCustomItemColor3(void);

public:
	void SetColor(COLORREF clr);
	COLORREF GetColor() { return m_clrValue; }
	static COLORREF StringToRGB(CString str);
	static CString RGBToString(COLORREF clr);
	// void BindToColor(COLORREF* pBindColor);

	virtual BOOL OnDrawItemValue(CDC& dc, CRect rcValue);
	virtual CRect GetValueRect();
	virtual void SetValue(CString strValue);
	virtual void OnInplaceButtonDown();

	COLORREF m_clrValue;		// Value of the item.
	COLORREF *m_pBindColor;		// Binded value.
	RtgVertex3 *m_v3;
};


class CCustomItemColor4 : public CXTPPropertyGridItem
{
public:
	CCustomItemColor4(CString strCaption, RtgVertex4 *v4);
	virtual ~CCustomItemColor4(void);

public:
	void SetColor(COLORREF clr);
	COLORREF GetColor() { return m_clrValue; }
	static COLORREF StringToRGB(CString str);
	static CString RGBToString(COLORREF clr);
	// void BindToColor(COLORREF* pBindColor);

	virtual BOOL OnDrawItemValue(CDC& dc, CRect rcValue);
	virtual CRect GetValueRect();
	virtual void SetValue(CString strValue);
	virtual void OnInplaceButtonDown();

	COLORREF m_clrValue;		// Value of the item.
	COLORREF *m_pBindColor;		// Binded value.
	RtgVertex4 *m_v4;
};
#endif
