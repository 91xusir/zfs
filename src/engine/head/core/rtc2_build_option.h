/**
* @file rtc2_build_option.h ��������ѡ��.
*  ����û�Ҫ�޸ı������ã������������޸ģ����Ǳ������±������еĴ������Ӧ�õ�����
* @author janhail
*/

#ifndef DO_CHECK
#   define DO_CHECK 1
#endif

// Ч��ͳ�ƿ���.
#ifndef DO_STAT
#   define DO_STAT 1
#endif

 //����̨����.
 #ifndef DO_CONSOLE
 #   define DO_CONSOLE 1
 #endif

#   define  ENABLE_MEMORY_ALLOCATOR_DEBUG
#   define  ENABLE_LOG

/*
#ifdef _DEBUG
#   define  ENABLE_MEMORY_ALLOCATOR_DEBUG
#   define  ENABLE_LOG
#endif
*/

#define RTE_FILE_VERSION    1
#define RTE_CORE_VERSION    1

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
