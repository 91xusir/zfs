#include "gdi_plus_font_imp.h"
using namespace Gdiplus;

void CGdiPlusFontImp::GdiPlusDrawString(const string& vrString,const RtwRect& vrTargetRect,
								 const RtwPixel& textColor,bool vUnderLine)
{/*LOG("");*/
	//�»��ߵĴ���
	Font* pFont = NULL; 
	if(vUnderLine)
	{
		pFont= &mUnderLineFont;
	}
	else
	{
		pFont = &mFont;
	}

	//�任��unicode
	wchar_t* pUnicodeStr=NULL; 
	pUnicodeStr = GbToUnicode(vrString);

	//���ָ�ʽ�Ĵ���
	StringFormat Format;
	Format.SetFormatFlags(StringFormatFlagsNoWrap|StringFormatFlagsNoClip);
	Format.SetTrimming(StringTrimmingNone);

	//����Ŀ������Ĵ���//Ҫ����߶�+1��Ϊ��GDI+�ü��򲻺��Һ��±�
	RectF TextRect(vrTargetRect.left,vrTargetRect.top,vrTargetRect.getWidth()+1,vrTargetRect.getHeight()+1);

	//gdi+����//˳�㴦�������ֵ���ɫ
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

	//���ò���
	RtwRect Clip(0,5,160,126);

	RealDrawString("��������a[1]��������b[2]��������c[3]",TestRect,vrTextColor,Wrap,Align,vUnderLine,&Clip);

	mpRender->DrawRectangle(&Pen(Color(3,0,0,255)),TestRect.left,TestRect.top,TestRect.getWidth()-1,TestRect.getHeight()-1);
	mpRender->DrawRectangle(&Pen(Color(3,255,255,255)),TestRect.left,TestRect.top,TestRect.getWidth()/2-1,TestRect.getHeight()/2-1);
	mpRender->DrawRectangle(&Pen(Color(3,255,0,0)),Clip.left,Clip.top,Clip.getWidth()-1,Clip.getHeight()-1);

}

void CGdiPlusFontImp::RealDrawString(const std::string& vrString, const RtwRect& vrTargetRect,const RtwPixel& vrTextColor, 
						bool vWordWrap,SUiTextAlignment vAlign, bool vUnderLine , const RtwRect* vpClipRect )
{/*LOG("");*/
	vector<string> AllString;

	//������
	if(vWordWrap)
	{//����
		int LineBegin=0;//�п�ʼ
		int TestLineWidth=0;//�����п�
		int LineWidth=0;//�п�
		int CharWidth=0;//�ֿ�

		for(int i=0;i!=vrString.size();)
		{
			CharWidth = (vrString[i]& 0x80) ?  2 : 1 ;//�����Ƿ��Ǻ���//�õ�����ַ��Ŀ��
			
			TestLineWidth += CharWidth ;//�õ��������п�
			if(TestLineWidth * mFontSize/2.0 > vrTargetRect.getWidth())
			{//����//����
				if(LineWidth == 0 )
				{//����Ϊ0//��ʾһ���ַ��Ѿ����ڿ��//�˳�
					break;
				}

				AllString.push_back(vrString.substr(LineBegin,LineWidth));//���һ��
				LineBegin = i;
				TestLineWidth = LineWidth =  0;
			}
			else
			{//û�л���
				LineWidth += CharWidth;
				i += CharWidth;
			}
		}
		if(LineWidth!=0)
		{
			AllString.push_back(vrString.substr(LineBegin,LineWidth));//���һ��
		}
	}
	else
	{//������
		AllString.push_back(vrString);//���һ��
	}


	//�������
	SSize   TextSize;
	TextSize.width = (AllString.size() > 1) ? vrTargetRect.getWidth() : GetStringWidth(vrString) ;		//��
	TextSize.height = (int) (getFontSize()*AllString.size());											//��

	RtwRect   TextRect;
	TextRect.ReSize(TextSize);												//��Ϊ������//�ı�Ŀ���С
	TextRect.Align(vrTargetRect,vAlign.align,vAlign.lineAlign);				//���ݲ�������


	//�������
	//����
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
		TargetRect.top = i*(mFontSize+2)+TextRect.top ;//2���м��
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
		return mFontSize/2 * (int)str.length();//��Ϊ�õ����ǵ��ֽ���//����Ҫ�������С��һ��
	}
}

bool CGdiPlusFontImp::PickCharacter(const SPoint& point, int& outIndex, 
						   bool& outDoubleByte, bool& outNear, const std::string& str, 
						   const RtwRect& rect, int RenderSize/* = 0*/)
{/*LOG("");*/
	//��û�д���
	return false;
}

bool CGdiPlusFontImp::PickCharacter(int width, int& outIndex, bool& outDoubleByte, 
						   bool& outNear, const std::string& str, int RenderSize/* = 0*/)
{/*LOG("");*/
	//��û�д���
	return false;
}

void CGdiPlusFontImp::ResetTexture(bool isReset)
{/*LOG("");*/
}