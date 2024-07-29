#include "gc_include.h"
#include "ui_form_analyse_image.h"

UIFormAnalyseImage::UIFormAnalyseImage()
{
	guard;

	m_pFrmThis = LOAD_UI("fmwaigua");
	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);

	m_pBtnApplyNext = LOAD_UI("fmwaigua.btnrestart");
	//m_pText = LOAD_UI("fmwaigua.lbtxt");
	m_pImage = LOAD_UI("fmwaigua.btnPic");
	m_pTimeLeft = LOAD_UI("fmwaigua.lbtimenum");
	m_arrSelections[0] = LOAD_UI("fmwaigua.btnno1");
	m_arrSelections[1] = LOAD_UI("fmwaigua.btnno2");
	m_arrSelections[2] = LOAD_UI("fmwaigua.btnno3");
	m_arrSelections[3] = LOAD_UI("fmwaigua.btnno4");
    m_arrSelections[4] = LOAD_UI("fmwaigua.btnno5");
    m_arrSelections[5] = LOAD_UI("fmwaigua.btnno6");
    m_arrSelections[6] = LOAD_UI("fmwaigua.btnno7");
    m_arrSelections[7] = LOAD_UI("fmwaigua.btnno8");
    m_pType1Hint = LOAD_UI("fmwaigua.lbtxt");
    m_pType2Hint = LOAD_UI("fmwaigua.lbsecondtime");
    m_pType2Text = LOAD_UI("fmwaigua.txtinputno");
    m_pType2Ok = LOAD_UI("fmwaigua.btnok");

	for (int i=0; i<ANALYSE_IMAGE_SELECTION_COUNT; i++)
	{
		m_arrSelections[i]->EvLClick += RTW_CALLBACK(this, UIFormAnalyseImage, OnSelect);
	}
	m_pBtnApplyNext->EvLClick += RTW_CALLBACK(this, UIFormAnalyseImage, OnBtnApplyNext);
    m_pType2Ok->EvLClick += RTW_CALLBACK(this, UIFormAnalyseImage, OnBtnOK);
    m_pType2Text->EvKey += RTW_CALLBACK(this, UIFormAnalyseImage, OnKeyType2Text);

	m_pImage->SetText("");
	ClearData();
	m_pFrmThis->Hide();

	CM_MEMPROTECTOR(m_Characters, ANALYSE_IMAGE_SELECTION_COUNT)

	unguard;
}

UIFormAnalyseImage::~UIFormAnalyseImage()
{
	guard;
	CM_MEMUNPROTECTOR(m_Characters)
	unguard;
}

#pragma pack(push, bmp_struct, 1)

struct SRandomImageBmpHead2
{
	unsigned short  bfType;
	int             bfSize;
	unsigned int    bfReserved;
	int             bfDataOff;
	int             biSize;
	int             biWidth;
	int             biHeight;
	short           biPlanes;
	short           biBitCount;
	int             biCompression;
	int             biSizeImage;
	int             biXPelsPerMeter;
	int             biYPelsPerMeter;
	int             biClrUsed;
	int             biClrImportant;
};

#pragma pack(pop, bmp_struct)

void UIFormAnalyseImage::SetData(const char* data, const long dataLen, const char* strSelections, int nSelectNum, int nSecondsLeft, int nResendLeft, int nErrorLeft)
{
	guard;

	DWORD nowTick = rtGetMilliseconds();

	{
	// 解压缩图片
		bool bDeCompressSuccess = true;
		char* pRLEData = (char*)data;
		int iRLEDataSize = dataLen;
		unsigned char pRawData2[32*32*5+sizeof(SRandomImageBmpHead2)], *pRawData;
		int iRawDataSize = 32*32*5;

		unsigned char dwTextColorR = 0, dwTextColorG = 0, dwTextColorB = 0;
		unsigned char dwBGColorR=0xFF, dwBGColorG=0xFF, dwBGColorB=0xFF;

		//////////////////////////////

		SRandomImageBmpHead2* pBmpHead = (SRandomImageBmpHead2*)pRawData2;
		pRawData = pRawData2+sizeof(SRandomImageBmpHead2);
        int iImageFileSize = 32*32*3+sizeof(SRandomImageBmpHead2);

		pBmpHead->bfType = ('B'|('M'<<8));
		pBmpHead->bfSize = iImageFileSize;
		pBmpHead->bfReserved = ('J'|('H'<<8)|('.'<<16)| (0<<24));
		pBmpHead->bfDataOff = sizeof(SRandomImageBmpHead2);

		pBmpHead->biSize = 0x28;
		pBmpHead->biWidth  = 32;
		pBmpHead->biHeight = -32;
		pBmpHead->biPlanes = 1;
		pBmpHead->biBitCount = 24;
		pBmpHead->biCompression = 0;
		pBmpHead->biSizeImage = 0;
		pBmpHead->biXPelsPerMeter = 0;
		pBmpHead->biYPelsPerMeter = 0;
		pBmpHead->biClrUsed = 0;
		pBmpHead->biClrImportant = 0;

		int i, j, iRawPos=0;
		iRawDataSize /= 4;

		for (i=0; i<iRLEDataSize; i++)
		{
			if (pRLEData[i]&0x80)
			{
				j = pRLEData[i]&0x3F;
				if (pRLEData[i]&0x40)
				{
					while (j>0)
					{
						pRawData[iRawPos++] = dwTextColorR;
						pRawData[iRawPos++] = dwTextColorG;
						pRawData[iRawPos++] = dwTextColorB;
						if (iRawPos>=iRawDataSize)		bDeCompressSuccess=false;
						j --;
					}
				}else
				{
					while (j>0)
					{
						pRawData[iRawPos++] = dwBGColorR;
						pRawData[iRawPos++] = dwBGColorG;
						pRawData[iRawPos++] = dwBGColorB;
						if (iRawPos>=iRawDataSize)		bDeCompressSuccess = false;
						j --;
					}
				}
			}else
			{
				for (j=0; j<7; j++)
				{
					if (pRLEData[i]& (0x01<<j))
					{
						pRawData[iRawPos++] = dwTextColorR;
						pRawData[iRawPos++] = dwTextColorG;
						pRawData[iRawPos++] = dwTextColorB;
						if (iRawPos>=iRawDataSize)		bDeCompressSuccess = true;
					}else
					{
						pRawData[iRawPos++] = dwBGColorR;
						pRawData[iRawPos++] = dwBGColorG;
						pRawData[iRawPos++] = dwBGColorB;
						if (iRawPos>=iRawDataSize)		bDeCompressSuccess = true;
					}
				}
			}
		}
        unsigned char* pBuf;
        for (i=1; i<31; i++)
        {
            for (j=1; j<31; j++)
            {
                if (pRawData[(i*32+j)*3]==dwTextColorR)
                {
                    pBuf = pRawData+(((i-1)<<5)+(j-1))*3;
                    if (*pBuf!=dwTextColorR)
                    {
                        pBuf[2] = pBuf[1] = pBuf[0] = 0xbf;
                    }
                    pBuf = pRawData+(((i-1)<<5)+(j+1))*3;
                    if (*pBuf!=dwTextColorR)
                    {
                        pBuf[2] = pBuf[1] = pBuf[0] = 0xbf;
                    }
                    pBuf = pRawData+(((i+1)<<5)+(j-1))*3;
                    if (*pBuf!=dwTextColorR)
                    {
                        pBuf[2] = pBuf[1] = pBuf[0] = 0xbf;
                    }
                    pBuf = pRawData+(((i+1)<<5)+(j+1))*3;
                    if (*pBuf!=dwTextColorR)
                    {
                        pBuf[2] = pBuf[1] = pBuf[0] = 0xbf;
                    }
                }
            }
        }
        /*
        // 新加 20051221
        for (i=1; i<31; i++)
        {
            for (j=1; j<31; j++)
            {
                if (pRawData[(i*32+j)*3]==dwBGColorR)
                {
                    pBuf = pRawData+(((i)<<5)+(j))*3;
                    pBuf[2] = pBuf[1] = pBuf[0] = dwTextColorR;
                }
            }
        }
        // 新加结束
        */

        unsigned char* pBuf2;
        for (i=0; i<16; i++)
        {
            for (j=0; j<32; j++)
            {
                pBuf = pRawData+((i<<5)+j)*3;
                pBuf2 = pRawData+(((31-i)<<5)+j)*3;
                dwBGColorR = pBuf[0];
                dwBGColorG = pBuf[1];
                dwBGColorB = pBuf[2];
				//pBuf[0] = 255;
                pBuf[0] = pBuf2[0];
                pBuf[1] = pBuf2[1];
				pBuf[2] = pBuf2[2];
				//pBuf2[0] = 255;
                pBuf2[0] = dwBGColorR;
                pBuf2[1] = dwBGColorG;
				pBuf2[2] = dwBGColorB;
            }
        }
		////////////////////////////////
		// 设置图片
// 		RtwImage *pImage = new RtwImage();
// 		pImage->SetImage((void*)pRawData2, iImageFileSize);// change [3/17/2009 tooth.shi]
		//RtwImage *pImage = g_workspace.getImageFactory()->LoadFromMemory(32, 32 , iImageFileSize ,(void*)pRawData2);
		//RtwImage *pComImage = g_workspace.getImageFactory()->createImageSequence();

 		RtwImage *pImage = g_workspace.getImageFactory()->createImage((void*)pRawData2, iImageFileSize);
		//pComImage->
		//pImage->border = NULL;

		//if (!m_pImage->GetBackground())
		{
			m_pImage->SetBackgroundImage(pImage);
		}

        /*
        FILE* fp = fopen("randomimage.bmp", "wb");
        fwrite(pRawData2, 1, iImageFileSize, fp);
        fclose(fp);
        */
    }

	for (int i=0; i<ANALYSE_IMAGE_SELECTION_COUNT; i++)
	{
		m_Characters[i] = *(strSelections + i);
	}
	m_nSelectNum = nSelectNum;
	m_nEndTick = nowTick + nSecondsLeft*1000;
	m_nResendLeft = nResendLeft;
	m_nErrorLeft = nErrorLeft;

    m_pType2Text->SetText("");

	for (int i=0; i<ANALYSE_IMAGE_SELECTION_COUNT; i++)
	{
		m_arrSelections[i]->Enable();
	}
    m_pType2Ok->Enable();

	m_dwSetDataTick = rtGetMilliseconds();

	//char cTmp128[128];
	//sprintf(cTmp128, "strSelections=%s, selNum=%d, SecondsLeft=%d, ResendLeft=%d, ErrorLeft=%d", strSelections, nSelectNum, nSecondsLeft, nResendLeft, nErrorLeft);
	//PlayerFailOperate(cTmp128);

	unguard;
}

void UIFormAnalyseImage::ClearData()
{
	guard;

	m_pImage->SetBackgroundImage((RtwImage*)NULL);
	for (int i=0; i<ANALYSE_IMAGE_SELECTION_COUNT; i++)
	{
		m_Characters[i] = '\0';
	}
	m_nSelectNum = -1;
	m_nEndTick = 0;
	m_nResendLeft = 0;
	m_nErrorLeft = 0;

	m_dwSetDataTick = 0;

	unguard;
}

void UIFormAnalyseImage::AnswerQuestion(const std::string& str)
{
	guard;

	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_check_answer);
	pPacket->WriteByte(0);
	pPacket->WriteString((char*)str.c_str());
	NetSend(pPacket);

	for (int i=0; i<ANALYSE_IMAGE_SELECTION_COUNT; i++)
	{
		m_arrSelections[i]->Disable();
        m_pType2Ok->Disable();
	}

	unguard;
}

void UIFormAnalyseImage::AskForAnotherString()
{
	guard;

	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_check_answer);
	pPacket->WriteByte(1);
	NetSend(pPacket);

	for (int i=0; i<ANALYSE_IMAGE_SELECTION_COUNT; i++)
	{
		m_arrSelections[i]->Disable();
	}

	unguard;
}

void UIFormAnalyseImage::OnPreDraw()
{
	guard;

    char cTmp256[256];
    if (m_nSelectNum >= 0)
    {
        // 提示信息
        m_pType1Hint->Show();
        m_pType2Hint->Hide();
	    rt2_sprintf(cTmp256, R(MSG_IMAGE_SELECTION_HINT), m_nSelectNum);
	    m_pType1Hint->SetText(cTmp256);

        // 字母列表
	    for (int i=0; i<ANALYSE_IMAGE_SELECTION_COUNT; i++)
	    {
		    string text;
		    text += m_Characters[i];
            m_arrSelections[i]->Show();
		    m_arrSelections[i]->SetText(text);
	    }

        // 文本框
        m_pType2Text->Hide();

        // 按钮
        m_pType2Ok->Hide();
    }
    else
    {
        // 提示信息
        m_pType1Hint->Hide();
        m_pType2Hint->Show();

        // 字母列表
	    for (int i=0; i<ANALYSE_IMAGE_SELECTION_COUNT; i++)
	    {
		    m_arrSelections[i]->Hide();
	    }

        // 文本框
        m_pType2Text->Show();

        // 按钮
        m_pType2Ok->Show();
    }

    // 剩余时间
	int nowTick = rtGetMilliseconds();
	int nSecondsLeft = (m_nEndTick-nowTick)/1000;
	if (nSecondsLeft<0)
		nSecondsLeft = 0;
	rt2_sprintf(cTmp256, R(MSG_IMAGE_SELECTION_TIME_LEFT), nSecondsLeft);
	m_pTimeLeft->SetText(cTmp256);

    // “下一个”按钮
	if (m_nResendLeft==0)
	{
		m_pBtnApplyNext->Disable();
	}
	else 
	{
		m_pBtnApplyNext->Enable();
	}

    // “重发”按钮
	rt2_sprintf(cTmp256, R(MSG_RESEND_IMAGE_BTN), m_nResendLeft);
	m_pBtnApplyNext->SetText(cTmp256);

	unguard;
}

void UIFormAnalyseImage::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormAnalyseImage::Show()
{
	guard;

	OnPreDraw();
	m_pFrmThis->Show();

	unguard;
}

bool UIFormAnalyseImage::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormAnalyseImage::Hide()
{
	guard;
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormAnalyseImage::OnSelect(RtwWidget* sender, void*)
{
	guard;
	int i;
	for (i=0; i<ANALYSE_IMAGE_SELECTION_COUNT; i++)
	{
		if (sender == m_arrSelections[i])
			break;
	}
	if (i>=ANALYSE_IMAGE_SELECTION_COUNT)
	{
		CHECK(0);
		return;
	}

	DWORD dwNowTick = rtGetMilliseconds();
	if (dwNowTick - m_dwSetDataTick < 1500)
	{
		return;
	}

    std::string str;
    str += m_Characters[i];
    AnswerQuestion(str);

	unguard;
}

void UIFormAnalyseImage::OnBtnApplyNext(RtwWidget* sender, void*)
{
	guard;
	AskForAnotherString();
	unguard;
}

void UIFormAnalyseImage::OnBtnOK(RtwWidget* sender, void*)
{
    guard;

    if (m_nSelectNum>=0)
        return;

    string str = m_pType2Text->GetText();
    strupr((char*)str.c_str());
    AnswerQuestion(str);

    unguard;
}

void UIFormAnalyseImage::OnKeyType2Text(RtwWidget* sender, void*)
{
    guard;

    unguard;
}

void UIFormAnalyseImage::Run()
{
	guard;

	static int dwLastUpdateTick = 0;
	int dwNowTick = rtGetMilliseconds();
	if (dwNowTick - dwLastUpdateTick > 600)
	{
		Refresh();
		dwLastUpdateTick = dwNowTick;
	}

	unguard;
}
