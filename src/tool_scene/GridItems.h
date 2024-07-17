
#pragma once

class CGridItemRtString : public CXTPPropertyGridItem
{
public:
    CGridItemRtString(CString strCaption, RtString* pStr):CXTPPropertyGridItem(strCaption)
    {
        m_pString = pStr; SetString(pStr);
    }
    CGridItemRtString(UINT nID, RtString* pStr):CXTPPropertyGridItem(nID)
    {
        m_pString = pStr; SetString(pStr);
    }
    ~CGridItemRtString(void){}
    void SetString(RtString *pStr)
    {
        *m_pString = *pStr;
        CString strValue = *pStr;
        CXTPPropertyGridItem::SetValue(strValue);
    }
protected:
    void SetValue(CString strValue)
    {
        *m_pString = (const char*)strValue;
        CXTPPropertyGridItem::SetValue(strValue);
    }
    RtString *m_pString;
};

//////////////////////////////////////////////////////////////////////////

class CGridItemSizeString : public CXTPPropertyGridItem
{
public:
    CGridItemSizeString(CString strCaption, char* pStr, int iSize):CXTPPropertyGridItem(strCaption)
    {
        m_pString = pStr; m_iSize = iSize; SetString(pStr);
    }
    CGridItemSizeString(UINT nID, char* pStr, int iSize):CXTPPropertyGridItem(nID)
    {
        m_pString = pStr; m_iSize = iSize; SetString(pStr);
    }
    ~CGridItemSizeString(void){}
    void SetString(char* pStr)
    {
        strncpy(m_pString, pStr, m_iSize);
        m_pString[m_iSize-1] = 0;
        CXTPPropertyGridItem::SetValue(m_pString);
    }
protected:
    void SetValue(CString strValue)
    {
        strncpy(m_pString, strValue, m_iSize);
        m_pString[m_iSize-1] = 0;
        CXTPPropertyGridItem::SetValue(strValue);
    }
    char *m_pString;
    int  m_iSize;
};
//////////////////////////////////////////////////////////////////////////
class CGridItemInt : public CXTPPropertyGridItem
{
public:
	CGridItemInt(CString strCaption, int *i) : CXTPPropertyGridItem(strCaption)
	{
		m_pInt = i; SetInt(i);
	}
	CGridItemInt(UINT nID, int *i) : CXTPPropertyGridItem(nID)
	{
		m_pInt = i; SetInt(i);
	}
	~CGridItemInt() {}
	void SetInt(int *i)
	{
		*m_pInt = *i;
		CString strValue;
		strValue.Format(_T("%d"), *i);
		CXTPPropertyGridItem::SetValue(strValue);
	}
protected:
	void SetValue(CString strValue)
	{
		*m_pInt = atoi((const char*)strValue);
		CXTPPropertyGridItem::SetValue(strValue);
	}
	int *m_pInt;
};
//////////////////////////////////////////////////////////////////////////

class CGridItemFloat : public CXTPPropertyGridItem
{
public:
    CGridItemFloat(CString strCaption, float *f) : CXTPPropertyGridItem(strCaption)
    {
        m_pFloat = f; SetFloat(f);
    }
    CGridItemFloat(UINT nID, float *f) : CXTPPropertyGridItem(nID)
    {
        m_pFloat = f; SetFloat(f);
    }
    ~CGridItemFloat() {}
    void SetFloat(float *f)
    {
        *m_pFloat = *f;
        CString strValue;
        strValue.Format(_T("%.3f"), *f);
        CXTPPropertyGridItem::SetValue(strValue);
    }
protected:
    void SetValue(CString strValue)
    {
        *m_pFloat = atof((const char*)strValue);
        CXTPPropertyGridItem::SetValue(strValue);
    }
    float *m_pFloat;
};

//////////////////////////////////////////////////////////////////////////

class CGridItemColor : public CXTPPropertyGridItemColor
{
public:
    CGridItemColor(CString strCaption, DWORD *pColor) : CXTPPropertyGridItemColor(strCaption)
    {
        m_pColor = pColor; SetMyColor(pColor);
    }
    CGridItemColor(UINT nID, DWORD *pColor) : CXTPPropertyGridItemColor(nID)
    {
        m_pColor = pColor; SetMyColor(pColor);
    }
    ~CGridItemColor() {}
    void SetMyColor(DWORD *pColor)
    {
        *m_pColor = *pColor;
        DWORD dwWin32Color = *pColor;
        ConvertColor(dwWin32Color);
        CXTPPropertyGridItemColor::SetValue(RGBToString(dwWin32Color));
    }
protected:
    virtual void SetValue(CString strValue)
    {
        *m_pColor = StringToRGB(strValue);
        ConvertColor(*m_pColor);
        CXTPPropertyGridItemColor::SetValue(strValue);
    }
    void ConvertColor(DWORD& dwColor)
    {
        dwColor = (dwColor&0xFF00FF00) | ((dwColor&0x00FF0000)>>16) | ((dwColor&0x000000FF)<<16);
    }
    DWORD *m_pColor;
};
