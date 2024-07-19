#include "gdi_plus_font_imp.h"
using namespace Gdiplus;

void CGdiPlusFontImp::GdiPlusDrawString(const string& vrString,const RtwRect& vrTargetRect,
								 const RtwPixel& textColor,bool vUnderLine)
{/*LOG("");*/
	//下划线的处理
	Font* pFont = NULL; 
	if(vUnderLine)
	{
		pFont= &mUnderLineFont;
	}
	else
	{
		pFont = &mFont;
	}

	//变换成unicode
	wchar_t* pUnicodeStr=NULL; 
	pUnicodeStr = GbToUnicode(vrString);

	//上字格式的处理
	StringFormat Format;
	Format.SetFormatFlags(StringFormatFlagsNoWrap|StringFormatFlagsNoClip);
	Format.SetTrimming(StringTrimmingNone);

	//上字目标区域的处理//要将宽高都+1因为，GDI+裁减框不含右和下边
	RectF TextRect(vrTargetRect.left,vrTargetRect.top,vrTargetRect.getWidth()+1,vrTargetRect.getHeight()+1);

	//gdi+上字//顺便处理了上字的颜色
	mpRender->DrawString(pUnicodeStr,-1,pFont,TextRect,
		&Format,&SolidBrush(Color(textColor)));
}

void CGdiPlusFontImp::DrawString(const std::string& vrString, const RtwRect& vrTargetRect,
								 const RtwPixel& vrTextColor,  bool vWordWrap,
								 SUiTextAlignment vAlign, bool vUnderLine , const RtwRect* vpClipRect, 
								 int RenderSize/* = 0*/)
{/*LOG("");*/
	if(vrString.empty())
	{
		return;
	}

	RealDrawString(vrString,vrTargetRect,vrTextColor,vWordWrap,vAlign,vUnderLine,vpClipRect);
}

void CGdiPlusFontImp::DrawString3D(const std::string& str, const SRect3D& rect, const RtwPixel& textColor,
								   bool bWordWrap , SUiTextAlignment align ,bool bUnderLine ,
								   const RtwRect* pClipRect, int RenderSize /* = 0 */)
{/*LOG("");*/
}
void CGdiPlusFontImp::TestDrawString(const std::string& vrString, const RtwRect& vrTargetRect,const RtwPixel& vrTextColor, 
								 bool vWordWrap,SUiTextAlignment vAlign, bool vUnderLine , const RtwRect* vpClipRect )
{/*LOG("");*/
	RtwRect TestRect(100,100,199,499);
	SUiTextAlignment Align;
	Align.align = alignNear;
	Align.lineAlign = alignNear;

	bool Wrap=true;

	//剪裁测试
	RtwRect Clip(0,5,160,126);

	RealDrawString("测试上字a[1]测试上字b[2]测试上字c[3]",TestRect,vrTextColor,Wrap,Align,vUnderLine,&Clip);

	mpRender->DrawRectangle(&Pen(Color(3,0,0,255)),TestRect.left,TestRect.top,TestRect.getWidth()-1,TestRect.getHeight()-1);
	mpRender->DrawRectangle(&Pen(Color(3,255,255,255)),TestRect.left,TestRect.top,TestRect.getWidth()/2-1,TestRect.getHeight()/2-1);
	mpRender->DrawRectangle(&Pen(Color(3,255,0,0)),Clip.left,Clip.top,Clip.getWidth()-1,Clip.getHeight()-1);

}

void CGdiPlusFontImp::RealDrawString(const std::string& vrString, const RtwRect& vrTargetRect,const RtwPixel& vrTextColor, 
						bool vWordWrap,SUiTextAlignment vAlign, bool vUnderLine , const RtwRect* vpClipRect )
{/*LOG("");*/
	vector<string> AllString;

	//处理换行
	if(vWordWrap)
	{//换行
		int LineBegin=0;//行开始
		int TestLineWidth=0;//测试行宽
		int LineWidth=0;//行宽
		int CharWidth=0;//字宽

		for(int i=0;i!=vrString.size();)
		{
			CharWidth = (vrString[i]& 0x80) ?  2 : 1 ;//根据是否是汉字//得到这个字符的宽度
			
			TestLineWidth += CharWidth ;//得到测试用行宽
			if(TestLineWidth * mFontSize/2.0 > vrTargetRect.getWidth())
			{//超过//换行
				if(LineWidth == 0 )
				{//长度为0//表示一个字符已经大于宽度//退出
					break;
				}

				AllString.push_back(vrString.substr(LineBegin,LineWidth));//添加一行
				LineBegin = i;
				TestLineWidth = LineWidth =  0;
			}
			else
			{//没有换行
				LineWidth += CharWidth;
				i += CharWidth;
			}
		}
		if(LineWidth!=0)
		{
			AllString.push_back(vrString.substr(LineBegin,LineWidth));//最后一行
		}
	}
	else
	{//不换行
		AllString.push_back(vrString);//添加一行
	}


	//处理对齐
	SSize   TextSize;
	TextSize.width = (AllString.size() > 1) ? vrTargetRect.getWidth() : GetStringWidth(vrString) ;		//宽
	TextSize.height = (int) (getFontSize()*AllString.size());											//高

	RtwRect   TextRect;
	TextRect.ReSize(TextSize);												//因为不缩放//改变目标大小
	TextRect.Align(vrTargetRect,vAlign.align,vAlign.lineAlign);				//根据参数对齐


	//处理剪裁
	//剪裁
	static Region OldClip;

	if(vpClipRect!=NULL)
	{
		mpRender->GetClip(&OldClip);
		mpRender->SetClip(Rect(vpClipRect->left,vpClipRect->top,vpClipRect->getWidth(),vpClipRect->getHeight()));
	}


	RtwRect TargetRect;
	for(int i=0;i!=AllString.size();++i)
	{
		TargetRect.left=TextRect.left;
		TargetRect.top = i*(mFontSize+2)+TextRect.top ;//2是行间距
		TargetRect.right = TextRect.right;
		TargetRect.bottom = TargetRect.top + mFontSize;

		GdiPlusDrawString(AllString[i],TargetRect,vrTextColor,vUnderLine);
	}

	if(vpClipRect!=NULL)
	{
		mpRender->SetClip(&OldClip);
	}
}

int CGdiPlusFontImp::GetStringWidth(const std::string& str, int length, int RenderSize/* = 0*/)
{/*LOG("");*/
	if (length >= 0)
	{
		return mFont.GetSize() * length;
	}
	else
	{
		return mFontSize/2 * (int)str.length();//因为得到的是单字节数//所以要乘字体大小的一半
	}
}

bool CGdiPlusFontImp::PickCharacter(const SPoint& point, int& outIndex, 
						   bool& outDoubleByte, bool& outNear, const std::string& str, 
						   const RtwRect& rect, int RenderSize/* = 0*/)
{/*LOG("");*/
	//还没有处理
	return false;
}

bool CGdiPlusFontImp::PickCharacter(int width, int& outIndex, bool& outDoubleByte, 
						   bool& outNear, const std::string& str, int RenderSize/* = 0*/)
{/*LOG("");*/
	//还没有处理
	return false;
}

void CGdiPlusFontImp::ResetTexture(bool isReset)
{/*LOG("");*/
}