#include "ui/rtw_ui.h"

IUiFontManager::IUiFontManager(){}

IUiFontManager::~IUiFontManager(){}

RtwFont* IUiFontManager::getDefaultFont()
{
	if (m_Fonts.empty())
	{
		std::string strFontName = "宋体";
		if (g_workspace.GetFontFileOk())
		{
			strFontName = g_workspace.GetDefaultFontName();
		}
		return CreateFont(strFontName.c_str(), 12);
	}
	return m_Fonts[0];
}

RtwFont * IUiFontManager::getChineseFont()
{
	return CreateFont("楷体", 12, RtwFont::FONT_HOLLOW);
}

RtwFont * IUiFontManager::getEnglishFont()
{
	return CreateFont("Consolas", 12);
}

RtwFont* IUiFontManager::CreateFont(const std::string& strFontName,const int iFontSize , int nFontStyle /*= RtwFont::FONT_NORMAL*/)
{
	int FontSize = iFontSize;
	if (FontSize == 0)
	{
		FontSize = 12;		
	}

	std::string FontName = strFontName;
	if (FontName == "")
	{
		FontName = "宋体";
	}

	RtwFont* pFont = FindFont( FontName , nFontStyle , FontSize );
	bool bNeedCreateFont = false;

	if (pFont ==NULL)
	{//不存在
		bNeedCreateFont = true;	
	}
	else if( pFont->getFontSize() != FontSize )
	{
		bNeedCreateFont = true;
	}

	if( bNeedCreateFont )
	{
		pFont = OnCreateFont(FontName,FontSize , nFontStyle);
		if(pFont!=NULL)
		{//创建成功
			m_Fonts.push_back(pFont);
		}
	}
	return pFont;
};

void IUiFontManager::Close()
{

	foreach(std::vector<RtwFont*>, iter, m_Fonts)
	{
		RtwFont* pFont = *iter;
		pFont->Close();
		DEL_ONE(pFont);
	}
	m_Fonts.clear();
}


void IUiFontManager::CloseFont(const std::string& FontName)
{
	RtwFont* pFont = FindFont(FontName);
	if (pFont)
	{
		pFont->Close();
	}
}
void IUiFontManager::DrawString3D(const std::string& str, const SRect3D& rect, 
								  const RtwPixel& textColor, bool bWordWrap/* = true*/, 
								  SUiTextAlignment align/* = SUiTextAlignment(alignNear, alignNear)*/, 
								  bool bUnderLine/* = false*/,
								  const RtwRect* pClipRect/* = NULL*/,
								  const std::string& FontName/* = ""*/)
{
	if (m_Fonts.empty())
		return;

	RtwFont* pFont = NULL;
	if (FontName.empty())
	{
		pFont = getDefaultFont();
	}
	else 
	{
		pFont = FindFont(FontName);
	}

	if (pFont)
	{
		pFont->DrawString3D(str, rect, textColor, bWordWrap, align, bUnderLine, pClipRect);
	}
}

void IUiFontManager::DrawString(const std::string& str, 
								const RtwRect& rect,
								const RtwPixel& textColor,
								RtwFont* pkFont,
								bool bWordWrap         /*= true*/, 
								SUiTextAlignment align /*= SUiTextAlignment(alignNear, alignNear)*/, 
								bool bUnderLine        /*= false*/,
								const RtwRect* pClipRect /*= NULL*/ )
{
	if (m_Fonts.empty())
		return;

	RtwFont* pFont = pkFont;
	if ( pFont == NULL )
	{
		pFont = getDefaultFont();
	}
	if (pFont)
	{
		pFont->DrawString(str, rect, textColor, bWordWrap, align, bUnderLine, pClipRect);
	}
}

/// <summary>
/// 绘制指定的文本字符串到指定的矩形区域，允许设置文本颜色、换行、对齐方式、下划线、剪裁区域、字体和字体大小。
/// </summary>
/// <param name="str">要绘制的文本字符串。</param>
/// <param name="rect">指定文本绘制的矩形区域。</param>
/// <param name="textColor">文本的颜色。</param>
/// <param name="bWordWrap">是否自动换行。如果为 true，文本在超出矩形区域时会自动换行；否则，不换行。</param>
/// <param name="align">文本的对齐方式，包括水平和垂直对齐。</param>
/// <param name="bUnderLine">是否对文本加下划线。如果为 true，则文本下会有下划线；否则，没有下划线。</param>
/// <param name="pClipRect">指定剪裁区域。绘制的文本将被限制在该矩形区域内；如果为 null，则不进行剪裁。</param>
/// <param name="FontName">要使用的字体名称。如果为空字符串，则使用默认字体。</param>
/// <param name="nFontSize">字体的大小。如果未指定，则默认为 12。</param>
void IUiFontManager::DrawString(const std::string& str,
								const RtwRect& rect, 
								const RtwPixel& textColor,
								bool bWordWrap/* = true*/, 
								SUiTextAlignment align/* = SUiTextAlignment(alignNear, alignNear)*/, 
								bool bUnderLine/* = false*/,
								const RtwRect* pClipRect/* = NULL*/,
								const std::string& FontName/* = ""*/,
								const int& nFontSize/* = 12*/)
{
	if (m_Fonts.empty())
		return;

	int iFontSize = nFontSize;
 	RtwFont* pFont = NULL;
	if (FontName.empty() || FontName=="")
	{
		pFont = getDefaultFont();
	}
	else 
	{
		pFont = CreateFont( FontName,iFontSize);//FindFont(FontName);
	}

	if (pFont)
	{
		pFont->DrawString(str, rect, textColor, bWordWrap, align, bUnderLine, pClipRect);
	}
}

int IUiFontManager::GetStringWidth(const std::string& str, int length/* = -1*/, const std::string& FontName/* = ""*/)
{
	if (m_Fonts.empty())
		return 0;

	RtwFont* pFont = NULL;

	if (FontName.empty())
		pFont = getDefaultFont();
	else 
		pFont = FindFont(FontName);

	if (!pFont)
		return 0;

	return pFont->GetStringWidth(str, length);
}

bool IUiFontManager::PickCharacter(const SPoint& point, int& outIndex, bool& outDoubleByte, bool& outNear, const std::string& str, const RtwRect& rect, const std::string& FontName/* = ""*/)
{
	if (m_Fonts.empty())
		return 0;

	RtwFont* pFont = NULL;

	if (FontName.empty())
		pFont = getDefaultFont();
	else 
		pFont = FindFont(FontName);

	if (!pFont)
		return false;

	return pFont->PickCharacter(point, outIndex, outDoubleByte, outNear, str, rect);
}

bool IUiFontManager::PickCharacter(int width, int& outIndex, bool& outDoubleByte, bool& outNear, const std::string& str, const std::string& FontName/* = ""*/)
{
	if (m_Fonts.empty())
		return 0;

	RtwFont* pFont = NULL;

	if (FontName.empty())
		pFont = getDefaultFont();
	else 
		pFont = FindFont(FontName);

	if (!pFont)
		return false;

	return pFont->PickCharacter(width, outIndex, outDoubleByte, outNear, str);
}

RtwFont* IUiFontManager::FindFont(const std::string& FontName , int nFontStyle /*= -1 */ , int bFontSize /*= -1 */) const 
{
	bool  bHadFond = false;
	for (std::vector<RtwFont*>::const_iterator iterFont = m_Fonts.begin(); iterFont != m_Fonts.end(); ++iterFont)
	{
		RtwFont* pFont = *iterFont;

		if (pFont->getFontName() == FontName )
		{
			if( bFontSize == -1 )
			{
				if( nFontStyle == -1 ||
					nFontStyle == pFont->GetFontStyle() )
				{
					return pFont;
				}
			}else
			{
				if( bFontSize == pFont->getFontSize() )
				{
					if( nFontStyle == -1 || nFontStyle == pFont->GetFontStyle() )
					{
						return pFont;
					}
				}
			}			
		}	
	}
	return NULL;
}

int IUiFontManager::getFontSize(const std::string& FontName/* = ""*/)
{
	if (m_Fonts.empty())
		return 0;

	RtwFont* pFont = NULL;

	if (FontName.empty())
		pFont = getDefaultFont();
	else 
		pFont = FindFont(FontName);

	if (!pFont)
		return false;

	return pFont->getFontSize();
}

void IUiFontManager::OnInvalidateDevice()
{
	foreach (std::vector<RtwFont*>, iterFont, m_Fonts)
	{
		RtwFont* pFont = *iterFont;
		pFont->OnInvalidateDevice();
	}
}

void IUiFontManager::OnRestoreDevice()
{
	foreach (std::vector<RtwFont*>, iterFont, m_Fonts)
	{
		RtwFont* pFont = *iterFont;
		pFont->OnRestoreDevice();
	}
}
