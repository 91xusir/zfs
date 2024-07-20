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

	// 析构函数，如果对该字符串的存取个数为0的时候就释放该字符串
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

	// 默认构造函数
	RtString::RtString()
	{
		Init();
	}

	// 初始化
	void RtString::Init()
	{
		m_pchData = (char*)GetNullRtStringData();
	}

	// 直接复制一个字符串对象
	//      1, 目标字符串是空的，就生成一个空字符串
	//      2, 目标字符串是引用多次的，就再引用一次
	//      3, 目标字符串是锁定的，就复制一次字符串
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

	// 生成一个重复 nRepeat次 ch字符的字符串
	RtString::RtString(char ch, int nRepeat)
	{
		Init();
		if (nRepeat >= 1)
		{
			AllocBuffer(nRepeat);
			memset(m_pchData, ch, nRepeat);
		}
	}

	// 生成一个内容为lpsz的字符串
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

	// 生成一个内容为lpsz的字符串
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

	// 生成一个内容为psz的字符串
	RtString::RtString(const unsigned char* psz)
	{
		Init();
		*this = (char*)psz;
	}

	// 生成一个内容为lpch的长度为nLength的字符串
	//   1, nLength小于lpch的长度, 取lpch的前面nLength个字符
	//   2, nLength大于lpch的长度, 取全部的lpch
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

	// 取得字符串的长度
	int RtString::GetLength() const
	{
		return GetData()->nDataLength;
	}

	// 检查是否为空字符串
	bool RtString::IsEmpty() const
	{
		return GetData()->nDataLength == 0;
	}

	// 清空字符串
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

	// 取得指定位置的字符串
	char RtString::GetAt(int nIndex) const
	{
		return m_pchData[nIndex];
	}

	// 取得指定位置的字符串, 同GetAt
	char RtString::operator[](int nIndex) const
	{
		return m_pchData[nIndex];
	}

	// 设置指定位置的字符
	void RtString::SetAt(int nIndex, char ch)
	{
		CopyBeforeWrite();
		m_pchData[nIndex] = ch;
	}

	// 赋值操作
	const RtString& RtString::operator=(const RtString& stringSrc)
	{

		if (m_pchData != stringSrc.m_pchData)   // 如果不是自己赋值自己
		{
			if ((GetData()->nRefs < 0 && GetData() != GetNullRtString()) || stringSrc.GetData()->nRefs < 0)
			{
				// actual copy necessary since one of the strings is locked
				AssignCopy(stringSrc.GetData()->nDataLength, stringSrc.m_pchData);
			}
			else
			{
				// 直接释放本身的内存，并且指向对方的内存，共享同一段内存
				Release();
				m_pchData = stringSrc.m_pchData;
				RtInterlockedIncrement(&GetData()->nRefs);
			}
		}
		return *this;
	}

	// 生成一个长度为1内容为ch的字符串
	const RtString& RtString::operator=(char ch)
	{
		AssignCopy(1, &ch);
		return *this;
	}

	// 生成一个内容为lpsz的字符串
	const RtString& RtString::operator=(char* lpsz)
	{
		AssignCopy(SafeStrlen(lpsz), lpsz);
		return *this;
	}

	// 生成一个内容为lpsz的字符串
	const RtString& RtString::operator=(const char* lpsz)
	{
		AssignCopy(SafeStrlen(lpsz), lpsz);
		return *this;

	}

	// 生成一个内容为psz的字符串
	const RtString& RtString::operator=(const unsigned char* psz)
	{
		*this = (char*)psz;
		return *this;
	}

	// 把str追加到字符串后面
	const RtString& RtString::operator+=(const RtString& str)
	{
		ConcatInPlace(str.GetData()->nDataLength, str.m_pchData);
		return *this;
	}

	// 把 lpsz 追加到字符串后面
	const RtString& RtString::operator+=(const char* lpsz)
	{
		ConcatInPlace(SafeStrlen(lpsz), lpsz);
		return *this;
	}

	// 把 ch 追加到字符串后面
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

	// 比较 this 和 lpsz 并返回结果
	//      区分大小写
	int RtString::Compare(const char* lpsz) const
	{
		return strcmp(m_pchData, lpsz);
	}

	// 比较 this 和 lpsz 并返回结果
	//      不区分大小写
	int RtString::CompareNoCase(const char* lpsz) const
	{
		return stricmp(m_pchData, lpsz);
	}

	// 取得从nFirst开始的nCount个字符的子字符串
	RtString RtString::Mid(int nFirst, int nCount) const
	{
		// 纠正范围
		if (nFirst < 0)
			nFirst = 0;
		if (nCount < 0)
			nCount = 0;

		if (nFirst + nCount > GetData()->nDataLength)
			nCount = GetData()->nDataLength - nFirst;
		if (nFirst > GetData()->nDataLength)
			nCount = 0;

		// 如果范围包含这整个字符串就直接返回自己
		if (nFirst == 0 && nFirst + nCount == GetData()->nDataLength)
			return *this;

		RtString dest;
		AllocCopy(dest, nCount, nFirst, 0);
		return dest;
	}

	// 取得从 nFirst 开始到结束的子字符串
	RtString RtString::Mid(int nFirst) const
	{
		return Mid(nFirst, GetData()->nDataLength - nFirst);
	}

	// 取得字符串左边 nCount 个字符的子字符串
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

	// 取得字符串右边 nCount 个字符的子字符串
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

	// 取得包含在lpszChatSet中字符的子字符串
	RtString RtString::SpanIncluding(const char* lpszCharSet) const
	{
		return Left((int)strspn(m_pchData, lpszCharSet));
	}

	// 取得不包含lpszChatSet中任何字符的子字符串
	RtString RtString::SpanExcluding(const char* lpszCharSet) const
	{
		return Left((int)strcspn(m_pchData, lpszCharSet));
	}

	// 转换为大写
	void RtString::MakeUpper()
	{
		CopyBeforeWrite();
		strupr(m_pchData);
	}

	// 转换为小写
	void RtString::MakeLower()
	{
		CopyBeforeWrite();
		strlwr(m_pchData);
	}

	// 反转字符串
	void RtString::MakeReverse()
	{
		CopyBeforeWrite();
		strrev(m_pchData);
	}

	// 去掉右边指定的字符 chTarget
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

	// 去掉右边指定的字符集
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

	// 去掉左边指定的字符 chTarget
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

	// 去掉左边指定的字符集
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

	// 替换字符串内的 chOld 为 chNew， 返回替换的个数
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

	// 替换字符串内的字符串 lpszOld 为 lpszNew 返回替换的个数
	int RtString::Replace(const char* lpszOld, const char* lpszNew)
	{
		int nSourceLen = SafeStrlen(lpszOld);
		if (nSourceLen == 0)
			return 0;
		int nReplacementLen = SafeStrlen(lpszNew);

		// 检查是否需要替换
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

			// 如果原来的buf太短，就需要重新分配一个buf
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

			// 执行替换
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

	// 删除字符串内所有的字符 chRemove
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

	// 把字符 ch 插入到指定位置 nIndex
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

	// 把字符串 pstr 插入到指定位置 nIndex
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

	// 删除从 nIndex 开始的 nCount 个字符
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

	// 反向查找字符ch，返回找到的位置，没有找到返回-1
	int RtString::ReverseFind(char ch) const
	{
		// find last single character
		char* lpsz = strrchr(m_pchData, (unsigned char)ch);

		// return -1 if not found, distance from beginning otherwise
		return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
	}

	// 查找字符ch，返回找到的位置，没有找到返回-1
	int RtString::Find(char ch) const
	{
		return Find(ch, 0);
	}

	// 从nStart开始查找字符ch，返回找到的位置，没有找到返回-1
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

	// 从字符串中查找字符集lpszCharSet中有的字符
	//      找到一个就立即返回，返回找到的位置，没有找到返回-1
	int RtString::FindOneOf(const char* lpszCharSet) const
	{
		char* lpsz = strpbrk(m_pchData, lpszCharSet);
		return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
	}

	// 查找字符串lpszSub，返回找到的位置，没有找到返回-1
	int RtString::Find(const char* lpszSub) const
	{
		return Find(lpszSub, 0);
	}

	// 从nStart开始查找字符串lpszSub，返回找到的位置，没有找到返回-1
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

	// 格式化输入
	void RtString::Format(const char* lpszFormat, ...)
	{
		char    pBuf[512];
		va_list argList;
		va_start(argList, lpszFormat);
		vsprintf(pBuf, lpszFormat, argList);
		*this = pBuf;
		va_end(argList);
	}

	// 取得Buffer，可以指定大小
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

	// 释放Buffer
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

	// 释放没有使用到的的空间
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

	// 分配 nCopyLen+nExtraLen 长度的空间给 dest 字符串，
	//      并复制自己的从 nCopyIndex 开始的 nCopyLen 长度的字符串给 dest
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

	// 会多分配一个byte用于保存结束符 '\0'
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

	// 分配一段内存，并把指定长度的数据复制到里面
	void RtString::AssignCopy(int nSrcLen, const char* lpszSrcData)
	{
		AllocBeforeWrite(nSrcLen);
		memcpy(m_pchData, lpszSrcData, nSrcLen * sizeof(char));
		GetData()->nDataLength = nSrcLen;
		m_pchData[nSrcLen] = '\0';
	}

	// 连接两个字符串
	//      连接新的字符串到this指针的字符串之后
	//      这个函数会分配一个buf，并且直接覆盖原来的buf
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

	// 这个函数用户 += 的操作，起到连接字符串的作用
	void RtString::ConcatInPlace(int nSrcLen, const char* lpszSrcData)
	{
		if (nSrcLen == 0)
			return;

		// 如果是引用的内存或者原来分配的空间已经不够用，就重分配空间
		if (GetData()->nRefs > 1 || GetData()->nDataLength + nSrcLen > GetData()->nAllocLength)
		{
			// 用连接函数来增长字符串
			SRegularMem* pOldData = GetData();
			ConcatCopy(GetData()->nDataLength, m_pchData, nSrcLen, lpszSrcData);
			RtString::Release(pOldData);
		}
		else
		{
			// 直接把数据连接到字符串后面
			memcpy(m_pchData + GetData()->nDataLength, lpszSrcData, nSrcLen * sizeof(char));
			GetData()->nDataLength += nSrcLen;
			m_pchData[GetData()->nDataLength] = '\0';
		}
	}

	// 准备一个可写的字符串, 字符串的内容不变
	void RtString::CopyBeforeWrite()
	{
		// 如果字符串是引用的,就把字符串复制一份出来
		if (GetData()->nRefs > 1)
		{
			SRegularMem* pData = GetData();
			Release();
			AllocBuffer(pData->nDataLength);
			memcpy(m_pchData, pData->data(), (pData->nDataLength + 1) * sizeof(char));
		}
	}

	// 分配一个指定长度的字符串用于写, 字符串没有内容
	void RtString::AllocBeforeWrite(int nLen)
	{
		// 如果字符串是引用的或者需要分配的字符串比当前分配的大就需要新分配空间
		if (GetData()->nRefs > 1 || nLen > GetData()->nAllocLength)
		{
			Release();
			AllocBuffer(nLen);
		}
	}

	// 释放内存
	void RtString::Release()
	{
		if (GetData() != GetNullRtString())
		{
			if (RtInterlockedDecrement(&GetData()->nRefs) <= 0)
				FreeData(GetData());
			Init();
		}
	}

	// 释放指定的内存
	void RtString::Release(SRegularMem* pData)
	{
		if (pData != GetNullRtString())
		{
			if (RtInterlockedDecrement(&pData->nRefs) <= 0)
				FreeData(pData);
		}
	}

	// 直接删除该内存
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

	// 连接 string1 和 string2 生成新的字符串返回
	RtString operator+(const RtString& string1, const RtString& string2)
	{
		RtString s;
		s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pchData,
			string2.GetData()->nDataLength, string2.m_pchData);
		return s;
	}

	// 连接 str 和 ch 生成新的字符串返回
	RtString operator+(const RtString& str, char ch)
	{
		RtString s;
		s.ConcatCopy(str.GetData()->nDataLength, str.m_pchData, 1, &ch);
		return s;
	}

	// 连接 ch 和 str 生成新的字符串返回
	RtString operator+(char ch, const RtString& str)
	{
		RtString s;
		s.ConcatCopy(1, &ch, str.GetData()->nDataLength, str.m_pchData);
		return s;
	}

	// 连接 str 和 lpsz 生成新的字符串返回
	RtString operator+(const RtString& str, const char* lpsz)
	{
		RtString s;
		s.ConcatCopy(str.GetData()->nDataLength, str.m_pchData, SafeStrlen(lpsz), lpsz);
		return s;
	}

	// 连接 lpsz 和 str 生成新的字符串返回
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
