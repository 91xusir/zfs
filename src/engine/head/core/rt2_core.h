/**
* @file rt2_core.h rt_core 2.0 版
* @author janhail
*/

#ifndef _RT_CORE_VERSION_2_0_H_
#define _RT_CORE_VERSION_2_0_H_

/*! \mainpage RT引擎 2.0版 CoreSDK
 *
 * \section intro_sec 概述
 *
 *     Core是引擎的最底层，提供了和操作系统相关的基本操作，让上层致力于实际的功能。他使用了很多
 *  简单的方法封装了原本复杂的功能，使他们能被简单的使用。同时Core也针对游戏这种软件产品的特性
 *  进行有了特别的优化。
 *
 * \section content_sec 包含了以下几个方面的内容
 *
 *     - 初始化
 *     - 文件管理
 *     - 线程
 *     - 资源锁/信号量/互斥量
 *     - Log输出
 *     - 数学函数
 *     - 内存管理
 *     - 基本对象/RTTI
 *     - 游戏进程管理
 *     - 智能指针
 *     - 字符串
 *     - 系统信息
 *     - 配置文件读取
 *         - INI文件
 *         - CSV文件
 *         - XML文件
 *         - 文本配置程序
 *     - 时间相关
 *     - 算法
 *         - MD5
 *         - CRC
 *         - LZW
 *         - ZLIB
 *         - DES
 *         - ASE
 *
 * \section use_sec 使用步骤
 *    - 在main或者WinMain中通过调用下面函数进行初始化
 *      \code
 *        bool rtCoreInit(const char* szIniFile, RtRuntimeClass* pCoreFactory);
 *      \endcode
 *      注意：内存和RtCore的实例化可能在这个初始化之前完成，因为如果有重载new，就一定
 *            会在main函数之前大量的使用内存管理模块，所以需要使用单键进行自动初始化。<BR><BR>
 *    - 在程序最后退出的时候需要调用rtCoreExit()进行退出操作
 *      \code
 *        bool rtCoreExit();
 *      \endcode
  *      注意：同样的，内存管理和RtCore的析构可能在更后面才会完成
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

	const float fRtTimeFixTime = 4294967296.f; // 0xFFFFFFFF, 前面是秒后面是毫秒

	/** 时间函数，内部的变量是64位的整数.
	* 如果返回float就是以秒为单位，这个时间用于高精度的计时，适合于长时间运行的服务器
	* 服务器强烈推荐使用被时间类。如果功能不够可以添加
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

	/** 创建工厂，这个类用于用户自定义Core的行为.
	*/
	class RtCoreFactory : public RtObject
	{
	public:
		RT_DECLARE_DYNCREATE(RtCoreFactory, RtObject, 0, "RtCore")
			RtCoreFactory() { }
		virtual ~RtCoreFactory() { }
		void SetCore(RtCore* pCore) { m_pCore = pCore; }

		/// 创建文件管理器
		virtual RtFileManager* CreateFileManager() { return NULL; }

		/// 初始化Log，默认的初始化是输出到控制台和文件中，用户可以重载这个函数改变默认初始化
		virtual void InitLogRoot(RtLogType rootLog);

	protected:
		RtCore* m_pCore;
	};

	/*-----------------------------------------------------------------------------
	-     RtCore
	-----------------------------------------------------------------------------*/

	/** 初始化函数.
	*  @param szIniFile 用于设置的INI文件，不能直接传入RtIni的原因是RtIni依赖于Core的文件系统
	*  @param pCoreFactory 创建工厂的RTTI，如果为NULL就默认创建值，用户希望定制系统就要重载这个类
	*  @return 返回true为成功，否则失败
	*/
	bool rtCoreInit(const char* szIniFile, RtRuntimeClass* pCoreFactory = 0);

	/** 退出函数.
	*  @return 返回true为成功，否则失败
	*/
	bool rtCoreExit();

	/** Core的一些核心变量存储的地方.
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
		RtFileManager* pFileManager;   ///< 文件管理，用 RtFileManager& RtCoreFile() 取得
		char* pBuffer2048;    ///< 分配的一个静态内存，大小为2048，做一些缓冲用，用 char* RtCoreBuffer2048() 取得
		RtLogType               rootLog;        ///< 默认的Log输出，用RtLog& RtCoreLog() 取得
		RtCPU                   cpu;            ///< cpu状态和CPU周期函数
		RtString                strName;        ///< 本模块的命名
		RtString                strCmdLine;     ///< 执行本应用所使用的命令行参数
		RtString                strBaseDir;     ///< 工作目录
		RtString                strExePathName; ///< 执行文件所在目录，包括文件名
		RtString                strComputerName;///< 计算机名字
		RtString                strUserName;    ///< 计算机登录用户的名字

	};

	/*-----------------------------------------------------------------------------
	-     开关选项
	-----------------------------------------------------------------------------*/

#define SafeRelease(p)     { if (p) (p)->Release(); (p) = NULL; }
#define SafeDeleteArray(p) { if (p) delete[] (p); (p) = NULL;   }
#define SafeDelete(p)      { if (p) delete (p); (p) = NULL;     }
#define SafeFree(p)        { if (p) free(p); (p) = NULL;        }


	/** 内存分配器
	*/
	inline RtMemoryAllocator& RtCoreMem()
	{
		return RtMemoryAllocator::Instance();
	}

	/** 小内存分配器
	*/
	inline RtNanoMemAllocator& RtCoreNanoMem()
	{
		return RtNanoMemAllocator::Instance();
	}

	/** 取得默认的缓冲区
	*       这个缓冲区是公用的，需要的时候不用分配一个这么大的内存
	*/
	inline char* RtCoreBuffer2048()
	{
		return RtCore::Instance().pBuffer2048;
	}

	/** 文件管理器
	*/
	inline RtFileManager& RtCoreFile()
	{
		return *(RtCore::Instance().pFileManager);
	}

	/** 默认Log，所有的引擎都使用这个Log输出，可以定义这个Log为空
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
//for 二维数组
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
	//for 二维数组
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
//for 二维数组
#	define CM_MEMDEF2(Name, iSize1, iSize2)	char Name[iSize1][iSize2 + PADING_SIZE];
#	define CM_MEMPROTECTOR2(p, iSize1, iSize2) MemProtector2((char*)p, iSize1, iSize2, __FILE__, __LINE__);
#	define CM_MEMUNPROTECTOR2(p, iSize1, iSize2)		MemUnProtector2((char*)p, iSize1, iSize2, __FILE__, __LINE__);

#else
#    define RT_NEW           new	

//for Globle Mem
#	define G_MEMDEF(Name, iSize)	char Name[iSize];
#	define G_MEMPROTECTOR(p, iSize)
//for 二维数组
#	define G_MEMDEF2(Name, iSize1, iSize2)	char Name[iSize1][iSize2];
#	define G_MEMPROTECTOR2(p, iSize1, iSize2)

//for static Mem
#	define S_MEMDEF(Name, iSize)	static char Name[iSize];
#	define S_MEMPROTECTOR(p, iSize, bMemProtected)
//for 二维数组
#	define S_MEMDEF2(Name, iSize1, iSize2)	static char Name[iSize1][iSize2];
#	define S_MEMPROTECTOR2(p, iSize1, iSize2, bMemProtected)

//for class member Mem
#	define CM_MEMDEF(Name, iSize)	char Name[iSize];
#	define CM_MEMPROTECTOR(p, iSize)
#	define CM_MEMUNPROTECTOR(p)
//for 二维数组
#	define CM_MEMDEF2(Name, iSize1, iSize2)	char Name[iSize1][iSize2];
#	define CM_MEMPROTECTOR2(p, iSize1, iSize2)
#	define CM_MEMUNPROTECTOR2(p, iSize1, iSize2)

#endif


//单个对象安全释放
#ifndef DEL_ONE
#   define DEL_ONE(x)        do { if (x) { delete (x); (x)=nullptr; } } while(0)
#endif
//对象数组的分配和释放安全释放
#ifndef DEL_ARRAY
#   define DEL_ARRAY(x)      do { if (x) { delete[] (x); (x)=nullptr; } } while(0)
#endif

/// 记录行记录的使用.
// 在需要分配内存记录文件名的地方所有的include后面加上下面语句就可以了
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
