#include "ui/rtw_ui.h"
#include <fstream>
#include <string.h>
#include <vector>
#include "gif89a.h"

GLOBAL_INFO *pgInfo = NULL;
CGif89a::CGif89a():opened(FALSE), error(FALSE), m_nCurPos(0), m_nBufSize(0), m_pBuf(NULL)
{
	CM_MEMPROTECTOR(version, 4)
	//CM_MEMPROTECTOR(gColorTable, 256*3)
	//CM_MEMPROTECTOR(lColorTable, 256*3)
}

CGif89a::CGif89a(LPCSTR fileName,BOOL inMem):opened(FALSE), m_nCurPos(0), m_nBufSize(0), m_pBuf(NULL)
{	
	CM_MEMPROTECTOR(version, 4)
	//CM_MEMPROTECTOR(gColorTable, 256*3)
	//CM_MEMPROTECTOR(lColorTable, 256*3)

	if(open(fileName,inMem))
	{
		opened = TRUE;
		error = FALSE;
	}
	else
	{
		opened = FALSE;
		error = TRUE;
	}
}

CGif89a::CGif89a(const CGif89a &object)
{
	memcpy(this, &object, sizeof(CGif89a));
	CM_MEMPROTECTOR(version, 4)
	//CM_MEMPROTECTOR(gColorTable, 256*3)
	//CM_MEMPROTECTOR(lColorTable, 256*3)
}

CGif89a::~CGif89a()
{
	CM_MEMUNPROTECTOR(version)
	//CM_MEMUNPROTECTOR(gColorTable)
	//CM_MEMUNPROTECTOR(lColorTable)
	close();
}

BOOL CGif89a::operator!()
{
	return error;
}

BOOL CGif89a::open(LPCSTR fileName,BOOL b)
{
	//初始化,如果有值,则删除
	if (m_pBuf)
	{
		DEL_ARRAY(m_pBuf);
		m_pBuf = NULL;
	}
	m_nCurPos = 0;
	m_nBufSize = 0;

	//使用通用filepack文件类读取
	RtArchive* readfile = RtCoreFile().CreateFileReader(fileName);
	if (!readfile) return FALSE;

	m_nBufSize = readfile->TotalSize()+1;
	m_pBuf = RT_NEW char[m_nBufSize];
	readfile->Serialize(m_pBuf, readfile->TotalSize());
	m_pBuf[m_nBufSize-1]='\0';
	readfile->Close();
	DEL_ONE(readfile);

	char cc[4];
	BYTE be;
	BOOL fileEnd = FALSE;
	inMem = b;
	allFrames = NULL;
	curIndex = 0;
	curFrame.pColorTable = NULL;
	curFrame.dataBuf = NULL;
	ctrlExt.active = FALSE;

	if(opened)
		return FALSE;

	memcpy(cc, m_pBuf+m_nCurPos, 3);
	m_nCurPos += 3;

	if(strncmp(cc,"GIF",3) != 0)
		goto error;

	memcpy(version, m_pBuf+m_nCurPos, 3);
	m_nCurPos += 3;
	version[3] = 0;

	if(strncmp(version,"89a",3) > 0)
		goto error;

	memcpy((char*)&gInfo.scrWidth,	m_pBuf+m_nCurPos, 2);
	m_nCurPos += 2;

	memcpy((char*)&gInfo.scrHeight, m_pBuf+m_nCurPos, 2);
	m_nCurPos += 2;

	if (m_nCurPos >= m_nBufSize)
	{
		goto error;
	}

	memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
	m_nCurPos += 1;

	if((be&0x80) != 0)
		gInfo.gFlag = TRUE;
	else
		gInfo.gFlag = FALSE;
	gInfo.colorRes = ((be&0x70)>>4)+1;

	if(gInfo.gFlag)
	{
		if((be&0x08) != 0)
			gInfo.gSort = TRUE;
		else
			gInfo.gSort = FALSE;
		gInfo.gSize = 1;
		gInfo.gSize <<= ((be&0x07)+1);
	}

	memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
	m_nCurPos += 1;

	gInfo.BKColorIdx = be;

	memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
	m_nCurPos += 1;
	gInfo.pixelAspectRatio = be;

	if (m_nCurPos >= m_nBufSize)
	{
		goto error;
	}

	if(gInfo.gFlag)
	{
		memcpy((char*)gColorTable, m_pBuf+m_nCurPos, gInfo.gSize*3);
		m_nCurPos += gInfo.gSize*3;
		gInfo.gColorTable = gColorTable;
	}
	else
		gInfo.gColorTable = NULL;

	dataStart = m_nCurPos;

	if((gInfo.frames = checkFrames()) == 0)
		goto error;

	if(inMem)
	{	
		if((allFrames = RT_NEW FRAME[gInfo.frames]) == NULL)
			goto error;
		ZeroMemory(allFrames,sizeof(FRAME)*gInfo.frames);
		if(!getAllFrames())
		{	
			delete[] allFrames;
			allFrames = NULL;
			goto error;
		}
	}

	opened = TRUE;
	pgInfo = &gInfo;
	return TRUE;
error:
	return FALSE;
}

UINT CGif89a::checkFrames()
{
	BYTE be;
	BOOL fileEnd = FALSE;
	UINT frames=0;
	streampos pos = m_nCurPos;
	while(m_nCurPos<m_nBufSize && !fileEnd)
	{
		memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
		m_nCurPos += 1;
		switch(be)
		{
		case 0x21:
			memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
			m_nCurPos += 1;
			switch(be)
			{
			case 0xf9:
			case 0xfe:
			case 0x01:
			case 0xff:
				while (m_nCurPos < m_nBufSize)
				{	
					memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
					m_nCurPos += 1;
					if(be == 0)
						break;
					m_nCurPos += be;
				}
				break;
			default:
				return 0;
			}
			break;
		case 0x2c:
			{
				BYTE bp;
				BOOL lFlag=FALSE;
				UINT lSize=1;

				frames++;
				m_nCurPos += 8;

				memcpy((char*)&bp, m_pBuf+m_nCurPos, 1);
				m_nCurPos += 1;

				if((bp&0x80) != 0)
					lFlag = TRUE;

				lSize <<= ((bp&0x07)+1);

				if(lFlag)
				{
					m_nCurPos += lSize*3;
				}

				if (m_nCurPos >= m_nBufSize)
				{
					return 0;
				}

				memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
				m_nCurPos += 1;

				while(m_nCurPos < m_nBufSize)
				{
					memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
					m_nCurPos += 1;
					if(be == 0)
						break;
					m_nCurPos += be;
				}
				break;
			}
		case 0x3b:
			fileEnd = TRUE;
			break;
		case 0x00:
			break;
		default:
			return 0;
		}
	}

	m_nCurPos = pos;
	return frames;
}

BOOL CGif89a::getAllFrames()
{	
	BYTE be;
	BOOL fileEnd = FALSE;
	FRAME *pf = allFrames;
	streampos pos = m_nCurPos;
	int i;
	while(m_nCurPos<m_nBufSize && !fileEnd)
	{
		memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
		m_nCurPos += 1;
		switch(be)
		{
		case 0x21:
			memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
			m_nCurPos += 1;
			switch(be)
			{	
			case 0xf9:
				while(m_nCurPos<m_nBufSize)
				{
					memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
					m_nCurPos += 1;
					if(be == 0)
						break;

					if(be == 4)
					{
						ctrlExt.active = TRUE;
						memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
						m_nCurPos += 1;
						ctrlExt.disposalMethod = (be&0x1c)>>2;
						if((be&0x02) != 0)
							ctrlExt.userInputFlag = TRUE;
						else
							ctrlExt.userInputFlag = FALSE;
						if((be&0x01) != 0)
							ctrlExt.trsFlag = TRUE;
						else
							ctrlExt.trsFlag = FALSE;

						memcpy((char*)&ctrlExt.delayTime, m_pBuf+m_nCurPos, 2);
						m_nCurPos += 2;

						memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
						m_nCurPos += 1;
						ctrlExt.trsColorIndex = be;
					}
					else
						m_nCurPos += be;
				}
				break;
			case 0xfe:
			case 0x01:
			case 0xff:
				while(m_nCurPos<m_nBufSize)
				{
					memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
					m_nCurPos += 1;

					if(be == 0)
						break;
					m_nCurPos += be;
				}
				break;
			default:
				goto error;
			}
			break;
		case 0x2c:
			{
				BYTE bp;
				memcpy((char*)&pf->imageLPos,	m_pBuf+m_nCurPos, 2);
				m_nCurPos += 2;

				memcpy((char*)&pf->imageTPos,	m_pBuf+m_nCurPos, 2);
				m_nCurPos += 2;

				memcpy((char*)&pf->imageWidth,	m_pBuf+m_nCurPos, 2);
				m_nCurPos += 2;

				memcpy((char*)&pf->imageHeight, m_pBuf+m_nCurPos, 2);
				m_nCurPos += 2;

				memcpy((char*)&bp, m_pBuf+m_nCurPos, 1);
				m_nCurPos += 1;

				if((bp&0x80) != 0)
					pf->lFlag = TRUE;

				if((bp&0x40) != 0)
					pf->interlaceFlag = TRUE;

				if((bp&0x20) != 0)
					pf->sortFlag = TRUE;

				pf->lSize = 1;
				pf->lSize <<= ((bp&0x07)+1);

				if(pf->lFlag)
				{
					if((pf->pColorTable = RT_NEW BYTE[pf->lSize*3]) == NULL)
						goto error;

					memcpy((char*)&pf->pColorTable, m_pBuf+m_nCurPos, pf->lSize*3);
					m_nCurPos += pf->lSize*3;
				}
				if(m_nCurPos>=m_nBufSize)
					goto error;

				if(!extractData(pf))
					goto error;

				if(ctrlExt.active)
				{
					pf->ctrlExt = ctrlExt;
					ctrlExt.active = FALSE;
				}
				pf++;
				break;
			}
		case 0x3b:
			fileEnd = TRUE;
			break;
		case 0x00:
			break;
		default:
			goto error;
		}
	}
	m_nCurPos = pos;
	return TRUE;
error:
	pf = allFrames;
	for(i=0;i<(int)gInfo.frames;++i)
	{
		if(pf->pColorTable != NULL)
		{	
			delete[] pf->pColorTable;
			pf->pColorTable = NULL;
		}

		if(pf->dataBuf != NULL)
		{
			delete[] pf->dataBuf;
			pf->dataBuf = NULL;
		}
	}
	return FALSE;
}

LPCFRAME CGif89a::getNextFrame()
{	
	if(inMem)
	{
		FRAME* p =  allFrames+curIndex;
		curIndex++;

		if(curIndex >= gInfo.frames)
			curIndex = 0;

		return p;
	}
	else
	{	
		BYTE be;
		BOOL fileEnd = FALSE;
		if(curFrame.pColorTable != NULL)
		{	
			delete[] curFrame.pColorTable;
			curFrame.pColorTable = NULL;
		}

		if(curFrame.dataBuf != NULL)
		{
			delete[] curFrame.dataBuf;
		curFrame.dataBuf = NULL;
		}

		ZeroMemory(&curFrame,sizeof(FRAME));
		while(TRUE)
		{
			memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
			m_nCurPos += 1;
			switch(be)
			{	
			case 0x21:
				memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
				m_nCurPos += 1;
				switch(be)
				{
				case 0xf9:
					while(m_nCurPos<m_nBufSize)
					{	
						memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
						m_nCurPos += 1;
						if(be == 0)
							break;
						if(be == 4)
						{	
							ctrlExt.active = TRUE;
							memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
							m_nCurPos += 1;
							ctrlExt.disposalMethod = (be&0x1c)>>2;

							if((be&0x02) != 0)
								ctrlExt.userInputFlag = TRUE;
							else
								ctrlExt.userInputFlag = FALSE;

							if((be&0x01) != 0)
								ctrlExt.trsFlag = TRUE;
							else
								ctrlExt.trsFlag = FALSE;

							memcpy((char*)&ctrlExt.delayTime, m_pBuf+m_nCurPos, 2);
							m_nCurPos += 2;

							memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
							m_nCurPos += 1;
							ctrlExt.trsColorIndex = be;
						}
						else
						{
							m_nCurPos += be;
						}
					}
					break;
				case 0xfe:
				case 0x01:
				case 0xff:
					while(m_nCurPos<m_nBufSize)
					{
						memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
						m_nCurPos += 1;
						if(be == 0)
							break;

						m_nCurPos += be;
					}
					break;
				default:
					goto error;
				}
				break;
			case 0x2c:
				{
					BYTE bp;
					memcpy((char*)&curFrame.imageLPos,	m_pBuf+m_nCurPos, 2);
					m_nCurPos += 2;

					memcpy((char*)&curFrame.imageTPos,	m_pBuf+m_nCurPos, 2);
					m_nCurPos += 2;

					memcpy((char*)&curFrame.imageWidth,	m_pBuf+m_nCurPos, 2);
					m_nCurPos += 2;

					memcpy((char*)&curFrame.imageHeight, m_pBuf+m_nCurPos, 2);
					m_nCurPos += 2;

					memcpy((char*)&bp, m_pBuf+m_nCurPos, 1);
					m_nCurPos += 1;

					if((bp&0x80) != 0)
						curFrame.lFlag = TRUE;

					if((bp&0x40) != 0)
						curFrame.interlaceFlag = TRUE;

					if((bp&0x20) != 0)
						curFrame.sortFlag = TRUE;

					curFrame.lSize = 1;
					curFrame.lSize <<= ((bp&0x07)+1);

					if((curFrame.pColorTable = RT_NEW BYTE[curFrame.lSize*3]) == NULL)
						goto error;

					if(curFrame.lFlag)
					{
						memcpy((char*)&curFrame.pColorTable, m_pBuf+m_nCurPos, curFrame.lSize*3);
						m_nCurPos += curFrame.lSize*3;
					}

					if(m_nCurPos>=m_nBufSize)
						goto error;

					if(!extractData(&curFrame))
						goto error;

					curFrame.ctrlExt = ctrlExt;

					if(ctrlExt.active = TRUE)
						ctrlExt.active = FALSE;

					return &curFrame;
				}
			case 0x3b:
				m_nCurPos = dataStart;
				break;
			case 0x00:
				break;
			default:
				goto error;
			}
		}	
		return &curFrame;
error:
		if(curFrame.pColorTable != NULL)
		{	
			delete[] curFrame.pColorTable;
			curFrame.pColorTable = NULL;
		}

		if(curFrame.dataBuf != NULL)
		{	
			delete[] curFrame.dataBuf;
			curFrame.dataBuf = NULL;
		}
		return NULL;
	}
}

BOOL CGif89a::initStrTable(STRING_TABLE_ENTRY* strTable,UINT rootSize)
{	
	UINT i;
	unsigned char *cc;
	for(i=0;i<rootSize;++i)
	{	
		if((cc = RT_NEW unsigned char[2]) == NULL)
			goto error;

		cc[0] = i,cc[1] = 0;
		strTable[i].p = cc;
		strTable[i].len = 1;
	}

	return TRUE;
error:
	for(i=0;i<rootSize;++i)
		if(strTable[i].p != NULL)
		{	
			delete[] strTable[i].p;
			strTable[i].p = NULL;
		}
		return FALSE;
}

BOOL CGif89a::addStrTable(STRING_TABLE_ENTRY* strTable,UINT addIdx,UINT idx,unsigned char c)
{
	unsigned char *cc;
	UINT l = strTable[idx].len;
	if(addIdx >= 4096)
		return FALSE;

	if((cc = RT_NEW unsigned char[l+2]) == NULL)
		return FALSE;

	for(UINT i=0;i<l;++i)
		cc[i] = strTable[idx].p[i];

	cc[l] = c;
	cc[l+1] = 0;
	strTable[addIdx].p = cc;
	strTable[addIdx].len = strTable[idx].len +1;
	return TRUE;
}

BOOL CGif89a::extractData(FRAME* f)
{
	STRING_TABLE_ENTRY *strTable;
	UINT codeSize,rootSize,tableIndex,codeSizeBK;
	int remainInBuf = 0,i;
	UINT bufIndex = 0,outIndex = 0;
	UINT bitIndex = 0;
	DWORD code,oldCode;
	BYTE be,*outP;
	BYTE buf[262];
	BOOL readOK = FALSE;
	UINT bufLen = f->imageWidth*f->imageHeight;
	if((strTable = RT_NEW STRING_TABLE_ENTRY[4096]) == NULL)
		return FALSE;

	ZeroMemory(strTable,sizeof(STRING_TABLE_ENTRY)*4096);
	outP = f->dataBuf = RT_NEW BYTE[bufLen];

	if(f->dataBuf == NULL)
	{
		delete[] strTable;
		return FALSE;
	}

	memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
	m_nCurPos += 1;
	codeSizeBK = codeSize = be+1;
	rootSize = 1;
	rootSize <<= be; 
	tableIndex = rootSize+2;

	if(!initStrTable(strTable,rootSize))
		goto error;
begin:
	if(remainInBuf<=4 && !readOK)
	{	
		for(i=0;i<remainInBuf;++i)
			buf[i] = buf[bufIndex+i];

		bufIndex = 0;
		memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
		m_nCurPos += 1;

		if(be != 0)
		{
			memcpy((char*)(buf+remainInBuf), m_pBuf+m_nCurPos, be);
			m_nCurPos += be;
			remainInBuf += be;
		}
		else
			readOK = TRUE;

		if(m_nCurPos >= m_nBufSize)
			goto error;
	}
	if(remainInBuf<=4)
		if(remainInBuf<=0 || codeSize > (remainInBuf*8-bitIndex))
			goto done;

	code = *((DWORD*)(buf+bufIndex));
	code <<= 32-codeSize-bitIndex;
	code >>= 32-codeSize;
	bitIndex += codeSize;
	bufIndex += bitIndex/8;
	remainInBuf -= bitIndex/8;
	bitIndex %= 8;
	if(code >= rootSize+1)
		goto error;

	if(code == rootSize)
		goto begin;
	else
	{	
		outP[outIndex++] = *strTable[code].p;
		oldCode = code;
	}

	for(;;)
	{
		if(remainInBuf<=4 && !readOK)
		{	
			for(i=0;i<remainInBuf;++i)
				buf[i] = buf[bufIndex+i];

			bufIndex = 0;
			memcpy((char*)&be, m_pBuf+m_nCurPos, 1);
			m_nCurPos += 1;

			if(be != 0)
			{	
				memcpy((char*)(buf+remainInBuf), m_pBuf+m_nCurPos, be);
				m_nCurPos += be;
				remainInBuf += be;
			}
			else
				readOK = TRUE;

			if(m_nCurPos >= m_nBufSize)
				goto error;
		}
		if(remainInBuf<=4)
			if(remainInBuf<=0 || codeSize > (remainInBuf*8-bitIndex))
				break;

		code = *((DWORD*)(buf+bufIndex));
		code <<= 32-codeSize-bitIndex;
		code >>= 32-codeSize;
		bitIndex += codeSize;
		bufIndex += bitIndex/8;
		remainInBuf -= bitIndex/8;
		bitIndex %= 8;

		if(code == rootSize)
		{	
			codeSize = codeSizeBK;
			for(i=rootSize;i<4096;++i)
				if(strTable[i].p != NULL)
				{
					DEL_ONE(strTable[i].p);
					strTable[i].p = NULL;
					strTable[i].len = 0;
				}

				tableIndex = rootSize+2;
				goto begin;
		}
		else if(code == rootSize+1)
			break;
		else
		{	
			unsigned char *p = strTable[code].p;
			int l = strTable[code].len;
			unsigned char c;
			if(p != NULL)
			{	
				c = *p;
				if(outIndex+l <= bufLen)
					for(i=0;i<l;++i)
						outP[outIndex++] = *p++;
				else
					goto error;
				if(!addStrTable(strTable,tableIndex++,oldCode,c))
					goto error;
				oldCode = code;
			}
			else
			{	
				p = strTable[oldCode].p;
				l = strTable[oldCode].len;
				c = *p;
				if(outIndex+l+1 <= bufLen)
				{
					for(i=0;i<l;++i)
						outP[outIndex++] = *p++;
					outP[outIndex++] = c;
				}
				else
					goto error;

				if(!addStrTable(strTable,tableIndex++,oldCode,c))
					goto error;

				oldCode = code;
			}

			if(tableIndex == (((UINT)1)<<codeSize) && codeSize != 12)
				codeSize++;
		}
	}
done:
	for(i=0;i<4096;++i)
		if(strTable[i].p != NULL)
		{	
			DEL_ONE(strTable[i].p);
			strTable[i].p = NULL;
		}
		delete[] strTable;
		return TRUE;
error:
		for(i=0;i<4096;++i)
			if(strTable[i].p != NULL)
			{
				DEL_ONE(strTable[i].p);
				strTable[i].p = NULL;
			}
			delete[] strTable;
			delete[] f->dataBuf;
			f->dataBuf = NULL;
			return FALSE;
}

LPCGLOBAL_INFO CGif89a::getGlobalInfo()
{	
	return &gInfo;
}

void CGif89a::close()
{	
	if(opened)
	{
		opened = FALSE;
		if(inMem && allFrames != NULL)
		{
			FRAME* pf = allFrames;
			for(UINT i=0;i<gInfo.frames;++i)
			{
				if(pf->pColorTable != NULL)
				{	
					delete[] pf->pColorTable;
					pf->pColorTable = NULL;
				}

				if(pf->dataBuf != NULL)
				{
					delete[] pf->dataBuf;
					pf->dataBuf = NULL;
				}
			}
			delete[] allFrames;
			allFrames = NULL;
		}

		if(!inMem)
		{
			if(curFrame.pColorTable != NULL)
			{	
				delete[] curFrame.pColorTable;
				curFrame.pColorTable = NULL;
			}

			if(curFrame.dataBuf != NULL)
			{
				delete[] curFrame.dataBuf;
				curFrame.dataBuf = NULL;
			}
		}
	}

	if (m_pBuf)
	{
		DEL_ARRAY(m_pBuf);
		m_pBuf = NULL;
	}
}