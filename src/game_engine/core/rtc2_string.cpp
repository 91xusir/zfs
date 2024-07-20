#include "core/rt2_core.h"
/*
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
*/

namespace rt2_core {

	RtString::SRegularMem* GetNullRtString()
	{
		static int s_pStrInitData[] = { -1, 0, 0, 0 };
		static RtString::SRegularMem* s_pStrNull = (RtString::SRegularMem*)&s_pStrInitData;
		return s_pStrNull;
	}
	const char* GetNullRtStringData()
	{
		static const char* s_pStrNullData = (const char*)(GetNullRtString() + 1);
		return s_pStrNullData;
	}

	int RT2_CORE_API SafeStrlen(const char* lpsz)
	{
		return (lpsz == NULL) ? 0 : (int)strlen(lpsz);
	}

	int RT2_CORE_API IsChineseChar(const char* lpsz, int iIndex)
	{
		int     iChinese = 0;
		char* pStr = (char*)lpsz;

		if (*pStr)
		{
			for (int i = 0; i <= iIndex && *pStr; i++, pStr++)
			{
				if (*pStr < 0)
				{
					pStr++;
					i++;
					if (*pStr == 0)
					{
						break;
					}
					if (i >= iIndex)
					{
						iChinese = 2 - (i - iIndex);
						break;
					}
				}
			}
		}
		return iChinese;
	}

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	// ��������������Ը��ַ����Ĵ�ȡ����Ϊ0��ʱ����ͷŸ��ַ���
	RtString::~RtString()
	{
		if (GetData() != GetNullRtString())
		{
			if (RtInterlockedDecrement(&GetData()->nRefs) <= 0)
			{
				FreeData(GetData());
			}
		}
	}

	// Ĭ�Ϲ��캯��
	RtString::RtString()
	{
		Init();
	}

	// ��ʼ��
	void RtString::Init()
	{
		m_pchData = (char*)GetNullRtStringData();
	}

	// ֱ�Ӹ���һ���ַ�������
	//      1, Ŀ���ַ����ǿյģ�������һ�����ַ���
	//      2, Ŀ���ַ��������ö�εģ���������һ��
	//      3, Ŀ���ַ����������ģ��͸���һ���ַ���
	RtString::RtString(const RtString& stringSrc)
	{
		if (stringSrc.GetData()->nRefs >= 0)
		{
			if (stringSrc.GetData() == GetNullRtString())
			{
				Init();
			}
			else
			{
				m_pchData = stringSrc.m_pchData;
				RtInterlockedIncrement(&GetData()->nRefs);
			}
		}
		else
		{
			Init();
			*this = stringSrc.m_pchData;
		}
	}

	// ����һ���ظ� nRepeat�� ch�ַ����ַ���
	RtString::RtString(char ch, int nRepeat)
	{
		Init();
		if (nRepeat >= 1)
		{
			AllocBuffer(nRepeat);
			memset(m_pchData, ch, nRepeat);
		}
	}

	// ����һ������Ϊlpsz���ַ���
	RtString::RtString(char* lpsz)
	{
		Init();
		int nLen = SafeStrlen(lpsz);
		if (nLen != 0)
		{
			AllocBuffer(nLen);
			memcpy(m_pchData, lpsz, nLen * sizeof(char));
		}
	}

	// ����һ������Ϊlpsz���ַ���
	RtString::RtString(const char* lpsz)
	{
		Init();
		int nLen = SafeStrlen(lpsz);
		if (nLen != 0)
		{
			AllocBuffer(nLen);
			memcpy(m_pchData, lpsz, nLen * sizeof(char));
		}
	}

	// ����һ������Ϊpsz���ַ���
	RtString::RtString(const unsigned char* psz)
	{
		Init();
		*this = (char*)psz;
	}

	// ����һ������Ϊlpch�ĳ���ΪnLength���ַ���
	//   1, nLengthС��lpch�ĳ���, ȡlpch��ǰ��nLength���ַ�
	//   2, nLength����lpch�ĳ���, ȡȫ����lpch
	RtString::RtString(char* lpch, int nLength)
	{
		Init();
		if (nLength != 0)
		{
			int nSize = SafeStrlen(lpch);
			if (nLength > nSize)
			{
				nLength = nSize;
			}
			AllocBuffer(nLength);
			memcpy(m_pchData, lpch, nLength * sizeof(char));
		}
	}

	// ȡ���ַ����ĳ���
	int RtString::GetLength() const
	{
		return GetData()->nDataLength;
	}

	// ����Ƿ�Ϊ���ַ���
	bool RtString::IsEmpty() const
	{
		return GetData()->nDataLength == 0;
	}

	// ����ַ���
	void RtString::Empty()
	{
		if (GetData()->nDataLength == 0)
		{
			return;
		}
		if (GetData()->nRefs >= 0)
		{
			Release();
		}
		else
		{
			*this = '\0';
		}
	}

	// ȡ��ָ��λ�õ��ַ���
	char RtString::GetAt(int nIndex) const
	{
		return m_pchData[nIndex];
	}

	// ȡ��ָ��λ�õ��ַ���, ͬGetAt
	char RtString::operator[](int nIndex) const
	{
		return m_pchData[nIndex];
	}

	// ����ָ��λ�õ��ַ�
	void RtString::SetAt(int nIndex, char ch)
	{
		CopyBeforeWrite();
		m_pchData[nIndex] = ch;
	}

	// ��ֵ����
	const RtString& RtString::operator=(const RtString& stringSrc)
	{

		if (m_pchData != stringSrc.m_pchData)   // ��������Լ���ֵ�Լ�
		{
			if ((GetData()->nRefs < 0 && GetData() != GetNullRtString()) || stringSrc.GetData()->nRefs < 0)
			{
				// actual copy necessary since one of the strings is locked
				AssignCopy(stringSrc.GetData()->nDataLength, stringSrc.m_pchData);
			}
			else
			{
				// ֱ���ͷű�����ڴ棬����ָ��Է����ڴ棬����ͬһ���ڴ�
				Release();
				m_pchData = stringSrc.m_pchData;
				RtInterlockedIncrement(&GetData()->nRefs);
			}
		}
		return *this;
	}

	// ����һ������Ϊ1����Ϊch���ַ���
	const RtString& RtString::operator=(char ch)
	{
		AssignCopy(1, &ch);
		return *this;
	}

	// ����һ������Ϊlpsz���ַ���
	const RtString& RtString::operator=(char* lpsz)
	{
		AssignCopy(SafeStrlen(lpsz), lpsz);
		return *this;
	}

	// ����һ������Ϊlpsz���ַ���
	const RtString& RtString::operator=(const char* lpsz)
	{
		AssignCopy(SafeStrlen(lpsz), lpsz);
		return *this;

	}

	// ����һ������Ϊpsz���ַ���
	const RtString& RtString::operator=(const unsigned char* psz)
	{
		*this = (char*)psz;
		return *this;
	}

	// ��str׷�ӵ��ַ�������
	const RtString& RtString::operator+=(const RtString& str)
	{
		ConcatInPlace(str.GetData()->nDataLength, str.m_pchData);
		return *this;
	}

	// �� lpsz ׷�ӵ��ַ�������
	const RtString& RtString::operator+=(const char* lpsz)
	{
		ConcatInPlace(SafeStrlen(lpsz), lpsz);
		return *this;
	}

	// �� ch ׷�ӵ��ַ�������
	const RtString& RtString::operator+=(char ch)
	{
		ConcatInPlace(1, &ch);
		return *this;
	}

	const RtString& RtString::operator*=(const char* pStr)
	{
		int iLen = GetLength();
		if (iLen > 1 && (*this)[iLen - 1] != PATH_SEPARATOR[0] && (*this)[iLen - 1] != PATH_SEPARATOR[1])
			*this += PATH_SEPARATOR[0];
		return *this += pStr;
	}

	const RtString& RtString::operator*=(const RtString& str)
	{
		int iLen = GetLength();
		if (iLen > 1 && (*this)[iLen - 2] != PATH_SEPARATOR[0])
			*this += PATH_SEPARATOR;
		return *this += str;
	}

	RtString RtString::operator*(const char* pStr) const
	{
		return RtString(*this) *= pStr;
	}

	// �Ƚ� this �� lpsz �����ؽ��
	//      ���ִ�Сд
	int RtString::Compare(const char* lpsz) const
	{
		return strcmp(m_pchData, lpsz);
	}

	// �Ƚ� this �� lpsz �����ؽ��
	//      �����ִ�Сд
	int RtString::CompareNoCase(const char* lpsz) const
	{
		return stricmp(m_pchData, lpsz);
	}

	// ȡ�ô�nFirst��ʼ��nCount���ַ������ַ���
	RtString RtString::Mid(int nFirst, int nCount) const
	{
		// ������Χ
		if (nFirst < 0)
			nFirst = 0;
		if (nCount < 0)
			nCount = 0;

		if (nFirst + nCount > GetData()->nDataLength)
			nCount = GetData()->nDataLength - nFirst;
		if (nFirst > GetData()->nDataLength)
			nCount = 0;

		// �����Χ�����������ַ�����ֱ�ӷ����Լ�
		if (nFirst == 0 && nFirst + nCount == GetData()->nDataLength)
			return *this;

		RtString dest;
		AllocCopy(dest, nCount, nFirst, 0);
		return dest;
	}

	// ȡ�ô� nFirst ��ʼ�����������ַ���
	RtString RtString::Mid(int nFirst) const
	{
		return Mid(nFirst, GetData()->nDataLength - nFirst);
	}

	// ȡ���ַ������ nCount ���ַ������ַ���
	RtString RtString::Left(int nCount) const
	{
		if (nCount < 0)
			nCount = 0;
		if (nCount >= GetData()->nDataLength)
			return *this;

		RtString dest;
		AllocCopy(dest, nCount, 0, 0);
		return dest;
	}

	// ȡ���ַ����ұ� nCount ���ַ������ַ���
	RtString RtString::Right(int nCount) const
	{
		if (nCount < 0)
			nCount = 0;
		if (nCount >= GetData()->nDataLength)
			return *this;

		RtString dest;
		AllocCopy(dest, nCount, GetData()->nDataLength - nCount, 0);
		return dest;
	}

	// ȡ�ð�����lpszChatSet���ַ������ַ���
	RtString RtString::SpanIncluding(const char* lpszCharSet) const
	{
		return Left((int)strspn(m_pchData, lpszCharSet));
	}

	// ȡ�ò�����lpszChatSet���κ��ַ������ַ���
	RtString RtString::SpanExcluding(const char* lpszCharSet) const
	{
		return Left((int)strcspn(m_pchData, lpszCharSet));
	}

	// ת��Ϊ��д
	void RtString::MakeUpper()
	{
		CopyBeforeWrite();
		strupr(m_pchData);
	}

	// ת��ΪСд
	void RtString::MakeLower()
	{
		CopyBeforeWrite();
		strlwr(m_pchData);
	}

	// ��ת�ַ���
	void RtString::MakeReverse()
	{
		CopyBeforeWrite();
		strrev(m_pchData);
	}

	// ȥ���ұ�ָ�����ַ� chTarget
	void RtString::TrimRight(char chTarget)
	{
		CopyBeforeWrite();
		char* lpsz = m_pchData;
		char* lpszLast = NULL;

		while (*lpsz != '\0')
		{
			if (*lpsz == chTarget)
			{
				if (lpszLast == NULL)
				{
					lpszLast = lpsz;
				}
			}
			else
			{
				lpszLast = NULL;
			}
			lpsz++;
		}

		if (lpszLast != NULL)
		{
			*lpszLast = '\0';
			GetData()->nDataLength = lpszLast - m_pchData;
		}
	}

	// ȥ���ұ�ָ�����ַ���
	void RtString::TrimRight(const char* lpszTargets)
	{
		CopyBeforeWrite();
		char* lpsz = m_pchData;
		char* lpszLast = NULL;

		while (*lpsz != '\0')
		{
			if (strchr(lpszTargets, *lpsz) != NULL)
			{
				if (lpszLast == NULL)
				{
					lpszLast = lpsz;
				}
			}
			else
			{
				lpszLast = NULL;
			}
			lpsz++;
		}

		if (lpszLast != NULL)
		{
			*lpszLast = '\0';
			GetData()->nDataLength = lpszLast - m_pchData;
		}
	}

	// ȥ�����ָ�����ַ� chTarget
	void RtString::TrimLeft(char chTarget)
	{
		CopyBeforeWrite();
		const char* lpsz = m_pchData;

		while (chTarget == *lpsz)
		{
			lpsz++;
		}

		if (lpsz != m_pchData)
		{
			int nDataLength = GetData()->nDataLength - (lpsz - m_pchData);
			memmove(m_pchData, lpsz, (nDataLength + 1) * sizeof(char));
			GetData()->nDataLength = nDataLength;
		}
	}

	// ȥ�����ָ�����ַ���
	void RtString::TrimLeft(const char* lpszTargets)
	{
		if (SafeStrlen(lpszTargets) == 0)
			return;

		CopyBeforeWrite();
		const char* lpsz = m_pchData;

		while (*lpsz != '\0')
		{
			if (strchr(lpszTargets, *lpsz) == NULL)
			{
				break;
			}
			lpsz++;
		}

		if (lpsz != m_pchData)
		{
			int nDataLength = GetData()->nDataLength - (lpsz - m_pchData);
			memmove(m_pchData, lpsz, (nDataLength + 1) * sizeof(char));
			GetData()->nDataLength = nDataLength;
		}
	}

	// �滻�ַ����ڵ� chOld Ϊ chNew�� �����滻�ĸ���
	int RtString::Replace(char chOld, char chNew)
	{
		int nCount = 0;

		if (chOld != chNew)
		{
			CopyBeforeWrite();
			char* psz = m_pchData;
			char* pszEnd = psz + GetData()->nDataLength;
			while (psz < pszEnd)
			{
				if (*psz == chOld)
				{
					*psz = chNew;
					nCount++;
				}
				psz++;
			}
		}
		return nCount;
	}

	// �滻�ַ����ڵ��ַ��� lpszOld Ϊ lpszNew �����滻�ĸ���
	int RtString::Replace(const char* lpszOld, const char* lpszNew)
	{
		int nSourceLen = SafeStrlen(lpszOld);
		if (nSourceLen == 0)
			return 0;
		int nReplacementLen = SafeStrlen(lpszNew);

		// ����Ƿ���Ҫ�滻
		int nCount = 0;
		char* lpszStart = m_pchData;
		char* lpszEnd = m_pchData + GetData()->nDataLength;
		char* lpszTarget;
		while (lpszStart < lpszEnd)
		{
			while ((lpszTarget = strstr(lpszStart, lpszOld)) != NULL)
			{
				nCount++;
				lpszStart = lpszTarget + nSourceLen;
			}
			lpszStart += strlen(lpszStart) + 1;
		}

		if (nCount > 0)
		{
			CopyBeforeWrite();

			// ���ԭ����buf̫�̣�����Ҫ���·���һ��buf
			int nOldLength = GetData()->nDataLength;
			int nNewLength = nOldLength + (nReplacementLen - nSourceLen) * nCount;
			if (GetData()->nAllocLength < nNewLength || GetData()->nRefs > 1)
			{
				SRegularMem* pOldData = GetData();
				char* pstr = m_pchData;
				AllocBuffer(nNewLength);
				memcpy(m_pchData, pstr, pOldData->nDataLength * sizeof(char));
				RtString::Release(pOldData);
			}
			lpszStart = m_pchData;
			lpszEnd = m_pchData + GetData()->nDataLength;

			// ִ���滻
			while (lpszStart < lpszEnd)
			{
				while ((lpszTarget = strstr(lpszStart, lpszOld)) != NULL)
				{
					int nBalance = nOldLength - (lpszTarget - m_pchData + nSourceLen);
					memmove(lpszTarget + nReplacementLen, lpszTarget + nSourceLen,
						nBalance * sizeof(char));
					memcpy(lpszTarget, lpszNew, nReplacementLen * sizeof(char));
					lpszStart = lpszTarget + nReplacementLen;
					lpszStart[nBalance] = '\0';
					nOldLength += (nReplacementLen - nSourceLen);
				}
				lpszStart += strlen(lpszStart) + 1;
			}
			GetData()->nDataLength = nNewLength;
		}

		return nCount;
	}

	// ɾ���ַ��������е��ַ� chRemove
	int RtString::Remove(char chRemove)
	{
		CopyBeforeWrite();

		char* pstrSource = m_pchData;
		char* pstrDest = m_pchData;
		char* pstrEnd = m_pchData + GetData()->nDataLength;

		while (pstrSource < pstrEnd)
		{
			if (*pstrSource != chRemove)
			{
				*pstrDest = *pstrSource;
				pstrDest++;
			}
			pstrSource++;
		}
		*pstrDest = '\0';
		int nCount = pstrSource - pstrDest;
		GetData()->nDataLength -= nCount;

		return nCount;
	}

	// ���ַ� ch ���뵽ָ��λ�� nIndex
	int RtString::Insert(int nIndex, char ch)
	{
		CopyBeforeWrite();

		if (nIndex < 0)
			nIndex = 0;

		int nNewLength = GetData()->nDataLength;
		if (nIndex > nNewLength)
			nIndex = nNewLength;
		nNewLength++;

		if (GetData()->nAllocLength < nNewLength)
		{
			SRegularMem* pOldData = GetData();
			char* pstr = m_pchData;
			AllocBuffer(nNewLength);
			memcpy(m_pchData, pstr, (pOldData->nDataLength + 1) * sizeof(char));
			RtString::Release(pOldData);
		}

		// move existing bytes down
		memmove(m_pchData + nIndex + 1, m_pchData + nIndex, (nNewLength - nIndex) * sizeof(char));
		m_pchData[nIndex] = ch;
		GetData()->nDataLength = nNewLength;

		return nNewLength;
	}

	// ���ַ��� pstr ���뵽ָ��λ�� nIndex
	int RtString::Insert(int nIndex, const char* pstr)
	{
		if (nIndex < 0)
			nIndex = 0;

		int nInsertLength = SafeStrlen(pstr);
		int nNewLength = GetData()->nDataLength;
		if (nInsertLength > 0)
		{
			CopyBeforeWrite();
			if (nIndex > nNewLength)
				nIndex = nNewLength;
			nNewLength += nInsertLength;

			if (GetData()->nAllocLength < nNewLength)
			{
				SRegularMem* pOldData = GetData();
				char* pstr = m_pchData;
				AllocBuffer(nNewLength);
				memcpy(m_pchData, pstr, (pOldData->nDataLength + 1) * sizeof(char));
				RtString::Release(pOldData);
			}

			// move existing bytes down
			memmove(m_pchData + nIndex + nInsertLength,
				m_pchData + nIndex,
				(nNewLength - nIndex - nInsertLength + 1) * sizeof(char));
			memcpy(m_pchData + nIndex,
				pstr, nInsertLength * sizeof(char));
			GetData()->nDataLength = nNewLength;
		}

		return nNewLength;
	}

	// ɾ���� nIndex ��ʼ�� nCount ���ַ�
	int RtString::Delete(int nIndex, int nCount)
	{
		if (nIndex < 0)
			nIndex = 0;
		int nNewLength = GetData()->nDataLength;
		if (nCount > 0 && nIndex < nNewLength)
		{
			CopyBeforeWrite();
			int nBytesToCopy = nNewLength - (nIndex + nCount) + 1;

			memcpy(m_pchData + nIndex,
				m_pchData + nIndex + nCount, nBytesToCopy * sizeof(char));
			GetData()->nDataLength = nNewLength - nCount;
		}

		return nNewLength;
	}

	// ��������ַ�ch�������ҵ���λ�ã�û���ҵ�����-1
	int RtString::ReverseFind(char ch) const
	{
		// find last single character
		char* lpsz = strrchr(m_pchData, (unsigned char)ch);

		// return -1 if not found, distance from beginning otherwise
		return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
	}

	// �����ַ�ch�������ҵ���λ�ã�û���ҵ�����-1
	int RtString::Find(char ch) const
	{
		return Find(ch, 0);
	}

	// ��nStart��ʼ�����ַ�ch�������ҵ���λ�ã�û���ҵ�����-1
	int RtString::Find(char ch, int nStart) const
	{
		int nLength = GetData()->nDataLength;
		if (nStart >= nLength)
			return -1;

		// find first single character
		char* lpsz = strchr(m_pchData + nStart, (unsigned char)ch);

		// return -1 if not found and index otherwise
		return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
	}

	// ���ַ����в����ַ���lpszCharSet���е��ַ�
	//      �ҵ�һ�����������أ������ҵ���λ�ã�û���ҵ�����-1
	int RtString::FindOneOf(const char* lpszCharSet) const
	{
		char* lpsz = strpbrk(m_pchData, lpszCharSet);
		return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
	}

	// �����ַ���lpszSub�������ҵ���λ�ã�û���ҵ�����-1
	int RtString::Find(const char* lpszSub) const
	{
		return Find(lpszSub, 0);
	}

	// ��nStart��ʼ�����ַ���lpszSub�������ҵ���λ�ã�û���ҵ�����-1
	int RtString::Find(const char* lpszSub, int nStart) const
	{
		int nLength = GetData()->nDataLength;
		if (nStart > nLength)
			return -1;

		// find first matching substring
		char* lpsz = strstr(m_pchData + nStart, lpszSub);

		// return -1 for not found, distance from beginning otherwise
		return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
	}

	// ��ʽ������
	void RtString::Format(const char* lpszFormat, ...)
	{
		char    pBuf[512];
		va_list argList;
		va_start(argList, lpszFormat);
		vsprintf(pBuf, lpszFormat, argList);
		*this = pBuf;
		va_end(argList);
	}

	// ȡ��Buffer������ָ����С
	char* RtString::GetBuffer(int nMinBufLength)
	{
		if (GetData()->nRefs > 1 || nMinBufLength > GetData()->nAllocLength)
		{
			SRegularMem* pOldData = GetData();
			int nOldLen = GetData()->nDataLength;   // AllocBuffer will tromp it
			if (nMinBufLength < nOldLen)
				nMinBufLength = nOldLen;
			AllocBuffer(nMinBufLength);
			memcpy(m_pchData, pOldData->data(), (nOldLen + 1) * sizeof(char));
			GetData()->nDataLength = nOldLen;
			RtString::Release(pOldData);
		}
		return m_pchData;
	}

	// �ͷ�Buffer
	void RtString::ReleaseBuffer(int nNewLength)
	{
		CopyBeforeWrite();  // just in case GetBuffer was not called

		if (nNewLength == -1)
			nNewLength = (int)strlen(m_pchData); // zero terminated

		if (nNewLength >= GetData()->nAllocLength)
		{
			nNewLength = GetData()->nAllocLength - 1;
		}

		GetData()->nDataLength = nNewLength;
		m_pchData[nNewLength] = '\0';
	}

	// 
	char* RtString::GetBufferSetLength(int nNewLength)
	{
		GetBuffer(nNewLength);
		GetData()->nDataLength = nNewLength;
		m_pchData[nNewLength] = '\0';
		return m_pchData;
	}

	// �ͷ�û��ʹ�õ��ĵĿռ�
	void RtString::FreeExtra()
	{
		if (GetData()->nDataLength != GetData()->nAllocLength)
		{
			SRegularMem* pOldData = GetData();
			AllocBuffer(GetData()->nDataLength);
			memcpy(m_pchData, pOldData->data(), pOldData->nDataLength * sizeof(char));
			RtString::Release(pOldData);
		}
	}

	char* RtString::LockBuffer()
	{
		char* lpsz = GetBuffer(0);
		GetData()->nRefs = -1;
		return lpsz;
	}

	void RtString::UnlockBuffer()
	{
		if (GetData() != GetNullRtString())
			GetData()->nRefs = 1;
	}

	int RtString::GetAllocLength() const
	{
		return GetData()->nAllocLength;
	}

	RtString::SRegularMem* RtString::GetData() const
	{
		return ((SRegularMem*)m_pchData) - 1;
	}

	// ���� nCopyLen+nExtraLen ���ȵĿռ�� dest �ַ�����
	//      �������Լ��Ĵ� nCopyIndex ��ʼ�� nCopyLen ���ȵ��ַ����� dest
	void RtString::AllocCopy(RtString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const
	{
		int nNewLen = nCopyLen + nExtraLen;
		if (nNewLen == 0)
		{
			dest.Init();
		}
		else
		{
			dest.AllocBuffer(nNewLen);
			memcpy(dest.m_pchData, m_pchData + nCopyIndex, nCopyLen * sizeof(char));
		}
	}

	// ������һ��byte���ڱ�������� '\0'
	void RtString::AllocBuffer(int nLen)
	{
		if (nLen == 0)
		{
			Init();
		}
		else
		{
			SRegularMem* pData;
			pData = (SRegularMem*)RtCoreMem().Alloc(sizeof(SRegularMem) + (nLen + 1) * sizeof(char), __FILE__, __LINE__);
			pData->nAllocLength = nLen;
			pData->nRefs = 1;
			pData->data()[nLen] = '\0';
			pData->nDataLength = nLen;
			m_pchData = pData->data();
		}
	}

	// ����һ���ڴ棬����ָ�����ȵ����ݸ��Ƶ�����
	void RtString::AssignCopy(int nSrcLen, const char* lpszSrcData)
	{
		AllocBeforeWrite(nSrcLen);
		memcpy(m_pchData, lpszSrcData, nSrcLen * sizeof(char));
		GetData()->nDataLength = nSrcLen;
		m_pchData[nSrcLen] = '\0';
	}

	// ���������ַ���
	//      �����µ��ַ�����thisָ����ַ���֮��
	//      ������������һ��buf������ֱ�Ӹ���ԭ����buf
	void RtString::ConcatCopy(int nSrc1Len, const char* lpszSrc1Data, int nSrc2Len, const char* lpszSrc2Data)
	{
		SRegularMem* pOldData = GetData();
		int nNewLen = nSrc1Len + nSrc2Len;
		if (nNewLen != 0)
		{
			AllocBuffer(nNewLen);
			memcpy(m_pchData, lpszSrc1Data, nSrc1Len * sizeof(char));
			memcpy(m_pchData + nSrc1Len, lpszSrc2Data, nSrc2Len * sizeof(char));
		}
	}

	// ��������û� += �Ĳ������������ַ���������
	void RtString::ConcatInPlace(int nSrcLen, const char* lpszSrcData)
	{
		if (nSrcLen == 0)
			return;

		// ��������õ��ڴ����ԭ������Ŀռ��Ѿ������ã����ط���ռ�
		if (GetData()->nRefs > 1 || GetData()->nDataLength + nSrcLen > GetData()->nAllocLength)
		{
			// �����Ӻ����������ַ���
			SRegularMem* pOldData = GetData();
			ConcatCopy(GetData()->nDataLength, m_pchData, nSrcLen, lpszSrcData);
			RtString::Release(pOldData);
		}
		else
		{
			// ֱ�Ӱ��������ӵ��ַ�������
			memcpy(m_pchData + GetData()->nDataLength, lpszSrcData, nSrcLen * sizeof(char));
			GetData()->nDataLength += nSrcLen;
			m_pchData[GetData()->nDataLength] = '\0';
		}
	}

	// ׼��һ����д���ַ���, �ַ��������ݲ���
	void RtString::CopyBeforeWrite()
	{
		// ����ַ��������õ�,�Ͱ��ַ�������һ�ݳ���
		if (GetData()->nRefs > 1)
		{
			SRegularMem* pData = GetData();
			Release();
			AllocBuffer(pData->nDataLength);
			memcpy(m_pchData, pData->data(), (pData->nDataLength + 1) * sizeof(char));
		}
	}

	// ����һ��ָ�����ȵ��ַ�������д, �ַ���û������
	void RtString::AllocBeforeWrite(int nLen)
	{
		// ����ַ��������õĻ�����Ҫ������ַ����ȵ�ǰ����Ĵ����Ҫ�·���ռ�
		if (GetData()->nRefs > 1 || nLen > GetData()->nAllocLength)
		{
			Release();
			AllocBuffer(nLen);
		}
	}

	// �ͷ��ڴ�
	void RtString::Release()
	{
		if (GetData() != GetNullRtString())
		{
			if (RtInterlockedDecrement(&GetData()->nRefs) <= 0)
				FreeData(GetData());
			Init();
		}
	}

	// �ͷ�ָ�����ڴ�
	void RtString::Release(SRegularMem* pData)
	{
		if (pData != GetNullRtString())
		{
			if (RtInterlockedDecrement(&pData->nRefs) <= 0)
				FreeData(pData);
		}
	}

	// ֱ��ɾ�����ڴ�
	void RtString::FreeData(SRegularMem* pData)
	{
		char* pDel = (char*)pData;
		RtCoreMem().Free(pDel);
	}

	// convert to integer
	int RtString::ToInt()
	{
		return atoi(m_pchData);
	}

	// convert to float
	float RtString::ToFloat()
	{
		return (float)atof(m_pchData);
	}

	// convert to long
	long RtString::ToLong()
	{
		return atol(m_pchData);
	}

	// convert to integer
	void RtString::ToNumber(int& data)
	{
		data = atoi(m_pchData);
	}

	// convert to float
	void RtString::ToNumber(float& data)
	{
		data = (float)atof(m_pchData);
	}

	// convert to long
	void RtString::ToNumber(long& data)
	{
		data = atol(m_pchData);
	}

	// convert to double
	double RtString::ToDouble()
	{
		return atof(m_pchData);
	}

	// convert to double
	void RtString::ToNumber(double& data)
	{
		data = atof(m_pchData);
	}

#ifndef _DEBUG

	void* RtString::operator new(size_t iSize)
	{
		return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__);
	}
	void RtString::operator delete(void* p)
	{
		return RtMemoryAllocator::Instance().Free(p);
	}
	void* RtString::operator new[](size_t iSize)
	{
		return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__);
	}
	void RtString::operator delete[](void* p)
	{
		return RtMemoryAllocator::Instance().Free(p);
	}
#endif

	RT2_CORE_API bool operator==(const RtString& s1, const RtString& s2)
	{
		return s1.Compare(s2) == 0;
	}
	RT2_CORE_API bool operator==(const RtString& s1, const char* s2)
	{
		return s1.Compare(s2) == 0;
	}
	RT2_CORE_API bool operator==(const char* s1, const RtString& s2)
	{
		return s2.Compare(s1) == 0;
	}
	RT2_CORE_API bool operator!=(const RtString& s1, const RtString& s2)
	{
		return s1.Compare(s2) != 0;
	}
	RT2_CORE_API bool operator!=(const RtString& s1, const char* s2)
	{
		return s1.Compare(s2) != 0;
	}
	RT2_CORE_API bool operator!=(const char* s1, const RtString& s2)
	{
		return s2.Compare(s1) != 0;
	}
	RT2_CORE_API bool operator<(const RtString& s1, const RtString& s2)
	{
		return s1.Compare(s2) < 0;
	}
	RT2_CORE_API bool operator<(const RtString& s1, const char* s2)
	{
		return s1.Compare(s2) < 0;
	}
	RT2_CORE_API bool operator<(const char* s1, const RtString& s2)
	{
		return s2.Compare(s1) > 0;
	}
	RT2_CORE_API bool operator>(const RtString& s1, const RtString& s2)
	{
		return s1.Compare(s2) > 0;
	}
	RT2_CORE_API bool operator>(const RtString& s1, const char* s2)
	{
		return s1.Compare(s2) > 0;
	}
	RT2_CORE_API bool operator>(const char* s1, const RtString& s2)
	{
		return s2.Compare(s1) < 0;
	}
	RT2_CORE_API bool operator<=(const RtString& s1, const RtString& s2)
	{
		return s1.Compare(s2) <= 0;
	}
	RT2_CORE_API bool operator<=(const RtString& s1, const char* s2)
	{
		return s1.Compare(s2) <= 0;
	}
	RT2_CORE_API bool operator<=(const char* s1, const RtString& s2)
	{
		return s2.Compare(s1) >= 0;
	}
	RT2_CORE_API bool operator>=(const RtString& s1, const RtString& s2)
	{
		return s1.Compare(s2) >= 0;
	}
	RT2_CORE_API bool operator>=(const RtString& s1, const char* s2)
	{
		return s1.Compare(s2) >= 0;
	}
	RT2_CORE_API bool operator>=(const char* s1, const RtString& s2)
	{
		return s2.Compare(s1) <= 0;
	}

	// ���� string1 �� string2 �����µ��ַ�������
	RtString operator+(const RtString& string1, const RtString& string2)
	{
		RtString s;
		s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pchData,
			string2.GetData()->nDataLength, string2.m_pchData);
		return s;
	}

	// ���� str �� ch �����µ��ַ�������
	RtString operator+(const RtString& str, char ch)
	{
		RtString s;
		s.ConcatCopy(str.GetData()->nDataLength, str.m_pchData, 1, &ch);
		return s;
	}

	// ���� ch �� str �����µ��ַ�������
	RtString operator+(char ch, const RtString& str)
	{
		RtString s;
		s.ConcatCopy(1, &ch, str.GetData()->nDataLength, str.m_pchData);
		return s;
	}

	// ���� str �� lpsz �����µ��ַ�������
	RtString operator+(const RtString& str, const char* lpsz)
	{
		RtString s;
		s.ConcatCopy(str.GetData()->nDataLength, str.m_pchData, SafeStrlen(lpsz), lpsz);
		return s;
	}

	// ���� lpsz �� str �����µ��ַ�������
	RtString operator+(const char* lpsz, const RtString& str)
	{
		RtString s;
		s.ConcatCopy(SafeStrlen(lpsz), lpsz, str.GetData()->nDataLength,
			str.m_pchData);
		return s;
	}

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
