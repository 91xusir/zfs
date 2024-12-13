/**
* @file rt2_core.h rt_core 2.0 ��
* @author janhail
*/

#ifndef _RT_CORE_VERSION_2_0_H_
#define _RT_CORE_VERSION_2_0_H_

/*! \mainpage RT���� 2.0�� CoreSDK
 *
 * \section intro_sec ����
 *
 *     Core���������ײ㣬�ṩ�˺Ͳ���ϵͳ��صĻ������������ϲ�������ʵ�ʵĹ��ܡ���ʹ���˺ܶ�
 *  �򵥵ķ�����װ��ԭ�����ӵĹ��ܣ�ʹ�����ܱ��򵥵�ʹ�á�ͬʱCoreҲ�����Ϸ���������Ʒ������
 *  ���������ر���Ż���
 *
 * \section content_sec ���������¼������������
 *
 *     - ��ʼ��
 *     - �ļ�����
 *     - �߳�
 *     - ��Դ��/�ź���/������
 *     - Log���
 *     - ��ѧ����
 *     - �ڴ����
 *     - ��������/RTTI
 *     - ��Ϸ���̹���
 *     - ����ָ��
 *     - �ַ���
 *     - ϵͳ��Ϣ
 *     - �����ļ���ȡ
 *         - INI�ļ�
 *         - CSV�ļ�
 *         - XML�ļ�
 *         - �ı����ó���
 *     - ʱ�����
 *     - �㷨
 *         - MD5
 *         - CRC
 *         - LZW
 *         - ZLIB
 *         - DES
 *         - ASE
 *
 * \section use_sec ʹ�ò���
 *    - ��main����WinMain��ͨ���������溯�����г�ʼ��
 *      \code
 *        bool rtCoreInit(const char* szIniFile, RtRuntimeClass* pCoreFactory);
 *      \endcode
 *      ע�⣺�ڴ��RtCore��ʵ���������������ʼ��֮ǰ��ɣ���Ϊ���������new����һ��
 *            ����main����֮ǰ������ʹ���ڴ����ģ�飬������Ҫʹ�õ��������Զ���ʼ����<BR><BR>
 *    - �ڳ�������˳���ʱ����Ҫ����rtCoreExit()�����˳�����
 *      \code
 *        bool rtCoreExit();
 *      \endcode
  *      ע�⣺ͬ���ģ��ڴ�����RtCore�����������ڸ�����Ż����
*/
#include <list>
#include <map>
#include <set>
#include <list>
#include <queue>
#include <algorithm>
#include <vector>
#include <stack>
#include <string>
#include <iostream>

#if (defined(_MSC_VER) || defined(__ICL))
#include <unordered_set>
#include <unordered_map>
#elif defined(__GNUC__)
#include <ext/unordered_set>
#include <ext/unordered_map>
#endif

#include "rtc2_build_option.h"
#include "rtc2_platform.h"




namespace rt2_core {

	/*-----------------------------------------------------------------------------
	-     RtTime
	-----------------------------------------------------------------------------*/

	const float fRtTimeFixTime = 4294967296.f; // 0xFFFFFFFF, ǰ����������Ǻ���

	/** ʱ�亯�����ڲ��ı�����64λ������.
	* �������float��������Ϊ��λ�����ʱ�����ڸ߾��ȵļ�ʱ���ʺ��ڳ�ʱ�����еķ�����
	* ������ǿ���Ƽ�ʹ�ñ�ʱ���ࡣ������ܲ����������
	*/
	class RtTime
	{
	public:

		RtTime() { v = 0; }
		RtTime(RTTIME64 i) { v = i; }
		RtTime(float f) { v = (RTTIME64)(f * fRtTimeFixTime); }
		RtTime(double d) { v = (RTTIME64)(d * fRtTimeFixTime); }
		RtTime(DWORD L, DWORD H) { v = L; RTTIME64 th = H; v |= (th << 32); }
		float   GetFloat() { return v / fRtTimeFixTime; }
		long    GetLong() { return reinterpret_cast<long&>(v); }
		RtTime  operator+  (float f) const { return RtTime(v + (RTTIME64)(f * fRtTimeFixTime)); }
		float   operator-  (RtTime t) const { return (v - t.v) / fRtTimeFixTime; }
		RtTime  operator*  (float f) const { return RtTime(v * f); }
		RtTime  operator/  (float f) const { return RtTime(v / f); }
		RtTime& operator+= (float f) { v = v + (RTTIME64)(f * fRtTimeFixTime); return *this; }
		RtTime& operator*= (float f) { v = (RTTIME64)(v * f); return *this; }
		RtTime& operator/= (float f) { v = (RTTIME64)(v / f); return *this; }
		int     operator== (RtTime t) { return v == t.v; }
		int     operator!= (RtTime t) { return v != t.v; }
		int     operator>  (RtTime t) { return v > t.v; }
		RtTime& operator=  (const RtTime& t) { v = t.v; return *this; }

		RtTime(bool bNow);
		void    UpdateNow();
	private:
		RTTIME64 v;
	};

	inline char* RtCoreBuffer2048();

	struct RtRuntimeClass;

	class RtCore;
	class RtObject;
	class RtArchive;

} // namespace rt2_core

#include "rtc2_smartptr.h"
#include "rtc2_string.h"
#include "rtc2_process.h"
#include "rtc2_math.h"
#include "rtc2_debug.h"
#include "rtc2_lock.h"
#include "rtc2_memory.h"
#include "rtc2_thread_event.h"
#include "rtc2_thread.h"
#include "rtc2_file.h"
#include "rtc2_log.h"
#include "rtc2_object.h"
#include "rtc2_text_ini.h"
#include "rtc2_text_table.h"
#include "rtc2_text_util.h"
#include "rtc2_text_xml.h"
#include "rtc2_util.h"
#include "rtc2_time.h"
#include "rtc2_system_info.h"
#include "rtc2_engine_sys.h"
#include "rtc2_encrypt.h"
#include "rt2_farray.h"
#include "rtc2_safestr.h"

#ifdef _DATABASE
#include "rtc2_database.h"
#endif

namespace rt2_core {

	/*-----------------------------------------------------------------------------
	-     RtCoreFactory
	-----------------------------------------------------------------------------*/

	/** ��������������������û��Զ���Core����Ϊ.
	*/
	class RtCoreFactory : public RtObject
	{
	public:
		RT_DECLARE_DYNCREATE(RtCoreFactory, RtObject, 0, "RtCore")
			RtCoreFactory() { }
		virtual ~RtCoreFactory() { }
		void SetCore(RtCore* pCore) { m_pCore = pCore; }

		/// �����ļ�������
		virtual RtFileManager* CreateFileManager() { return NULL; }

		/// ��ʼ��Log��Ĭ�ϵĳ�ʼ�������������̨���ļ��У��û�����������������ı�Ĭ�ϳ�ʼ��
		virtual void InitLogRoot(RtLogType rootLog);

	protected:
		RtCore* m_pCore;
	};

	/*-----------------------------------------------------------------------------
	-     RtCore
	-----------------------------------------------------------------------------*/

	/** ��ʼ������.
	*  @param szIniFile �������õ�INI�ļ�������ֱ�Ӵ���RtIni��ԭ����RtIni������Core���ļ�ϵͳ
	*  @param pCoreFactory ����������RTTI�����ΪNULL��Ĭ�ϴ���ֵ���û�ϣ������ϵͳ��Ҫ���������
	*  @return ����trueΪ�ɹ�������ʧ��
	*/
	bool rtCoreInit(const char* szIniFile, RtRuntimeClass* pCoreFactory = 0);

	/** �˳�����.
	*  @return ����trueΪ�ɹ�������ʧ��
	*/
	bool rtCoreExit();

	/** Core��һЩ���ı����洢�ĵط�.
	*/
	class RtCore
	{
	private:
		RtCore();
		void Create(RtCoreFactory* pFactory);
		static void CreateInstance();
		static RtCore* s_pInstance;
		static RtResLock    s_lockSingleton;
		static bool         s_bSingletonDestroyed;

	public:
		virtual bool Init(const char* szIniFile);
		virtual void Tick(int delta);
		virtual void Exit();
		virtual bool DoCommand(const char* cmd, const char* param);
		virtual const char* GetName();

	public:
		friend bool rtCoreInit(const char* szIniFile, RtRuntimeClass* pCoreFactory);
		static RtCore& Instance()
		{
			if (!s_pInstance)
			{
				CreateInstance();
			}
			return *s_pInstance;
		}
		~RtCore();

	public:
		RtLogType CreateLog(const char* szName);
		void SetDefaultFileLog() { m_bIsDefaultFileLog = true; }

	private:
		void PlatformInitInfo();

	private:
		bool                    m_bIsDefaultFileLog;

	public:
		RtFileManager* pFileManager;   ///< �ļ������� RtFileManager& RtCoreFile() ȡ��
		char* pBuffer2048;    ///< �����һ����̬�ڴ棬��СΪ2048����һЩ�����ã��� char* RtCoreBuffer2048() ȡ��
		RtLogType               rootLog;        ///< Ĭ�ϵ�Log�������RtLog& RtCoreLog() ȡ��
		RtCPU                   cpu;            ///< cpu״̬��CPU���ں���
		RtString                strName;        ///< ��ģ�������
		RtString                strCmdLine;     ///< ִ�б�Ӧ����ʹ�õ������в���
		RtString                strBaseDir;     ///< ����Ŀ¼
		RtString                strExePathName; ///< ִ���ļ�����Ŀ¼�������ļ���
		RtString                strComputerName;///< ���������
		RtString                strUserName;    ///< �������¼�û�������

	};

	/*-----------------------------------------------------------------------------
	-     ����ѡ��
	-----------------------------------------------------------------------------*/

#define SafeRelease(p)     { if (p) (p)->Release(); (p) = NULL; }
#define SafeDeleteArray(p) { if (p) delete[] (p); (p) = NULL;   }
#define SafeDelete(p)      { if (p) delete (p); (p) = NULL;     }
#define SafeFree(p)        { if (p) free(p); (p) = NULL;        }


	/** �ڴ������
	*/
	inline RtMemoryAllocator& RtCoreMem()
	{
		return RtMemoryAllocator::Instance();
	}

	/** С�ڴ������
	*/
	inline RtNanoMemAllocator& RtCoreNanoMem()
	{
		return RtNanoMemAllocator::Instance();
	}

	/** ȡ��Ĭ�ϵĻ�����
	*       ����������ǹ��õģ���Ҫ��ʱ���÷���һ����ô����ڴ�
	*/
	inline char* RtCoreBuffer2048()
	{
		return RtCore::Instance().pBuffer2048;
	}

	/** �ļ�������
	*/
	inline RtFileManager& RtCoreFile()
	{
		return *(RtCore::Instance().pFileManager);
	}

	/** Ĭ��Log�����е����涼ʹ�����Log��������Զ������LogΪ��
	*/
	inline RtLog& RtCoreLog()
	{
		return *(RtCore::Instance().rootLog);
	}

} // namespace rt2_core

////////////////////////////////////////////////////////////////////////

using namespace rt2_core;
#ifdef ENABLE_OPERATOR_NEW
inline void* operator new(size_t iSize) { return RtCoreMem().Alloc(iSize); }
inline void* operator new(size_t iSize, const char* szFileName, int nLine) { return RtCoreMem().Alloc(iSize, szFileName, nLine); }
inline void* operator new[](size_t iSize) { return RtCoreMem().Alloc(iSize); }
inline void* operator new[](size_t iSize, const char* szFileName, int nLine) { return RtCoreMem().Alloc(iSize, szFileName, nLine); }

inline void operator delete(void* pPointer) { return RtCoreMem().Free(pPointer); }
inline void operator delete[](void* pPointer) { return RtCoreMem().Free(pPointer); }
//inline void operator delete(void* pPointer, const char* szFileName, int nLine)  { return RtCoreMem().Free(pPointer); }
//inline void operator delete[](void* pPointer, const char* szFileName, int nLine){ return RtCoreMem().Free(pPointer); }

inline bool MemProtector(void* p, size_t iSize, const char* szFileName, int nLine)
{
	return RtStaticMemProtector::Instance().AddMemBlock(p, iSize, szFileName, nLine);
}

inline bool MemProtector2(char* p, size_t iSize1, size_t iSize2, const char* szFileName, int nLine)
{
	char* pTemp = p;
	for (size_t i = 0; i < iSize1; i++)
	{
		RtStaticMemProtector::Instance().AddMemBlock((void*)pTemp, iSize2, szFileName, nLine);
		pTemp += (iSize2 + PADING_SIZE);
	}
	return true;
}

inline bool MemUnProtector(void* p)
{
	return RtStaticMemProtector::Instance().RemoveMemBlock(p);
}

inline bool MemUnProtector2(char* p, size_t iSize1, size_t iSize2, const char* szFileName, int nLine)
{
	char* pTemp = p;
	for (size_t i = 0; i < iSize1; i++)
	{
		RtStaticMemProtector::Instance().RemoveMemBlock((void*)pTemp);
		pTemp += (iSize2 + PADING_SIZE);
	}

	return true;
}
#   define RT_NEW new(__FILE__, __LINE__)

//for Globle Mem
#	define G_MEMDEF(Name, iSize)	char Name[iSize + PADING_SIZE];
#	define G_MEMPROTECTOR(p, iSize) MemProtector(p, iSize, __FILE__, __LINE__);
//for ��ά����
#	define G_MEMDEF2(Name, iSize1, iSize2)	char Name[iSize1][iSize2 + PADING_SIZE];
#	define G_MEMPROTECTOR2(p, iSize1, iSize2) MemProtector2((char*)p, iSize1, iSize2, __FILE__, __LINE__);

//for static Mem
#	define S_MEMDEF(Name, iSize)	static char Name[iSize + PADING_SIZE];
#	define S_MEMPROTECTOR(p, iSize, bMemProtected) \
	static bool bMemProtected = false;\
	if (!bMemProtected)\
	{\
		MemProtector(p, iSize, __FILE__, __LINE__);\
		bMemProtected = true;\
	}
	//for ��ά����
#	define S_MEMDEF2(Name, iSize1, iSize2)	static char Name[iSize1][iSize2 + PADING_SIZE];
#	define S_MEMPROTECTOR2(p, iSize1, iSize2, bMemProtected) \
	static bool bMemProtected = false;\
	if (!bMemProtected)\
	{\
		MemProtector2((char*)p, iSize1, iSize2, __FILE__, __LINE__);\
		bMemProtected = true;\
	}

	//for class member Mem
#	define CM_MEMDEF(Name, iSize)	char Name[iSize + PADING_SIZE];
#	define CM_MEMPROTECTOR(p, iSize) MemProtector(p, iSize, __FILE__, __LINE__);
#	define CM_MEMUNPROTECTOR(p)		MemUnProtector(p);
//for ��ά����
#	define CM_MEMDEF2(Name, iSize1, iSize2)	char Name[iSize1][iSize2 + PADING_SIZE];
#	define CM_MEMPROTECTOR2(p, iSize1, iSize2) MemProtector2((char*)p, iSize1, iSize2, __FILE__, __LINE__);
#	define CM_MEMUNPROTECTOR2(p, iSize1, iSize2)		MemUnProtector2((char*)p, iSize1, iSize2, __FILE__, __LINE__);

#else
#    define RT_NEW           new	

//for Globle Mem
#	define G_MEMDEF(Name, iSize)	char Name[iSize];
#	define G_MEMPROTECTOR(p, iSize)
//for ��ά����
#	define G_MEMDEF2(Name, iSize1, iSize2)	char Name[iSize1][iSize2];
#	define G_MEMPROTECTOR2(p, iSize1, iSize2)

//for static Mem
#	define S_MEMDEF(Name, iSize)	static char Name[iSize];
#	define S_MEMPROTECTOR(p, iSize, bMemProtected)
//for ��ά����
#	define S_MEMDEF2(Name, iSize1, iSize2)	static char Name[iSize1][iSize2];
#	define S_MEMPROTECTOR2(p, iSize1, iSize2, bMemProtected)

//for class member Mem
#	define CM_MEMDEF(Name, iSize)	char Name[iSize];
#	define CM_MEMPROTECTOR(p, iSize)
#	define CM_MEMUNPROTECTOR(p)
//for ��ά����
#	define CM_MEMDEF2(Name, iSize1, iSize2)	char Name[iSize1][iSize2];
#	define CM_MEMPROTECTOR2(p, iSize1, iSize2)
#	define CM_MEMUNPROTECTOR2(p, iSize1, iSize2)

#endif


//��������ȫ�ͷ�
#ifndef DEL_ONE
#   define DEL_ONE(x)        do { if (x) { delete (x); (x)=nullptr; } } while(0)
#endif
//��������ķ�����ͷŰ�ȫ�ͷ�
#ifndef DEL_ARRAY
#   define DEL_ARRAY(x)      do { if (x) { delete[] (x); (x)=nullptr; } } while(0)
#endif

/// ��¼�м�¼��ʹ��.
// ����Ҫ�����ڴ��¼�ļ����ĵط����е�include��������������Ϳ�����
// #define new RT_NEW



#ifdef WIN32
//  windows lib
#   pragma comment(lib, "winmm.lib")
#   pragma comment(lib, "vfw32.lib")

// #ifndef NOEXPORT_LIB
// #   ifndef _DEBUG
// #       ifdef _MT
// #           ifdef _DLL
// #               pragma comment(lib, "rtcore_lib_MD.lib")
// #           else
// #               pragma comment(lib, "rtcore_lib_MT.lib")
// #           endif
// #       else
// #           pragma comment(lib, "rtcore_lib_ML.lib")
// #       endif
// #   else
// #       ifdef _MT
// #           ifdef _DLL
// #               pragma comment(lib, "rtcore_lib_MDD.lib")
// #           else
// #               pragma comment(lib, "rtcore_lib_MTD.lib")
// #           endif
// #       else
// #           pragma comment(lib, "rtcore_lib_MLD.lib")
// #       endif
// #   endif
// #endif

#else

#endif // WIN32

#endif // _RT_CORE_VERSION_2_0_H_

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
